#pragma once

#include <stdint.h>
#include "Mcu_MemMap.h"

#define USER_APP_RESET_HANDLER_ADDRESS  (*( (uint32_t*)((USER_APP_VT_ADDRESS) + (4u)) ) )

const uint32_t BOOTLOADER_VT_ADDRESS = SECTOR0_ADDRESS;
const uint32_t USER_APP_VT_ADDRESS = SECTOR2_ADDRESS;

