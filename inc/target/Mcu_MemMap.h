#pragma once

#include <stdint.h>

/* MCU-ID Register*/
const uint32_t MCUID_REGISTER_ADDRESS = 0xE0042000u;

/* MCU Peripheral Memory Mapping */
const uint32_t FLASH_INTERFACE_ADDRESS = 0x40023C00u;
const uint32_t RCC_ADDRESS    = 0x40023800u;
const uint32_t CRC_ADDRESS    = 0x40023000u;
const uint32_t GPIOA_ADDRESS  = 0x40020000u;
const uint32_t GPIOB_ADDRESS  = 0x40020400u;
const uint32_t GPIOC_ADDRESS  = 0x40020800u;
const uint32_t GPIOD_ADDRESS  = 0x40020C00u;
const uint32_t GPIOE_ADDRESS  = 0x40021000u;
const uint32_t GPIOH_ADDRESS  = 0x40021C00u;
const uint32_t EXTI_ADDRESS   = 0x40013C00u;
const uint32_t SYSCFG_ADDRESS = 0x40013800u;
const uint32_t USART2_ADDRESS = 0x40004400u;
const uint32_t USART1_ADDRESS = 0x40011000u;
const uint32_t USART6_ADDRESS = 0x40011400u;

/* MCU FLASH ORGANIZATION */
const uint32_t FLASH_NUMBER_OF_SECTORS = 8u;
const uint32_t SECTOR0_ADDRESS = 0x08000000u;    /* 16KB */
const uint32_t SECTOR1_ADDRESS = 0x08004000u;    /* 16KB */
const uint32_t SECTOR2_ADDRESS = 0x08008000u;    /* 16KB */
const uint32_t SECTOR3_ADDRESS = 0x0800C000u;    /* 16KB */
const uint32_t SECTOR4_ADDRESS = 0x08010000u;    /* 64KB */
const uint32_t SECTOR5_ADDRESS = 0x08020000u;    /* 128KB */
const uint32_t SECTOR6_ADDRESS = 0x08040000u;    /* 128KB */
const uint32_t SECTOR7_ADDRESS = 0x08060000u;    /* 128KB */
