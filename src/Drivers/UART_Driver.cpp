#include <stdio.h>
#include "m_memory.h"
#include "UART_Driver.h"

USART USART2((USART_Registers_t*)USART2_ADDRESS);
USART USART1((USART_Registers_t*)USART1_ADDRESS);
USART USART6((USART_Registers_t*)USART6_ADDRESS);



static const USART_baudrate_comp_t BaudrateLookUpTable[]=
                                            {
                                                {833, 5,  1200},
                                                {416, 11, 2400},
                                                {104, 3 , 9600},
                                                {52,  1 , 19200},
                                                {26,  1 , 38400},
                                                {17,  6 , 57600},
                                                {8,   11, 115200},
                                                {4,   5 , 230400},
                                                {2,   3 , 460800},
                                                {1,   1 , 921600}
                                            };

volatile USART_MessageStr_t USART::USART1_MessageStr;
volatile USART_MessageStr_t USART::USART2_MessageStr;
volatile USART_MessageStr_t USART::USART6_MessageStr;



OperationStatus_t USART::Config(const USART_word_length_t word_length, const USART_parity_t parity,
                                const uint32_t baud_rate, const USART_num_stop_bits_t no_stop_bits,
                                bool callbackSwitch, pCallbackFunc_t RxIntCallbackFunc)
{
    OperationStatus_t retVal = ST_OK;

    if(this->currentState == USART_AVAILABLE)
    {
        // 1. Word length configuration
        this->word_length = word_length;

        // 2. Parity configuration
        this->parity = parity;

        // 3. Configure number of stop bits
        this->no_stop_bits = no_stop_bits;

        // 4. Configure baud rate
        this->baud_rate = baud_rate;

        // 5. Configure callback function for RX interrupt
        this->callbackSwitch = callbackSwitch;
        this->pRxCallbackFunc = RxIntCallbackFunc;
    }
    else
    {
        retVal = ST_BUSY;
    }

    return retVal;
}

OperationStatus_t USART::ConfigBaudRate(const uint32_t baud_rate)
{
    OperationStatus_t retVal = ST_OK;
    uint8_t index = 0;
    uint16_t BaudrateLookUpTable_length = sizeof(BaudrateLookUpTable)/sizeof(USART_baudrate_comp_t);

    if(   ((Clock::registers->CFGR) & (0b100 << RCC_CFGR_BitPos::PPRE1) ) 
        ||((Clock::registers->CFGR) & (0b1000 << RCC_CFGR_BitPos::HPRE) ) 
        ||(this->registers->CR1    & (1 << USART_CR1_BitPos::OVER8) ) 
        ||(this->currentState == USART_BUSY) )
    {
        /* To be implemented, a function that computes the oversampling and division values dynamically */
        retVal = ST_NOK;
    }
    else
    {
        this->registers->BRR = 0x00;
        while( (BaudrateLookUpTable[index].baud_rate != baud_rate) && (++index < BaudrateLookUpTable_length) );
        if(index == BaudrateLookUpTable_length)
        {
            // This is the default case, the requested baud rate is not supported
            this->registers->BRR |= (BaudrateLookUpTable[2].fraction << USART_BRR_BitPos::FRACTION);
            this->registers->BRR |= (BaudrateLookUpTable[2].mantissa << USART_BRR_BitPos::MANTISSA);
        }
        else
        {
            this->registers->BRR |= (BaudrateLookUpTable[index].fraction << USART_BRR_BitPos::FRACTION);
            this->registers->BRR |= (BaudrateLookUpTable[index].mantissa << USART_BRR_BitPos::MANTISSA);
        }
    }

    return retVal;
}


OperationStatus_t USART::Init(void)
{
    OperationStatus_t retVal = ST_OK;
    if(this->currentState == USART_AVAILABLE)
    {
        // 1. Enable USART Clock
        Clock::EnableClock_USART2();

        // 2. Configure GPIO
        GPIO PA2_USART2TX(GPIOA, PIN2);
        GPIO PA3_USART2RX(GPIOA, PIN3);
        PA2_USART2TX.Config(ALTERNATE_FUNCTION, OUT_PUSHPULL, MEDIUM_SPEED, NO_PULL_UP_DOWN, AF7);
        PA3_USART2RX.Config(ALTERNATE_FUNCTION, OUT_PUSHPULL, MEDIUM_SPEED, NO_PULL_UP_DOWN, AF7);
        PA2_USART2TX.Init();
        PA3_USART2RX.Init();

        // 1. Enable USART by writing UE bit in CR1
        this->registers->CR1 |= (1 << USART_CR1_BitPos::UE);

        // 2. Word length configuration
        if(WORD_LENTGTH_9BIT == this->word_length)
        {
            this->registers->CR1 |= (1 << USART_CR1_BitPos::M);
        }
        else
        {
            this->registers->CR1 &= ~(1 << USART_CR1_BitPos::M);
        }

        // 3. Parity configuration
        if(ODD_PARITY == this->parity)
        {
            this->registers->CR1 |= (1 << USART_CR1_BitPos::PCE); // enable partity
            this->registers->CR1 |= (1 << USART_CR1_BitPos::PS);  // select odd parity
        }
        else if(EVEN_PARITY == this->parity)
        {
            this->registers->CR1 |= (1 << USART_CR1_BitPos::PCE); // enable partity
            this->registers->CR1 &= ~(1 << USART_CR1_BitPos::PS); // select even parity
        }
        else
        {
            this->registers->CR1 &= ~(1 << USART_CR1_BitPos::PCE); // no parity
        }

        // 4. Configure number of stop bits
        this->registers->CR2 &= ~(0b11 << USART_CR2_BitPos::STOP); // clean bits
        this->registers->CR2 |= (this->no_stop_bits << USART_CR2_BitPos::STOP);

        retVal = this->ConfigBaudRate(this->baud_rate); 
    }
    else
    {
        retVal = ST_BUSY;
    }

    return retVal;
}


OperationStatus_t USART::Transmit(const char *message, uint32_t size, uint32_t waitTime)
{
    this->currentState = USART_BUSY;
    uint32_t startTime = SysTick::GetGlobalTime();
    uint8_t index = 0;
    OperationStatus_t retVal = ST_OK;

    // 1. Set the TE bit in CR1 to send an Idle Frame as first transmission
    this->registers->CR1 |= (1 << USART_CR1_BitPos::TE);

    // 2. Write data to DR register, this will clear TXE bit, repeat for each byte of data
    this->registers->DR = message[index++];
    while(index < size)
    {
        if(DELAY_EXCEEDED(startTime, SysTick::GetGlobalTime(), waitTime)) 
        {
            retVal = TIMEOUT;
        }

        if(this->registers->SR & (1 << USART_SR_BitPos::TXE))
        {
            this->registers->DR = message[index++];
        }
    }


    // 3. After last byte is written to DR wait until TC = 1. This will indicate that the transmission of last byte is complete.
    while(!(this->registers->SR & (1 << USART_SR_BitPos::TC)))
    {
        if(DELAY_EXCEEDED(startTime, SysTick::GetGlobalTime(), waitTime))
        {
            retVal = TIMEOUT;
        }   
    }

    this->currentState = USART_AVAILABLE;

    return retVal;
}

OperationStatus_t USART::Read(uint8_t *buffer, uint32_t size, uint32_t waitTime)
{
    this->currentState = USART_BUSY;
    uint32_t startTime = SysTick::GetGlobalTime();
    uint8_t index = 0;
    OperationStatus_t retVal = ST_OK;

    // 1. Set the RE bit to enable the receiver search for a start bit
    this->registers->CR1 |= (1 << USART_CR1_BitPos::RE);

    while(index < size)
    {
        if(DELAY_EXCEEDED(startTime, SysTick::GetGlobalTime(), waitTime)) 
        {
            retVal = TIMEOUT;
        }

        // 2. Wait for a character to be received
        if(this->registers->SR & (1 << USART_SR_BitPos::RXNE))
        {
            buffer[index++] = this->registers->DR;
        }
    }

    // 3. The entire message has been received
    this->currentState = USART_AVAILABLE;

    return retVal;

}


OperationStatus_t USART::TransmitIT(const char *message, uint32_t size, uint32_t waitTime)
{
    this->currentState = USART_BUSY;
    OperationStatus_t retVal = ST_OK;
    PARAM_UNUSED(waitTime);

    if( (USART_TX_BUFFER_SIZE < size) || (0 >= size) )
        retVal = ST_NOK;
    else
    {
        if(this->registers == (USART_Registers_t*)USART1_ADDRESS)
        {
            USART::USART1_MessageStr.idxTXBufferUSART = 0;
            m_memcpy((uint8_t*)USART::USART1_MessageStr.txBufferUSART, (void*)message, size);
            USART::USART1_MessageStr.txLen = size;
        }
        else if(this->registers == (USART_Registers_t*)USART2_ADDRESS)
        {
            USART::USART2_MessageStr.idxTXBufferUSART = 0;
            m_memcpy((uint8_t*)USART::USART2_MessageStr.txBufferUSART, message, size);
            USART::USART2_MessageStr.txLen = size;
        }
        else if(this->registers == (USART_Registers_t*)USART6_ADDRESS)
        {
            USART::USART6_MessageStr.idxTXBufferUSART = 0;
            m_memcpy((uint8_t*)USART::USART6_MessageStr.txBufferUSART, message, size);
            USART::USART6_MessageStr.txLen = size;
        }
        else
        {
            retVal = ST_NOK;
        }

    }

    while (!(this->registers->SR & (1 << USART_SR_BitPos::TXE)));
    // Wait for USART to be ready for transmission (TXE flag)
    
    this->registers->DR = USART::USART2_MessageStr.txBufferUSART[USART::USART2_MessageStr.idxTXBufferUSART++];

    // Activate the NVIC interrupt for USART
    NVIC::NVIC_EnableInterrupt(USART2_INT_POS);

    this->registers->CR1 |= (1 << USART_CR1_BitPos::TXEIE) | // Activate transmit register empty interrupt
                            (1 << USART_CR1_BitPos::TCIE)  | // Activate transmision complete interrupt

                            (1 << USART_CR1_BitPos::TE);     // Set the TE bit in CR1 to send an Idle Frame as first transmission

    return retVal;
}

OperationStatus_t USART::ReadIT(uint8_t *buffer, uint32_t size, uint32_t waitTime)
{
    OperationStatus_t retVal = ST_OK;
    PARAM_UNUSED(waitTime);
    this->currentState = USART_BUSY;

    if( (USART_RX_BUFFER_SIZE < size) && (0 >= size) && (buffer != nullptr))
        retVal = ST_NOK;
    else
    {
        if( this->registers == (USART_Registers_t*)USART1_ADDRESS )
        {
            USART::USART1_MessageStr.rxLen = size;
            USART::USART1_MessageStr.idxRXBufferUSART = 0;
            USART::USART1_MessageStr.rxBufferUSART = buffer;
        }
        else if( this->registers == (USART_Registers_t*)USART2_ADDRESS )
        {
            USART::USART2_MessageStr.rxLen = size;
            USART::USART2_MessageStr.idxRXBufferUSART = 0;
            USART::USART2_MessageStr.rxBufferUSART = buffer;
        }
        else if( this->registers == (USART_Registers_t*)USART6_ADDRESS )
        {
            USART::USART6_MessageStr.rxLen = size;
            USART::USART6_MessageStr.idxRXBufferUSART = 0;
            USART::USART6_MessageStr.rxBufferUSART = buffer;
        }
    }

    // 1. Activate the NVIC interrupt for USART
    NVIC::NVIC_EnableInterrupt(USART2_INT_POS);

    // 2. Enable read data register not empty interrupt RXNEIE
    this->registers->CR1 |= (1 << USART_CR1_BitPos::RXNEIE);

    // 3. Set the RE bit to enable the receiver search for a start bit
    this->registers->CR1 |= (1 << USART_CR1_BitPos::RE);

    return retVal;
}

// There are multiple interrupts multiplexed, make sure to check which interrupt was triggered
void USART2_Interrupt(void)
{
    /* Check which interrupt event triggered the interrupt */
    /* 1. Interrupt was triggered by a sent byte */
    if ( (((USART_Registers_t*)USART2_ADDRESS)->SR & (1 << USART_SR_BitPos::TXE)) && (((USART_Registers_t*)USART2_ADDRESS)->CR1 & (1 << USART_CR1_BitPos::TXEIE)) )
    {
        if(USART::USART2_MessageStr.idxTXBufferUSART < USART::USART2_MessageStr.txLen)
        {
            ((USART_Registers_t*)USART2_ADDRESS)->DR = USART::USART2_MessageStr.txBufferUSART[USART::USART2_MessageStr.idxTXBufferUSART++];
        }
        else
        {
            /* The entire message has been sent, deactivate further interrupts triggered by TXE */
            ((USART_Registers_t*)USART2_ADDRESS)->CR1 &= ~(1 << USART_CR1_BitPos::TXEIE);
            /* Deactivate the NVIC interrupt for USART */
            NVIC::NVIC_DisableInterrupt(USART2_INT_POS);

            USART2.currentState = USART_AVAILABLE;
        }

    }

    /* 2. Interrupt was triggered by a received byte*/
    else if( (((USART_Registers_t*)USART2_ADDRESS)->SR & (1 << USART_SR_BitPos::RXNE)) && (((USART_Registers_t*)USART2_ADDRESS)->CR1 & (1 << USART_CR1_BitPos::RXNEIE)) )
    {
        if(++USART::USART2_MessageStr.idxRXBufferUSART < USART::USART2_MessageStr.rxLen)
            USART::USART2_MessageStr.rxBufferUSART[USART::USART2_MessageStr.idxRXBufferUSART-1] = ((USART_Registers_t*)USART2_ADDRESS)->DR;
        else
        {
            /* Read the last transmitted byte */
            USART::USART2_MessageStr.rxBufferUSART[USART::USART2_MessageStr.idxRXBufferUSART-1] = ((USART_Registers_t*)USART2_ADDRESS)->DR;

            /* The entire message has been received, deactivate further interrupts triggered by RXNEIE*/
            ((USART_Registers_t*)USART2_ADDRESS)->CR1 &= ~(1 << USART_CR1_BitPos::RXNEIE);
            /* Deactivate the NVIC interrupt for USART */
            NVIC::NVIC_DisableInterrupt(USART2_INT_POS);

            USART2.currentState = USART_AVAILABLE;

            if( (USART2.callbackSwitch) && (USART2.pRxCallbackFunc != nullptr) )
                USART2.pRxCallbackFunc();
        }
    }
    
}
