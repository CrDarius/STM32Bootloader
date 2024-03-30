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

void Clock::EnableClock_GPIOB(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::GPIOB_EN);
}

void Clock::EnableClock_GPIOC(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::GPIOC_EN);
}

void Clock::EnableClock_GPIOD(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::GPIOD_EN);
}

void Clock::EnableClock_GPIOE(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::GPIOE_EN);
}

void Clock::EnableClock_GPIOH(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::GPIOH_EN);
}

void Clock::EnableClock_CRC(void)
{
    Clock::registers->AHB1ENR |= (1 << RCC_AHB1ENR_BitPos::CRC_EN);
}