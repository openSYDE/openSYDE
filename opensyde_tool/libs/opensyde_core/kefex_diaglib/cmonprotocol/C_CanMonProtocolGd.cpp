//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion class for the Generic Driver protocol

   Converts a CAN message structure to text. The interpretation is based on the Generic Driver CAN protocol
   specification.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolGd.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
#define GD_REQUEST_SRR_WO_HS         0xC5
#define GD_REQUEST_SRR_HS            0xC3
#define GD_REQUEST_ECRR_WO_HS        0xCA
#define GD_REQUEST_ECRR_HS           0xCB
#define GD_REQUEST_TCRR              0xCF
#define GD_HS_TO_FIRST_RESPONSE_SRR  0xB3
#define GD_HS_TO_FIRST_RESPONSE_ECRR 0xBB
#define GD_ABORT_GLOBAL_INDIV_RQ     0xC0
#define GD_IMMEDIATE_WRITE_WO_HS     0xD5
#define GD_IMMEDITAE_WRITE_HS        0xD3
#define GD_BT_REQ_CTRL_ACK           0xC7
#define GD_BT_DATA                   0xD7
#define GD_LIFE_MESSAGE              0xFA
#define GD_LOGON                     0xF0
#define GD_LOGOFF                    0xFF
/* Acknowledge status */
#define GD_ACK_TRANS_COMPLETE        0x00
#define GD_ACK_TRANS_PARTIAL         0x01
#define GD_ACK_TRANS_ABORTED         0x02
#define GD_ACK_REPEAT_DATA           0x05
/* Control states */
#define GD_CTRL_TRANS_COMPLETE       0x00
#define GD_CTRL_TRANS_PARTIAL        0x01
#define GD_CTRL_TRANS_ABORTED        0x02
#define GD_CTRL_REPEAT_DATA          0x05
#define GD_TRANS_TIMEOUT             0x03
#define GD_TRANS_IDLE                0x04

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
C_SclString C_CanMonProtocolGd::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   bool q_IsRequest;
   uint8_t u8_SenderSegment;
   uint8_t u8_SenderNode;
   uint8_t u8_ReceiverSegment;
   uint8_t u8_ReceiverNode;
   uint16_t u16_VariableIndex;
   uint16_t u16_Data;
   uint16_t u16_NumberOfBytes;
   uint32_t u32_SysTime;
   C_SclString c_Text;
   C_SclString c_Help;
   C_SclString c_HelpSender;
   C_SclString c_HelpReceiver;
   C_SclString c_HelpTime;
   C_SclString c_HelpHystHigh;
   C_SclString c_HelpHystLow;

   if ((orc_Msg.u32_ID < 0x600) || (orc_Msg.u32_ID > 0x6FFU))
   {
      return "";
   }

   //get sender "network segment" and "node number"
   u8_SenderSegment = static_cast<uint8_t>((orc_Msg.u32_ID & 0xF0U) >> 4U);
   u8_SenderNode    = static_cast<uint8_t>(orc_Msg.u32_ID & 0x0FU);
   c_HelpSender = m_GetByteAsStringFormat(u8_SenderSegment) + "/" + m_GetByteAsStringFormat(u8_SenderNode);

   //get receiver "network segment" and "node number"
   u8_ReceiverSegment = static_cast<uint8_t>((orc_Msg.au8_Data[0] & 0xF0U) >> 4U);
   u8_ReceiverNode    = static_cast<uint8_t>(orc_Msg.au8_Data[0] & 0x0FU);
   c_HelpReceiver = m_GetByteAsStringFormat(u8_ReceiverSegment) + "/" + m_GetByteAsStringFormat(u8_ReceiverNode);

   //get the variables index:
   u16_VariableIndex = static_cast<uint16_t>(orc_Msg.au8_Data[1] +
                                             ((static_cast<uint16_t>(orc_Msg.au8_Data[2]) & 0x7FU) << 8U));

   //response or request
   if ((orc_Msg.au8_Data[2] & 0x80U) == 0x80U)
   {
      q_IsRequest = true;
   }
   else
   {
      q_IsRequest = false;
   }

   switch (orc_Msg.au8_Data[3])
   {
   case GD_REQUEST_SRR_WO_HS:
      //single response request (SRR) without handshake
      if (q_IsRequest == true)
      {
         //request
         c_Text = c_HelpSender +
                  " -> " +
                  c_HelpReceiver +
                  ", SRR REQ IDX: " +
                  m_GetValueDecHex(u16_VariableIndex);
      }
      else
      {
         //response
         u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
         c_Text = c_HelpSender +
                  " -> " +
                  c_HelpReceiver +
                  ", SRR RES IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) +
                  ", DATA: " +
                  m_GetValueDecHex(u16_Data);
      }
      break;
   case GD_REQUEST_SRR_HS:
      //single response request (SRR) with handshake
      if (q_IsRequest == true)
      {
         //request
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", SRR REQ HS IDX: " +
                  m_GetValueDecHex(u16_VariableIndex);
      }
      else
      {
         //response
         u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", SRR RES HS IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) + ", DATA: " + m_GetValueDecHex(u16_Data);
      }
      break;
   case GD_REQUEST_ECRR_WO_HS:
      //event controlled response request (ECRR) without handshake
      if (q_IsRequest == true)
      {
         //request
         //evaluate maximum wait time
         if (orc_Msg.au8_Data[4] == 0U)
         {
            c_HelpTime = "NO";
         }
         else
         {
            c_HelpTime = m_GetValueDecHex(static_cast<uint16_t>(orc_Msg.au8_Data[4]) * 100) + "ms";
         }
         //evaluate upper hysteresis
         if (orc_Msg.au8_Data[5] <= 99U)
         {
            //0.0% .. 9.9%
            c_HelpHystHigh = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[5] / 10)) +
                             "." +
                             m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[5] % 10)) +
                             "%";
         }
         else if ((orc_Msg.au8_Data[5] >= 110U) && (orc_Msg.au8_Data[5] <= 200U))
         {
            //10% .. 100%
            c_HelpHystHigh = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[5] - 100)) + "%";
         }
         else
         {
            //invalid upper hysteresis
            c_HelpHystHigh = "invalid";
         }
         //evaluate lower hysteresis
         if (orc_Msg.au8_Data[6] <= 99U)
         {
            //0.0% .. 9.9%
            c_HelpHystLow = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[6] / 10)) +
                            "." +
                            m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[6] % 10)) +
                            "%";
         }
         else if ((orc_Msg.au8_Data[6] >= 110U) && (orc_Msg.au8_Data[6] <= 200U))
         {
            //10% .. 100%
            c_HelpHystLow = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[6] - 100)) + "%";
         }
         else
         {
            //invalid lower hysteresis
            c_HelpHystLow = "invalid";
         }

         c_Text = c_HelpSender + " -> " + c_HelpReceiver +
                  ", ECRR REQ IDX: " + m_GetValueDecHex(u16_VariableIndex) +
                  ", WAIT: " + c_HelpTime +
                  ", HYST HIGH: " + c_HelpHystHigh +
                  ", HYST LOW: " + c_HelpHystLow;
      }
      else
      {
         //response
         u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
         c_Text = c_HelpSender + " -> " +
                  c_HelpReceiver + ", ECRR RES IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) + ", DATA: " +
                  m_GetValueDecHex(u16_Data);
      }
      break;
   case GD_REQUEST_ECRR_HS:
      //event controlled response request (ECRR) with handshake
      if (q_IsRequest == true)
      {
         //request
         //evaluate maximum wait time
         if (orc_Msg.au8_Data[4] == 0U)
         {
            c_HelpTime = "NO";
         }
         else
         {
            c_HelpTime = m_GetValueDecHex(static_cast<uint16_t>(orc_Msg.au8_Data[4]) * 100) + "ms";
         }
         //evaluate upper hysteresis
         if (orc_Msg.au8_Data[5] <= 99U)
         {
            //0.0% .. 9.9%
            c_HelpHystHigh = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[5] / 10)) + "." +
                             m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[5] % 10)) + "%";
         }
         else if ((orc_Msg.au8_Data[5] >= 110U) && (orc_Msg.au8_Data[5] <= 200U))
         {
            //10% .. 100%
            c_HelpHystHigh = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[5] - 100U)) + "%";
         }
         else
         {
            //invalid upper hysteresis
            c_HelpHystHigh = "invalid";
         }
         //evaluate lower hysteresis
         if (orc_Msg.au8_Data[6] <= 99U)
         {
            //0.0% .. 9.9%
            c_HelpHystLow = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[6] / 10)) + "." +
                            m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[6] % 10)) + "%";
         }
         else if ((orc_Msg.au8_Data[6] >= 110U) && (orc_Msg.au8_Data[6] <= 200U))
         {
            //10% .. 100%
            c_HelpHystLow = m_GetByteAsStringFormat(static_cast<uint8_t>(orc_Msg.au8_Data[6] - 100)) + "%";
         }
         else
         {
            //invalid lower hysteresis
            c_HelpHystLow = "invalid";
         }

         c_Text = c_HelpSender + " -> " +
                  c_HelpReceiver + ", ECRR REQ HS IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) +
                  ", WAIT: " + c_HelpTime +
                  ", HYST HIGH: " + c_HelpHystHigh +
                  ", HYST LOW: " + c_HelpHystLow;
      }
      else
      {
         //response
         u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
         c_Text = c_HelpSender + " -> " +
                  c_HelpReceiver + ", ECRR RES HS IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) + ", DATA: " +
                  m_GetValueDecHex(u16_Data);
      }
      break;
   case GD_REQUEST_TCRR:
      //time controlled response request (TCRR)
      if (q_IsRequest == true)
      {
         //request
         //evaluate period
         if (orc_Msg.au8_Data[4] == 0U)
         {
            c_HelpTime = "5ms";
         }
         else if ((orc_Msg.au8_Data[4] >= 1U) && (orc_Msg.au8_Data[4] <= 99U))
         {
            c_HelpTime = m_GetValueDecHex(static_cast<uint16_t>(orc_Msg.au8_Data[4]) * 10U) + "ms";
         }
         else if (orc_Msg.au8_Data[4] >= 101U)
         {
            c_HelpTime = m_GetValueDecHex((static_cast<uint32_t>(orc_Msg.au8_Data[4]) - 100U) * 1000U) + "sec";
         }
         else
         {
            c_HelpTime = "invalid";
         }

         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", TCRR REQ IDX: " + m_GetValueDecHex(u16_VariableIndex) +
                  ", PERIOD: " + c_HelpTime;
      }
      else
      {
         //response
         u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", TCRR RES IDX: " + m_GetValueDecHex(u16_VariableIndex) +
                  ", DATA: " + m_GetValueDecHex(u16_Data);
      }
      break;
   case GD_HS_TO_FIRST_RESPONSE_SRR:
      //handshake to first single response request
      c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", HS on SRR IDX: " + m_GetValueDecHex(u16_VariableIndex);
      break;
   case GD_HS_TO_FIRST_RESPONSE_ECRR:
      //handshake to first event controlled response request
      c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", HS on ECRR IDX: " + m_GetValueDecHex(u16_VariableIndex);
      break;
   case GD_ABORT_GLOBAL_INDIV_RQ:
      //abort individual and global request
      if (orc_Msg.au8_Data[0] == 0xFFU)
      {
         //it´s an abort global request
         c_Text = c_HelpSender + ", ABORT GLOBAL REQ";
      }
      else
      {
         //it´s an abort individual request
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", ABORT INDIVIDUAL REQ IDX: " +
                  m_GetValueDecHex(u16_VariableIndex);
      }
      break;
   case GD_IMMEDIATE_WRITE_WO_HS:
      //immediate write without handshake
      u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
      c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", IMMEDIATE WRITE IDX: " +
               m_GetValueDecHex(u16_VariableIndex) + ", DATA: " + m_GetValueDecHex(u16_Data);
      break;
   case GD_IMMEDITAE_WRITE_HS:
      //immediate write with handshake
      if (q_IsRequest == true)
      {
         //request
         u16_Data = static_cast<uint16_t>(orc_Msg.au8_Data[5] + (static_cast<uint16_t>(orc_Msg.au8_Data[6]) << 8U));
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", IMMEDIATE WRITE HS IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) + ", DATA: " + m_GetValueDecHex(u16_Data);
      }
      else
      {
         //response
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", HS on IMMEDIATE WRITE IDX: " +
                  m_GetValueDecHex(u16_VariableIndex);
      }
      break;
   case GD_BT_REQ_CTRL_ACK:
      if (q_IsRequest == true)
      {
         switch (orc_Msg.au8_Data[6])
         {
         case 0x0U:
            //block transfer read request
            u16_NumberOfBytes = static_cast<uint16_t>(orc_Msg.au8_Data[4] +
                                                      (static_cast<uint16_t>(orc_Msg.au8_Data[5]) << 8U));
            c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", BLOCK TRANSFER READ REQ IDX: " +
                     m_GetValueDecHex(u16_VariableIndex) + ", NOB: " + m_GetValueDecHex(u16_NumberOfBytes) +
                     ", REP: " + m_GetValueDecHex(orc_Msg.au8_Data[7] >> 4U) + ", ACK: " +
                     m_GetValueDecHex(orc_Msg.au8_Data[7] & 0x0FU);
            break;
         case 0x2DU:
            //block transfer read acknowledge
            switch (orc_Msg.au8_Data[7])
            {
            case GD_ACK_TRANS_COMPLETE:
               c_Help = "COMPLETED";
               break;
            case GD_ACK_TRANS_PARTIAL:
               c_Help = "PARTIAL";
               break;
            case GD_ACK_TRANS_ABORTED:
               c_Help = "ABORTED";
               break;
            case GD_ACK_REPEAT_DATA:
               c_Help = "REPEAT";
               break;
            default:
               c_Help = "UNKNOWN";
               break;
            }
            c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", BLOCK TRANSFER READ ACK IDX: " +
                     m_GetValueDecHex(u16_VariableIndex) + ", STATUS: " + c_Help;
            break;
         case 0x80U:
            //block transfer write request
            u16_NumberOfBytes = static_cast<uint16_t>(orc_Msg.au8_Data[4] +
                                                      (static_cast<uint16_t>(orc_Msg.au8_Data[5]) << 8U));
            c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", BLOCK TRANSFER WRITE REQ IDX: " +
                     m_GetValueDecHex(u16_VariableIndex) + ", NOB: " + m_GetValueDecHex(u16_NumberOfBytes) +
                     ", REP: " + m_GetValueDecHex(orc_Msg.au8_Data[7] >> 4U) +
                     ", ACK: " + m_GetValueDecHex(orc_Msg.au8_Data[7] & 0x0FU);
            break;
         case 0xACU:
            //block transfer write control
            switch (orc_Msg.au8_Data[7])
            {
            case GD_CTRL_TRANS_COMPLETE:
               c_Help = "COMPLETED";
               break;
            case GD_CTRL_TRANS_PARTIAL:
               c_Help = "PARTIAL";
               break;
            case GD_CTRL_TRANS_ABORTED:
               c_Help = "ABORTED";
               break;
            case GD_CTRL_REPEAT_DATA:
               c_Help = "REPEAT";
               break;
            case GD_TRANS_TIMEOUT:
               c_Help = "TIMEOUT";
               break;
            case GD_TRANS_IDLE:
               c_Help = "IDLE";
               break;
            default:
               c_Help = "UNKNOWN";
               break;
            }
            c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", BLOCK TRANSFER WRITE CTRL IDX: " +
                     m_GetValueDecHex(u16_VariableIndex) + ", STAT: " + m_GetValueDecHex(orc_Msg.au8_Data[7]) +
                     ", CTRL: " + c_Help;
            break;
         default:
            break;
         }
      }
      else
      {
         switch (orc_Msg.au8_Data[6])
         {
         case 0x2CU:
            //block transfer read control
            switch (orc_Msg.au8_Data[7])
            {
            case GD_CTRL_TRANS_COMPLETE:
               c_Help = "COMPLETED";
               break;
            case GD_CTRL_TRANS_PARTIAL:
               c_Help = "PARTIAL";
               break;
            case GD_CTRL_TRANS_ABORTED:
               c_Help = "ABORTED";
               break;
            case GD_CTRL_REPEAT_DATA:
               c_Help = "REPEAT";
               break;
            case GD_TRANS_TIMEOUT:
               c_Help = "TIMEOUT";
               break;
            case GD_TRANS_IDLE:
               c_Help = "IDLE";
               break;
            default:
               c_Help = "UNKNOWN";
               break;
            }
            c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", BLOCK TRANSFER READ CTRL IDX: " +
                     m_GetValueDecHex(u16_VariableIndex) + ", STAT: " + m_GetValueDecHex(orc_Msg.au8_Data[7]) +
                     ", CTRL: " + c_Help;
            break;
         case 0xADU:
            //block transfer write acknowledge
            switch (orc_Msg.au8_Data[7])
            {
            case GD_ACK_TRANS_COMPLETE:
               c_Help = "COMPLETED";
               break;
            case GD_ACK_TRANS_PARTIAL:
               c_Help = "PARTIAL";
               break;
            case GD_ACK_TRANS_ABORTED:
               c_Help = "ABORTED";
               break;
            case GD_ACK_REPEAT_DATA:
               c_Help = "REPEAT";
               break;
            default:
               c_Help = "UNKNOWN";
               break;
            }
            c_Text = c_HelpSender + " -> " + c_HelpReceiver +
                     ", BLOCK TRANSFER WRITE ACK IDX: " + m_GetValueDecHex(u16_VariableIndex) +
                     ", STATUS: " + c_Help;
            break;
         default:
            break;
         }
      }
      break;
   case GD_BT_DATA:
      //block transfer data
      if (q_IsRequest == true)
      {
         //write data
         c_Text = c_HelpSender + " -> " + c_HelpReceiver + ", BLOCK TRANSFER WRITE DATA IDX: " +
                  m_GetValueDecHex(u16_VariableIndex) +
                  ", DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                  ", DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                  ", DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                  ", DB4: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
      }
      else
      {
         //read data
         c_Text = c_HelpSender + " -> " + c_HelpReceiver +
                  ", BLOCK TRANSFER READ DATA IDX: " + m_GetValueDecHex(u16_VariableIndex) +
                  ", DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                  ", DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                  ", DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                  ", DB4: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
      }
      break;
   case GD_LIFE_MESSAGE:
      //life messages (with mapped system time in [ms])
      u32_SysTime = static_cast<uint32_t>(orc_Msg.au8_Data[4]) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 8U) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[6]) << 16U) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[7]) << 24U);
      c_Text = c_HelpSender +
               ", LIFE (SYSTIME: " + m_GetValueDecHex(u32_SysTime) + "ms)";
      break;
   case GD_LOGON:
      //logon messages (with mapped system time in [ms])
      u32_SysTime = static_cast<uint32_t>(orc_Msg.au8_Data[4]) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 8U) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[6]) << 16U) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[7]) << 24U);
      c_Text = c_HelpSender + ", LOGON (SYSTIME: " + m_GetValueDecHex(u32_SysTime) + "ms)";
      break;
   case GD_LOGOFF:
      //logon messages (with mapped system time in [ms])
      u32_SysTime = static_cast<uint32_t>(orc_Msg.au8_Data[4]) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 8U) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[6]) << 16U) +
                    (static_cast<uint32_t>(orc_Msg.au8_Data[7]) << 24U);
      c_Text = c_HelpSender + ", LOGOFF (SYSTIME: " + m_GetValueDecHex(u32_SysTime) + "ms)";
      break;
   default:
      c_Text += " unknown service";
      switch (orc_Msg.u8_XTD)
      {
      case 1U:
         c_Help = "29Bit";
         break;
      case 0U:
      default:
         c_Help = "11Bit";
         break;
      }
      c_Text += " (ID: " + m_GetValueDecHex(orc_Msg.u32_ID) +
                ", XTD: " + c_Help +
                ", DLC: " + m_GetValueDecHex(orc_Msg.u8_DLC) +
                ", DB0: " + m_GetValueDecHex(orc_Msg.au8_Data[0]) +
                ", DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[1]) +
                ", DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[2]) +
                ", DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                ", DB4: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                ", DB5: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                ", DB6: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                ", DB7: " + m_GetValueDecHex(orc_Msg.au8_Data[7]) + ")";
      return c_Text;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolGd::GetProtocolName(void) const
{
   return "WD Generic Driver";
}

//----------------------------------------------------------------------------------------------------------------------
