#pragma once

#include <stdio.h>
#include "general_types.h"
#include "Mcu_MemMap.h"

#define ALL_SECTORS  0xFFu

const uint32_t FLASH_OPTION_BYTES_ADDRESS   = 0x1FFFC000u;

/* Flash keys to allow access to control register */
const uint32_t FLASH_KEY1    = 0x45670123u;
const uint32_t FLASH_KEY2    = 0xCDEF89ABu;

/* Flash keys to allow changes in User Option Bytes */
const uint32_t FLASH_OPTKEY1 = 0x08192A3Bu;
const uint32_t FLASH_OPTKEY2 = 0x4C5D6E7Fu;

typedef struct
{
    uint32_t FLASH_ACR;
    uint32_t FLASH_KEYR;
    uint32_t FLASH_OPTKEYR;
    uint32_t FLASH_SR;
    uint32_t FLASH_CR;
    uint32_t FLASH_OPTCR;
}FLASH_Registers_t;

typedef enum{
    LATENCY, PRFTEN = 8, ICEN, DCEN, 
    ICRST, DCRST
}FLASH_ACR_BitPos;

typedef enum{
    EOP, OPERR, WRPERR = 4, PGAERR,
    PGPERR, PGSERR, RDERR, BSY = 16
}FLASH_SR_BitPos;

typedef enum{
    PG, SER, MER, SNB,
    PSIZE = 8, STRT = 16,
    EOPIE = 24, ERRIE = 25,
    LOCK = 31
}FLASH_CR_BitPos;

typedef enum{
    OPTLOCK, OPTSTRT, BOR_LEV, WDG_SW = 5,
    nRST_STOP, nRST_STBDY, RDP,
    nWRP = 16, SPRMOD = 31
}FLASH_OPTCR_BitPos;

/* State of the Flash Driver */
typedef enum
{
    FLASH_BUSY,
    FLASH_AVAILABLE,
    FLASH_NOT_AVAILABLE
}Flash_state_t;

/* Status of the Flash Operation */
typedef enum
{
    FLASH_OP_BUSY, /* Operation ongoing */
    FLASH_OP_FREE  /* No operation ongoing */
}FlashOP_state_t;

typedef enum
{
    DISABLED,
    ENABLED
}Flash_option_t;

typedef enum
{
    SECTOR0,
    SECTOR1,
    SECTOR2,
    SECTOR3,
    SECTOR4,
    SECTOR5,
    SECTOR6,
    SECTOR7
}Flash_sectornumber_t;

typedef enum
{
    ZERO_WAIT_STATES,
    TWO_WAIT_STATE,
    THREE_WAIT_STATE,
    FOUR_WAIT_STATE,
    FIVE_WAIT_STATE,
    SIX_WAIT_STATE,
    SEVEN_WAIT_STATE,
    EIGHT_WAIT_STATE,
    NINE_WAIT_STATE,
    TEN_WAIT_STATE,
    ELEVEN_WAIT_STATE,
    TWELVE_WAIT_STATE,
    THIRTEEN_WAIT_STATE,
    FOURTEEN_WAIT_STATE,
    FIFTEEN_WAIT_STATE
}Flash_latency_t;

typedef enum
{
    X8,     /* Flash write operations are done on BYTE level */
    X16,    /* Flash write operations are done on HALF-WORD level */
    X32,    /* Flash write operations are done on WORD level */
    X64     /* Flash write operations are done on DOUBLE-WORD level*/
}Flash_parallel_t;

typedef enum
{
    NEW,
    SUCCESSIVE
}Flash_WriteReq_t;


struct FLASH
{
private:
    static volatile FLASH_Registers_t * const registers; 

    /* Pointer used for successive programming of Flash memory */
    static uint32_t *currentProgLocation; 
    static Flash_option_t prefetch;
    static Flash_latency_t latency;
    static Flash_parallel_t paralellism;

private:
    FLASH() = default;
    static OperationStatus_t LockControlRegister(void);
    static OperationStatus_t UnlockControlRegister(void);
    static FlashOP_state_t GetOPStatus(void);

public:
    static Flash_state_t currentState;

public:
    static OperationStatus_t MassErase(void);
    static OperationStatus_t SectorErase(uint8_t& sectorsBitMask);
    static OperationStatus_t WriteFlash(const void* src, void* dest, uint32_t length);
    static void ReadProtOptionBytes(uint8_t& status);
    static void Config(Flash_option_t prefetch, Flash_latency_t latency, Flash_parallel_t paralellism);
    static void Init(void);
};