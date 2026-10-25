/*
 * dianji.h
 *
 *  Created on: 2026Äê1ÔÂ18ÈÕ
 *      Author: yhm
 */

#ifndef CODE_DIANJI_H_
#define CODE_DIANJI_H_

/* Measured 2026-10-07: P02.5 drives physical RIGHT / encoder6.
 * P02.7 drives physical LEFT / encoder5. Keep each DIR with its PWM. */
#define DIR_R      (P02_4)
#define PWM_A      (ATOM1_CH5_P02_5)

#define DIR_L      (P02_6)
#define PWM_B      (ATOM1_CH7_P02_7)

void DRV8701_Init_D(void);
void DRV_8701_MOTOR_DRIVER_D(int L_SPEED,int R_SPEED);

#endif /* CODE_DIANJI_H_ */
