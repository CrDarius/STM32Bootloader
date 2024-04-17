
#include <cstring>
#include "Commands.h"
#include "UART_Driver.h"
#include "Flash_Driver.h"
#include "BootUtility.h"

static const char *bootloaderVersion = "R24.03";

static inline OperationStatus_t GetSectorsForFlash(uint32_t binSize, uint8_t& sectorsToBeErased);

Commands_t serviceTable[] = 
{
    /* Get Bootloader Version */
    {
        CMD_CODE_BOOT_GET_VERSION,
        BootGetVersion
    },

    /* Get Chip ID */
    {
        CMD_CODE_BOOT_GET_MCUID,
        BootGetMCUID
    },

    /* Erase Flash Memory */
    {
        CMD_CODE_BOOT_FLASH_ERASE,
        BootFlashErase
    },

    /* Program Flash Memory */
    {
        CMD_CODE_BOOT_FLASH_WRITE,
        BootFlashWrite
    },
    
    /* Verify Flash Memory */
    {
        CMD_CODE_BOOT_FLASH_VERIFY,
        BootFlashVerify
    },

    /* Read Protection Status of Flash Memory */
    {
        CMD_CODE_BOOT_READ_FLASH_PROT_STATUS,
        BootReadFlashProtStatus
    },

    /* Get Read/Write Protection of Flash Memory Sectors */
    {
        CMD_CODE_BOOT_GET_FLASH_RW_PROT,
        BootGetRWProt
    },

    {
        CMD_CODE_BOOT_JUMP_TO_APP,
        BootJumpToApplication
    },

    /* Disable Flash Memory Protection */
    {
        CMD_CODE_BOOT_SET_FLASH_RW_PROT,
        BootSetRWProt
    }
};


const uint8_t NUMBER_OF_COMMANDS = ( (sizeof(serviceTable))/(sizeof(Commands_t)) );

OperationStatus_t BootGetVersion(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(frameType);
    OperationStatus_t retVal = ST_OK;
    uint8_t positiveResponse = POSITIVE_RESP;
    uint8_t cmd = CMD_CODE_BOOT_GET_VERSION;
    dataLength = strlen(bootloaderVersion);
    retVal = USART2.Transmit((const char*)&cmd, sizeof(cmd), MAX_DELAY);
    retVal = USART2.Transmit((const char*)&positiveResponse, sizeof(positiveResponse), MAX_DELAY);
    retVal = USART2.Transmit((const char*)&dataLength, sizeof(dataLength), MAX_DELAY);
    retVal = USART2.Transmit(bootloaderVersion, dataLength, MAX_DELAY);

    return retVal;
}

OperationStatus_t BootGetMCUID(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(frameType);
    OperationStatus_t retVal = ST_OK;
    uint8_t idx = 0u;
    uint8_t positiveResponse = POSITIVE_RESP;
    uint8_t cmd = CMD_CODE_BOOT_GET_MCUID;
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

OperationStatus_t BootFlashErase(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(frameType);
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

    buffer[CMD_POS] = CMD_CODE_BOOT_FLASH_ERASE;
    buffer[RESP_POS] = POSITIVE_RESP;
    buffer[LEN_POS] = dataLength;
    dataLength += NUMBER_CONTROL_BYTES;
    
    retVal = USART2.Transmit((const char*)buffer, dataLength, MAX_DELAY);

    return retVal;
}

OperationStatus_t BootFlashWrite(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    OperationStatus_t retVal = ST_OK;

    /* The start address for user application programming is the beginning of sector 2 */
    static uint8_t* progAddress = (uint8_t*)SECTOR2_ADDRESS;
    static MemWrite_SM_state state = Get_BIN_Size;
    static uint8_t sectorsToBeErased = 0u;
    uint32_t binSize = 0u;

    switch (state)
    {
    case Get_BIN_Size:
        /* State 1 - Get total size that will be programmed */
        /*   1. First of all Host will send a consecutive frame containing the size to be programmed */
        binSize = ( (uint32_t)(buffer[0]) | (uint32_t)(buffer[1] << 8u) | (uint32_t)(buffer[2] << 16u) | (uint32_t)(buffer[3] << 24u) );

        /*   2. Based on the received size, the Bootloader will compute how many sectors of memory it needs to erase before starting the programming */
        GetSectorsForFlash(binSize, sectorsToBeErased);

        /*   3. Bootloader will send back to the Host an ack message containing the sectors to be erased */
        buffer[CMD_POS] = CMD_CODE_BOOT_FLASH_WRITE;
        buffer[RESP_POS] = POSITIVE_RESP;
        dataLength = sizeof(sectorsToBeErased);
        buffer[LEN_POS] = dataLength;
        buffer[DATA_POS] = sectorsToBeErased;
        dataLength += NUMBER_CONTROL_BYTES;
        USART2.Transmit((const char *)buffer, dataLength, MAX_DELAY);

        /* State 2 - Erase */
        /*   1. Bootloader starts the erasing procedure */
        retVal = FLASH::SectorErase(sectorsToBeErased);
        /*   2. Host is not allowed to send any data until Bootloader send one more message to ack that the previously sent sectors have been erased */
        if(retVal == ST_OK)
        {
            buffer[CMD_POS] = CMD_CODE_BOOT_FLASH_WRITE;
            buffer[RESP_POS] = POSITIVE_RESP;
            dataLength = sizeof(sectorsToBeErased);
            buffer[LEN_POS] = dataLength;
            buffer[DATA_POS] = sectorsToBeErased;
            dataLength += NUMBER_CONTROL_BYTES;
            USART2.Transmit((const char *)buffer, dataLength, MAX_DELAY);
            state = Write_FLASH_Sectors;
        }
        
        break;

    case Write_FLASH_Sectors:
        /* State 3 - Programming */
        /*   1. Once Host received the ack it will start sending packets of data to be written into memory, max 255 bytes at a time */
        retVal = FLASH::WriteFlash(buffer, progAddress, dataLength);
        progAddress += dataLength;
        /*   2. After each packet the Bootloader needs to send an ack message to the Host to confirm that the packet has been processed */
        if(retVal == ST_OK)
        {
            buffer[CMD_POS] = CMD_CODE_BOOT_FLASH_WRITE;
            buffer[RESP_POS] = POSITIVE_RESP;
            dataLength = 0;
            buffer[LEN_POS] = dataLength;
            dataLength += NUMBER_CONTROL_BYTES;
            USART2.Transmit((const char *)buffer, dataLength, MAX_DELAY);
        }
        if(frameType == LAST_FRAME)
        {
            /* State 4 - Reset to default */
            /*   1. After Bootloader received the last data packet from the Host, it will reset all the parameters to the default value (offset -> 0, progStartAddress...) */
            progAddress = (uint8_t*)SECTOR2_ADDRESS;
            state = Get_BIN_Size;
            sectorsToBeErased = 0u;
        }
        break;

    default:
        break;
    }

    return retVal;
}

OperationStatus_t BootFlashVerify(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(buffer);
    PARAM_UNUSED(dataLength);
    PARAM_UNUSED(frameType);
    return ST_OK;
}

OperationStatus_t BootReadFlashProtStatus(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(frameType);
    uint8_t protStatus;

    dataLength = 1u;
    FLASH::ReadProtOptionBytes(protStatus);
    buffer[CMD_POS] = CMD_CODE_BOOT_READ_FLASH_PROT_STATUS;
    buffer[RESP_POS] = POSITIVE_RESP;
    buffer[LEN_POS] = dataLength;
    buffer[DATA_POS] = protStatus;

    USART2.Transmit((const char*)buffer, dataLength + NUMBER_CONTROL_BYTES, MAX_DELAY);

    return ST_OK;
}

OperationStatus_t BootGetRWProt(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(frameType);
    OperationStatus_t retVal = ST_OK;
    Flash_ProtLevel_t prot;
    dataLength = FLASH_NUMBER_OF_SECTORS;

    for(uint8_t sector = SECTOR0; sector <= SECTOR7; sector++)
    {
        retVal = FLASH::GetSectorRWProtection(sector, prot);
        buffer[DATA_POS + sector] = prot;
        if(retVal != ST_OK)
            break;
    }

    if(retVal == ST_OK)
    {
        dataLength += NUMBER_CONTROL_BYTES;
        buffer[CMD_POS] = CMD_CODE_BOOT_GET_FLASH_RW_PROT;
        buffer[RESP_POS] = POSITIVE_RESP;
        buffer[LEN_POS] = dataLength;
        USART2.Transmit((const char*)buffer, dataLength, MAX_DELAY);
    }
    else
    {

        dataLength = 0;
        buffer[CMD_POS] = CMD_CODE_BOOT_GET_FLASH_RW_PROT;
        buffer[RESP_POS] = NEGATIVE_RESP;
        buffer[LEN_POS] = dataLength;
        USART2.Transmit((const char*)buffer, dataLength, MAX_DELAY);
    }

    return retVal;
}

OperationStatus_t BootSetRWProt(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(frameType);
    OperationStatus_t retVal = ST_OK;
    uint8_t protectionMode = 0;
    uint8_t sectorBitMask = 0;

    protectionMode = buffer[0];
    sectorBitMask = buffer[1];

    retVal = FLASH::SetFlashProtectionMode((Flash_ProtMode_t)protectionMode);
    if(retVal == ST_OK)
    {
        retVal = FLASH::SetSectorRWProtection(sectorBitMask);
    }
    
    if(retVal == ST_OK)
    {
        dataLength = 0;
        buffer[CMD_POS] = CMD_CODE_BOOT_GET_FLASH_RW_PROT;
        buffer[RESP_POS] = POSITIVE_RESP;
        buffer[LEN_POS] = dataLength;
        USART2.Transmit((const char*)buffer, NUMBER_CONTROL_BYTES, MAX_DELAY);
    }
    else
    {

        dataLength = 0;
        buffer[CMD_POS] = CMD_CODE_BOOT_GET_FLASH_RW_PROT;
        buffer[RESP_POS] = NEGATIVE_RESP;
        buffer[LEN_POS] = dataLength;
        USART2.Transmit((const char*)buffer, NUMBER_CONTROL_BYTES, MAX_DELAY);
    }

    return retVal;
}

OperationStatus_t BootJumpToApplication(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType)
{
    PARAM_UNUSED(frameType);
    dataLength = 0u;
    buffer[CMD_POS] = CMD_CODE_BOOT_JUMP_TO_APP;
    buffer[RESP_POS] = POSITIVE_RESP;
    buffer[LEN_POS] = dataLength;

    USART2.Transmit((const char*)buffer, NUMBER_CONTROL_BYTES, MAX_DELAY);

    JumpToUserApp();

    return ST_OK;
}

static inline OperationStatus_t GetSectorsForFlash(uint32_t binSize, uint8_t& sectorsToBeErased)
{
    OperationStatus_t retVal = ST_OK;
    uint32_t totalSizeRequired = 0u;
    uint8_t sector = 0u;

    sectorsToBeErased = 0u;
    for(sector = SECTOR2; sector <= SECTOR7; sector++)
    {
        sectorsToBeErased |= (1u << sector);
        totalSizeRequired += SECTORS_SIZE[sector];
        if(binSize < totalSizeRequired)
            break;
    }

    if(sector > SECTOR7)
        retVal = ST_NOK;

    return retVal;
}