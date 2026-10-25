/*

 * cam_image.c

 * 2D 摄像头图像处理实现

 *

 * 设计参照 Jason_Image.c：

 *   1) image_copy：把库缓冲区 mt9v03x_image 复制到自己的数组，避免处理中途被新帧覆盖

 *   2) 全图大津法（Ostu）求阈值

 *   3) 二值化：> 阈值为白（v077 修正）（跑道），否则黑（蓝底/黑线）

 *   4) 逐行找左右边线（本工程为"白跑道"场景，故取最左/最右白点）

 */

#include "cam_image.h"
#include "track_adapter.h"



/* ★v066：大数组（cam_gray/cam_bin/cam_row 共约 51KB）放 CPU1 的 DSRAM★

 * CPU0 的 DSRAM 只有 72KB 且已被旧业务占满（链接报 108K 缺 RAM）；

 * CPU1 的 DSRAM 有 120KB，且旧工程的 image[] 本来就放在 0x60000000 段。 */

#pragma section all "cpu1_dsram"



IFX_ALIGN(4) uint8 cam_gray[CAM_H][CAM_W];

uint8  cam_bin[CAM_H][CAM_W];

uint8  cam_thr = 128;

uint8  cam_gmin = 255, cam_gmax = 0, cam_gavg = 0;

uint16 cam_frame_cnt = 0;

uint16 cam_bin_white = 0;


/* ★v105：两级递归大津法的诊断量 */
uint8  cam_thr1 = 128;      /* 一级（传统）大津阈值 */
uint8  cam_otsu_level = 1;  /* 1=一级已分好；2=做了二级（蓝底|白路） */
uint8  cam_bin_suspect = 0; /* 1=二值后白占比仍然离谱（很可能整帧不是路） */



int16  cam_edge_l[CAM_H];

int16  cam_edge_r[CAM_H];

int16  cam_center[CAM_H];

uint8  cam_line_rows = 0;



static uint32 cam_hist[256];



/* ---------------- 大津法（参考 Jason 的 otsuThreshold） ---------------- */

uint8 cam_ostu(const uint8 *img, uint32 n)

{

    uint32 i;

    uint32 total = n;

    uint32 sum_all = 0, sum_b = 0, w_b = 0;

    float  v_max = 0.0f;

    uint8  thr = 128;



    for (i = 0; i < 256u; i++) cam_hist[i] = 0;

    for (i = 0; i < n; i++) cam_hist[img[i]]++;



    for (i = 0; i < 256u; i++) sum_all += (uint32)i * cam_hist[i];



    for (i = 0; i < 256u; i++)

    {

        float u0, u1, v;

        uint32 w_f;

        w_b += cam_hist[i];

        if (w_b == 0u) continue;

        w_f = total - w_b;

        if (w_f == 0u) break;

        sum_b += (uint32)i * cam_hist[i];

        u0 = (float)sum_b / (float)w_b;

        u1 = ((float)sum_all - (float)sum_b) / (float)w_f;

        v  = (float)w_b * (float)w_f * (u0 - u1) * (u0 - u1);

        if (v > v_max) { v_max = v; thr = (uint8)i; }

    }

    return thr;

}


/* ==================== ★v105：两级递归大津法 ====================
 * 为什么需要它（真实帧证据，见 docs/v105_视觉升级_蓝白赛道_20261009.md）：
 *   本车赛道是「蓝底 + 白色跑道 + 黑胶带边界」，灰度直方图是**三峰**：
 *     黑胶带 ≈ 40~100，蓝底 ≈ 190~220，白路 ≈ 248~255。
 *   大津法只做一次二分。当"黑胶带 vs 其余"的类间方差更大时（蓝底和白路靠得近、
 *   黑胶带离得远），它会取 thr≈40~169，把**蓝底和白路一起判成白**：
 *     实测 tmp/frame_gray.bin 一级 Otsu=169，而 y=90 行的"白段"是 (11,186,176)
 *     ——横跨整幅宽度、其中大半是蓝底。中线因此落到蓝区上（实车"蓝区也识别出中线
 *     然后乱跑"）。这同时是 v103"路口中线不对齐"的根因，v104 只是绕开了症状。
 *
 * 做法：一级 Otsu 先分"暗/亮"；若亮类占比过高（>62%，说明一级只把黑胶带分出去），
 *       就对**亮类**再跑一次 Otsu，得到"蓝底 | 白路"的分界。
 * 兜底：亮类内部灰度跨度 <12（没有双峰）→ 退回一级阈值；
 *       二级结果必须比一级高出 >10 才采用。
 * 参考：国赛队伍普遍用"大津法 + 偏置/形态学"（NEUQ TS_OTSU_with_bias、
 *       CQU Fast_otsuThreshold+image_filter）；本实现按本车三峰场景推导，
 *       不照搬任何外来阈值/偏置系数。 */
#define OTS_L1_GUARD_PCT 62u   /* 一级亮类占比超过它(%) → 一级只分出了黑胶带 */
#define OTS_L1_MIN_SEP   10u   /* 二级阈值至少要比一级高这么多才有意义 */
#define OTS_L2_MIN_RANGE 12u   /* 亮类内部灰度跨度小于它 → 不是真的双峰 */

uint8 cam_otsu_two_level(const uint8 *img, uint32 n)

{

    uint32 i;

    uint32 hist2[256];

    uint32 n_hi = 0, tot_hi = 0, gmin_hi = 255, gmax_hi = 0;

    uint8  t1 = cam_ostu(img, n);



    cam_thr1 = t1;

    cam_otsu_level = 1;

    if (n == 0u) return t1;

    for (i = 0; i < n; i++) if (img[i] > t1) n_hi++;

    if (n_hi * 100u <= n * (uint32)OTS_L1_GUARD_PCT) return t1;   /* 一级已经分好了 */



    for (i = 0; i < 256u; i++) hist2[i] = 0;

    for (i = 0; i < n; i++) {

        uint8 g = img[i];

        if (g > t1) {

            hist2[g]++; tot_hi++;

            if (g < gmin_hi) gmin_hi = g;

            if (g > gmax_hi) gmax_hi = g;

        }

    }

    if (gmax_hi - gmin_hi < (uint32)OTS_L2_MIN_RANGE) return t1;  /* 亮类内部没有双峰 */

    {

        uint32 sum_all = 0, sum_b = 0, w_b = 0;

        float  v_max = 0.0f;

        uint8  t2 = t1;

        for (i = 0; i < 256u; i++) sum_all += (uint32)i * hist2[i];

        for (i = 0; i < 256u; i++) {

            float u0, u1, v; uint32 w_f;

            w_b += hist2[i];

            if (w_b == 0u) continue;

            w_f = tot_hi - w_b;

            if (w_f == 0u) break;

            sum_b += (uint32)i * hist2[i];

            u0 = (float)sum_b / (float)w_b;

            u1 = ((float)sum_all - (float)sum_b) / (float)w_f;

            v  = (float)w_b * (float)w_f * (u0 - u1) * (u0 - u1);

            if (v > v_max) { v_max = v; t2 = (uint8)i; }

        }

        if ((uint32)t2 > (uint32)t1 + (uint32)OTS_L1_MIN_SEP) {

            cam_otsu_level = 2;

            return t2;

        }

    }

    return t1;

}



/* ================== v079：帧快照（防撕裂） ==================
 * 详见 cam_image.h 顶部说明。协议：
 *   ISR 侧  cam_frame_snap()   : busy? 跳过(记 drop) : 复制 -> g_snap_seq++
 *   主循环  cam_take_snapshot(): 临界区内查新序号并置 busy；临界区外复制；清 busy
 * 单核 + 临界区保证原子；ISR 复制期间不可被抢占（中断优先级 60>43）。
 * ============================================================ */
#define SNAP_WORDS   ((uint32)CAM_H * (uint32)CAM_W / 4u)     /* 22560/4 = 5640 */

#pragma section all "cpu1_dsram"
IFX_ALIGN(4) uint8 cam_isr_buf[CAM_H][CAM_W];
#pragma section all restore

#pragma section all "cpu0_dsram"
volatile unsigned g_snap_stamp=0,cam_taken_stamp=0;
volatile unsigned g_snap_seq  = 0;     /* 干净帧发布数（cpu0 段，DAS 可读） */
volatile unsigned g_snap_drop = 0;     /* 消费者忙时跳过的快照数（诊断） */
static volatile int g_snap_busy = 0;   /* 1 = 主循环正在复制 cam_isr_buf */
static unsigned cam_taken = 0;         /* 主循环已取到的快照序号 */
#pragma section all restore

static void snap_copy32(uint8 *dst, const uint8 *src)
{
    uint32 i;
    uint32 *d = (uint32 *)dst;
    const uint32 *s = (const uint32 *)src;
    for (i = 0; i < SNAP_WORDS; i++) { d[i] = s[i]; }
}

/* 在 DMA 完成中断内调用：把完整帧复制到快照缓冲 */
void cam_frame_snap(void)
{
    if (g_snap_busy) { g_snap_drop++; return; }    /* 主循环正在复制，跳过本帧 */
    snap_copy32(&cam_isr_buf[0][0], &mt9v03x_image[0][0]);
    g_snap_stamp=TrackNow();
    g_snap_seq++;
}

/* 主循环取帧：有新快照则复制到 dst（返回 1），否则返回 0 */
int cam_take_snapshot(uint8 (*dst)[CAM_W])
{
    uint32 pm;
    unsigned seq,stamp;

    pm = interrupt_global_disable();
    seq = g_snap_seq;stamp=g_snap_stamp;
    if (seq == cam_taken) { interrupt_global_enable(pm); return 0; }
    g_snap_busy = 1;
    interrupt_global_enable(pm);

    snap_copy32(&dst[0][0], &cam_isr_buf[0][0]);

    cam_taken = seq;cam_taken_stamp=stamp;
    g_snap_busy = 0;
    return 1;
}

/* ---------------- 一帧处理 ---------------- */

void cam_process(void)

{

    uint32 i, j;

    uint32 sum = 0, white = 0;

    uint8  lo = 255, hi = 0;



    /* 1) 统计（v079：帧已由 cam_take_snapshot 复制进 cam_gray，此处不再读 DMA 缓冲） */

    for (i = 0; i < CAM_H; i++)

    {

        for (j = 0; j < CAM_W; j++)

        {

            uint8 v = cam_gray[i][j];


            sum += v;

            if (i >= 28u && v < lo) lo = v;

            if (i >= 28u && v > hi) hi = v;

        }

    }

    cam_gmin = lo;

    cam_gmax = hi;

    cam_gavg = (uint8)(sum / (uint32)(CAM_W * CAM_H));



    /* 2) 大津法（整幅统计）。★v105：改用两级递归大津法，抗"蓝底被判成白" */

    cam_thr = cam_otsu_two_level(&cam_gray[28][0], (uint32)CAM_W * (CAM_H-28u));



    /* 3) 二值化 + 数白像素 */

    for (i = 0; i < CAM_H; i++)

    {

        for (j = 0; j < CAM_W; j++)

        {

            if (cam_gray[i][j] > cam_thr) { cam_bin[i][j] = 255; white++; }   /* v077: >= -> > */

            else                           { cam_bin[i][j] = 0; }

        }

    }

    cam_bin_white = (uint16)white;

    /* v105：二值后白占比仍然离谱（>72%）→ 很可能整帧不是路（冲出赛道/对白墙/
     * 对空）。标出来给屏幕/诊断看；下游 vt_analyze 找不到种子会走 all_white 分支。 */
    cam_bin_suspect = (white * 100u > (uint32)CAM_W * (uint32)CAM_H * 72u) ? 1u : 0u;

    cam_frame_cnt++;

}



/* ---------------- 逐行边线（诊断/后续巡线） ---------------- */

void cam_trace_edges(void)

{

    uint32 i, j;

    uint8  rows = 0;



    for (i = 0; i < CAM_H; i++)

    {

        int16 l = -1, r = -1;

        /* 从中间向两侧找，能更好处理"两侧都是边界"的情况 */

        for (j = 0; j < CAM_W; j++)

        {

            if (cam_bin[i][j] == 255) { l = (int16)j; break; }

        }

        for (j = CAM_W; j > 0u; j--)

        {

            if (cam_bin[i][j - 1u] == 255) { r = (int16)(j - 1u); break; }

        }

        cam_edge_l[i] = l;

        cam_edge_r[i] = r;

        if (l >= 0 && r >= 0)

        {

            cam_center[i] = (int16)((l + r) / 2);

            rows++;

        }

        else

        {

            cam_center[i] = -1;     /* 该行没同时找到两边 */

        }

    }

    cam_line_rows = rows;

}

