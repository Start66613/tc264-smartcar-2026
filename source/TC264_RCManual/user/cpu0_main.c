/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu0_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.10.2
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"

/* ======================== TC264_RCManual（独立遥控车） ========================
 * 枪式遥控器（UART 接收机版）手动驾驶程序。与巡线固件完全独立，烧哪个用哪个。
 * 主循环无阻塞（无摄像头/图像/巡线），控制延迟远低于 1ms。
 *
 * 通道映射：
 *   CH1 方向摇杆  -> 转向
 *   CH2 油门扳机  -> 速度（前推前进；后拉先"滑行刹车"、再拉到底=倒车）
 *   转弯自动差速：内轮按转向量减速，外轮全速（过弯更顺）
 *   CH4 三段开关  -> 速度档基准（低 30% / 中 50% / 高 80% 占空比）
 *   CH6 按键      -> 加速（速度上限 +500/次，上限 9000）
 *   CH5 按键      -> 减速（速度上限 -500/次，下限 1200）
 * 安全：
 *   1) 上电软启动：必须先让扳机回中，才允许输出动力（防上电窜车）。
 *   2) 失联保护：500ms 无新帧或接收机 state=0 -> 立即停车 + 舵机回中。
 *
 * 引脚（与巡线车相同）：
 *   舵机   P33.9（50Hz，中位 750=1500us；右'L'极限1320us 左'R'极限1640us）
 *   左电机 PWM=ATOM1_CH7_P02_7  DIR=P02_6
 *   右电机 PWM=ATOM1_CH5_P02_5  DIR=P02_4（实测 DIR 高电平=前进）
 *   接收机 TX -> P10.6（UART2, 100k 8E2 SBUS）
 * ============================================================================ */

#define RC_MID          992         /* SBUS 通道中位 */
#define RC_THR_DEAD     50          /* 油门死区 */
#define RC_STEER_DEAD   40          /* 转向死区 */
#define RC_THR_FULL     800         /* 扳机满偏（映射基准） */
#define RC_STEER_FULL   700         /* 摇杆满偏（映射基准） */
#define RC_LOST_MS      500u        /* 失联判定（ms） */
#define RC_CAP_LOW      3000        /* 速度档：低（PWM duty 0~10000） */
#define RC_CAP_MID      5000        /* 速度档：中 */
#define RC_CAP_HIGH     8000        /* 速度档：高 */
#define RC_CAP_STEP     500         /* CH5/CH6 加减步进 */
#define RC_CAP_MIN      1200        /* 速度上限下限 */
#define RC_CAP_MAX      9000        /* 速度上限上限 */
#define RC_BRAKE_ZONE   300         /* 刹车段宽（后拉 300 以内=滑行减速，再拉才倒车） */
#define SERVO_CENTER    740         /* 舵机中位 raw */
#define SERVO_TRAVEL_L  85          /* 'L'（车头向右）行程 raw = 170us/2 */
#define SERVO_TRAVEL_R  65          /* 'R'（车头向左）行程 raw = 130us/2 */
#define SERVO_SLEW      400         /* 单圈最大变化 raw（主循环极快 -> 近似瞬时） */

static int    servo_cmd  = SERVO_CENTER;
static uint32 rc_frames  = 0u;      /* 收帧计数 */
static uint32 rc_tick    = 0u;      /* 最近收帧时刻 */
static uint8  rc_armed   = 0u;      /* 软启动锁：扳机回中后置 1 */
static int    rc_cap     = RC_CAP_LOW;  /* 当前速度上限（CH5/CH6 微调） */
static uint16 rc_prev5   = 0u;      /* CH5 上次值（按键边沿） */
static uint16 rc_prev6   = 0u;      /* CH6 上次值 */
static uint16 rc_last4   = 0xFFFFu; /* CH4 上次值（档位变化检测） */
static uint32 stm_per_ms = 0u;      /* STM ticks / ms */

/* ---------- 舵机输出（'L'=车头向右 / 'R'=车头向左 / 'C'=回中） ---------- */
static void rc_servo(char dir, int duty)
{
    int target = SERVO_CENTER, off, delta;
    if (duty < 0)   duty = 0;
    if (duty > 100) duty = 100;
    if      (dir == 'L') { off = duty * SERVO_TRAVEL_L / 100; target = SERVO_CENTER - off; }
    else if (dir == 'R') { off = duty * SERVO_TRAVEL_R / 100; target = SERVO_CENTER + off; }
    delta = target - servo_cmd;
    if (delta >  SERVO_SLEW) delta =  SERVO_SLEW;
    if (delta < -SERVO_SLEW) delta = -SERVO_SLEW;
    servo_cmd += delta;
    pwm_set_duty(ATOM0_CH1_P33_9, servo_cmd);
}

/* ---------- 电机输出（带符号，自动切换 DIR） ---------- */
static void rc_motor(int left, int right)
{
    if (left  >= 0) { gpio_set_level(P02_6, 1); pwm_set_duty(ATOM1_CH7_P02_7,  left);  }
    else            { gpio_set_level(P02_6, 0); pwm_set_duty(ATOM1_CH7_P02_7, -left);  }
    if (right >= 0) { gpio_set_level(P02_4, 1); pwm_set_duty(ATOM1_CH5_P02_5,  right); }
    else            { gpio_set_level(P02_4, 0); pwm_set_duty(ATOM1_CH5_P02_5, -right); }
}

/* ---------- 硬件初始化 ---------- */
static void rc_hw_init(void)
{
    pwm_init(ATOM0_CH1_P33_9, 50, SERVO_CENTER);      /* 舵机 50Hz */
    gpio_init(P02_6, GPO, 1, GPO_PUSH_PULL);          /* 左电机 DIR */
    pwm_init(ATOM1_CH7_P02_7, 17000, 0);              /* 左电机 PWM 17kHz */
    gpio_init(P02_4, GPO, 1, GPO_PUSH_PULL);          /* 右电机 DIR */
    pwm_init(ATOM1_CH5_P02_5, 17000, 0);              /* 右电机 PWM 17kHz */
}

/* ---------- 屏幕（每 100ms 一次；全部定宽防残影） ---------- */
static void rc_screen(uint32 age, int pwm, int duty, char sdir)
{
    char buf[48];
    uint16 ch1 = uart_receiver.channel[0];
    uint16 ch2 = uart_receiver.channel[1];
    uint16 ch4 = uart_receiver.channel[3];
    const char *gear = (ch4 >= 1500u) ? "HI " : ((ch4 >= 700u) ? "MID" : "LO ");

    ips200_set_color(RGB565_CYAN, RGB565_BLACK);
    ips200_show_string(104, 2, "== RC MANUAL ==");

    if ((age < RC_LOST_MS) && (uart_receiver.state != 0u))
        ips200_set_color(RGB565_GREEN, RGB565_BLACK);
    else
        ips200_set_color(RGB565_RED, RGB565_BLACK);
    sprintf(buf, "link:%-4s frames:%-8u",
            ((age < RC_LOST_MS) && (uart_receiver.state != 0u)) ? "ON" : "LOST", (unsigned)rc_frames);
    ips200_show_string(8, 26, buf);

    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "CH1:%4u  CH2:%4u", ch1, ch2);
    ips200_show_string(8, 52, buf);
    sprintf(buf, "CH3:%4u  CH4:%4u", uart_receiver.channel[2], ch4);
    ips200_show_string(8, 72, buf);
    sprintf(buf, "CH5:%4u  CH6:%4u", uart_receiver.channel[4], uart_receiver.channel[5]);
    ips200_show_string(8, 92, buf);

    ips200_set_color(RGB565_YELLOW, RGB565_BLACK);
    sprintf(buf, "gear:%s cap:%5d", gear, rc_cap);
    ips200_show_string(8, 122, buf);
    sprintf(buf, "steer:%c%3d armed:%u pwm:%5d", (sdir == 'C') ? '-' : sdir, duty, (unsigned)rc_armed, pwm);
    ips200_show_string(8, 142, buf);

    ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    ips200_show_string(8, 176, "CH4: gear CH6:+cap CH5:-cap");
    ips200_show_string(8, 196, "CH2: speed (mid = arm)");
    ips200_show_string(8, 216, "RX P10.6 UART2 100k");
}

/* ---------------------------------------------------------------- */
int core0_main(void)
{
    uint32 now, age, last_draw = 0u;
    int    thr, steer, pwm, duty, cap;
    char   sdir;
    uint16 ch4v, ch5v, ch6v;

    clock_init();
    debug_init();
    ips200_set_dir(IPS200_CROSSWISE);
    ips200_init(IPS200_TYPE_SPI);

    rc_hw_init();
    uart_receiver_init();

    stm_per_ms = IfxStm_getFrequency(&MODULE_STM0) / 1000u;

    cpu_wait_event_ready();

    while (TRUE)
    {
        /* 1) 接收帧维护 */
        if (uart_receiver.finsh_flag != 0u)
        {
            uart_receiver.finsh_flag = 0u;
            rc_frames++;
            rc_tick = IfxStm_getLower(&MODULE_STM0);
        }
        now = IfxStm_getLower(&MODULE_STM0);
        age = (uint32)((now - rc_tick) / stm_per_ms);

        /* 2) 通道解析 */
        thr = (int)uart_receiver.channel[1] - RC_MID;
        if (thr > -RC_THR_DEAD && thr < RC_THR_DEAD) thr = 0;
        steer = RC_MID - (int)uart_receiver.channel[0];
        if (steer > -RC_STEER_DEAD && steer < RC_STEER_DEAD) steer = 0;

        duty = (steer < 0 ? -steer : steer) * 100 / RC_STEER_FULL;
        if (duty > 100) duty = 100;
        if      (steer > 0) sdir = 'L';        /* 摇杆右打 -> 车头向右 */
        else if (steer < 0) sdir = 'R';
        else                sdir = 'C';

        ch4v = uart_receiver.channel[3];
        ch5v = uart_receiver.channel[4];
        ch6v = uart_receiver.channel[5];
        /* CH4 档位变化 -> 重置速度上限；CH6 加 / CH5 减（按一次一步） */
        if (ch4v != rc_last4)
        {
            rc_last4 = ch4v;
            rc_cap = (ch4v >= 1500u) ? RC_CAP_HIGH : ((ch4v >= 700u) ? RC_CAP_MID : RC_CAP_LOW);
        }
        if ((ch6v >= 992u) && (rc_prev6 < 992u))
        {
            rc_cap += RC_CAP_STEP;
            if (rc_cap > RC_CAP_MAX) rc_cap = RC_CAP_MAX;
        }
        if ((ch5v >= 992u) && (rc_prev5 < 992u))
        {
            rc_cap -= RC_CAP_STEP;
            if (rc_cap < RC_CAP_MIN) rc_cap = RC_CAP_MIN;
        }
        rc_prev5 = ch5v;
        rc_prev6 = ch6v;
        cap = rc_cap;

        /* 3) 输出（软启动 + 失联保护） */
        if ((age >= RC_LOST_MS) || (uart_receiver.state == 0u))
        {
            pwm = 0;                            /* 失联：停车 + 回中 */
            rc_motor(0, 0);
            rc_servo('C', 0);
        }
        else if (rc_armed == 0u)
        {
            pwm = 0;                            /* 软启动：扳机回中才解锁 */
            rc_motor(0, 0);
            rc_servo(sdir, duty);
            if (thr == 0) rc_armed = 1u;
        }
        else
        {
            if (thr < 0)
            {
                if (thr > -RC_BRAKE_ZONE) pwm = 0;          /* 刹车段：轻拉 -> 切断动力滑行 */
                else pwm = (thr + RC_BRAKE_ZONE) * (cap / 2) / (RC_THR_FULL - RC_BRAKE_ZONE);
            }
            else
            {
                pwm = thr * cap / RC_THR_FULL;              /* 前推 -> 前进 */
                if (pwm > cap) pwm = cap;
            }
            if      (sdir == 'L') { rc_motor(pwm, pwm * (100 - duty / 2) / 100); }  /* 车头向右：右轮（内）减速 */
            else if (sdir == 'R') { rc_motor(pwm * (100 - duty / 2) / 100, pwm); }  /* 车头向左：左轮（内）减速 */
            else                  { rc_motor(pwm, pwm); }
            rc_servo(sdir, duty);
        }

        /* 4) 屏幕（100ms 一次） */
        if ((uint32)((now - last_draw) / stm_per_ms) >= 100u)
        {
            rc_screen(age, pwm, duty, sdir);
            last_draw = now;
        }
    }
}
