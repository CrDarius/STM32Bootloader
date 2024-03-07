#include <stdio.h>
#include <cstring>
#include "UART_Driver.h"
#include "general_types.h"
#include "SysTick.h"
#include "MCU_Utilities.h"

void SetGlobalTimer(void)
{           
    SysTick::SysTick_SetReloadValue(1000000);
    SysTick::SysTick_EnableInt();
    SysTick::SysTick_SetTickSrc(SYSTICK_ClockSrc::PROC_CLOCK);
    SysTick::SysTick_Enable();
}

int main(void)
{
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
    SetGlobalTimer();
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2);
    if(USART2.Init() == ST_OK)
    {
        uint32_t currentTime = globalTime;
        uint8_t i = 0;
        while(i < 10)
        {
            if(globalTime-currentTime)
            {
                char msg[] = "Message\n\r";
                USART2.Print((const char*)&msg[0], strlen(msg));
                currentTime = globalTime;
                i++;
            }

        }
    }
    else
    {
        // should not be reached
    }   
    while(1);
}


