
#include <cstring>
#include "Commands.h"
#include "UART_Driver.h"
#include "Flash_Driver.h"
#include "BootUtility.h"

static const char *bootloaderVersion = "R24.03";

Commands_t serviceTable[] = 
{
    /* Get Bootloader Version */
    {
        0x10,
        BootGetVersion
    },

    /* Get Chip ID */
    {
        0x20,
        BootGetMCUID
    },

    /* Erase Flash Memory */
    {
        0x30,
        BootFlashErase
    },

    /* Program Flash Memory */
    {
        0x40,
        BootFlashWrite
    },
    
    /* Verify Flash Memory */
    {
        0x50,
        BootFlashVerify
    },

    /* Read Status of Flash Memory Sector */
    {
        0x60,
        BootReadFlashProtStatus
    },

    /* Control Read/Write Protection of Flash Memory */
    {
        0x70,
        BootControlRWProt
    },

    {
        0x80,
        BootJumpToApplication
    },

    /* Disable Flash Memory Protection */
    {
        0x90,
        BootDisableProt
    }
};


const uint8_t NUMBER_OF_COMMANDS = ( (sizeof(serviceTable))/(sizeof(Commands_t)) );

OperationStatus_t BootGetVersion(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);

    OperationStatus_t retVal = ST_OK;
    uint8_t positiveResponse = POSITIVE_RESP;
    uint8_t cmd = 0x10u;
    dataLength = strlen(bootloaderVersion);
    retVal = USART2.Transmit((const char*)&cmd, sizeof(cmd), MAX_DELAY);
    retVal = USART2.Transmit((const char*)&positiveResponse, sizeof(positiveResponse), MAX_DELAY);
    retVal = USART2.Transmit((const char*)&dataLength, sizeof(dataLength), MAX_DELAY);
    retVal = USART2.Transmit(bootloaderVersion, dataLength, MAX_DELAY);

    return retVal;
}

OperationStatus_t BootGetMCUID(uint8_t *buffer, uint8_t dataLength)
{
    OperationStatus_t retVal = ST_OK;
    uint8_t idx = 0u;
    uint8_t positiveResponse = POSITIVE_RESP;
    uint8_t cmd = 0x20u;
    uint32_t mcu_id = *((uint32_t*)MCUID_REGISTER_ADDRESS);
    uint16_t device_id = ((uint16_t)mcu_id & 0x0FFFu);
    uint16_t revision_id = (uint16_t)(mcu_id >> 16u);
    
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)device_id; idx++;
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)(device_id >> 8u); idx++;
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)revision_id; idx++;
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)(revision_id >> 8u); idx++;
    buffer[CMD_POS] = cmd;
    buffer[RESP_POS] = positiveResponse;
    buffer[LEN_POS] = idx;
    dataLength = NUMBER_CONTROL_BYTES + idx;

    retVal = USART2.Transmit((const char*)buffer, dataLength, MAX_DELAY);

    return retVal;
}

OperationStatus_t BootFlashErase(uint8_t *buffer, uint8_t dataLength)
{
    OperationStatus_t retVal = ST_OK;

    /* Extract Sector to be erased */
    uint8_t sectors = buffer[0];
    
    if(sectors == ALL_SECTORS)
    {
        retVal = FLASH::MassErase();

        /* If this point is reached mass erase was not performed successfully */
        return retVal;
    }
    
    /* Call SectorErase from FLASH to erase the bit encoded sectors */
    retVal = FLASH::SectorErase(sectors);

    if(retVal == ST_OK)
    {
        for(dataLength = 0; dataLength < FLASH_NUMBER_OF_SECTORS; dataLength++)
        {
            /* Check if the sector was requested */
            if((sectors >> dataLength) % 2u == 1u)
                buffer[DATA_POS + dataLength] = POSITIVE_RESP;
            else
                buffer[DATA_POS + dataLength] = 0x00;
        }
    }
    else
    {
        for(dataLength = 0; dataLength < FLASH_NUMBER_OF_SECTORS; dataLength++)
        {
            /* Check if the sector was requested */
            if((sectors >> dataLength) % 2u == 1u)
                buffer[DATA_POS + dataLength] = NEGATIVE_RESP;
            else
                buffer[DATA_POS + dataLength] = 0x00;
        }
    }

    buffer[CMD_POS] = 0x30;
    buffer[RESP_POS] = POSITIVE_RESP;
    buffer[LEN_POS] = dataLength;
    dataLength += NUMBER_CONTROL_BYTES;
    
    retVal = USART2.Transmit((const char*)buffer, dataLength, MAX_DELAY);

    return retVal;
}

OperationStatus_t BootFlashWrite(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    /* Perform erase before writing the memory. Without prior erase the memory will not be correctly written */
    return ST_OK;
}

OperationStatus_t BootFlashVerify(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootReadFlashProtStatus(uint8_t *buffer, uint8_t dataLength)
{
    uint8_t protStatus;

    dataLength = 1u;
    FLASH::ReadProtOptionBytes(protStatus);
    buffer[CMD_POS] = 0x60;
    buffer[RESP_POS] = POSITIVE_RESP;
    buffer[LEN_POS] = dataLength;
    buffer[DATA_POS] = protStatus;

    USART2.Transmit((const char*)buffer, dataLength + NUMBER_CONTROL_BYTES, MAX_DELAY);

    return ST_OK;
}

OperationStatus_t BootControlRWProt(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootDisableProt(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootJumpToApplication(uint8_t *buffer, uint8_t dataLength)
{
    dataLength = 0u;
    buffer[CMD_POS] = 0x80;
    buffer[RESP_POS] = POSITIVE_RESP;
    buffer[LEN_POS] = dataLength;

    USART2.Transmit((const char*)buffer, NUMBER_CONTROL_BYTES, MAX_DELAY);

    JumpToUserApp();

    return ST_OK;
}