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
#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.h"
#include "stwerrors.h"
#include "CCMONProtocolL2.h"
#include "CSCLString.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_cmon_protocol;
using namespace stw_scl;
using namespace stw_can;

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
C_SCLString C_CMONProtocolL2::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Text;
   C_SCLString c_Help;
   charn cn_XTD;
   charn cn_RTR;
   sint32 j;
   uint8 u8_Len;

   cn_XTD = (orc_Msg.u8_XTD == 1U) ? 'x':' ';
   cn_RTR = (orc_Msg.u8_RTR == 1U) ? 'r':' '; //note: displaying RTR correctly will only work if comm driver supplies
                                           // that information
   u8_Len = (orc_Msg.u8_DLC > 8U) ? static_cast<uint8>(8U) : orc_Msg.u8_DLC;

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
C_SCLString C_CMONProtocolL2::GetProtocolName(void) const
{
   return "CAN Layer 2";
}

//----------------------------------------------------------------------------------------------------------------------

