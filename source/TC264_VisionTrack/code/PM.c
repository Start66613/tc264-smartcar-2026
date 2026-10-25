/*
 * PM.c
 *
 *  Created on: 2026Äê1ÔÂ19ÈÕ
 *      Author: yhm
 */

#include "zf_common_headfile.h"

void PM_init(void)
{
    ips200_set_dir(IPS200_CROSSWISE); ips200_init(IPS200_TYPE_SPI);
    ips200_clear ();

}



