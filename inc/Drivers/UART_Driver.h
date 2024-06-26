#pragma once

/* ---Include Section--- */
#include "general_types.h"
#include "Mcu_MemMap.h"
#include "MCU_Utilities.h"
#include "Clock_Driver.h"
#include "GPIO_Driver.h"
#include "SysTick.h"
#include "NVIC_Drivers.h"

#define USART1_INT_POS         37u
#define USART2_INT_POS         38u
#define USART6_INT_POS         71u

#define USART_TX_BUFFER_SIZE   256u
#define USART_RX_BUFFER_SIZE   256u

typedef void (*pCallbackFunc_t) (void);

typedef enum{
    USART_BUSY,
    USART_AVAILABLE
}USART_state_t;

typedef enum{
    WORD_LENTGTH_8BIT,
    WORD_LENTGTH_9BIT
}USART_word_length_t;

typedef enum{
    NO_STOPBITS_1,
    NO_STOPBITS_0_5,
    NO_STOPBITS_2,
    NO_STOPBITS_1_5
}USART_num_stop_bits_t;

typedef enum{
    NO_PARITY,
    ODD_PARITY,
    EVEN_PARITY
}USART_parity_t;

typedef enum{
    FULL_DUPLEX_MODE,
    HALF_DUPLEX_MODE
}USART_operation_mode_t;

typedef enum{
    PE, FE, NF, ORE, IDLE,
    RXNE, TC, TXE, LBD, CTS
}USART_SR_BitPos;

typedef enum{
    FRACTION,
    MANTISSA = 4
}USART_BRR_BitPos;

typedef enum{
    SBK, RWU, RE, TE,
    IDLEIE, RXNEIE, TCIE, TXEIE,
    PEIE, PS, PCE, WAKE,
    M, UE, OVER8 = 15
}USART_CR1_BitPos;

typedef enum{
    ADD, LBDL = 5, LBDIE, LBCL = 8,
    CPHA, CPOL, CLKEN, STOP, LINEN = 14
}USART_CR2_BitPos;

typedef enum{
    EIE, IREN, IRLP, HDSEL,
    NACK, SCEN, DMAR, DMAT, 
    RTSE, CTSE, CTSIE, ONEBIT
}USART_CR3_BitPos;

typedef enum{
    PSC,
    GT = 8
}USART_GTPR_BitPos;


typedef struct{
    uint32_t SR;
    uint32_t DR;
    uint32_t BRR;
    uint32_t CR1;
    uint32_t CR2;
    uint32_t CR3;
    uint32_t GTPR;
}USART_Registers_t;

typedef struct{
public:
    uint16_t mantissa:12;
    uint16_t fraction:4;
    uint32_t baud_rate;
}USART_baudrate_comp_t;

typedef struct{
    uint8_t txBufferUSART[USART_TX_BUFFER_SIZE];
    uint8_t* rxBufferUSART;
    uint8_t idxTXBufferUSART; // buffer index
    uint8_t idxRXBufferUSART; // buffer index
    uint8_t rxLen;
    uint8_t txLen;
}USART_MessageStr_t;

/* ---Functions Section--- */
void USART2_Interrupt(void);


struct USART{
/* Attributes area */ 
public:
    USART_word_length_t word_length;
    USART_num_stop_bits_t no_stop_bits;
    uint32_t baud_rate;
    USART_parity_t parity;
    USART_operation_mode_t operation_mode;
    USART_state_t currentState;
    bool callbackSwitch;
    pCallbackFunc_t pRxCallbackFunc; // pointer to function implemented by application (intended to be used for copying data from USART buffer to application buffer)

private:
    volatile USART_Registers_t * const registers;
    volatile static USART_MessageStr_t USART1_MessageStr;
    volatile static USART_MessageStr_t USART2_MessageStr;
    volatile static USART_MessageStr_t USART6_MessageStr;

/* Methods area */
public:
    USART(volatile USART_Registers_t *usart_address)
        : word_length(WORD_LENTGTH_8BIT), no_stop_bits(NO_STOPBITS_1), baud_rate(9600u),
        parity(NO_PARITY), operation_mode(FULL_DUPLEX_MODE), currentState(USART_AVAILABLE), 
        callbackSwitch(false), pRxCallbackFunc(nullptr), registers(usart_address){}

    OperationStatus_t Init(void);
    OperationStatus_t Config(const USART_word_length_t word_length, const USART_parity_t parity, 
                             const uint32_t baud_rate, const USART_num_stop_bits_t no_stop_bits, 
                             bool callbackSwitch = false, pCallbackFunc_t RxIntCallbackFunc = nullptr);
    OperationStatus_t Transmit(const char *message, uint32_t size, uint32_t waitTime = MAX_DELAY);
    OperationStatus_t TransmitIT(const char *message, uint32_t size, uint32_t waitTime = MAX_DELAY);
    OperationStatus_t Read(uint8_t *buffer, uint32_t size, uint32_t waitTime = MAX_DELAY);
    OperationStatus_t ReadIT(uint8_t *buffer, uint32_t size, uint32_t waitTime = MAX_DELAY);

    friend void USART1_Interrupt(void);
    friend void USART2_Interrupt(void);
    friend void USART6_Interrupt(void);

private:
    OperationStatus_t ConfigBaudRate(const uint32_t baud_rate);
};

extern USART USART1;
extern USART USART2;
extern USART USART6;
