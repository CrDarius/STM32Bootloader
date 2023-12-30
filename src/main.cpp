#include <stdio.h>
#include "UART_Driver.h"
#include "general_types.h"

int main(void)
{
    char text[] = "USART2 running\n\r";
    USART usart2(USART2);
    (void)usart2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2);
    if(usart2.Init() == ST_OK)
    {
        while(1)
        {
            for(uint32_t count = 0; count < 1000000; count++);
            if(usart2.currentState == USART_AVAILABLE)
                usart2.Print((const char*)text, sizeof(text), MAX_DELAY);
        }
    }
    else
    {
        // should not be reached
    }

}

/* Interrupts must be activated for ARM Cortex based on NVIC, don't forget to do that*/