/*
 * duoji.h
 *
 *  Created on: 2026年1月18日
 *      Author: yhm
 */

#ifndef CODE_DUOJI_H_
#define CODE_DUOJI_H_

void servo_init(void);//舵机初始化
void servo_control(char direction,int duty);//舵机方向及角度控制（0<duty=<100）
void servo_release(void);//停止脉冲输出，舵机松开（可手动推车）



#endif /* CODE_DUOJI_H_ */
