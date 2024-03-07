#include "MCU_Utilities.h"

volatile uint32_t globalTime;

void Systick_Interrupt()
{
    globalTime++;
}