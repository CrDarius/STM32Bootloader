
#include <cstring>
#include "Commands.h"
#include "UART_Driver.h"

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
        BootGetCID
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
        BootReadSectorStatus
    },

    /* Control Read/Write Protection of Flash Memory */
    {
        0x70,
        BootControlRWProt
    },

    {
        0x80,
        JumpToApplication
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
    PARAM_UNUSED(dataLength);
    uint8_t positiveResponse = 0xAA;
    uint8_t cmd = 0x10;
    dataLength = strlen(bootloaderVersion);
    USART2.Transmit((const char*)&cmd, sizeof(cmd), MAX_DELAY);
    USART2.Transmit((const char*)&positiveResponse, sizeof(positiveResponse), MAX_DELAY);
    USART2.Transmit((const char*)&dataLength, sizeof(dataLength), MAX_DELAY);
    USART2.Transmit(bootloaderVersion, dataLength, MAX_DELAY);
    return ST_OK;
}

OperationStatus_t BootGetCID(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootFlashErase(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootFlashWrite(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootFlashVerify(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}

OperationStatus_t BootReadSectorStatus(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
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

OperationStatus_t JumpToApplication(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}