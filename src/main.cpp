#include <stdio.h>
#include "UART_Driver.h"
#include "general_types.h"

void RxUSARTCallbackFunc(void);

int main(void)
{
    unsigned char text[200];
    // unsigned char nrRxBytes;
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2);
    if(USART2.Init() == ST_OK)
    {
        USART2.PrintIT("Print before read", 17, MAX_DELAY);
        while(USART2.ReadIT(text, 4, MAX_DELAY) == ST_BUSY);
        while(USART2.PrintIT("Print before read", 17, MAX_DELAY) == ST_BUSY);
    }
    else
    {
        // should not be reached
    }
    while(1);
}


