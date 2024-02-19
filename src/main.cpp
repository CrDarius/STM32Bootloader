#include <stdio.h>
#include "UART_Driver.h"
#include "general_types.h"


int main(void)
{
    unsigned char text[200];
    unsigned char nrRxBytes;
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2, false, nullptr);
    if(USART2.Init() == ST_OK)
    {
        USART2.Print("Print noInt_1\n", 13, MAX_DELAY);
        USART2.Print("Print noInt_2\n", 13, MAX_DELAY);
        USART2.Read(&nrRxBytes, 1, MAX_DELAY);
        USART2.Read(&text[0], nrRxBytes, MAX_DELAY);
        while(1)
        {
            USART2.PrintIT("Print INT_1\n", 11, MAX_DELAY);
            for(uint32_t i = 0; i < 10000000; ++i);
        }
        
    }
    else
    {
        // should not be reached
    }

}

