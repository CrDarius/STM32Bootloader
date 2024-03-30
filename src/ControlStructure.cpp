#include "ControlStructure.h"


void BootControl()
{
    uint8_t cmdCode, dataLength;
    FrameType_t frameType;
    uint8_t cmdIdx; //index of the command inside Commands service table
    uint8_t localBuffer[USART_RX_BUFFER_SIZE];
    uint32_t receivedCRC, computedCRC;

    while(true)
    {
        OperationStatus_t retStatus = ST_OK;
        /* Wait to receive 3 bytes via UART: Frame Type, Command Code, Number of Data Bytes */
        ReceiveCommand(frameType, cmdCode , dataLength);

        /* Read Number of Data Bytes via UART */
        USART2.Read(localBuffer, dataLength, MAX_DELAY);    

        /* Read 4 bytes CRC */
        ReadCRC(receivedCRC);

        /* Compute the CRC on the received message: 1byte Frame Type, 1byte Command Code, 1byte Nr of Data Byted, n bytes of Data */

        /*      CRC valid?                      */
        /*     ______|______                    */
        /* YES|             |NO                 */
        /*    |             |                   */        
        /* Verify CMD       |                   */
        /*    |             |                   */
        /* Process CMD    Send NACK with reason */
        ComputeCRC(frameType, cmdCode, dataLength, localBuffer, computedCRC);

        if(receivedCRC == computedCRC)
        {
            retStatus = VerifyCommand(cmdCode, cmdIdx);
        }
        else
        {
            retStatus = ST_WRONG_CRC;
        }

        if( (retStatus == ST_OK) && (serviceTable[cmdIdx].serviceHandler != nullptr) )
        {
            /* Call the service handler of the command */
            retStatus = serviceTable[cmdIdx].serviceHandler(localBuffer, dataLength);
        }
        if(retStatus != ST_OK)
        {
            /* Command could not be executed successfully. Send negative response */
            USART2.Transmit((const char *)&retStatus, 1, MAX_DELAY);
        }
        else if(frameType == CONSEC_FRAME)
        {
        /* If CMD was processed and Frame type is Consecutive Frame send ACK message to tell the host that a new frame can be received */
            retStatus = ACK;
            USART2.Transmit((const char *)&retStatus, 1, MAX_DELAY);
        }
        else
        {
            /* If last frame was received do nothing. CMD already handled this case */
        }
    }
}



static inline void ReceiveCommand(FrameType_t& frameType, uint8_t& cmdCode, uint8_t& dataLength)
{
    USART2.Read((uint8_t*)&frameType, 1, MAX_DELAY);
    USART2.Read(&cmdCode, 1, MAX_DELAY);
    USART2.Read(&dataLength, 1, MAX_DELAY);
}

static inline void ReadCRC(uint32_t& receivedCRC)
{
    uint8_t crcBuffer[CRC_SIZE];
    USART2.Read(crcBuffer, CRC_SIZE, MAX_DELAY);
    receivedCRC = ( ((uint32_t)crcBuffer[3] << 24u) | ((uint32_t)crcBuffer[2] << 16u) | ((uint32_t)crcBuffer[1] << 8u) | (uint32_t)crcBuffer[0]);
}

static inline void ComputeCRC(const FrameType_t& frameType, const uint8_t& cmdCode, const uint8_t& dataLength, const uint8_t (&localBuffer)[], uint32_t& computedCRC)
{
    CRC::CRC_Compute((const uint8_t*)&frameType, 1u, computedCRC);
    CRC::CRC_ComputeWithAccumulate(&cmdCode, 1u, computedCRC);
    CRC::CRC_ComputeWithAccumulate(&dataLength, 1u, computedCRC);
    CRC::CRC_ComputeWithAccumulate(localBuffer, dataLength, computedCRC);
}

static inline OperationStatus_t VerifyCommand(const uint8_t& cmdCode, uint8_t& cmdIdx)
{
    OperationStatus_t retVal = ST_OK;   
    uint8_t idx;
    
    for(idx = 0; idx < NUMBER_OF_COMMANDS; idx++)
    {
        if(serviceTable[idx].serviceCode == cmdCode)
        {   
            cmdIdx = idx;
            break;
        }
    }
    
    if(idx == NUMBER_OF_COMMANDS)
        retVal = CMD_NOT_FOUND;
    
    return retVal;
}