#pragma once
#include <stdint.h>
#include "general_types.h"
#include "Arm_MemMap.h"

/* Number of NVIC Interrupt Set-Enable Registers */
const uint8_t NrOfNVICIntSetEnableRegs = 8;

/* Number of NVIC Interrupt Clear-Enable Registers */
const uint8_t NrOfNVICIntClearEnableRegs = 8;

/* Number of NVIC Interrupt Set-Pending Registers */
const uint8_t NrOfNVICIntSetPendingRegs = 8;

/* Number of NVIC Interrupt Clear-Pending Registers */
const uint8_t NrOfNVICIntClearPendingRegs = 8;

/* Number of NVIC Interrupt Active Bit Register */
const uint8_t NrOfNVICIntActivateBitRegs = 8;

/* Number of NVIC Interrupt Priority Register */
const uint8_t NrOfNVICIntPrioRegs = 60;

typedef struct 
{
    uint32_t * const NVIC_ISER;
    uint32_t * const NVIC_ICER;
    uint32_t * const NVIC_ISPR;
    uint32_t * const NVIC_ICPR;
    uint32_t * const NVIC_IABR;
    uint32_t * const NVIC_IPR;
    uint32_t * const STIR;
}NVIC_Registers_t;

struct NVIC
{
private:
    static volatile NVIC_Registers_t registers; 
    NVIC() = default;

public:
    static void NVIC_EnableInterrupt(uint8_t IRn);
    static void NVIC_DisableInterrupt(uint8_t IRn);
    static void NVIC_SetInterruptPending(uint8_t IRn);
    static void NVIC_ClearInterruptPending(uint8_t IRn);
    static bool NVIC_GetInterruptPendingStatus(uint8_t IRn);
    static void NVIC_SetInterruptPriority(uint8_t IRn, uint8_t priority);
    static uint8_t NVIC_GetInterruptPriority(uint8_t IRn);
};

void EnableAllInterrupts();
void DisableAllInterrupts();
void SetBasePriority(uint32_t priority);
