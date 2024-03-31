
#include <cstring>
#include "Commands.h"
#include "UART_Driver.h"
#include "Flash_Driver.h"

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

    OperationStatus_t retVal = ST_OK;
    uint8_t positiveResponse = 0xAAu;
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
    uint8_t positiveResponse = 0xAAu;
    uint8_t cmd = 0x20u;
    uint32_t mcu_id = *((uint32_t*)MCUID_REGISTER_ADDRESS);
    uint16_t device_id = ((uint16_t)mcu_id & 0x0FFFu);
    uint16_t revision_id = (uint16_t)(mcu_id >> 16u);
    
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)device_id; idx++;
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)(device_id >> 8u); idx++;
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)revision_id; idx++;
    buffer[NUMBER_CONTROL_BYTES + idx] = (uint8_t)(revision_id >> 8u); idx++;
    buffer[0] = cmd;
    buffer[1] = positiveResponse;
    buffer[2] = idx;
    dataLength = NUMBER_CONTROL_BYTES + idx;

    retVal = USART2.Transmit((const char*)buffer, dataLength, MAX_DELAY);

    return retVal;
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

OperationStatus_t BootReadFlashProtStatus(uint8_t *buffer, uint8_t dataLength)
{
    uint8_t protStatus;

    dataLength = 1u;
    FLASH::ReadProtOptionBytes(protStatus);
    buffer[CMD_POS] = 0x60;
    buffer[RESP_POS] = 0xAA;
    buffer[LEN_POS] = dataLength;
    buffer[LEN_POS + 1] = protStatus;

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

OperationStatus_t JumpToApplication(uint8_t *buffer, uint8_t dataLength)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    return ST_OK;
}