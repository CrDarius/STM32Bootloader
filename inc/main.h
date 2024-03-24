#pragma once

#include <stdio.h>
#include <cstring>
#include "UART_Driver.h"
#include "general_types.h"
#include "SysTick.h"
#include "MCU_Utilities.h"
#include "GPIO_Driver.h"
#include "Bootloader_MemMap.h"

typedef bool Button_State_t;
#define BUTTON_PRESSED  true
#define BUTTON_RELEASED false


#define BOOT_TIMEOUT    3u

inline void SetGlobalTimer(void);
inline Button_State_t GetUserButtonState(const GPIO& button);
inline void JumpToUserApp(void);

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

inline void JumpToUserApp(void)
{
    /* Set location of Vector Table for user app */
    SetVTOR(USER_APP_VT_ADDRESS - BOOTLOADER_VT_ADDRESS);

    /* Set new location for Main Stack Pointer */
    uint32_t user_sp = *((uint32_t *)USER_APP_VT_ADDRESS); // First 4 bytes from the Vector Table represent the Stack Pointer
    SetStackPointer(user_sp);

    /* Jump to the reset handler of the user app */
    void (*fp_handler)(void) = (void (*)(void))USER_APP_RESET_HANDLER_ADDRESS; 
    fp_handler();
}