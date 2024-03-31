#include "Flash_Driver.h"

volatile FLASH_Registers_t * const FLASH::registers = (volatile FLASH_Registers_t * )FLASH_INTERFACE_ADDRESS;

Flash_state_t FLASH::currentState = FLASH_AVAILABLE;
Flash_option_t FLASH::prefetch = DISABLED;
Flash_latency_t FLASH::latency = ZERO_WAIT_STATES;
Flash_parallel_t FLASH::paralellism = X8;

FlashOP_state_t FLASH::GetOPStatus(void)
{
    return ((FLASH::registers->FLASH_SR & (1u << FLASH_SR_BitPos::BSY)) != 0) ? FLASH_OP_BUSY : FLASH_OP_FREE;
}

OperationStatus_t FLASH::UnlockControlRegister()
{
    OperationStatus_t retVal = ST_OK;
    if((FLASH::GetOPStatus() == FLASH_OP_FREE))
    {   
        FLASH::registers->FLASH_KEYR = FLASH_KEY1;
        FLASH::registers->FLASH_KEYR = FLASH_KEY2;
    }
    else
    {
        retVal = ST_BUSY;
    }
    
    return retVal;
}

OperationStatus_t FLASH::LockControlRegister()
{
    OperationStatus_t retVal = ST_OK;

    if( (FLASH::GetOPStatus() == FLASH_OP_FREE) )
    {
        FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::LOCK);
    }
    else
    {
        retVal = ST_BUSY;
    }

    return retVal;
}

OperationStatus_t FLASH::MassErase()
{
    OperationStatus_t retVal = ST_OK;

    /* Check that no operation is ongoing */
    if( (FLASH::currentState == FLASH_AVAILABLE) && (FLASH::GetOPStatus() == FLASH_OP_FREE) )
    {
        FLASH::currentState = FLASH_BUSY;

        (void)FLASH::UnlockControlRegister();

        /* Select Mass Erase operation */
        FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::MER);

        /* Start the operation */
        FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::STRT);

        /* Wait until current operation is done */
        while(FLASH::GetOPStatus() != FLASH_OP_FREE){}

        /* Deselect Sector Erase operation */
        FLASH::registers->FLASH_CR &= ~(1u << FLASH_CR_BitPos::MER);

        (void)FLASH::LockControlRegister();

        FLASH::currentState = FLASH_AVAILABLE;
    }
    else
    {
        retVal = ST_BUSY;
    }

    return retVal;
}

/* Erase one or multiple sectors. Sectors are bit encoded in the passed parameter */
OperationStatus_t FLASH::SectorErase(uint8_t sectorsBitMask)
{
    OperationStatus_t retVal = ST_OK;

    /* Check that no operation is ongoing */
    if( (FLASH::currentState == FLASH_AVAILABLE) && (FLASH::GetOPStatus() == FLASH_OP_FREE) )
    {
        FLASH::currentState = FLASH_BUSY;

        (void)FLASH::UnlockControlRegister();

        /* Select Sector Erase operation */
        FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::SER);

        for(uint8_t sectorIdx = SECTOR0; sectorIdx < FLASH_NUMBER_OF_SECTORS; sectorIdx++)
        {
            if(sectorsBitMask & (1u << sectorIdx))
            {
                /* Clear sector number register */
                FLASH::registers->FLASH_CR &= ~(0b1111u << FLASH_CR_BitPos::SNB);

                /* Select the sector to be erased */
                FLASH::registers->FLASH_CR |= (sectorIdx << FLASH_CR_BitPos::SNB);

                /* Start the operation */
                FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::STRT);

                /* Wait until operation is finished */
                while(FLASH::GetOPStatus() != FLASH_OP_FREE){}
            }
        }

        /* Deselect Sector Erase operation */
        FLASH::registers->FLASH_CR &= ~(1u << FLASH_CR_BitPos::SER);

        /* Clear sector number register */
        FLASH::registers->FLASH_CR &= ~(0b1111u << FLASH_CR_BitPos::SNB);

        (void)FLASH::LockControlRegister();

        FLASH::currentState = FLASH_AVAILABLE;
    }
    else
    {
        retVal = ST_BUSY;
    }

    return retVal;
}

/*
    Description: Write Flash memory
    src: pointer to the beginning of a block that needs to be written in memory
    dest: pointer to the destination in Flash memory where the data from src will be written
    length: number of bytes to be written from src in flash memory at dest
            length must be multiple of:
                - 1 in case selected paralellism is X8  (write by byte)
                - 2 in case selected paralellism is x16 (write by half-word)
                - 4 in case selected paralellism is x32 (write by word)
                - 8 in case selected paralellism is x64 (write by double-word)  
*/
OperationStatus_t FLASH::WriteFlash(const void* src, void* dest, uint32_t length)
{
    OperationStatus_t retVal = ST_OK;

    /* Check that no operation is ongoin */
    if( (FLASH::currentState == FLASH_AVAILABLE) && (FLASH::GetOPStatus() == FLASH_OP_FREE) )
    {
        FLASH::currentState = FLASH_BUSY;

        /* Verify if requested length is valid based on selected paralellism */
        switch (FLASH::paralellism)
        {
            case X16:
                if(length % 2u == 0u)
                {
                    length = length / 2u;

                    /* Unlock the control register */
                    (void)FLASH::UnlockControlRegister();

                    /* Select program operation */
                    FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::PG);

                    for(uint32_t idx = 0u; idx < length; idx++)
                    {
                        /* Perform memory write operation */
                        *(((uint16_t*)dest) + idx) = *(((uint16_t*)src) + idx);

                        /* Wait for operation to complete */
                        while(FLASH::GetOPStatus() != FLASH_OP_FREE){}
                    }
                }
                else
                    retVal = ST_NOK;
                break;

            case X32:
                if(length % 4u == 0u)
                {
                    length = length / 4u;

                    /* Unlock the control register */
                    (void)FLASH::UnlockControlRegister();

                    /* Select program operation */
                    FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::PG);

                    for(uint32_t idx = 0u; idx < length; idx++)
                    {
                        /* Perform memory write operation */
                        *(((uint32_t*)dest) + idx) = *(((uint32_t*)src) + idx);

                        /* Wait for operation to complete */
                        while(FLASH::GetOPStatus() != FLASH_OP_FREE){}
                    }

                }
                else
                    retVal = ST_NOK;
                break;

            case X64:
                if(length % 8u == 0u)
                {
                    length = length / 8u;

                    /* Unlock the control register */
                    (void)FLASH::UnlockControlRegister();

                    /* Select program operation */
                    FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::PG);

                    for(uint32_t idx = 0u; idx < length; idx++)
                    {
                        /* Perform memory write operation */
                        *(((uint64_t*)dest) + idx) = *(((uint64_t*)src) + idx);

                        /* Wait for operation to complete */
                        while(FLASH::GetOPStatus() != FLASH_OP_FREE){}
                    }

                }
                else
                    retVal = ST_NOK;
                break;

            default/*X8*/: 
                /* Unlock the control register */
                (void)FLASH::UnlockControlRegister();

                /* Select program operation */
                FLASH::registers->FLASH_CR |= (1u << FLASH_CR_BitPos::PG);

                for(uint32_t idx = 0u; idx < length; idx++)
                {
                    /* Perform memory write operation */
                    *(((uint8_t*)dest) + idx) = *(((uint8_t*)src) + idx);

                    /* Wait for operation to complete */
                    while(FLASH::GetOPStatus() != FLASH_OP_FREE){}
                }
                break;
        }

        if(retVal == ST_OK)
        {
            /* Deselect the program operation */
            FLASH::registers->FLASH_CR &= ~(1u << FLASH_CR_BitPos::PG);

            /* Lock the control register */
            (void)FLASH::LockControlRegister();
        }
        
        FLASH::currentState = FLASH_AVAILABLE;

    }
    else
    {
        retVal = ST_BUSY;
    }

    return retVal;
}

void FLASH::ReadProtOptionBytes(uint8_t& status)
{
    status = (uint8_t)(*((uint32_t*)FLASH_OPTION_BYTES_ADDRESS) >> 8u);
}

void FLASH::Config(Flash_option_t prefetch, Flash_latency_t latency, Flash_parallel_t paralellism)
{
    FLASH::prefetch = prefetch;
    FLASH::latency = latency;
    FLASH::paralellism = paralellism;
}

void FLASH::Init()
{
    FLASH::registers->FLASH_ACR |= (latency << FLASH_ACR_BitPos::LATENCY);
    FLASH::registers->FLASH_ACR |= (prefetch << FLASH_ACR_BitPos::PRFTEN);
    FLASH::currentState = FLASH_AVAILABLE;
}
