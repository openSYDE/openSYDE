//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion class for layer 2 CAN messages

   Converts a CAN message structure to text.
   The interpretation is based on no high level protocol.
   The raw CAN message data will be returned as a string.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolL2.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert CAN message to text representation.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolL2::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text;
   C_SclString c_Help;
   char_t cn_XTD;
   char_t cn_RTR;
   int32_t j;
   uint8_t u8_Len;

   cn_XTD = (orc_Msg.u8_XTD == 1U) ? 'x' : ' ';
   cn_RTR = (orc_Msg.u8_RTR == 1U) ? 'r' : ' '; //note: displaying RTR correctly will only work if comm driver supplies
                                                // that information
   u8_Len = (orc_Msg.u8_DLC > 8U) ? static_cast<uint8_t>(8U) : orc_Msg.u8_DLC;

   if (mq_Decimal == false)
   {
      (void)c_Text.PrintFormatted("%08X%c%c %d ", orc_Msg.u32_ID, cn_XTD, cn_RTR, orc_Msg.u8_DLC);
      for (j = 0; j < u8_Len; j++)
      {
         (void)c_Help.PrintFormatted(" %02X ", orc_Msg.au8_Data[j]);
         c_Text += c_Help;
      }
      for (; j < 8; j++)
      {
         c_Text += "    ";
      }
   }
   else
   {
      (void)c_Text.PrintFormatted("%8d%c%c %d ", orc_Msg.u32_ID, cn_XTD, cn_RTR, orc_Msg.u8_DLC);
      for (j = 0; j < u8_Len; j++)
      {
         (void)c_Help.PrintFormatted("%3d ", orc_Msg.au8_Data[j]);
         c_Text += c_Help;
      }
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolL2::GetProtocolName(void) const
{
   return "CAN Layer 2";
}

//----------------------------------------------------------------------------------------------------------------------
