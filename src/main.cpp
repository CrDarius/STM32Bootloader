#include "main.h"

int main(void)
{
    bool enterBootMode = false;
    SetBasePriority(0xFF);
    EnableAllInterrupts();
    // Set the interrupt priority for USART2
    NVIC::NVIC_SetInterruptPriority(USART2_INT_POS, 0xFFu);
    SetGlobalTimer();
    GPIO userButton(GPIOC, PIN13);
    userButton.Config(INPUT);
    userButton.Init();
    (void)USART2.Config(WORD_LENTGTH_8BIT, NO_PARITY, 9600, NO_STOPBITS_2);
    (void)USART2.Init();

    uint32_t currentTime = SysTick::GetGlobalTime();

    while( SysTick::GetGlobalTime() < (currentTime + BOOT_TIMEOUT) )
    {
        if(GetUserButtonState(userButton) == BUTTON_PRESSED)
        {
            enterBootMode = true;      
            break;
        }
    }

    if(enterBootMode == false)
    {
        // jump to user application
        const char *msg = "Jumping to user app\r\n";
        USART2.Print(msg, strlen(msg),MAX_DELAY);
        JumpToUserApp();
    }
    else
    {
        // jump to bootloader application 
        const char *msg = "Message from bootloader\r\n";
        USART2.Print(msg, strlen(msg),MAX_DELAY);
    }
    

    while(1);
}

