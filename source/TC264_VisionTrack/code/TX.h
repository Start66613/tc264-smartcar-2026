/*
 * TX.h
 *
 *  Created on: 2026Äê1ÔÂ19ÈÕ
 *      Author: yhm
 */

#ifndef CODE_TX_H_
#define CODE_TX_H_

#include "zf_common_headfile.h"



extern uint8 base_image[1][128];
extern uint8 image[1][128];
extern uint8 img_threshold;


uint8 Ostu(uint8 index[1][128]);
void set_image_twovalues(uint8 value);

#endif /* CODE_TX_H_ */
