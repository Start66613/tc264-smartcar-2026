/*
 * cam_image.h
 * 2D 摄像头（MT9V03X 188x120）图像处理
 *
 * 参考：20届双车跟随（Jason）的 Jason_Image.c 结构
 *   - 原始图像 -> 副本 -> 大津法阈值 -> 二值化 -> 边线提取
 * 差异：Jason 跑在 TC377 三核上（图像处理放 CPU1）；本工程在 TC264 上先放 CPU0
 *       主循环，等验证稳定后再考虑迁到 CPU1。
 */
#ifndef CODE_CAM_IMAGE_H_
#define CODE_CAM_IMAGE_H_

#include "zf_common_headfile.h"

/* 图像尺寸 CAM_W/CAM_H 在 zf_common_headfile.h 中统一定义（188x120）*/

/* ---- 全局数据（供显示/诊断/后续巡线使用） ---- */
extern uint8  cam_gray[CAM_H][CAM_W];    /* 灰度副本（避免处理时被新帧覆盖） */
extern uint8  cam_bin[CAM_H][CAM_W];     /* 二值化：255=白(跑道)  0=黑(边界/蓝底) */
extern uint8  cam_thr;                   /* 大津法阈值（v105 起 = cam_otsu_two_level 的结果） */
extern uint8  cam_thr1;                  /* v105：一级（传统）大津阈值，诊断用 */
extern uint8  cam_otsu_level;            /* v105：1=一级即分好；2=做了二级（蓝底|白路） */
extern uint8  cam_bin_suspect;           /* v105：1=二值后白占比仍>72%（很可能整帧不是路） */
extern uint8  cam_gmin, cam_gmax;        /* 灰度极值（曝光诊断） */
extern uint8  cam_gavg;                  /* 平均灰度 */
extern uint16 cam_frame_cnt;             /* 已处理帧数 */
extern uint16 cam_bin_white;             /* 白像素总数（判"整幅白"用） */

/* 逐行边线：每一行的最左/最右白像素、以及中点 */
extern int16  cam_edge_l[CAM_H];
extern int16  cam_edge_r[CAM_H];
extern int16  cam_center[CAM_H];
extern uint8  cam_line_rows;             /* 找到有效边线的行数 */

/* **********************************************************************
 * v079：帧快照系统（修复"画面重叠/撕裂"）
 *
 * 背景：DMA 几乎整帧都在写 mt9v03x_image（1 像素 1 个 DMA 请求）。
 *   主循环任何时刻去读这幅缓冲，都可能读到"上调半旧、下段半新"的混合帧
 *   （= 用户看到的画面重叠/接缝，中线随之乱跳、舵机乱甩）。
 *
 * 修复（三层保险）：
 *   1) 采集侧：DMA 完成中断内（此刻缓冲完整、下一帧尚未开始）立即复制到
 *      cam_isr_buf —— 复制期间不会被打断（DMA 中断优先级 60 > VSYNC 43，
 *      TC264 默认中断不嵌套）。
 *   2) 消费侧：主循环用 cam_take_snapshot() 把快照复制进自己的 cam_gray，
 *      之后所有处理（Ostu/二值/中线）只读这份私有副本，永不被覆盖。
 *   3) 竞态保护：g_snap_busy 标志 + 临界区，中断与主循环不会同时写同一缓冲。
 * ********************************************************************** */
extern uint8 cam_isr_buf[CAM_H][CAM_W];      /* ISR 快照缓冲（只被中断写） */
extern volatile unsigned g_snap_seq;         /* 已发布的干净帧序号（ISR 递增） */
extern volatile unsigned g_snap_drop;        /* 因消费者占用被跳过的快照数（诊断） */
void cam_frame_snap(void);                   /* 在 DMA 完成 ISR 内调用 */
int  cam_take_snapshot(uint8 (*dst)[CAM_W]); /* 主循环：有新干净帧时复制到 dst */

void cam_process(void);      /* 一帧：统计 + 大津法 + 二值化（v079：帧已由 cam_take_snapshot 就绪） */
void cam_trace_edges(void);  /* 逐行扫描左右边线与中点 */
uint8 cam_ostu(const uint8 *img, uint32 n);
uint8 cam_otsu_two_level(const uint8 *img, uint32 n);  /* ★v105 两级递归大津法 */

#endif /* CODE_CAM_IMAGE_H_ */
