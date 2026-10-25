#ifndef TRACK_CORE_H
#define TRACK_CORE_H
#include <stdint.h>
#define VT_W 188
#define VT_H 120
/* 短暂丢线容忍（转弯略微出道不再立即停车）：连续 VT_LOST_FRAMES 帧都找不出
 * 有效道路才判 VT_LOST 停车；期间保持上一帧舵量、双轮降到 VT_LOST_CRAWL
 * 目标继续找线，线一回到视野就无缝恢复正常控制。
 * 20ms/控制拍，25 拍 ≈ 500ms（v119: 10->25，200ms 停下太快的反馈修正）。VT_LOST_CRAWL 是速度 PI 的目标值，输出约为其 3 倍。 */
#define VT_LOST_FRAMES 25u
#define VT_LOST_CRAWL  150
/* 弯道降级控制：急弯时远端/中部的道路跑出画面，只剩画面下方（y>=90）的近处线，
 * 此时 valid 条件里的 wsum>0 不成立会被误判丢线停车。只要近处还有 VT_NEAR_ROWS
 * 行线，就改用近处中心继续巡线，但把速度目标压到 VT_DEGRADE_SPEED 以内。 */
#define VT_NEAR_ROWS     6u
#define VT_DEGRADE_SPEED 320
/* ⚠️ v104 起 `VT_NEAR_ROWS` / `VT_WIDE_FILL_MAX` **已不再参与逻辑**：
 * "整行全白补线"与"近处单段降级"两条老路已被 vt_analyze 的模型插值取代，
 * 保留定义只为兼容历史文档与测试注释。`VT_DEGRADE_SPEED` 仍是降级限速，继续有效。 */
#define VT_WIDE_FILL_MAX 50u
/* 路口/整帧全白 的补线只能用"惯性"顶一会儿。车真冲出了赛道（白茫茫一片）时
 * 必须停下来，所以这里给"全程只靠补线"的帧数上限：100 帧 ≈ 2s。
 * 正常穿过一个十字/环岛入口只要几百 ms，够用；跑出赛道 2s 后必定 VT_LOST。 */
#define VT_WHITE_MAX_FRAMES 100u

/* ---- v105 中线提取：左右边线分离 + 双边界连续性 ------------------------------
 * 实车症状（用户口述）：「跑起来很严重，蓝区也会识别出中线，然后乱跑」。
 * 两个根因（都有真实帧证据，见 docs/v105_视觉升级_蓝白赛道_20261009.md）：
 *
 * (1) 二值化——蓝白赛道是"黑胶带 / 蓝底 / 白路"三峰直方图。全局大津法只做一次
 *     二分，当"黑胶带 vs 其余"的类间方差更大时，它会把**蓝底和白路一起判成白**
 *     （真实帧 tmp/frame_gray.bin：一级 Otsu=169，而蓝底灰度 205~220、白路 248~255，
 *     阈值落在蓝底之下 → y=90 那一行的"白段"是 (11,186,176)，横跨整幅宽度，
 *     其中大半是蓝底）。中线自然落到蓝区上。**这就是 v103"路口中线不对齐"的同一个
 *     根因**，v104 的"贴边行不测量"只是绕开了症状。
 *     → 二值化改**两级递归大津法**：一级分"暗/亮"；若亮类占比过高（说明一级只把
 *       黑胶带分出去了），就对亮类再跑一次 Otsu 得到"蓝底|白路"的分界。
 *       实现见 cam_image.c:cam_otsu_two_level()。
 *
 * (2) 中线——旧算法逐行挑"离上一中心最近的那一条白段"，只用**一个中点**做连续性
 *     闸门（|Δ|>35 就丢行）。路口/岔口时上下可信行可能落在**不同分支**，第 2 遍
 *     插值出来的中线就会横跨中间那条蓝缝（历史 v006/v021 反复出现的"横跨蓝区抄近道"）。
 *     → v105 改为**左右边线各自跟踪**（对齐国赛 get_left/get_right、逐行求左右边线
 *       的做法）：先从画面最下方（车头正前方，必有路）取"种子行"，
 *       再向上逐行要求**左端与右端同时**落在上一行的窗口内；中线 = (L+R)/2。
 *       这样中线在结构上就不可能跨过中间的暗缝跳到另一段白上。
 *
 * 第 3 遍仍保留 v104 的"可信测量 + 模型插值"：续不上的行用最近可信行插值/外推。
 * 由于第 2 遍已经用双边界连续性保证了所有可信行都在**同一分支**上，插值是安全的。
 *
 * (3) 单边贴边补线（对齐国赛"单边丢线用另一边 + 学习路宽补线"，
 *     HammerTa IMG_DEAL.c 的 TrackWild[row]/2、CQU findline.c 的补线思路）：
 *     急弯时白路会跑出画面左右边缘，贴边那一侧的边界根本不可见，旧做法是
 *     "整行丢掉 → 该行没有测量值 → 只能冻住上一帧"（v104 冻结中线的老毛病）。
 *     v105 起：只要**可见的那一侧**仍然连续，就用 `t->width[y]`（逐行学习到的路宽）
 *     把不可见的一侧补出来，中线 = 可见边 ∓ 路宽/2。这类行记 VT_ROW_CLIPL/CLIPR，
 *     计入 measured 与 clip_rows，但**不参与**宽度学习与环岛用的角点/直道统计
 *     （那两处必须只用真实双边界，免得把推断出来的边当成事实）。 */
#define VT_ROW_TOP       28       /* 分析区间上界（远处） */
#define VT_ROW_BOT       110      /* 分析区间下界（近处） */
#define VT_SEG_MIN       12       /* 白段最小可信宽度，短于此忽略 */
#define VT_SEG_MAX       150      /* 白段最大可信宽度，超过视为"整片铺满" */
#define VT_FULL_W        (VT_W-2) /* 整行全白判据 */
#define VT_SEG_K         4u       /* 每行最多记录几个白段（岔口行） */
#define VT_EDGE_WIN      12       /* 相邻行左/右边界允许的最大位移（px） */
#define VT_WID_LO        40       /* 白段宽度合理区间（相对学习宽度，百分比） */
#define VT_WID_HI        250
#define VT_SEED_ROWS     14u      /* 种子只从最下方这么多行里找 */
#define VT_MODEL_DEGRADE 24u
/* v122 (CQU-style): yaw-rate damping injected into the steering error.
 * gyro = filtered gyro_z (bias removed). Convention taken here: positive
 * gyro = CCW/left rotation. While the car already rotates left the left
 * demand is reduced -> less overshoot / wobble. Small, hard-capped;
 * flip VT_GYRO_DAMP_SIGN if the on-car rotation check disagrees. */
#define VT_GYRO_DAMP_K    (1.0f/30000.0f)
#define VT_GYRO_DAMP_MAX  0.06f
#define VT_GYRO_DAMP_SIGN (+1.0f)      /* 模型补出的行数 >= 它 → degraded（过路口限速） */

/* ---- v106 十字路口：不许拿"唯一幸存的贴边小白斑"当真路面 --------------------
 * 实车症状：「十字路口黄线一直在屏幕左边和中线两个地方闪」。
 * 真实路口帧 tmp/frame_gray_v102_crossing.bin（77% 白）实测：
 *   83 行里 82 行是 VT_ROW_FULL（整行铺满），只剩 y=106 一个 **0..15 的贴左边缘
 *   小白斑**；它被当成种子 → 中心 x=7 → measured=1、其余 82 行全从 x=7 插值
 *   → 整条中线画在屏幕最左；下一帧小白斑位置一变就是 x≈93 → 逐帧闪烁。
 * 判据（几何无关，只看"横向白带有没有把画面铺满"）：
 *   一帧里 FULL 行 >= VT_FULL_MIN_ROWS 且可信测量行 < VT_OK_MIN
 *   → 判定"正在穿路口"，用上一帧中心直着顶过去（degraded 限速），
 *     而不是相信那几行来路不明的白斑。
 * 注意：急弯不会误触——急弯每行只有一条窄白带，其余是暗的，永远凑不出 FULL 行。 */
#define VT_FULL_MIN_ROWS 24u      /* FULL 行数门槛（分析带共 83 行，≈29%） */
#define VT_OK_MIN        8u       /* 可信测量行下限；低于它 + FULL 行多 → 判路口 */

/* ⚠️ v105 起 VT_CONT_MAX / VT_WIDE_FILL_MAX 不再参与逻辑
 * （单点连续性闸门被"双边界连续性"取代），保留定义只为兼容历史文档与测试注释。 */
#define VT_CONT_MAX      35
/* VT_ROW_LINE（v107）：既不是实测也不是贴边补线，而是"拉线补全"推出来的行
 * （国赛 connect_line 那一招：用远端锚点与近端参考两点连直线铺点）。
 * 计入 solved 但不计入 measured —— 它没有真实边界，只是几何外推。 */
enum {VT_ROW_NONE,VT_ROW_OK,VT_ROW_CLIPL,VT_ROW_CLIPR,VT_ROW_LINE,VT_ROW_FULL,VT_ROW_MULTI};
enum {VT_NORMAL,VT_RING_BEGIN,VT_RING_IN,VT_RING_RUN,VT_RING_OUT,VT_RING_END,VT_CROSS};
enum {VT_OK,VT_MANUAL,VT_CAMERA,VT_LOST,VT_MAIN,VT_STALL,VT_RING_TIMEOUT,VT_IMU,VT_TILT,VT_ENCODER,VT_FINISH};
typedef struct {
 uint32_t sequence,stamp,valid,confidence,rows,near_rows;
 int32_t mid,far_mid,left,right,width;
 uint32_t open_left,open_right,corner_left,corner_right,straight_left,straight_right;
 uint32_t fork,hole,finish;
 uint32_t degraded;   /* 1 = 远端无路但近处有线，走降级控制 */
 uint32_t all_white;  /* 1 = 整帧没找到任何真实边界，中线全靠惯性补出来 */
 uint32_t model_rows; /* v104：本轮有多少行是"模型插值补出来的"（非可信测量） */
 uint32_t measured;   /* v104：本轮可信测量行数（左右边界都在画面内的行） */
 uint32_t special_route; /* Independent, explicitly enabled large-ring route. */
 uint32_t special_fault; /* Explicit large-ring fault; 0 means no fault. */
 uint32_t clip_rows;  /* v105：其中"单边贴边补线"的行数（白段被画面裁掉、用学习路宽补出另一边） */
} VTFrame;
typedef struct {
 int16_t left[VT_H],right[VT_H],mid[VT_H],width[VT_H];
 int32_t previous_mid,previous_left,previous_right;
 uint32_t initialized,edge_mode;
} VTTracker;
typedef struct {
 uint32_t active,fault,now,start_ms,phase,phase_ms,last_seq,good,phase_count,cool_until;
 uint32_t seen_missing,normal_frames,finish_frames,finish_armed,ring_completed,lost_frames,white_frames;
 uint32_t blind_frames;  /* v143：连续无有效线帧计数（含 all_white），≥2 触发盲区降速 */
 uint32_t stall_ms[2],pwm[2],speed[2],target[2],updates,steering_initialized;
 uint32_t steering_stamp,reliable_stamp,reliable_seen,blind_distance,speed_target_state;
 int32_t reliable_steering;
 int32_t ring_direction,ring_yaw0,steering;
 float integral[2],previous_error,derivative,filtered_error;
} VTControl;
void vt_tracker_reset(VTTracker *t);
void vt_analyze(VTTracker *t,const uint8_t *bin,uint32_t contrast,uint32_t now,uint32_t sequence,VTFrame *v);
void vt_control_reset(VTControl *c,uint32_t now);
void vt_control_stop(VTControl *c,uint32_t fault);
void vt_control_start(VTControl *c,uint32_t now);
void vt_step(VTControl *c,const VTFrame *v,uint32_t now,uint32_t dt_ms,int32_t left,int32_t right,int32_t yaw10,int32_t gyro,uint32_t imu_valid,uint32_t main_age,uint32_t tipped,uint32_t scale,uint32_t gain_scale);
uint32_t vt_edge_mode(const VTControl *c);
#endif
