//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for message trace

   Utility functions for message trace

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_CamMetUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor/destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetUtil::C_CamMetUtil()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get multiplexer value ordering

   \param[in]  orc_Signals    Signals to look for multiplexer in

   \return
   Multiplexer value ordering
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<sintn> C_CamMetUtil::h_GetMultiplexerOrder(const std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals)
{
   std::vector<sintn> c_Order;
   //Multiplexer order
   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Signals.size(); ++u32_ItSig)
   {
      const C_OSCComMessageLoggerDataSignal & rc_SignalData = orc_Signals[u32_ItSig];
      if (rc_SignalData.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         if (rc_SignalData.q_DlcError == false)
         {
            c_Order.push_back(rc_SignalData.c_RawValueDec.ToInt());
         }
         else
         {
            // Invalid multiplexer but it is still a kind of multiplexer value
            c_Order.push_back(-1);
         }
      }
   }
   //lint -e{864} Call as expected by interface
   std::sort(c_Order.begin(), c_Order.end());
   return c_Order;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get multiplexer value for this instance of a message

   \param[in]  orc_Signals    Signals to search the multiplexer value in

   \return
   -1:   invalid
   else: found multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMetUtil::h_GetMultiplexerValue(const std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals)
{
   sint32 s32_Value = -1;

   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Signals.size(); ++u32_ItSig)
   {
      const C_OSCComMessageLoggerDataSignal & rc_Sig = orc_Signals[u32_ItSig];
      if ((rc_Sig.c_OscSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL) &&
          (rc_Sig.q_DlcError == false))
      {
         s32_Value = rc_Sig.c_RawValueDec.ToInt();
         break;
      }
   }
   return s32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get row in message for multiplexer value

   \param[in]  orc_Signals             Signals to search in
   \param[in]  os32_MultiplexerValue   Multiplexer value to search for

   \return
   -1:   invalid
   else: valid row (of message index)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMetUtil::h_GetRowForMultiplexerValue(const std::vector<C_OSCComMessageLoggerDataSignal> & orc_Signals,
                                                 const sint32 os32_MultiplexerValue)
{
   sint32 s32_Retval = -1;
   const std::vector<sintn> c_MuxValues = C_CamMetUtil::h_GetMultiplexerOrder(orc_Signals);

   for (uint32 u32_It = 0UL; u32_It < c_MuxValues.size(); ++u32_It)
   {
      if (c_MuxValues[u32_It] == os32_MultiplexerValue)
      {
         s32_Retval = static_cast<sint32>(u32_It);
      }
   }
   return s32_Retval;
}
