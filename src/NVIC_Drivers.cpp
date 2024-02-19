#include "NVIC_Drivers.h"

volatile NVIC_Registers_t NVIC::registers = {   
                                                (uint32_t *)NVIC_ISER_ADDRESS,
                                                (uint32_t *)NVIC_ICER_ADDRESS,
                                                (uint32_t *)NVIC_ISPR_ADDRESS,
                                                (uint32_t *)NVIC_ICPR_ADDRESS,
                                                (uint32_t *)NVIC_IABR_ADDRESS,
                                                (uint32_t *)NVIC_IPR_ADDRESS,
                                                (uint32_t *)NVIC_STIR_ADDRESS
                                            };


void NVIC::NVIC_EnableInterrupt(uint8_t IRn)
{
    registers.NVIC_ISER[IRn/32u] |= (1u << (IRn % 32u));
}

void NVIC::NVIC_DisableInterrupt(uint8_t IRn)
{
    registers.NVIC_ICER[IRn/32u] |= (1 << (IRn % 32u));
}

void NVIC::NVIC_SetInterruptPending(uint8_t IRn)
{
    registers.NVIC_ISPR[IRn/32u] |= (1 << (IRn % 32u));
}

void NVIC::NVIC_ClearInterruptPending(uint8_t IRn)
{
    registers.NVIC_ICPR[IRn/32u] |= (1 << (IRn % 32u));
}

bool NVIC::NVIC_GetInterruptPendingStatus(uint8_t IRn)
{
    return registers.NVIC_ISPR[IRn/32u] & (1 << (IRn % 32u));
}

void NVIC::NVIC_SetInterruptPriority(uint8_t IRn, uint8_t priority)
{
    registers.NVIC_IPR[IRn/4u] |= (priority << (IRn % 4u));
}

uint8_t NVIC::NVIC_GetInterruptPriority(uint8_t IRn)
{
    return registers.NVIC_IPR[IRn/4u] & (1 << (IRn % 4u));
}

void EnableAllInterrupts() {
    asm volatile("cpsie i");  // Enable interrupts globally
    asm volatile("dsb");      // Data Synchronization Barrier
    asm volatile("isb");      // Instruction Synchronization Barrier
}

void DisableAllInterrupts() {
    asm volatile("cpsid i");  // Disable interrupts globally
    asm volatile("dsb");      // Data Synchronization Barrier
    asm volatile("isb");      // Instruction Synchronization Barrier
}

void SetBasePriority(uint32_t priority) {
    asm volatile("msr basepri, %0" : : "r"(priority));  // Set BASEPRI
    asm volatile("dsb");      // Data Synchronization Barrier
    asm volatile("isb");      // Instruction Synchronization Barrier
}