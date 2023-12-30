#include <stdio.h>
#include <cstring>
#include "UART_Driver.h"


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

USART_MessageStr_t USART::USART1_MessageStr;
USART_MessageStr_t USART::USART2_MessageStr;
USART_MessageStr_t USART::USART6_MessageStr;

OperationStatus_t USART::Config(const USART_word_length_t word_length, const USART_parity_t parity, const uint32_t baud_rate, const USART_num_stop_bits_t no_stop_bits)
{
    OperationStatus_t retVal = ST_OK;

    // 1. Word length configuration
    this->word_length = word_length;

    // 2. Parity configuration
    this->parity = parity;

    // 3. Configure number of stop bits
    this->no_stop_bits = no_stop_bits;

    // 4. Configure baud rate
    this->baud_rate = baud_rate;


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

    return retVal;
}


OperationStatus_t USART::Print(const char *message, uint32_t size, uint32_t waitTime)
{
    this->currentState = USART_BUSY;
    uint32_t startTime = globalTime;
    uint8_t index = 0;
    OperationStatus_t retVal = ST_OK;
    
    // 1. Set the TE bit in CR1 to send an Idle Frame as first transmission
    this->registers->CR1 |= (1 << USART_CR1_BitPos::TE);

    // 2. Write data to DR register, this will clear TXE bit, repeat for each byte of data
    this->registers->DR = message[index++];
    while(index < size)
    {
        if(DELAY_EXCEEDED(startTime, globalTime, waitTime)) 
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
        if(DELAY_EXCEEDED(startTime, globalTime, waitTime))
        {
            retVal = TIMEOUT;
        }   
    }

    this->currentState = USART_AVAILABLE;

    return retVal;
}

OperationStatus_t USART::PrintIT(const char *message, uint32_t size, uint32_t waitTime)
{
    this->currentState = USART_BUSY;
    OperationStatus_t retVal = ST_OK;
    USART::USART2_MessageStr.i_TXBuffer_USART = 0;
    PARAM_UNUSED(waitTime);

    if( (USART_TX_BUFFER_SIZE < size) && (size > 0) )
        retVal = ST_NOK;
    else
    {
        if(this->registers == USART1)
        {
            memcpy(USART::USART1_MessageStr.TXBuffer_USART, message, size);
            USART::USART1_MessageStr.tx_Len = size;
        }
        else if(this->registers == USART2)
        {
            memcpy(USART::USART2_MessageStr.TXBuffer_USART, message, size);
            USART::USART2_MessageStr.tx_Len = size;
        }
        else if(this->registers == USART6)
        {
            memcpy(USART::USART6_MessageStr.TXBuffer_USART, message, size);
            USART::USART6_MessageStr.tx_Len = size;
        }
        else
        {
            retVal = ST_NOK;
        }

    }

    // Activate transmit register empty interrupt
    this->registers->CR1 |= (1 << USART_CR1_BitPos::TXEIE);
    // Activate transmision complete interrupt
    this->registers->CR1 |= (1 << USART_CR1_BitPos::TCIE);
    // Set the TE bit in CR1 to send an Idle Frame as first transmission
    this->registers->CR1 |= (1 << USART_CR1_BitPos::TE);

    this->registers->DR = USART::USART2_MessageStr.TXBuffer_USART[USART::USART2_MessageStr.i_TXBuffer_USART++];



    return retVal;
}

// There are multiple interrupts multiplexed, make sure to check which interrupt was triggered
void USART2_Interrupt(void)
{
    /* Check which interrupt event triggered the interrupt */
    if (USART2->SR & (1 << USART_SR_BitPos::TXE))
    {
        if(USART::USART2_MessageStr.i_TXBuffer_USART < USART::USART2_MessageStr.tx_Len)
        {
            USART2->DR = USART::USART2_MessageStr.TXBuffer_USART[USART::USART2_MessageStr.i_TXBuffer_USART++];
        }
        else
        {
            /* The entire message has been sent, deactivate further interrupts triggered by TXE */
            USART2->CR1 &= ~(1 << USART_CR1_BitPos::TXEIE);
        }

    }
}