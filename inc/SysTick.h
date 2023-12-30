#pragma once
#include <stdint.h>


typedef struct
{
public:
    uint32_t SYST_CSR;
    uint32_t SYST_RVR;
    uint32_t SYST_CVR;
    uint32_t SYST_CALIB;
}SYSTICK_registers_t;



struct SysTick
{
public:
    

protected:
    SysTick() = default;

private:
    static volatile SYSTICK_registers_t * const registers;
};
