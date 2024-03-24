#include <stdint.h>
#include "interrupts.h"

#define NULL    0u

#define MSTACK  0x20017FFFu

#define RAM_INIT_SUCCESS    (1 == 1)
#define RAM_INIT_FAIL       (1 != 1)

extern void (*_init_array_start []) (void);
extern void (*_init_array_end []) (void);

extern void Reset_Handler(void);
static void init_ram(void);
static void call_ctors(void);
static void init_stack(void);
extern int main(void);

extern uint32_t _bss_start, _bss_end, _data_start, _data_end, _text_end;
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
    (uint32_t)NULL,
    (uint32_t)NULL,
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


static void init_ram(void)
{
    uint32_t bss_size, data_size;
    uint32_t i, j;
    uint8_t *pDest, *pSrc;

   
   bss_size = (uint8_t *)&_bss_end - (uint8_t *)&_bss_start;
   data_size = (uint8_t *)&_data_end - (uint8_t *)&_data_start;
   

    pDest = (uint8_t*)&_bss_start;
    for(j = 0; j < bss_size; j++)
    {
        pDest[j] = 0;
    }

    pDest = (uint8_t*)&_data_start;
    pSrc = (uint8_t*)&_text_end;
    for(i = 0; i < data_size; i++)
    {
        pDest[i] = pSrc[i];
    }
}

static void call_ctors(void)
{
    uint32_t count = _init_array_end - _init_array_start;
    
    for(uint32_t i = 0; i < count; i++)
        _init_array_start[i] ();
}

static void init_stack(void)
{
    asm volatile("mov sp, %0" : : "r"(vector_table[0]));
}

void Reset_Handler(void)
{
    init_stack();   

    init_ram();

    call_ctors();

    main();
}