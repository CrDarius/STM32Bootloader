#pragma once
#include "SysTick.h"

#define MCU_CLOCK_SPEED_HZ  16000000U

/* Using MAX_DELAY for timing functions will evaluate to undefinitive waiting posibility */
#define MAX_DELAY   UINT32_MAX

#define DELAY_EXCEEDED(startTime, currentTime, waitTime)     ( ((currentTime)>((startTime)+(waitTime)) && ((currentTime) > (startTime)) ) \
                                                            || ((((currentTime)+((MAX_DELAY)-(startTime)))>(waitTime))  && ((currentTime) < (startTime))) )

extern volatile uint32_t globalTime;