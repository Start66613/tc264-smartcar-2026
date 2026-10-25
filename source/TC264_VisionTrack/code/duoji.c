#include "zf_common_headfile.h"
/* P33.9, 50 Hz. raw duty 0..10000 spans the 20 ms period.
 * 1500 us -> 750; verified pulse range 1000..2000 us -> 500..1000. */
void servo_init(void)
{
    pwm_init(ATOM0_CH1_P33_9, 50, 740);
}
/* ===== 舵机行程与缓变控制 =====
 * P33.9, 50Hz；raw duty 0..10000 = 20ms，故 1us = 0.5 raw；中位 750 = 1500us。
 *
 * 实车发现：舵机大角度转弯时会被车模底座卡住（卡住 = 前轮转不到位 → 转向不足
 * 冲出赛道，而且堵转发热）。所以：①两侧行程分别限幅（哪边卡就减哪边）；
 * ②加变化率限制，避免一帧猛打到极限造成“撞死”。
 *
 * 调法（用屏幕上的 S:xxxx 值或推前轮感受）：
 *   若某侧仍被卡 → 把对应 *_US 调小（每次减 10）。
 *   若转向不够  → 在“不卡”的前提下尽量调大。
 * 当前 160us ≈ 1340~1660us。 */
#define SERVO_CENTER_RAW   740
/* ★v039：行程 160 → 200us★
 * 原因：转向链实测早已在 |steering_output|>=10 时打满舵，也就是说"打满"的
 *       上限完全由这里决定。而 1500±160us 只用了普通舵机全行程(±400~500us)的
 *       约 1/3 → 前轮最大转角太小 → 转弯甩不进去、略出道。
 *       200us = 1300~1700us（仍在舵机安全区内），先加 25% 看效果。
 * 注意：若前轮在满舵时顶到车模底座（堵转、发烫），必须先把机械干涉处理掉，
 *       否则加行程只会让舵机堵转更严重。两侧可分别调（哪边不够加哪边）。 */
/* ★v042：按 TC264_ServoCheck 实测极限精确设置（原本两侧都写 160 是错的）★
 * 实测（脉宽 us）：右转极限 1320，左转极限 1640（中位 1500）。
 * 换算：raw = us/2，故 'L' 方向最多减 180us、'R' 方向最多加 140us。
 *   - 'L' 命令 = 车头向【右】转 → 脉宽变短 → 可用 180us
 *   - 'R' 命令 = 车头向【左】转 → 脉宽变长 → 只可用 140us
 * 两侧各留 10us 余量。注意：旧值 160 会让左转打到 1660us（>1640 极限）→ 一直顶底座。
 * 【机械建议】可用范围其实是以 1480us 为中心的 ±160us；若把舵机臂重新对中到
 *            1480us，就能左右各拿满 160us（左转立刻多 30us，转弯更咬得住）。 */
#define SERVO_TRAVEL_L_US  170      /* 'L'：脉宽 1500-170 → 1330us（极限1320，留10） */
#define SERVO_TRAVEL_R_US  130      /* 'R'：脉宽 1500+130 → 1630us（极限1640，留10） */
#define SERVO_SLEW_RAW     150      /* v123: restored 150 (user request, v117 slowdown reverted) */      /* ★v057：60 → 150（≈一帧到位）★
                                     * 用户反馈"舵机打得还是慢"。查国赛代码（Jason_PWM.c）：
                                     * 他们的舵机通道【没有变化率限幅】，只做 480~980 钳位 —— 给多少打多少。
                                     * 60/帧时满舵要 ~3 帧(40ms)，转弯明显滞后；150 后 1 帧内到位。
                                     * 保留限幅只是防"单帧突变"，不再拖慢响应。 */
/* 原注释如下（已放宽）：*/ /* ★v039：36 → 60（更快到位）★
                                     * 控制是每 CCD 帧一次(≈72Hz, 14ms/帧)。60 raw/帧 → 满舵
                                     * 200us 只需 ~1.3 帧(≈19ms)，基本"该到就到"，转弯不再拖;
                                     * 仍有限幅，保留"防一帧猛打撞死"的保护。 */
/* v038：18 → 36
                                     * 控制调用频率由 ~140Hz 降到 CCD 帧率 ~72Hz（见 cpu0_main），
                                     * 若仍写 18，舵机等效转速会减半、转向变迟钝；
                                     * 翻倍后等效转速与之前完全一致（18×140 ≈ 36×72）。 */

static int servo_cmd = SERVO_CENTER_RAW;   /* 当前实际输出（用于缓变） */

void servo_control(char direction, int duty)
{
    int target = SERVO_CENTER_RAW, off, delta;
    if (duty < 0) duty = 0;
    if (duty > 100) duty = 100;

    if (direction == 'L') { off = duty * (SERVO_TRAVEL_L_US / 2) / 100; target = SERVO_CENTER_RAW - off; if(target < 665) target = 665; }
    else if (direction == 'R') { off = duty * (SERVO_TRAVEL_R_US / 2) / 100; target = SERVO_CENTER_RAW + off; if(target > 815) target = 815; }

    /* 变化率限制：单次最多动 SERVO_SLEW_RAW，避免瞬间打到极限撞死 */
    delta = target - servo_cmd;
    if (delta >  SERVO_SLEW_RAW) delta =  SERVO_SLEW_RAW;
    if (delta < -SERVO_SLEW_RAW) delta = -SERVO_SLEW_RAW;
    servo_cmd += delta;
    pwm_set_duty(ATOM0_CH1_P33_9, servo_cmd);
}

/* 停止脉冲输出：舵机失去保持力，前轮可自由摆动，方便手动推车前进。 */
void servo_release(void)
{
    pwm_set_duty(ATOM0_CH1_P33_9, 0);
    servo_cmd = SERVO_CENTER_RAW;    /* 松开后重新从中位缓起 */
}
