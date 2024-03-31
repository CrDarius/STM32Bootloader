#pragma once
#include "Mcu_MemMap.h"
#include "general_types.h"
#include "Clock_Driver.h"
#include <cstring>

#define GPIOA         ((volatile GPIO_registers_t*)GPIOA_ADDRESS)
#define GPIOB         ((volatile GPIO_registers_t*)GPIOB_ADDRESS)
#define GPIOC         ((volatile GPIO_registers_t*)GPIOC_ADDRESS)
#define GPIOD         ((volatile GPIO_registers_t*)GPIOD_ADDRESS)
#define GPIOE         ((volatile GPIO_registers_t*)GPIOE_ADDRESS)
#define GPIOH         ((volatile GPIO_registers_t*)GPIOH_ADDRESS)

#define ROFFSET       16u

typedef struct
{
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFRL;
    uint32_t AFRH;
}GPIO_registers_t;

typedef enum
{
    PIN_LOW,
    PIN_HIGH
}PIN_State_t;


typedef enum
{
    INPUT,
    OUTPUT,
    ALTERNATE_FUNCTION,
    ANALOG
}GPIO_port_mode_t;

typedef enum
{
    OUT_PUSHPULL,
    OUT_OPENDRAIN,
}GPIO_port_output_config_t;

typedef enum
{
    LOW_SPEED,
    MEDIUM_SPEED,
    HIGH_SPEED,
    VERY_HIGH_SPEED
}GPIO_port_speed_t;

typedef enum
{
    NO_PULL_UP_DOWN,
    PULL_UP,
    PULL_DOWN,
}GPIO_port_pull_t;

typedef enum
{
    AF0,  AF1,  AF2,  AF3,
    AF4,  AF5,  AF6,  AF7,
    AF8,  AF9,  AF10, AF11,
    AF12, AF13, AF14, AF15
}GPIO_port_alternatefunction_t;

typedef enum
{
    PIN0, PIN1, PIN2, PIN3, PIN4,
    PIN5, PIN6, PIN7, PIN8, PIN9,
    PIN10, PIN11, PIN12, PIN13, PIN14 
}GPIO_pin_t;

typedef enum
{
    MODER0, MODER1 = 2, MODER2 = 4, MODER3 = 6,
    MODER4 = 8, MODER5 = 10, MODER6 = 12, MODER7 = 14,
    MODER8 = 16, MODER9 = 18, MODER10 = 20, MODER11 = 22,
    MODER12 = 24, MODER13 = 26, MODER14 = 28, MODER15 = 30
}GPIO_MODER_BitPos;

typedef enum
{
    OT0, OT1, OT2, OT3, 
    OT4, OT5, OT6, OT7,
    OT8, OT9, OT10, OT11,
    OT12, OT13, OT14, OT15
}GPIO_OTYPER_BitPos;

typedef enum
{
    OSPEEDR0, OSPEEDR1 = 2, OSPEEDR2 = 4, OSPEEDR3 = 6,
    OSPEEDR4 = 8, OSPEEDR5 = 10, OSPEEDR6 = 12, OSPEEDR7 = 14,
    OSPEEDR8 = 16, OSPEEDR9 = 18, OSPEEDR10 = 20, OSPEEDR11 = 22,
    OSPEEDR12 = 24, OSPEEDR13 = 26, OSPEEDR14 = 28, OSPEEDR15 = 30
}GPIO_OSPEEDR_BitPos;

typedef enum
{
    PUPDR0, PUPDR1 = 2, PUPDR2 = 4, PUPDR3 = 6,
    PUPDR4 = 8, PUPDR5 = 10, PUPDR6 = 12, PUPDR7 = 14,
    PUPDR8 = 16, PUPDR9 = 18, PUPDR10 = 20, PUPDR11 = 22,
    PUPDR12 = 24, PUPDR13 = 26, PUPDR14 = 28, PUPDR15 = 30
}GPIO_PUPDR_BitPos;


struct GPIO
{
/* Attributes area */ 
public:
    GPIO_port_mode_t mode;
    GPIO_port_output_config_t output_configuration;
    GPIO_port_speed_t speed;
    GPIO_port_pull_t pull_configuration;
    GPIO_port_alternatefunction_t alternate_function;

private:
    volatile GPIO_registers_t * const registers;
    GPIO_pin_t pin;


/* Methods area */
public:
    GPIO(volatile GPIO_registers_t *GPIO_Address, GPIO_pin_t pin)
        :registers(GPIO_Address), pin(pin){}

    void Config(GPIO_port_mode_t mode, GPIO_port_output_config_t output_configuration = OUT_PUSHPULL, 
                GPIO_port_speed_t speed = LOW_SPEED, GPIO_port_pull_t pull_configuration = NO_PULL_UP_DOWN, 
                GPIO_port_alternatefunction_t alternate_function = AF0);
    void Init(void);
    PIN_State_t GPIO_ReadPin(void) const;
    void GPIO_WritePin(PIN_State_t state);
};

