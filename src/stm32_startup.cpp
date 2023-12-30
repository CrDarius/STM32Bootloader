#include <stdint.h>
#include "interrupts.h"

#define NULL    0u

#define MSTACK  0x20017FFFu

#define RAM_INIT_SUCCESS    (1 == 1)
#define RAM_INIT_FAIL       (1 != 1)

extern void Reset_Handler(void);
uint8_t init_ram(void);
extern int main(void);

extern uint32_t _bss_start, _bss_end;
uint32_t bss_size;

__attribute__ ((section(".isr_vector"))) uint32_t vector_table[] = {
    (uint32_t)MSTACK,
    (uint32_t)Reset_Handler,
    (uint32_t)NMI_Interrupt,
    (uint32_t)HardFault_Interrupt,
    (uint32_t)MemManage_Interrupt,
    (uint32_t)BusFault_Interrupt,
    (uint32_t)UsageFault_Interrupt,
    (uint32_t)NULL,
    (uint32_t)SVCall_Interrupt,
    (uint32_t)DebugMonitor_Interrupt,
    (uint32_t)NULL,
    (uint32_t)PendSV_Interrupt,
    (uint32_t)Systick_Interrupt,
    (uint32_t)WWDG_Interrupt,
    (uint32_t)EXTI16_Interrupt,
    (uint32_t)EXTI21_Interrupt,
    (uint32_t)EXTI22_Interrupt,
    (uint32_t)FLASH_Interrupt,
    (uint32_t)RCC_Interrupt,
    (uint32_t)EXTI0_Interrupt,
    (uint32_t)EXTI1_Interrupt,
    (uint32_t)EXTI2_Interrupt,
    (uint32_t)EXTI3_Interrupt,
    (uint32_t)EXTI4_Interrupt,
    (uint32_t)DMA1_Stream0_Interrupt,
    (uint32_t)DMA1_Stream1_Interrupt,
    (uint32_t)DMA1_Stream2_Interrupt,
    (uint32_t)DMA1_Stream3_Interrupt,
    (uint32_t)DMA1_Stream4_Interrupt,
    (uint32_t)DMA1_Stream5_Interrupt,
    (uint32_t)DMA1_Stream6_Interrupt,
    (uint32_t)ADC_Interrupt,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)EXTI9_5_Interrupt,
    (uint32_t)TIM1_BRK_TIM9_Interrupt,
    (uint32_t)TIM1_UP_TIM10_Interrupt,
    (uint32_t)TIM1_TRG_COM_TIM11_Interrupt,
    (uint32_t)TIM1_CC_Interrupt,
    (uint32_t)TIM2_Interrupt,
    (uint32_t)TIM3_Interrupt,
    (uint32_t)TIM4_Interrupt,
    (uint32_t)I2C1_EV_Interrupt,
    (uint32_t)I2C1_ER_Interrupt,
    (uint32_t)I2C2_EV_Interrupt,
    (uint32_t)I2C2_ER_Interrupt,
    (uint32_t)SPI1_Interrupt,
    (uint32_t)SPI2_Interrupt,
    (uint32_t)USART1_Interrupt,
    (uint32_t)USART2_Interrupt,
    (uint32_t)NULL,
    (uint32_t)EXTI15_10_Interrupt,
    (uint32_t)EXTI17_Interrupt,
    (uint32_t)EXTI18_Interrupt,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)DMA1_Stream7_Interrupt,
    (uint32_t)NULL,
    (uint32_t)SDIO_Interrupt,
    (uint32_t)TIM5_Interrupt,
    (uint32_t)SPI3_Interrupt,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)DMA2_Stream0_Interrupt,
    (uint32_t)DMA2_Stream1_Interrupt,
    (uint32_t)DMA2_Stream2_Interrupt,
    (uint32_t)DMA2_Stream3_Interrupt,
    (uint32_t)DMA2_Stream4_Interrupt,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)OTG_FS_Interrupt,
    (uint32_t)DMA2_Stream5_Interrupt,
    (uint32_t)DMA2_Stream6_Interrupt,
    (uint32_t)DMA2_Stream7_Interrupt,
    (uint32_t)USART6_Interrupt,
    (uint32_t)I2C3_EV_Interrupt,
    (uint32_t)I2C3_ER_Interrupt,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)FPU_Interrupt,
    (uint32_t)NULL,
    (uint32_t)NULL,
    (uint32_t)SPI4_Interrupt
};


uint8_t init_ram(void)
{
    uint32_t i, bss_size;
    uint8_t ret_val = RAM_INIT_FAIL;
    uint8_t *pDest = (uint8_t*)&_bss_start;

    if(&_bss_start > &_bss_end)
        return ret_val;
    else
        bss_size = &_bss_end - &_bss_start;

    for(i = 0; i < bss_size; i++)
    {
        pDest[i] = 0;
    }

    if(i == bss_size)
        ret_val  = RAM_INIT_SUCCESS;

    return ret_val;
}

void Reset_Handler(void)
{
    while(init_ram() == RAM_INIT_FAIL);

    main();
}