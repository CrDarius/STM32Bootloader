#include "NVIC_Drivers.h"

volatile const NVIC_Registers_t NVIC::registers = {(uint32_t (*)[NrOfNVICIntSetEnableRegs])NVIC_ISER_ADDRESS,
                                                   (uint32_t (*)[NrOfNVICIntClearEnableRegs])NVIC_ICER_ADDRESS,
                                                   (uint32_t (*)[NrOfNVICIntSetPendingRegs])NVIC_ISPR_ADDRESS,
                                                   (uint32_t (*)[NrOfNVICIntClearPendingRegs])NVIC_ICPR_ADDRESS,
                                                   (uint32_t (*)[NrOfNVICIntActivateBitRegs])NVIC_IABR_ADDRESS,
                                                   (uint32_t (*)[NrOfNVICIntPrioRegs])NVIC_IPR_ADDRESS,
                                                   (uint32_t *)NVIC_STIR_ADDRESS};

NVIC& NVIC::get_instance()
{
    static NVIC instance;
    return instance;
}

void NVIC::NVIC_EnableInterrupt(uint8_t IRn)
{
    *(registers.NVIC_ISER)[IRn/32u] |= (1 << (IRn % 32u));
}

void NVIC::NVIC_DisableInterrupt(uint8_t IRn)
{
    *(registers.NVIC_ICER)[IRn/32u] |= (1 << (IRn % 32u));
}

void NVIC::NVIC_SetInterruptPending(uint8_t IRn)
{
    *(registers.NVIC_ISPR)[IRn/32u] |= (1 << (IRn % 32u));
}

void NVIC::NVIC_ClearInterruptPending(uint8_t IRn)
{
    *(registers.NVIC_ICPR)[IRn/32u] |= (1 << (IRn % 32u));
}

bool NVIC::NVIC_GetInterruptPendingStatus(uint8_t IRn)
{
    return (*(registers.NVIC_ISPR)[IRn/32u] & (1 << (IRn % 32u)));
}

void NVIC::NVIC_SetInterruptPriority(uint8_t IRn, uint8_t priority)
{
    *(registers.NVIC_IPR)[IRn/4u] |= (priority << (IRn % 4u));
}

uint8_t NVIC::NVIC_GetInterruptPriority(uint8_t IRn)
{
    return (*(registers.NVIC_IPR)[IRn/4u] & (1 << (IRn % 4u)));
}