#include "Clock_Driver.h"

volatile clock_registers_t * const Clock::registers = (clock_registers_t *)RCC_ADDRESS;


void Clock::EnableClock_USART2(void)
{
    Clock::registers->APB1ENR |= (1 << RCC_APB1ENR_BitPos::USART2_EN);
}

void Clock::EnableClock_GPIOA(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::GPIOA_EN);
}


// to be moved
volatile uint32_t globalTime;