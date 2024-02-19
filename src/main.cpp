#include <stdio.h>
#include "UART_Driver.h"
#include "general_types.h"


int main(void)
{
    char text[] = "USART2 running\n\r";
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2);
    if(USART2.Init() == ST_OK)
    {
        while(1)
        {
            for(uint32_t count = 0; count < 1000000; count++);
            if(USART2.currentState == USART_AVAILABLE)
            {
                USART2.PrintIT((const char*)text, sizeof(text), MAX_DELAY);
            }
        
        }
    }
    else
    {
        // should not be reached
    }

}

