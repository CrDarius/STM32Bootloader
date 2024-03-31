#include "CRC_Driver.h"
#include "Clock_Driver.h"

volatile CRC_Registers_t * const CRC::registers = (volatile CRC_Registers_t*)CRC_ADDRESS;
CRC_state_t CRC::currentState = CRC_NOT_AVAILABLE;

void CRC::CRC_Init(void)
{
    /* Enable the Clock for CRC Peripheral */
    Clock::EnableClock_CRC();

    /* Reset the data regiter contents */
    CRC_Reset();

    CRC::currentState = CRC_AVAILABLE;
}

void CRC::CRC_Reset(void)
{
    CRC::registers->CRC_CR |= 1;
}

void CRC::CRC_Read(uint32_t& dest)
{
    dest = CRC::registers->CRC_DR;
}

/* This computation will also include the current data in the data register */
OperationStatus_t CRC::CRC_ComputeWithAccumulate(const uint8_t *src, const uint8_t length, uint32_t& dest)
{
    if(CRC::currentState != CRC_AVAILABLE)
        return ST_BUSY;
    
    CRC::currentState = CRC_BUSY;
    
    for(uint16_t i = 0; i < length; i++)
        CRC::registers->CRC_DR = (uint32_t)src[i];

    dest = CRC::registers->CRC_DR;

    CRC::currentState = CRC_AVAILABLE;

    return ST_OK;
}

/* This computation will only include the data passed in src */
OperationStatus_t CRC::CRC_Compute(const uint8_t *src, const uint8_t length, uint32_t& dest)
{
    if(CRC::currentState != CRC_AVAILABLE)
        return ST_BUSY;
    

    CRC::currentState = CRC_BUSY;

    CRC::CRC_Reset();
    for(uint16_t i = 0; i < length; i++)
        CRC::registers->CRC_DR = (uint32_t)src[i];

    dest = CRC::registers->CRC_DR;

    CRC::currentState = CRC_AVAILABLE;

    return ST_OK;
}