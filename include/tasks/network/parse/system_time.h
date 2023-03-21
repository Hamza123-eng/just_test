
#ifndef TIME_H_
#define TIME_H_

#include "stdint.h"

uint64_t GetStandardTime(bool sync_time);

bool TimeIsAccurate(void *param);
void InitTimeManger(void *param);
void test();
#endif