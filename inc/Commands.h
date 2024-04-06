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

extern const uint8_t NUMBER_OF_COMMANDS;

typedef struct
{
    uint8_t serviceCode;
    OperationStatus_t (*serviceHandler) (uint8_t *buffer, uint8_t dataLength);   
}Commands_t;

extern Commands_t serviceTable[];

OperationStatus_t BootGetVersion(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootGetMCUID(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootFlashErase(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootFlashWrite(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootFlashVerify(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootReadFlashProtStatus(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootControlRWProt(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootDisableProt(uint8_t *buffer, uint8_t dataLength);
OperationStatus_t BootJumpToApplication(uint8_t *buffer, uint8_t dataLength);
