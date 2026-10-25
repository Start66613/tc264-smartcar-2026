/*
 * pid.h
 *
 *  Created on: 2026年1月19日
 *      Author: yhm
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_

#include "zf_common_headfile.h"

// 连续丢线达到该帧数即停车，避免无有效线时盲冲（8 帧 * 10ms 约 80ms）。
#define LOST_STOP_LIMIT 8u
// 丢线超过这么多帧才彻底停车（不是 8 帧就停，太早）
#define LOST_GIVEUP 30u

// PID参数结构体
typedef struct {
    float Kp;           // 比例系数
    float Ki;           // 积分系数
    float Kd;           // 微分系数
    float integral_max; // 积分限幅
    float output_max;   // 输出限幅
} PID_Param;

// PID控制器结构体
typedef struct {
    PID_Param param;    // PID参数
    float error;        // 当前误差
    float last_error;   // 上次误差
    float integral;     // 积分项
    float derivative;   // 微分项
    float output;       // 输出值
} PID_Controller;

// 函数声明
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float integral_max, float output_max);
float PID_Calculate(PID_Controller *s, float target, float current);
void PID_Reset(PID_Controller *pid);

// 循迹专用函数
int CalculateCenterLine(uint8 image[1][128]);
float CalculateDeviation(uint8 image[1][128], int center_position);
void LineFollow_PID(PID_Controller *pid_steering, PID_Controller *pid_speed, uint8 image[1][128]);
void LineFollow_PID_End(PID_Controller *pid_steering, PID_Controller *pid_speed, uint8 image[1][128]);

// 最近一帧循迹输出，供上层显示与运行报告读取，避免重复计算。
extern volatile int      line_out_center;
extern volatile int      line_out_servo_duty;
extern volatile float    line_out_deviation;
extern volatile float    line_out_steering;
extern volatile int      line_out_left_speed;
extern volatile int      line_out_right_speed;
extern volatile int      line_out_flags;        /* v116: bit0 valid / bit1 degraded / bit2 all_white */
extern volatile unsigned line_out_lost_count;
extern volatile unsigned line_out_black_pixels;
extern volatile int      line_speed_scale_pct;   /* v083：遥控速度缩放（%） */
extern volatile int      line_steer_kp_scale_pct;/* v084：遥控转向力度缩放（%） */

#endif /* CODE_PID_H_ */
