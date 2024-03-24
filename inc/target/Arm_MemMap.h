#pragma once
#include <stdint.h>
/*  -------------------------------------- */
/* | ARM Cortex Perhiperal Memory Mapping |*/
/*  -------------------------------------- */

/* NVIC Memmap */
const uint32_t NVIC_ISER_ADDRESS   = 0xE000E100u;
const uint32_t NVIC_ICER_ADDRESS   = 0xE000E180u;
const uint32_t NVIC_ISPR_ADDRESS   = 0xE000E200u;
const uint32_t NVIC_ICPR_ADDRESS   = 0xE000E280u;
const uint32_t NVIC_IABR_ADDRESS   = 0xE000E300u;
const uint32_t NVIC_IPR_ADDRESS    = 0xE000E400u;
const uint32_t NVIC_STIR_ADDRESS   = 0xE000EF00u;

/* SYSTICK Memmap */
const uint32_t SYST_CSR_ADRESS     = 0xE000E010;
const uint32_t SYST_RVR_ADRESS     = 0xE000E014;
const uint32_t SYST_CVR_ADRESS     = 0xE000E018;
const uint32_t SYST_CALIB_ADRESS   = 0xE000E01C;

/* VECTOR TABLE OFFSET REGISTER */
const uint32_t VTOR_ADDRESS        = 0xE000ED08;