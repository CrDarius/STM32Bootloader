#pragma once
#include <stdint.h>
#include "Mcu_MemMap.h"
#include "general_types.h"

const uint8_t CRC_SIZE = 4u;

typedef struct
{
    uint32_t CRC_DR;
    uint32_t CRC_IDR;
    uint32_t CRC_CR;
}CRC_Registers_t;

typedef enum
{
    CRC_BUSY,
    CRC_AVAILABLE,
    CRC_NOT_AVAILABLE
}CRC_state_t;

struct CRC
{
private:
    static volatile CRC_Registers_t * const registers;
    CRC() = default;
public:
    static CRC_state_t currentState;

public:
    static void CRC_Init(void);
    static void CRC_Reset(void);
    static void CRC_Read(uint32_t& dest);
    static OperationStatus_t CRC_ComputeWithAccumulate(const uint8_t *src, uint8_t length, uint32_t& dest);
    static OperationStatus_t CRC_Compute(const uint8_t *src, uint8_t length, uint32_t& dest);
};