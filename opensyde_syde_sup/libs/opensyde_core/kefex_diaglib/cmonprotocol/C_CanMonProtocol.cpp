#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstring>
#include "C_CanMonProtocolTarget.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocol.hpp"
#include "C_CanMonProtocolCanOpen.hpp"
#include "C_CanMonProtocolKefex.hpp"
#include "C_CanMonProtocolShipIpIva.hpp"
#include "C_CanMonProtocolGd.hpp"
#include "C_CanMonProtocolStwFf.hpp"
#include "C_CanMonProtocolXfl.hpp"
#include "C_CanMonProtocolL2.hpp"
#include "C_CanMonProtocolJ1939.hpp"
#include "C_CanMonProtocolOpenSyde.hpp"

#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"
#include "TglFile.hpp"
#include "TglUtils.hpp"

//---------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::can;

//---------------------------------------------------------------------------

int32_t C_CanMonProtocols::GetProtocolName(const e_CanMonL7Protocols oe_L7Protocol, C_SclString & orc_Description) const
{
   int32_t s32_Return = C_NO_ERR;

   if (static_cast<int32_t>(oe_L7Protocol) < gs32_KFX_CMON_NUM_PROTOCOLS)
   {
      orc_Description = "L7-Protocol: " + mapc_Protocols[oe_L7Protocol]->GetProtocolName();
   }
   else
   {
      orc_Description = "Protocol: Unknown";
      s32_Return = C_RANGE;
   }
   return s32_Return;
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocols::MessageToString(const T_STWCAN_Msg_TX & orc_Msg) const
{
   T_STWCAN_Msg_RX c_Msg;

   c_Msg.u32_ID = orc_Msg.u32_ID;
   c_Msg.u8_XTD = orc_Msg.u8_XTD;
   c_Msg.u8_DLC = orc_Msg.u8_DLC;
   c_Msg.u8_RTR = orc_Msg.u8_RTR;
   (void)std::memcpy(c_Msg.au8_Data, orc_Msg.au8_Data, 8U);
   c_Msg.u64_TimeStamp = 0;
   return this->MessageToString(c_Msg);
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocols::MessageToStringLog(const T_STWCAN_Msg_TX & orc_Msg) const
{
   T_STWCAN_Msg_RX c_Msg;

   c_Msg.u32_ID = orc_Msg.u32_ID;
   c_Msg.u8_XTD = orc_Msg.u8_XTD;
   c_Msg.u8_DLC = orc_Msg.u8_DLC;
   c_Msg.u8_RTR = orc_Msg.u8_RTR;
   (void)std::memcpy(c_Msg.au8_Data, orc_Msg.au8_Data, 8);
   c_Msg.u64_TimeStamp = 0U;
   return this->MessageToStringLog(c_Msg);
}

//---------------------------------------------------------------------------
//for logging to file; the layer 2 part has a different format than the one for the screen
// (mainly separating ";" inserted for easier parsing)
C_SclString C_CanMonProtocols::MessageToStringLog(const T_STWCAN_Msg_RX & orc_Msg) const
{
   const uint16_t u16_MAX_CHARS_RAW_DATA = 73U;

   C_SclString c_Text;
   C_SclString c_Help;
   int32_t s32_Index;
   uint8_t u8_Len;
   bool q_Decimal = GetDecimalMode();

   u8_Len = (orc_Msg.u8_DLC > 8) ? static_cast<uint8_t>(8U) : orc_Msg.u8_DLC;
   if (q_Decimal == true)
   {
      (void)c_Text.PrintFormatted("%8d; %s; %s; %d; ", orc_Msg.u32_ID, (orc_Msg.u8_XTD == 1U) ? "29B" : "11B",
                                  (orc_Msg.u8_RTR == 1U) ? "RTR" : "STD", orc_Msg.u8_DLC);
      for (s32_Index = 0; s32_Index < u8_Len; s32_Index++)
      {
         (void)c_Help.PrintFormatted("%3d; ", orc_Msg.au8_Data[s32_Index]);
         c_Text += c_Help;
      }
   }
   else
   {
      (void)c_Text.PrintFormatted("0x%08X; %s; %s; %d; ", orc_Msg.u32_ID, (orc_Msg.u8_XTD == 1U) ? "29B" : "11B",
                                  (orc_Msg.u8_RTR == 1U) ? "RTR" : "STD", orc_Msg.u8_DLC);
      for (s32_Index = 0; s32_Index < u8_Len; s32_Index++)
      {
         (void)c_Help.PrintFormatted("0x%02X; ", orc_Msg.au8_Data[s32_Index]);
         c_Text += c_Help;
      }
   }
   for (; s32_Index < 8; s32_Index++)
   {
      c_Text += ";";
   }

   if (me_ActiveProtocol != CMONL7ProtocolNone)
   {
      if (c_Text.Length() < u16_MAX_CHARS_RAW_DATA)
      {
         c_Text = c_Text + C_SclString::StringOfChar(' ', u16_MAX_CHARS_RAW_DATA - c_Text.Length());
      }
      c_Text += (MessageToString(orc_Msg) + ";");
   }
   return c_Text;
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocols::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text;

   c_Text = "";
   if (static_cast<int32_t>(me_ActiveProtocol) < gs32_KFX_CMON_NUM_PROTOCOLS)
   {
      c_Text = mapc_Protocols[me_ActiveProtocol]->MessageToString(orc_Msg);
   }
   if (c_Text == "")
   {
      //get L2 representation
      c_Text = mapc_Protocols[CMONL7ProtocolNone]->MessageToString(orc_Msg);
   }
   return c_Text;
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocols::MessageToString(const T_STWCAN_Msg_RX & orc_Message, const uint32_t ou32_Count) const
{
   C_SclString c_Text;
   C_SclString c_Help;

   c_Text = this->MessageToString(orc_Message);
   //count
   (void)c_Help.PrintFormatted("%7d  ", ou32_Count);
   return (c_Help + c_Text);
}

//---------------------------------------------------------------------------

C_CanMonProtocols::C_CanMonProtocols() :
   mq_Decimal(false),
   me_ActiveProtocol(CMONL7ProtocolNone)
{
   //set shortcut pointers to be able to iterate:
   mapc_Protocols[CMONL7ProtocolNone]         = &mc_ProtocolL2;
   mapc_Protocols[CMONL7ProtocolCANopen]      = &mc_ProtocolCANopen;
   mapc_Protocols[CMONL7ProtocolKEFEX]        = &mc_ProtocolKEFEX;
   mapc_Protocols[CMONL7ProtocolXFL]          = &mc_ProtocolXFL;
   mapc_Protocols[CMONL7ProtocolSTWFF]        = &mc_ProtocolSTWFF;
   mapc_Protocols[CMONL7ProtocolGD]           = &mc_ProtocolGD;
   mapc_Protocols[CMONL7Protocol_SHIP_IP_IVA] = &mc_ProtocolSHIP_IP_IVA;
   mapc_Protocols[CMONL7ProtocolJ1939]        = &mc_ProtocolJ1939;
   mapc_Protocols[CMONL7ProtocolOpenSYDE]     = &mc_ProtocolOpenSYDE;
}

//---------------------------------------------------------------------------

C_CanMonProtocols::~C_CanMonProtocols(void)
{
}

//---------------------------------------------------------------------------

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
sint32 C_CMONProtocols::KFXSetVariableInfo(const stw::diag_lib::C_KFXVariableLists * const opc_Lists,
                                           const uint16 ou16_ListOffset)
{
   mc_ProtocolKEFEX.SetVariableInfo(opc_Lists, ou16_ListOffset);
   return C_NO_ERR;
}
#endif

//---------------------------------------------------------------------------

e_CanMonL7Protocols C_CanMonProtocols::GetProtocolMode(void) const
{
   return me_ActiveProtocol;
}

//---------------------------------------------------------------------------

int32_t C_CanMonProtocols::SetProtocolMode(const e_CanMonL7Protocols oe_L7Protocol)
{
   me_ActiveProtocol = oe_L7Protocol;
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

bool C_CanMonProtocols::GetDecimalMode(void) const
{
   return mq_Decimal;
}

//---------------------------------------------------------------------------

int32_t C_CanMonProtocols::SetDecimalMode(const bool oq_Decimal)
{
   int32_t s32_Loop;

   mq_Decimal = oq_Decimal;

   //set in all  protocol instances:
   for (s32_Loop = 0; s32_Loop < gs32_KFX_CMON_NUM_PROTOCOLS; s32_Loop++)
   {
      mapc_Protocols[s32_Loop]->SetDecimal(oq_Decimal);
   }

   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Write protocol specific parameters to INI

   Write protocol specific parameters for all known protocols to a .INI file.
   Can be read back later with "LoadProtocolParametersFromINI".
   All parameters are written regardless of the active L7-protocol mode.
   No leading ".\\" will be added if no full path is given !

   \param[in]     orc_FileName    full file path to the .ini file
   \param[in]     orc_Section     INI file section to write to

   \return
   C_NO_ERR  -> written
   C_RD_WR   -> could not write
*/
//-----------------------------------------------------------------------------
int32_t C_CanMonProtocols::SaveProtocolParametersToINI(const C_SclString & orc_FileName,
                                                       const C_SclString & orc_Section)
const
{
   int32_t s32_Return;
   int32_t s32_Loop;

   try
   {
      C_SclIniFile c_IniFile(orc_FileName);
      s32_Return = 0;
      for (s32_Loop = 0; s32_Loop < gs32_KFX_CMON_NUM_PROTOCOLS; s32_Loop++)
      {
         s32_Return += mapc_Protocols[s32_Loop]->SaveParamsToIni(c_IniFile, orc_Section);
      }
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RD_WR;
      }
   }
   catch (...)
   {
      s32_Return = C_RD_WR;
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Read protocol specific parameters from INI

   Load protocol specific parameters for all known protocols from a .INI file.
   All parameters are read regardless of the active L7-protocol mode.
   No leading ".\\" will be added if no full path is given !

   \param[in]     orc_FileName    full file path to the .ini file
   \param[in]     orc_Section     INI file section to read from

   \return
   C_NO_ERR  -> written
   C_RD_WR   -> could not read (file does not exist)
*/
//-----------------------------------------------------------------------------
int32_t C_CanMonProtocols::LoadProtocolParametersFromINI(const C_SclString & orc_FileName,
                                                         const C_SclString & orc_Section)
const
{
   int32_t s32_Return;
   int32_t s32_Loop;

   if (TglFileExists(orc_FileName) == false)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      C_SclIniFile c_IniFile(orc_FileName);

      s32_Return  = 0;
      for (s32_Loop = 0; s32_Loop < gs32_KFX_CMON_NUM_PROTOCOLS; s32_Loop++)
      {
         s32_Return += mapc_Protocols[s32_Loop]->LoadParamsFromIni(c_IniFile, orc_Section);
      }
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Return micro-second time stamp as string

   \param[in]     ou64_TimeStampUs  timestamps [us]
   \param[in]     oq_LeftFillBlanks true: fill left with " "
                                    false: fill left with "0"

   \return
   Formatted timestamp ("mmmmmmmmmm.uuu").
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocols::FormatTimeStamp(const uint64_t ou64_TimeStampUs, const bool oq_LeftFillBlanks)
{
   C_SclString c_Time;

   if (oq_LeftFillBlanks == false)
   {
      (void)c_Time.PrintFormatted("%013llu", ou64_TimeStampUs);
   }
   else
   {
      if (ou64_TimeStampUs >= 1000)
      {
         (void)c_Time.PrintFormatted("%13llu", ou64_TimeStampUs);
      }
      else
      {
         //we need at least 4 characters so we don't get strings list " . 12" but "0.012"
         (void)c_Time.PrintFormatted("         %04llu", ou64_TimeStampUs);
      }
   }
   (void)c_Time.Insert(".", 11);
   return c_Time;
}

//---------------------------------------------------------------------------

bool C_CanMonProtocols::GetProtocolHasParameters(const e_CanMonL7Protocols oe_L7Protocol) const
{
   bool q_HasParams;

   switch (oe_L7Protocol)
   {
   case CMONL7ProtocolNone:
      q_HasParams = false;
      break;
   case CMONL7ProtocolCANopen:
      q_HasParams = false;
      break;
   case CMONL7ProtocolKEFEX:
      q_HasParams = true;
      break;
   case CMONL7ProtocolXFL:
      q_HasParams = true;
      break;
   case CMONL7ProtocolSTWFF:
      q_HasParams = false;
      break;
   case CMONL7ProtocolGD:
      q_HasParams = false;
      break;
   case CMONL7Protocol_SHIP_IP_IVA:
      q_HasParams = false;
      break;
   case CMONL7ProtocolJ1939:
      q_HasParams = false;
      break;
   case CMONL7ProtocolOpenSYDE:
      q_HasParams = false;
      break;
   default:
      q_HasParams = false;
      break;
   }
   return q_HasParams;
}

//---------------------------------------------------------------------------

uint16_t C_CanMonProtocols::KFXGetBaseID(void) const
{
   return mc_ProtocolKEFEX.GetBaseID();
}

//---------------------------------------------------------------------------

void C_CanMonProtocols::KFXSetBaseID(const uint16_t ou16_BaseID)
{
   mc_ProtocolKEFEX.SetBaseID(ou16_BaseID);
}

//---------------------------------------------------------------------------

uint32_t C_CanMonProtocols::XFLGetSendID(void) const
{
   return mc_ProtocolXFL.GetSendID();
}

//---------------------------------------------------------------------------

void C_CanMonProtocols::XFLSetSendID(const uint32_t ou32_SendID)
{
   mc_ProtocolXFL.SetSendID(ou32_SendID);
}

//---------------------------------------------------------------------------
