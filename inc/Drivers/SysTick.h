#pragma once
#include <stdint.h>
#include "Arm_MemMap.h"

#define MCU_CLOCK_SPEED_HZ  16000000U

/* Using MAX_DELAY for timing functions will evaluate to undefinitive waiting posibility */
#define MAX_DELAY   UINT32_MAX

#define DELAY_EXCEEDED(startTime, currentTime, waitTime)     ( ((currentTime)>((startTime)+(waitTime)) && ((currentTime) > (startTime)) ) \
                                                            || ((((currentTime)+((MAX_DELAY)-(startTime)))>(waitTime))  && ((currentTime) < (startTime))) )

void Systick_Interrupt(void);

typedef struct
{
public:
    uint32_t SYST_CSR;
    uint32_t SYST_RVR;
    uint32_t SYST_CVR;
    uint32_t SYST_CALIB;
}SYSTICK_registers_t;

typedef enum{
    ENABLE,
    TICKINT,
    CLKSOURCE,
    COUNTFLAG = 16
}SYSTICK_CSR_BitPos;

typedef enum{
    RELOAD
}SYSTICK_RVR_BitPos;

typedef enum{
    CURRENT
}SYSTICK_CVR_BitPos;

typedef enum{
    TENMS,
    SKEW = 30,
    NOREF
}SYSTICK_CALIB_BitPos;

typedef enum{
    EXT_CLOCK,
    PROC_CLOCK
}SYSTICK_ClockSrc;

struct SysTick
{
public:
    static void Enable(void);
    static void Disable(void);

    /* Set the system timer in microseconds */
    static void SetReloadValue(uint32_t);
    static void SetTickSrc(SYSTICK_ClockSrc);
    static void EnableInt(void);
    static void DisableInt(void);
    static uint32_t GetGlobalTime(void);

    friend void Systick_Interrupt(); 
private:
    SysTick() = default;
    static volatile uint32_t globalTime;
    static volatile SYSTICK_registers_t * const registers;
};
