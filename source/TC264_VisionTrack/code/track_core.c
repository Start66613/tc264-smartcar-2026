/* GPL-3.0: TC264 vehicle-specific vision/control integration.
 * Architectural references: HammerTa fixed-period independent wheel control;
 * SJTU AuTop circle.c boundary switching and cross.c element separation.
 * v105 中线提取参照国赛二维视觉做法：逐行求左右边线、中线 =(左+右)/2
 *   （CQU findline.c 的 get_left/get_right、caipingjiang my_image.c 的
 *    boder_L/boder_R、HammerTa IMG_DEAL.c 的左右边线），
 *   本工程按本车单位与图像尺寸重写，不照搬任何外来系数。
 * No foreign camera calibration tables or PWM values are copied.
 */
#include "track_core.h"
#include <string.h>
#include <stdlib.h>
static int clamp(int x,int lo,int hi){return x<lo?lo:(x>hi?hi:x);}
void vt_tracker_reset(VTTracker *t)
{
 memset(t,0,sizeof(*t));t->previous_mid=VT_W/2;t->previous_left=44;t->previous_right=144;
}
/* v105：逐行白段缓冲。vt_analyze 只在主循环里调用（非中断、非可重入）。 */
static int16_t vt_sl[VT_H][VT_SEG_K];   /* 各白段左端 */
static int16_t vt_sr[VT_H][VT_SEG_K];   /* 各白段右端 */
static uint8_t vt_sn[VT_H];             /* 该行记下的白段数 */
static uint8_t vt_rowc[VT_H];           /* VT_ROW_* 分类；VT_ROW_OK=单段候选/已确认测量 */
static int16_t vt_rowv[VT_H];           /* 已确认测量的行→中线；未确认→-1 */
static int16_t vt_roww[VT_H];           /* 已确认测量的行→宽度；未确认→0 */
static int16_t vt_clr[VT_H];            /* 左贴边白段可见的**右端**（=-1 表示本行没有） */
static int16_t vt_cll[VT_H];            /* 右贴边白段可见的**左端**（=-1 表示本行没有） */
static int16_t vt_wfit[VT_H];           /* 本帧每行可用的"预期路宽"（见下方填充说明） */

/* v105：在行 y 上尝试把边线续下去。优先"左右边界都连续"的完整白段；
 * 都不满足时退到**单边贴边补线**（对齐国赛：单边丢线用另一边 + 学习路宽补线）。
 * 命中返回 1 并写好 vt_rowc/vt_rowv/vt_roww，同时把跟踪锚点 L/R 前移。
 *   *out_clip —— 本行中线用到了"学习路宽"（有一侧边界在画面外），计入 clip_rows
 *   *out_fake —— 本行的 l/r 是**推断**出来的，调用方必须跳过宽度学习与角点/直道统计
 *                （那两处只能用真实双边界；但 open_left/open_right 仍用真实可见边，
 *                  所以"可见边那条"路径的 out_fake=0，环岛判据不受影响）。 */
static int vt_take_row(VTTracker *t,int y,int *L,int *R,int *out_clip,int *out_fake)
{
 int k,pick=-1,pd=0x7fffffff;
 int own=t->width[y];      /* 本行自己学到的宽度：只用来做"宽度合理性"检查 */
 int exp=vt_wfit[y];       /* 本行可用的预期宽度：贴边时用来补出不可见的那一侧 */
 *out_clip=0;*out_fake=0;
 for(k=0;k<(int)vt_sn[y];k++){
  int l=vt_sl[y][k],r=vt_sr[y][k],w=r-l+1,dl=abs(l-*L),dr=abs(r-*R);
  if(dl>VT_EDGE_WIN||dr>VT_EDGE_WIN)continue;                           /* 双边界都要连续 */
  if(own>=16&&(w*100<own*VT_WID_LO||w*100>own*VT_WID_HI))continue;      /* 宽度要合理 */
  if(dl+dr<pd){pd=dl+dr;pick=k;}
 }
 if(pick>=0){
  int l=vt_sl[y][pick],r=vt_sr[y][pick],c=(l+r)/2;
  *L=l;*R=r;
  /* 白段有一侧顶到画面边缘 → 那一侧的边界看不见，(l+r)/2 会被"裁掉的宽度"带偏。
   * 用学习路宽把不可见的一侧补出来再取中点。l/r 本身照旧交给统计用。 */
  if(exp>=16){
   if(l<=2&&r<VT_W-3){c=r-exp/2;*out_clip=1;}
   else if(r>=VT_W-3&&l>2){c=l+exp/2;*out_clip=1;}
  }
  if(c<2)c=2;
  if(c>VT_W-3)c=VT_W-3;
  vt_rowc[y]=VT_ROW_OK;vt_rowv[y]=(int16_t)c;vt_roww[y]=(int16_t)(r-l+1);
  return 1;
 }
 /* A bounded white region can lose one tape edge without clipping the
  * image. Only accept expansion, one continuous endpoint and learned width;
  * never reconnect across a multi-branch row or learn inferred width. */
 if(exp>=16&&vt_sn[y]==1u){
  int l=vt_sl[y][0],r=vt_sr[y][0],c=-1,side=0;
  if(abs(l-*L)<=VT_EDGE_WIN&&r>*R+VT_EDGE_WIN){c=l+exp/2;side=VT_ROW_CLIPR;}
  else if(abs(r-*R)<=VT_EDGE_WIN&&l<*L-VT_EDGE_WIN){c=r-exp/2;side=VT_ROW_CLIPL;}
  if(c>=l+3&&c<=r-3){
   *L=side==VT_ROW_CLIPR?l:r-exp;*R=*L+exp;
   vt_rowc[y]=(uint8_t)side;vt_rowv[y]=(int16_t)c;vt_roww[y]=(int16_t)exp;
   *out_clip=1;*out_fake=1;return 1;
  }
 }
 /* 贴边白段因为超宽（>VT_SEG_MAX）进不了正常白段池：本行一条可用的段都没有。
  * 只要**可见的那一侧**仍然连续，就用学习路宽把另一边补出来接着跟。 */
 if(exp>=16){
  if(vt_cll[y]>=0){                       /* 右侧被画面裁掉，可见的是左端 */
   int l=vt_cll[y];
   if(abs(l-*L)<=VT_EDGE_WIN){
    *L=l;*R=l+exp;
    vt_rowc[y]=VT_ROW_CLIPR;vt_rowv[y]=(int16_t)(l+exp/2);vt_roww[y]=(int16_t)exp;
    *out_clip=1;*out_fake=1;
    return 1;
   }
  }else if(vt_clr[y]>=0){                 /* 左侧被画面裁掉，可见的是右端 */
   int r=vt_clr[y];
   if(abs(r-*R)<=VT_EDGE_WIN){
    *L=r-exp;*R=r;
    vt_rowc[y]=VT_ROW_CLIPL;vt_rowv[y]=(int16_t)(r-exp/2);vt_roww[y]=(int16_t)exp;
    *out_clip=1;*out_fake=1;
    return 1;
   }
  }
 }
 return 0;
}

static int vt_trust(int y){return vt_rowc[y]>=VT_ROW_OK&&vt_rowc[y]<=VT_ROW_LINE&&vt_rowv[y]>=0;}

/* A distant white patch is not an exit unless its bridge stays on white road.
 * Permit two dark samples for tape/noise, never a continuous blue island. */
static int vt_white_bridge(const uint8_t *bin,int y0,int x0,int y1,int x1)
{
 int y,run=0;
 if(y1<=y0)return 0;
 for(y=y0;y<=y1;y++){
  int x=x0+(x1-x0)*(y-y0)/(y1-y0);
  if(x<0||x>=VT_W)return 0;
  if(!bin[y*VT_W+x]){if(++run>=3)return 0;}else run=0;
 }
 return 1;
}

/* v106：一个白段能不能当"整帧种子"。种子的中心会决定整条中线（其余行都从它插值），
 * 所以它必须是**可信的路面横截面**：
 *   ① 学习路宽已知 → 用与 vt_take_row 完全相同的宽度合理性闸门。路口里唯一幸存的
 *      16px 贴边小白斑 vs 学到的 ~90px 路宽 → 16% < VT_WID_LO(40%) → 淘汰；
 *      急弯的贴边白段（38/62 ≈ 61%）能过。
 *   ② 学习路宽未知（刚复位 / 整帧过曝）→ 只接受**两端都可见**的白段：贴边的白段一侧
 *      被画面裁掉、又没有路宽可补，中点纯属瞎猜，绝不能拿它定整条中线。 */
static int vt_seed_ok(int y,int l,int r)
{
 int own=vt_wfit[y],w=r-l+1;
 if(own>=16)return w*100>=own*VT_WID_LO&&w*100<=own*VT_WID_HI;
 return l>2&&r<VT_W-3;
}
/* v121: consecutive dual-edge chain length above (y,l,r), up to 40 rows.
 * The bottom seed and the mid-corridor candidate must be compared by their
 * REAL chain length: with the old 8-row cap a 20px bottom fragment looked
 * exactly as good as a 60-row clean corridor (J1 hijack).  strict=1 also
 * rejects edge-clipped rows (for mid-image candidates). */
static int vt_chain_len(int y,int l,int r,int strict)
{
 int yy,j,n=0,pl=l,pr=r;
 for(yy=y-1;yy>=VT_ROW_TOP&&yy>=y-40;yy--){
  int found=0;
  for(j=0;j<(int)vt_sn[yy];j++){
   int ll=vt_sl[yy][j],rr=vt_sr[yy][j];
   if(strict&&(ll<=2||rr>=VT_W-3))continue;
   if(abs(ll-pl)<=VT_EDGE_WIN&&abs(rr-pr)<=VT_EDGE_WIN&&vt_seed_ok(yy,ll,rr)){pl=ll;pr=rr;n++;found=1;break;}
  }
  if(!found)break;
 }
 return n;
}


void vt_analyze(VTTracker *t,const uint8_t *bin,uint32_t contrast,uint32_t now,uint32_t sequence,VTFrame *v)
{
 int y,k,x;
 int previous=t->previous_mid;
 int sum=0,wsum=0,fsum=0,fn=0,nl=0,nr=0,nw=0,nn=0,last_l=-1,last_r=-1;
 int max_w=0,ok_rows=0,measured=0,modeled=0,clip_rows=0,hw=22,hwsum=0,hwn=0,full_rows=0;
 unsigned valid=0,near=0,op_l=0,op_r=0,op_n=0,corners_l=0,corners_r=0,straight_l=0,straight_r=0;
 unsigned finish_rows=0,fork_rows=0,hole_rows=0;
 int seed_y=-1,seed_l=0,seed_r=0,seed_sup=0,seed_clip=0;
 memset(v,0,sizeof(*v));v->stamp=now;v->sequence=sequence;v->mid=v->far_mid=-1;
 for(y=0;y<VT_H;y++){t->left[y]=t->right[y]=t->mid[y]=-1;vt_rowc[y]=VT_ROW_NONE;vt_rowv[y]=-1;vt_roww[y]=0;vt_sn[y]=0;vt_clr[y]=-1;vt_cll[y]=-1;}
 if(contrast<18)return;
 /* ---------- 第 1 遍：逐行提取白段（岔口/蓝缝行会有 2 条以上），并按形状分类 ---------- */
 for(y=VT_ROW_BOT;y>=VT_ROW_TOP;y--){
  int x=0,seg=0,full=0,widest=0,internal_gap=0,prior_end=-1;
  while(x<VT_W){
   int l,r,w;
   if(!bin[y*VT_W+x]){x++;continue;}
   l=x;while(x<VT_W&&bin[y*VT_W+x])x++;r=x-1;w=x-l;
   if(w<4)continue;                              /* 太窄：噪声，不计段 */
   if(w>widest)widest=w;
   if(prior_end>=0&&l-prior_end-1>internal_gap)internal_gap=l-prior_end-1;
   prior_end=r;
   /* v105：贴边白段单独记一份。这类段因为被画面裁掉，宽度会超过 VT_SEG_MAX，
    * 进不了正常白段池，但它的**可见那一侧边界**是真实可靠的，用来做单边补线。
    * 越宽 = 越深入画面外侧，所以左贴边取最大 r、右贴边取最小 l。 */
   if(w>=VT_SEG_MIN){
    if(l<=2&&r<VT_W-3){if(r>vt_clr[y])vt_clr[y]=(int16_t)r;}
    else if(r>=VT_W-3&&l>2){if(vt_cll[y]<0||l<vt_cll[y])vt_cll[y]=(int16_t)l;}
   }
   if(w>=VT_FULL_W){full=1;continue;}            /* 整行铺满：本行没有左右边界 */
   if(w<VT_SEG_MIN||w>VT_SEG_MAX)continue;       /* 过窄忽略；过宽视为整片铺满 */
   if(seg<(int)VT_SEG_K){vt_sl[y][seg]=(int16_t)l;vt_sr[y][seg]=(int16_t)r;}
   seg++;
  }
  vt_sn[y]=(uint8_t)(seg<(int)VT_SEG_K?seg:(int)VT_SEG_K);
  if(widest>max_w)max_w=widest;
  if(full||widest>=VT_FULL_W){vt_rowc[y]=VT_ROW_FULL;full_rows++;}   /* v106：数 FULL 行 → 路口判据 */
  else if(seg==0)              vt_rowc[y]=VT_ROW_NONE;
  else if(seg>=2)              vt_rowc[y]=VT_ROW_MULTI;
  else                         vt_rowc[y]=VT_ROW_OK;  /* 单段候选，等第 2 遍确认 */
  /* 岔口/蓝缝：同一行出现两条及以上独立白段（环内蓝洞、岔口楔形蓝缝） */
  if(seg>=2&&y<85){fork_rows++;if(internal_gap>=25)hole_rows++;}
 }
 /* ---------- 逐行"预期路宽"表（给单边贴边补线用） ----------
  * 优先用本行学到的宽度；本行还没学到（典型情形：它一直贴在画面边缘、从来没完整
  * 出现过，所以宽度学习永远跳过它）就借用最近一行学到的——路宽随 y 是平滑变化的，
  * 相邻行借过来足够准。两遍扫描：先自下往上、再自上往下，把空缺填满。 */
 for(y=VT_ROW_TOP;y<=VT_ROW_BOT;y++)vt_wfit[y]=t->width[y];
 {
  int last=-1;
  for(y=VT_ROW_BOT;y>=VT_ROW_TOP;y--){if(vt_wfit[y]>=16)last=vt_wfit[y];else if(last>=0)vt_wfit[y]=(int16_t)last;}
  last=-1;
  for(y=VT_ROW_TOP;y<=VT_ROW_BOT;y++){if(vt_wfit[y]>=16)last=vt_wfit[y];else if(last>=0)vt_wfit[y]=(int16_t)last;}
 }
 /* ---------- 第 2 遍：从画面最下方（车头正前方，必有路）取种子，
  *            向上逐行要求"左端与右端同时落在上一行窗口内"。
  * 种子行取法：自下往上第一条有白段的行；该行内只在"与最宽段同量级(>=3/4)"的段里
  * 挑离上一帧中心最近的——既排除小的反光/噪声斑，又用上时间连续性。 ---------- */
 {
 int seed_score=0x7fffffff;
 for(y=VT_ROW_BOT;y>=(int)(VT_ROW_BOT-VT_SEED_ROWS)&&y>=VT_ROW_TOP;y--){
  int best=-1,bestd=0x7fffffff,best_sup=0,wmax=0;
  if(vt_sn[y]==0)continue;
  for(k=0;k<(int)vt_sn[y];k++){int w=vt_sr[y][k]-vt_sl[y][k]+1;if(w>wmax)wmax=w;}
  for(k=0;k<(int)vt_sn[y];k++){
   int l=vt_sl[y][k],r=vt_sr[y][k],c=(l+r)/2,w=r-l+1,d,supk=0;
   if(w*4<wmax*3)continue;                    /* 只考虑与该行最宽段同量级的 */
   if(!vt_seed_ok(y,l,r))continue;            /* v106：不信"来路不明的白斑"当整帧起点 */
   {
    int support=vt_chain_len(y,l,r,0);   /* v121: full chain length (40-row cap) */
    if(support<4)continue;
    supk=support;
    d=abs(c-previous)+(VT_ROW_BOT-y)*2-support*8;
   }
   if(d<bestd){bestd=d;best=k;best_sup=supk;}
  }
  if(best>=0&&bestd<seed_score){seed_score=bestd;seed_y=y;seed_sup=best_sup;seed_l=vt_sl[y][best];seed_r=vt_sr[y][best];}
 }
 }
 /* v121: mid-corridor candidate. Compare the bottom chain and the best
  * bounded corridor in the near/mid image by real chain length; a thin
  * bottom fragment (J1: ~9 rows) loses to a long clean corridor (60+ rows)
  * instead of hijacking the centre line. */
 {
  int b_y=-1,b_l=0,b_r=0,b_sup=0;
  for(y=VT_ROW_BOT-(int)VT_SEED_ROWS-1;y>=55;y--){
   int chosen=-1,score=0x7fffffff,ca2=0;
   for(k=0;k<(int)vt_sn[y];k++){
    int l=vt_sl[y][k],r=vt_sr[y][k],sup;
    if(l<=2||r>=VT_W-3||!vt_seed_ok(y,l,r))continue;
    sup=vt_chain_len(y,l,r,1);
    if(sup>=8&&abs((l+r)/2-previous)<score){score=abs((l+r)/2-previous);chosen=k;ca2=sup;}
   }
   if(chosen>=0){b_y=y;b_l=vt_sl[y][chosen];b_r=vt_sr[y][chosen];b_sup=ca2;break;}
  }
  if(b_y>=0){
   if(seed_y<0){seed_y=b_y;seed_l=b_l;seed_r=b_r;seed_sup=b_sup;}
   else{
    int ca=(seed_l+seed_r)/2,cb=(b_l+b_r)/2;
    if(b_sup>=24&&b_sup>=2*seed_sup&&
       ((long)b_sup*10-abs(cb-previous))>((long)seed_sup*10-abs(ca-previous)+5)){
     seed_y=b_y;seed_l=b_l;seed_r=b_r;seed_sup=b_sup;
    }
   }
  }
 }
 /* 种子兜底（v105）：最下方这十几行整段都在画面外时（急弯打到底），上面那条
  * 找不到任何"完整白段"。这时用**贴边白段的可见边 + 学习路宽**当种子，
  * 否则整帧一个测量都建立不起来 → 中线又被冻住。只在这里用一次，且要求
  * 推出来的中心离上一帧不至于太远（时间连续性）。 */
 if(seed_y<0){
  for(y=VT_ROW_BOT;y>=(int)(VT_ROW_BOT-VT_SEED_ROWS)&&y>=VT_ROW_TOP;y--){
   int w=vt_wfit[y],c;
   if(w<16)continue;
   if(vt_cll[y]>=0){c=vt_cll[y]+w/2;if(c<2||c>VT_W-3)continue;
    if(abs(c-previous)<=VT_EDGE_WIN*3){seed_y=y;seed_l=vt_cll[y];seed_r=vt_cll[y]+w;seed_clip=1;break;}}
   else if(vt_clr[y]>=0){c=vt_clr[y]-w/2;if(c<2||c>VT_W-3)continue;
    if(abs(c-previous)<=VT_EDGE_WIN*3){seed_y=y;seed_l=vt_clr[y]-w;seed_r=vt_clr[y];seed_clip=1;break;}}
  }
 }
 if(seed_y>=0){
  int L=seed_l,R=seed_r,clip=0,miss=0;
  {
   int c=(seed_l+seed_r)/2;
   if(!seed_clip){
    /* 种子行本身也可能顶到画面边缘（急弯时最常见的正是最下面那几行），
     * 一样要用学习路宽把不可见的一侧补出来，否则中线的"起点"就是偏的。 */
    int e2=vt_wfit[seed_y];
    if(e2>=16){
     if(seed_l<=2&&seed_r<VT_W-3){c=seed_r-e2/2;clip=1;}
     else if(seed_r>=VT_W-3&&seed_l>2){c=seed_l+e2/2;clip=1;}
    }
   }
   if(c<2)c=2;
   if(c>VT_W-3)c=VT_W-3;
   vt_rowc[seed_y]=seed_clip?(seed_r>=VT_W-3?VT_ROW_CLIPR:VT_ROW_CLIPL):VT_ROW_OK;
   vt_rowv[seed_y]=(int16_t)c;vt_roww[seed_y]=(int16_t)(seed_r-seed_l+1);
  }
  ok_rows++;
  if(seed_clip)clip_rows++;
  else{hwsum+=vt_roww[seed_y]/2;hwn++;last_l=seed_l;last_r=seed_r;if(clip)clip_rows++;}
  /* 从种子向**上**（远处）延伸 */
  for(y=seed_y-1;y>=VT_ROW_TOP;y--){
   int l,r,fake;
   if(!vt_take_row(t,y,&L,&R,&clip,&fake)){if(++miss>=3)break;continue;}
   miss=0;
   ok_rows++;
   l=L;r=R;
   if(clip)clip_rows++;
   if(fake)continue;                               /* l/r 是推断的：不进统计 */
   hwsum+=vt_roww[y]/2;hwn++;
   /* 行宽只在"普通单段 + 左右边界都在画面内"的行上学习（岔口行不学） */
   if(t->edge_mode==0&&vt_sn[y]==1&&l>2&&r<VT_W-3&&vt_roww[y]>=16&&vt_roww[y]<VT_SEG_MAX){
    int exp=t->width[y];
    t->width[y]=(int16_t)(exp?((int)exp*7+vt_roww[y])/8:vt_roww[y]);
   }
   if(y>=45&&y<=100){
    op_n++;op_l+=(l<=2);op_r+=(r>=VT_W-3);
    if(last_l>=0){int dl2=abs(l-last_l),dr2=abs(r-last_r);corners_l+=dl2>=7;corners_r+=dr2>=7;straight_l+=dl2<=2;straight_r+=dr2<=2;}
    last_l=l;last_r=r;
   }
  }
  /* 从种子向**下**（车头正前方，最容易跑出画面的一段）延伸。
   * 单独一条锚链，不要接着上面循环结束的 L/R——种子才是共同起点。 */
  {
   int L2=seed_l,R2=seed_r,miss2=0;
   for(y=seed_y+1;y<=VT_ROW_BOT;y++){
    int l,r,fake;
    if(!vt_take_row(t,y,&L2,&R2,&clip,&fake)){if(++miss2>=3)break;continue;}
    miss2=0;
    ok_rows++;
    l=L2;r=R2;
    if(clip)clip_rows++;
    if(fake)continue;
    hwsum+=vt_roww[y]/2;hwn++;
    if(t->edge_mode==0&&vt_sn[y]==1&&l>2&&r<VT_W-3&&vt_roww[y]>=16&&vt_roww[y]<VT_SEG_MAX){
     int exp=t->width[y];
     t->width[y]=(int16_t)(exp?((int)exp*7+vt_roww[y])/8:vt_roww[y]);
    }
    if(y>=45&&y<=100){
     op_n++;op_l+=(l<=2);op_r+=(r>=VT_W-3);
    }
   }
  }
 }
/* 没被第 2 遍确认的"单段候选"降级，避免第 3 遍读到 -1 的中心 */
for(y=VT_ROW_TOP;y<=VT_ROW_BOT;y++)
 if(vt_rowc[y]==VT_ROW_OK&&vt_rowv[y]<0) vt_rowc[y]=VT_ROW_NONE;
/* ---------- 第 2b 遍（v107）：远端锚点 + 拉线补全 ----------
 * 十字路口的实拍结构（tmp/frame_gray_v102_crossing.bin 的 ASCII 二值图）：
 *     y=28..46   一条干净横截面 x=66..120（中心 93）  ← **远处前方的真实路面**
 *     y=47..110  整块白（横向道路铺满）              ← 底部 14 行全被盖住，底部种子无从取
 * v106 此时只能"顶住上一帧中心"画一条竖线，**远处那条真中线根本没被用上**。
 *
 * 对齐国赛做法——NEFU `element.c:connect_line(x1,y1,x2,y2,way)`「拉一条补线」、
 * HammerTa `lian_xian_zhi`「两点反解斜率直线外推」（A_轮次2_NEFU 1744–1800 / A008 2232）：
 * 找到**远端锚点**后，用「远端锚点」与「近端参考（上一帧中心）」两点连一条直线，
 * 把中间看不到的横向白带铺出来。
 *
 * 只在"整行铺满的横向白带确实存在"（full_rows 够多）时才启用——这样急弯/岔口
 * 不会把"另一条分支"误当成远处路面。 */
if(ok_rows<(int)VT_OK_MIN&&full_rows>=20){
 int fy=-1,fl=0,fr=0;
 for(y=VT_ROW_TOP;y<=VT_ROW_BOT;y++){          /* 从上（远）往下找第一条干净横截面 */
  int l,r;
  if(vt_sn[y]!=1)continue;
  l=vt_sl[y][0];r=vt_sr[y][0];
  if(!vt_seed_ok(y,l,r))continue;
  if(!vt_white_bridge(bin,y,(l+r)/2,VT_ROW_BOT,t->previous_mid))continue;
  fy=y;fl=l;fr=r;break;
 }
 if(fy>=0){
  int fc=(fl+fr)/2,L=fl,R=fr,clip,fake,ny,nc;
  /* ① 远端锚点本身是**真实测量**：从它向下（朝车头）延伸，双边界连续性照旧。
   *    注意 y 增大 = 更近，所以这里往 y+ 走。 */
  vt_rowc[fy]=VT_ROW_OK;vt_rowv[fy]=(int16_t)fc;vt_roww[fy]=(int16_t)(fr-fl+1);ok_rows++;
  for(y=fy+1;y<=VT_ROW_BOT;y++){
   if(vt_trust(y)){L=vt_rowv[y]-vt_roww[y]/2;R=vt_rowv[y]+vt_roww[y]/2;continue;}
   if(!vt_take_row(t,y,&L,&R,&clip,&fake))continue;
   ok_rows++;if(clip)clip_rows++;if(fake)continue;
   if(t->edge_mode==0&&vt_sn[y]==1&&L>2&&R<VT_W-3&&vt_roww[y]>=16&&vt_roww[y]<VT_SEG_MAX){
    int exp=t->width[y];
    t->width[y]=(int16_t)(exp?((int)exp*7+vt_roww[y])/8:vt_roww[y]);
   }
  }
  /* ② 拉线：远端锚点 → 近端参考之间，凡是仍然没有可信测量的行，沿两点直线铺点。
   *    这就是国赛的"拉线补全"——中间那条横向白带没有任何边界，只能用几何外推。 */
  ny=VT_ROW_BOT;nc=(t->previous_mid>0)?(int)t->previous_mid:fc;
  for(y=fy+1;y<=ny;y++){
   if(vt_trust(y))continue;
   vt_rowv[y]=(int16_t)(nc+(fc-nc)*(ny-y)/(ny-fy));
   vt_roww[y]=vt_wfit[y]>=16?vt_wfit[y]:(int16_t)60;
   vt_rowc[y]=VT_ROW_LINE;ok_rows++;
  }
 }
}
/* v108: reconnect across a boundary-free band, preferring straight ahead.
 * Do not extrapolate the turning edge of the incoming horizontal branch. */
if(full_rows>=8&&ok_rows>=(int)VT_OK_MIN){
 int ny=-1,fy=-1,fl=0,fr=0,best=0x7fffffff;
 for(y=VT_ROW_BOT-1;y>=VT_ROW_TOP;y--){
  if(vt_rowc[y]==VT_ROW_FULL&&vt_trust(y+1)){ny=y+1;break;}
 }
 if(ny>=0){
  int aim=vt_rowv[ny];
  /* With a clipped incoming lane the visible edge is turning away;
   * vehicle-forward (optical centre) is the straight-exit reference. */
  if(clip_rows>=(int)VT_OK_MIN)aim=VT_W/2;
  for(y=ny-1;y>=VT_ROW_TOP;y--){
   if(vt_rowc[y]==VT_ROW_FULL||vt_sn[y]==0)continue;
   for(k=0;k<(int)vt_sn[y];k++){
    int l=vt_sl[y][k],r=vt_sr[y][k],c=(l+r)/2,score,yy,support=0;
    if(!vt_seed_ok(y,l,r))continue;
    /* A persistent exit is required; a single edge speck cannot steer. */
    for(yy=y-1;yy>=VT_ROW_TOP&&yy>=y-4;yy--){
     int j;
     for(j=0;j<(int)vt_sn[yy];j++)
      if(abs(vt_sl[yy][j]-l)<=VT_EDGE_WIN*2&&abs(vt_sr[yy][j]-r)<=VT_EDGE_WIN*2){support++;break;}
    }
    if(support<3)continue;
    if(!vt_white_bridge(bin,y,c,ny,vt_rowv[ny]))continue;
    score=abs(c-aim);
    if(score<best){best=score;fy=y;fl=l;fr=r;}
   }
   if(fy>=0)break;
  }
  if(fy>=0&&ny-fy>=4){
   int L=fl,R=fr,clip,fake,fc=(fl+fr)/2,nc=vt_rowv[ny];
   v->degraded=1;
   /* Replace the old far-side chain rather than keeping its wrong branch. */
   for(y=VT_ROW_TOP;y<ny;y++){vt_rowv[y]=-1;vt_roww[y]=0;
    if(vt_rowc[y]!=VT_ROW_FULL)vt_rowc[y]=VT_ROW_NONE;}
   vt_rowc[fy]=VT_ROW_OK;vt_rowv[fy]=(int16_t)fc;vt_roww[fy]=(int16_t)(fr-fl+1);ok_rows++;
   for(y=fy-1;y>=VT_ROW_TOP;y--){
    if(!vt_take_row(t,y,&L,&R,&clip,&fake))break;
    ok_rows++;
   }
   for(y=fy+1;y<ny;y++)if(!vt_trust(y)){
    vt_rowv[y]=(int16_t)(fc+(nc-fc)*(y-fy)/(ny-fy));
    vt_roww[y]=vt_wfit[y]>=16?vt_wfit[y]:(int16_t)(fr-fl+1);
    vt_rowc[y]=VT_ROW_LINE;ok_rows++;
   }
  }
 }
}
/* A merged far junction is not a measured lane cross-section. Preserve
 * the near chain and only reconstruct the far ROI when a bounded exit has
 * eight supported rows above it and a white bridge to the near path. */
{
 int anchor=75,exit_y=-1,exit_x=0;
 /* VT_CROSS feeds edge_mode==3 live (TrackEdgeMode()->vt_edge_mode()); the
  * forward reconstruction is exactly what crossing a junction needs, so run
  * it for NORMAL(0) and CROSS(3).  Static replays defaulted to 0 and hid
  * this: with a learned width the far rows fell back to the clip-recenter
  * (visible-left-edge + width/2) and the midline folded onto the left branch. */
 if((t->edge_mode==0u||t->edge_mode==3u)&&vt_trust(anchor)&&bin[28*VT_W]&&
    bin[28*VT_W+150]&&!bin[28*VT_W+VT_W-1]){
  int aim=vt_rowv[anchor],best=0x7fffffff;
  for(y=22;y>=18;y--){
   int pos=0;
   while(pos<VT_W){
    int l,r,c,yy,support=0;
    if(!bin[y*VT_W+pos]){pos++;continue;}
    l=pos;while(pos<VT_W&&bin[y*VT_W+pos])pos++;r=pos-1;c=(l+r)/2;
    if(l<=2||r>=VT_W-3||r-l+1<24||r-l+1>100)continue;
    for(yy=y-1;yy>=y-8&&yy>=0;yy--){
     if(bin[yy*VT_W+c]&&bin[yy*VT_W+c-6]&&bin[yy*VT_W+c+6])support++;else break;
    }
    if(support<8||!vt_white_bridge(bin,y,c,anchor,aim))continue;
    if(abs(c-aim)<best){best=abs(c-aim);exit_y=y;exit_x=c;}
   }
  }
  if(exit_y>=0){
   for(y=VT_ROW_TOP;y<anchor;y++){
    int c=exit_x+(vt_rowv[anchor]-exit_x)*(y-exit_y)/(anchor-exit_y);
    if(!bin[y*VT_W+c])continue;
    vt_rowv[y]=(int16_t)c;vt_rowc[y]=VT_ROW_LINE;
    vt_roww[y]=vt_wfit[y]>=16?vt_wfit[y]:60;
   }
   v->degraded=1;
  }
 }
}
#if 0 /* Retired global ring heuristic: the isolated big_ring module owns this. */
/* v112: the ring split may lie above the normal analysis ROI.  Never use
 * the centre of a merged white junction as the far exit when two persistent
 * corridors are visible just above it.  Keep the near path unchanged. */
{
 int ay=-1,ax=0,sy=-1,sx=0,best=0x7fffffff;
 for(y=90;y>=70;y--)if(vt_trust(y)){ay=y;ax=vt_rowv[y];break;}
 if(ay>=0){
  for(y=27;y>=18;y--){
   int ls[4],rs[4],n=0,p=0;
   while(p<VT_W){int l,r;if(!bin[y*VT_W+p]){p++;continue;}
    l=p;while(p<VT_W&&bin[y*VT_W+p])p++;r=p-1;
    if(r-l+1>=16&&n<4){ls[n]=l;rs[n++]=r;}
   }
   if(n!=2||ls[1]-rs[0]<8)continue;
   for(k=0;k<n;k++){
    int l=ls[k],r=rs[k],c=(l+r)/2,yy,support=0,score;
    if(l<=2||r>=VT_W-3)continue;
    for(yy=y-1;yy>=y-3&&yy>=0;yy--)
     if(bin[yy*VT_W+c]&&bin[yy*VT_W+clamp(l+3,0,VT_W-1)]&&bin[yy*VT_W+clamp(r-3,0,VT_W-1)])support++;
    if(support<3||!vt_white_bridge(bin,y,c,ay,ax))continue;
    score=abs(c-ax);
    if(score<best){best=score;sy=y;sx=c;}
   }
   if(sy>=0)break;
  }
  if(sy>=0){
   for(y=VT_ROW_TOP;y<ay;y++){
    int c=sx+(ax-sx)*(y-sy)/(ay-sy);
    if(!bin[y*VT_W+c])continue;
    vt_rowv[y]=(int16_t)c;
    vt_roww[y]=vt_wfit[y]>=16?vt_wfit[y]:(int16_t)60;
    vt_rowc[y]=VT_ROW_LINE;
   }
   v->degraded=1;
  }
 }
}
#endif
/* ---------- 第 3 遍：可信测量 + 模型插值，生成连续中线 ----------
 * v106：除了"一个可信测量都没有"，还多一种必须"顶住"的情况——**正在穿十字路口**：
 * 横向白带把画面铺满（FULL 行很多），可信测量行却少到个位数。这时那几行多半是横带
 * 边缘的残缺白斑，中心不可信；让它们决定整条中线，实车症状就是"黄线在屏幕左边和
 * 中线之间来回闪"。判据只看"FULL 行多 + 可信行少"，与赛道形状无关——急弯每行只有
 * 一条窄白带、其余是暗的，永远凑不出这么多 FULL 行，所以不会误伤急弯。 */
int crossing=(full_rows>=(int)VT_FULL_MIN_ROWS)&&(ok_rows<(int)VT_OK_MIN),hold=0;
if(ok_rows==0||crossing){
 /* 整帧白茫茫 / 正在穿路口：用上一帧中心直着顶过去（degraded 限速），
  * 直到路口走完、下方重新出现可信的路面横截面。 */
 if(max_w>=VT_W/2&&t->previous_mid>0){
   int m=t->previous_mid,fl,fr;
   hw=(t->previous_right-t->previous_left)/2;
   if(hw<8)hw=8;
   if(hw>70)hw=70;
   for(y=VT_ROW_BOT;y>=VT_ROW_TOP;y--){
    fl=m-hw;fr=m+hw;
    if(fl<0)fl=0;
    if(fr>VT_W-1)fr=VT_W-1;
    t->left[y]=(int16_t)fl;t->right[y]=(int16_t)fr;t->mid[y]=(int16_t)m;
    valid++;
    if(y>=90){near++;nl+=fl;nr+=fr;nw+=fr-fl+1;nn++;}
    if(y>=48&&y<=78){int weight=16-abs(y-63)/2;sum+=m*weight;wsum+=weight;}
    if(y>=38&&y<52){fsum+=m;fn++;}
   }
   v->all_white=1;v->degraded=1;hold=1;
  }
 } else {
  if(hwn>0){hw=hwsum/hwn;if(hw<8)hw=8;if(hw>70)hw=70;}
  for(y=VT_ROW_BOT;y>=VT_ROW_TOP;y--){
   int m=-1,rw=0,fl,fr;
   /* v107：VT_ROW_LINE 是"拉线"推出来的，没有真实边界 → 计 modeled 不计 measured */
   if(vt_trust(y)){m=vt_rowv[y];rw=vt_roww[y];if(vt_rowc[y]==VT_ROW_LINE)modeled++;else measured++;}
   else{
    int yb=-1,ya=-1,kk;
    for(kk=y+1;kk<=VT_ROW_BOT;kk++)if(vt_trust(kk)){yb=kk;break;}  /* 更近的可信行 */
    for(kk=y-1;kk>=VT_ROW_TOP;kk--)if(vt_trust(kk)){ya=kk;break;}  /* 更远的可信行 */
    if(yb>=0&&ya>=0)m=vt_rowv[yb]+(vt_rowv[ya]-vt_rowv[yb])*(y-yb)/(ya-yb);
    else if(yb>=0)m=vt_rowv[yb];
    else if(ya>=0)m=vt_rowv[ya];
    if(m<0)continue;                                                       /* 本帧无任何证据 */
    if(m<2)m=2;
    if(m>VT_W-3)m=VT_W-3;
    /* Do not draw an unobserved extension across blue background. */
    if(!bin[y*VT_W+m]){v->degraded=1;continue;}
    modeled++;
   }
   if(rw<=0)rw=hw*2+1;
   fl=m-rw/2;fr=m+rw/2;
   if(fl<0)fl=0;
   if(fr>VT_W-1)fr=VT_W-1;
   t->left[y]=(int16_t)fl;t->right[y]=(int16_t)fr;t->mid[y]=(int16_t)m;
   valid++;
   if(y>=90){near++;nl+=fl;nr+=fr;nw+=fr-fl+1;nn++;}
   if(y>=48&&y<=78){int weight=16-abs(y-63)/2;sum+=m*weight;wsum+=weight;}
   if(y>=38&&y<52){fsum+=m;fn++;}
  }
 }
 v->rows=valid;v->near_rows=near;v->measured=(uint32_t)measured;v->model_rows=(uint32_t)modeled;
 v->clip_rows=(uint32_t)clip_rows;
 /* Coverage is not confidence: modeled rows carry no measurement evidence. */
 v->confidence=(uint32_t)measured*100u/83u;
/* 一行可信测量都没有 / 正在穿路口（hold）→ 不算"看到道路"，交给 controller 走
 * degraded 限速 + 上一帧中心顶住；all_white 的 2s 超时兜底仍然生效。 */
/* v121: a frame with plenty of REAL measurement somewhere (a long clean
 * corridor) is "seen road" even if the near rows are a couple short; the
 * J1 pose had 63 measured rows but only 5 near rows and was rejected. */
v->valid=valid>=20u&&(near>=6u||measured>=40u)&&wsum>0&&ok_rows>0&&!hold;
 if(v->valid){
  v->mid=sum/wsum;v->far_mid=fn?fsum/fn:v->mid;
  v->left=nn?nl/nn:-1;v->right=nn?nr/nn:-1;v->width=nn?nw/nn:0;
  t->previous_mid=nn?(nl+nr)/(2*nn):v->mid;t->previous_left=v->left;t->previous_right=v->right;t->initialized=1;
  /* 补出来的行占了一大块（正在穿路口/环岛入口）：降级限速，别全速冲过去 */
  if(modeled>=(int)VT_MODEL_DEGRADE)v->degraded=1;
 } else if(v->all_white){
  v->mid=sum/wsum;v->far_mid=fn?fsum/fn:v->mid;
  v->left=nn?nl/nn:-1;v->right=nn?nr/nn:-1;v->width=nn?nw/nn:0;
 }
 if(op_n){v->open_left=op_l*1000u/op_n;v->open_right=op_r*1000u/op_n;}
 v->corner_left=corners_l>=2;v->corner_right=corners_r>=2;
 v->straight_left=op_n>=20&&straight_l*100u/op_n>=75u;
 v->straight_right=op_n>=20&&straight_r*100u/op_n>=75u;
 v->fork=fork_rows>=4;v->hole=hole_rows>=4;
 /* Independent zebra ROI: genuine grayscale-derived binary pixels, not the
  * synthetic navigation line. Count internal black bars inside white road. */
 for(y=78;y<=105;y+=3){
  int l=12,r=VT_W-13,bars=0,start;
  while(l<r&&!bin[y*VT_W+l])l++;
  while(r>l&&!bin[y*VT_W+r])r--;
  if(r-l<40)continue;
  for(x=l;x<=r;){
   if(bin[y*VT_W+x]){x++;continue;}
   start=x;while(x<=r&&!bin[y*VT_W+x])x++;
   if(x-start>=2&&x-start<=10)bars++;
  }
  if(bars>=6)finish_rows++;
 }
 v->finish=finish_rows>=3;
 /* A zebra may fragment road runs; it is valid only as a measured stop
  * marker, not as a path to accelerate towards. */
}
void vt_control_reset(VTControl *c,uint32_t now){memset(c,0,sizeof(*c));c->now=now;}
void vt_control_stop(VTControl *c,uint32_t fault)
{
 c->active=0;c->fault=fault;c->pwm[0]=c->pwm[1]=0;c->target[0]=c->target[1]=0;c->steering=0;
 c->integral[0]=c->integral[1]=0;c->previous_error=c->derivative=c->filtered_error=0;c->steering_initialized=0;c->steering_stamp=0;
 c->phase=VT_NORMAL;c->phase_count=c->seen_missing=c->normal_frames=0;c->ring_direction=c->ring_yaw0=0;
 c->cool_until=0;c->good=c->finish_frames=0;c->stall_ms[0]=c->stall_ms[1]=0;c->lost_frames=0;c->white_frames=0;
}
void vt_control_start(VTControl *c,uint32_t now){vt_control_reset(c,now);c->active=1;c->start_ms=now;}
static void phase(VTControl *c,unsigned p){c->phase=p;c->phase_ms=c->now;c->phase_count=0;}
uint32_t vt_edge_mode(const VTControl *c)
{
 if(c->phase==VT_CROSS)return 3;
 if(c->phase==VT_NORMAL)return 0;
 if(c->phase==VT_RING_IN||c->phase==VT_RING_OUT)return c->ring_direction>0?1u:2u;
 return c->ring_direction>0?2u:1u;
}
static void element(VTControl *c,const VTFrame *v,int yaw10)
{
 int rel=abs(yaw10-c->ring_yaw0);
 int opening=(v->open_left>=600&&v->corner_left&&v->straight_right)?1:
             ((v->open_right>=600&&v->corner_right&&v->straight_left)?-1:0);
 if(c->phase==VT_NORMAL){
  if(v->valid&&!v->hole&&!v->fork&&v->width<145)c->normal_frames++;
  if(v->corner_left&&v->corner_right){phase(c,VT_CROSS);return;}
  if(c->now>=c->cool_until&&c->normal_frames>=15&&opening){
   if(opening==c->ring_direction)c->phase_count++;else{c->ring_direction=opening;c->phase_count=1;}
   if(c->phase_count>=3){c->ring_yaw0=yaw10;c->seen_missing=0;phase(c,VT_RING_BEGIN);}
  }else c->phase_count=0;
 }else if(c->phase==VT_CROSS){
  if(v->valid&&!v->fork&&!v->corner_left&&!v->corner_right){if(++c->phase_count>=6)phase(c,VT_NORMAL);}else c->phase_count=0;
 }else{
  unsigned opened=c->ring_direction>0?v->open_left:v->open_right;
  unsigned opposite_corner=c->ring_direction>0?v->corner_right:v->corner_left;
  unsigned opposite_straight=c->ring_direction>0?v->straight_right:v->straight_left;
  if(rel>4600||c->now-c->phase_ms>12000u){vt_control_stop(c,VT_RING_TIMEOUT);return;}
  if(c->phase==VT_RING_BEGIN){
   if(opened>=600)c->seen_missing=1;
   if(c->seen_missing&&opened<300&&v->valid){if(++c->phase_count>=3)phase(c,VT_RING_IN);}else c->phase_count=0;
  }else if(c->phase==VT_RING_IN){
   if(rel>=600&&v->valid){if(++c->phase_count>=3)phase(c,VT_RING_RUN);}else c->phase_count=0;
  }else if(c->phase==VT_RING_RUN){
   if(rel>=2300&&opposite_corner&&v->fork){if(++c->phase_count>=3)phase(c,VT_RING_OUT);}else c->phase_count=0;
  }else if(c->phase==VT_RING_OUT){
   if(v->valid&&opposite_straight&&!v->hole){if(++c->phase_count>=6)phase(c,VT_RING_END);}else c->phase_count=0;
  }else if(c->phase==VT_RING_END){
   if(v->valid&&!v->fork&&v->width<145&&rel>=2800){
    if(++c->phase_count>=8){c->ring_completed++;c->cool_until=c->now+2000u;c->normal_frames=0;phase(c,VT_NORMAL);}
   }else c->phase_count=0;
  }
 }
}
void vt_step(VTControl *c,const VTFrame *v,uint32_t now,uint32_t dt_ms,int32_t left,int32_t right,int32_t yaw10,int32_t gyro,uint32_t imu_valid,uint32_t main_age,uint32_t tipped,uint32_t scale,uint32_t gain_scale)
{
 unsigned i,new_frame,reliable,guard;int target,inner;float e,d,turn,kp,gd=0.0f;
 /* v122 (CQU-style yaw-rate damping): CQU injects (err - yawrate/10) into
  * the steering PID; we subtract a bounded scaled yaw rate from the error:
  * while the car is already rotating left (gyro>0) the left demand is
  * reduced -> less overshoot / less wobble. Flip VT_GYRO_DAMP_SIGN if the
  * on-car rotation check disagrees. */
 if(imu_valid){gd=(float)gyro*(float)VT_GYRO_DAMP_K;
  if(gd>VT_GYRO_DAMP_MAX)gd=VT_GYRO_DAMP_MAX;
  if(gd< -VT_GYRO_DAMP_MAX)gd=-VT_GYRO_DAMP_MAX;
  gd*=VT_GYRO_DAMP_SIGN;}
 c->now=now;
 if(dt_ms<10u||dt_ms>40u){if(c->active)vt_control_stop(c,VT_MAIN);return;}
 c->speed[0]=(uint32_t)abs(left)*20u/dt_ms;c->speed[1]=(uint32_t)abs(right)*20u/dt_ms;
 if(!c->active)return;
 if(v->special_route&&v->special_fault){vt_control_stop(c,v->special_fault);return;}
 if(!imu_valid&&c->phase!=VT_NORMAL&&c->phase!=VT_CROSS){vt_control_stop(c,VT_IMU);return;}
 if(main_age>250u){vt_control_stop(c,VT_MAIN);return;}
 if(now-v->stamp>150u){vt_control_stop(c,VT_CAMERA);return;}
 if(tipped){vt_control_stop(c,VT_TILT);return;}
 if(c->speed[0]>2500u||c->speed[1]>2500u){vt_control_stop(c,VT_ENCODER);return;}
 new_frame=v->sequence!=c->last_seq;
 if(new_frame){
  c->last_seq=v->sequence;
  /* v143 盲区计数：连续无有效线帧数（含 all_white），线恢复即清零 */
  if(!v->valid){if(c->blind_frames<100000u)c->blind_frames++;}else c->blind_frames=0;
  if(v->finish&&now-c->start_ms>=2000u){if(++c->finish_frames>=3){vt_control_stop(c,VT_FINISH);return;}}else c->finish_frames=0;
  if(v->special_route){c->phase=VT_NORMAL;c->phase_count=0;c->normal_frames=0;c->cool_until=now+2000u;}
  else if(v->valid)element(c,v,yaw10);
 }
 if(!c->active)return;
 if(v->finish&&now-c->start_ms>=2000u){c->pwm[0]=c->pwm[1]=0;c->target[0]=c->target[1]=0;c->steering=0;return;}
 reliable=v->valid&&!v->all_white&&(v->measured>=8u||v->confidence>=40u);
 if(!v->valid&&(!v->degraded||(!v->all_white&&(v->near_rows<6u||(v->measured==0u&&v->confidence==0u))))){
  /* 丢线找回（对齐国赛学长做法）：朝"最后一次转向方向"打满舵 + 双轮降到爬行速度，
   * 让车立刻把弯转回来，而不是保持原舵量直冲出去（v100 保持舵量的版本实车救不回）。
   * 不拿无效帧更新偏差/微分；线一回视野下一帧无缝接回正常控制；
   * 连续 VT_LOST_FRAMES 帧仍无道路才判丢线停车。 */
  if(++c->lost_frames>=VT_LOST_FRAMES){vt_control_stop(c,VT_LOST);return;}
  {
   unsigned crawl=(unsigned)(VT_LOST_CRAWL*3);
   /* Keep the last computed steering angle while blind. At the same time
   * trim the speed; a full lock here made the previous version overreact. */
   if(c->reliable_seen)c->steering=c->reliable_steering;
   c->blind_distance+=(uint32_t)((c->speed[0]+c->speed[1])/2u);
   c->steering=clamp(c->steering,-100,100);
   for(i=0;i<2;i++){
    c->target[i]=(uint32_t)VT_LOST_CRAWL;
    if(c->pwm[i]>crawl+160u)c->pwm[i]-=160u;
    else if(c->pwm[i]>crawl)c->pwm[i]=crawl;   /* 只降不升 */
   }
   c->updates++;
  }
  return;
 }
 c->lost_frames=0;
 /* 整帧全白（路口 / 环岛入口 / 冲出赛道）：这几种图长得一样，只能靠时间区分。
   先让惯性顶着走，连顶 VT_WHITE_MAX_FRAMES 帧（≈2s）就直接判丢线停车。 */
 if(v->all_white){if(++c->white_frames>=VT_WHITE_MAX_FRAMES){vt_control_stop(c,VT_LOST);return;}}
 else c->white_frames=0;
 /* One center filter and fixed-period derivative; reset state is explicit.
  * Physical positive turn = LEFT, original camera positive x = RIGHT. */
 /* v119: v118 guard over-triggered (degraded/fork/hole/corner are ALSO set
  * in the start / dashed-line / first-curve region, verified on a real
  * recording), which killed normal steering. Keep the guard engaged only
  * on all-white frames; the junction-strength policy needs a discriminator
  * that the recorded flags (now including fork/hole/corner/finish) can
  * actually separate. */
 guard=v->all_white;
 /* v120: mid<0 (invalid frame that still reached control) must never
  * invent a +/-1.0 lock from a missing line; keep the previous error. */
 if(v->mid>=0)e=(94.0f-(float)v->mid)/94.0f;
 else e=c->steering_initialized?c->filtered_error:0.0f;
 if(guard){if(e>0.15f)e=0.15f;if(e< -0.15f)e=-0.15f;}
 /* v120: a mostly-fabricated midline (too few real measurements) must not
  * steer the car. On the real J1 frame valid=1/measured=9/mid=62 gave
  * e=+0.34 -> full-left lock that threw the car into the perpendicular
  * side road. Drive straight until real measurements return (track doc:
  * crossings are passed straight). */
 if(v->valid&&v->measured<12u&&!v->special_route)e=0.0f;
 if(v->mid>=0)e-=gd;   /* v122: yaw-rate damping (only when this frame had a line) */
 if(!c->steering_initialized){c->filtered_error=e;c->previous_error=e;c->derivative=0;c->steering_initialized=1;c->steering_stamp=v->stamp;}
 else if(new_frame){
  float change=e-c->filtered_error,capv=guard?0.12f:0.20f;
  /* Bound a single-frame outlier without delaying a persistent bend forever. */
  if(change>capv)change=capv;
  if(change< -capv)change=-capv;
  c->filtered_error+=change*0.65f;
 }
 if(new_frame){
  unsigned image_dt=v->stamp-c->steering_stamp;
  d=image_dt?(c->filtered_error-c->previous_error)*20.0f/(float)clamp((int)image_dt,10,150):0.0f;
  c->steering_stamp=v->stamp;
  c->previous_error=c->filtered_error;
  c->derivative=c->derivative*0.35f+d*0.65f;
 }else c->derivative*=0.65f;
 kp=10.0f+20.0f*(float)clamp((int)(abs((int)(c->filtered_error*1000))/0.35f),0,1000)/1000.0f;
 turn=(kp*c->filtered_error+20.0f*c->derivative)*10.0f*(float)gain_scale/100.0f;
 /* Do not add uncalibrated raw gyro to the steering demand. */
 if(e<0.025f&&e> -0.025f)turn*=0.5f; /* decay towards center, no retained turn */
 /* A brief neutral, motor-off arming interval. Do not derive a D kick
  * from the first image or move wheels while re-engaging the servo. */
 if(now-c->start_ms<100u){c->steering=0;c->pwm[0]=c->pwm[1]=0;c->target[0]=c->target[1]=0;return;}
 {
  int desired=clamp((int)turn,-100,100);
  if(guard){if(desired>45)desired=45;if(desired< -45)desired=-45;}  /* v118: no large swings on junction ground */
  int speed=(int)((c->speed[0]+c->speed[1])/2u);
  int slew=(speed>400?10:20)*(int)dt_ms/20;
  if(slew<5)slew=5;
  c->steering+=clamp(desired-c->steering,-slew,slew);
  if(v->all_white&&c->reliable_seen)c->steering=c->reliable_steering;
  if(reliable){c->reliable_steering=c->steering;c->reliable_stamp=now;c->reliable_seen=1u;c->blind_distance=0;}
 }
 target=460-(int)(350.0f*(float)abs((int)(e*1000))/1000.0f);if(target<280)target=280;
 if(c->phase!=VT_NORMAL)target=260;
 if(now-c->start_ms<2000u){int ramp=150+(int)((now-c->start_ms)*310u/2000u);if(target>ramp)target=ramp;}
 target=target*(int)clamp((int)scale,50,200)/100;
 if(v->degraded){int dcap=(int)VT_DEGRADE_SPEED*(int)clamp((int)scale,50,200)/100;if(target>dcap)target=dcap;}
 /* Pixel-space lookahead trend, not calibrated metric curvature. Encoder
  * overspeed adds braking demand; it never raises the existing target. */
 if(v->valid&&v->mid>=0&&v->far_mid>=0){
  int cap=460,actual=(int)((c->speed[0]+c->speed[1])/2u);
  if(v->measured<24u||v->confidence<40u||v->all_white){
   cap=220; /* Too few points: slow, never silently disable speed planning. */
  }else{
   int bend=abs(v->far_mid-v->mid);
   /* v146 弯道速度放松（用户方案"弯道速度提高一点"）：
    * ① 弯度限速 (bend-4)*10→*8、最多砍 240→200（硬弯 220→260）；
    * ② 转向限速 360-|steer| → 400-|steer|（硬弯 260→300）。
    * 影子验证：硬弯 target 250→292(+17%)、中弯 298→338(+13%)。 */
   if(bend>4)cap=460-clamp((bend-4)*8,0,200);
   if(abs(c->steering)>30){int turncap=400-abs(c->steering);if(cap>turncap)cap=turncap;}
  }
  cap=cap*(int)clamp((int)scale,50,200)/100;
  if(actual>cap+30){int extra=clamp((actual-cap)/2,0,80);cap-=extra;}
  if(cap<100)cap=100;
  if(target>cap)target=cap;
 }
 /* Deceleration follows demand immediately; acceleration recovers slowly. */
 if(!c->speed_target_state||target<(int)c->speed_target_state)c->speed_target_state=(uint32_t)target;
 else if(target>(int)c->speed_target_state){
  unsigned rise=(unsigned)(8u*dt_ms/20u);if(!rise)rise=1;
  c->speed_target_state+=rise;
  if(c->speed_target_state>(uint32_t)target)c->speed_target_state=(uint32_t)target;
 }
 target=(int)c->speed_target_state;
 /* 内轮差速：对齐国赛学长"内轮刹死咬弯"的思路（学长内外比可达 1:10，
  * 我们保守取满舵时内轮降到 45%），转弯时用内轮减速把车头拉进弯，
  * 减少冲出赛道导致的丢线。 */
 inner=target*(100-abs(c->steering)*55/100)/100;
 c->target[0]=(uint32_t)(c->steering>0?inner:target);c->target[1]=(uint32_t)(c->steering<0?inner:target);
 /* v143 盲区降速：连续 ≥2 拍无有效线（约130ms；单帧闪断不触发）→ 强制把输出下压到爬行档，
  * 跳过 PI（杜绝"车轮被挡/反馈偏低→积分反向加油"的盲飞加速）。线回来立即恢复，无迟滞。
  * 依据：6 次实车盲飞事件（PWM 648→1980/2820）+ 影子验证（原版复现/补丁压速）。 */
 if(!v->valid&&c->blind_frames>=2u){
  unsigned crawlp=(unsigned)(VT_LOST_CRAWL*3);
  for(i=0;i<2;i++){
   c->target[i]=(uint32_t)VT_LOST_CRAWL;
   if(c->pwm[i]>crawlp+160u)c->pwm[i]-=160u;
   else if(c->pwm[i]>crawlp)c->pwm[i]=crawlp;
  }
  c->updates++;
  return;
 }
 for(i=0;i<2;i++){
  float err=(float)c->target[i]-(float)c->speed[i],candidate=c->integral[i]+err*0.20f*(float)dt_ms/20.0f;
  float output=(float)c->target[i]*3.0f+err*2.5f+candidate;unsigned previous=c->pwm[i];
  if(candidate>1800)candidate=1800;
  if(candidate< -1800)candidate=-1800;
  if(!((output>5500&&err>0)||(output<0&&err<0)))c->integral[i]=candidate;
  output=(float)c->target[i]*3.0f+err*2.5f+c->integral[i];
  if(output<0)output=0;
  if(output>5500)output=5500;
  if(output>(float)previous+100)output=(float)previous+100;
  if(output<(float)previous-160)output=(float)previous-160;
  if(output<0)output=0;
  c->pwm[i]=(uint32_t)output;
  if(now-c->start_ms>500u&&c->target[i]>50u&&c->speed[i]<4u&&c->pwm[i]>650u)c->stall_ms[i]+=dt_ms;else c->stall_ms[i]=0;
  if(c->stall_ms[i]>=400u){vt_control_stop(c,VT_STALL);return;}
 }
 c->updates++;
}
