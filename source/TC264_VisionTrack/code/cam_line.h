/*
 * cam_line.h
 * 2D 摄像头巡线核心：逐行白段 + 中线跟踪 + 环岛特征
 *
 * 赛道：蓝底 + 白色跑道 + 黑色胶带边界。
 * 思路（与 1D CCD 的本质区别）：
 *   - 保存【整幅】的行信息，而不是一条扫描线；
 *   - 从底部（近处）向上（远处）逐行跟踪中线，利用"行间连续性"锁定跑道；
 *   - 环岛/岔路等几何特征用【多行组合】判定，而不是单点。
 *
 * 阶段：v001 只测量（不参与控制）+ 屏幕可视化；数值确认后再接控制。
 */
#ifndef CODE_CAM_LINE_H_
#define CODE_CAM_LINE_H_

#include "zf_common_headfile.h"

#define CAM_MAX_SEG   8          /* 每行最多记录的白段数 */

/* 一行里的一个白色段：[left, right]，宽度 width */
typedef struct {
    int16 left;
    int16 right;
    int16 width;
} CamSegT;

/* 一行的扫描结果 */
typedef struct {
    uint8   n;                    /* 该行白段数量 */
    int16   white_total;          /* 该行白像素总数 */
    int16   dark_max;             /* 该行最长连续暗段长度（判环心用） */
    CamSegT seg[CAM_MAX_SEG];
} CamRowT;

/* ---- 全局（每帧更新） ---- */
extern CamRowT cam_row[CAM_H];        /* 每行的段信息 */
extern int16   cam_mid[CAM_H];        /* 跟踪得到的每行中线（-1=无效）*/
extern int16   cam_mid_near;          /* 近处中线（车正前方）*/
extern int16   cam_mid_far;           /* 远处中线（前瞻）*/
extern int16   cam_dev_near;          /* 近处偏差（千分比，±1000）*/
extern int16   cam_dev_far;           /* 远处偏差（千分比）*/
extern uint8   cam_mid_rows;          /* 有效中线的行数（可信度）*/

/* ---- 环岛特征（只测量，供显示与后续控制） ---- */
extern uint8   cam_ring_hole;         /* 1=检测到"环心"（中上部白夹暗，两侧都有白）*/
extern int16   cam_ring_hole_y;       /* 环心所在行 */
extern uint8   cam_ring_wide;         /* 1=有行出现"整行大白"（环口/宽区）*/
extern int16   cam_ring_wide_y;       /* 该行行号 */
extern volatile short cam_ring_cl;    /* ★v095：环口指纹·左横断率（0..1000）★ */
extern volatile short cam_ring_cr;    /* ★v095：环口指纹·右横断率（0..1000）★ */

void cam_line_scan(void);             /* 逐行找白段 */
void cam_line_track(void);            /* 逐行跟踪中线 */
void cam_line_ref(void);              /* 计算近/远偏差 */
void cam_line_ring(void);             /* 环岛特征检测（只测量）*/
void cam_line_all(void);              /* 上面四步一次做完 */

#endif /* CODE_CAM_LINE_H_ */
