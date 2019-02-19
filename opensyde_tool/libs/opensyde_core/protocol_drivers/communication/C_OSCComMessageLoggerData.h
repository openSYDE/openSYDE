//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for logging CAN messages (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGERDATA_H
#define C_OSCCOMMESSAGELOGGERDATA_H

/* -- Includes ------------------------------------------------------------- */
#include <algorithm>

#include "stwtypes.h"
#include "stw_can.h"
#include "CSCLString.h"
#include "C_OSCCanSignal.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCComMessageLoggerDataSignal
{
public:
   C_OSCComMessageLoggerDataSignal(void);

   bool operator <(const C_OSCComMessageLoggerDataSignal & orc_Cmp) const;

   stw_scl::C_SCLString c_Name;
   stw_scl::C_SCLString c_Value;
   stw_scl::C_SCLString c_Unit;
   stw_scl::C_SCLString c_RawValueDec;
   stw_scl::C_SCLString c_RawValueHex;
   stw_scl::C_SCLString c_Comment;
   bool q_DlcError;

   stw_opensyde_core::C_OSCCanSignal c_OscSignal;
};

class C_OSCComMessageLoggerData
{
public:
   C_OSCComMessageLoggerData(void);
   virtual ~C_OSCComMessageLoggerData(void);

   void SortSignals(void);

   stw_types::uint64 u64_TimeStampRelative;
   stw_types::uint64 u64_TimeStampAbsolute;
   stw_scl::C_SCLString c_TimeStampRelative;
   stw_scl::C_SCLString c_TimeStampAbsolute;
   stw_scl::C_SCLString c_CanIdDec;
   stw_scl::C_SCLString c_CanIdHex;
   stw_scl::C_SCLString c_CanDlc;
   bool q_CanDlcError;
   stw_scl::C_SCLString c_CanDataDec;
   stw_scl::C_SCLString c_CanDataHex;
   stw_scl::C_SCLString c_Name;
   stw_scl::C_SCLString c_ProtocolTextDec;
   stw_scl::C_SCLString c_ProtocolTextHex;
   stw_scl::C_SCLString c_Counter;

   std::vector<C_OSCComMessageLoggerDataSignal> c_Signals;

   stw_can::T_STWCAN_Msg_RX c_CanMsg;
   bool q_IsTx;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
