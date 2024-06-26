#pragma once

#include <stdint.h>
#include "general_types.h"

#define NUMBER_CONTROL_BYTES    3u
#define CMD_POS                 0u
#define RESP_POS                1u
#define LEN_POS                 2u
#define DATA_POS                3u
#define POSITIVE_RESP           0xAAu
#define NEGATIVE_RESP           0xFFu

#define CMD_CODE_BOOT_GET_VERSION                   0x10u
#define CMD_CODE_BOOT_GET_MCUID                     0x20u
#define CMD_CODE_BOOT_FLASH_ERASE                   0x30u
#define CMD_CODE_BOOT_FLASH_WRITE                   0x40u
#define CMD_CODE_BOOT_FLASH_VERIFY                  0x50u
#define CMD_CODE_BOOT_READ_FLASH_PROT_STATUS        0x60u
#define CMD_CODE_BOOT_GET_FLASH_RW_PROT             0x70u
#define CMD_CODE_BOOT_JUMP_TO_APP                   0x80u
#define CMD_CODE_BOOT_SET_FLASH_RW_PROT             0x90u

#define CMD_CODE_BOOT_SET_FLASH_RW_PROT_DATALEN     2u


extern const uint8_t NUMBER_OF_COMMANDS;

typedef enum : uint8_t
{
    LAST_FRAME,
    CONSEC_FRAME
}FrameType_t;

typedef enum : uint8_t
{
    Get_BIN_Size,
    Erase_FLASH_Sectors,
    Write_FLASH_Sectors,
    Reset_STATE
}MemWrite_SM_state;

typedef struct
{
    uint8_t serviceCode;
    OperationStatus_t (*serviceHandler) (uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);   
}Commands_t;

extern Commands_t serviceTable[];

OperationStatus_t BootGetVersion(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootGetMCUID(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootFlashErase(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootFlashWrite(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootFlashVerify(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootReadFlashProtStatus(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootGetRWProt(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootSetRWProt(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
OperationStatus_t BootJumpToApplication(uint8_t *buffer, uint8_t dataLength, FrameType_t frameType);
