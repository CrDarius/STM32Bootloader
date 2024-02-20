#include <stdio.h>
#include "UART_Driver.h"
#include "general_types.h"

void RxUSARTCallbackFunc(void);

int main(void)
{
    // unsigned char text[200];
    // unsigned char nrRxBytes;
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2, true, RxUSARTCallbackFunc);
    if(USART2.Init() == ST_OK)
    {
        // USART2.Print("Print noInt_1\n", 13, MAX_DELAY);
        // USART2.Print("Print noInt_2\n", 13, MAX_DELAY);
        // USART2.Read(&nrRxBytes, 1, MAX_DELAY);
        // USART2.Read(&text[0], nrRxBytes, MAX_DELAY);
        //USART2.PrintIT("Print interrupt", 12, MAX_DELAY);
        while(USART2.ReadIT(4, MAX_DELAY) == ST_BUSY);

    }
    else
    {
        // should not be reached
    }
    while(1);
}

void RxUSARTCallbackFunc(void)
{
    USART2.PrintIT((const char*)&USART::USART2_MessageStr.rxBufferUSART[0], USART::USART2_MessageStr.rxLen, MAX_DELAY);
}

