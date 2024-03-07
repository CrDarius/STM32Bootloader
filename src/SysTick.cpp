#include "SysTick.h"

volatile SYSTICK_registers_t * const SysTick::registers = (SYSTICK_registers_t *)SYST_CSR_ADRESS;

void SysTick::SysTick_Enable(void)
{
    SysTick::registers->SYST_CSR |= (1 << SYSTICK_CSR_BitPos::ENABLE);
}

void SysTick::SysTick_Disable(void)
{
    SysTick::registers->SYST_CSR &= ~(1 << SYSTICK_CSR_BitPos::ENABLE);
}

void SysTick::SysTick_SetReloadValue(uint32_t microSec)
{
    SysTick::registers->SYST_RVR = MCU_CLOCK_SPEED_HZ/1000000 * microSec - 1;
}

void SysTick::SysTick_SetTickSrc(SYSTICK_ClockSrc src)
{
    if(src != EXT_CLOCK)
        SysTick::registers->SYST_CSR |= (1 << SYSTICK_CSR_BitPos::CLKSOURCE);
    else
        SysTick::registers->SYST_CSR &= ~(1 << SYSTICK_CSR_BitPos::CLKSOURCE);
}
    
void SysTick::SysTick_EnableInt(void)
{
    SysTick::registers->SYST_CSR |= (1 << SYSTICK_CSR_BitPos::TICKINT);
}

void SysTick::SysTick_DisableInt(void)
{
    SysTick::registers->SYST_CSR &= ~(1 << SYSTICK_CSR_BitPos::TICKINT);
}
