/* big_ring.c — 大圆环 · 反向行驶重写版（br2, 2026-10-11；v134 稳健化）
 *
 * 旧状态机（mouth/route/BR_*，正向设计）已整体废弃，见 big_ring.c.bak_brv1。
 * 本文件按用户方案重写（主方案A）：
 *   点3 → 武装；点2(1st) → 右转进环+计数；环内巡线；再遇点2 → 保持直行；点1 → 退出。
 *
 * v134 修复背景：v133 首跑全程未触发（实车走线偏离标定位置仅±5px 检测器即失效）。
 * 修复方法：以三帧为种子，合成 30 个平移/亮度变体 + 8 个真实负样本，在 PC 上做参数
 * 搜索（scripts/search_params_v4.py / v5），得到容忍 ±20~30px / ±8行 的参数组合；
 * 全部变体验证通过、负样本全静默（scripts/test_bigring_rev_detect.c 固化）。
 *
 *
 * v141（2026-10-11，用户方案；从 v134 重新起底）：
 *   ① 左转协调——环内允许轻微左转；大左转（mid<BR2_LEFT_SOFT_MID）改为右转
 *      （修 f104-119 满左+10°约1s 的"视觉锁左侧线"事件；用户："大左转时让他右转"）；
 *   ② 8s 强制关闭保险（同 v138）——开跑后 8s 未 DONE 则强制 DONE。
 *   改动全部在本模块（big_ring.c/h）；track_core 等环岛外逻辑零改动。
 * 规则（行段几何，>=16px 白段；行号 y 自上=远 到下=近）：
 *   点3 武装: y41-56 单段且右缘>=140 且左缘 12..95 行数>=8
 *             且 y18-31 右缘 85..160 行数>=8
 *             且 y100-119 左缘最小>=8
 *   点2 口:   y26-48 存在"多段且末段右缘>=162 且首段左缘>=25"的行，行数>=2，
 *             其中末段左缘逐行收敛(减小>2)次数>=2
 *             且 y100-119 左缘最小 0..26
 *   点1 退出: y18-28 末段右缘>=166 行数>=4
 *             且 y30-42 末段右缘 112..160 行数>=4
 */
#include "big_ring.h"
#include <string.h>
#include <stdlib.h>

/* 可调参数（首次实车后按现象微调） */
#define BR2_TURN_BIAS     30     /* 右转注入偏置：mid=94+bias（e<0 → 右转） */
#define BR2_TURN_MS       600u   /* 右转持续时间 */
#define BR2_LOOP_MIN_MS   1200u  /* 进环后最短再检出时间（防刚进环立即二次触发） */
#define BR2_STRAIGHT_MS   600u   /* 出环口直行保持（最短） */
#define BR2_STRAIGHT_MAX  1500u  /* 直行保持上限 */
#define BR2_ARMED_TO_MS   30000u /* 武装后等待进环口超时 → 放弃 */
#define BR2_LOOP_TO_MS    25000u /* 环内超时（没等到二次口）→ 放弃 */
#define BR2_TAIL_TO_MS     8000u /* 等待点1超时 → 结束（8秒兜底，相当于方案B的"跑完就关"） */
#define BR2_CONFIRM_WIN   165u   /* 特征确认窗口：165ms(≈5帧@33ms)内累计>=2次命中即触发 */

static int segments(const uint8_t *b,int y,int *ls,int *rs)
{
 int x=0,n=0;
 while(x<VT_W){int l,r;if(!b[y*VT_W+x]){x++;continue;}
  l=x;while(x<VT_W&&b[y*VT_W+x])x++;r=x-1;
  if(r-l+1>=16&&n<4){ls[n]=l;rs[n++]=r;}
 }
 return n;
}
int br2_detect_arm(const uint8_t *bin)
{
 int y,c1=0,c2=0,minL=9999;
 for(y=41;y<=56;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n==1&&rs[0]>=140&&ls[0]>=12&&ls[0]<=95)c1++;
 }
 for(y=18;y<=31;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n>=1&&rs[n-1]>=85&&rs[n-1]<=160)c2++;
 }
 for(y=100;y<=119;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n>=1&&ls[0]<minL)minL=ls[0];
 }
 return c1>=8&&c2>=8&&minL>=8;
}
int br2_detect_mouth(const uint8_t *bin)
{
 int y,c1=0,conv=0,prevl=-1,minL=9999;
 for(y=26;y<=48;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n>=2&&rs[n-1]>=162&&ls[0]>=25){
   c1++;
   if(prevl>=0&&ls[n-1]<prevl-2)conv++;
   prevl=ls[n-1];
  }else prevl=-1;
 }
 for(y=100;y<=119;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n>=1&&ls[0]<minL)minL=ls[0];
 }
 return c1>=2&&conv>=2&&minL<=26;
}
int br2_detect_exit(const uint8_t *bin)
{
 int y,c1=0,c2=0;
 for(y=18;y<=28;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n>=1&&rs[n-1]>=166)c1++;
 }
 for(y=30;y<=42;y++){
  int ls[4],rs[4],n=segments(bin,y,ls,rs);
  if(n>=1&&rs[n-1]>=112&&rs[n-1]<=160)c2++;
 }
 return c1>=4&&c2>=4;
}
void br2_reset(BR2State *s){memset(s,0,sizeof(*s));s->phase=BR2_OFF;}
/* v134 窗口确认：165ms 内累计 >=2 次命中即触发（容忍高速下的单帧漏检/闪烁）。 */
static int confirm(BR2State *s,int seen,uint32_t stamp)
{
 if(!seen)return 0;
 if(stamp-s->last_hit>BR2_CONFIRM_WIN)s->hits=0;
 s->last_hit=stamp;
 if(++s->hits>=2u){s->hits=0;return 1;}
 return 0;
}
static void enter(BR2State *s,uint32_t phase,uint32_t stamp)
{s->phase=phase;s->stamp=stamp;s->hits=0;s->last_hit=0;}
void br2_process(BR2State *s,VTTracker *t,const uint8_t *bin,VTFrame *f,
 uint32_t enabled,uint32_t running,uint32_t imu_valid,int32_t yaw10)
{
 int am,mo,ex;
 (void)t;(void)imu_valid;(void)yaw10;
 if(!enabled){br2_reset(s);return;}
 if(!running||s->phase==BR2_DONE)return;
 /* v141 保险（用户方案）：车发动（首个 running 帧）起算——BR2_GUARD_MS 内模块必须关闭（DONE）。
  * 起点=车发动（用户指定，同 v138 语义）。到点仍未关闭 → 强制进入 DONE：静默、零注入、
  * 不再武装，与自然跑完状态一致，不影响后续任何巡线逻辑。
  * （正常一跑约 8s 内自然 DONE → 保险不动作；全程跑若发车到环岛超过 8s，模块会在到环岛前关闭。） */
 if(!s->guard_run){s->guard_run=1u;s->guard_start=f->stamp;}
 if((f->stamp-s->guard_start)>=BR2_GUARD_MS){
  s->guard_closed=1u;
  enter(s,BR2_DONE,f->stamp);
  return;
 }
 if(f->sequence!=s->seq){
  s->seq=f->sequence;
  /* v134 诊断：每帧刷新三个检测器原始输出（供记录通道回读） */
  am=br2_detect_arm(bin);mo=br2_detect_mouth(bin);ex=br2_detect_exit(bin);
  s->dbg=(uint32_t)((am?1:0)|(mo?2:0)|(ex?4:0));
  switch(s->phase){
   case BR2_OFF:
    if(confirm(s,am,f->stamp))enter(s,BR2_ARMED,f->stamp);
    break;
   case BR2_ARMED:
    if(confirm(s,mo,f->stamp)){
     s->count_mark2=1u;s->entered_ms=f->stamp;
     s->hold_until=f->stamp+BR2_TURN_MS;
     enter(s,BR2_TURN,f->stamp);
    }else if(f->stamp-s->stamp>BR2_ARMED_TO_MS)enter(s,BR2_DONE,f->stamp);
    break;
   case BR2_TURN:
    if(f->stamp>=s->hold_until)enter(s,BR2_LOOP,f->stamp);
    break;
   case BR2_LOOP:
    if(f->stamp-s->stamp>=BR2_LOOP_MIN_MS&&confirm(s,mo,f->stamp)){
     s->hold_until=f->stamp+BR2_STRAIGHT_MAX;
     enter(s,BR2_STRAIGHT,f->stamp);
    }else if(f->stamp-s->stamp>BR2_LOOP_TO_MS)enter(s,BR2_DONE,f->stamp);
    break;
   case BR2_STRAIGHT:
    if(f->stamp-s->stamp>=BR2_STRAIGHT_MS&&(!mo||f->stamp>=s->hold_until))
     enter(s,BR2_TAIL,f->stamp);
    break;
   case BR2_TAIL:
    if(confirm(s,ex,f->stamp))enter(s,BR2_DONE,f->stamp);
    else if(f->stamp-s->stamp>BR2_TAIL_TO_MS)enter(s,BR2_DONE,f->stamp);
    break;
   default:break;
  }
 }
 /* 注入（每次调用都执行，与是否新帧无关） */
 if(s->phase==BR2_TURN){
  f->mid=94+BR2_TURN_BIAS;f->far_mid=f->mid;
  f->valid=1;f->degraded=1;f->all_white=0;f->measured=20;f->confidence=45;
  f->special_route=1;f->special_fault=0;
 }else if(s->phase==BR2_STRAIGHT){
  f->mid=94;f->far_mid=94;
  f->valid=1;f->degraded=1;f->all_white=0;f->measured=20;f->confidence=45;
  f->special_route=1;f->special_fault=0;
 }else if(s->phase==BR2_LOOP){
  /* v141 左转协调：轻微左转放行；大左转（mid<BR2_LEFT_SOFT_MID）改为右转 */
  if(f->mid>=0&&f->mid<(int)BR2_LEFT_SOFT_MID)f->mid=(int)(94u+BR2_LEFT_FIX_BIAS);
  if(f->far_mid>=0&&f->far_mid<(int)BR2_LEFT_SOFT_MID)f->far_mid=(int)(94u+BR2_LEFT_FIX_BIAS);
  f->special_route=1;f->special_fault=0;
 }else if(s->phase==BR2_TAIL){
  /* v141 左转协调（出环口同规则）：防"视觉锁左侧线→大左转→往回跑" */
  if(f->mid>=0&&f->mid<(int)BR2_LEFT_SOFT_MID)f->mid=(int)(94u+BR2_LEFT_FIX_BIAS);
  if(f->far_mid>=0&&f->far_mid<(int)BR2_LEFT_SOFT_MID)f->far_mid=(int)(94u+BR2_LEFT_FIX_BIAS);
 }
}
