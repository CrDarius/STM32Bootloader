#pragma once
#include "MCU_Utilities.h"

void JumpToUserApp(void);

const uint32_t SIZE_SECTOR0 = 16384u; /* 16KB */
const uint32_t SIZE_SECTOR1 = 16384u; /* 16KB */
const uint32_t SIZE_SECTOR2 = 16384u; /* 16KB */
const uint32_t SIZE_SECTOR3 = 16384u; /* 16KB */
const uint32_t SIZE_SECTOR4 = 65536u; /* 64KB */
const uint32_t SIZE_SECTOR5 = 131072u; /* 128KB */
const uint32_t SIZE_SECTOR6 = 131072u; /* 128KB */
const uint32_t SIZE_SECTOR7 = 131072u; /* 128KB */
const uint32_t SECTORS_SIZE[FLASH_NUMBER_OF_SECTORS] = {
    16384u, /* 16KB */
    16384u, /* 16KB */
    16384u, /* 16KB */
    16384u, /* 16KB */
    65536u, /* 64KB */
    131072u, /* 128KB */
    131072u, /* 128KB */
    131072u  /* 128KB */
};