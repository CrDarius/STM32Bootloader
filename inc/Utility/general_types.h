#pragma once
#include <stdint.h>

#define PARAM_UNUSED(x)     ((void)x)

#define TRUE    (1 == 1)
#define FALSE   (1 != 1)


typedef enum{
    ST_OK,
    ST_NOK,
    ST_BUSY,
    TIMEOUT, 
    ST_WRONG_CRC,
    CMD_NOT_FOUND,
    ACK = 0xAA
}OperationStatus_t;
