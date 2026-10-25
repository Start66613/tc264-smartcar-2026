#include "control_time.h"
int LineStartupLimit(unsigned elapsed_ms, int full_target)
{
    if (full_target <= 150 || elapsed_ms >= 2000u) return full_target;
    return 150 + (int)((unsigned)(full_target - 150) * elapsed_ms / 2000u);
}
float LineYawDelta(float degrees_per_second, unsigned elapsed_ms)
{
    return degrees_per_second * (float)elapsed_ms * 0.001f;
}
