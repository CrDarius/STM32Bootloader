#pragma once
#include "SysTick.h"

/* Using MAX_DELAY for timing functions will evaluate to undefinitive waiting posibility */
#define MAX_DELAY   UINT32_MAX

#define DELAY_EXCEEDED(startTime, currentTime, waitTime)     ( ((currentTime)>((startTime)+(waitTime)) && ((currentTime) > (startTime)) ) \
                                                            || ((((currentTime)+((MAX_DELAY)-(startTime)))>(waitTime))  && ((currentTime) < (startTime))) )

void Systick_Interrupt(void);

extern volatile uint32_t globalTime;


