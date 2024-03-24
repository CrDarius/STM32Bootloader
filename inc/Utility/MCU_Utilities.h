#pragma once
#include "SysTick.h"
#include "Bootloader_MemMap.h"

void constexpr SetVTOR(const uint32_t offset);
void inline SetStackPointer(const uint32_t address);

void constexpr SetVTOR(const uint32_t offset)
{
    *(uint32_t*)VTOR_ADDRESS = offset; 
}

void inline SetStackPointer(const uint32_t address)
{
    asm volatile("mov sp, %0" : : "r"(address));
}
