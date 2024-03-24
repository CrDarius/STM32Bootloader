#include "GPIO_Driver.h"

void GPIO::Config(GPIO_port_mode_t mode, GPIO_port_output_config_t output_configuration, GPIO_port_speed_t speed, GPIO_port_pull_t pull_configuration, GPIO_port_alternatefunction_t alternate_function)
{
    this->mode = mode;
    this->output_configuration = output_configuration;
    this->speed = speed;
    this->pull_configuration = pull_configuration;
    this->alternate_function = alternate_function;
}

void GPIO::Init(void)
{
    // 1. Enable GPIO clock
    if(this->registers == GPIOA) 
        Clock::EnableClock_GPIOA();
    else if(this->registers == GPIOB)
        Clock::EnableClock_GPIOB();
    else if(this->registers == GPIOC)
        Clock::EnableClock_GPIOC();
    else if(this->registers == GPIOD)
        Clock::EnableClock_GPIOD();
    else if(this->registers == GPIOE)
        Clock::EnableClock_GPIOE();
    else if(this->registers == GPIOH)
        Clock::EnableClock_GPIOH();

    // 2. Set Mode register
    this->registers->MODER |= (this->mode << (this->pin * 2));

    if(this->mode != INPUT)
    {    
        // 3. Set Output type register
        this->registers->OTYPER |= (this->output_configuration << this->pin);

        // 4. Set pin output speed
        this->registers->OSPEEDR |= (this->speed << (this->pin * 2));
    }

    // 5. Set PullUp/PullDown configuration
    this->registers->PUPDR |= (this->pull_configuration << (this->pin * 2));

    if(this->mode == ALTERNATE_FUNCTION)
        // 6. Set Alternate Function register
        this->pin < PIN8 ? (this->registers->AFRL |= this->alternate_function << (this->pin * 4)) : 
                           (this->registers->AFRH |= this->alternate_function << ((this->pin - 8) * 4));
}

PIN_State_t GPIO::GPIO_ReadPin(void) const
{
    if(this->registers->IDR & (1 << this->pin))
        return PIN_HIGH;
    else
        return PIN_LOW;  
}

void GPIO::GPIO_WritePin(PIN_State_t state)
{
    (state == PIN_LOW) ? (this->registers->BSRR |= (1 << (this->pin + ROFFSET))) : (this->registers->BSRR |= (1 << this->pin));
}