//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class for logging CAN messages (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCComMessageLoggerData.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     20.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if current smaller than orc_Cmp

   Comparing by start bit

   \param[in] orc_Cmp Compared instance

   \return
   true  Current smaller than orc_Cmp
   false orc_Cmp equal or bigger than current

   \created     25.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_OSCComMessageLoggerDataSignal::operator <(const C_OSCComMessageLoggerDataSignal & orc_Cmp) const
{
   bool q_Return = false;

   if (this->c_OscSignal.u16_ComBitStart < orc_Cmp.c_OscSignal.u16_ComBitStart)
   {
      q_Return = true;
   }

   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.09.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLoggerData::C_OSCComMessageLoggerData(void) :
   u64_TimeStampRelative(0U),
   u64_TimeStampAbsolute(0U),
   c_TimeStampRelative(""),
   c_TimeStampAbsolute(""),
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

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     17.12.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLoggerData::~C_OSCComMessageLoggerData(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sort the signals

   \created     25.01.2019  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OSCComMessageLoggerData::SortSignals(void)
{
   std::sort(this->c_Signals.begin(), this->c_Signals.end());
}
