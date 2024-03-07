#pragma once
#include <stdint.h>
#include "Arm_MemMap.h"

#define MCU_CLOCK_SPEED_HZ  16000000U

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
    static void SysTick_Enable(void);
    static void SysTick_Disable(void);

    /* Set the system timer in microseconds */
    static void SysTick_SetReloadValue(uint32_t);
    static void SysTick_SetTickSrc(SYSTICK_ClockSrc);
    static void SysTick_EnableInt(void);
    static void SysTick_DisableInt(void);

private:
    SysTick() = default;
    static volatile SYSTICK_registers_t * const registers;
};
