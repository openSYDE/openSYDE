//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for logging CAN messages (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGERDATA_H
#define C_OSCCOMMESSAGELOGGERDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <algorithm>

#include "stwtypes.hpp"
#include "stw_can.hpp"
#include "C_SclString.hpp"
#include "C_OscCanSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComMessageLoggerDataSignal
{
public:
   C_OscComMessageLoggerDataSignal(void);

   bool operator <(const C_OscComMessageLoggerDataSignal & orc_Cmp) const;

   stw::scl::C_SclString c_Name;
   stw::scl::C_SclString c_Value;
   stw::scl::C_SclString c_Unit;
   stw::scl::C_SclString c_RawValueDec;
   stw::scl::C_SclString c_RawValueHex;
   stw::scl::C_SclString c_Comment;
   bool q_DlcError;

   stw::opensyde_core::C_OscCanSignal c_OscSignal;
};

class C_OscComMessageLoggerData
{
public:
   C_OscComMessageLoggerData(void);
   virtual ~C_OscComMessageLoggerData(void);

   void SortSignals(void);
   static stw::scl::C_SclString h_GetTimestampAsString(const uint64_t ou64_TimeStamp);

   uint64_t u64_TimeStampRelative;
   uint64_t u64_TimeStampAbsoluteStart;
   uint64_t u64_TimeStampAbsoluteTimeOfDay;
   stw::scl::C_SclString c_TimeStampRelative;
   stw::scl::C_SclString c_TimeStampAbsoluteStart;
   stw::scl::C_SclString c_TimeStampAbsoluteTimeOfDay;
   stw::scl::C_SclString c_CanIdDec;
   stw::scl::C_SclString c_CanIdHex;
   stw::scl::C_SclString c_CanDlc;
   bool q_CanDlcError;
   stw::scl::C_SclString c_CanDataDec;
   stw::scl::C_SclString c_CanDataHex;
   stw::scl::C_SclString c_Name;
   stw::scl::C_SclString c_ProtocolTextDec;
   stw::scl::C_SclString c_ProtocolTextHex;
   stw::scl::C_SclString c_Counter;

   std::vector<C_OscComMessageLoggerDataSignal> c_Signals;

   stw::can::T_STWCAN_Msg_RX c_CanMsg;
   bool q_IsTx;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
