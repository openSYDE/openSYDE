//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for logging CAN messages (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCComMessageLoggerData.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComMessageLoggerDataSignal::C_OSCComMessageLoggerDataSignal(void) :
   c_Name(""),
   c_Value(""),
   c_Unit(""),
   c_RawValueDec(""),
   c_RawValueHex(""),
   c_Comment(""),
   q_DlcError(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller than orc_Cmp

   Comparing by start bit

   \param[in] orc_Cmp Compared instance

   \return
   true  Current smaller than orc_Cmp
   false orc_Cmp equal or bigger than current
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCComMessageLoggerDataSignal::operator <(const C_OSCComMessageLoggerDataSignal & orc_Cmp) const
{
   bool q_Return = false;

   if (this->c_OscSignal.u16_ComBitStart < orc_Cmp.c_OscSignal.u16_ComBitStart)
   {
      q_Return = true;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComMessageLoggerData::C_OSCComMessageLoggerData(void) :
   u64_TimeStampRelative(0U),
   u64_TimeStampAbsoluteStart(0U),
   u64_TimeStampAbsoluteTimeOfDay(0U),
   c_TimeStampRelative(""),
   c_TimeStampAbsoluteStart(""),
   c_TimeStampAbsoluteTimeOfDay(""),
   c_CanIdDec(""),
   c_CanIdHex(""),
   c_CanDlc(""),
   q_CanDlcError(false),
   c_CanDataDec(""),
   c_CanDataHex(""),
   c_Name(""),
   c_ProtocolTextDec(""),
   c_ProtocolTextHex(""),
   c_Counter(""),
   c_CanMsg(stw_can::T_STWCAN_Msg_RX()),
   q_IsTx(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCComMessageLoggerData::~C_OSCComMessageLoggerData(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort the signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCComMessageLoggerData::SortSignals(void)
{
   std::sort(this->c_Signals.begin(), this->c_Signals.end());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the timestamp as string

   Following format will be returned:
   hh:mm:ss.m_s.u_s
   00:00:00.000.000

   \param[in]     ou64_TimeStamp   Timestamp in us

   \return
   Formatted timestamp
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCComMessageLoggerData::h_GetTimestampAsString(const uint64 ou64_TimeStamp)
{
   C_SCLString c_TimeStamp;
   const uint16 u16_MicroSec = static_cast<uint16>(ou64_TimeStamp % 1000U);
   uint64 u64_TimeStampTemp = ou64_TimeStamp / 1000U; // To ms
   const uint16 u16_MilliSec = static_cast<uint16>(u64_TimeStampTemp % 1000U);
   uint8 u8_Sec;
   uint8 u8_Min;
   uint32 u32_Hours;

   u64_TimeStampTemp /= 1000U; // To sec
   u8_Sec = static_cast<uint8>(u64_TimeStampTemp % 60U);

   u64_TimeStampTemp /= 60U; // To min
   u8_Min = static_cast<uint8>(u64_TimeStampTemp % 60U);

   u64_TimeStampTemp /= 60U; // To hours
   u32_Hours = static_cast<uint32>(u64_TimeStampTemp);

   c_TimeStamp.PrintFormatted("%02d:%02d:%02d.%03d.%03d",
                              u32_Hours, u8_Min, u8_Sec, u16_MilliSec, u16_MicroSec);

   return c_TimeStamp;
}
