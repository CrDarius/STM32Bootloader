#pragma once
#include <stdint.h>

/* ARM Cortex Perhiperal Memory Mapping*/
const uint32_t ARM_SYSTICK_ADDRESS = 0xE000E010u;
const uint32_t NVIC_ISER_ADDRESS   = 0xE000E100u;
const uint32_t NVIC_ICER_ADDRESS   = 0xE000E180u;
const uint32_t NVIC_ISPR_ADDRESS   = 0xE000E200u;
const uint32_t NVIC_ICPR_ADDRESS   = 0xE000E280u;
const uint32_t NVIC_IABR_ADDRESS   = 0xE000E300u;
const uint32_t NVIC_IPR_ADDRESS    = 0xE000E400u;
const uint32_t NVIC_STIR_ADDRESS   = 0xE000EF00u;
