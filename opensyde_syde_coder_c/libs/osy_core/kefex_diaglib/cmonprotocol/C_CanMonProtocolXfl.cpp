//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion class for the STW Flashloader protocol

   Converts a CAN message structure to text. The interpretation is based on
   the STW Flashloader CAN protocol specification.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "C_CanMonProtocolTarget.hpp"

#include <cctype>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolXfl.hpp"
#include "C_SclString.hpp"
#include "stwcompid.h"

//---------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::cmon_protocol;
using namespace stw::can;

//---------------------------------------------------------------------------
//have the protocol constants here as copies, so we can put this class into a library of its own
// without having to add all the protocol header files

//STW Flashloader protocol contants
//command groups
static const uint8_t mu8_XFL_CMD_GRP_EE_COMMAND          = 0x10U;
static const uint8_t mu8_XFL_CMD_GRP_EE_WRITE_CMD        = 0x11U;
static const uint8_t mu8_XFL_CMD_GRP_GET_COMMAND         = 0x20U;
static const uint8_t mu8_XFL_CMD_GRP_GET_SNR             = 0x21U;
static const uint8_t mu8_XFL_CMD_GRP_SET_COMMAND         = 0x30U;
static const uint8_t mu8_XFL_CMD_GRP_FLASH_COMMAND       = 0x40U;
static const uint8_t mu8_XFL_CMD_GRP_NODE_WAKEUP         = 0x41U;
static const uint8_t mu8_XFL_CMD_GRP_NET_COMMAND         = 0x50U;

//ee commands
static const uint8_t mu8_XFL_CMD_EE_READ_CMD             = 0x00U;

//get commands
static const uint8_t mu8_XFL_CMD_GET_LOKAL_ID            = 0x00U;
static const uint8_t mu8_XFL_CMD_GET_SECTOR_COUNT        = 0x01U;
static const uint8_t mu8_XFL_CMD_GET_VERSION_NUMBER      = 0x02U;
static const uint8_t mu8_XFL_CMD_GET_ERASE_COUNT         = 0x03U;
static const uint8_t mu8_XFL_CMD_GET_DEVICE_ID           = 0x04U;
static const uint8_t mu8_XFL_CMD_GET_CONTROL_ID          = 0x05U;
static const uint8_t mu8_XFL_CMD_GET_SEC_CRC             = 0x06U;
static const uint8_t mu8_XFL_CMD_GET_SEC_MODE_COMPARE    = 0x07U;
static const uint8_t mu8_XFL_CMD_GET_TIMEOUT_FACTOR      = 0x08U;
static const uint8_t mu8_XFL_CMD_GET_LAST_USER           = 0x09U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION   = 0x0AU;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION = 0x0BU;
static const uint8_t mu8_XFL_CMD_GET_FINGER_PRINT        = 0x0CU;
static const uint8_t mu8_XFL_CMD_GET_DEVICE_INFO_ADDRESS = 0x0DU;
static const uint8_t mu8_XFL_CMD_GET_BLOCK_START_ADDRESS = 0x0EU;
static const uint8_t mu8_XFL_CMD_GET_BLOCK_END_ADDRESS   = 0x0FU;
static const uint8_t mu8_XFL_CMD_GET_BLOCK_CHECKSUM      = 0x10U;
static const uint8_t mu8_XFL_CMD_GET_BLOCK_COMPARE_MODE  = 0x11U;

//sub-commands:
//get_flash_information
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_IC_COUNT          = 0x01;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_TOTAL_SIZE        = 0x02;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROTECTED_SECTORS = 0x03;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_OFFSET_ADDRESS    = 0x04;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_REGIONS       = 0x05;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_REGION_INFO       = 0x06;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ERASE_TIME        = 0x07;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROGRAM_TIME      = 0x08;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_ALIASES       = 0x09U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_PHYSICAL    = 0x0AU;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_SIZE        = 0x0BU;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_ADDRESS     = 0x0CU;

//get_implementation_information
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_PROTOCOL_VERSION = 0x01;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SPECIAL_SERVICES = 0x02;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_GET_SERVICES     = 0x03;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SET_SERVICES     = 0x04;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_FLASH_SERVICES   = 0x05;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_TARGET_SPECIFIC_SERVICES = 0x06;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_HEX_RECORDS      = 0x07;
//get_set_finger_print
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_SUPPORTED_INDEXES = 0x00;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_TIME  = 0x01;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_DATE  = 0x02;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_CHECKSUM          = 0x03;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_1        = 0x04;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_2        = 0x05;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_3        = 0x06;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_4        = 0x07;
//get_device_info_address
//const uint8 XFL_CMD_GET_DEVICE_INFO_ADDRESS_STRUCT_ADDRESS = 0x00;

//set commands
static const uint8_t mu8_XFL_CMD_SET_LOKAL_ID            = 0x00U;
static const uint8_t mu8_XFL_CMD_SET_BITRATE_CAN         = 0x01U;
static const uint8_t mu8_XFL_CMD_SET_CAN_ID              = 0x02U;
static const uint8_t mu8_XFL_CMD_SET_CAN_TYPE            = 0x03U;
static const uint8_t mu8_XFL_CMD_SET_CONTROL_ID          = 0x04U;
static const uint8_t mu8_XFL_CMD_SET_SEC_CRC             = 0x05U;
static const uint8_t mu8_XFL_CMD_SET_SEC_MODE_COMPARE    = 0x06U;
static const uint8_t mu8_XFL_CMD_SET_TIMEOUT_FACTOR      = 0x07U;
static const uint8_t mu8_XFL_CMD_SET_DIVERT_PARAM        = 0x08U;
static const uint8_t mu8_XFL_CMD_SET_XFL_EXCHANGE        = 0x09U;
static const uint8_t mu8_XFL_CMD_SET_FINGER_PRINT        = 0x0AU;
static const uint8_t mu8_XFL_CMD_SET_TEMP_BITRATE        = 0x0BU;
static const uint8_t mu8_XFL_CMD_SET_BLOCK_START_ADDRESS = 0x0CU;
static const uint8_t mu8_XFL_CMD_SET_BLOCK_END_ADDRESS   = 0x0DU;
static const uint8_t mu8_XFL_CMD_SET_BLOCK_CHECKSUM      = 0x0EU;
static const uint8_t mu8_XFL_CMD_SET_BLOCK_COMPARE_MODE  = 0x0FU;

//flash commands
static const uint8_t mu8_XFL_CMD_ERASE_SECTOR       = 0x00U;
static const uint8_t mu8_XFL_CMD_PROG_FLASH         = 0x01U;
static const uint8_t mu8_XFL_CMD_NODE_SLEEP         = 0x02U;
static const uint8_t mu8_XFL_CMD_NODE_RESET         = 0x03U;
static const uint8_t mu8_XFL_CMD_NODE_RETURN        = 0x04U;
static const uint8_t mu8_XFL_CMD_READ_FLASH         = 0x05U;
static const uint8_t mu8_XFL_CMD_NODE_COMPID        = 0x06U;
static const uint8_t mu8_XFL_CMD_DIVERT_STREAM      = 0x07U;
static const uint8_t mu8_XFL_CMD_BB_WAKEUP          = 0x08U;

//network commands
static const uint8_t mu8_XFL_CMD_NET_START         = 0x00U;
static const uint8_t mu8_XFL_CMD_NET_RESET         = 0x01U;

//flashloader error codes
static const uint8_t mu8_COMMAND_ERR      = 0xFFU;
static const uint8_t mu8_XFL_ERR_UNDEFINED_ERROR  = 0x00U;
//const uint8 u8_XFL_ERR_APP_DEF_ERR_MIN  = 0x10;
//const uint8 u8_XFL_ERR_APP_DEF_ERR_MAX  = 0x3F;
static const uint8_t mu8_XFL_ERR_CHECKSUM_ERR     = 0xD6U;
static const uint8_t mu8_XFL_ERR_FLASH_PROG_ERR   = 0xE1U;
static const uint8_t mu8_XFL_ERR_FLASH_ERASE_ERR  = 0xE2U;
static const uint8_t mu8_XFL_ERR_CAN_BITTIME_ERR  = 0xE3U;
static const uint8_t mu8_XFL_ERR_OUT_OF_RANGE_ERR = 0xF7U;
static const uint8_t mu8_XFL_ERR_EE_VERIFY_ERR    = 0xFBU;
static const uint8_t mu8_XFL_ERR_EE_READ_ERR      = 0xFCU;
static const uint8_t mu8_XFL_ERR_EE_WRITE_ERR     = 0xFDU;
static const uint8_t mu8_XFL_ERR_DIVERT_STREAM_COM_ERR = 0xFEU;
static const uint8_t mu8_XFL_ERR_WRONG_REC_TYPE   = 0xFFU;

//---------------------------------------------------------------------------

C_CanMonProtocolXfl::C_CanMonProtocolXfl(void) :
   C_CanMonProtocolBase(),
   mu32_XFLSendID(0x51U)
{
}

//---------------------------------------------------------------------------

uint32_t C_CanMonProtocolXfl::GetSendID(void) const
{
   return mu32_XFLSendID;
}

//---------------------------------------------------------------------------

void C_CanMonProtocolXfl::SetSendID(const uint32_t ou32_Send)
{
   mu32_XFLSendID = ou32_Send;
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
int32_t C_CanMonProtocolXfl::SaveParamsToIni(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   int32_t s32_Error = C_NO_ERR;

   try
   {
      orc_IniFile.WriteInteger(orc_Section, "PP_XFL_SEND_ID", static_cast<int32_t>(this->mu32_XFLSendID));
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
int32_t C_CanMonProtocolXfl::LoadParamsFromIni(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   this->mu32_XFLSendID = static_cast<uint32_t>(orc_IniFile.ReadInteger(orc_Section, "PP_XFL_SEND_ID",
                                                                        static_cast<int32_t>(this->mu32_XFLSendID)));
   return C_NO_ERR;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Convert CAN message to text representation.

   \param[in]     orc_Msg              message to convert

   \return
   Text interpretation of CAN message ("" if the message can not be interpreted)
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolXfl::MessageToString(const T_STWCAN_Msg_RX & orc_Msg) const
{
   int32_t s32_Index;
   C_SclString c_Text;
   C_SclString c_Help;
   C_SclString c_Help2;
   uint16_t u16_Help;
   int32_t s32_Return;
   char_t acn_CIDBuffer[6];

   if (orc_Msg.u32_ID == mu32_XFLSendID)
   {
      if ((orc_Msg.u8_DLC == 5U) &&
          (orc_Msg.au8_Data[0] == 0x46U) &&
          (orc_Msg.au8_Data[1] == 0x4CU) &&
          (orc_Msg.au8_Data[2] == 0x41U) &&
          (orc_Msg.au8_Data[3] == 0x53U) &&
          (orc_Msg.au8_Data[4] == 0x48U))
      {
         c_Text = "REQ \"FLASH\"";
         return c_Text;
      }

      //request from client
      c_Text = "REQ Node ";
      c_Help = m_GetByteAsStringFormat(orc_Msg.au8_Data[0]) + " ";
      c_Text += c_Help;
      switch (orc_Msg.au8_Data[1])
      {
      case mu8_XFL_CMD_GRP_EE_COMMAND:
         if (orc_Msg.u8_DLC != 6U)
         {
            c_Help = "";
            break;
         }
         if (orc_Msg.au8_Data[2] == mu8_XFL_CMD_EE_READ_CMD) //sub-command ee-read
         {
            c_Help = "ee_read NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[3]) + " ADD: " +
                     m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
         }
         else
         {
            c_Help = ""; //no other sub-commands defined yet
         }
         break;
      case mu8_XFL_CMD_GRP_EE_WRITE_CMD:
         if (orc_Msg.u8_DLC < 5U)
         {
            c_Help = "";
            break;
         }
         c_Help = "ee_write ADD: " + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) + " DATA: [";
         for (s32_Index = 4; s32_Index < orc_Msg.u8_DLC; s32_Index++)
         {
            c_Help += m_GetValueDecHex(orc_Msg.au8_Data[s32_Index]);
            if (s32_Index < (orc_Msg.u8_DLC - 1))
            {
               c_Help += ",";
            }
         }
         c_Help += "]";
         break;
      case 0x12U:
         if (orc_Msg.u8_DLC != 6U)
         {
            c_Help = "";
            break;
         }
         if (orc_Msg.au8_Data[2] == 0x00U) //sub-command ee-read
         {
            c_Help = "ee_read_word NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[3]) + " ADD: " +
                     m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
         }
         else
         {
            c_Help = ""; //no other sub-commands defined yet
         }
         break;
      case 0x13U:
         if (orc_Msg.u8_DLC != 8U)
         {
            c_Help = "";
            break;
         }
         if (mq_Decimal == true)
         {
            (void)c_Help.PrintFormatted("ee_write_word ADD: %d DATA: [%d,%d]",
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
         }
         else
         {
            (void)c_Help.PrintFormatted("ee_write_word ADD: %04X DATA: [%04X,%04X]",
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
         }
         break;
      case mu8_XFL_CMD_GRP_GET_COMMAND:
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_GET_LOKAL_ID:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Text = "REQ "; //local ID not relevant
            c_Help = "get_local_id ";
            break;
         case mu8_XFL_CMD_GET_SECTOR_COUNT:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_sector_count ";
            break;
         case mu8_XFL_CMD_GET_VERSION_NUMBER:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_version_number ";
            break;
         case mu8_XFL_CMD_GET_ERASE_COUNT:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_download_count ";
            break;
         case mu8_XFL_CMD_GET_DEVICE_ID:
            if ((orc_Msg.u8_DLC < 3U) || (orc_Msg.u8_DLC > 4U))
            {
               c_Help = "";
               break;
            }
            if (orc_Msg.u8_DLC == 3U)
            {
               c_Help = "get_device_id ";
            }
            else
            {
               c_Help = "get_device_id IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            }
            break;
         case mu8_XFL_CMD_GET_CONTROL_ID:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_control_id ";
            break;
         case mu8_XFL_CMD_GET_SEC_CRC:
            switch (orc_Msg.u8_DLC)
            {
            case 4U:
               c_Help = "get_sec_crc SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
               break;
            case 5U:
               c_Help = "get_sec_crc SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_GET_SEC_MODE_COMPARE:
            switch (orc_Msg.u8_DLC)
            {
            case 4U:
               c_Help = "get_sec_mode_compare SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
               break;
            case 5U:
               c_Help = "get_sec_mode_compare SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_GET_TIMEOUT_FACTOR:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_timeout_factor TYPE:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_GET_LAST_USER:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_last_user ";
            break;
         case mu8_XFL_CMD_GET_FLASH_INFORMATION:
            if ((orc_Msg.u8_DLC < 4U) || (orc_Msg.u8_DLC > 6U))
            {
               c_Help = "";
               break;
            }
            c_Help = "get_flash_information ";
            c_Help2 = "";
            switch (orc_Msg.au8_Data[3])
            {
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_IC_COUNT:
               c_Help = c_Help + "number_of_ics";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_TOTAL_SIZE:
               c_Help = c_Help + "total_size";
               c_Help2 = " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROTECTED_SECTORS:
               c_Help = c_Help + "protected_sectors";
               c_Help2 = " IDX: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_OFFSET_ADDRESS:
               c_Help = c_Help + "sector_0_offset";
               c_Help2 = " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_REGIONS:
               c_Help = c_Help + "number_of_regions";
               c_Help2 = " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_REGION_INFO:
               c_Help = c_Help + "region_information";
               c_Help2 = " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                         " REG: " + m_GetValueDecHex(orc_Msg.au8_Data[5]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ERASE_TIME:
               c_Help = c_Help + "max_erase_time";
               c_Help2 = " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROGRAM_TIME:
               c_Help = c_Help + "max_program_time";
               c_Help2 = " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_ALIASES:
               c_Help = c_Help + "num_aliases";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_PHYSICAL:
               c_Help = c_Help + "aliases_physical_address";
               c_Help2 = " RANGE: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_SIZE:
               c_Help = c_Help + "aliases_size";
               c_Help2 = " RANGE: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_ADDRESS:
               c_Help = c_Help + "aliases_alias_address";
               c_Help2 = " RANGE: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            default:
               c_Help = c_Help + "undefined_index:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
               break;
            }
            if (c_Help2 == "")
            {
               switch (orc_Msg.u8_DLC)
               {
               case 4U:
                  break;
               case 5U:
                  c_Help2 = " PAR1:" + m_GetValueDecHex(orc_Msg.au8_Data[4]);
                  break;
               case 6U:
                  c_Help2 = " PAR1:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                            " PAR2:" + m_GetValueDecHex(orc_Msg.au8_Data[5]);
                  break;
               default:
                  break;
               }
            }
            c_Help = c_Help + c_Help2;
            break;
         case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_implementation_information ";
            switch (orc_Msg.au8_Data[3])
            {
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_PROTOCOL_VERSION:
               c_Help = c_Help + "protocol_version";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SPECIAL_SERVICES:
               c_Help = c_Help + "special_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_GET_SERVICES:
               c_Help = c_Help + "get_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SET_SERVICES:
               c_Help = c_Help + "set_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_FLASH_SERVICES:
               c_Help = c_Help + "flash_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_TARGET_SPECIFIC_SERVICES:
               c_Help = c_Help + "target_specific_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_HEX_RECORDS:
               c_Help = c_Help + "hex_record_config";
               break;
            default:
               c_Help = c_Help + "undefined_index:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
               break;
            }
            break;
         case mu8_XFL_CMD_GET_FINGER_PRINT:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_finger_print ";
            switch (orc_Msg.au8_Data[3])
            {
            case 0U:
               c_Help = c_Help + "supported_indexes";
               break;
            case 1U:
               c_Help = c_Help + "programming_time";
               break;
            case 2U:
               c_Help = c_Help + "programming_date";
               break;
            case 3U:
               c_Help = c_Help + "checksum";
               break;
            case 4U:
               c_Help = c_Help + "username1";
               break;
            case 5U:
               c_Help = c_Help + "username2";
               break;
            case 6U:
               c_Help = c_Help + "username3";
               break;
            case 7U:
               c_Help = c_Help + "username4";
               break;
            default:
               c_Help = c_Help + "undefined_index:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
               break;
            }
            break;
         case mu8_XFL_CMD_GET_DEVICE_INFO_ADDRESS:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_device_info_address IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_GET_BLOCK_START_ADDRESS:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_start_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_GET_BLOCK_END_ADDRESS:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_end_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_GET_BLOCK_CHECKSUM:
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_checksum BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) + " IDX:" +
                     m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         case mu8_XFL_CMD_GET_BLOCK_COMPARE_MODE:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_compare_mode BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         default:
            c_Help = "";
            break;
         }
         break;
      case mu8_XFL_CMD_GRP_GET_SNR:
         if (orc_Msg.u8_DLC != 2U)
         {
            c_Help = "";
            break;
         }
         c_Help = "get_snr ";
         break;
      case mu8_XFL_CMD_GRP_SET_COMMAND:
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_SET_LOKAL_ID:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_local_id LID:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_SET_BITRATE_CAN:
            switch (orc_Msg.u8_DLC)
            {
            case 5U:
               c_Help = "set_bitrate_can BITRATE:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            case 7U:
               c_Help = "set_bitrate_can BITRATE:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_SET_CAN_ID:
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_can_id ID:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_SET_CAN_TYPE:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_can_type TYPE:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_SET_CONTROL_ID:
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_control_id CONID:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_SET_SEC_CRC:
            switch (orc_Msg.u8_DLC)
            {
            case 4U:
               c_Help = "set_sec_crc SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
               break;
            case 5U:
               c_Help = "set_sec_crc SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_SET_SEC_MODE_COMPARE:
            switch (orc_Msg.u8_DLC)
            {
            case 5U:
               c_Help = "set_sec_mode_compare SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                        " MODE:" + C_SclString::IntToStr(orc_Msg.au8_Data[4]);
               break;
            case 6U:
               c_Help = "set_sec_mode_compare SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3])) +
                        " MODE:" + C_SclString::IntToStr(orc_Msg.au8_Data[5]);
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_SET_TIMEOUT_FACTOR:
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_timeout_factor TYPE:" +
                     C_SclString::IntToStr(orc_Msg.au8_Data[3]) + " FAC:" + m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         case mu8_XFL_CMD_SET_DIVERT_PARAM:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_gateway_parameter DEV:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " POS:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                     " PAR:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            break;
         case mu8_XFL_CMD_SET_XFL_EXCHANGE:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_xfl_exchange ";
            break;
         case mu8_XFL_CMD_SET_FINGER_PRINT:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_finger_print IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " DB0:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " DB1:" + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                     " DB2:" + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                     " DB3:" + m_GetValueDecHex(orc_Msg.au8_Data[7]);
            break;
         case mu8_XFL_CMD_SET_TEMP_BITRATE:
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_temp_bitrate BITRATE:" + C_SclString::IntToStr(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_SET_BLOCK_START_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_start_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_SET_BLOCK_END_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_end_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_SET_BLOCK_CHECKSUM:
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_checksum BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_SET_BLOCK_COMPARE_MODE:
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_compare_mode BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " MOD:" + m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         default:
            c_Help = "";
            break;
         }
         break;
      case mu8_XFL_CMD_GRP_FLASH_COMMAND:
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_ERASE_SECTOR:
            switch (orc_Msg.u8_DLC)
            {
            case 4U:
               c_Help = "erase_sector SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
               break;
            case 5U:
               c_Help = "erase_sector SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_PROG_FLASH:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "prog_flash ";
            break;
         case mu8_XFL_CMD_NODE_SLEEP:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_sleep ";
            break;
         case mu8_XFL_CMD_NODE_RESET:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_reset ";
            break;
         case mu8_XFL_CMD_NODE_RETURN:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_return ";
            break;
         case mu8_XFL_CMD_READ_FLASH:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "read_flash NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_NODE_COMPID:
            switch (orc_Msg.u8_DLC)
            {
            case 5: // 2 bytes
            case 8: // 5 bytes
               s32_Return = CID_bytes_to_string(&orc_Msg.au8_Data[3], static_cast<uint8_t>(orc_Msg.u8_DLC - 3),
                                                &acn_CIDBuffer[0]);
               if (s32_Return == C_NO_ERR)
               {
                  (void)c_Help.PrintFormatted("node_compid ID:%s", acn_CIDBuffer);
               }
               else
               {
                  c_Help = "node_compid ID:invalid_id";
               }
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_DIVERT_STREAM:
            if ((orc_Msg.u8_DLC != 6U) && (orc_Msg.u8_DLC != 7U))
            {
               c_Help = "";
               break;
            }
            c_Help = "divert_stream DEV:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " POS:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " STATE:" + m_GetValueDecHex(orc_Msg.au8_Data[5]);
            if (orc_Msg.u8_DLC == 7U)
            {
               c_Help += " HANDLE:" + m_GetValueDecHex(orc_Msg.au8_Data[6]);
            }
            break;
         case mu8_XFL_CMD_BB_WAKEUP:
            if (orc_Msg.u8_DLC != 6U)
            {
               c_Help = "";
               break;
            }
            if (mq_Decimal == true)
            {
               (void)c_Help.PrintFormatted("wakeup_divert_client POS:%d UID:%d", orc_Msg.au8_Data[3],
                                           mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
            }
            else
            {
               (void)c_Help.PrintFormatted("wakeup_divert_client POS:%02X UID:%04X", orc_Msg.au8_Data[3],
                                           mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
            }
            break;
         case 0x09U:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "read_flash_word NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[2]) +
                     " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         default:
            c_Help = "";
            break;
         }
         break;
      case mu8_XFL_CMD_GRP_NODE_WAKEUP:
         if (orc_Msg.u8_DLC == 2U)
         {
            c_Help = "node_wakeup LID";
         }
         else
         {
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Text = "REQ "; //LocalID not relevant !
            c_Help = "node_wakeup SNR:" + this->mh_SerialNumberToString(&orc_Msg.au8_Data[2]);
         }
         break;
      case mu8_XFL_CMD_GRP_NET_COMMAND:
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_NET_START:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "net_start ";
            break;
         case mu8_XFL_CMD_NET_RESET:
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "net_reset ";
            break;
         default:
            c_Help = "";
         }
         break;
      default:
         c_Help = "";
         break;
      }
      if (c_Help != "")
      {
         c_Text += c_Help;
      }
      else
      {
         c_Text = "";
      }
   }
   else if (orc_Msg.u32_ID == (mu32_XFLSendID + 1U))
   {
      //response from ECU
      c_Text = "RES Node ";
      c_Help = m_GetByteAsStringFormat(orc_Msg.au8_Data[0]) + " ";
      c_Text += c_Help;
      switch (orc_Msg.au8_Data[1])
      {
      case mu8_XFL_CMD_GRP_EE_COMMAND: //ee_read
         if (orc_Msg.u8_DLC != 6U)
         {
            c_Help = "";
            break;
         }
         c_Help = "ee_read NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[3]) + " ADD: " +
                  m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
         break;
      case mu8_XFL_CMD_GRP_EE_WRITE_CMD: //ee_write
         if (orc_Msg.u8_DLC < 5U)
         {
            c_Help = "";
            break;
         }
         c_Help = "ee_write ADD: " + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2])) + " DATA: [";
         for (s32_Index = 4; s32_Index < orc_Msg.u8_DLC; s32_Index++)
         {
            c_Help += m_GetValueDecHex(orc_Msg.au8_Data[s32_Index]);
            if (s32_Index < (orc_Msg.u8_DLC - 1))
            {
               c_Help += ",";
            }
         }
         c_Help += "]";
         break;
      case 0x12U: //ee_read_word
         if (orc_Msg.u8_DLC != 6U)
         {
            c_Help = "";
            break;
         }
         c_Help = "ee_read_word NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[2]) + " ADD: " +
                  m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
         break;
      case 0x13U: //ee_write_word
         if (orc_Msg.u8_DLC != 8U)
         {
            c_Help = "";
            break;
         }
         if (mq_Decimal == true)
         {
            (void)c_Help.PrintFormatted("ee_write_word ADD: %d DATA: [%d,%d]",
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
         }
         else
         {
            (void)c_Help.PrintFormatted("ee_write_word ADD: %04X DATA: [%04X,%04X]",
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[2]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]),
                                        mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
         }
         break;
      case mu8_XFL_CMD_GRP_GET_COMMAND: //get command
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_GET_LOKAL_ID: //get_local_id
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_local_id ID:" + m_GetValueDecHex(orc_Msg.au8_Data[0]);
            break;
         case mu8_XFL_CMD_GET_SECTOR_COUNT: //get_sector_count
            switch (orc_Msg.u8_DLC)
            {
            case 4U:
               c_Help = "get_sector_count NUM:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
               break;
            case 5U:
               c_Help = "get_sector_count NUM:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_GET_VERSION_NUMBER: //get_version_number
            if (orc_Msg.u8_DLC == 3U)
            {
               c_Help = "get_version_number VER:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            }
            else
            {
               if (orc_Msg.u8_DLC != 8U)
               {
                  c_Help = "";
                  break;
               }
               (void)c_Help.PrintFormatted("get_version_number VER: %c.%c%c/%d",
                                           orc_Msg.au8_Data[3], orc_Msg.au8_Data[4], orc_Msg.au8_Data[5],
                                           (static_cast<uint16_t>((static_cast<uint16_t>(orc_Msg.au8_Data[6])) << 8U)) +
                                           orc_Msg.au8_Data[7]);
            }
            break;
         case mu8_XFL_CMD_GET_ERASE_COUNT: //get_download_count
            if (orc_Msg.u8_DLC != 6U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_download_count COUNT:" +
                     m_GetValueDecHex(static_cast<uint32_t>(orc_Msg.au8_Data[3] +
                                                            ((static_cast<uint32_t>(orc_Msg.au8_Data[4])) << 8U) +
                                                            ((static_cast<uint32_t>(orc_Msg.au8_Data[5])) << 16U)));
            break;
         case mu8_XFL_CMD_GET_DEVICE_ID: //get_device_id
            //Some ECUs have a shorter DLC if (orc_Msg.bDLC != 8) { c_Help = ""; break; }
            //There is no way we can know whether this is an indexed or traditional response
            //-> check for each character whether it can be displayed
            if (orc_Msg.u8_DLC < 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_device_id ID:";
            for (s32_Index = 3; s32_Index < orc_Msg.u8_DLC; s32_Index++)
            {
               if (std::isalnum(orc_Msg.au8_Data[s32_Index]) == 0)
               {
                  c_Help2 = "[" + m_GetValueDecHex(orc_Msg.au8_Data[s32_Index]) + "] ";
               }
               else
               {
                  (void)c_Help2.PrintFormatted("'%c' ", orc_Msg.au8_Data[s32_Index]);
               }
               c_Help += c_Help2;
            }
            break;
         case mu8_XFL_CMD_GET_CONTROL_ID: //get_control_id
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_control_id CONID:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_GET_SEC_CRC: //get_sec_crc
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_sec_crc CRCCALC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3])) +
                     " CRCEE:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[5]));
            break;
         case mu8_XFL_CMD_GET_SEC_MODE_COMPARE: //get_sec_mode_compare
            switch (orc_Msg.u8_DLC)
            {
            case 5U:
               c_Help = "get_sec_mode_compare SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                        " MODE:" + C_SclString::IntToStr(orc_Msg.au8_Data[4]);
               break;
            case 6U:
               c_Help = "get_sec_mode_compare SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3])) +
                        " MODE:" + C_SclString::IntToStr(orc_Msg.au8_Data[5]);
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_GET_TIMEOUT_FACTOR: //get_timeout_factor
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_timeout_factor TYPE:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " FAC:" + m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         case mu8_XFL_CMD_GET_LAST_USER: //get_last_user
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            if (mq_Decimal == true)
            {
               (void)c_Help.PrintFormatted("get_last_user UID:%d", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
            }
            else
            {
               (void)c_Help.PrintFormatted("get_last_user UID:%04X", mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
            }
            break;
         case mu8_XFL_CMD_GET_FLASH_INFORMATION:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_flash_information ";
            c_Help2 = "";
            switch (orc_Msg.au8_Data[3])
            {
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_IC_COUNT:
               c_Help = c_Help + "number_of_ics";
               c_Help2 = " ICS: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_TOTAL_SIZE:
               c_Help = c_Help + "total_size";
               c_Help2 = " SIZE: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4])) + "bytes";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROTECTED_SECTORS:
               c_Help = c_Help + "protected_sectors";
               c_Help2 = " SEC: " + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4])) +
                         " IC: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                         " STAT: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_OFFSET_ADDRESS:
               c_Help = c_Help + "sector_0_offset";
               c_Help2 = " OFFS: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4])) + "bytes";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_REGIONS:
               c_Help = c_Help + "number_of_regions";
               c_Help2 = " REGS: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_REGION_INFO:
               c_Help = c_Help + "region_information";
               u16_Help = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]);
               if (u16_Help == 0U)
               {
                  c_Help2 = " SIZE: 0bytes";
               }
               else
               {
                  c_Help2 = " SIZE: " + m_GetValueDecHex((static_cast<uint32_t>(u16_Help)) * 256U) + "bytes";
               }
               u16_Help = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]);
               c_Help2 = c_Help2 + " NUMSECS: " + m_GetValueDecHex(u16_Help);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ERASE_TIME:
               c_Help = c_Help + "max_erase_time";
               c_Help2 = " TIME: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4])) + "ms";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROGRAM_TIME:
               c_Help = c_Help + "max_program_time";
               c_Help2 = " TIME: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4])) + "ms";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_ALIASES:
               c_Help = c_Help + "num_aliases";
               c_Help2 = " RANGES: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_PHYSICAL:
               c_Help = c_Help + "aliases_physical_address";
               c_Help2 = " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_SIZE:
               c_Help = c_Help + "aliases_size";
               c_Help2 = " SIZE: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4])) + "bytes";
               break;
            case mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_ADDRESS:
               c_Help = c_Help + "aliases_alias_address";
               c_Help2 = " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
               break;
            default:
               c_Help = c_Help + "undefined_index:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
               break;
            }
            if (c_Help2 == "")
            {
               c_Help2 = " DB0: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                         " DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                         " DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                         " DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
            }
            c_Help = c_Help + c_Help2;
            break;
         case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_implementation_information ";
            c_Help2 = "";
            switch (orc_Msg.au8_Data[3])
            {
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_PROTOCOL_VERSION:
               c_Help = c_Help + "protocol_version";
               u16_Help = mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]);
               (void)c_Help2.PrintFormatted(" V%x.%x%xr%x",
                                            static_cast<uint8_t>((u16_Help >> 12U) & 0x0FU),
                                            static_cast<uint8_t>((u16_Help >> 8U) & 0x0FU),
                                            static_cast<uint8_t>((u16_Help >> 4U) & 0x0FU),
                                            static_cast<uint8_t>((u16_Help) & 0x0FU));
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SPECIAL_SERVICES:
               c_Help = c_Help + "special_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_GET_SERVICES:
               c_Help = c_Help + "get_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SET_SERVICES:
               c_Help = c_Help + "set_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_FLASH_SERVICES:
               c_Help = c_Help + "flash_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_TARGET_SPECIFIC_SERVICES:
               c_Help = c_Help + "target_specific_services";
               break;
            case mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_HEX_RECORDS:
               c_Help = c_Help + "hex_record_config";
               c_Help2 = " MAX: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) + "bytes" +
                         " GRA: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) + "bytes";
               break;
            default:
               c_Help = c_Help + "undefined_index:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
               break;
            }
            if (c_Help2 == "")
            {
               c_Help2 = " DB0: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                         " DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                         " DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                         " DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
            }
            c_Help = c_Help + c_Help2;
            break;
         case mu8_XFL_CMD_GET_FINGER_PRINT:
            if (orc_Msg.u8_DLC != 8)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_finger_print ";
            switch (orc_Msg.au8_Data[3])
            {
            case mu8_XFL_CMD_FINGER_PRINT_SUPPORTED_INDEXES:
               c_Help = c_Help + "supported_indexes";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_TIME:
               c_Help = c_Help + "programming_time";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_DATE:
               c_Help = c_Help + "programming_date";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_CHECKSUM:
               c_Help = c_Help + "checksum";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_USERNAME_1:
               c_Help = c_Help + "username1";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_USERNAME_2:
               c_Help = c_Help + "username2";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_USERNAME_3:
               c_Help = c_Help + "username3";
               break;
            case mu8_XFL_CMD_FINGER_PRINT_USERNAME_4:
               c_Help = c_Help + "username4";
               break;
            default:
               c_Help = c_Help + "undefined_index:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
               break;
            }
            c_Help = c_Help + " DB0: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                     " DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                     " DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
            break;
         case mu8_XFL_CMD_GET_DEVICE_INFO_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_device_info_address IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            c_Help = c_Help + " DB0: " + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " DB1: " + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                     " DB2: " + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                     " DB3: " + m_GetValueDecHex(orc_Msg.au8_Data[7]);
            break;
         case mu8_XFL_CMD_GET_BLOCK_START_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_start_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_GET_BLOCK_END_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_end_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_GET_BLOCK_CHECKSUM:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_checksum IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " CHK: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_GET_BLOCK_COMPARE_MODE:
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "get_block_compare_mode BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " MOD: " + m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         default:
            c_Help = "";
            break;
         }
         break;
      case mu8_XFL_CMD_GRP_GET_SNR: //get_snr
         if (orc_Msg.u8_DLC != 8U)
         {
            c_Help = "";
            break;
         }
         c_Help = "get_snr SNR:" + this->mh_SerialNumberToString(&orc_Msg.au8_Data[2]);
         break;
      case mu8_XFL_CMD_GRP_SET_COMMAND: //set command
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_SET_LOKAL_ID: //set_local_id
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_local_id LID:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_SET_BITRATE_CAN: //set_bitrate_can
            switch (orc_Msg.u8_DLC)
            {
            case 5U:
               c_Help = "set_bitrate_can BITRATE:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            case 7U:
               c_Help = "set_bitrate_can BITRATE:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_SET_CAN_ID: //set_can_id
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_can_id ID:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_SET_CAN_TYPE: //set_can_type
            if (orc_Msg.u8_DLC != 4U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_can_type TYPE:" + C_SclString::IntToStr(orc_Msg.au8_Data[3]);
            break;
         case mu8_XFL_CMD_SET_CONTROL_ID: //set_control_id
            if (orc_Msg.u8_DLC != 7U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_control_id CONID:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_SET_SEC_CRC: //set_sec_crc
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_sec_crc CRC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         case mu8_XFL_CMD_SET_SEC_MODE_COMPARE: //set_sec_mode_compare
            switch (orc_Msg.u8_DLC)
            {
            case 5U:
               c_Help = "set_sec_mode_compare SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                        " MODE:" + C_SclString::IntToStr(orc_Msg.au8_Data[4]);
               break;
            case 6U:
               c_Help = "set_sec_mode_compare SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3])) +
                        " MODE:" + C_SclString::IntToStr(orc_Msg.au8_Data[5]);
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_SET_TIMEOUT_FACTOR: //set_timeout_factor
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_timeout_factor TYPE:" +
                     C_SclString::IntToStr(orc_Msg.au8_Data[3]) + " FAC:" + m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         case mu8_XFL_CMD_SET_DIVERT_PARAM:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_gateway_parameter DEV:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " POS:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                     " PAR:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[6]));
            break;
         case mu8_XFL_CMD_SET_FINGER_PRINT:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_finger_print IDX:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " DB0:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " DB1:" + m_GetValueDecHex(orc_Msg.au8_Data[5]) +
                     " DB2:" + m_GetValueDecHex(orc_Msg.au8_Data[6]) +
                     " DB3:" + m_GetValueDecHex(orc_Msg.au8_Data[7]);
            break;
         case mu8_XFL_CMD_SET_BLOCK_START_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_start_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_SET_BLOCK_END_ADDRESS:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_end_address BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_SET_BLOCK_CHECKSUM:
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_checksum BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " CHK:" + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));
            break;
         case mu8_XFL_CMD_SET_BLOCK_COMPARE_MODE:
            if (orc_Msg.u8_DLC != 5U)
            {
               c_Help = "";
               break;
            }
            c_Help = "set_block_compare_mode BLK:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " MOD:" + m_GetValueDecHex(orc_Msg.au8_Data[4]);
            break;
         default:
            c_Help = "";
            break;
         }
         break;
      case mu8_XFL_CMD_GRP_FLASH_COMMAND: //flash commands
         switch (orc_Msg.au8_Data[2])
         {
         case mu8_XFL_CMD_ERASE_SECTOR: //erase_sector
            switch (orc_Msg.u8_DLC)
            {
            case 4U:
               c_Help = "erase_sector SEC:" + m_GetValueDecHex(orc_Msg.au8_Data[3]);
               break;
            case 5U:
               c_Help = "erase_sector SEC:" + m_GetValueDecHex(mh_BytesToWordLowHigh(&orc_Msg.au8_Data[3]));
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_PROG_FLASH: //prog_flash
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "prog_flash ";
            break;
         case mu8_XFL_CMD_NODE_SLEEP: //node_sleep
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_sleep ";
            break;
         case mu8_XFL_CMD_NODE_RESET: //node_reset
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_reset ";
            break;
         case mu8_XFL_CMD_NODE_RETURN: //node_return
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_return ";
            break;
         case mu8_XFL_CMD_READ_FLASH: //read_flash
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "read_flash NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[4]));

            break;
         case mu8_XFL_CMD_NODE_COMPID: //node_compid
            switch (orc_Msg.u8_DLC)
            {
            case 5U: // 2 bytes
            case 8U: // 5 bytes
               s32_Return = CID_bytes_to_string(&orc_Msg.au8_Data[3], static_cast<uint8_t>(orc_Msg.u8_DLC - 3U),
                                                &acn_CIDBuffer[0]);
               if (s32_Return == C_NO_ERR)
               {
                  (void)c_Help.PrintFormatted("node_compid ID:%s", acn_CIDBuffer);
               }
               else
               {
                  c_Help = "node_compid ID:invalid_id";
               }
               break;
            default:
               c_Help = "";
               break;
            }
            break;
         case mu8_XFL_CMD_DIVERT_STREAM: //divert_stream
            if ((orc_Msg.u8_DLC != 6U) && (orc_Msg.u8_DLC != 7U))
            {
               c_Help = "";
               break;
            }
            c_Help = "divert_stream DEV:" + m_GetValueDecHex(orc_Msg.au8_Data[3]) +
                     " POS:" + m_GetValueDecHex(orc_Msg.au8_Data[4]) +
                     " STATE:" + m_GetValueDecHex(orc_Msg.au8_Data[5]);
            if (orc_Msg.u8_DLC == 7U)
            {
               c_Help += " HANDLE:" + m_GetValueDecHex(orc_Msg.au8_Data[6]);
            }
            break;
         case mu8_XFL_CMD_BB_WAKEUP: //wakeup_divert_client
            if (orc_Msg.u8_DLC != 6U)
            {
               c_Help = "";
               break;
            }
            if (mq_Decimal == true)
            {
               (void)c_Help.PrintFormatted("wakeup_divert_client POS:%d UID:%d", orc_Msg.au8_Data[3],
                                           mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
            }
            else
            {
               (void)c_Help.PrintFormatted("wakeup_divert_client POS:%02X UID:%04X", orc_Msg.au8_Data[3],
                                           mh_BytesToWordLowHigh(&orc_Msg.au8_Data[4]));
            }
            break;
         case 0x09U: //read_flash_word
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "read_flash_word NUM: " + m_GetValueDecHex(orc_Msg.au8_Data[2]) +
                     " ADD: " + m_GetValueDecHex(mh_BytesToDwordLowHigh(&orc_Msg.au8_Data[3]));
            break;
         default:
            c_Help = "";
            break;
         }
         break;
      case mu8_XFL_CMD_GRP_NODE_WAKEUP: //node wakeup
         if (orc_Msg.u8_DLC == 2U)
         {
            c_Help = "node_wakeup LID";
         }
         else
         {
            if (orc_Msg.u8_DLC != 8U)
            {
               c_Help = "";
               break;
            }
            c_Help = "node_wakeup SNR:" + this->mh_SerialNumberToString(&orc_Msg.au8_Data[2]);
         }
         break;
      case mu8_COMMAND_ERR: //error
         if (orc_Msg.u8_DLC == 2U)
         {
            c_Help = "ERR: service not supported";
         }
         else
         {
            if (orc_Msg.u8_DLC != 3U)
            {
               c_Help = "";
               break;
            }
            if ((orc_Msg.au8_Data[2] >= 0x10U) && (orc_Msg.au8_Data[2] <= 0x3FU))
            {
               c_Help = "ERR: devicespecific " + m_GetValueDecHex(orc_Msg.au8_Data[2]);
            }
            else
            {
               switch (orc_Msg.au8_Data[2])
               {
               case mu8_XFL_ERR_UNDEFINED_ERROR:
                  c_Help = "ERR: UNDEFINED_ERR";
                  break;
               case mu8_XFL_ERR_CHECKSUM_ERR:
                  c_Help = "ERR: CHECKSUM_ERR";
                  break;
               case mu8_XFL_ERR_FLASH_PROG_ERR:
                  c_Help = "ERR: FLASH_PROG_ERR";
                  break;
               case mu8_XFL_ERR_FLASH_ERASE_ERR:
                  c_Help = "ERR: FLASH_ERASE_ERR";
                  break;
               case mu8_XFL_ERR_CAN_BITTIME_ERR:
                  c_Help = "ERR: CAN_BITTIME_ERR";
                  break;
               case mu8_XFL_ERR_OUT_OF_RANGE_ERR:
                  c_Help = "ERR: OUT_OF_RANGE_ERR";
                  break;
               case mu8_XFL_ERR_EE_VERIFY_ERR:
                  c_Help = "ERR: EE_VERIFY_ERR";
                  break;
               case mu8_XFL_ERR_EE_READ_ERR:
                  c_Help = "ERR: EE_READ_ERR";
                  break;
               case mu8_XFL_ERR_EE_WRITE_ERR:
                  c_Help = "ERR: EE_WRITE_ERR";
                  break;
               case mu8_XFL_ERR_DIVERT_STREAM_COM_ERR:
                  c_Help = "ERR: DIVERT_STREAM_COM_ERR";
                  break;
               case mu8_XFL_ERR_WRONG_REC_TYPE:
                  c_Help = "ERR: WRONG_REC_TYPE";
                  break;
               default:
                  c_Help = "ERR: unknown error (" + C_SclString::IntToStr(orc_Msg.au8_Data[2]) + "d)";
                  break;
               }
            }
         }
         break;
      default:
         c_Help = "";
         break;
      }
      if (c_Help != "")
      {
         c_Text += c_Help;
      }
      else
      {
         c_Text = "";
      }
   }
   else
   {
      //nothing we know about ...
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
C_SclString C_CanMonProtocolXfl::GetProtocolName(void) const
{
   return "STW Flashloader";
}

//---------------------------------------------------------------------------
