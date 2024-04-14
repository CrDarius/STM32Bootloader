#include <stdint.h>
#include "UART_Driver.h"
#include "CRC_Driver.h"
#include "Commands.h"

extern void BootControl(void);

static inline void ReceiveCommand(FrameType_t& frameType, uint8_t& cmdCode, uint8_t& dataLength);
static inline void ReadCRC(uint32_t& receivedCRC);
static inline void ComputeCRC(const FrameType_t& frameType, const uint8_t& cmdCode, const uint8_t&  dataLength, const uint8_t (&localBuffer)[], uint32_t& computedCRC);
static inline OperationStatus_t VerifyCommand(const uint8_t& cmdCode, uint8_t& cmdIdx);