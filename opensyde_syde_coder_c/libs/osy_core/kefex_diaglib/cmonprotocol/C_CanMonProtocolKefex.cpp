//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion class for the STW KEFEX protocol

   Converts a CAN message structure to text. The interpretation is based on
   the STW KEFEX CAN protocol specification.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.hpp"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "C_CanMonProtocolTarget.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolKefex.hpp"

//---------------------------------------------------------------------------


using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;
using namespace stw::can;

//---------------------------------------------------------------------------
//have the protocol constants here as copies, so we can put this class into a library of its own
// without having to add all the protocol header files

//KEFEX protocol contants
static const uint8_t mu8_KFX_PROTOCOL_SRR                          = 0x01U;
static const uint8_t mu8_KFX_PROTOCOL_ECRR                         = 0x02U;
static const uint8_t mu8_KFX_PROTOCOL_TCRR                         = 0x03U;
static const uint8_t mu8_KFX_PROTOCOL_TCRR_TIMESTAMPED             = 0x04U;
static const uint8_t mu8_KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE    = 0x0AU;
static const uint8_t mu8_KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS = 0x0BU;
static const uint8_t mu8_KFX_PROTOCOL_ABORT_ALL_RESPONSES          = 0x0CU;
static const uint8_t mu8_KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS       = 0x0DU;
static const uint8_t mu8_KFX_PROTOCOL_IMMEDIATE_WRITE_HS           = 0x0FU;
static const uint8_t mu8_KFX_PROTOCOL_LOGON_HS                     = 0x11U;
static const uint8_t mu8_KFX_PROTOCOL_LOGOFF_HS                    = 0x15U;
static const uint8_t mu8_KFX_PROTOCOL_SERVICE_READ                 = 0x16U;
static const uint8_t mu8_KFX_PROTOCOL_SERVICE_WRITE                = 0x17U;
static const uint8_t mu8_KFX_PROTOCOL_STARTUP_INDICATION           = 0x1AU;
static const uint8_t mu8_KFX_PROTOCOL_READ_EEPROM                  = 0x20U;
static const uint8_t mu8_KFX_PROTOCOL_READ_FLASH_EEPROM            = 0x21U;
static const uint8_t mu8_KFX_PROTOCOL_READ_SRAM                    = 0x22U;
static const uint8_t mu8_KFX_PROTOCOL_READ_EPROM                   = 0x23U;
static const uint8_t mu8_KFX_PROTOCOL_WRITE_EEPROM_HS              = 0x27U;
static const uint8_t mu8_KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS        = 0x29U;
static const uint8_t mu8_KFX_PROTOCOL_WRITE_SRAM_HS                = 0x2BU;
static const uint8_t mu8_KFX_PROTOCOL_UPDATE_TASK                  = 0x2EU;
static const uint8_t mu8_KFX_PROTOCOL_UPDATE_TASK_HS               = 0x2FU;
static const uint8_t mu8_KFX_PROTOCOL_WRITE_EEPROM_SSL             = 0x30U;
static const uint8_t mu8_KFX_PROTOCOL_IWR_FF                       = 0x40U;
static const uint8_t mu8_KFX_PROTOCOL_IWR_FC                       = 0x40U;
static const uint8_t mu8_KFX_PROTOCOL_IWR_CF                       = 0x41U;
static const uint8_t mu8_KFX_PROTOCOL_SRR_FF                       = 0x42U;
static const uint8_t mu8_KFX_PROTOCOL_SRR_FC                       = 0x42U;
static const uint8_t mu8_KFX_PROTOCOL_SRR_CF                       = 0x43U;

//-----------------------------------------------------------------------------
/*!
   \brief    constructor

   Initialize class members
*/
//-----------------------------------------------------------------------------
C_CanMonProtocolKefex::C_CanMonProtocolKefex(void) :
   C_CMONProtocolKEFEX_IVA(),
   mu16_KFXBaseID(0x600U)
{
}

//---------------------------------------------------------------------------

uint16_t C_CanMonProtocolKefex::GetBaseID(void) const
{
   return mu16_KFXBaseID;
}

//---------------------------------------------------------------------------

void C_CanMonProtocolKefex::SetBaseID(const uint16_t ou16_BaseID)
{
   mu16_KFXBaseID = ou16_BaseID;
}

//-----------------------------------------------------------------------------
/*!
   \brief    save protocol specific parameters to INI file

   Write our parameters to the specified section.

   \param[in,out]   orc_IniFile  opened INI file to write to
   \param[in]       orc_Section  section in INI file to write to

   \return
   C_NO_ERR  -> data added to INI file
   C_RD_WR   -> error writing data
*/
//-----------------------------------------------------------------------------
int32_t C_CanMonProtocolKefex::SaveParamsToIni(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   int32_t s32_Error = C_NO_ERR;
   try
   {
      orc_IniFile.WriteInteger(orc_Section, "PP_KFX_BASE_ID",     this->mu16_KFXBaseID);
      orc_IniFile.WriteInteger(orc_Section, "PP_KFX_LIST_OFFSET", this->mu16_KFXListOffset);
      //const t_RAMLists *m_ptKFXLists; No easy way to save this.
   }
   catch (...)
   {
      s32_Error = C_RD_WR;
   }
   return s32_Error;
}

//-----------------------------------------------------------------------------
/*!
   \brief    load protocol specific parameters from INI file

   Load our parameters from the specified section.

   \param[in,out]   orc_IniFile  opened INI file to read from
   \param[in]       orc_Section  section in INI file to read from

   \return
   C_NO_ERR  -> data read from INI file
   C_RD_WR   -> error reading data
*/
//-----------------------------------------------------------------------------
int32_t C_CanMonProtocolKefex::LoadParamsFromIni(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   this->mu16_KFXBaseID     = static_cast<uint16_t>(orc_IniFile.ReadInteger(orc_Section, "PP_KFX_BASE_ID",
                                                  this->mu16_KFXBaseID));
   this->mu16_KFXListOffset = static_cast<uint16_t>(orc_IniFile.ReadInteger(orc_Section, "PP_KFX_LIST_OFFSET",
                                                  this->mu16_KFXListOffset));
   //const t_RAMLists *m_ptKFXLists; No easy way to load this.
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocolKefex::m_KFXIndexAndErrorToString(const char_t * const opcn_Text, const uint16_t ou16_Index,
                                                              const uint16_t ou16_Error,
                                                              const bool oq_IsKEFEXVarIndex) const
{
   C_SclString c_Help;
   C_SclString c_Help2;

   c_Help = static_cast<C_SclString>(" ") + opcn_Text + "  " + m_KFXIndexToString(ou16_Index, oq_IsKEFEXVarIndex) +
            "  ERROR ";

   if (mq_Decimal == true)
   {
      (void)c_Help2.PrintFormatted("%d", ou16_Error);
   }
   else
   {
      (void)c_Help2.PrintFormatted("%X", ou16_Error);
   }
   return c_Help + c_Help2;
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocolKefex::m_KFXTextAndValueToString(const char_t * const opcn_Text,
                                                           const uint32_t ou32_Value) const
{
   C_SclString c_Help;
   if (mq_Decimal == true)
   {
      (void)c_Help.PrintFormatted("%s %d", opcn_Text, ou32_Value);
   }
   else
   {
      (void)c_Help.PrintFormatted("%s %X", opcn_Text, ou32_Value);
   }
   return c_Help;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert CAN message to text representation.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolKefex::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   C_SclString c_Text;
   C_SclString c_Help;
   C_SclString c_Help2;
   C_SclString c_Help3;
   c_Text = "";

   if ((orc_Msg.u32_ID >= static_cast<uint32_t>(mu16_KFXBaseID)) &&
       (orc_Msg.u32_ID <= ((static_cast<uint32_t>(mu16_KFXBaseID)) + 0x1FFU)))
   {
      //it is a KFX protocol message !
      uint8_t u8_Sender;
      uint8_t u8_Receiver;
      uint16_t u16_Time;
      uint16_t u16_Hysteresis;
      uint32_t u32_Address;
      uint32_t u32_Size;
      uint16_t u16_Value;
      bool q_Request;

      //ID:
      //10,9 = base ID
      //8..1 = sender ID
      //0    = Request
      q_Request = static_cast<bool>((orc_Msg.u32_ID & 0x0001U) == 0x0000U);
      u8_Sender = static_cast<uint8_t>((orc_Msg.u32_ID & 0x1FEU) >> 1U);

      //request or response ?
      if (q_Request == true)
      {
         if (orc_Msg.u8_DLC < 2U)
         {
            c_Text = m_GetByteAsStringFormat(u8_Sender) + " -> XXX (*DLC Error*)";
         }
         else
         {
            u8_Receiver = orc_Msg.au8_Data[0];
            c_Text = m_GetByteAsStringFormat(u8_Sender)+" -> "+m_GetByteAsStringFormat(u8_Receiver);
            //which service ?
            switch (orc_Msg.au8_Data[1] & 0x7FU)
            {
            case mu8_KFX_PROTOCOL_SERVICE_READ:
               if (orc_Msg.u8_DLC < 4U)
               {
                  c_Help = " ServiceRead REQ  (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndValueToString(" ServiceRead REQ  INDEX ",
                                                     mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_SERVICE_WRITE:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " ServiceWrite REQ  (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndValueToString(" ServiceWrite REQ  INDEX ",
                          static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]))) +
                          "  VALUE "+ m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_SRR:
               if (orc_Msg.u8_DLC < 4U)
               {
                  c_Help = " SRR REQ  (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndIndexToString(" SRR REQ ", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_SRR_FC:
               if (orc_Msg.u8_DLC < 7U)
               {
                  c_Help = " SRR SEG FC REQ  (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[1] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = m_KFXIndexAndErrorToString("SRR SEG FC REQ", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]),
                                                         mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
                  }
                  else
                  {
                     c_Help = " SRR SEG FC REQ  " +
                             m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                             "  BS "  + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                             "  MI "  + m_GetValueDecHex(orc_Msg.au8_Data[5]) + "ms" +
                             "  PAR " + m_GetValueDecHex(orc_Msg.au8_Data[6]);
                  }
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_ECRR:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " ECRR SEG   (*DLC Error*)";
               }
               else
               {
                  u16_Time       = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]);
                  u16_Hysteresis = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]);
                  const uint16_t u16_HystBase  = static_cast<uint16_t>((u16_Hysteresis >> 14U) & 0x03U);
                  const uint16_t u16_UpperHyst = static_cast<uint16_t>((u16_Hysteresis >> 7U)  & 0x7FU);
                  const uint16_t u16_LowerHyst = static_cast<uint16_t>( u16_Hysteresis         & 0x7FU);
                  float64_t f64_Help;

                  c_Help2 += " UHYST ";
                  if (u16_UpperHyst < 30U)
                  {
                     f64_Help = (static_cast<float64_t>(u16_UpperHyst) / 10.0);
                  }
                  else //30..127 -> 3% .. 100%
                  {
                     f64_Help = (static_cast<float64_t>(u16_UpperHyst) - 27.0);
                  }
                  c_Help2 += C_SclString::FloatToStr(f64_Help) + "%";

                  c_Help2 = " LHYST ";
                  if (u16_LowerHyst < 30U)
                  {
                     f64_Help = (static_cast<float64_t>(u16_LowerHyst) / 10.0);
                  }
                  else //30..127 -> 3% .. 100%
                  {
                     f64_Help = (static_cast<float64_t>(u16_LowerHyst) - 27.0);
                  }
                  c_Help2 += C_SclString::FloatToStr(f64_Help) + "%";

                  c_Help2 += " HBASE ";
                  switch (u16_HystBase)
                  {
                  case 0:
                     c_Help2 += m_GetValueDecHex(100);
                     break;
                  case 1:
                     c_Help2 += m_GetValueDecHex(10000);
                     break;
                  case 2:
                     c_Help2 += m_GetValueDecHex(1000000);
                     break;
                  case 3:
                     c_Help2 += m_GetValueDecHex(100000000);
                     break;
                  default:
                     break;
                  }
                  c_Help = " ECRR REQ  " + m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]))+
                          "  MAXTIME "  + m_GetValueDecHex(u16_Time) +
                          "ms " + c_Help2;
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_TCRR:
               if (orc_Msg.u8_DLC < 6)
               {
                  c_Help = " TCRR SEG   (*DLC Error*)";
               }
               else
               {
                  u16_Time = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]);
                  c_Help = " TCRR REQ  " + m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]))+
                          "  TIME "+ m_GetValueDecHex(u16_Time) + "ms";
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_TCRR_TIMESTAMPED:
               if (orc_Msg.u8_DLC < 6)
               {
                  c_Help = " TCRR TS SEG   (*DLC Error*)";
               }
               else
               {
                  u16_Time = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]);
                  c_Help = " TCRR TS REQ  " +
                          m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) +
                          "  TIME " + m_GetValueDecHex(u16_Time) + "ms";
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE:
               if (orc_Msg.u8_DLC < 4U)
               {
                  c_Help = " AIR REQ   (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndIndexToString(" AIR REQ  ", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS:
               if (orc_Msg.u8_DLC < 4U)
               {
                  c_Help = " AIR HS REQ   (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndIndexToString(" AIR HS REQ  ", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_ABORT_ALL_RESPONSES:
               c_Text += " AAR REQ";
               break;
            case mu8_KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS:
               c_Text += " AAR HS REQ";
               break;
            case mu8_KFX_PROTOCOL_IWR_FF:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " IMM SEG FF REQ   (*DLC Error*)";
               }
               else
               {
                  u32_Size = orc_Msg.au8_Data[4] +
                      ((static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 8U)) +
                      ((static_cast<uint32_t>(orc_Msg.au8_Data[6]) << 16U));
                  if (mq_Decimal == true)
                  {
                     (void)c_Help.PrintFormatted(" IMM SEG FF REQ  %s NUM %d PAR %d",
                         m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])).c_str(), u32_Size,
                         orc_Msg.au8_Data[7]);
                  }
                  else
                  {
                     (void)c_Help.PrintFormatted(" IMM SEG FF REQ  %s  NUM %X  PAR %X",
                         m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])).c_str(), u32_Size,
                         orc_Msg.au8_Data[7]);
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_IWR_CF:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " IMM SEG CF REQ   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = " IMM SEG CF REQ  BLK " + m_GetValueDecHex(orc_Msg.au8_Data[2])+
                             "  ERROR "+
                             m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4])));
                  }
                  else
                  {
                     if (mq_Decimal == true)
                     {
                        (void)c_Help.PrintFormatted(" IMM SEG CF REQ  BLK %d  DATA [%d,%d,%d,%d,%d]",
                            orc_Msg.au8_Data[2], orc_Msg.au8_Data[3], orc_Msg.au8_Data[4],
                            orc_Msg.au8_Data[5], orc_Msg.au8_Data[6], orc_Msg.au8_Data[7]);
                     }
                     else
                     {
                        (void)c_Help.PrintFormatted(" IMM SEG CF REQ  BLK %02X  DATA [%02X,%02X,%02X,%02X,%02X]",
                            orc_Msg.au8_Data[2], orc_Msg.au8_Data[3], orc_Msg.au8_Data[4],
                            orc_Msg.au8_Data[5], orc_Msg.au8_Data[6], orc_Msg.au8_Data[7]);
                     }
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_IMMEDIATE_WRITE_HS:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " IMM HS REQ   (*DLC Error*)";
               }
               else
               {
                  c_Help = " IMM HS REQ  " + m_KFXIndexToString((mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])))+
                          "  VALUE "+ m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_LOGON_HS:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " LOGON HS REQ   (*DLC Error*)";
               }
               else
               {
                  c_Help = " LOGON HS REQ  CRC " +
                          m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]))) +
                          "  VER "+ m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]))) +
                          "  NUM "+ m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6])));
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_LOGOFF_HS:
               c_Text += " LOGOFF HS REQ";
               break;
            case mu8_KFX_PROTOCOL_READ_EEPROM: //no break
            case mu8_KFX_PROTOCOL_READ_FLASH_EEPROM: //no break
            case mu8_KFX_PROTOCOL_READ_SRAM: //no break
            case mu8_KFX_PROTOCOL_READ_EPROM: //no break
               switch (orc_Msg.au8_Data[1] & 0x7FU)
               {
               case mu8_KFX_PROTOCOL_READ_EEPROM:
                  c_Text += " READ EEPROM REQ";
                  break;
               case mu8_KFX_PROTOCOL_READ_FLASH_EEPROM:
                  c_Text += " READ FLASH EEPROM REQ";
                  break;
               case mu8_KFX_PROTOCOL_READ_SRAM:
                  c_Text += " READ SRAM REQ";
                  break;
               case mu8_KFX_PROTOCOL_READ_EPROM:
                  c_Text += " READ EPROM REQ";
                  break;
               default:
                  c_Text +=" READ ?\?\? REQ";
                  break;
               }
               if (orc_Msg.u8_DLC < 6U)
               {
                  c_Help = c_Text + "   (*DLC Error*)";
               }
               else
               {
                  if (orc_Msg.au8_Data[2] == 0U)
                  {
                     c_Text += " WORD ";
                  }
                  else
                  {
                     c_Text += " BYTE ";
                  }
                  u32_Address = orc_Msg.au8_Data[3] + ((static_cast<uint32_t>(orc_Msg.au8_Data[4])) << 8U) +
                                                      ((static_cast<uint32_t>(orc_Msg.au8_Data[5])) << 16U);
                  c_Help = " ADD: " + m_GetValueDecHex(u32_Address);
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_WRITE_EEPROM_HS: //no break
            case mu8_KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS: //no break
            case mu8_KFX_PROTOCOL_WRITE_SRAM_HS:
               switch (orc_Msg.au8_Data[1] & 0x7FU)
               {
               case mu8_KFX_PROTOCOL_WRITE_EEPROM_HS:
                  c_Text += " WRITE EEPROM HS REQ";
                  break;
               case mu8_KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS:
                  c_Text += " WRITE FLASH EEPROM HS REQ";
                  break;
               case mu8_KFX_PROTOCOL_WRITE_SRAM_HS:
                  c_Text += " WRITE SRAM HS REQ";
                  break;
               default:
                  c_Text += " WRITE ?\?\? HS REQ";
                  break;
               }
               if (orc_Msg.u8_DLC < 6U)
               {
                  c_Help = c_Text + "   (*DLC Error*)";
               }
               else
               {
                  if (orc_Msg.au8_Data[2] == 0)
                  {
                     c_Text += " WORD ";
                  }
                  else
                  {
                     c_Text += " BYTE ";
                  }
                  u32_Address = orc_Msg.au8_Data[3] + ((static_cast<uint32_t>(orc_Msg.au8_Data[4]) << 8U)) +
                                                      ((static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 16U));
                  u16_Value = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]);
                  c_Help = " ADD: " + m_GetValueDecHex(u32_Address) + "  VAL:";
                  if (orc_Msg.au8_Data[2] == 0U)
                  {
                     c_Help += m_GetWordAsStringFormat(u16_Value);
                  }
                  else
                  {
                     c_Help += m_GetByteAsStringFormat(static_cast<uint8_t>(u16_Value));
                  }
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_UPDATE_TASK:
               if (orc_Msg.u8_DLC < 4)
               {
                  c_Help = " UPDATE TASK REQ   (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndValueToString(" UPDATE TASK REQ  INDEX",
                                                     mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_UPDATE_TASK_HS:
               if (orc_Msg.u8_DLC < 4)
               {
                  c_Help = " UPDATE TASK HS REQ   (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndValueToString(" UPDATE TASK HS REQ  INDEX",
                                                     mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]));
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_WRITE_EEPROM_SSL:
               if (orc_Msg.u8_DLC != 8)
               {
                  c_Help = " WRITE EEPROM SSL REQ   (*DLC Error*)";
               }
               else
               {
                  if (orc_Msg.au8_Data[2] == 0x1U) //START
                  {
                     c_Help = m_KFXTextAndValueToString(" WRITE EEPROM SSL REQ  START  NUM_SERVICES ",
                                                         mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                                                         "  CRC " +
                                                         m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
                  }
                  else if (orc_Msg.au8_Data[2] == 0x2U) //END
                  {
                     c_Help = m_KFXTextAndValueToString(" WRITE EEPROM SSL REQ  END  NUM_SERVICES ",
                                                         mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                                                         "  CRC " +
                                                         m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
                  }
                  else
                  {
                     c_Help = " WRITE EEPROM SSL REQ   (*undefined subservice " +
                              m_GetValueDecHex(orc_Msg.au8_Data[2]) + "*)";
                  }
               }
               c_Text += c_Help;
               break;
            default:
               c_Text+= "?\?\?";
               break;
            }
            if (c_Text != "")
            {
               return c_Text;
            }
         }
      }
      else
      {
         //response
         c_Text = m_GetByteAsStringFormat(u8_Sender)+" -> XXX";
         if (orc_Msg.u8_DLC < 2U)
         {
            c_Text += " (*DLC Error*)";
         }
         else
         {
            c_Text = m_GetByteAsStringFormat(u8_Sender)+" -> XXX";
            //which service ?
            switch (orc_Msg.au8_Data[0] & 0x7FU)
            {
            case mu8_KFX_PROTOCOL_SRR:  //no break
            case mu8_KFX_PROTOCOL_ECRR: //no break
            case mu8_KFX_PROTOCOL_SERVICE_READ: //no break
            case mu8_KFX_PROTOCOL_TCRR:
               switch (orc_Msg.au8_Data[0] & 0x7FU)
               {
                  case mu8_KFX_PROTOCOL_SRR:
                     c_Help3 = "SRR";
                     break;
                  case mu8_KFX_PROTOCOL_ECRR:
                     c_Help3 = "ECRR";
                     break;
                  case mu8_KFX_PROTOCOL_TCRR:
                     c_Help3 = "TCRR";
                     break;
                  case mu8_KFX_PROTOCOL_SERVICE_READ:
                     c_Help3 = "ServiceRead";
                     break;
                  default:
                     c_Help3 = "?\?\?";
                     break;
               }
               c_Help3 += " RES";

               if (orc_Msg.u8_DLC < 7U)
               {
                  c_Help += " " + c_Help3 + "   (*DLC Error*)";
                  c_Help2 = "";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {  //error
                     c_Help = m_KFXIndexAndErrorToString(c_Help3.c_str(), mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]),
                                                                          mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                     c_Help2 = "";
                  }
                  else
                  {
                     c_Help = " " + c_Help3 + "  " + m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]));
                     if (mq_Decimal == true)
                     {
                        c_Help2 = "  VALUE: " + C_SclString::IntToStr(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
                     }
                     else
                     {
                        (void)c_Help2.PrintFormatted("  VALUE: %X", mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
                     }
                  }
               }
               c_Text = c_Text + c_Help + c_Help2;
               break;
            case mu8_KFX_PROTOCOL_SRR_FF:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " SRR SEG FF RES   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = m_KFXIndexAndErrorToString("SRR SEG FF RES", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]),
                                                                           mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
                  else
                  {
                     c_Help = " SRR SEG FF RES  " + m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]))+
                             "  NUM " + m_GetValueDecHex(orc_Msg.au8_Data[3] +
                                        (static_cast<uint32_t>(orc_Msg.au8_Data[4]) << 8U) +
                                        (static_cast<uint32_t>(orc_Msg.au8_Data[5]) << 16U))+
                             "  PAR " + m_GetValueDecHex(orc_Msg.au8_Data[6])+
                             "  DATA [" + m_GetValueDecHex(orc_Msg.au8_Data[7]) + "]";
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_SRR_CF:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " SRR SEG CF RES   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = " SRR SEG CF RES  BLK " + m_GetValueDecHex(orc_Msg.au8_Data[1]) +
                             "  ERROR " + m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3])));
                  }
                  else
                  {
                     if (mq_Decimal == true)
                     {
                        (void)c_Help.PrintFormatted(" SRR SEG CF RES  BLK %d  DATA [%d,%d,%d,%d,%d,%d]",
                           orc_Msg.au8_Data[1], orc_Msg.au8_Data[2],  orc_Msg.au8_Data[3],
                           orc_Msg.au8_Data[4], orc_Msg.au8_Data[5],  orc_Msg.au8_Data[6], orc_Msg.au8_Data[7]);
                     }
                     else
                     {
                        (void)c_Help.PrintFormatted(" SRR SEG CF RES  BLK %02X  DATA [%02X,%02X,%02X,%02X,%02X,%02X]",
                           orc_Msg.au8_Data[1], orc_Msg.au8_Data[2],  orc_Msg.au8_Data[3],
                           orc_Msg.au8_Data[4], orc_Msg.au8_Data[5],  orc_Msg.au8_Data[6], orc_Msg.au8_Data[7]);
                     }
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_TCRR_TIMESTAMPED:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " TCRR TS RES   (*DLC Error*)";
                  c_Help2 = "";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = m_KFXIndexAndErrorToString("TCRR TS RES", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]),
                                                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                     c_Help2 = "";
                  }
                  else
                  {
                     c_Help = m_KFXTextAndIndexToString(" TCRR TS RES  ", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]));
                     c_Help2 = "  VALUE: " + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3])) +
                              "  TS: " + m_GetValueDecHex(orc_Msg.au8_Data[5] +
                                 (static_cast<uint32_t>(orc_Msg.au8_Data[6]) << 8U) +
                                 (static_cast<uint32_t>(orc_Msg.au8_Data[7]) << 16U)) + "ms";
                  }
               }
               c_Text = c_Text + c_Help + c_Help2;
               break;
            case mu8_KFX_PROTOCOL_ABORT_INDIVIDUAL_RESPONSE_HS:
               if (orc_Msg.u8_DLC < 3U)
               {
                  c_Help = " AIR HS   (*DLC Error*)";
               }
               else
               {
                  c_Help = m_KFXTextAndValueToString(" AIR HS  ", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]));
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_ABORT_ALL_RESPONSES_HS:
               c_Text += " AAR HS RES";
               break;
            case mu8_KFX_PROTOCOL_SERVICE_WRITE:
               c_Help3 = " ServiceWrite RES";
               if (orc_Msg.u8_DLC < 5)
               {
                  c_Help = c_Help3 + "   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = m_KFXIndexAndErrorToString(c_Help3.c_str(),
                                 mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]), mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]),
                                 false);
                  }
                  else
                  {
                     c_Help = m_KFXTextAndValueToString((c_Help3 + "  INDEX").c_str(),
                                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]));
                     c_Help += " OK";
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_IMMEDIATE_WRITE_HS:
               c_Help3 = " IMM RES";
               if (orc_Msg.u8_DLC < 3U)
               {
                  c_Help = c_Help3 + "   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = m_KFXIndexAndErrorToString(c_Help3.c_str(),
                         mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]), mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
                  else
                  {
                     c_Help = m_KFXTextAndIndexToString(c_Help3.c_str(), mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]));
                     c_Help += " OK";
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_IWR_FC:
               if (orc_Msg.u8_DLC < 5U)
               {
                  c_Help = " IMM SEG FC RES   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  { //error
                     c_Help = m_KFXIndexAndErrorToString("IMM SEG FC RES",
                             mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]), mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
                  else
                  {
                     c_Help = " IMM SEG FC RES  " +
                             m_KFXIndexToString(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1])) +
                             "  BS " + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                             "  MI " + m_GetValueDecHex(orc_Msg.au8_Data[4]) + "ms" +
                             "  PAR " + m_GetValueDecHex(orc_Msg.au8_Data[5]);
                  }
               }
               c_Text+=c_Help;
               break;
            case mu8_KFX_PROTOCOL_LOGON_HS:
               if (orc_Msg.u8_DLC < 5U)
               {
                  c_Text = " LOGON HS RES   (*DLC Error*)";
               }
               else
               {
                  c_Text += " LOGON HS RES";
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     c_Text += m_KFXTextAndValueToString("  ERROR", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
               }
               break;
            case mu8_KFX_PROTOCOL_LOGOFF_HS:
               if (orc_Msg.u8_DLC < 5U)
               {
                  c_Text = " LOGOFF HS RES   (*DLC Error*)";
               }
               else
               {
                  c_Text += " LOGOFF HS RES";
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {  //error
                     c_Text = m_KFXTextAndValueToString("  ERROR", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
               }
               break;
            case mu8_KFX_PROTOCOL_STARTUP_INDICATION:
               c_Text+= " STARTUP";
               break;
            case mu8_KFX_PROTOCOL_READ_EEPROM: //no break
            case mu8_KFX_PROTOCOL_READ_FLASH_EEPROM: //no break
            case mu8_KFX_PROTOCOL_READ_SRAM: //no break
            case mu8_KFX_PROTOCOL_READ_EPROM: //no break
               switch (orc_Msg.au8_Data[0] & 0x7FU)
               {
               case mu8_KFX_PROTOCOL_READ_EEPROM:
                  c_Help = " READ EEPROM RES";
                  break;
               case mu8_KFX_PROTOCOL_READ_FLASH_EEPROM:
                  c_Help = " READ FLASH EEPROM RES";
                  break;
               case mu8_KFX_PROTOCOL_READ_SRAM:
                  c_Help = " READ SRAM RES";
                  break;
               case mu8_KFX_PROTOCOL_READ_EPROM:
                  c_Help = " READ EPROM RES";
                  break;
               default:
                  c_Help = " READ ?\?\? RES";
                  break;
               }

               if (orc_Msg.u8_DLC < 7)
               {
                  c_Help += "   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help += m_KFXTextAndValueToString("  ERROR", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
                  else
                  {
                     if (orc_Msg.au8_Data[1] == 0U)
                     {
                        c_Help += " WORD ";
                     }
                     else
                     {
                        c_Help += " BYTE ";
                     }
                     u32_Address = orc_Msg.au8_Data[2] +
                           ((static_cast<uint32_t>(orc_Msg.au8_Data[3]) << 8U)) +
                           ((static_cast<uint32_t>(orc_Msg.au8_Data[4]) << 16U));
                     u16_Value = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[5]);
                     c_Help += " ADD: " + m_GetValueDecHex(u32_Address) + "  VAL:";
                     if (orc_Msg.au8_Data[1] == 0U)
                     {
                        c_Help += m_GetWordAsStringFormat(u16_Value);
                     }
                     else
                     {
                        c_Help += m_GetByteAsStringFormat(static_cast<uint8_t>(u16_Value));
                     }
                  }
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS: //no break
            case mu8_KFX_PROTOCOL_WRITE_EEPROM_HS: //no break
            case mu8_KFX_PROTOCOL_WRITE_SRAM_HS:
               switch (orc_Msg.au8_Data[0] & 0x7FU)
               {
               case mu8_KFX_PROTOCOL_WRITE_EEPROM_HS:
                  c_Help += " WRITE EEPROM HS RES";
                  break;
               case mu8_KFX_PROTOCOL_WRITE_FLASH_EEPROM_HS:
                  c_Help += " WRITE FLASH EEPROM HS RES";
                  break;
               case mu8_KFX_PROTOCOL_WRITE_SRAM_HS:
                  c_Help += " WRITE SRAM HS RES";
                  break;
               default:
                  c_Help += " WRITE ?\?\? HS RES";
                  break;
               }
               if (orc_Msg.u8_DLC < 7U)
               {
                  c_Help += "   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  { //error
                     c_Help += m_KFXTextAndValueToString("  ERROR", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
                  }
                  else
                  {
                     if (orc_Msg.au8_Data[1] == 0U)
                     {
                        c_Help += " WORD ";
                     }
                     else
                     {
                        c_Help += " BYTE ";
                     }
                     u32_Address = orc_Msg.au8_Data[2] +
                                ((static_cast<uint32_t>(orc_Msg.au8_Data[3]) << 8U)) +
                                ((static_cast<uint32_t>(orc_Msg.au8_Data[4]) << 16U));
                     u16_Value = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[5]);
                     c_Help += " ADD: " + m_GetValueDecHex(u32_Address) + "  VAL:";
                     if (orc_Msg.au8_Data[1] == 0U)
                     {
                        c_Help += m_GetWordAsStringFormat(u16_Value);
                     }
                     else
                     {
                        c_Help += m_GetByteAsStringFormat(static_cast<uint8_t>(u16_Value));
                     }
                  }
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_UPDATE_TASK_HS:
               if (orc_Msg.u8_DLC < 5U)
               {
                  c_Text = " UPDATE TASK RES   (*DLC Error*)";
               }
               else
               {
                  if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                  {
                     //error
                     c_Help = m_KFXIndexAndErrorToString("UPDATE TASK RES", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]),
                                                         mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]), false);
                  }
                  else
                  {
                     c_Help = m_KFXTextAndValueToString(" UPDATE TASK RES  INDEX",
                                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[1]));
                     c_Help += " OK";
                  }
               }
               c_Text += c_Help;
               break;
            case mu8_KFX_PROTOCOL_WRITE_EEPROM_SSL:
               if (orc_Msg.u8_DLC < 8U)
               {
                  c_Help = " WRITE EEPROM SSL RES   (*DLC Error*)";
               }
               else
               {
                  if (orc_Msg.au8_Data[1] == 0x1U) //START
                  {
                     if (orc_Msg.u8_DLC < 6U)
                     {
                        c_Help = " WRITE EEPROM SSL RES   (*DLC Error*)";
                     }

                     if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                     {
                        //error
                        c_Help = " WRITE EEPROM SSL RES  START  ERROR " +
                                 m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])));
                     }
                     else
                     {
                        c_Help = m_KFXTextAndValueToString(" WRITE EEPROM SSL RES  START  NUM_SERVICES ",
                                                           mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
                     }
                  }
                  else if (orc_Msg.au8_Data[1] == 0x2U) //END
                  {
                     if ((orc_Msg.au8_Data[0] & 0x80U) == 0x80U)
                     {
                        c_Help = " WRITE EEPROM SSL RES  END   ERROR " +
                                 m_GetValueDecHex(static_cast<uint32_t>(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])));

                     }
                     else
                     {
                        c_Help = m_KFXTextAndValueToString(" WRITE EEPROM SSL RES  END  NUM_SERVICES ",
                                                            mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                                                            "  CRC " +
                                                            m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
                     }
                  }
                  else
                  {
                     c_Help = " WRITE EEPROM SSL RES   (*undefined subservice " +
                              m_GetValueDecHex(orc_Msg.au8_Data[1]) + "*)";
                  }
               }
               c_Text += c_Help;
               break;
            default:
               c_Text = "";
               break;
            }
         }
      }
   }
   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get name of protocol as string

   \return
   Text representation of protocol name
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolKefex::GetProtocolName(void) const
{
   return "STW KEFEX";
}

//---------------------------------------------------------------------------

