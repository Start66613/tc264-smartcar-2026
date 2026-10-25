#ifndef BIG_RING_H
#define BIG_RING_H
#include "track_core.h"
/* 大圆环 · 反向行驶重写版（br2, 2026-10-11；v134 稳健化）
 * 用户方案（主方案A）：
 *   点3特征 → 武装(ARMED)
 *   点2特征(第1次) → 右转进环 + count_mark2=1 (TURN)
 *   环内正常巡线 (LOOP，不注入)
 *   再遇点2特征且count_mark2==1 → 保持直行 (STRAIGHT)
 *   点1特征 → 退出 (TAIL→DONE)；本次起跑内不再参与
 * v134: 检测器稳健化——标定自三帧+30个平移/亮度变体+8个负样本（scripts/fixtures_bigring_rev），
 *       容忍 ±20~30px 横向 / ±8行 纵向 的位置偏差（旧版±5px即失效，实车首次测试未触发）。
 */
/* v141（2026-10-11，用户方案；从 v134 重新起底，替代 v135"完全禁止左转"）：
 *   ① 左转协调——环内(LOOP/TAIL)允许轻微左转（mid>=BR2_LEFT_SOFT_MID 放行）；
 *      大左转（mid<BR2_LEFT_SOFT_MID）改为右转（mid=94+BR2_LEFT_FIX_BIAS）。
 *      依据：160帧实录 f104-119 满左+10.0°持续约1s（视觉锁左侧线）；用户："大左转时让他右转"。
 *   ② 8 秒强制关闭保险（同 v138）——车发动（首个 running 帧）后 8s 内模块必须关闭（DONE），到点未关则强制 DONE。
 *   改动全部限于本模块（big_ring.c/h）；track_core / track_vision 等环岛外逻辑零改动。
 */
/* v141 参数 */
#define BR2_LEFT_SOFT_MID  80u   /* 允许的最左中线（含）：mid>=80 的轻微左转放行；<80 = 大左转 → 右转 */
#define BR2_LEFT_FIX_BIAS  24u   /* 大左转改为右转的注入偏置：mid = 94+24 = 118 */
#define BR2_GUARD_MS     8000u   /* 车发动（首个 running 帧）后 8s 内未 DONE → 强制关闭（用户指定起点） */
enum {BR2_OFF,BR2_ARMED,BR2_TURN,BR2_LOOP,BR2_STRAIGHT,BR2_TAIL,BR2_DONE};
typedef struct {
 uint32_t phase,seq,hits,last_hit,count_mark2,stamp,hold_until,entered_ms;
 uint32_t dbg;   /* v134 诊断：bit0 arm命中 bit1 mouth命中 bit2 exit命中（原始检测器输出，每帧刷新） */
 uint32_t guard_run,guard_start,guard_closed;  /* v141 保险：跑动计时起点/已强制关闭标志 */
} BR2State;
void br2_reset(BR2State *s);
void br2_process(BR2State *s,VTTracker *t,const uint8_t *bin,VTFrame *f,
 uint32_t enabled,uint32_t running,uint32_t imu_valid,int32_t yaw10);
/* 三个特征检测器（供 host 测试直接调用） */
int br2_detect_arm(const uint8_t *bin);    /* 点3：武装 */
int br2_detect_mouth(const uint8_t *bin);  /* 点2：进环口/出环口 */
int br2_detect_exit(const uint8_t *bin);   /* 点1：退出 */
#endif
