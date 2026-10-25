/*
 * TX.c
 *
 *  Created on: 2026年1月19日
 *      Author: yhm
 */

#include "zf_common_headfile.h"

    uint8 hist[256]={0};
    float P[256]={0};
    float PK[256]={0};
    float MK[256]={0};
    uint8 img_threshoid;
    float imgsize;
    uint8 image[1][128]={{0}};

uint8 Ostu(uint8 base_image[1][128])
{



        uint8 threshold = 0;
        float imgsize = 128.0;

        // 1. 统计直方图
        uint32 hist[256] = {0};  // 使用32位避免溢出
        for(uint8 i=0;i<1;i++)
        {
            for(uint8 j = 0; j< 128; j++)
            {
                uint8 pixel_value = base_image[i][j];
                hist[pixel_value]++;
            }
        }

        // 2. 计算全局平均灰度
        float global_mean = 0.0;
        for(int i = 0; i < 256; i++)
        {
            global_mean += i * hist[i];
        }
        global_mean /= imgsize;

        // 3. 大津法求最佳阈值
        float max_variance = 0.0;
        float w0 = 0.0;      // 前景像素比例
        float m0 = 0.0;      // 前景平均灰度
        float total_pixels = imgsize;

        for(int t = 0; t < 256; t++)
        {
            w0 += hist[t];
            if(w0 == 0) continue;

            m0 += t * hist[t];

            float w1 = total_pixels - w0;
            if(w1 == 0) break;

            // 前景平均灰度
            float u0 = m0 / w0;

            // 背景平均灰度
            float u1 = (global_mean * total_pixels - m0) / w1;

            // 类间方差
            float variance = w0 * w1 * (u0 - u1) * (u0 - u1);

            if(variance > max_variance)
            {
                max_variance = variance;
                threshold = (uint8)t;
            }
        }

        return threshold;


}

/* 二值化：大津法阈值本身属于暗类，因此用 <= 而不是 <。
 * 原实现用 < ，当阈值恰好等于暗类灰度时（例如两级灰度 20/200，
 * Ostu 返回 20），20<20 为假 -> 整幅被判成白底，黑线消失。
 * 改成 <= 后 20<=20 归为黑，黑线保留。 */
void set_image_twovalues(uint8 value)
{
    uint8 temp_value;
    for(uint8 i=0;i<1;i++)
    {
        for(uint8 j=0;j<128;j++)
        {
            temp_value=base_image[i][j];
            if(temp_value<=value)
            {
                image[i][j]=0;
            }
            else
            {
                image[i][j]=255;
            }
        }
    }
}
