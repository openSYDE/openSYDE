#include "precomp_headers.h"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstring>
#include "CCMONProtocolTarget.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "CCMONProtocol.h"
#include "CCMONProtocolCANopen.h"
#include "CCMONProtocolKEFEX.h"
#include "CCMONProtocolSHIPIPIVA.h"
#include "CCMONProtocolGD.h"
#include "CCMONProtocolSTWFF.h"
#include "CCMONProtocolXFL.h"
#include "CCMONProtocolL2.h"
#include "CCMONProtocolJ1939.h"
#include "CCMONProtocolOpenSYDE.h"

#include "CSCLString.h"
#include "CSCLIniFile.h"
#include "TGLFile.h"
#include "TGLUtils.h"

//---------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_cmon_protocol;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_can;

//---------------------------------------------------------------------------

sint32 C_CMONProtocols::GetProtocolName(const e_CMONL7Protocols oe_L7Protocol, C_SCLString & orc_Description) const
{
   sint32 s32_Return = C_NO_ERR;
   if (static_cast<sint32>(oe_L7Protocol) < gs32_KFX_CMON_NUM_PROTOCOLS)
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

C_SCLString C_CMONProtocols::MessageToString(const T_STWCAN_Msg_TX & orc_Msg) const
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

C_SCLString C_CMONProtocols::MessageToStringLog(const T_STWCAN_Msg_TX & orc_Msg) const
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
C_SCLString C_CMONProtocols::MessageToStringLog(const T_STWCAN_Msg_RX & orc_Msg) const
{
   const uint16 u16_MAX_CHARS_RAW_DATA = 73U;

   C_SCLString c_Text;
   C_SCLString c_Help;
   sint32 s32_Index;
   uint8 u8_Len;
   bool q_Decimal = GetDecimalMode();


   u8_Len = (orc_Msg.u8_DLC > 8) ? static_cast<uint8>(8U) : orc_Msg.u8_DLC;
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
         c_Text = c_Text + C_SCLString::StringOfChar(' ', u16_MAX_CHARS_RAW_DATA - c_Text.Length());
      }
      c_Text += (MessageToString(orc_Msg) + ";");
   }
   return c_Text;
}

//---------------------------------------------------------------------------

C_SCLString C_CMONProtocols::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SCLString c_Text;

   c_Text = "";
   if (static_cast<sint32>(me_ActiveProtocol) < gs32_KFX_CMON_NUM_PROTOCOLS)
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

C_SCLString C_CMONProtocols::MessageToString(const T_STWCAN_Msg_RX & orc_Message, const uint32 ou32_Count) const
{
   C_SCLString c_Text;
   C_SCLString c_Help;

   c_Text = this->MessageToString(orc_Message);
   //count
   (void)c_Help.PrintFormatted("%7d  ", ou32_Count);
   return (c_Help + c_Text);
}

//---------------------------------------------------------------------------

C_CMONProtocols::C_CMONProtocols() :
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

C_CMONProtocols::~C_CMONProtocols(void)
{
}

//---------------------------------------------------------------------------

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
sint32 C_CMONProtocols::KFXSetVariableInfo(const stw_diag_lib::C_KFXVariableLists * const opc_Lists,
                                           const uint16 ou16_ListOffset)
{
   mc_ProtocolKEFEX.SetVariableInfo(opc_Lists, ou16_ListOffset);
   return C_NO_ERR;
}
#endif

//---------------------------------------------------------------------------

e_CMONL7Protocols C_CMONProtocols::GetProtocolMode(void) const
{
   return me_ActiveProtocol;
}

//---------------------------------------------------------------------------

sint32 C_CMONProtocols::SetProtocolMode(const e_CMONL7Protocols oe_L7Protocol)
{
   me_ActiveProtocol = oe_L7Protocol;
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

bool C_CMONProtocols::GetDecimalMode(void) const
{
   return mq_Decimal;
}

//---------------------------------------------------------------------------

sint32 C_CMONProtocols::SetDecimalMode(const bool oq_Decimal)
{
   sint32 s32_Loop;
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
sint32 C_CMONProtocols::SaveProtocolParametersToINI(const C_SCLString & orc_FileName, const C_SCLString & orc_Section)
   const
{
   sint32 s32_Return;
   sint32 s32_Loop;

   try
   {
      C_SCLIniFile c_IniFile(orc_FileName);
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
sint32 C_CMONProtocols::LoadProtocolParametersFromINI(const C_SCLString & orc_FileName, const C_SCLString & orc_Section)
   const
{
   sint32 s32_Return;
   sint32 s32_Loop;

   if (TGL_FileExists(orc_FileName) == false)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      C_SCLIniFile c_IniFile(orc_FileName);

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
C_SCLString C_CMONProtocols::FormatTimeStamp(const uint64 ou64_TimeStampUs, const bool oq_LeftFillBlanks)
{
   C_SCLString c_Time;
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

bool C_CMONProtocols::GetProtocolHasParameters(const e_CMONL7Protocols oe_L7Protocol) const
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

uint16 C_CMONProtocols::KFXGetBaseID(void) const
{
   return mc_ProtocolKEFEX.GetBaseID();
}

//---------------------------------------------------------------------------

void C_CMONProtocols::KFXSetBaseID(const uint16 ou16_BaseID)
{
   mc_ProtocolKEFEX.SetBaseID(ou16_BaseID);
}

//---------------------------------------------------------------------------

uint32 C_CMONProtocols::XFLGetSendID(void) const
{
   return mc_ProtocolXFL.GetSendID();
}

//---------------------------------------------------------------------------

void C_CMONProtocols::XFLSetSendID(const uint32 ou32_SendID)
{
   mc_ProtocolXFL.SetSendID(ou32_SendID);
}

//---------------------------------------------------------------------------

