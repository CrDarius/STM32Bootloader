#pragma once
#include "Mcu_MemMap.h"

typedef struct{
public:
    uint32_t CR;
    uint32_t PLLCFGR;
    uint32_t CFGR;
    uint32_t CIR;
    uint32_t AHB1RSTR;
    uint32_t AHB2RSTR;
private:
    uint32_t reserved1;
    uint32_t reserved2;
public:
    uint32_t APB1RSTR;
    uint32_t APB2RSTR;
private:
    uint32_t reserved3;
    uint32_t reserved4;
public:
    uint32_t AHB1ENR;
    uint32_t AHB2ENR;
private:
    uint32_t reserved5;
    uint32_t reserved6;
public:
    uint32_t APB1ENR;
    uint32_t APB2ENR;
private:
    uint32_t reserved7;
    uint32_t reserved8;
public:
    uint32_t AHB1LPENR;
    uint32_t AHB2LPENR;
private:
    uint32_t reserved9;
    uint32_t reserved10;
public:
    uint32_t APB1LPENR;
    uint32_t APB2LPENR;
private:
    uint32_t reserved11;
    uint32_t reserved12;
public:
    uint32_t BDCR;
    uint32_t CSR;
private:
    uint32_t reserved13;
    uint32_t reserved14;
public:
    uint32_t SSCGR;
    uint32_t PLLI2SCFGR;
    uint32_t DCKCFGR;
}clock_registers_t;


typedef enum{
    HSI_ON, HSI_RDY, HSI_TRIM  = 3, HSI_CAL = 8,
    HSE_ON = 16, HSE_RDY, HSE_BYP, CSS_ON,
    PLL_ON = 24, PLL_RDY, PLLI2S_ON, PLLI2S_RDY
}RCC_CR_BitPos;

typedef enum{
    PLLM0, PLLM1, PLLM2, PLLM3,
    PLLM4, PLLM5, PLLN, PLLP0 = 16,
    PLLP1, PLLSRC = 22, PLLQ0 = 24,
    PLLQ1, PLLQ2, PLLQ3
}RCC_PLLCFGR_BitPos;

typedef enum{
    SW0, SW1, SWS0, SWS1,
    HPRE, PPRE1 = 10, PPRE2 = 13,
    RTCPRE = 16, MCO1 = 21, I2SSCR = 23,
    MCO1_PRE, MCO2_PRE = 27, MCO2 = 30
}RCC_CFGR_BitPos;

typedef enum{
    LSI_RDYF, LSE_RDYF, HSI_RDYF, HSE_RDYF,
    PLL_RDYF, PLLI2S_RDYF, CSSF = 7, LSI_RDYIE,
    LSE_RDYIE, HSI_RDYIE, HSE_RDYIE, PLL_RDYIE,
    PLLI2S_RDYIE, LSS_RDYC = 16, LSE_RDYC, HSI_RDYC,
    HSE_RDYC, PLL_RDYC, PLLI2S_RDYC, CCSC = 23
}RCC_CIR_BitPos;

typedef enum{
    GPIOA_RST, GPIOB_RST, GPIOC_RST, GPIOD_RST,
    GPIOE_RST, GPIOH_RST = 7, CRCRST = 12, DMA1_RST = 21,
    DMA2_RST
}RCC_AHB1RSTR_BitPos;

typedef enum{
    OTGFS_RST = 7
}RCC_AHB2RSTR_BitPos;

typedef enum{
    TIM2_RST, TIM3_RST, TIM4_RST, TIM5_RST,
    WWDG_RST = 11, SPI2_RST = 14, SPI3_RST,
    USART2_RST = 17, I2C1_RST = 21, I2C2_RST,
    I2C3_RST, PWR_RST = 28 
}RCC_APB1RSTR_BitPos;

typedef enum{
    TIM1_RST, USART1_RST = 4, USART6_RST,
    ADC1_RST = 8, SDIO_RST = 11, SPI1_RST, 
    SPI4_RST, SYSCFG_RST, TIM9_RST = 16, TIM10_RST,
    TIM11_RST
}RCC_APB2RSTR_BitPos;

typedef enum{
    GPIOA_EN, GPIOB_EN, GPIOC_EN, GPIOD_EN,
    GPIOE_EN, GPIOH_EN = 7, CRC_EN = 12, DMA1_EN =21, DMA2_EN
}RCC_AHB1ENR_BitPos;

typedef enum{
    OTGFS_EN = 7
}RCC_AHB2ENR_BitPos;

typedef enum{
    TIM2_EN, TIM3_EN, TIM4_EN, TIM5_EN,
    WWDG_EN = 11, SPI2_EN = 14, SPI3_EN,
    USART2_EN = 17, I2C1_EN = 21, I2C2_EN, I2C3_EN,
    PWR_EN = 28
}RCC_APB1ENR_BitPos;

typedef enum{
    TIM1_EN, USART1_EN = 4, USART6_EN, ADC1_EN = 8,
    SDIO_EN = 11, SPI1_EN, SPI4_EN, SYSCFG_EN,
    TIM9_EN = 16, TIM10_EN, TIM11_EN
}RCC_APB2ENR_BitPos;

typedef enum{
    GPIOA_LPEN, GPIOB_LPEN, GPIOC_LPEN, GPIOD_LPEN, 
    GPIOE_LPEN, GPIOH_LPEN = 7, CRC_LPEN = 12, FLITF_LPEN = 15,
    SRAM1_LPEN, DMA1_LPEN = 21, DMA2_LPEN
}RCC_AHB1LPENR_BitPos;

typedef enum{
    OTGFS_LPEN = 7
}RCC_AHB2LPENR_BitPos;

typedef enum{
    TIM2_LPEN, TIM3_LPEN, TIM4_LPEN, TIM5_LPEN,
    WWDG_LPEN = 11, SPI2_LPEN = 14, SPI3_LPEN,
    USART2_LPEN = 17, I2C1_LPEN = 21, I2C2_LPEN, I2C3_LPEN,
    PWR_LPEN = 28
}RCC_APB1LPENR_BitPos;

typedef enum{
    TIM1_LPEN, USART1_LPEN = 4, USART6_LPEN, ADC1_LPEN = 8,
    SDIO_LPEN = 11, SPI1_LPEN, SPI4_LPEN, SYSCFG_LPEN,
    TIM9_LPEN = 16, TIM10_LPEN, TIM11_LPEN
}RCC_APB2LPENR_BitPos;

typedef enum{
    LSE_ON, LSE_RDY, LSE_BYP, RTC_SEL = 8,
    RTC_EN = 15, BDRST
}RCC_BDCR_BitPos;

typedef enum{
    LSI_ON, LSI_RDY, RMVF = 24, BORRSTF,
    PIN_RSTF, POR_RSTF, SFT_RSTF, IWDF_RSTF, 
    WWDG_RSTF, LPWR_RSTF
}RCC_CSR_BitPos;

typedef enum{
    MODPER, INCSTEP = 13, SPREAD_SEL = 30, SSCG_EN
}RCC_SSCGR_BitPos;


typedef enum{
    PLLI2S_N0 = 6, PLLI2S_N1, PLLI2S_N2, PLLI2S_N3,
    PLLI2S_N4, PLLI2S_N5, PLLI2S_N6, PLLI2S_N7, 
    PLLI2S_N8, PLLI2S_R0 = 28, PLLI2S_R1, PLLI2S_R2
}RCC_PLLI2SCFGR_BitPos;

typedef enum{
    TIMPRE = 24
}RCC_DCKCFGR_BitPos;

/*TODO: Make this structure a singleton*/
struct Clock{
public:
    static volatile clock_registers_t * const registers;

protected:
    Clock() = default;

public:
    /* Create a generic Clock Enable for a peripheral type and then get the index as parameter */
    static void EnableClock_USART2(void);
    static void EnableClock_GPIOA(void);
    static void EnableClock_GPIOB(void);
    static void EnableClock_GPIOC(void);
    static void EnableClock_GPIOD(void);
    static void EnableClock_GPIOE(void);
    static void EnableClock_GPIOH(void);
};

