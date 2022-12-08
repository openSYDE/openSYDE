//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       STW CAN DLL interface definitions

   Header with definitions of types for STW CAN DLL interface

   Provides a standardized reception mechanism for incoming CAN messages.
   Multiple clients can register in order to receive all incoming CAN traffic.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWCAN_H
#define STWCAN_H

#include "stwtypes.hpp"

namespace stw
{
namespace can
{
//we keep the "old" naming conventions and prefixes to signal these are deprecated
///11bit "old" style CAN message
struct STW_CAN_MSG
{
   uint16_t id;     ///< CAN message ID (11bit)
   uint8_t length;  ///< data length code (0..8)
   uint8_t overrun; ///< not used
   uint8_t msg[8];  ///< message data
};

///11bit or 29bit "old" style CAN message
struct STW_CAN_EXTMSG
{
   uint32_t id;     ///< CAN message ID (11bit or 29bit; MSB set -> 29bit)
   uint8_t length;  ///< data length code (0..8)
   uint8_t overrun; ///< not used
   uint8_t msg[8];  ///< message data
};

///11bit or 29bit CAN message structure for received messages
struct T_STWCAN_Msg_RX
{
   uint32_t u32_ID;        ///< CAN message ID
   uint8_t u8_XTD;         ///< 0 -> 11bit ID; 1 -> 29bit ID
   uint8_t u8_DLC;         ///< data length code (0..8)
   uint8_t u8_RTR;         ///< 0 -> regular frame; 1 -> remote frame
   uint8_t u8_Align;       ///< alignment dummy
   uint8_t au8_Data[8];    ///< message data
   uint64_t u64_TimeStamp; ///< message reception time in micro-seconds since system start; 0 -> not available
};

///11bit or 29bit CAN message structure for sending messages
struct T_STWCAN_Msg_TX
{
   uint32_t u32_ID;     ///< CAN message ID
   uint8_t u8_XTD;      ///< 0 -> 11bit ID; 1 -> 29bit ID
   uint8_t u8_DLC;      ///< data length code (0..8)
   uint8_t u8_RTR;      ///< 0 -> regular frame; 1 -> remote frame
   uint8_t u8_Align;    ///< alignment dummy
   uint8_t au8_Data[8]; ///< message data
};

//const int32_t STWCAN_STATUS_FUNC_NOT_SUPPORTED = -1;
const int32_t STATUS_FUNCTION_NOT_SUPPORTED = -1; //use old name for compatibility

///CAN status structure
//use old prefixes to stay compatible with existing applications:
struct T_STWCAN_Status
{
   int32_t iBusOff;
   int32_t iBusWarn;
   int32_t iTXBufCount;
   int32_t iRXBufCount;
   int32_t iTXOverflow;
   int32_t iRXOverflow;
   int32_t iTXOK;    ///< last transmission OK ?
   int32_t iTXFree;  ///< ready to send next message ?
   int32_t iBusload; ///< in 1/1000
   int32_t iActBitrate;
   int32_t iReserved[6];
};

typedef T_STWCAN_Status STW_CAN_STATUS; //we use the same structure in "old" and "ext" DLL functions

///DLL info structure
//use old prefixes to stay compatible with existing applications:
struct T_STWCAN_DLLInfo
{
   uint8_t bXTDSupported; //0/1 bezieht sich auf DLL in Kombination mit Hardware
   //also die 29Bit Faehigkeit
   uint8_t abDLLVersionNumber[5];      ///< not neccessarily zero-terminated
   uint8_t abHardwareVersionNumber[5]; ///< not neccessarily zero-terminated
   uint8_t abDeviceName[20];           ///< not neccessarily zero-terminated
   uint8_t abManufacturerName[20];     ///< not neccessarily zero-terminated
   uint8_t bDLLRelease;
   uint8_t abReserved[12];
};

typedef T_STWCAN_DLLInfo STW_CAN_DLL_INFO; //we use the same structure in "old" and "ext" DLL functions
}
}
#endif
