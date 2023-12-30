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
    uint32_t (*NVIC_ISER)[NrOfNVICIntSetEnableRegs];
    uint32_t (*NVIC_ICER)[NrOfNVICIntClearEnableRegs];
    uint32_t (*NVIC_ISPR)[NrOfNVICIntSetPendingRegs];
    uint32_t (*NVIC_ICPR)[NrOfNVICIntClearPendingRegs];
    uint32_t (*NVIC_IABR)[NrOfNVICIntActivateBitRegs];
    uint32_t (*NVIC_IPR)[NrOfNVICIntPrioRegs];
    uint32_t *STIR;
}NVIC_Registers_t;

struct NVIC
{
private:
    volatile static const NVIC_Registers_t registers; 
    NVIC() = default;

public:
    static NVIC& get_instance();
    void NVIC_EnableInterrupt(uint8_t IRn);
    void NVIC_DisableInterrupt(uint8_t IRn);
    void NVIC_SetInterruptPending(uint8_t IRn);
    void NVIC_ClearInterruptPending(uint8_t IRn);
    bool NVIC_GetInterruptPendingStatus(uint8_t IRn);
    void NVIC_SetInterruptPriority(uint8_t IRn, uint8_t priority);
    uint8_t NVIC_GetInterruptPriority(uint8_t IRn);
};

