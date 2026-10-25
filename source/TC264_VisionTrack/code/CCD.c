/*
 * CCD.c
 *
 *  Created on: 2026年1月19日
 *      Author: yhm
 */

#include "zf_common_headfile.h"

uint8 base_image[1][128]={{0}};
uint8 img_threshold;
//uint8 img_threshold0=0;

/* 8 位灰度下的最小有效对比度：低于它说明整幅既没有清晰黑线也没有白底，
 * 属于全黑、全白、镜头被遮挡或曝光异常。此时不二值化，整幅判为白底，
 * 让 CalculateCenterLine 返回“丢线”，而不是把噪声硬分成黑白。 */
#define CCD_CONTRAST_MIN 24

extern volatile unsigned ccd_frames;   // 定义在 cpu0_main.c（放 CPU0 RAM，便于 DAS 回读）

void CCD(void)
{
    int i;
    uint8 lo = 255, hi = 0;

    // 采集由 CCU61_CH1 周期中断驱动（曝光 10ms）。只有当一帧完整采集完成
    // （finish_flag 置位）时才搬运数据，避免主循环读到“采到一半”的帧造成中线跳变。
    if (tsl1401_finish_flag)
    {
        tsl1401_finish_flag = 0;
        ccd_frames++;
        for (i = 0; i < 128; i++)
        {
            base_image[0][i] = (uint8_t)tsl1401_data[0][i];
        }
    }

    /* ★v047★：改用"分位数对比度"(10%~90%)代替 min/max。
       实测（车停在大环白面上）：整幅灰度 63~109，其实是一片均匀的白，
       但 max=109 来自个别高光点 → max-min=46 被误判"有对比度" →
       Ostu 把均匀白面切成大半个黑 → 车报"丢线"，行为完全乱。
       分位数法把两头各 10% 的离群点排除，均匀面就正确判为"无对比度→整幅判白"。 */
    {
        int hist[256];
        int acc, p10 = 0, p90 = 0, n10 = (128 * 10) / 100, n90 = (128 * 90) / 100;
        for (i = 0; i < 256; i++) hist[i] = 0;
        for (i = 0; i < 128; i++) hist[base_image[0][i]]++;
        acc = 0;
        for (i = 0; i < 256; i++) { acc += hist[i]; if (acc >= n10) { p10 = i; break; } }
        acc = 0;
        for (i = 0; i < 256; i++) { acc += hist[i]; if (acc >= n90) { p90 = i; break; } }
        lo = (uint8)p10; hi = (uint8)p90;
    }

    if ((int)hi - (int)lo < CCD_CONTRAST_MIN)
    {
        for (i = 0; i < 128; i++)
        {
            image[0][i] = 255;
        }
        img_threshold = 0;
        return;
    }

    img_threshold=Ostu(base_image);

    set_image_twovalues(img_threshold);
}

