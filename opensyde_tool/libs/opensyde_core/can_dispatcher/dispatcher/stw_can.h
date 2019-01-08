//-----------------------------------------------------------------------------
/*!
   \file
   \brief       STW CAN DLL interface definitions

   Header with definitions of types for STW CAN DLL interface

   Provides a standardized reception mechanism for incoming CAN messages.
   Multiple clients can register in order to receive all incoming CAN traffic.

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.04.2010  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef STWCAN_H
#define STWCAN_H

#include "stwtypes.h"

namespace stw_can
{

//we keep the "old" naming conventions and prefixes to signal these are deprecated
///11bit "old" style CAN message
struct STW_CAN_MSG
{
   stw_types::uint16 id;       ///< CAN message ID (11bit)
   stw_types::uint8  length;   ///< data length code (0..8)
   stw_types::uint8  overrun;  ///< not used
   stw_types::uint8  msg[8];   ///< message data
};

///11bit or 29bit "old" style CAN message
struct STW_CAN_EXTMSG
{
   stw_types::uint32 id;       ///< CAN message ID (11bit or 29bit; MSB set -> 29bit)
   stw_types::uint8  length;   ///< data length code (0..8)
   stw_types::uint8  overrun;  ///< not used
   stw_types::uint8  msg[8];   ///< message data
};

///11bit or 29bit CAN message structure for received messages
struct T_STWCAN_Msg_RX
{
   stw_types::uint32 u32_ID;         ///< CAN message ID
   stw_types::uint8  u8_XTD;         ///< 0 -> 11bit ID; 1 -> 29bit ID
   stw_types::uint8  u8_DLC;         ///< data length code (0..8)
   stw_types::uint8  u8_RTR;         ///< 0 -> regular frame; 1 -> remote frame
   stw_types::uint8  u8_Align;       ///< alignment dummy
   stw_types::uint8  au8_Data[8];    ///< message data
   stw_types::uint64 u64_TimeStamp;  ///< message reception time in micro-seconds since system start; 0 -> not available
};

///11bit or 29bit CAN message structure for sending messages
struct T_STWCAN_Msg_TX
{
   stw_types::uint32 u32_ID;         ///< CAN message ID
   stw_types::uint8  u8_XTD;         ///< 0 -> 11bit ID; 1 -> 29bit ID
   stw_types::uint8  u8_DLC;         ///< data length code (0..8)
   stw_types::uint8  u8_RTR;         ///< 0 -> regular frame; 1 -> remote frame
   stw_types::uint8  u8_Align;       ///< alignment dummy
   stw_types::uint8  au8_Data[8];    ///< message data
};

//const STWTYPES::sint32 STWCAN_STATUS_FUNC_NOT_SUPPORTED = -1;
const stw_types::sint32 STATUS_FUNCTION_NOT_SUPPORTED = -1; //use old name for compatibility

///CAN status structure
//use old prefixes to stay compatible with existing applications:
struct T_STWCAN_Status
{
   stw_types::sint32 iBusOff;
   stw_types::sint32 iBusWarn;
   stw_types::sint32 iTXBufCount;
   stw_types::sint32 iRXBufCount;
   stw_types::sint32 iTXOverflow;
   stw_types::sint32 iRXOverflow;
   stw_types::sint32 iTXOK;        ///< last transmission OK ?
   stw_types::sint32 iTXFree;      ///< ready to send next message ?
   stw_types::sint32 iBusload;     ///< in 1/1000
   stw_types::sint32 iActBitrate;
   stw_types::sint32 iReserved[6];
};

typedef T_STWCAN_Status STW_CAN_STATUS; //we use the same structure in "old" and "ext" DLL functions



///DLL info structure
//use old prefixes to stay compatible with existing applications:
struct T_STWCAN_DLLInfo
{
   stw_types::uint8 bXTDSupported; //0/1 bezieht sich auf DLL in Kombination mit Hardware
                                    //also die 29Bit Faehigkeit
   stw_types::uint8 abDLLVersionNumber[5];      ///< not neccessarily zero-terminated
   stw_types::uint8 abHardwareVersionNumber[5]; ///< not neccessarily zero-terminated
   stw_types::uint8 abDeviceName[20];           ///< not neccessarily zero-terminated
   stw_types::uint8 abManufacturerName[20];     ///< not neccessarily zero-terminated
   stw_types::uint8 bDLLRelease;
   stw_types::uint8 abReserved[12];
};
typedef T_STWCAN_DLLInfo STW_CAN_DLL_INFO; //we use the same structure in "old" and "ext" DLL functions


}

#endif

