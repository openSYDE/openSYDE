//**************************************************************.DA*
//   .MODULE [ KFXProtocol ]
//--------------------------------------------------------------.D1-
//   .FILE [ KFXProtocol.cpp ]
//   .GROUP [ KEFEX Library ]
//   .AUTHOR [ Stangl ]
//--------------------------------------------------------------.F1-
//   .DESCRIPTION
//   Class definition encapsulating KEFEX protocol functions.
//   Caution: Logon,Logoff,ServiceRead,ServiceWrite are not protected by CriticalSection
//   -> don't use from multiple threads in parallel
//--------------------------------------------------------------
//   .SPEC
//   SWKE KEFEX
//--------------------------------------------------------------
//   .HISTORY
//    Date      Author      Commentary
//    28.08.17  AST         Added option to "Sleep" when polling for responses (default: do not Sleep, as before)
//    15.11.10  AST         Receiving responses: Fixed random erroneous C_NO_ERR returns when there are no responses
//                           (depending on stack content).
//    26.08.10  AST         Standardized timestamp format of protocol file
//              AST         Fixed RX-filter for block transfers
//    21.06.10  AST         Harmonized the return values of segmented transfer functions with non-segmented functions
//    31.05.10  AST         Removed support for "parameter" for indexed services (never used or documented)
//    07.05.10  AST         Reception: Reworked to use CANDispatcher instead of messed up callback mechanism
//    02.04.09  AST         Logon: highest nibble of data-version can be used
//    22.08.07  AST         Removed Execute service (was for internal use only)
//    27.10.06  AST         When polling with timeout = 0 we always read in
//                           as long as there are messages to prevent the RX-buffer from
//                           overflowing with non-KEFEX messages and thus blocking
//                           reception of the messages we really want
//    22.03.06  AST         fixed CKFXProtocol::ProtocolSetup
//    05.10.05  AST         Added ::ProtocolSetup
//    15.11.04  AST         Reworked Lockout Mechanism
//                          - Each confirmed service has it's own CriticalSection
//                          -> No problem when calling one service more than one time from different Threads
//                            (locking for our own Thread is not required as CKFXProtocol does not call ProcessMessages
//                            -> no way for the application to preemptively interrupt)
//    13.09.04  AST         Lock will be released as often as aquired (caused problems when called in thread)
//          04  AST         Added handling of segmented blocks
//    17.11.03  AST         Added Execute service
//    15.04.03  AST         Added evaluation of StartupIndication
//    28.02.03  AST         Added ServiceRead/ServiceWrite
//    14.01.03  AST         Only one logon type
//    23.07.02  AST         Low/High bytes of ECRR hysteresis were exchanged
//         .01  AST         Created
//**************************************************************.DE*
#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cmath>
#include <cstring>

#include "stwtypes.h"
#include "stwerrors.h"
#include "CKFXProtocol.h"
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
#include "CCMONProtocol.h"
#endif
#include "CSCLString.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "CSCLChecksums.h"

//---------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
using namespace stw_cmon_protocol;
#endif
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_can;
using namespace stw_diag_lib;

//---------------------------------------------------------------------------

//Return a 16bit unsigned value consisting of 2 bytes.
//Could be named "..Little" as well. But Intel might be quicker to read.
uint16 C_KFXProcotolResponse::GetU16FromDataIntel(const uint8 ou8_StartIndex) const
{
   tgl_assert(ou8_StartIndex < (sizeof(au8_Data) - 1));
   if (ou8_StartIndex >= (sizeof(au8_Data) - 1))
   {
      return 0U;
   }
   return static_cast<uint16>( static_cast<uint16>(au8_Data[ou8_StartIndex]) +
                              (static_cast<uint16>((static_cast<uint16>(au8_Data[ou8_StartIndex + 1U])) << 8)));
}

//---------------------------------------------------------------------------

//Return a 32bit unsigned value consisting of 4 bytes.
//Could be named "..Little" as well. But Intel might be quicker to read.
uint32 C_KFXProcotolResponse::GetU32FromDataIntel(const uint8 ou8_StartIndex) const
{
   tgl_assert(ou8_StartIndex < (sizeof(au8_Data) - 3));
   if (ou8_StartIndex >= (sizeof(au8_Data) - 3))
   {
      return 0U;
   }
   return ( static_cast<uint32>(au8_Data[ou8_StartIndex]) +
           ((static_cast<uint32>(au8_Data[ou8_StartIndex + 1U])) << 8) +
           ((static_cast<uint32>(au8_Data[ou8_StartIndex + 2U])) << 16) +
           ((static_cast<uint32>(au8_Data[ou8_StartIndex + 3U])) << 24));
}

//---------------------------------------------------------------------------

C_KFXProtocol::C_KFXProtocol(C_KFXCommConfiguration * const opt_CommConfig, const bool oq_SleepBetweenPolling) :
   mpr_ResetCallBack(NULL),
   mpv_ResetCallBackInstance(NULL),
   mu32_SendIDRQ(0xFFFFFFFFU),
   mu32_ReceiveIDRSP(0xFFFFFFFFU),
   mpc_Dispatcher(NULL),
   mu16_CANDispatcherHandle(0U),
   mpt_Config(opt_CommConfig),
   mq_SleepBetweenPolling(oq_SleepBetweenPolling),
   u8_CreateCommProtocol(0U),
   mpr_OnNewCyclicTransmissionReceived(NULL),
   mpv_CyclicCallbackInstance(NULL)
{
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
   (void)mc_Protocol.SetProtocolMode(CMONL7ProtocolKEFEX);
   (void)mc_Protocol.SetDecimalMode(false);
   mpt_DebugFileHandle = NULL;
#endif
}

//---------------------------------------------------------------------------

#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
void C_KFXProtocol::m_ProtocolCheckInit(void)
{
   if (mpt_DebugFileHandle == NULL)
   {
      mpt_DebugFileHandle = std::fopen("kfxprotocol.txt", "wb"); //will erase if it already exists !
   }
}

//---------------------------------------------------------------------------

void C_KFXProtocol::m_ProtocolSend(const bool oq_OK, const T_STWCAN_Msg_TX & orc_Msg)
{
   m_ProtocolCheckInit();
   if (mpt_DebugFileHandle != NULL)
   {
      C_SCLString c_Text;
      C_SCLString c_Help;
      C_SCLString c_Time;
      sint32 i;

      c_Time = C_CMONProtocols::FormatTimeStamp(TGL_GetTickCountUS(), false);
      if (u8_CreateCommProtocol == 1U)
      {
         (void)c_Text.PrintFormatted("%s Msg_Sent: MSG: %03x %d ", c_Time.c_str(), orc_Msg.u32_ID, orc_Msg.u8_DLC);
         for (i = 0; i < orc_Msg.u8_DLC; i++)
         {
            (void)c_Help.PrintFormatted("%02x ", orc_Msg.au8_Data[i]);
            c_Text += c_Help;
         }
         c_Text += "\r\n    ";
      }

      c_Text += (c_Time + " " + mc_Protocol.MessageToString(orc_Msg));

      if (oq_OK == true)
      {
         c_Text += " sent OK !\r\n";
      }
      else
      {
         c_Text += " sent ERROR !\r\n";
      }
      fwrite(c_Text.c_str(), 1, c_Text.Length(), mpt_DebugFileHandle);
   }
}

//---------------------------------------------------------------------------

void C_KFXProtocol::m_ProtocolReceive(const T_STWCAN_Msg_RX & orc_Msg)
{
   m_ProtocolCheckInit();
   if (mpt_DebugFileHandle != NULL)
   {
      C_SCLString c_Text;
      C_SCLString c_Help;
      C_SCLString c_Time;
      sint32 i;
      c_Time = C_CMONProtocols::FormatTimeStamp(TGL_GetTickCountUS(), false);
      if (u8_CreateCommProtocol == 1U)
      {
         (void)c_Text.PrintFormatted("%s Msg_Read OK: MSG: %03x %d ", c_Time.c_str(), orc_Msg.u32_ID, orc_Msg.u8_DLC);
         for (i = 0; i < orc_Msg.u8_DLC; i++)
         {
            (void)c_Help.PrintFormatted("%02x ", orc_Msg.au8_Data[i]);
            c_Text += c_Help;
         }
         c_Text += "\r\n    ";
      }

      c_Text += (c_Time + " " + mc_Protocol.MessageToString(orc_Msg) + "\r\n");
      fwrite(c_Text.c_str(), 1, c_Text.Length(), mpt_DebugFileHandle);
   }
}
#endif

//---------------------------------------------------------------------------

void C_KFXProtocol::ResetCommProtocol(void)
{
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
   if (mpt_DebugFileHandle != NULL)
   {
      fclose(mpt_DebugFileHandle);
      mpt_DebugFileHandle = NULL;
   }
#endif

   if (mpc_Dispatcher != NULL)
   {
      mpc_Dispatcher->RemoveClient(mu16_CANDispatcherHandle);
   }
}

//---------------------------------------------------------------------------

C_KFXProtocol::~C_KFXProtocol()
{
   this->ResetCommProtocol();
   mpv_ResetCallBackInstance = NULL;
   mpc_Dispatcher = NULL;
   mpt_Config = NULL;
   mpv_CyclicCallbackInstance = NULL;
}

//**************************************************************.FA*
//   .FUNCTION [ C_KFXProtocol::ConfigChanged ]
//   .GROUP [ C_KFXProtocol public functions ]
//   .AUTHOR [ Filser ]
//--------------------------------------------------------------.F1-
//   .DESCRIPTION
//    Someone lets us know that protocol parameters have been changed.
//    -> Reconsider all local parameters
//--------------------------------------------------------------
//   .HISTORY
//    Date       Author      Commentary
//    26.08.10   A.Stangl    fixed RX-filter for block transfers
//    09.02.10   A.Stangl    fixed check for valid ou8_BaseID
//    23.01.08   A.Stangl    created
//**************************************************************.FE*
sint32 C_KFXProtocol::ConfigChanged(void)
{
   uint8 u8_BaseID        = mpt_Config->GetBaseID();
   uint8 u8_ClientAddress = mpt_Config->GetClientAddress();
   uint8 u8_ServerAddress = mpt_Config->GetServerAddress();

   //base-ID must be 0,2,4,6
   if ((u8_BaseID > 6U) || ((u8_BaseID & 0x01U) == 0x01U))
   {
      return C_RANGE;
   }

   mu32_SendIDRQ     = (((static_cast<uint32>(u8_BaseID)) << 8) | ((static_cast<uint32>(u8_ClientAddress)) << 1));
   mu32_ReceiveIDRSP = (((static_cast<uint32>(u8_BaseID)) << 8) | ((static_cast<uint32>(u8_ServerAddress)) << 1)) | 1U;

   if (mpc_Dispatcher != NULL)
   {
      C_CAN_RXFilter c_Filter;
      //we only need exactly one ID
      c_Filter.PassOneID(mu32_ReceiveIDRSP, false, false);
      mpc_Dispatcher->SetRXFilter(mu16_CANDispatcherHandle, c_Filter);
   }

   return C_NO_ERR;
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::m_SendMessage(T_STWCAN_Msg_TX & orc_Msg, const bool oq_SetXtdOff)
{
   if (oq_SetXtdOff == true)
   {
      orc_Msg.u8_XTD = 0U; //KEFEX: always 11bit IDs !
   }
   orc_Msg.u8_RTR = 0U;

   sint32 s32_Return = mpc_Dispatcher->CAN_Send_Msg(orc_Msg);
   if (u8_CreateCommProtocol > 0U)
   {
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
      m_ProtocolSend(s32_Return == C_NO_ERR, orc_Msg);
#endif
   }
   return s32_Return;
}

//---------------------------------------------------------------------------
//02.04.09  project index is simply put into the highest nibble without any checks
//          -> allows to use the highest nibble for the data version for projects
//             that do not require the project-index
sint32 C_KFXProtocol::SendLogonRequest(const uint16 ou16_TypeAddressCRC, const uint8 ou8_ProjectIndex,
                                       const uint16 ou16_DataVersion, const uint16 ou16_NumOfVariables)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if (ou8_ProjectIndex > 0x0FU)
   {
      return C_RANGE;
   }

   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = static_cast<uint8>(KFX_PROTOCOL_LOGON_HS);
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_TypeAddressCRC);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_TypeAddressCRC >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou16_DataVersion);
   c_MSG.au8_Data[5] = static_cast<uint8>((ou16_DataVersion >> 8) | (static_cast<uint8>(ou8_ProjectIndex << 4)));
   c_MSG.au8_Data[6] = static_cast<uint8>(ou16_NumOfVariables);
   c_MSG.au8_Data[7] = static_cast<uint8>(ou16_NumOfVariables >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendLogoffRequest(void)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }

   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 2U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_LOGOFF_HS;
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendSRRRequest(const uint16 ou16_VariableIndex)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 4U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_SRR;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::m_SendSRRRequest(const uint16 ou16_VariableIndex)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 5U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_SRR;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   c_MSG.au8_Data[4] = 0U;
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------
//23.07.02   AST         Low/High bytes of hysteresis were exchanged
sint32 C_KFXProtocol::SendECRRRequest(const uint16 ou16_VariableIndex, const uint16 ou16_MaxWaitTime,
                                      const uint8 ou8_UpperHysteresis,
                                      const uint8 ou8_LowerHysteresis, const uint8 ou8_HysteresisBase)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if ((ou8_UpperHysteresis > 127U) || (ou8_LowerHysteresis > 127U) || (ou8_HysteresisBase  > 3U))
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_ECRR;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou16_MaxWaitTime);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou16_MaxWaitTime >> 8);
   c_MSG.au8_Data[6] = static_cast<uint8>((static_cast<uint8>(ou8_UpperHysteresis << 7)) | ou8_LowerHysteresis);
   c_MSG.au8_Data[7] = static_cast<uint8>((static_cast<uint8>(ou8_HysteresisBase << 6)) | (ou8_UpperHysteresis >> 1));
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendTCRRRequest(const uint16 ou16_VariableIndex, const bool oq_TimeStamped,
                                      const uint16 ou16_Interval)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 6U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   if (oq_TimeStamped == true)
   {
      c_MSG.au8_Data[1] = KFX_PROTOCOL_TCRR_TIMESTAMPED;
   }
   else
   {
      c_MSG.au8_Data[1] = KFX_PROTOCOL_TCRR;
   }
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou16_Interval);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou16_Interval >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendAbortIndividualResponseRequest (const uint16 ou16_VariableIndex, const bool oq_Handshake)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 4U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = static_cast<uint8>((oq_Handshake == false) ? KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE :
                                                                     KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS);
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendAbortAllResponsesRequest (const bool oq_Handshake)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 2U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = static_cast<uint8>((oq_Handshake == false) ? KFX_PROTOCOL_ABORT_ALL_RESPONSES :
                                                                     KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendImmediateWriteRequest (const uint16 ou16_VariableIndex, const uint32 ou32_Value)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = static_cast<uint8>(KFX_PROTOCOL_IMMEDIATE_WRITE_HS);
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_Value);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_Value >> 8);
   c_MSG.au8_Data[6] = static_cast<uint8>(ou32_Value >> 16);
   c_MSG.au8_Data[7] = static_cast<uint8>(ou32_Value >> 24);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendReadDataWordRequest (const uint8 ou8_Type, const uint32 ou32_Address)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if (((ou8_Type != KFX_PROTOCOL_READ_EEPROM) && (ou8_Type != KFX_PROTOCOL_READ_FLASH_EEPROM) &&
        (ou8_Type != KFX_PROTOCOL_READ_SRAM  ) && (ou8_Type != KFX_PROTOCOL_READ_EPROM)) ||
        (ou32_Address > 0xFFFFFFU))
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 6U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = ou8_Type;
   c_MSG.au8_Data[2] = 0x00U;
   c_MSG.au8_Data[3] = static_cast<uint8>(ou32_Address);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_Address >> 8);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_Address >> 16);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendReadDataByteRequest (const uint8 ou8_Type, const uint32 ou32_Address)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if (((ou8_Type != KFX_PROTOCOL_READ_EEPROM) && (ou8_Type != KFX_PROTOCOL_READ_FLASH_EEPROM) &&
        (ou8_Type != KFX_PROTOCOL_READ_SRAM  ) && (ou8_Type != KFX_PROTOCOL_READ_EPROM)) ||
        (ou32_Address > 0xFFFFFFU))
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 6U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = ou8_Type;
   c_MSG.au8_Data[2] = 0x01U;
   c_MSG.au8_Data[3] = static_cast<uint8>(ou32_Address);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_Address >> 8);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_Address >> 16);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendWriteDataWordRequest(const uint8 ou8_Type, const uint32 ou32_Address,
                                               const uint8 oau8_Values[2])
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if (((ou8_Type != KFX_PROTOCOL_WRITE_EEPROM_HS) && (ou8_Type != KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS) &&
        (ou8_Type != KFX_PROTOCOL_WRITE_SRAM_HS)) ||
        (ou32_Address > 0xFFFFFFU))
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = ou8_Type;
   c_MSG.au8_Data[2] = 0x00U;
   c_MSG.au8_Data[3] = static_cast<uint8>(ou32_Address);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_Address >> 8);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_Address >> 16);
   c_MSG.au8_Data[6] = oau8_Values[0];
   c_MSG.au8_Data[7] = oau8_Values[1];
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendWriteDataByteRequest(const uint8 ou8_Type, const uint32 ou32_Address, const uint8 ou8_Value)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if (((ou8_Type != KFX_PROTOCOL_WRITE_EEPROM_HS) && (ou8_Type != KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS) &&
        (ou8_Type != KFX_PROTOCOL_WRITE_SRAM_HS)) ||
        (ou32_Address > 0xFFFFFFU))
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = ou8_Type;
   c_MSG.au8_Data[2] = 0x01U;
   c_MSG.au8_Data[3] = static_cast<uint8>(ou32_Address);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_Address >> 8);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_Address >> 16);
   c_MSG.au8_Data[6] = ou8_Value;
   c_MSG.au8_Data[7] = 0x00U;
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendUpdateTaskRequest (const uint16 ou16_TaskIndex, const bool oq_Handshake)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   if (ou16_TaskIndex > 0x7FFFU)
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 4U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = static_cast<uint8>((oq_Handshake == false) ? KFX_PROTOCOL_UPDATE_TASK : KFX_PROTOCOL_UPDATE_TASK_HS);
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_TaskIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_TaskIndex >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Send WRITE EEPROM SSL START request

   Send request for EEPROM write procedure with CRC over the sent data.

   \param[in]  ou16_NumFollowingWriteServices   number of following WRITE EEPROM services

   \return
   C_NO_ERR  request sent
   C_NOACT   no dispatcher installed
   else      could not send request

   \created     29.10.2014  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_KFXProtocol::SendWriteEepromSslStartRequest(const uint16 ou16_NumFollowingWriteServices)
{
   T_STWCAN_Msg_TX c_MSG;
   uint16 u16_CRC;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_WRITE_EEPROM_SSL;
   c_MSG.au8_Data[2] = 0x01U; //"START"
   c_MSG.au8_Data[3] = 0x00U; //reserved
   c_MSG.au8_Data[4] = static_cast<uint8>(ou16_NumFollowingWriteServices);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou16_NumFollowingWriteServices >> 8);
   u16_CRC = 0x1D0FU; //set CCITT25 start value
   C_SCLChecksums::CalcCRC16(&c_MSG.au8_Data[2], 4, u16_CRC);
   c_MSG.au8_Data[6] = static_cast<uint8>(u16_CRC);
   c_MSG.au8_Data[7] = static_cast<uint8>(u16_CRC >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Send WRITE EEPROM SSL END request

   Send request for confirmation of finished EEPROM write procedure with CRC over the sent data.

   \param[in]  ou16_NumRequestedWriteServices   number of WRITE EEPROM services sent since calling
                                                 SendWriteEepromSslStartRequest
   \param[in]  ou16_CRCOverSentPayload          CRC over payload sent since calling SendWriteEepromSslStartRequest

   \return
   C_NO_ERR  request sent
   C_NOACT   no dispatcher installed
   else      could not send request

   \created     29.10.2014  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_KFXProtocol::SendWriteEepromSslEndRequest(const uint16 ou16_NumRequestedWriteServices,
                                                   const uint16 ou16_CRCOverSentPayload)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;

   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_WRITE_EEPROM_SSL;
   c_MSG.au8_Data[2] = 0x02U; //"END"
   c_MSG.au8_Data[3] = 0x00U; //reserved
   c_MSG.au8_Data[4] = static_cast<uint8>(ou16_NumRequestedWriteServices);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou16_NumRequestedWriteServices >> 8);
   c_MSG.au8_Data[6] = static_cast<uint8>(ou16_CRCOverSentPayload);
   c_MSG.au8_Data[7] = static_cast<uint8>(ou16_CRCOverSentPayload >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//**************************************************************.FA*
//   .FUNCTION [ C_KFXProtocol::m_CalcHysteresis ]
//   .GROUP [ C_KFXProtocol private functions ]
//   .AUTHOR [ Stangl ]
//--------------------------------------------------------------.F1-
//   .DESCRIPTION
//    Adapt an absolute hysteresis value to the format supported
//     by the protocol
//--------------------------------------------------------------
//   .PARAMETERS
//    Bezeichner                        Bedeutung
//    uint32  dwAbsoluteValue    Absolute numeric hysteresis value
//    uint8 *pbBase             Resulting value base (cf. spec)
//    uint8 *pbValue            Resulting value (cf. spec)
//    bool qForce              true -> do not automatically adapt value base
//                                     in this case the submitted *pbBase will be used
//--------------------------------------------------------------
//   .RETURNVALUE
//   sint32   C_NO_ERR  no errors
//            C_RANGE   dwAbsoluteValue out of range
//--------------------------------------------------------------
//   .HISTORY
//    Date       Author      Commentary
//**************************************************************.FE*
sint32 C_KFXProtocol::m_CalcHysteresis(const uint32 ou32_AbsoluteValue, uint8 * const opu8_Base,
                                       uint8 * const opu8_Value, const bool oq_ForceBase) const
{
   sint64 s64_Value;
   float64 f64_Factor;
   s64_Value = ou32_AbsoluteValue;
   if (oq_ForceBase == false)
   {
      if (s64_Value <= 100)
      {
         *opu8_Base = 0U;
      }
      else if (s64_Value <= 10000)
      {
         *opu8_Base = 1U;
      }
      else if (s64_Value <= 1000000)
      {
         *opu8_Base = 2U;
      }
      else if (s64_Value <= 100000000)
      {
         *opu8_Base = 3U;
      }
      else
      {
         return C_RANGE;
      }
   }
   f64_Factor = std::pow(100.0, static_cast<float64>(*opu8_Base));
   if (s64_Value <= static_cast<sint64>(3.0 * f64_Factor))
   {
      *opu8_Value = static_cast<uint8>((static_cast<float64>(s64_Value) * 10.0) / f64_Factor);
   }
   else
   {
      *opu8_Value = static_cast<uint8>((static_cast<float64>(s64_Value) / f64_Factor) + 27.0);
   }
   return C_NO_ERR;
}

//**************************************************************.FA*
//   .FUNCTION [ C_KFXProtocol::SendECRRRequestAbsolute ]
//   .GROUP [ C_KFXProtocol public functions ]
//   .AUTHOR [ Stangl ]
//--------------------------------------------------------------.F1-
//   .DESCRIPTION
//   Send an ECRR request. In contrast to SendECRRRequest the hysteresis
//    values can be specified in absolute numbers.
//--------------------------------------------------------------
//   .PARAMETERS
//    Bezeichner                        Bedeutung
//    uint16  ou16_VariableIndex     Index of requested variable
//    uint16  ou16_MaxWaitTime       Maximum wait time
//    uint32  ou32_UpperHysteresis
//    uint32  ou32_LowerHysteresis
//--------------------------------------------------------------
//   .RETURNVALUE
//   sint32   C_NO_ERR  no errors
//            C_WARN    upper and lower hysteresis require different value bases
//                      but have been forced to use the one of the greater number
//--------------------------------------------------------------
//   .HISTORY
//    Date       Author      Commentary
//    23.07.02   AST         Low/High bytes of hysteresis were exchanged
//**************************************************************.FE*
sint32 C_KFXProtocol::SendECRRRequestAbsolute(const uint16 ou16_VariableIndex, const uint16 ou16_MaxWaitTime,
                                              const uint32 ou32_UpperHysteresis, const uint32 ou32_LowerHysteresis)
{
   T_STWCAN_Msg_TX c_MSG;
   uint8 u8_BaseLow;
   uint8 u8_BaseHigh;
   uint8 u8_ValLow;
   uint8 u8_ValHigh;
   sint32 s32_Return;
   sint32 s32_Return2;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   s32_Return = m_CalcHysteresis(ou32_UpperHysteresis, &u8_BaseHigh, &u8_ValHigh, false);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   s32_Return = m_CalcHysteresis(ou32_LowerHysteresis, &u8_BaseLow, &u8_ValLow, false);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   if (u8_BaseHigh != u8_BaseLow)
   {
      if (ou32_LowerHysteresis > ou32_UpperHysteresis)
      {
         u8_BaseHigh = u8_BaseLow;
         s32_Return = m_CalcHysteresis(ou32_UpperHysteresis, &u8_BaseHigh, &u8_ValHigh, true);
         if (s32_Return != C_NO_ERR)
         {
            return s32_Return;
         }
      }
      else
      {
         u8_BaseLow = u8_BaseHigh;
         s32_Return = m_CalcHysteresis(ou32_LowerHysteresis, &u8_BaseLow, &u8_ValLow, true);
         if (s32_Return != C_NO_ERR)
         {
            return s32_Return;
         }
      }
      s32_Return = C_WARN;
   }
   if ((u8_ValHigh > 127U) || (u8_ValLow > 127U) || (u8_BaseLow > 3U))
   {
      return C_RANGE;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_ECRR;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou16_MaxWaitTime);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou16_MaxWaitTime >> 8);
   c_MSG.au8_Data[6] = static_cast<uint8>((static_cast<uint8>(u8_ValHigh << 7)) | u8_ValLow);
   c_MSG.au8_Data[7] = static_cast<uint8>((static_cast<uint8>(u8_BaseLow << 6)) | (u8_ValHigh >> 1));
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   if (s32_Return == C_NO_ERR)
   {
      return m_SendMessage(c_MSG);
   }
   s32_Return2 = m_SendMessage(c_MSG);
   if (s32_Return2 != C_NO_ERR)
   {
      return s32_Return2;
   }
   return s32_Return;
}

//--------------------------------------------------------------

void C_KFXProtocol::m_SetLastResponse(const uint8 ou8_Service, const uint32 ou32_Index, const uint8 * const opu8_Data,
                                      const uint8 ou8_Size, const bool oq_Error)
{
   if (mc_LastResponse.q_NewValue == true)
   {
      mc_LastResponse.q_Overrun = true;
   }
   mc_LastResponse.u8_Service = ou8_Service;
   mc_LastResponse.u32_IndexOrAddress = ou32_Index;
   if (opu8_Data != NULL)
   {
      memcpy(&mc_LastResponse.au8_Data[0], &opu8_Data[0], ou8_Size);
   }
   else
   {
      mc_LastResponse.au8_Data[0] = 0;
      mc_LastResponse.au8_Data[1] = 0;
      mc_LastResponse.au8_Data[2] = 0;
      mc_LastResponse.au8_Data[3] = 0;
      mc_LastResponse.au8_Data[4] = 0;
      mc_LastResponse.au8_Data[5] = 0;
      mc_LastResponse.au8_Data[6] = 0;
   }
   mc_LastResponse.u8_Size    = ou8_Size;
   mc_LastResponse.q_NewValue = true;
   mc_LastResponse.q_Error    = oq_Error;
}

//--------------------------------------------------------------

void C_KFXProtocol::m_GetLastResponse(C_KFXProcotolResponse & orc_Response)
{
   orc_Response = mc_LastResponse;
   mc_LastResponse.q_NewValue = false;
   mc_LastResponse.q_Overrun  = false;
}

//--------------------------------------------------------------
//    15.11.2010  AST         Preset q_SomeResponse to false. Fixes random erroneous C_NO_ERR returns when there are
//                             no responses (depending on stack content).
//    07.05.2010  AST         Reworked to use CANDispatcher instead of messed up callback mechanism
//    15.11.2004  AST         Another Lockfix ...
//    13.09.2004  AST         Lock will be released as often as aquired
//                             (caused problems when called in thread)
//return value:
// C_NO_ERR -> expected message received (or: at least one message received if oq_CheckExpectedService == false)
// C_WARN   -> expected message not received (or: no new messages if oq_CheckExpectedService == false)
sint32 C_KFXProtocol::m_EvalResponses(const uint16 ou16_TimeOut, const uint8 ou8_ExpectedService,
                                      const bool oq_CheckExpectedService, C_KFXProcotolResponse & orc_Response)
{
   T_STWCAN_Msg_RX c_MSG;
   uint32 u32_OldTime;
   sint32 s32_Return;
   bool q_SomeResponse = false;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   u32_OldTime = TGL_GetTickCount();
   while(true)
   {
      (void)mpc_Dispatcher->DispatchIncoming();
      s32_Return = C_NO_ERR;
      while (s32_Return == C_NO_ERR) //make sure to read until there are no more new messages.
      {
         s32_Return = mpc_Dispatcher->ReadFromQueue(mu16_CANDispatcherHandle, c_MSG);
         if (s32_Return == C_NO_ERR)
         {
            q_SomeResponse = this->m_MessageReceived(c_MSG);
            if (q_SomeResponse == true)
            {
               if (oq_CheckExpectedService == true)
               {
                  if ((ou8_ExpectedService == mc_LastResponse.u8_Service) && (mc_LastResponse.q_NewValue == true))
                  {
                     //it's a response to a confirmed service exactly as we like it
                     m_GetLastResponse(orc_Response);
                     return C_NO_ERR;
                  }
                  else
                  {
                      //not what we want !
                      q_SomeResponse = false;
                  }
               }

               //if it's a cyclic transmission it will already have been added to the buffer by "m_MessageReceived"
            }
         }
      }

      if ((TGL_GetTickCount() - u32_OldTime) > static_cast<uint32>(ou16_TimeOut))
      {
         break;
      }

      if (mq_SleepBetweenPolling == true)
      {
         stw_tgl::TGL_Sleep(0U); //rescind CPU time to other threads ...
      }
   }
   if (q_SomeResponse == false)
   {
      return C_WARN;
   }
   return C_NO_ERR;
}

//--------------------------------------------------------------

void C_KFXProtocol::EvaluateAllResponses(void)
{
   C_KFXProcotolResponse c_Dummy;
   (void)m_EvalResponses(0U, 0U, false, c_Dummy);
}

//--------------------------------------------------------------

//Read responses for a maximum of wTimeout ms but return immediately if
// one service of type bService is reveived
sint32 C_KFXProtocol::EvaluateResponses(const uint8 ou8_Service, C_KFXProcotolResponse & orc_Response)
{
   return m_EvalResponses(mpt_Config->GetTimeout(), ou8_Service, true, orc_Response);
}

//--------------------------------------------------------------

bool C_KFXProtocol::m_MessageReceived(const T_STWCAN_Msg_RX & orc_MSG)
{
   uint8 u8_Size;
   uint8 u8_Service;
   bool q_Error;
   uint8 au8_Data[4];
   uint32 u32_Address;
   bool q_IsTimeStamped;

   if (orc_MSG.u32_ID == mu32_ReceiveIDRSP)
   {
      //it is a response !
      if (u8_CreateCommProtocol > 0U)
      {
#ifndef DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING
         m_ProtocolReceive(orc_MSG);
#endif
      }

      u8_Service = static_cast<uint8>(orc_MSG.au8_Data[0] & 0x7FU);
      q_Error = ((orc_MSG.au8_Data[0] & 0x80U) == 0x80U) ? true : false;
      switch (u8_Service)
      {
      case KFX_PROTOCOL_SRR:          //no break (same format)
      case KFX_PROTOCOL_SERVICE_READ: //no break (same format)
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[1] + ((static_cast<uint32>(orc_MSG.au8_Data[2])) << 8), //index
                           &orc_MSG.au8_Data[3], //value
                           4U, q_Error);
         break;
      case KFX_PROTOCOL_SRR_FF:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[1] + ((static_cast<uint32>(orc_MSG.au8_Data[2])) << 8), //index
                           &orc_MSG.au8_Data[3], //numbytes + parameter + first two bytes
                           5U, q_Error);
         break;
      case KFX_PROTOCOL_SRR_CF:
         m_SetLastResponse(u8_Service,
                           0U, //index N/A
                           &orc_MSG.au8_Data[1], //block number + six bytes
                           7U, q_Error);
         break;
      case KFX_PROTOCOL_IWR_FC:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[1] + ((static_cast<uint32>(orc_MSG.au8_Data[2])) << 8), //index
                           &orc_MSG.au8_Data[3], //blocksize + stmin
                           2U, q_Error);
         break;
      case KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[1] + ((static_cast<uint32>(orc_MSG.au8_Data[2])) << 8), //index
                           NULL,
                           0U, false);
         break;
      case KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS:
         m_SetLastResponse(u8_Service, 0U, NULL, 0U, false);
         break;
      case KFX_PROTOCOL_IMMEDIATE_WRITE_HS: //no break (same format)
      case KFX_PROTOCOL_SERVICE_WRITE:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[1] + ((static_cast<uint32>(orc_MSG.au8_Data[2])) << 8), //index
                           &orc_MSG.au8_Data[3], //possible error code
                           2U, q_Error);
         break;
      //response looks exactly the same for read and write(w/hs)
      case KFX_PROTOCOL_READ_EEPROM:       //no break
      case KFX_PROTOCOL_READ_FLASH_EEPROM: //no break
      case KFX_PROTOCOL_READ_SRAM:         //no break
      case KFX_PROTOCOL_READ_EPROM:        //no break
      case KFX_PROTOCOL_WRITE_EEPROM_HS:   //no break
      case KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS: //no break
      case KFX_PROTOCOL_WRITE_SRAM_HS:     //no break
         if (orc_MSG.au8_Data[1] == 0U)
         {
            u8_Size = 2U;
            au8_Data[0] = orc_MSG.au8_Data[5];
            au8_Data[1] = orc_MSG.au8_Data[6];
         }
         else
         {
            u8_Size = 1U;
            au8_Data[0] = orc_MSG.au8_Data[5];
         }
         if (q_Error == true)
         {
            u8_Size = 2U;
            au8_Data[0] = orc_MSG.au8_Data[3];
            au8_Data[1] = orc_MSG.au8_Data[4];
            u32_Address = 0U;
         }
         else
         {
            u32_Address = orc_MSG.au8_Data[2] + (((static_cast<uint32>(orc_MSG.au8_Data[3])) << 8) +
                                                 ((static_cast<uint32>(orc_MSG.au8_Data[4])) << 16));
         }
         m_SetLastResponse(u8_Service,
                           u32_Address, //address
                           &au8_Data[0], //value(s) or error
                           u8_Size, q_Error);
         break;
      case KFX_PROTOCOL_UPDATE_TASK_HS:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[2],
                           &orc_MSG.au8_Data[3], //possible error code,
                           0U, q_Error);
         break;
      case KFX_PROTOCOL_LOGOFF_HS:
         m_SetLastResponse(u8_Service,
                           0U,                  //no Index
                           &orc_MSG.au8_Data[3],    //possible error code
                           2U, q_Error);
         break;
      case KFX_PROTOCOL_LOGON_HS:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[5] + ((static_cast<uint32>(orc_MSG.au8_Data[6])) << 8), //num of variables
                           &orc_MSG.au8_Data[1], //CRC and Versionnumber or Errorcode
                           6U, q_Error);
         break;
      case KFX_PROTOCOL_STARTUP_INDICATION:
         m_SetLastResponse(u8_Service, 0, NULL, 0, false);
         if (mpr_ResetCallBack != NULL)
         {
            mpr_ResetCallBack(mpv_ResetCallBackInstance);
         }
         break;
      case KFX_PROTOCOL_ECRR:
      case KFX_PROTOCOL_TCRR:
      case KFX_PROTOCOL_TCRR_TIMESTAMPED:
         uint32 u32_TimeStamp;
         sint64 s64_Value;
         uint16 u16_Index;
         u16_Index = static_cast<uint16>(orc_MSG.au8_Data[1] +
                   (static_cast<uint16>(static_cast<uint16>(orc_MSG.au8_Data[2]) << 8)));
         if (u8_Service == KFX_PROTOCOL_TCRR_TIMESTAMPED)
         {
            s64_Value = orc_MSG.au8_Data[3] + (static_cast<uint16>(static_cast<uint16>(orc_MSG.au8_Data[4]) << 8));
            u32_TimeStamp = orc_MSG.au8_Data[5] + (static_cast<uint32>(orc_MSG.au8_Data[6]) << 8) +
                                                  (static_cast<uint32>(orc_MSG.au8_Data[7])<< 16);
            q_IsTimeStamped = true;
         }
         else
         {
            s64_Value = static_cast<uint32>(orc_MSG.au8_Data[3])         +
                      ((static_cast<uint32>(orc_MSG.au8_Data[4])) << 8)  +
                      ((static_cast<uint32>(orc_MSG.au8_Data[5])) << 16) +
                      ((static_cast<uint32>(orc_MSG.au8_Data[6])) << 24);
            u32_TimeStamp = 0U;
            q_IsTimeStamped = false;
         }
         if (mpr_OnNewCyclicTransmissionReceived != NULL)
         {
            mpr_OnNewCyclicTransmissionReceived(mpv_CyclicCallbackInstance, u16_Index, s64_Value, u32_TimeStamp,
                                                q_IsTimeStamped, q_Error);
         }
         break;
      case KFX_PROTOCOL_WRITE_EEPROM_SSL:
         m_SetLastResponse(u8_Service,
                           orc_MSG.au8_Data[4] + ((static_cast<uint32>(orc_MSG.au8_Data[5])) << 8), //num of services
                           &orc_MSG.au8_Data[1], //subservice + payload
                           7U, q_Error);
         break;
      default:
         break; //not interesting for us
      }
      return true;
   }
   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read data via segmented transfer functionality.

   This function will determine which type of segmented transport mechanism is configured and use the corresponding
   functionality.

   For regular segmented transfer the functionality is described here:

   If there is ongoing asynchronuous communication in the background (TCRRS/ECRRS) corresponding incoming messages are
   submitted to the corresponding handler functions.
   This function will actively wait for the response data.

   \param[in]     ou16_Index              variable index
   \param[in]     ou32_NumBytesExpected   number of bytes expected in response
   \param[out]    opu8_Data               read data (caller has to provide a buffer of ou32_NumBytesExpected bytes)
   \param[out]    opu16_Error             error code if receiving an error response (NULL -> don't return error code)

   \return
   C_RANGE        opu8_Data is NULL                              \n
   C_NO_ERR       variable read                                  \n
   C_WARN         error response (opu16_Error contains the code) \n
   C_COM          no response                                    \n
   C_RD_WR        TX communication error or unexpected response  \n
   C_CONFIG       BSmax is 0

   \created     xx.xx.20xx  STW/A.Stangl

   \internal
   \history
   Date(dd.mm.yyyy)  Author        Description
   xx.xx.20xx        STW/A.Stangl  function created
   \endhistory
*/
//-----------------------------------------------------------------------------
sint32 C_KFXProtocol::SegmentedSRRTransfer(const uint16 ou16_Index, const uint32 ou32_NumBytesExpected,
                                           uint8 * const opu8_Data, uint16 * const opu16_Error)
{
   sint32 s32_Return;
   T_STWCAN_Msg_TX t_MSG;
   sint32 i;
   uint32 u32_NumMessagesExpected;
   uint32 u32_NumMessagesReceived;
   uint32 u32_NumBytesReported;
   uint32 u32_NumBytesLeft;
   uint8 u8_NextFCRequired;
   uint8 * pu8_Act;
   C_KFXProcotolResponse c_Service;

   if (opu8_Data == NULL)
   {
      return C_RANGE;
   }

   if (mpt_Config->GetBSMax() == 0)
   {
      return C_CONFIG;
   }

   //first send a "normal" SRR request
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   s32_Return = this->m_SendSRRRequest(ou16_Index);
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }
   //now we expect a FF from the controller:
   s32_Return = this->EvaluateResponses(KFX_PROTOCOL_SRR_FF, c_Service);
   if ((s32_Return != C_NO_ERR) || (ou16_Index != static_cast<uint16>(c_Service.u32_IndexOrAddress)))
   {
      return C_RD_WR;
   }

   if (c_Service.q_Error == true)
   {
      if (opu16_Error != NULL)
      {
         *opu16_Error = c_Service.au8_Data[0] + (static_cast<uint16>((static_cast<uint16>(c_Service.au8_Data[1])) << 8));
      }
      return C_WARN;
   }

   u32_NumBytesReported = c_Service.au8_Data[0] + ((static_cast<uint32>(c_Service.au8_Data[1])) << 8) +
                                                  ((static_cast<uint32>(c_Service.au8_Data[2])) << 16);
   if (u32_NumBytesReported != ou32_NumBytesExpected)
   {
      return C_RD_WR;
   }
   u32_NumMessagesExpected = ((ou32_NumBytesExpected - 1U) + 5U) / 6U;
   u32_NumBytesLeft = u32_NumBytesReported;

   //copy the first byte to the buffer:
   pu8_Act = opu8_Data;
   pu8_Act[0] = c_Service.au8_Data[4];
   pu8_Act++;
   u32_NumBytesLeft--;

   if (u32_NumBytesLeft == 0U)
   {
      return C_NO_ERR;
   }

   //send the first FC:
   t_MSG.u32_ID = mu32_SendIDRQ;
   t_MSG.u8_DLC = 7U;
   t_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   t_MSG.au8_Data[1] = KFX_PROTOCOL_SRR_FC;
   t_MSG.au8_Data[2] = static_cast<uint8>(ou16_Index);
   t_MSG.au8_Data[3] = static_cast<uint8>(ou16_Index >> 8);
   t_MSG.au8_Data[4] = mpt_Config->GetBSMax();
   t_MSG.au8_Data[5] = mpt_Config->GetSTMin();
   t_MSG.au8_Data[6] = 0U; //parameter (unused)
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   s32_Return = m_SendMessage(t_MSG);
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }
   u8_NextFCRequired = mpt_Config->GetBSMax();

   //now expect all the data frames and send another FC whenever we have received bBSMax messages
   u32_NumMessagesReceived = 0U;
   for (i = 0; i < static_cast<sint32>(u32_NumMessagesExpected); i++)
   {
      //ProcessMessages is handled within EvaluateResponses ...
      s32_Return = this->EvaluateResponses(KFX_PROTOCOL_SRR_CF, c_Service);
      if (s32_Return != C_NO_ERR)
      {
         return C_RD_WR;
      }

      if (c_Service.q_Error == true)
      {
         return C_WARN;
      }

      if (c_Service.au8_Data[0] != static_cast<uint8>(u32_NumMessagesReceived))
      {
         return C_RD_WR; //block number wrong value ...
      }

      (void)memcpy(pu8_Act, &c_Service.au8_Data[1], (u32_NumBytesLeft > 6U) ? 6U : u32_NumBytesLeft);
      u32_NumBytesLeft -= 6U;
      u32_NumMessagesReceived++;
      //stop Codeguard from complaining
      if (u32_NumMessagesReceived != u32_NumMessagesExpected)
      {
         pu8_Act += 6;
      }
      u8_NextFCRequired--;
      if ((u8_NextFCRequired == 0U) && (i != static_cast<sint32>(u32_NumMessagesExpected - 1U)))
      {
         //now we must send another FC ...
         mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
         s32_Return = m_SendMessage(t_MSG);
         if (s32_Return != C_NO_ERR)
         {
            return C_RD_WR;
         }
         u8_NextFCRequired = mpt_Config->GetBSMax();
      }
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------
//C_RANGE        -> opu8_Data is NULL; invalid ou32_NumBytes      \n
//C_NO_ERR       -> variable written                              \n
//C_WARN         -> error response                                \n
//C_COM          -> no response                                   \n
//C_RD_WR        -> TX communication error or unexpected response \n
//27.08.03 added retry on send failure
sint32 C_KFXProtocol::SegmentedIWRTransfer(const uint16 ou16_Index, const uint32 ou32_NumBytes,
                                           const uint8 * opu8_Data)
{
   //Sequence:
   //1. FF,
   //2. expect FC,
   //3. BS * CF,
   //4. FC
   //continue with 3. until all frames are sent
   T_STWCAN_Msg_TX c_MSG;
   uint8 u8_STMin;
   C_KFXProcotolResponse c_Service;
   uint32 u32_NumMessagesTotal;
   uint32 u32_NumMessagesSent;
   uint32 u32_NumBytesLeft;
   uint8 u8_NextFCRequired;
   sint32 s32_Return;
   sint32 i;
   sint32 j;

   if (opu8_Data == NULL)
   {
      return C_RANGE;
   }

   if (ou32_NumBytes > 0xFFFFFFUL) //maximum possibly with KEFEX protocol: 16MB in a block
   {
      return C_RANGE;
   }

   //first send the first frame
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_IWR_FF;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_Index);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_Index >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_NumBytes);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_NumBytes >> 8);
   c_MSG.au8_Data[6] = static_cast<uint8>(ou32_NumBytes >> 16);
   c_MSG.au8_Data[7] = 0U; //parameter (unused)
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   s32_Return = m_SendMessage(c_MSG);
   if (s32_Return != C_NO_ERR)
   {
      return C_RD_WR;
   }

   //expect FC
   s32_Return = this->EvaluateResponses(KFX_PROTOCOL_IWR_FC, c_Service);
   if ((s32_Return != C_NO_ERR) || (ou16_Index != static_cast<uint16>(c_Service.u32_IndexOrAddress)))
   {
      return C_RD_WR;
   }

   if (c_Service.q_Error == true)
   {
      return C_WARN;
   }

   u8_NextFCRequired = c_Service.au8_Data[0];
   u8_STMin          = c_Service.au8_Data[1];

   u32_NumMessagesTotal = (ou32_NumBytes + 4U) / 5U;
   u32_NumMessagesSent  = 0U;
   u32_NumBytesLeft = ou32_NumBytes;
   for (i = 0; i < static_cast<sint32>(u32_NumMessagesTotal); i++)
   {
      //send CF
      c_MSG.u32_ID = mu32_SendIDRQ;
      c_MSG.u8_DLC = 8U;
      c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
      c_MSG.au8_Data[1] = KFX_PROTOCOL_IWR_CF;
      c_MSG.au8_Data[2] = static_cast<uint8>(u32_NumMessagesSent);
      (void)memcpy(&c_MSG.au8_Data[3], opu8_Data, (u32_NumBytesLeft > 5) ? 5 : u32_NumBytesLeft);
      u32_NumBytesLeft -= 5U;

      //stop Codeguard from complaining
      if (i < static_cast<sint32>(u32_NumMessagesTotal - 1U))
      {
         opu8_Data += 5;
      }

      mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
      s32_Return = m_SendMessage(c_MSG);
      if (s32_Return != C_NO_ERR)
      {
         return C_RD_WR;
      }

      u8_NextFCRequired--;
      u32_NumMessagesSent++;
      if ((u8_NextFCRequired == 0U) && (i != static_cast<sint32>(u32_NumMessagesTotal - 1U)))
      {
         //get next FC
         s32_Return = this->EvaluateResponses(KFX_PROTOCOL_IWR_FC, c_Service);
         if ((s32_Return != C_NO_ERR) || (ou16_Index != static_cast<uint16>(c_Service.u32_IndexOrAddress)))
         {
            return C_RD_WR;
         }

         if (c_Service.q_Error == true)
         {
            return C_WARN;
         }

         u8_NextFCRequired = c_Service.au8_Data[0];
         u8_STMin          = c_Service.au8_Data[1];
      }
      if (u8_STMin == 0U)
      {
         TGL_HandleSystemMessages();
      }
      for (j = 0; j < u8_STMin; j++)
      {
         TGL_HandleSystemMessages();
         TGL_DelayUs(1000);
      }
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendServiceReadRequest(const uint16 ou16_VariableIndex)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 4U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_SERVICE_READ;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

sint32 C_KFXProtocol::SendServiceWriteRequest(const uint16 ou16_VariableIndex, const uint32 ou32_Value)
{
   T_STWCAN_Msg_TX c_MSG;
   if (mpc_Dispatcher == NULL)
   {
      return C_NOACT;
   }
   c_MSG.u32_ID = mu32_SendIDRQ;
   c_MSG.u8_DLC = 8U;
   c_MSG.au8_Data[0] = mpt_Config->GetServerAddress();
   c_MSG.au8_Data[1] = KFX_PROTOCOL_SERVICE_WRITE;
   c_MSG.au8_Data[2] = static_cast<uint8>(ou16_VariableIndex);
   c_MSG.au8_Data[3] = static_cast<uint8>(ou16_VariableIndex >> 8);
   c_MSG.au8_Data[4] = static_cast<uint8>(ou32_Value);
   c_MSG.au8_Data[5] = static_cast<uint8>(ou32_Value >> 8);
   c_MSG.au8_Data[6] = static_cast<uint8>(ou32_Value >> 16);
   c_MSG.au8_Data[7] = static_cast<uint8>(ou32_Value >> 24);
   mc_LastResponse.q_NewValue = false; //we don't have a response yet ...
   return m_SendMessage(c_MSG);
}

//---------------------------------------------------------------------------

void C_KFXProtocol::InstallECUResetHandler(const PR_KFXProtECUResetCallBack opr_Callback,
                                           void * const opv_InstancePointer)
{
   mpr_ResetCallBack         = opr_Callback;
   mpv_ResetCallBackInstance = opv_InstancePointer;
}

//--------------------------------------------------------------

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
void C_KFXProtocol::SetKEFEXVariableInfo(const C_KFXVariableLists * const opt_KFXLists, const uint16 ou16_ListOffset)
{
   (void)mc_Protocol.KFXSetVariableInfo(opt_KFXLists, ou16_ListOffset);
}
#endif

//--------------------------------------------------------------

void C_KFXProtocol::SetCommDispatcher(stw_can::C_CAN_Dispatcher * const opc_Dispatcher)
{
   if (mpc_Dispatcher != NULL)
   {
      (void)mpc_Dispatcher->RemoveClient(mu16_CANDispatcherHandle);
   }

   mpc_Dispatcher = opc_Dispatcher;
   if (mpc_Dispatcher != NULL)
   {
      (void)mpc_Dispatcher->RegisterClient(mu16_CANDispatcherHandle, NULL);
   }
}

//--------------------------------------------------------------

