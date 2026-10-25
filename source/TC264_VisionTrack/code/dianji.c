/*
 * dianji.c
 *
 *  Created on: 2026年1月18日
 *      Author: yhm
 */

#include "zf_common_headfile.h"

//DRV8701


//电机初始化
void DRV8701_Init_D(void)
{
    gpio_init(DIR_L,GPO,1,GPO_PUSH_PULL);
    pwm_init(PWM_B,17000,0);

    gpio_init(DIR_R,GPO,1,GPO_PUSH_PULL);
    pwm_init(PWM_A,17000,0);
}

//左右电机配速
void DRV_8701_MOTOR_DRIVER_D(int L_SPEED,int R_SPEED)
{
    if(L_SPEED>=0)//左电机正转(前进)
    {
        gpio_set_level(DIR_L,1);            // DIR 极性反了：实测高电平=前进
        pwm_set_duty(PWM_B,L_SPEED);
    }
    else//左电机反转(后退)
    {
        gpio_set_level(DIR_L,0);
        pwm_set_duty(PWM_B,-L_SPEED);
    }
    if(R_SPEED>=0)//右电机正转(前进)
    {
        gpio_set_level(DIR_R,1);
        pwm_set_duty(PWM_A,R_SPEED);
    }
    else//右电机反转(后退)
    {
        gpio_set_level(DIR_R,0);
        pwm_set_duty(PWM_A,-R_SPEED);
    }
}



