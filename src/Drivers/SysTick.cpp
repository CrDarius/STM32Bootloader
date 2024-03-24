#include "SysTick.h"

volatile SYSTICK_registers_t * const SysTick::registers = (SYSTICK_registers_t *)SYST_CSR_ADRESS;
volatile uint32_t SysTick::globalTime;

void Systick_Interrupt()
{
    SysTick::globalTime++;
}

void SysTick::Enable(void)
{
    SysTick::registers->SYST_CSR |= (1 << SYSTICK_CSR_BitPos::ENABLE);
}

void SysTick::Disable(void)
{
    SysTick::registers->SYST_CSR &= ~(1 << SYSTICK_CSR_BitPos::ENABLE);
}

void SysTick::SetReloadValue(uint32_t microSec)
{
    SysTick::registers->SYST_RVR = MCU_CLOCK_SPEED_HZ/1000000 * microSec - 1;
}

void SysTick::SetTickSrc(SYSTICK_ClockSrc src)
{
    if(src != EXT_CLOCK)
        SysTick::registers->SYST_CSR |= (1 << SYSTICK_CSR_BitPos::CLKSOURCE);
    else
        SysTick::registers->SYST_CSR &= ~(1 << SYSTICK_CSR_BitPos::CLKSOURCE);
}
    
void SysTick::EnableInt(void)
{
    SysTick::registers->SYST_CSR |= (1 << SYSTICK_CSR_BitPos::TICKINT);
}

void SysTick::DisableInt(void)
{
    SysTick::registers->SYST_CSR &= ~(1 << SYSTICK_CSR_BitPos::TICKINT);
}

uint32_t SysTick::GetGlobalTime(void)
{
    return SysTick::globalTime;
}
