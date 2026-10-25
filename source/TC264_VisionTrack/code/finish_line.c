// finish_line.c
#include "finish_line.h"

/* 终点检测：只认“黑白斑马线”那种规则密集的短黑条。
 * 学长原版是“短黑段 >=3 且连续 2 帧”就判终点，太松——赛道中途任意一条
 * 黑线、或转弯时两侧黑边界凑在一起，都会被误判成终点。
 * 现在收紧：短黑条 >=6 条、宽 2..6 像素、且连续 3 帧才确认。
 * 另外 cpu0_main 里还加了“起跑后 1.5s 内不检终点”的保护。 */
#define FINISH_SEG_MIN   6      /* 至少 6 条短黑条（斑马线特征） */
#define FINISH_SEG_WMIN  2
#define FINISH_SEG_WMAX  6
#define FINISH_FRAMES    3      /* 连续 3 帧才确认 */

int DetectFinishLine(uint8 image[1][128]) {
    static int good_frames = 0;
    int segments = 0;
    int i = 0, begin, run;

    while (i < 128) {
        if (image[0][i] != 0) { i++; continue; }
        begin = i;
        while (i < 128 && image[0][i] == 0) i++;
        run = i - begin;
        if (run >= FINISH_SEG_WMIN && run <= FINISH_SEG_WMAX) segments++;
    }

    if (segments >= FINISH_SEG_MIN) good_frames++;
    else                            good_frames = 0;

    if (good_frames >= FINISH_FRAMES) return 1;
    return 0;
}
