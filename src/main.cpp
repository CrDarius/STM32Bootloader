#include "main.h"

int main(void)
{
    GPIO userButton(GPIOC, PIN13);

    NVIC_INIT();

    FLASH_INIT();

    GPIO_INIT(userButton);

    USART_INIT();

    CRC_INIT();

    SetGlobalTimer();

    uint32_t currentTime = SysTick::GetGlobalTime();

    while( SysTick::GetGlobalTime() < (currentTime + BOOT_TIMEOUT) )
    {
        if(GetUserButtonState(userButton) == BUTTON_PRESSED)
        {
            /* jump to bootloader application */ 
            const char *msg = "Jumping to boot control\r\n";
            USART2.Transmit(msg, strlen(msg),MAX_DELAY);
            BootControl();
        }
    }

    /* jump to user application */
    const char *msg = "Jumping to user app\r\n";
    USART2.Transmit(msg, strlen(msg),MAX_DELAY);
    JumpToUserApp();
    

    while(true)
    {
        /* Should never reach this point */
    }
}