//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion class for the STW-FF protocol

   Converts a CAN message structure to text. The interpretation is based on the STW-FF CAN protocol
   specification.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolStwFf.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
#define FF_SERVICE_DATA       0
#define FF_SERVICE_GET_CONFIG 1
#define FF_SERVICE_SET_CONFIG 2
#define FF_SERVICE_HANDSHAKE  3
#define FF_SERVICE_DOWNLOAD   4
#define FF_SERVICE_RESPONSE_READY 5
#define FF_SERVICE_ERROR     15

#define FF_CONFIG_SERVICE_SET_SEG_PARAMS_VERSION  0

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
C_SclString C_CanMonProtocolStwFf::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text;
   bool q_IsRequest;
   uint8_t u8_NodeId;
   int32_t s32_Start;
   if ((orc_Msg.u32_ID < 0x480U) || (orc_Msg.u32_ID > 0x5C4U))
   {
      return "";
   }

   q_IsRequest = ((orc_Msg.u32_ID & 0x01U) != 0U) ? true : false;
   u8_NodeId    = static_cast<uint8_t>((orc_Msg.u32_ID - 0x480U) >> 1);
   if (q_IsRequest)
   {
      c_Text = "REQ -> " + m_GetByteAsStringFormat(u8_NodeId);
   }
   else
   {
      c_Text = "RES <- " + m_GetByteAsStringFormat(u8_NodeId);
   }
   if (orc_Msg.u8_DLC < 2U)
   {
      c_Text+=" !!WRONG_DLC!!";
      return c_Text;
   }
   switch (orc_Msg.au8_Data[0] & 0x0FU)
   {
   case FF_SERVICE_DATA:
      c_Text += " DATA";
      break;
   case FF_SERVICE_GET_CONFIG:
      c_Text += " GET_CFG";
      break;
   case FF_SERVICE_SET_CONFIG:
      c_Text += " SET_CFG";
      break;
   case FF_SERVICE_HANDSHAKE:
      c_Text += " HANDSHAKE";
      break;
   case FF_SERVICE_DOWNLOAD:
      c_Text += " DOWNLOAD";
      break;
   case FF_SERVICE_RESPONSE_READY:
      c_Text += "  RESPONSE_READY";
      break;
   case FF_SERVICE_ERROR:
      c_Text += " ERROR";
      break;
   default:
      c_Text += " unknown service";
      return c_Text;
   }
   if ((orc_Msg.au8_Data[0] & 0x80U) != 0U) //last frame ?
   {
      if (orc_Msg.au8_Data[1] == 0U)
      {
         c_Text += "(SF)"; //only one block -> single frame
      }
      else
      {
         c_Text += "(LF)"; //last of a number of blocks -> last frame
      }
   }
   else
   {
      if (orc_Msg.au8_Data[1] == 0U)
      {
         c_Text += "(FF)"; //first block of a longer transmission -> first frame
      }
      else
      {
         c_Text += "(CF)"; //consecutive frame
      }
   }

   if ((orc_Msg.au8_Data[0] & 0x0FU) != FF_SERVICE_RESPONSE_READY)
   {
      c_Text += (" BLK=" + m_GetByteAsStringFormat(orc_Msg.au8_Data[1]));
   }
   switch (orc_Msg.au8_Data[0] & 0x0FU)
   {
   case FF_SERVICE_DATA:
      if (orc_Msg.au8_Data[1] == 0U)
      {
         if (orc_Msg.u8_DLC < 5U)
         {
            c_Text+=" !!WRONG_DLC!!";
            return c_Text;
         }
         c_Text += (" SIZE:" + m_GetValueDecHex(orc_Msg.au8_Data[2] + (((uint16_t)orc_Msg.au8_Data[3]) << 8U)));
         s32_Start = 4;
      }
      else
      {
         if (orc_Msg.u8_DLC < 3U)
         {
            c_Text+=" !!WRONG_DLC!!";
            return c_Text;
         }
         s32_Start = 2;
      }
      c_Text += " DATA:";
      for (; s32_Start < 8; s32_Start++)
      {
         c_Text += " " + m_GetByteAsStringFormat(orc_Msg.au8_Data[s32_Start]);
      }
      break;
   case FF_SERVICE_GET_CONFIG:
      c_Text += " DATA:";
      for (s32_Start = 2; s32_Start < 8; s32_Start++)
      {
         c_Text += " " + m_GetByteAsStringFormat(orc_Msg.au8_Data[s32_Start]);
      }
      break;
   case FF_SERVICE_SET_CONFIG:
      if (orc_Msg.u8_DLC < 3U)
      {
         c_Text += " !!WRONG_DLC!!";
         return c_Text;
      }
      switch (orc_Msg.au8_Data[2])
      {
      case FF_CONFIG_SERVICE_SET_SEG_PARAMS_VERSION:
         c_Text += " SET_SEG_PARAMS";
         if (orc_Msg.u8_DLC < 7U)
         {
            c_Text += " !!WRONG_DLC!!";
            return c_Text;
         }
         c_Text+=(" Ver:" + m_GetValueDecHex(orc_Msg.au8_Data[3] + (((uint16_t)orc_Msg.au8_Data[4]) << 8U))+
                     " BSmax:"+m_GetByteAsStringFormat(orc_Msg.au8_Data[5]) +
                     " STmin:"+m_GetByteAsStringFormat(orc_Msg.au8_Data[6]));
         break;
      default:
         c_Text += " DATA:";
         for (s32_Start = 2; s32_Start < 8; s32_Start++)
         {
            c_Text += " " + m_GetByteAsStringFormat(orc_Msg.au8_Data[s32_Start]);
         }
         break;
      }
      break;
   case FF_SERVICE_HANDSHAKE:
      //no more info ...
      break;
   case FF_SERVICE_DOWNLOAD:
      if (orc_Msg.au8_Data[1] == 0U)
      {
         if (orc_Msg.u8_DLC < 6U)
         {
            c_Text+=" !!WRONG_DLC!!";
            return c_Text;
         }
         c_Text+=(" SIZE:" + m_GetValueDecHex(orc_Msg.au8_Data[2] + (((uint16_t)orc_Msg.au8_Data[3]) << 8U)) +
                                            (((uint32_t)orc_Msg.au8_Data[4]) << 16U) +
                                            (((uint32_t)orc_Msg.au8_Data[5]) << 24U));
         s32_Start = 6;
      }
      else
      {
         if (orc_Msg.u8_DLC < 3U)
         {
            c_Text+=" !!WRONG_DLC!!";
            return c_Text;
         }
         s32_Start = 2;
      }
      c_Text+=" DATA:";
      for (; s32_Start < 8; s32_Start++)
      {
         c_Text += " " + m_GetByteAsStringFormat(orc_Msg.au8_Data[s32_Start]);
      }
      break;
   case FF_SERVICE_RESPONSE_READY:
      if (orc_Msg.u8_DLC < 2U)
      {
         c_Text += " !!WRONG_DLC!!";
      }
      else
      {
         c_Text += (" HANDLE: " + m_GetValueDecHex(orc_Msg.au8_Data[1]));
      }
      break;
   case FF_SERVICE_ERROR:
      if (orc_Msg.u8_DLC < 6U)
      {
         c_Text+=" !!WRONG_DLC!!";
      }
      else
      {
         c_Text += (" ERROR: " + m_GetValueDecHex(orc_Msg.au8_Data[2] +
                                               (((uint16_t)orc_Msg.au8_Data[3]) << 8U)) +
                                               (((uint32_t)orc_Msg.au8_Data[4]) << 16U) +
                                               (((uint32_t)orc_Msg.au8_Data[5]) << 24U));
      }
      break;
   default:
      c_Text += " unknown service";
      break;
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolStwFf::GetProtocolName(void) const
{
   return "STW FF";
}

//----------------------------------------------------------------------------------------------------------------------

