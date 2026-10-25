#ifndef CONTROL_TIME_H
#define CONTROL_TIME_H
int LineStartupLimit(unsigned elapsed_ms, int full_target);
float LineYawDelta(float degrees_per_second, unsigned elapsed_ms);
#endif
