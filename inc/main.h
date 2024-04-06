#pragma once

#include <stdio.h>
#include <cstring>
#include "UART_Driver.h"
#include "general_types.h"
#include "SysTick.h"
#include "MCU_Utilities.h"
#include "GPIO_Driver.h"
#include "CRC_Driver.h"
#include "Bootloader_MemMap.h"
#include "BootUtility.h"
#include "Flash_Driver.h"


typedef bool Button_State_t;
#define BUTTON_PRESSED  true
#define BUTTON_RELEASED false


#define BOOT_TIMEOUT    3u

static inline void SetGlobalTimer(void);
static inline Button_State_t GetUserButtonState(const GPIO& button);
static inline void USART_INIT(void);
static inline void GPIO_INIT(GPIO& gpio);
static inline void NVIC_INIT(void);
static inline void CRC_INIT(void);
static inline void USART_INIT(void);

extern void BootControl(void);


void SetGlobalTimer(void)
{           
    SysTick::SetReloadValue(1000000);
    SysTick::EnableInt();
    SysTick::SetTickSrc(SYSTICK_ClockSrc::PROC_CLOCK);
    SysTick::Enable();
}

Button_State_t GetUserButtonState(const GPIO &button)
{
    return (button.GPIO_ReadPin() == PIN_LOW) ? BUTTON_PRESSED : BUTTON_RELEASED; 
}

void USART_INIT()
{
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2);
    (void)USART2.Init();
}

void GPIO_INIT(GPIO& gpio)
{
    gpio.Config(INPUT);
    gpio.Init();
}

void NVIC_INIT()
{
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
}

void CRC_INIT()
{
    CRC::CRC_Init();
}

void FLASH_INIT()
{
    //FLASH::Config();
    FLASH::Init();
}