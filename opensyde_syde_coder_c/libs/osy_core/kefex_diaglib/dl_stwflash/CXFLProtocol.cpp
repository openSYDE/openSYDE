//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Encapsulate STW flashloader services.

   Provides functions to use STW flashloader protocol services to interact with flashloader server nodes.

   \copyright   Copyright 1999 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstring>

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_HexFile.hpp"
#include "CXFLProtocol.hpp"
#include "TglTime.hpp"
#include "TglUtils.hpp"
#include "stw_can.hpp"
#include "DLLocalize.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::diag_lib;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::can;
using namespace stw::hex_file;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

//timeout values (sum of timeout specified in protocol + 100ms tolerance for communication delays)
static const uint16_t mu16_TIMEOUT_CHANGE_STATE_MS   =   300U;
static const uint16_t mu16_TIMEOUT_READ_INFO_MS      =   300U;
static const uint16_t mu16_TIMEOUT_WRITE_CONFIG_MS   =  1100U;
static const uint16_t mu16_TIMEOUT_CALC_CHECKSUM_MS  =  2100U;

//command groups
static const uint8_t mu8_XFL_CMD_GRP_EE_COMMAND    = 0x10U;
static const uint8_t mu8_XFL_CMD_GRP_EE_WRITE_CMD  = 0x11U;
static const uint8_t mu8_XFL_CMD_GRP_GET_COMMAND   = 0x20U;
static const uint8_t mu8_XFL_CMD_GRP_GET_SNR       = 0x21U;
static const uint8_t mu8_XFL_CMD_GRP_SET_COMMAND   = 0x30U;
static const uint8_t mu8_XFL_CMD_GRP_FLASH_COMMAND = 0x40U;
static const uint8_t mu8_XFL_CMD_GRP_NODE_WAKEUP   = 0x41U;
static const uint8_t mu8_XFL_CMD_GRP_NET_COMMAND   = 0x50U;

//ee commands
static const uint8_t mu8_XFL_CMD_EE_READ_CMD         = 0x00U;

//get commands
static const uint8_t mu8_XFL_CMD_GET_LOKAL_ID        = 0x00U;
static const uint8_t mu8_XFL_CMD_GET_SECTOR_COUNT    = 0x01U;
static const uint8_t mu8_XFL_CMD_GET_VERSION_NUMBER  = 0x02U;
static const uint8_t mu8_XFL_CMD_GET_ERASE_COUNT     = 0x03U;
static const uint8_t mu8_XFL_CMD_GET_DEVICE_ID       = 0x04U;
static const uint8_t mu8_XFL_CMD_GET_CONTROL_ID      = 0x05U;
static const uint8_t mu8_XFL_CMD_GET_SEC_CRC         = 0x06U;
static const uint8_t mu8_XFL_CMD_GET_SEC_MODE_COMPARE = 0x07U;
static const uint8_t mu8_XFL_CMD_GET_TIMEOUT_FACTOR  = 0x08U;
static const uint8_t mu8_XFL_CMD_GET_LAST_USER       = 0x09U;
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
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_IC_COUNT          = 0x01U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_TOTAL_SIZE        = 0x02U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROTECTED_SECTORS = 0x03U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_OFFSET_ADDRESS    = 0x04U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_REGIONS       = 0x05U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_REGION_INFO       = 0x06U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ERASE_TIME        = 0x07U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROGRAM_TIME      = 0x08U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_ALIASES       = 0x09U;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_PHYSICAL    = 0x0AU;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_SIZE        = 0x0BU;
static const uint8_t mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_ADDRESS     = 0x0CU;

//get_implementation_information
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_PROTOCOL_VERSION = 0x01U;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SPECIAL_SERVICES = 0x02U;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_GET_SERVICES     = 0x03U;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SET_SERVICES     = 0x04U;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_FLASH_SERVICES   = 0x05U;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_TARGET_SPECIFIC_SERVICES = 0x06U;
static const uint8_t mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_HEX_RECORDS      = 0x07U;

//get_set_finger_print
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_SUPPORTED_INDEXES = 0x00U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_TIME  = 0x01U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_DATE  = 0x02U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_CHECKSUM          = 0x03U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_1        = 0x04U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_2        = 0x05U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_3        = 0x06U;
static const uint8_t mu8_XFL_CMD_FINGER_PRINT_USERNAME_4        = 0x07U;

//get_device_info_address
static const uint8_t mu8_XFL_CMD_GET_DEVICE_INFO_ADDRESS_NUM_BLOCKS = 0xFFU;

//set command
static const uint8_t mu8_XFL_CMD_SET_LOKAL_ID            = 0x00U;
static const uint8_t mu8_XFL_CMD_SET_BITRATE_CAN         = 0x01U;
static const uint8_t mu8_XFL_CMD_SET_CAN_ID              = 0x02U;
static const uint8_t mu8_XFL_CMD_SET_CAN_TYPE            = 0x03U;
//static const uint8 mu8_XFL_CMD_SET_CONTROL_ID          = 0x04U;
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

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_XFLProtocolConfig::C_XFLProtocolConfig(void) :
   pc_CANDispatcher(NULL),
   u32_SendID(XFL_DEFAULT_SEND_ID),
   u32_ReceiveID(XFL_DEFAULT_SEND_ID + 1U),
   u8_LocalID(0U),
   u32_Bitrate(125U),
   q_XtdID(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
//returns offset to add to aliased address to reach physical address:
int64_t C_XFLAliasedRange::GetOffset(void) const
{
   return (static_cast<int64_t>(this->u32_PhysicalAddress) - static_cast<int64_t>(this->u32_AliasedAddress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the aliased range the specified address resides in

   \param[in]   ou32_Address       address to check
   \param[out]  oru8_Range         index of aliased range the address resides in

   \return
   C_NO_ERR    no problems; oru8_AliasedRange contains aliased range index
   else        could not find address within any aliased range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLAliasedRanges::GetRangeOccupiedByAddress(const uint32_t ou32_Address, uint8_t & oru8_Range) const
{
   int32_t s32_Return = C_RANGE;
   int32_t s32_Index;

   oru8_Range = 0U;
   for (s32_Index = 0; s32_Index < this->GetLength(); s32_Index++)
   {
      if ((ou32_Address >= this->operator [](s32_Index).u32_AliasedAddress) &&
          (ou32_Address < (this->operator [](s32_Index).u32_AliasedAddress + this->operator [](s32_Index).u32_Size)))
      {
         oru8_Range = static_cast<uint8_t>(s32_Index);
         s32_Return = C_NO_ERR;
         break;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLImplementedServices::Clear(void)
{
   q_EERead = false;
   q_EEWrite = false;
   q_NetStart = false;
   q_NetReset = false;
   q_NetSleep = false;
   q_GetSNR = false;
   q_NodeWakeupLID = false;
   q_NodeWakeupSNR = false;
   q_GetLocalID = false;
   q_GetSectorCount = false;
   q_GetVersionNumber = false;
   q_GetDownloadCount = false;
   q_GetDeviceID = false;
   q_GetControlID = false;
   q_GetSectorChecksum = false;
   q_GetSectorModeCompare = false;
   q_GetTimeOutFactor = false;
   q_GetLastUser = false;
   q_GetFlashInformation = false;
   q_GetImplementationInformation = false;
   q_GetFingerPrint = false;
   q_GetDeviceInfoAddress = false;
   q_GetBlockStartAddress = false;
   q_GetBlockEndAddress = false;
   q_GetBlockChecksum = false;
   q_GetBlockCompareMode = false;
   q_SetLocalID = false;
   q_SetCommBitrate = false;
   q_SetCommID = false;
   q_SetCommType = false;
   q_SetControlID = false;
   q_SetSectorChecksum = false;
   q_SetSectorModeCompare = false;
   q_SetTimeoutFactor = false;
   q_SetGatewayParameter = false;
   q_SetXflExchange = false;
   q_SetFingerPrint = false;
   q_SetTempBitrate = false;
   q_SetBlockStartAddress = false;
   q_SetBlockEndAddress = false;
   q_SetBlockChecksum = false;
   q_SetBlockCompareMode = false;
   q_EraseSector = false;
   q_ProgFlash = false;
   q_NodeSleep = false;
   q_NodeReset = false;
   q_NodeReturn = false;
   q_ReadFlash = false;
   q_NodeCompID = false;
   q_DivertStream = false;
   q_WakeupDivertClient = false;
   q_Reserved9 = false;
   q_ReadSeed = false;
   q_WriteKey = false;
   q_ResetLocks = false;
   q_EnterBSL = false;
   (void)memset(&aq_Reserved1[0], 0, sizeof(aq_Reserved1));
   (void)memset(&aq_Reserved2[0], 0, sizeof(aq_Reserved2));
   (void)memset(&aq_Reserved3[0], 0, sizeof(aq_Reserved3));
   (void)memset(&aq_Reserved4[0], 0, sizeof(aq_Reserved4));
   (void)memset(&aq_Reserved6[0], 0, sizeof(aq_Reserved6));
   (void)memset(&aq_Reserved7[0], 0, sizeof(aq_Reserved7));
   (void)memset(&aq_Reserved8[0], 0, sizeof(aq_Reserved8));
   (void)memset(&aq_TargetSpecific[0], 0, sizeof(aq_TargetSpecific));
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLImplementedServices::C_XFLImplementedServices(void)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLFingerPrintSupportedIndexes::C_XFLFingerPrintSupportedIndexes(void)
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLFingerPrintSupportedIndexes::Clear(void)
{
   q_ProgrammingTime = false;
   q_ProgrammingDate = false;
   q_ApplicationCheckSum = false;
   q_UsernamePart1 = false;
   q_UsernamePart2 = false;
   q_UsernamePart3 = false;
   q_UsernamePart4 = false;
   (void)memset(&aq_Reserved[0], 0, sizeof(aq_Reserved));
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t C_XFLProtocol::mh_BCD2DEC(const uint8_t ou8_BCDValue)
{
   return (static_cast<uint8_t>(((((ou8_BCDValue) / 16U) * 10U) + ((ou8_BCDValue) % 16U))));
}

//----------------------------------------------------------------------------------------------------------------------

uint8_t C_XFLProtocol::mh_DEC2BCD(const uint8_t ou8_DECValue)
{
   return (static_cast<uint8_t>(((((ou8_DECValue) / 10U) * 16U) + ((ou8_DECValue) % 10U))));
}

//----------------------------------------------------------------------------------------------------------------------
//convert array of 4 bytes in little-endian order to uint32 value
uint32_t C_XFLProtocol::mh_AU8ToU32LE(const uint8_t * const opu8_Data)
{
   return ((static_cast<uint32_t>(opu8_Data[0])) +
           ((static_cast<uint32_t>(opu8_Data[1])) <<  8U) +
           ((static_cast<uint32_t>(opu8_Data[2])) << 16U) +
           ((static_cast<uint32_t>(opu8_Data[3])) << 24U));
}

//----------------------------------------------------------------------------------------------------------------------
//convert array of 2 bytes in little-endian order to uint16 value
uint16_t C_XFLProtocol::mh_AU8ToU16LE(const uint8_t * const opu8_Data)
{
   return ((static_cast<uint16_t>(opu8_Data[0])) +
           static_cast<uint16_t>((static_cast<uint16_t>(opu8_Data[1])) <<  8U));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make comm dispatcher to use known

   Set comm dispatcher driver to use.
   Will register a client with the installed driver.

   \param[in]   opc_CANDispatcher  comm dispatcher to use (NULL: remove installed dispatcher)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::CfgSetCommDispatcher(C_CanDispatcher * const opc_CANDispatcher)
{
   if (mc_Config.pc_CANDispatcher != NULL)
   {
      //remove existing registration:
      (void)mc_Config.pc_CANDispatcher->RemoveClient(mu16_CANDispatcherHandle);
   }

   mc_Config.pc_CANDispatcher = opc_CANDispatcher;
   if (mc_Config.pc_CANDispatcher != NULL)
   {
      //new registration:
      mc_Config.pc_CANDispatcher->RegisterClient(mu16_CANDispatcherHandle, NULL);
      m_UpdateDispatcherRXFilter();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Make used CAN bitrate known to this class

   The configured bitrate is used for internal bus load calculations

   \param[in]   ou32_Bitrate   bitrate the CAN bus runs at
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::CfgSetBitrate(const uint32_t ou32_Bitrate)
{
   mc_Config.u32_Bitrate = ou32_Bitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set CAN TX and RX CAN identifiers

   If the ReceiveID is set to 0xFFFFFFFF we accept all responses for compatibility with stwrs232.dll

   \param[in]   ou32_SendId      TX CAN ID to send requests on
   \param[in]   ou32_ReceiveId   TX CAN ID to expect responses on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::CfgSetFlashId(const uint32_t ou32_SendId, uint32_t const ou32_ReceiveId)
{
   mc_Config.u32_SendID = ou32_SendId;
   mc_Config.u32_ReceiveID = ou32_ReceiveId;
   m_UpdateDispatcherRXFilter();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Configure whether to use 11bit or 29bit CAN IDs

   \param[in]   oq_XtdOn     false: 11bit IDs; true: 29bit IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::CfgSetXtdId(const bool oq_XtdOn)
{
   mc_Config.q_XtdID = oq_XtdOn;
   mc_CanWriteMessage.u8_XTD = oq_XtdOn;
   m_UpdateDispatcherRXFilter();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set local ID for target to communicate with

   Used for all services directed at one specific node.

   \param[in]   ou8_LocalID   local ID of server node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::CfgSetLocalId(const uint8_t ou8_LocalID)
{
   mc_Config.u8_LocalID = ou8_LocalID;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request net_reset

   Send net_reset request.

   \return
   C_NO_ERR    request send
   else        error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::NetReset(void)
{
   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[0] = 0xFFU; //all nodes !!
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_NET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_NET_RESET;
   return m_CANSendMessage();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Request net_start

   Send net_start request.

   \return
   C_NO_ERR    request send
   else        error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::NetStart(void)
{
   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[0] = 0xFFU; //all nodes !!
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_NET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_NET_START;
   return m_CANSendMessage();
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::m_SendFlashMessage(void)
{
   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.u8_DLC = 5U;
   mc_CanWriteMessage.au8_Data[0] = static_cast<uint8_t>('F');
   mc_CanWriteMessage.au8_Data[1] = static_cast<uint8_t>('L');
   mc_CanWriteMessage.au8_Data[2] = static_cast<uint8_t>('A');
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>('S');
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>('H');
   return m_CANSendMessage();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send "FLASH" requests

   Send the "FLASH" request for a defined period of time with a configurable interval.
   Will report progress to application via the installed progress callback.
   Using the progress callback this function can also be aborted.

   \param[in]   ou32_StartTimeMs       period to send the message for in milli-seconds
   \param[in]   ou8_FLASHIntervalMs    interval between individual messages in milli-seconds

   \return
   C_NO_ERR      finished sending "FLASH"   \n
   C_DEFAULT     aborted by user
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SendFLASH(const uint32_t ou32_StartTimeMs, const uint8_t ou8_FLASHIntervalMs)
{
   uint32_t u32_StartTime;
   int32_t s32_Return;
   uint32_t u32_Progress;
   uint32_t u32_Elapsed;

   u32_StartTime = TglGetTickCount();
   TRG_ReportProgress(0U, TGL_LoadStr(STR_FDL_TXT_WR_FLASH_RQ));
   do
   {
      (void)m_SendFlashMessage(); //do not check for return = OK here:
                                  //network might still be down resulting in send errors
      TRG_WaitMicroSeconds(static_cast<uint32_t>(ou8_FLASHIntervalMs) * 1000U);
      TRG_HandleSystemMessages(); //prevent system from freezing
      u32_Elapsed = TglGetTickCount() - u32_StartTime;
      if (ou32_StartTimeMs != 0U)
      {
         u32_Progress = (u32_Elapsed * 1000U) / ou32_StartTimeMs;
         if (u32_Progress > 1000U) //prevent overflows ...
         {
            u32_Progress = 1000U;
         }
         s32_Return = TRG_ReportProgress(static_cast<uint16_t>(u32_Progress), TGL_LoadStr(STR_FDL_TXT_WR_FLASH_RQ));
         if (s32_Return != C_NO_ERR)
         {
            return C_DEFAULT; //user abort
         }
      }
   }
   while (u32_Elapsed < ou32_StartTimeMs);
   TRG_ReportProgress(1000U, TGL_LoadStr(STR_FDL_TXT_WR_FLASH_RQ));
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of erase_sector service.

   erase_sector client implementation.
   Erases one sector of the server's flash memory.
   Communication timeout:
   For compatibility reasons ou32_TimeOut will only be used if it is greater than TIMEOUT_ERASE.
   Otherwise TIMEOUT_ERASE will be used.

   \param[in]   ou16_SectorNumber     index of sector to delete
   \param[in]   ou32_TimeOut          timeout for waiting for response

   \return
   C_NO_ERR     sector erased   \n
   C_NOACT      error response  \n
   C_COM        no response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::EraseSector(const uint16_t ou16_SectorNumber, const uint32_t ou32_TimeOut)
{
   uint32_t u32_TimeOut;

   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_ERASE_SECTOR;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou16_SectorNumber);
   if (ou16_SectorNumber > 255U)
   {
      mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou16_SectorNumber >> 8U);
      mc_CanWriteMessage.u8_DLC = 5U;
   }
   m_SendMessageWithIDs(); // send erase request

   if (ou32_TimeOut > TIMEOUT_ERASE_MS)
   {
      u32_TimeOut = ou32_TimeOut;
   }
   else
   {
      u32_TimeOut = TIMEOUT_ERASE_MS;
   }

   return m_WaitForResponse(mc_Config.u8_LocalID, u32_TimeOut, mc_CanWriteMessage.au8_Data,
                            static_cast<uint8_t>(mc_CanWriteMessage.u8_DLC - 1U), NULL, mc_CanWriteMessage.u8_DLC);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   wait for a "regular" response from the server node

   Expects an incoming message on the currently configured RX-ID.
   The first data byte must contain ou8_ExpectedLocalID.
   Detected error codes in data byte 2 will be stored in m_bLastError.
   If OK the response message will be in m_tCanReadMessage.

   \param[in]     ou8_ExpectedLocalID        expected local ID in data byte 0
   \param[in]     ou32_TimeOutMS             timeout in milli-seconds
   \param[in]     orau8_Match                CAN message data to check for match  \n
                                             orau8_Match[1] to orau8_Match[1 + ou8_NumExpectedToMatch] will be checked
   \param[in]     ou8_NumExpectedToMatch     number of bytes to check for match
   \param[in]     os16_CheckDLC              -1 -> do not check response DLC, else: response DLC must be ==os16_CheckDLC

   \param[out]    opc_MSG                    received message (NULL -> do not return received message)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server (code stored in m_bLastError)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::m_WaitForResponse(const uint8_t ou8_ExpectedLocalID, const uint32_t ou32_TimeOutMS,
                                         const uint8_t (&orau8_Match)[8], const uint8_t ou8_NumExpectedToMatch,
                                         T_STWCAN_Msg_RX * const opc_MSG, const int16_t os16_CheckDLC)
{
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;

   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if (t_RXMsg.au8_Data[0] == ou8_ExpectedLocalID)
         {
            if (memcmp(&orau8_Match[1], &t_RXMsg.au8_Data[1], ou8_NumExpectedToMatch) == 0)
            {
               if ((os16_CheckDLC == -1) || (t_RXMsg.u8_DLC == static_cast<uint8_t>(os16_CheckDLC)))
               {
                  if (opc_MSG != NULL)
                  {
                     (*opc_MSG) = t_RXMsg;
                  }
                  return C_NO_ERR; //expected response received !
               }
            }
            if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR) //error
            {
               mu8_LastError = t_RXMsg.au8_Data[2];
               return C_NOACT;
            }
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < ou32_TimeOutMS);
   return C_COM;
}

//----------------------------------------------------------------------------------------------------------------------
// ProgFlash ;  send programming mode request
int32_t C_XFLProtocol::ProgFlash(void)
{
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_PROG_FLASH;

   m_SendMessageWithIDs(); // send request
   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 2U, NULL,
                            3);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send one hex file line to server ECU

   Send one hex file line to the server.
   For compatibility reasons ou32_TimeOut will only be used if it is greater than TIMEOUT_FLASH.
   Otherwise TIMEOUT_FLASH will be used.

   \param[in]     opu8_HexLine              data to send
   \param[in]     ou16_LineSize             number of bytes in opu8_HexLine
   \param[in]     ou32_InterFrameDelayUs    delay to keep between individual CAN frames in micro-seconds
   \param[in]     ou32_TimeOut              maximum timeout for writing one hex line in ms

   \return
   C_NO_ERR                -> Lines successfully flashed                      \n
   C_CHECKSUM              -> controller checksum different from PC checksum  \n
   C_NOACT                 -> error response                                  \n
   C_COM                   -> no response from controller or TX error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::m_SendHexLine(const uint8_t * const opu8_HexLine, const uint16_t ou16_LineSize,
                                     const uint32_t ou32_InterFrameDelayUs, const uint32_t ou32_TimeOut)
{
   uint8_t u8_LineCheckSum;
   uint32_t u32_TimeOut;
   uint32_t u32_Tries;
   int32_t s32_RemainingLength;
   int32_t s32_Return;
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;
   static const uint16_t hu16_MAX_TRIES = 5U;

   u8_LineCheckSum = opu8_HexLine[ou16_LineSize - 1U];
   u32_Tries = 0U;

   if (ou32_TimeOut > TIMEOUT_FLASH_MS)
   {
      u32_TimeOut = ou32_TimeOut;
   }
   else
   {
      u32_TimeOut = TIMEOUT_FLASH_MS;
   }

   do
   {
      uint8_t u8_SendNow;
      int32_t s32_Counter;
      u32_Tries++;
      s32_Counter = 1;
      s32_RemainingLength = ou16_LineSize;

      //send one hex-line
      while (s32_RemainingLength > 0)
      {
         u8_SendNow = static_cast<uint8_t>((s32_RemainingLength > 7) ? 7 : s32_RemainingLength);
         (void)memcpy(&mc_CanWriteMessage.au8_Data[1], &opu8_HexLine[s32_Counter - 1], u8_SendNow);
         s32_Counter += u8_SendNow;
         mc_CanWriteMessage.u8_DLC = static_cast<uint8_t>(1U + u8_SendNow);
         s32_Return = m_SendMessageWithIDs();
         if (s32_Return != C_NO_ERR)
         {
            //TX error
            return C_COM;
         }
         s32_RemainingLength -= u8_SendNow;
         if (s32_RemainingLength > 0)
         {
            TRG_WaitMicroSeconds(ou32_InterFrameDelayUs); // TX-delay between individual messages
            TRG_HandleSystemMessages();
         }
      }

      u32_StartTime = TglGetTickCount();

      //evaluate response from ECU
      do
      {
         TRG_WaitMicroSeconds(ou32_InterFrameDelayUs);
         TRG_HandleSystemMessages();
         if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
         {
            if (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID)
            {
               switch (t_RXMsg.au8_Data[1])
               {
               case 0x00U:
                  //compare received Checksum with the sent one
                  if (t_RXMsg.au8_Data[2] != u8_LineCheckSum)
                  {
                     return C_CHECKSUM;
                  }
                  u32_Tries = hu16_MAX_TRIES + 1U; // checksum OK -> finished with this line
                  break;
               case mu8_COMMAND_ERR:
                  switch (t_RXMsg.au8_Data[2])
                  {
                  case XFL_ERR_CHECKSUM_ERR:
                  case 0xF5U: //incorrect checksum return value in ESX2 flashloader up to V2.01
                     if (u32_Tries == (hu16_MAX_TRIES - 1U))
                     {
                        mu8_LastError = t_RXMsg.au8_Data[2];
                        return C_NOACT;
                     }
                     break;
                  default:
                     mu8_LastError = t_RXMsg.au8_Data[2];
                     return C_NOACT;
                  }
                  break;
               default:
                  break;
               }
            }
         }
      }
      while ((u32_Tries < hu16_MAX_TRIES) && ((TglGetTickCount() - u32_StartTime) < u32_TimeOut));
      TRG_HandleSystemMessages();
   }
   while (u32_Tries < hu16_MAX_TRIES);
   if (u32_Tries != (hu16_MAX_TRIES + 1U))
   {
      return C_COM; // Maximale Fehlversuche erreicht
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Send lines of a hex file to server ECU

   Send hex file lines to the server.
   The line to start with is determined by the state of opc_HexFile:
   NextLine() will be used to get the first line to send.
   For compatibility reasons ou32_TimeOut will only be used if it is greater than TIMEOUT_FLASH.
   Otherwise TIMEOUT_FLASH will be used.

   \param[in,out] oru16_NumLines            number of lines to send / number of lines sent
   \param[in]     ou32_InterFrameDelayUs    delay to keep between individual CAN frames in micro-seconds
   \param[in,out] orc_HexFile               hex file class instance with opened hex file
   \param[out]    oru32_NumBytesSent        total number of bytes sent in call
   \param[in]     ou32_TimeOut              maximum timeout for writing one hex line in ms

   \return
   C_NO_ERR                -> Lines successfully flashed                      \n
   1                       -> end of file reached                             \n
   C_CHECKSUM              -> controller checksum different from PC checksum  \n
   C_NOACT                 -> error response                                  \n
   C_COM                   -> no response from controller or TX error
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SendMultiHexLine(uint16_t & oru16_NumLines, const uint32_t ou32_InterFrameDelayUs,
                                        uint32_t & oru32_NumBytesSent, C_HexFile & orc_HexFile,
                                        const uint32_t ou32_TimeOut)
{
   uint16_t u16_LineLength;
   const uint8_t * pu8_NextLine;
   uint16_t u16_NumLinesToSend;

   int32_t s32_Return;

   u16_NumLinesToSend = oru16_NumLines;
   oru32_NumBytesSent = 0U;
   oru16_NumLines = 0U;

   do
   {
      pu8_NextLine = orc_HexFile.NextLine();
      if (pu8_NextLine == NULL)
      {
         return 1; //finished: no more lines
      }

      if (pu8_NextLine[3] == 0x00U) //data record
      {
         oru32_NumBytesSent += pu8_NextLine[0];
      }

      u16_LineLength = static_cast<uint16_t>((static_cast<uint16_t>(pu8_NextLine[0])) + 5U);

      s32_Return = m_SendHexLine(pu8_NextLine, u16_LineLength, ou32_InterFrameDelayUs, ou32_TimeOut);
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      TRG_HandleSystemMessages();
      oru16_NumLines++;
   }
   while (oru16_NumLines < u16_NumLinesToSend);
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   perform a wakeup with serial number

   Sequence:
   - perform wakeup_snr
   - perform send_compid

   \param[in]     orau8_SerialNumber server's serial number
   \param[in]     orc_CompanyID      server's company ID
   \param[out]    oru8_LocalID       server's local ID
   \param[out]    opc_CompanyIDOut   server's reported company ID (NULL -> not interested)

   \return
   C_NO_ERR           no errors                                                      \n
   C_WARN             server responded with company ID different from ou8_CompanyID  \n
   C_COM              no response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::WakeupSerialNumber(const uint8_t (&orau8_SerialNumber)[6], const C_XFLCompanyID & orc_CompanyID,
                                          uint8_t & oru8_LocalID, C_XFLCompanyID * const opc_CompanyIDOut)
{
   uint32_t u32_StartTime;
   uint8_t u8_Dummy;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;

   mc_CanWriteMessage.u8_DLC = 8U;
   mc_CanWriteMessage.au8_Data[0] = 0x99U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_NODE_WAKEUP;
   (void)memcpy(&mc_CanWriteMessage.au8_Data[2], &orau8_SerialNumber[0], 6U);

   m_CANSendMessage();

   u32_StartTime = TglGetTickCount();

   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if ((t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_NODE_WAKEUP) &&
             (t_RXMsg.au8_Data[2] == orau8_SerialNumber[0]) &&
             (t_RXMsg.au8_Data[3] == orau8_SerialNumber[1]) &&
             (t_RXMsg.au8_Data[4] == orau8_SerialNumber[2]) &&
             (t_RXMsg.au8_Data[5] == orau8_SerialNumber[3]) &&
             (t_RXMsg.au8_Data[6] == orau8_SerialNumber[4]) &&
             (t_RXMsg.au8_Data[7] == orau8_SerialNumber[5]))
         {
            oru8_LocalID = t_RXMsg.au8_Data[0];
            return m_SendCompanyIDExt(oru8_LocalID, orc_CompanyID, u8_Dummy, opc_CompanyIDOut);
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_CHANGE_STATE_MS);
   return C_COM;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::m_NodeFlashFunc(const uint8_t ou8_SubCommand)
{
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = ou8_SubCommand;
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 2U, NULL);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::NodeSleep(void)
{
   return m_NodeFlashFunc(mu8_XFL_CMD_NODE_SLEEP);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::NodeReturn(void)
{
   return m_NodeFlashFunc(mu8_XFL_CMD_NODE_RETURN);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::NodeReset(void)
{
   return m_NodeFlashFunc(mu8_XFL_CMD_NODE_RESET);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::GetSectorCount(uint16_t & oru16_NumSectors)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_SECTOR_COUNT;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg);
   if (s32_Return == C_NO_ERR)
   {
      switch (t_RXMsg.u8_DLC)
      {
      case 4:
         oru16_NumSectors = t_RXMsg.au8_Data[3];
         break;
      case 5:
         oru16_NumSectors = mh_AU8ToU16LE(&t_RXMsg.au8_Data[3]);
         break;
      default:
         s32_Return = C_RANGE;
         break;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::GetLocalID(uint8_t & oru8_LocalID)
{
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[0] = 0x00U; // don't care, only one device may be
                                           //  in wakeup state at a time
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_LOKAL_ID;

   m_CANSendMessage();

   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if ((t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_GET_COMMAND) &&
             (t_RXMsg.au8_Data[2] == mu8_XFL_CMD_GET_LOKAL_ID))
         {
            oru8_LocalID = t_RXMsg.au8_Data[0];
            return C_NO_ERR;
         }
         if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
         {
            mu8_LastError = t_RXMsg.au8_Data[2];
            return C_NOACT;
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_READ_INFO_MS);
   return C_COM;
}

//************************************************************************
// .FUNCTION    GetLocalIdExt
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  - read local_ID from all controllers controller(s) who are active
//  - answers from more than one controller on one ID are possible
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS
// INPUT:
//              uint8 *opu8_LocalId          Array with all received Local_Ids from CAN network
//              uint8 oru8_NumMaxLocalId     Length of record *pbLocalID
// OUTPUT:
//              ou8_NumFoundECUs            number of detected ECUs
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR       -> no errors
// else error
//************************************************************************
int32_t C_XFLProtocol::GetLocalIDExt(uint8_t * const opu8_LocalId, const uint8_t ou8_NumMaxLocalId,
                                     uint8_t & oru8_NumFoundECUs)
{
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;
   uint8_t u8_Found = 0U;

   // Found Ecu reset
   oru8_NumFoundECUs = 0U;

   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[0] = 0x0U; // don't care, only one device may be
                                          //  in wakeup state at a time
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_LOKAL_ID;

   m_CANSendMessage();

   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if ((t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_GET_COMMAND) &&
             (t_RXMsg.au8_Data[2] == mu8_XFL_CMD_GET_LOKAL_ID))
         {
            if (u8_Found < ou8_NumMaxLocalId)
            {
               // Eintrag Local_Id
               opu8_LocalId[u8_Found] = t_RXMsg.au8_Data[0];
               u8_Found++;
            }
            else // kein Platz in Array mehr !
            {
               return 1;
            }
         }
         if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
         {
            mu8_LastError = t_RXMsg.au8_Data[2];
            return C_NOACT;
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_READ_INFO_MS);
   if (u8_Found > 0U)
   {
      oru8_NumFoundECUs = u8_Found;
      return C_NO_ERR;
   }
   return C_COM;
}

//************************************************************************
// .FUNCTION    NodeSleep
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  - execute node_sleep( LocalId ) command
//  - answers from exact one controller on ID are possible
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS
// INPUT:
//              uint8 ou8_LocalID    Local_Id for request
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR       -> no errors
// else C_WARN, C_COM
//************************************************************************
int32_t C_XFLProtocol::NodeSleep(const uint8_t ou8_LocalID)
{
   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.au8_Data[0] = ou8_LocalID;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_NODE_SLEEP;
   m_CANSendMessage();

   //use write timeout for compatibility reasons: some ancient flashloader implementations update a CRC over
   // configuration data when going to sleep ...
   return m_WaitForResponse(ou8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 2U, NULL, 3);
}

//----------------------------------------------------------------------------------------------------------------------

int32_t C_XFLProtocol::GetSNR(uint8_t (&orau8_SNR)[6])
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 2U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_SNR;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 1U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&orau8_SNR[0], &t_RXMsg.au8_Data[2], 6U);
   }
   return s32_Return;
}

//************************************************************************
// .FUNCTION    SetLocalID
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  - change the local ID of a node
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS  uint8 ou8_LocalIDNew    new local ID
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR                -> no errors
// C_COM                   -> no response
// C_NOACT                 -> error response
//************************************************************************
int32_t C_XFLProtocol::SetLocalID(const uint8_t ou8_LocalIDNew)
{
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_LOKAL_ID;
   mc_CanWriteMessage.au8_Data[3] = ou8_LocalIDNew;
   m_SendMessageWithIDs();

   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         //some ESX implementations will answer with old local ID in contradiction to the specification
         if ((t_RXMsg.au8_Data[0] == ou8_LocalIDNew) ||
             (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID))
         {
            if ((t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_SET_COMMAND) &&
                (t_RXMsg.au8_Data[2] == mu8_XFL_CMD_SET_LOKAL_ID) &&
                (t_RXMsg.au8_Data[3] == ou8_LocalIDNew))
            {
               mc_Config.u8_LocalID = ou8_LocalIDNew;
               return C_NO_ERR;
            }
            if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR) //error
            {
               mu8_LastError = t_RXMsg.au8_Data[2];
               return C_NOACT;
            }
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_WRITE_CONFIG_MS);
   return C_COM;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change the flash bitrate of a node

   To be compatible with all existing server nodes use only 16bit bitrate.

   \param[in]      ou32_Bitrate        new bitrate
   \param[in]      oq_32BitBitrate     true  -> 32 bit bitrate -> bitrate resolution bits/second \n
                                       false -> 16 bit bitrate -> bitrate resolution kBits/second

                                       \return
   C_NO_ERR   no errors        \n
   C_COM      no response      \n
   C_NOACT    error response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetBitrateCAN(const uint32_t ou32_Bitrate, const bool oq_32BitBitrate)
{
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_BITRATE_CAN;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou32_Bitrate);
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou32_Bitrate >> 8U);

   if (oq_32BitBitrate == false)
   {
      mc_CanWriteMessage.u8_DLC = 5U;
   }
   else
   {
      mc_CanWriteMessage.u8_DLC = 7U;
      mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou32_Bitrate >> 16U);
      mc_CanWriteMessage.au8_Data[6] = static_cast<uint8_t>(ou32_Bitrate >> 24U);
   }
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data,
                            static_cast<uint8_t>(mc_CanWriteMessage.u8_DLC - 1U), NULL, mc_CanWriteMessage.u8_DLC);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_comm_id service.

   set_comm_id client implementation.
   Change the message ID for flashing on the server node.

   \param[in]      ou32_CANID     new message ID

   \return
   C_NO_ERR   no errors        \n
   C_COM      no response      \n
   C_NOACT    error response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetCANID(const uint32_t ou32_CANID)
{
   mc_CanWriteMessage.u8_DLC = 7U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_CAN_ID;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou32_CANID);
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou32_CANID >> 8U);
   mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou32_CANID >> 16U);
   mc_CanWriteMessage.au8_Data[6] = static_cast<uint8_t>(ou32_CANID >> 24U);
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 6U, NULL,
                            7);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_comm_type service.

   Set_comm_type client implementation.
   Configure the message type 11/29Bit for flashing on the server node.

   \param[in]      oq_XtdOn     true  -> 29Bit; false -> 11Bit

   \return
   C_NO_ERR   no errors        \n
   C_COM      no response      \n
   C_NOACT    error response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetCANType(const bool oq_XtdOn)
{
   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_CAN_TYPE;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(oq_XtdOn);
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 3U, NULL,
                            4);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read data from server node.

   Read a maximum of 255 bytes from the server node's flash memory.

   \param[in]     ou8_NumBytes       number of bytes to read (1..255)
   \param[in]     ou32_StartAddress  start address to read from
   \param[out]    opu8_Data          read data

   \return
   C_NO_ERR           no error                                        \n
   C_COM              no response                                     \n
   C_NOACT            error response or invalid response              \n
   C_CHECKSUM         received and calculated checksums do not match
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::ReadFlash(const uint8_t ou8_NumBytes, const uint32_t ou32_StartAddress,
                                 uint8_t * const opu8_Data)
{
   uint16_t u16_Counter = 0U;
   uint8_t u8_ChecksumCalc;
   uint8_t u8_ChecksumReceived = 0U;
   uint32_t u32_StartTime;
   uint8_t u8_Loop;
   int32_t s32_Return;
   uint8_t u8_CopyNow;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 8U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_READ_FLASH;
   mc_CanWriteMessage.au8_Data[3] = ou8_NumBytes;
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou32_StartAddress);
   mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou32_StartAddress >> 8U);
   mc_CanWriteMessage.au8_Data[6] = static_cast<uint8_t>(ou32_StartAddress >> 16U);
   mc_CanWriteMessage.au8_Data[7] = static_cast<uint8_t>(ou32_StartAddress >> 24U);
   m_SendMessageWithIDs(); // send ReadFlash request

   //wait for 1st response
   s32_Return = C_COM;
   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         //ESX2 only replies with 7 bytes, so only check those
         if (memcmp(&mc_CanWriteMessage.au8_Data[0], &t_RXMsg.au8_Data[0], 7) == 0)
         {
            s32_Return = C_NO_ERR; //correct response message
         }
         else if ((t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID) && (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR))
         {
            mu8_LastError = t_RXMsg.au8_Data[2];
            s32_Return = C_NOACT;
         }
         else
         {
            //not what we expect ...
         }
         if (s32_Return != C_COM)
         {
            break;
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < TIMEOUT_FLASH_MS); //"TIMEOUT_FLASH" used for compatibility reasons
                                                                    // with previous client implementations
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }

   //1st response was OK, now data will follow
   for (u8_Loop = 0U; u8_Loop < static_cast<uint8_t>((ou8_NumBytes / 7U) + 1U); u8_Loop++)
   {
      u32_StartTime = TglGetTickCount();
      s32_Return = C_COM;
      do
      {
         TRG_HandleSystemMessages();
         if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
         {
            if (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID)
            {
               //data message -> copy to target
               u8_CopyNow =
                  static_cast<uint8_t>(((ou8_NumBytes - u16_Counter) > 7) ? 7U : (ou8_NumBytes - u16_Counter));
               if (t_RXMsg.u8_DLC < static_cast<uint8_t>(1U + u8_CopyNow))
               {
                  s32_Return = C_NOACT;
               }
               else
               {
                  (void)memcpy(&opu8_Data[u16_Counter], &t_RXMsg.au8_Data[1], u8_CopyNow);

                  u16_Counter = static_cast<uint16_t>(u16_Counter + 7U);
                  if (u16_Counter >= ou8_NumBytes)
                  {
                     u8_ChecksumReceived = t_RXMsg.au8_Data[(ou8_NumBytes % 7U) + 1U];
                  }
                  s32_Return = C_NO_ERR;
               }
               break;
            }
         }
      }
      while ((TglGetTickCount() - u32_StartTime) < TIMEOUT_FLASH_MS); //"TIMEOUT_FLASH" used for compatibility reasons
                                                                       // with previous client implementations
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
   }

   //evaluate checksum
   u8_ChecksumCalc = 0U;
   for (u8_Loop = 0U; u8_Loop < ou8_NumBytes; u8_Loop++)
   {
      u8_ChecksumCalc += opu8_Data[u8_Loop];
   }
   if (static_cast<uint8_t>(u8_ChecksumReceived + u8_ChecksumCalc) != 0U)
   {
      return C_CHECKSUM;
   }

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_version_number.

   get_version_number client implementation.
   Reads flashloader implementation version number.
   This is not the same as the flashloader protocol version !

   Sequence: \n
   - retrieve the bootloader version of node LokaleID   \n
   - 2 formats are supported as specified in the flashloader specification: \n
      1. response with DLC 4 -> 1 uint8 version information    \n
      2. response with DLC 8 -> 5 uint8 version information    \n
   - The size (1/5) is returned to the caller to determine how to handle the server node

   \param[out]    orau8_Version       read version number
   \param[out]    oru8_VersionSize    the number of bytes used in orau8_Version

   \return
   C_NO_ERR           no error                                        \n
   C_COM              no response                                     \n
   C_RANGE            wrong DLC in response (must be either 4 or 8)   \n
   C_NOACT            error response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetVersionNumber(uint8_t (&orau8_Version)[5], uint8_t & oru8_VersionSize)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_VERSION_NUMBER;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg);
   if (s32_Return == C_NO_ERR)
   {
      if ((t_RXMsg.u8_DLC == 4U) || (t_RXMsg.u8_DLC == 8U))
      {
         oru8_VersionSize = static_cast<uint8_t>(t_RXMsg.u8_DLC - 3U);
         (void)memcpy(&orau8_Version[0], &t_RXMsg.au8_Data[3], oru8_VersionSize);
      }
      else
      {
         s32_Return = C_RANGE;
      }
   }
   return s32_Return;
}

//************************************************************************
// .FUNCTION    SendSingleMessage
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  - send out one single CAN message (e.g. for Reset-Message to controller)
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS  orc_Msg   message to send
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32    0    -> no errors
//                       -1    -> error sending
//************************************************************************
int32_t C_XFLProtocol::SendSingleMessage(const T_STWCAN_Msg_TX & orc_Msg)
{
   return m_CANSendMessage(orc_Msg);
}

//************************************************************************
// .FUNCTION    m_SendCompanyIDExt
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  - send out the company ID to target ou8_LocalID
//  - responses from more than one targets are possible
//  - can handle 2 byte and 5 byte company IDs
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS  ou8_LocalID     ID of the target(s) to send the company ID to
//              orc_CompIDIn    Company ID to send
//              oru8_NumFound   Returns the number of responses (i.e. number of controllers)
//              opc_CompIDOut   Returns reported CompID; (for "Y*" Flashing); pass NULL if not interested
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_RANGE    -> invalid parameters
// C_NO_ERR   -> no errors
// C_COM      -> no response
//************************************************************************
int32_t C_XFLProtocol::m_SendCompanyIDExt(const uint8_t ou8_LocalID, const C_XFLCompanyID & orc_CompIDIn,
                                          uint8_t & oru8_NumFound, C_XFLCompanyID * const opc_CompIDOut)
{
   uint32_t u32_StartTime;
   int32_t s32_Return = C_NO_ERR;
   int32_t s32_Diff;
   T_STWCAN_Msg_RX t_RXMsg;

   uint8_t u8_Found = 0U;

   if ((orc_CompIDIn.u8_NumBytes != 2U) && (orc_CompIDIn.u8_NumBytes != 5U))
   {
      return C_RANGE;
   }

   mc_CanWriteMessage.u32_ID = mc_Config.u32_SendID;
   mc_CanWriteMessage.u8_DLC = static_cast<uint8_t>(3U + orc_CompIDIn.u8_NumBytes);
   mc_CanWriteMessage.au8_Data[0] = ou8_LocalID;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_NODE_COMPID;
   mc_CanWriteMessage.au8_Data[3] = orc_CompIDIn.au8_Data[0];
   mc_CanWriteMessage.au8_Data[4] = orc_CompIDIn.au8_Data[1];
   mc_CanWriteMessage.au8_Data[5] = orc_CompIDIn.au8_Data[2];
   mc_CanWriteMessage.au8_Data[6] = orc_CompIDIn.au8_Data[3];
   mc_CanWriteMessage.au8_Data[7] = orc_CompIDIn.au8_Data[4];

   m_CANSendMessage(); // send compid - request

   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if ((t_RXMsg.au8_Data[0] == ou8_LocalID) &&
             (t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_FLASH_COMMAND) &&
             (t_RXMsg.au8_Data[2] == mu8_XFL_CMD_NODE_COMPID) &&
             ((t_RXMsg.u8_DLC == 5U) || (t_RXMsg.u8_DLC == 8U)))
         {
            u8_Found++; //mark as found even if the returned company id is not the requested one
                        //an OK response means the target has accepted the company-ID

            s32_Diff = memcmp(&mc_CanWriteMessage.au8_Data[3], &t_RXMsg.au8_Data[3], orc_CompIDIn.u8_NumBytes);
            //is it the exact company-ID we sent ?
            if ((t_RXMsg.u8_DLC != mc_CanWriteMessage.u8_DLC) || (s32_Diff != 0))
            {
               if (s32_Return == C_NO_ERR) //only report the first differing response
               {
                  if (opc_CompIDOut != NULL)
                  {
                     opc_CompIDOut->u8_NumBytes = static_cast<uint8_t>(t_RXMsg.u8_DLC - 3U);
                     (void)memcpy(&opc_CompIDOut->au8_Data[0], &t_RXMsg.au8_Data[3], opc_CompIDOut->u8_NumBytes);
                  }
               }
               s32_Return = C_WARN; //at least one server responded with different CID !
            }
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_CHANGE_STATE_MS);

   oru8_NumFound = u8_Found;
   if ((u8_Found > 0U) && (s32_Return == C_NO_ERR))
   {
      return C_NO_ERR;
   }
   return C_WARN;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   perform a wakeup with local ID

   Sequence:
   - perform wakeup_lid
   - perform send_compid

   Responses from more than one controller are possible

   \param[in]      orc_CompanyID     server's company ID
   \param[out]     opu8_NumFound     Number of responses (i.e. number of controllers)
   \param[out]     opc_CompanyIDOut  Returns reported CompID (reports company ID of first node that differs
                                      from orc_CompanyID); pass NULL if not interested
   \return
   C_NO_ERR           no errors                                                                   \n
   C_WARN             at least one server responded with company ID different from ou8_CompanyID  \n
   C_COM              no response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::WakeupLocalId(const C_XFLCompanyID & orc_CompanyID, uint8_t * const opu8_NumFound,
                                     C_XFLCompanyID * const opc_CompanyIDOut)
{
   uint32_t u32_OldTime;
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;
   uint8_t u8_NumFoundWakeup = 0U;
   uint8_t u8_NumFoundCompID = 0U;

   mc_CanWriteMessage.u8_DLC = 2U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_NODE_WAKEUP;

   m_SendMessageWithIDs(); // send wakeup - request

   u32_OldTime = TglGetTickCount();
   for (;;)
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if ((t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID) && (t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_NODE_WAKEUP))
         {
            u8_NumFoundWakeup++;
         }
      }
      if ((TglGetTickCount() - u32_OldTime) > mu16_TIMEOUT_CHANGE_STATE_MS)
      {
         break;
      }
   }
   if (u8_NumFoundWakeup == 0U)
   {
      return C_COM;
   }

   s32_Return = m_SendCompanyIDExt(mc_Config.u8_LocalID, orc_CompanyID, u8_NumFoundCompID, opc_CompanyIDOut);
   if (((s32_Return == C_NO_ERR) || (s32_Return == C_WARN)) && (opu8_NumFound != NULL))
   {
      //whatever reported more:
      (*opu8_NumFound) = (u8_NumFoundWakeup > u8_NumFoundCompID) ? u8_NumFoundWakeup : u8_NumFoundCompID;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scan for nodes in the network.

   Scan for existing flashloader server nodes in the network.
   Responses from more than one controller on one ID are possible.

   \param[out]     orau8_LocalIds     Number of responses for each ID.
                                       e.g. orau8_LocalIds[1] contains the number of responding
                                       controllers on Local ID 1 after call.
   \param[out]     opu8_NumIDs        Total number of found nodes
   \param[in]      ou32_StartTime     Period to send the message for in milli-seconds
   \param[in]      ou8_FLASHInterval  Interval between individual messages in milli-seconds

   \return
   C_NO_ERR  no errors   \n
   -1        overflow in send queue while sending "FLASH"
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SearchId(uint8_t (&orau8_LocalIds)[XFL_NUM_DIFFERENT_LOCAL_IDS], uint8_t * const opu8_NumIDs,
                                const uint32_t ou32_StartTime, const uint8_t ou8_FLASHInterval)
{
   uint8_t u8_LID;
   int32_t s32_Return;
   uint8_t u8_NumIDs = 0U;
   uint32_t u32_OldTime;
   T_STWCAN_Msg_RX t_RXMsg;

   (void)memset(&orau8_LocalIds[0], 0, sizeof(orau8_LocalIds));

   s32_Return = SendFLASH(ou32_StartTime, ou8_FLASHInterval);
   if (s32_Return != 0)
   {
      return s32_Return;
   }

   mc_CanWriteMessage.u8_DLC = 2U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_NODE_WAKEUP;

   for (u8_LID = 0U; u8_LID < (XFL_NUM_DIFFERENT_LOCAL_IDS - 1U); u8_LID++) //-1: ID 255 is reserved !
   {
      mc_CanWriteMessage.au8_Data[0] = u8_LID;
      s32_Return = m_CANSendMessage(); // send wakeup request
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      if (mc_Config.u32_Bitrate != 0U)
      {
         TRG_WaitMicroSeconds(static_cast<uint32_t>(250000U) / mc_Config.u32_Bitrate); //use ca 50% busload ...
      }
      else
      {
         TRG_WaitMicroSeconds(2000U); //assume 125 kBit/s -> ca. 50% busload
      }
      TRG_HandleSystemMessages();
   }

   u32_OldTime = TglGetTickCount();
   for (;;)
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if (t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_NODE_WAKEUP)
         {
            orau8_LocalIds[t_RXMsg.au8_Data[0]]++;
            u8_NumIDs++;
         }
      }
      if ((TglGetTickCount() - u32_OldTime) > mu16_TIMEOUT_CHANGE_STATE_MS)
      {
         break;
      }
   }
   if (opu8_NumIDs != NULL)
   {
      (*opu8_NumIDs) = u8_NumIDs;
   }
   return C_NO_ERR;
}

//************************************************************************
// .FUNCTION    GetSNRExt
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  - read the SNR(s) from the controller(s) specified by LokaleId
//  - answers from more than one controller on one ID are possible
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS
// INPUT:
//    ou8_NumMax     Maximum number of answers to record to prevent overflow in pbSNR
// OUTPUT:
//    *opu8_SNR         Returns the received SNR(s), each 6 bytes
//                       the calling function has to provide a buffer
//                       large enough
//                      Set to NULL if only interested in the number of responses.
//    oru8_NumFound     number of responses ...
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR       -> no errors
// C_COM          -> no response
// XFL_ERR_GENERAL_ERR    -> error response
//************************************************************************
int32_t C_XFLProtocol::GetSNRExt(uint8_t * const opu8_SNR, const uint8_t ou8_NumMax, uint8_t & oru8_NumFound)
{
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;
   uint8_t u8_Found = 0U;

   mc_CanWriteMessage.u8_DLC = 2U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_SNR;
   m_SendMessageWithIDs();

   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if ((t_RXMsg.u8_DLC == 8U) &&
             (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID) &&
             (t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_GET_SNR))
         {
            if (opu8_SNR != NULL)
            {
               (void)memcpy(&opu8_SNR[u8_Found * 6U], &t_RXMsg.au8_Data[2], 6U);
            }
            u8_Found++;
            if (u8_Found >= ou8_NumMax)
            {
               break;
            }
         }
         if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
         {
            mu8_LastError = t_RXMsg.au8_Data[2];
            return C_NOACT;
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_READ_INFO_MS);

   oru8_NumFound = u8_Found;
   if (u8_Found > 0U)
   {
      return C_NO_ERR;
   }
   return C_COM;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_sec_crc service.

   get_sec_crc client implementation.

   \param[in]      ou16_Sector    sector index
   \param[out]     oru16_CRC      freshly calculated checksum returned by the server
   \param[out]     oru16_EECRC    checksum stored on the server

   \return
   C_NO_ERR   no errors        \n
   C_COM      no response      \n
   C_NOACT    error response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetSecCRC(const uint16_t ou16_Sector, uint16_t & oru16_CRC, uint16_t & oru16_EECRC)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_SEC_CRC;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou16_Sector);
   if (ou16_Sector > 255U)
   {
      mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou16_Sector >> 8U);
      mc_CanWriteMessage.u8_DLC = 5U;
   }
   m_SendMessageWithIDs();

   //might take some time to create checksum on server
   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CALC_CHECKSUM_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg, 7);
   if (s32_Return == C_NO_ERR)
   {
      oru16_CRC   = mh_AU8ToU16LE(&t_RXMsg.au8_Data[3]);
      oru16_EECRC = mh_AU8ToU16LE(&t_RXMsg.au8_Data[5]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_sec_crc service.

   get_sec_crc client implementation.

   \param[in]      ou16_Sector    sector index
   \param[out]     oru16_CRC      calculated checksum returned by the server

   \return
   C_NO_ERR   no errors        \n
   C_COM      no response      \n
   C_NOACT    error response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetSecCRC(const uint16_t ou16_Sector, uint16_t & oru16_CRC)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_SEC_CRC;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou16_Sector);
   if (ou16_Sector > 255U)
   {
      mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou16_Sector >> 8U);
      mc_CanWriteMessage.u8_DLC = 5U;
   }

   m_SendMessageWithIDs();

   //might take some time to create checksum on controller
   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CALC_CHECKSUM_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg, 5);
   if (s32_Return == C_NO_ERR)
   {
      oru16_CRC = mh_AU8ToU16LE(&t_RXMsg.au8_Data[3]);
   }
   return s32_Return;
}

//////////////////////////////////////////////////////////////////////////////////

//r-value
// C_COM   -> no valid response
// C_NOACT        -> error response
// C_NO_ERR       -> no errors
int32_t C_XFLProtocol::GetModeCompare(const uint16_t ou16_Sector, uint8_t & oru8_ModeCompare)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_SEC_MODE_COMPARE;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou16_Sector);
   if (ou16_Sector > 255U)
   {
      mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou16_Sector >> 8U);
      mc_CanWriteMessage.u8_DLC = 5U;
   }
   m_SendMessageWithIDs();

   //might take some time to create checksum on controller
   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data,
                                  static_cast<uint8_t>(mc_CanWriteMessage.u8_DLC - 1U), &t_RXMsg,
                                  static_cast<int16_t>(mc_CanWriteMessage.u8_DLC) + 1U);
   if (s32_Return == C_NO_ERR)
   {
      if (ou16_Sector > 255U)
      {
         oru8_ModeCompare = t_RXMsg.au8_Data[5];
      }
      else
      {
         oru8_ModeCompare = t_RXMsg.au8_Data[4];
      }
   }
   return s32_Return;
}

//************************************************************************

//r-value
// C_COM          -> no valid response
// C_NOACT        -> error response
// C_NO_ERR       -> no errors
int32_t C_XFLProtocol::SetModeCompare(const uint16_t ou16_Sector, const uint8_t ou8_ModeCompare)
{
   //on ESX2 this function call will always result in a write access to the
   // same EEPROM address !!
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_SEC_MODE_COMPARE;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou16_Sector);
   if (ou16_Sector <= 255U)
   {
      mc_CanWriteMessage.au8_Data[4] = ou8_ModeCompare;
      mc_CanWriteMessage.u8_DLC = 5U;
   }
   else
   {
      mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou16_Sector >> 8U);
      mc_CanWriteMessage.au8_Data[5] = ou8_ModeCompare;
      mc_CanWriteMessage.u8_DLC = 6U;
   }

   m_SendMessageWithIDs();
   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data,
                            static_cast<uint8_t>(mc_CanWriteMessage.u8_DLC - 1U), NULL, mc_CanWriteMessage.u8_DLC);
}

//////////////////////////////////////////////////////////////////////////////////

//r-value C_COM -> no valid response
//  XFL_ERR_GENERAL_ERR -> error response
int32_t C_XFLProtocol::GetEraseCount(uint32_t & oru32_EraseCount)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_ERASE_COUNT;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg, 6);
   if (s32_Return == C_NO_ERR)
   {
      oru32_EraseCount = t_RXMsg.au8_Data[3] +
                         ((static_cast<uint32_t>(t_RXMsg.au8_Data[4])) << 8U) +
                         ((static_cast<uint32_t>(t_RXMsg.au8_Data[5])) << 16U);
   }
   return s32_Return;
}

//************************************************************************
// .FUNCTION    sint32 SetDivertParameter
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  Implementation of protocol service "set_gateway_parameter".
//  Set parameter for divert stream. Parameters are ECU-specific and must
//   be configured before activating divert stream.
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS
//  ou8_Device             target device (0 = CAN/ 1 = RS232)
//  ou8_Position           target slot (e.g. 0 = CAN1/ 1 = CAN2)
//  ou8_ParameterNumber    index of parameter
//  orau8_Data             data to set for parameter
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE
// C_NO_ERR       -> no errors
// C_COM          -> no response
// C_RANGE        -> invalid bType
// C_NOACT        -> error response
//************************************************************************
int32_t C_XFLProtocol::SetDivertParameter(const uint8_t ou8_Device, const uint8_t ou8_Position,
                                          const uint8_t ou8_ParameterNumber, const uint8_t (&orau8_Data)[2])
{
   mc_CanWriteMessage.u8_DLC = 8U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_DIVERT_PARAM;
   mc_CanWriteMessage.au8_Data[3] = ou8_Device;
   mc_CanWriteMessage.au8_Data[4] = ou8_Position;
   mc_CanWriteMessage.au8_Data[5] = ou8_ParameterNumber;
   mc_CanWriteMessage.au8_Data[6] = orau8_Data[0];
   mc_CanWriteMessage.au8_Data[7] = orau8_Data[1];
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 5U, NULL,
                            8);
}

//************************************************************************
// .FUNCTION    SetTimeoutFactor
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  ESX2 specific function:
//  Set timeout factor for erase or write operations on AK4 SMM module flash
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS  ou8_Type       0 -> set erase timeout factor
//                             1 -> set write timeout factor
//              ou8_Factor     value to set (cf. xfloader.doc)
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR     -> no errors
// C_COM        -> no response
// C_RANGE      -> invalid bType
// C_NOACT      -> error response
//************************************************************************
int32_t C_XFLProtocol::SetTimeoutFactor(const uint8_t ou8_Type, const uint8_t ou8_Factor)
{
   if (ou8_Type > 1U)
   {
      return C_RANGE;
   }

   mc_CanWriteMessage.u8_DLC = 5U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_TIMEOUT_FACTOR;
   mc_CanWriteMessage.au8_Data[3] = ou8_Type;
   mc_CanWriteMessage.au8_Data[4] = ou8_Factor;
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 4U, NULL);
}

//************************************************************************
// .FUNCTION    GetTimeoutFactor
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  ESX2 specific function:
//  Get timeout factor for erase or write operations on AK4 SMM module flash.
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS  ou8_Type        0 -> get erase timeout factor
//                              1 -> get write timeout factor
//              oru8_Factor     timeout value in controller (cf. flashloader-spec)
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR         -> no errors
// C_COM            -> no response
// C_RANGE          -> invalid bType
// C_NOACT          -> error response
//************************************************************************
int32_t C_XFLProtocol::GetTimeoutFactor(const uint8_t ou8_Type, uint8_t & oru8_Factor)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   if (ou8_Type > 1U)
   {
      return C_RANGE;
   }

   mc_CanWriteMessage.u8_DLC  = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_TIMEOUT_FACTOR;
   mc_CanWriteMessage.au8_Data[3] = ou8_Type;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg);
   if (s32_Return == C_NO_ERR)
   {
      oru8_Factor = t_RXMsg.au8_Data[4];
   }
   return s32_Return;
}

// bbWakeup =====================================================================
//  ESX2 specific function
int32_t C_XFLProtocol::BBWakeup(const uint8_t ou8_Position, const uint8_t (&orau8_LastUserSignature)[2])
{
   mc_CanWriteMessage.u8_DLC = 6U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_BB_WAKEUP;
   mc_CanWriteMessage.au8_Data[3] = ou8_Position;
   mc_CanWriteMessage.au8_Data[4] = orau8_LastUserSignature[0];
   mc_CanWriteMessage.au8_Data[5] = orau8_LastUserSignature[1];
   m_SendMessageWithIDs(); // send bb_wakeup - request

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 5U, NULL,
                            6);
}

// DivertStream =====================================================================
// Compatibility version for flashloader protocol < V3.02r0
int32_t C_XFLProtocol::DivertStream(const uint8_t ou8_Device, const uint8_t ou8_Position, const uint8_t ou8_State)
{
   mc_CanWriteMessage.u8_DLC = 6U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_DIVERT_STREAM;
   mc_CanWriteMessage.au8_Data[3] = ou8_Device;
   mc_CanWriteMessage.au8_Data[4] = ou8_Position;
   mc_CanWriteMessage.au8_Data[5] = ou8_State;
   m_SendMessageWithIDs(); // send divert_stream - request

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 5U, NULL,
                            6);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activate or deactivate divert stream mechanism

   Also passes the "handle" parameter in the request.
   Requires STW flashloader protocol >= V3.02r0.
   For previous versions use the DivertStream() function without the "handle" parameter.
   For details about the "handle" cf. the STW flashloader protocol specification.

   \param[in]  ou8_Device      target bus type (e.g. CAN, RS232)
   \param[in]  ou8_Position    target bus channel number (e.g. 0 -> CAN_1)
   \param[in]  ou8_State       0 -> turn diverting off; 1 -> turn diverting on
   \param[in]  ou8_HopHandle   handle to identify the connection "hop"

   \return
   C_NO_ERR     no problems     \n
   C_NOACT      error response  \n
   C_COM        no response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::DivertStream(const uint8_t ou8_Device, const uint8_t ou8_Position, const uint8_t ou8_State,
                                    const uint8_t ou8_HopHandle)
{
   mc_CanWriteMessage.u8_DLC = 7U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_FLASH_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_DIVERT_STREAM;
   mc_CanWriteMessage.au8_Data[3] = ou8_Device;
   mc_CanWriteMessage.au8_Data[4] = ou8_Position;
   mc_CanWriteMessage.au8_Data[5] = ou8_State;
   mc_CanWriteMessage.au8_Data[6] = ou8_HopHandle;
   m_SendMessageWithIDs(); // send divert_stream - request

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 6U, NULL,
                            7);
}

//************************************************************************
// .FUNCTION    GetLastUser
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  ESX2 specific function:
//  Get signature of last flasher (only implemented on ESX-Babyboard B)
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS  orau8_LastUser  signature of last flasher
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE sint32
// C_NO_ERR            -> no errors
// C_COM               -> no response
//************************************************************************
int32_t C_XFLProtocol::GetLastUser(uint8_t (&orau8_LastUser)[2])
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC  = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_LAST_USER;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg);
   if (s32_Return == C_NO_ERR)
   {
      orau8_LastUser[0] = t_RXMsg.au8_Data[3];
      orau8_LastUser[1] = t_RXMsg.au8_Data[4];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_device ID service.

   reads one block of the device ID from the server

   \param[in]      ou8_BlockIndex     block number
   \param[out]     oracn_Data         read part of the device ID

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server                 \n
   C_WARN             OK -> this was the last frame (DLC < 8)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::m_GetDeviceIDBlock(const uint8_t ou8_BlockIndex, char_t (&oracn_Data)[4])
{
   uint32_t u32_StartTime;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_DEVICE_ID;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockIndex;

   m_SendMessageWithIDs();

   (void)memset(oracn_Data, 0, sizeof(oracn_Data));
   u32_StartTime = TglGetTickCount();
   do
   {
      TRG_HandleSystemMessages();
      if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
      {
         if (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID)
         {
            if ((t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_GET_COMMAND) &&
                (t_RXMsg.au8_Data[2] == mu8_XFL_CMD_GET_DEVICE_ID) &&
                (t_RXMsg.au8_Data[3] == ou8_BlockIndex) &&
                (t_RXMsg.u8_DLC >= 4U))
            {
               (void)memcpy(&oracn_Data[0], &t_RXMsg.au8_Data[4], static_cast<uint32_t>(t_RXMsg.u8_DLC) - 4U);
               if (t_RXMsg.u8_DLC < 8U)
               {
                  return C_WARN;
               }
               return C_NO_ERR;
            }
            if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
            {
               mu8_LastError = t_RXMsg.au8_Data[2];
               return C_NOACT;
            }
         }
      }
   }
   while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_READ_INFO_MS);
   return C_COM;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read device-ID from server node

   \param[in]      oq_LongID      true  -> use indexed protocol (up to 16 characters, requires protocol > V3.0 on server
                                  false -> use simple protocol (up to 5 characters, < protocol version V3.0)
   \param[out]     orc_DeviceId   Device ID string (max. 16 characters)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetDeviceID(const bool oq_LongID, C_SclString & orc_DeviceId)
{
   uint32_t u32_StartTime;
   int32_t s32_Return;
   uint8_t u8_Index;
   char_t acn_Buffer[4];
   char_t acn_DeviceID[17];
   T_STWCAN_Msg_RX t_RXMsg;

   orc_DeviceId = "";

   (void)memset(acn_DeviceID, 0, sizeof(acn_DeviceID));
   if (oq_LongID == true)
   {
      for (u8_Index = 0U; u8_Index < 4U; u8_Index++)
      {
         s32_Return = m_GetDeviceIDBlock(u8_Index, acn_Buffer);
         if ((s32_Return == C_NO_ERR) || (s32_Return == C_WARN))
         {
            (void)memcpy(&acn_DeviceID[u8_Index * 4], &acn_Buffer[0], 4);
         }
         if (s32_Return != C_NO_ERR)
         {
            if (s32_Return == C_WARN)
            {
               //last frame read -> OK
               s32_Return = C_NO_ERR;
            }
            break;
         }
      }
   }
   else
   {
      mc_CanWriteMessage.u8_DLC  = 3U;
      mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
      mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_DEVICE_ID;
      m_SendMessageWithIDs();

      s32_Return = C_COM;
      u32_StartTime = TglGetTickCount();
      do
      {
         TRG_HandleSystemMessages();
         if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
         {
            if (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID)
            {
               if ((t_RXMsg.au8_Data[1] == mu8_XFL_CMD_GRP_GET_COMMAND) &&
                   (t_RXMsg.au8_Data[2] == mu8_XFL_CMD_GET_DEVICE_ID) &&
                   (t_RXMsg.u8_DLC > 3U))
               {
                  (void)memcpy(&acn_DeviceID[0], &t_RXMsg.au8_Data[3], static_cast<uint32_t>(t_RXMsg.u8_DLC) - 3U);
                  s32_Return = C_NO_ERR;
               }
               if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
               {
                  mu8_LastError = t_RXMsg.au8_Data[2];
                  s32_Return = C_NOACT;
               }
            }
            if (s32_Return != C_COM)
            {
               break;
            }
         }
      }
      while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_READ_INFO_MS);
   }

   if (s32_Return == C_NO_ERR)
   {
      orc_DeviceId = acn_DeviceID;
   }
   return s32_Return;
}

//************************************************************************
// .FUNCTION    GetLastXFLError
// .GROUP
//----------------------------------------------------------------------------------------------------------------------
// .DESCRIPTION
//  Get last device specific error code
//----------------------------------------------------------------------------------------------------------------------
// .PARAMETERS
//----------------------------------------------------------------------------------------------------------------------
// .RETURNVALUE uint8   last errorcode received from ECU
//************************************************************************
uint8_t C_XFLProtocol::GetLastXFLError(void) const
{
   return mu8_LastError;
}

//************************************************************************

int32_t C_XFLProtocol::m_CANSendMessage(const T_STWCAN_Msg_TX & orc_MSG)
{
   tgl_assert(mc_Config.pc_CANDispatcher != NULL);
   return mc_Config.pc_CANDispatcher->CAN_Send_Msg(orc_MSG);
}

//************************************************************************

//Fill in send ID and Local ID and send out
int32_t C_XFLProtocol::m_SendMessageWithIDs(void)
{
   mc_CanWriteMessage.u32_ID      = mc_Config.u32_SendID;
   mc_CanWriteMessage.au8_Data[0] = mc_Config.u8_LocalID;
   return this->m_CANSendMessage();
}

//************************************************************************

int32_t C_XFLProtocol::m_CANSendMessage(void)
{
   tgl_assert(mc_Config.pc_CANDispatcher != NULL);
   if (mc_Config.pc_CANDispatcher == NULL)
   {
      return -1;
   }
   return mc_Config.pc_CANDispatcher->CAN_Send_Msg(mc_CanWriteMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Receive new CAN message

   Check for new incoming CAN message.

   If a message is received we must know if it is for us.
   So it will be checked against the configured RX ID and RX ID type.
   A configured RX ID of 0xFFFFFFFF has the special meaning "let pass all IDs".
    (useful for flashing via a point-2-point connection like RS232).

   \param[out]     orc_MSG        received CAN message

   \return
   C_NO_ERR    message received     \n
   else        no new message received
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::m_CANGetResponse(T_STWCAN_Msg_RX & orc_MSG)
{
   int32_t s32_Return = C_NO_ERR;

   tgl_assert(mc_Config.pc_CANDispatcher != NULL);
   if (mc_Config.pc_CANDispatcher == NULL)
   {
      return C_CONFIG;
   }

   //read until buffer is empty or we get a response
   while (s32_Return == C_NO_ERR)
   {
      s32_Return = mc_Config.pc_CANDispatcher->CAN_Read_Msg(mu16_CANDispatcherHandle, orc_MSG);
      if ((s32_Return == C_NO_ERR) &&
          ((mc_Config.u32_ReceiveID == 0xFFFFFFFFU) ||
           ((orc_MSG.u32_ID == mc_Config.u32_ReceiveID) &&
            (orc_MSG.u8_XTD == static_cast<uint8_t>(mc_Config.q_XtdID)))))
      {
         return C_NO_ERR;
      }
   }
   return C_WARN;
}

//************************************************************************

int32_t C_XFLProtocol::EEPROMRead(const uint8_t ou8_NumBytes, const uint16_t ou16_StartAddress,
                                  uint8_t * const opu8_Data)
{
   uint32_t u32_StartTime;
   int32_t s32_Return;
   uint8_t u8_NumMessages;
   uint8_t u8_BytesReceived;
   uint16_t u16_BytesLeft;
   uint8_t u8_ChecksumReceived = 0U;
   uint8_t u8_ChecksumCalc;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC  = 6U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_EE_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_EE_READ_CMD;
   mc_CanWriteMessage.au8_Data[3] = ou8_NumBytes;
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou16_StartAddress);
   mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou16_StartAddress >> 8U);
   m_SendMessageWithIDs();

   //wait for first message...
   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 5U,
                                  NULL, 6);
   if (s32_Return != C_NO_ERR)
   {
      return C_COM;
   }

   //ok -> data will follow

   //now the blocks of data will follow
   u8_NumMessages = static_cast<uint8_t>((ou8_NumBytes + 1U + 6U) / 7U); //7 bytes per message (+1 for checksum)
   u8_BytesReceived = 0U;
   u16_BytesLeft = static_cast<uint16_t>(static_cast<uint16_t>(ou8_NumBytes) + 1U); //+1 for checksum
   for (uint8_t u8_Message = 0U; u8_Message < u8_NumMessages; u8_Message++)
   {
      u32_StartTime = TglGetTickCount();
      do
      {
         TRG_HandleSystemMessages();
         if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
         {
            if (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID)
            {
               const uint8_t u8_Temp = static_cast<uint8_t>((u16_BytesLeft > 7U) ? 7U : u16_BytesLeft);
               if (t_RXMsg.u8_DLC == static_cast<uint8_t>(u8_Temp + 1U))
               {
                  //make sure to extract checksum directly to the correct uint8 as the
                  // buffer supplied to us might not be large enough !
                  for (uint8_t u8_Index = 0U; u8_Index < u8_Temp; u8_Index++)
                  {
                     if (u16_BytesLeft > 1U)
                     {
                        opu8_Data[u8_BytesReceived] = t_RXMsg.au8_Data[1 + u8_Index];
                     }
                     else
                     {
                        //it's the checksum !
                        u8_ChecksumReceived = t_RXMsg.au8_Data[1 + u8_Index];
                     }
                     u8_BytesReceived++;
                     u16_BytesLeft--;
                  }
                  break; //next message received OK
               }
               else //only way to detect a possible error
               {
                  if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
                  {
                     mu8_LastError = t_RXMsg.au8_Data[2];
                  }
                  return C_NOACT;
               }
            }
         }
      }
      while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_READ_INFO_MS);
   }
   if (u16_BytesLeft > 0U)
   {
      return C_NOACT;
   }

   //finally check checksum
   u8_ChecksumCalc = 0U;
   for (uint8_t u8_Byte = 0; u8_Byte < ou8_NumBytes; u8_Byte++)
   {
      u8_ChecksumCalc += opu8_Data[u8_Byte];
   }
   if (static_cast<uint8_t>(u8_ChecksumCalc + u8_ChecksumReceived) != 0U)
   {
      return C_CHECKSUM;
   }
   return C_NO_ERR;
}

//************************************************************************

int32_t C_XFLProtocol::EEPROMWrite(const uint8_t ou8_NumBytes, const uint16_t ou16_StartAddress,
                                   const uint8_t * const opu8_Data)
{
   uint32_t u32_StartTime;
   uint8_t u8_Message;
   uint8_t u8_Byte;
   bool q_OK;
   uint8_t u8_NumMessages;
   uint8_t u8_NumBytesNow;
   uint8_t u8_BytesLeft;
   uint16_t u16_Address;
   T_STWCAN_Msg_RX t_RXMsg;

   u8_NumMessages = static_cast<uint8_t>((ou8_NumBytes + 3U) / 4U);
   u8_BytesLeft = ou8_NumBytes;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_EE_WRITE_CMD;
   for (u8_Message = 0U; u8_Message < u8_NumMessages; u8_Message++)
   {
      u16_Address = (ou16_StartAddress + (static_cast<uint16_t>(u8_Message) * 4U));
      mc_CanWriteMessage.au8_Data[2] = static_cast<uint8_t>(u16_Address);
      mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(u16_Address >> 8U);

      mc_CanWriteMessage.u8_DLC = 4U + ((u8_BytesLeft > 4U) ? 4U : u8_BytesLeft);
      for (u8_Byte = 0U; u8_Byte < static_cast<uint8_t>((u8_BytesLeft > 4U) ? 4U : u8_BytesLeft); u8_Byte++)
      {
         mc_CanWriteMessage.au8_Data[4U + u8_Byte] = opu8_Data[u8_Byte + (u8_Message * 4U)];
      }
      m_SendMessageWithIDs();

      //wait for response
      //wait for first message...
      q_OK = false;
      u32_StartTime = TglGetTickCount();
      do
      {
         TRG_HandleSystemMessages();
         if (m_CANGetResponse(t_RXMsg) == C_NO_ERR)
         {
            //first message !!
            if (t_RXMsg.au8_Data[0] == mc_Config.u8_LocalID)
            {
               u8_NumBytesNow = 4U + ((u8_BytesLeft > 4U) ? 4U : u8_BytesLeft);

               if (memcmp(&mc_CanWriteMessage.au8_Data[0], &t_RXMsg.au8_Data[0], u8_NumBytesNow) == 0)
               {
                  //ok
                  q_OK = true;
                  break;
               }
               if (t_RXMsg.au8_Data[1] == mu8_COMMAND_ERR)
               {
                  mu8_LastError = t_RXMsg.au8_Data[2];
                  return C_NOACT;
               }
            }
         }
      }
      while ((TglGetTickCount() - u32_StartTime) < mu16_TIMEOUT_WRITE_CONFIG_MS);
      if (q_OK == false)
      {
         return C_COM;
      }
      u8_BytesLeft -= static_cast<uint8_t>(((u8_BytesLeft > 4U) ? 4U : u8_BytesLeft));
   }
   return C_NO_ERR;
}

//************************************************************************

void C_XFLProtocol::m_UpdateDispatcherRXFilter(void)
{
   C_CanRxFilter c_Filter;

   if (mc_Config.pc_CANDispatcher != NULL)
   {
      if (mc_Config.u32_ReceiveID == 0xFFFFFFFFUL)
      {
         c_Filter.PassAll();
      }
      else
      {
         c_Filter.PassOneID(mc_Config.u32_ReceiveID, mc_Config.q_XtdID, false);
      }
      mc_Config.pc_CANDispatcher->SetRXFilter(mu16_CANDispatcherHandle, c_Filter);
   }
}

//************************************************************************

C_XFLProtocol::C_XFLProtocol(const C_XFLProtocolConfig & orc_Config) :
   C_DLReportEvents(),
   mu16_CANDispatcherHandle(0U),
   mu8_LastError(0x00U)
{
   mc_Config = orc_Config;

   mc_CanWriteMessage.u8_XTD = (mc_Config.q_XtdID == true) ? 1U : 0U;
   mc_CanWriteMessage.u8_RTR = 0U;

   if (mc_Config.pc_CANDispatcher != NULL)
   {
      mc_Config.pc_CANDispatcher->RegisterClient(mu16_CANDispatcherHandle);
      m_UpdateDispatcherRXFilter();
   }
}

//************************************************************************

C_XFLProtocol::C_XFLProtocol(void) :
   C_DLReportEvents(),
   mu16_CANDispatcherHandle(0U),
   mu8_LastError(0x00U)
{
   mc_CanWriteMessage.u8_XTD = 0U;
   mc_CanWriteMessage.u8_RTR = 0U;
}

//************************************************************************

C_XFLProtocol::~C_XFLProtocol(void)
{
   if (mc_Config.pc_CANDispatcher != NULL)
   {
      //remove existing registration:
      (void)mc_Config.pc_CANDispatcher->RemoveClient(mu16_CANDispatcherHandle);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information service.

   "low-level" get_flash_information client implementation

   \param[in]     ou8_Index           index of information to read
   \param[in]     ou8_NumParamsToUse  number of parameters to send in request
   \param[in]     ou8_Parameter1      parameter 1
   \param[in]     ou8_Parameter2      parameter 2
   \param[out]    orau8_Result        response data

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformation(const uint8_t ou8_Index, const uint8_t ou8_NumParamsToUse,
                                           const uint8_t ou8_Parameter1, const uint8_t ou8_Parameter2,
                                           uint8_t (&orau8_Result)[4])
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = static_cast<uint8_t>(4U + ou8_NumParamsToUse);
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_FLASH_INFORMATION;
   mc_CanWriteMessage.au8_Data[3] = ou8_Index;
   mc_CanWriteMessage.au8_Data[4] = ou8_Parameter1;
   mc_CanWriteMessage.au8_Data[5] = ou8_Parameter2;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      orau8_Result[0] = t_RXMsg.au8_Data[4];
      orau8_Result[1] = t_RXMsg.au8_Data[5];
      orau8_Result[2] = t_RXMsg.au8_Data[6];
      orau8_Result[3] = t_RXMsg.au8_Data[7];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information number of ICs sub-service.

   get_flash_information get-number-of-ICs client implementation

   \param[out]    oru8_ICCount    number of flash ICs on server hardware

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationNumberOfICs(uint8_t & oru8_ICCount)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_IC_COUNT, 0U, 0U, 0U, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru8_ICCount = au8_Result[0];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information memory size sub-service.

   get_flash_information get-total-memory-size client implementation

   \param[in]     ou8_ICIndex     index of flash IC on server hardware
   \param[out]    oru32_Size      IC memory size in bytes

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationTotalMemorySize(const uint8_t ou8_ICIndex, uint32_t & oru32_Size)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_TOTAL_SIZE, 1U, ou8_ICIndex, 0U, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru32_Size = mh_AU8ToU32LE(&au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information protected sectors sub-service.

   get_flash_information get-protected-sectors client implementation

   \param[out]    orc_ProtectedSectors   array containing description of protected sectors

   \return
   C_NO_ERR           no errors                  \n
   C_COM              no response from server    \n
   C_NOACT            error response from server \n
   C_CONFIG           undefined response
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationProtectedSectors(
   C_SclDynamicArray<C_XFLProtectedSectorInfo> & orc_ProtectedSectors)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];
   uint8_t u8_BlockIndex = 0U;

   orc_ProtectedSectors.SetLength(0);
   bool q_Finished = false;

   while (q_Finished == false)
   {
      s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROTECTED_SECTORS, 1U, u8_BlockIndex, 0U,
                                       au8_Result);
      if (s32_Return == C_NO_ERR)
      {
         if (au8_Result[3] == 0x02U)
         {
            //not sector based !
            q_Finished = true;
         }
         else if (au8_Result[3] == 0x03U)
         {
            //no protected sectors at all !
            orc_ProtectedSectors.SetLength(0);
            q_Finished = true;
         }
         else
         {
            orc_ProtectedSectors.IncLength();
            orc_ProtectedSectors[orc_ProtectedSectors.GetHigh()].u8_ICIndex = au8_Result[2];
            orc_ProtectedSectors[orc_ProtectedSectors.GetHigh()].u16_SectorNumber = mh_AU8ToU16LE(&au8_Result[0]);
            switch (au8_Result[3])
            {
            case 0x00U: //last sector !
               q_Finished = true;
               break;
            case 0x01U: //more sectors
               u8_BlockIndex++;
               break;
            default:
               orc_ProtectedSectors.SetLength(0);
               s32_Return = C_CONFIG;
               q_Finished = true;
               break;
            }
         }
      }
      else
      {
         q_Finished = true;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information offset sector 0 sub-service.

   get_flash_information offset-sector-0 client implementation

   \param[in]     ou8_ICIndex     index of flash IC on server hardware
   \param[out]    oru32_Offset    address offset of sector 0 of flash IC in bytes

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationOffsetSector0(const uint8_t ou8_ICIndex, uint32_t & oru32_Offset)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_OFFSET_ADDRESS, 1U, ou8_ICIndex, 0U,
                                    au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru32_Offset = mh_AU8ToU32LE(&au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information number of regions sub-service.

   get_flash_information number-of-regions client implementation

   \param[in]     ou8_ICIndex        index of flash IC on server hardware
   \param[out]    oru8_RegionCount   number of memory regions

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationNumberRegions(const uint8_t ou8_ICIndex, uint8_t & oru8_RegionCount)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_REGIONS, 1U, ou8_ICIndex, 0U, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru8_RegionCount = au8_Result[0];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information region-information sub-service.

   get_flash_information region-information client implementation

   \param[in]     ou8_ICIndex        index of flash IC on server hardware
   \param[in]     ou8_RegionIndex    index of flash region
   \param[out]    oru32_BlockSize    size of each block in the specified region in bytes
   \param[out]    oru16_BlockCount   number of blocks which have the same size

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationRegionInformation(const uint8_t ou8_ICIndex, const uint8_t ou8_RegionIndex,
                                                            uint32_t & oru32_BlockSize, uint16_t & oru16_BlockCount)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];
   uint16_t u16_Help;

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_REGION_INFO, 2U, ou8_ICIndex, ou8_RegionIndex,
                                    au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      u16_Help = mh_AU8ToU16LE(&au8_Result[0]);
      if (u16_Help == 0U)
      {
         oru32_BlockSize = 0U;
      }
      else
      {
         oru32_BlockSize = (static_cast<uint32_t>(u16_Help)) * 256U;
      }
      oru16_BlockCount = mh_AU8ToU16LE(&au8_Result[2]) + 1U;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information erase-time sub-service.

   get_flash_information erase-time client implementation

   \param[in]     ou8_ICIndex        index of flash IC on server hardware
   \param[out]    oru32_EraseTime    time it takes to erase the slowest block in ms

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationEraseTime(const uint8_t ou8_ICIndex, uint32_t & oru32_EraseTime)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ERASE_TIME, 1U, ou8_ICIndex, 0U, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru32_EraseTime = mh_AU8ToU32LE(&au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information erase-time sub-service.

   get_flash_information erase-time client implementation

   \param[in]     ou8_ICIndex        index of flash IC on server hardware
   \param[out]    oru32_WriteTime    time it takes to write up to 255 bytes in ms

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationWriteTime(const uint8_t ou8_ICIndex, uint32_t & oru32_WriteTime)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_PROGRAM_TIME, 1U, ou8_ICIndex, 0U,
                                    au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru32_WriteTime = mh_AU8ToU32LE(&au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_flash_information aliasing sub-services

   Read information about aliased memory using the corresponding protocol services.

   \param[out]    orc_Aliases       Aliasing information returned from server

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFlashInformationAliases(C_XFLAliasedRanges & orc_Aliases)
{
   int32_t s32_Return;
   uint8_t u8_Region;
   uint8_t au8_Result[4];

   s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_NUM_ALIASES, 0U, 0U, 0U, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      orc_Aliases.SetLength(au8_Result[0]);

      for (u8_Region = 0U; u8_Region < orc_Aliases.GetLength(); u8_Region++)
      {
         s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_PHYSICAL, 1U, u8_Region, 0U,
                                          au8_Result);
         if (s32_Return == C_NO_ERR)
         {
            orc_Aliases[u8_Region].u32_PhysicalAddress = mh_AU8ToU32LE(&au8_Result[0]);

            s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_SIZE, 1U, u8_Region, 0U,
                                             au8_Result);
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_Aliases[u8_Region].u32_Size = mh_AU8ToU32LE(&au8_Result[0]);

            s32_Return = GetFlashInformation(mu8_XFL_CMD_GET_FLASH_INFORMATION_IDX_ALIAS_ADDRESS, 1U, u8_Region, 0U,
                                             au8_Result);
         }
         if (s32_Return == C_NO_ERR)
         {
            orc_Aliases[u8_Region].u32_AliasedAddress = mh_AU8ToU32LE(&au8_Result[0]);
         }
         else
         {
            break;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_implementation_information service.

   "low-level" get_implementation_information client implementation

   \param[in]     ou8_Index       index of information to read
   \param[out]    orau8_Result     response data

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetImplementationInformation(const uint8_t ou8_Index, uint8_t (&orau8_Result)[4])
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION;
   mc_CanWriteMessage.au8_Data[3] = ou8_Index;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      orau8_Result[0] = t_RXMsg.au8_Data[4];
      orau8_Result[1] = t_RXMsg.au8_Data[5];
      orau8_Result[2] = t_RXMsg.au8_Data[6];
      orau8_Result[3] = t_RXMsg.au8_Data[7];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_implementation_information protocol-version sub-service.

   get_implementation_information protocol-version client implementation

   \param[out]    oru16_Version     version number (0x1234 = V1.23r4)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetImplementationInformationProtocolVersion(uint16_t & oru16_Version)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_PROTOCOL_VERSION, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru16_Version = mh_AU8ToU16LE(&au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_implementation_information services sub-service.

   get_implementation_information services client implementation

   \param[out]    orc_ImplementedServices    structure containing boolean values flagging which services are implemented

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetImplementationInformationServices(C_XFLImplementedServices & orc_ImplementedServices)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SPECIAL_SERVICES, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      orc_ImplementedServices.q_EERead        = ((au8_Result[0] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_EEWrite       = ((au8_Result[0] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved1[0] = ((au8_Result[0] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved1[1] = ((au8_Result[0] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved1[2] = ((au8_Result[0] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved1[3] = ((au8_Result[0] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved1[4] = ((au8_Result[0] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved1[5] = ((au8_Result[0] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.q_NetStart      = ((au8_Result[1] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_NetReset      = ((au8_Result[1] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_NetSleep      = ((au8_Result[1] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved2[0] = ((au8_Result[1] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved2[1] = ((au8_Result[1] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved2[2] = ((au8_Result[1] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved2[3] = ((au8_Result[1] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved2[4] = ((au8_Result[1] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.q_GetSNR        = ((au8_Result[2] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_NodeWakeupLID = ((au8_Result[2] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_NodeWakeupSNR = ((au8_Result[2] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved3[0] = ((au8_Result[2] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved3[1] = ((au8_Result[2] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved3[2] = ((au8_Result[2] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved3[3] = ((au8_Result[2] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved3[4] = ((au8_Result[2] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_Reserved4[0] = ((au8_Result[3] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[1] = ((au8_Result[3] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[2] = ((au8_Result[3] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[3] = ((au8_Result[3] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[4] = ((au8_Result[3] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[5] = ((au8_Result[3] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[6] = ((au8_Result[3] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved4[7] = ((au8_Result[3] & 0x80U) > 0U) ? true : false;

      s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_GET_SERVICES, au8_Result);
   }
   if (s32_Return == C_NO_ERR)
   {
      orc_ImplementedServices.q_GetLocalID           = ((au8_Result[0] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetSectorCount       = ((au8_Result[0] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetVersionNumber     = ((au8_Result[0] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetDownloadCount     = ((au8_Result[0] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetDeviceID          = ((au8_Result[0] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetControlID         = ((au8_Result[0] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetSectorChecksum    = ((au8_Result[0] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetSectorModeCompare = ((au8_Result[0] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.q_GetTimeOutFactor             = ((au8_Result[1] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetLastUser                  = ((au8_Result[1] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetFlashInformation          = ((au8_Result[1] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetImplementationInformation = ((au8_Result[1] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetFingerPrint               = ((au8_Result[1] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetDeviceInfoAddress         = ((au8_Result[1] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetBlockStartAddress         = ((au8_Result[1] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetBlockEndAddress           = ((au8_Result[1] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.q_GetBlockChecksum = ((au8_Result[2] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_GetBlockCompareMode = ((au8_Result[2] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[0]    = ((au8_Result[2] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[1]    = ((au8_Result[2] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[2]    = ((au8_Result[2] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[3]    = ((au8_Result[2] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[4]    = ((au8_Result[2] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[5]    = ((au8_Result[2] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_Reserved6[6]    = ((au8_Result[3] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[7]    = ((au8_Result[3] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[8]    = ((au8_Result[3] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[9]    = ((au8_Result[3] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[10]   = ((au8_Result[3] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[11]   = ((au8_Result[3] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[12]   = ((au8_Result[3] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved6[13]   = ((au8_Result[3] & 0x80U) > 0U) ? true : false;

      s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_SET_SERVICES, au8_Result);
   }

   if (s32_Return == C_NO_ERR)
   {
      orc_ImplementedServices.q_SetLocalID           = ((au8_Result[0] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetCommBitrate       = ((au8_Result[0] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetCommID            = ((au8_Result[0] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetCommType          = ((au8_Result[0] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetControlID         = ((au8_Result[0] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetSectorChecksum    = ((au8_Result[0] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetSectorModeCompare = ((au8_Result[0] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetTimeoutFactor     = ((au8_Result[0] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.q_SetGatewayParameter  = ((au8_Result[1] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetXflExchange       = ((au8_Result[1] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetFingerPrint       = ((au8_Result[1] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetTempBitrate       = ((au8_Result[1] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetBlockStartAddress = ((au8_Result[1] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetBlockEndAddress   = ((au8_Result[1] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetBlockChecksum     = ((au8_Result[1] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.q_SetBlockCompareMode  = ((au8_Result[1] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_Reserved7[0]    = ((au8_Result[2] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[1]    = ((au8_Result[2] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[2]    = ((au8_Result[2] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[3]    = ((au8_Result[2] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[4]    = ((au8_Result[2] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[5]    = ((au8_Result[2] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[6]    = ((au8_Result[2] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[7]    = ((au8_Result[2] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_Reserved7[8]    = ((au8_Result[3] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[9]    = ((au8_Result[3] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[10]   = ((au8_Result[3] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[11]   = ((au8_Result[3] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[12]   = ((au8_Result[3] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[13]   = ((au8_Result[3] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[14]   = ((au8_Result[3] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved7[15]   = ((au8_Result[3] & 0x80U) > 0U) ? true : false;

      s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_FLASH_SERVICES, au8_Result);
   }

   if (s32_Return == C_NO_ERR)
   {
      orc_ImplementedServices.q_EraseSector      = ((au8_Result[0] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_ProgFlash        = ((au8_Result[0] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_NodeSleep        = ((au8_Result[0] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.q_NodeReset        = ((au8_Result[0] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.q_NodeReturn       = ((au8_Result[0] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.q_ReadFlash        = ((au8_Result[0] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.q_NodeCompID       = ((au8_Result[0] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.q_DivertStream     = ((au8_Result[0] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.q_WakeupDivertClient = ((au8_Result[1] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.q_Reserved9        = ((au8_Result[1] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.q_ReadSeed         = ((au8_Result[1] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.q_WriteKey         = ((au8_Result[1] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.q_ResetLocks       = ((au8_Result[1] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.q_EnterBSL         = ((au8_Result[1] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[0]    = ((au8_Result[1] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[1]    = ((au8_Result[1] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_Reserved8[2]    = ((au8_Result[2] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[3]    = ((au8_Result[2] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[4]    = ((au8_Result[2] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[5]    = ((au8_Result[2] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[6]    = ((au8_Result[2] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[7]    = ((au8_Result[2] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[8]    = ((au8_Result[2] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[9]    = ((au8_Result[2] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_Reserved8[10]   = ((au8_Result[3] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[11]   = ((au8_Result[3] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[12]   = ((au8_Result[3] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[13]   = ((au8_Result[3] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[14]   = ((au8_Result[3] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[15]   = ((au8_Result[3] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[16]   = ((au8_Result[3] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_Reserved8[17]   = ((au8_Result[3] & 0x80U) > 0U) ? true : false;

      s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_TARGET_SPECIFIC_SERVICES,
                                                au8_Result);
   }

   if (s32_Return == C_NO_ERR)
   {
      orc_ImplementedServices.aq_TargetSpecific[0]  = ((au8_Result[0] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[1]  = ((au8_Result[0] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[2]  = ((au8_Result[0] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[3]  = ((au8_Result[0] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[4]  = ((au8_Result[0] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[5]  = ((au8_Result[0] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[6]  = ((au8_Result[0] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[7]  = ((au8_Result[0] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_TargetSpecific[8]  = ((au8_Result[1] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[9]  = ((au8_Result[1] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[10] = ((au8_Result[1] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[11] = ((au8_Result[1] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[12] = ((au8_Result[1] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[13] = ((au8_Result[1] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[14] = ((au8_Result[1] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[15] = ((au8_Result[1] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_TargetSpecific[16] = ((au8_Result[2] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[17] = ((au8_Result[2] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[18] = ((au8_Result[2] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[19] = ((au8_Result[2] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[20] = ((au8_Result[2] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[21] = ((au8_Result[2] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[22] = ((au8_Result[2] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[23] = ((au8_Result[2] & 0x80U) > 0U) ? true : false;

      orc_ImplementedServices.aq_TargetSpecific[24] = ((au8_Result[3] & 0x01U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[25] = ((au8_Result[3] & 0x02U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[26] = ((au8_Result[3] & 0x04U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[27] = ((au8_Result[3] & 0x08U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[28] = ((au8_Result[3] & 0x10U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[29] = ((au8_Result[3] & 0x20U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[30] = ((au8_Result[3] & 0x40U) > 0U) ? true : false;
      orc_ImplementedServices.aq_TargetSpecific[31] = ((au8_Result[3] & 0x80U) > 0U) ? true : false;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_implementation_information hex-records sub-service.

   get_implementation_information hex-records client implementation

   \param[out]    oru8_MaxRecordLength   maximum hex file record length supported by this server
   \param[out]    oru8_Granularity       required alignment of hex-records (1,2,4 bytes)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetImplementationInformationHexRecords(uint8_t & oru8_MaxRecordLength,
                                                              uint8_t & oru8_Granularity)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetImplementationInformation(mu8_XFL_CMD_GET_IMPLEMENTATION_INFORMATION_HEX_RECORDS, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru8_MaxRecordLength = au8_Result[0];
      oru8_Granularity = au8_Result[1];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_finger_print service.

   "low-level" get_finger_print client implementation

   \param[in]     ou8_Index        index of information to read
   \param[out]    orau8_Result     response data (4 bytes)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFingerPrint(const uint8_t ou8_Index, uint8_t (&orau8_Result)[4])
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_FINGER_PRINT;
   mc_CanWriteMessage.au8_Data[3] = ou8_Index;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      orau8_Result[0] = t_RXMsg.au8_Data[4];
      orau8_Result[1] = t_RXMsg.au8_Data[5];
      orau8_Result[2] = t_RXMsg.au8_Data[6];
      orau8_Result[3] = t_RXMsg.au8_Data[7];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_finger_print supported-indexes sub-service.

   get_finger_print supported-indexes client implementation

   \param[out]    orc_SupportedIndexes   flags which sub-services are supported

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFingerPrintSupportedIndexes(C_XFLFingerPrintSupportedIndexes & orc_SupportedIndexes)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_SUPPORTED_INDEXES, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      orc_SupportedIndexes.q_ProgrammingTime     = ((au8_Result[0] & 0x01U) > 0U) ? true : false;
      orc_SupportedIndexes.q_ProgrammingDate     = ((au8_Result[0] & 0x02U) > 0U) ? true : false;
      orc_SupportedIndexes.q_ApplicationCheckSum = ((au8_Result[0] & 0x04U) > 0U) ? true : false;
      orc_SupportedIndexes.q_UsernamePart1       = ((au8_Result[0] & 0x08U) > 0U) ? true : false;
      orc_SupportedIndexes.q_UsernamePart2       = ((au8_Result[0] & 0x10U) > 0U) ? true : false;
      orc_SupportedIndexes.q_UsernamePart3       = ((au8_Result[0] & 0x20U) > 0U) ? true : false;
      orc_SupportedIndexes.q_UsernamePart4       = ((au8_Result[0] & 0x40U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[0]        = ((au8_Result[0] & 0x80U) > 0U) ? true : false;

      orc_SupportedIndexes.aq_Reserved[1]  = ((au8_Result[1] & 0x01U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[2]  = ((au8_Result[1] & 0x02U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[3]  = ((au8_Result[1] & 0x04U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[4]  = ((au8_Result[1] & 0x08U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[5]  = ((au8_Result[1] & 0x10U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[6]  = ((au8_Result[1] & 0x20U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[7]  = ((au8_Result[1] & 0x40U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[8]  = ((au8_Result[1] & 0x80U) > 0U) ? true : false;

      orc_SupportedIndexes.aq_Reserved[9]  = ((au8_Result[2] & 0x01U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[10] = ((au8_Result[2] & 0x02U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[11] = ((au8_Result[2] & 0x04U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[12] = ((au8_Result[2] & 0x08U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[13] = ((au8_Result[2] & 0x10U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[14] = ((au8_Result[2] & 0x20U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[15] = ((au8_Result[2] & 0x40U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[16] = ((au8_Result[2] & 0x80U) > 0U) ? true : false;

      orc_SupportedIndexes.aq_Reserved[17] = ((au8_Result[3] & 0x01U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[18] = ((au8_Result[3] & 0x02U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[19] = ((au8_Result[3] & 0x04U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[20] = ((au8_Result[3] & 0x08U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[21] = ((au8_Result[3] & 0x10U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[22] = ((au8_Result[3] & 0x20U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[23] = ((au8_Result[3] & 0x40U) > 0U) ? true : false;
      orc_SupportedIndexes.aq_Reserved[24] = ((au8_Result[3] & 0x80U) > 0U) ? true : false;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_finger_print programming-time sub-service.

   get_finger_print programming-time client implementation

   \param[out]    oru8_Hours     hours (0..23)
   \param[out]    oru8_Minutes   minutes (0..59)
   \param[out]    oru8_Seconds   seconds (0..59)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFingerPrintProgrammingTime(uint8_t & oru8_Hours, uint8_t & oru8_Minutes,
                                                     uint8_t & oru8_Seconds)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_TIME, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru8_Hours   = mh_BCD2DEC(au8_Result[0]);
      oru8_Minutes = mh_BCD2DEC(au8_Result[1]);
      oru8_Seconds = mh_BCD2DEC(au8_Result[2]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_finger_print programming-date sub-service.

   get_finger_print programming-date client implementation

   \param[out]    oru8_Years      years   (0..99)
   \param[out]    oru8_Months     minutes (1..12)
   \param[out]    oru8_Days       seconds (1..31)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFingerPrintProgrammingDate(uint8_t & oru8_Years, uint8_t & oru8_Months, uint8_t & oru8_Days)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_DATE, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru8_Years  = mh_BCD2DEC(au8_Result[2]);
      oru8_Months = mh_BCD2DEC(au8_Result[1]);
      oru8_Days   = mh_BCD2DEC(au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_finger_print checksum sub-service.

   get_finger_print checksum client implementation

   \param[out]    oru32_CheckSum   checksum stored on server

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFingerPrintChecksum(uint32_t & oru32_CheckSum)
{
   int32_t s32_Return;
   uint8_t au8_Result[4];

   s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_CHECKSUM, au8_Result);
   if (s32_Return == C_NO_ERR)
   {
      oru32_CheckSum = mh_AU8ToU32LE(&au8_Result[0]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_finger_print user-name sub-service.

   get_finger_print user-name client implementation

   \param[out]    orc_UserName   user name stored in server, zero terminated (max. 16 characters)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetFingerPrintUserName(C_SclString & orc_UserName)
{
   int32_t s32_Return;
   char_t acn_UserName[17];
   uint8_t au8_Buffer[4];

   orc_UserName = "";

   (void)memset(&acn_UserName[0], 0, sizeof(acn_UserName));

   //we can be sure to have enough buffer -> provide buffer through casting to avoid needing a local 4 byte buffer ...
   s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_1, au8_Buffer);
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&acn_UserName[0], &au8_Buffer[0], 4);
      s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_2, au8_Buffer);
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&acn_UserName[4], &au8_Buffer[0], 4);
      s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_3, au8_Buffer);
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&acn_UserName[8], &au8_Buffer[0], 4);
      s32_Return = GetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_4, au8_Buffer);
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&acn_UserName[12], &au8_Buffer[0], 4);
      orc_UserName = acn_UserName;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_device_info_address service.

   "low-level" get_device_info_address client implementation

   \param[in]     ou8_Index       index of information to read
   \param[out]    orau8_Result    result

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::m_GetDeviceInfoAddress(const uint8_t ou8_Index, uint8_t (&orau8_Result)[4])
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_DEVICE_INFO_ADDRESS;
   mc_CanWriteMessage.au8_Data[3] = ou8_Index;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      orau8_Result[0] = t_RXMsg.au8_Data[4];
      orau8_Result[1] = t_RXMsg.au8_Data[5];
      orau8_Result[2] = t_RXMsg.au8_Data[6];
      orau8_Result[3] = t_RXMsg.au8_Data[7];
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_device_info_address service.

   Read all available defined info addresses from server

   \param[out]    orc_Addresses            list of offsets of "device info" struct in ROM
   \param[in]     ou16_ProtocolVersion     protocol version
                                            (as returned by GetImplementationInformationProtocolVersion())

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server                 \n
   C_CONFIG           deviation from protocol specification
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetDeviceInfoAddresses(C_SclDynamicArray<uint32_t> & orc_Addresses,
                                              const uint16_t ou16_ProtocolVersion)
{
   uint8_t au8_Data[4];
   uint8_t u8_Index;
   uint8_t u8_NumStructures;
   int32_t s32_Return;

   if (ou16_ProtocolVersion < 0x3030U)
   {
      //protocol spec < V3.03r0: only one index (0)
      s32_Return = this->m_GetDeviceInfoAddress(0, au8_Data);
      if (s32_Return == C_NO_ERR)
      {
         orc_Addresses.SetLength(1);
         orc_Addresses[0] = mh_AU8ToU32LE(&au8_Data[0]);
      }
   }
   else
   {
      s32_Return = this->m_GetDeviceInfoAddress(mu8_XFL_CMD_GET_DEVICE_INFO_ADDRESS_NUM_BLOCKS, au8_Data);
      if (s32_Return == C_NO_ERR)
      {
         u8_NumStructures = au8_Data[0];
         if ((u8_NumStructures < 1U) || (u8_NumStructures > 32U))
         {
            s32_Return = C_CONFIG; //Spec.: protocol violation
         }
         else
         {
            orc_Addresses.SetLength(u8_NumStructures);
            for (u8_Index = 0U; u8_Index < u8_NumStructures; u8_Index++)
            {
               s32_Return = this->m_GetDeviceInfoAddress(u8_Index, au8_Data);
               if (s32_Return != C_NO_ERR)
               {
                  break;
               }
               orc_Addresses[u8_Index] = mh_AU8ToU32LE(&au8_Data[0]);
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read device information block from server

   Read device information block from ECU and map it into a structure.
   Supports "V1" and "V2" of structure.

   \param[in]     ou32_Address  address of info structure in server's flash memory
   \param[out]    orc_Data      device info block

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server                 \n
   C_CONFIG           no valid structure data found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetDeviceInfoBlock(const uint32_t ou32_Address, C_XFLECUInformation & orc_Data)
{
   int32_t s32_Return;
   uint16_t u16_Offset = 0U;
   uint8_t u8_Size;
   bool q_IsV2 = false;
   uint8_t u8_VersionIndex;

   orc_Data.ClearContents();

   //check that sizes match; otherwise the following charn / uint8 assignment will fail miserable
   tgl_assert(sizeof(char_t) == sizeof(uint8_t)); //lint !e948  //evaluates to contant true if everything is fine ...

   //first read the first couple of bytes to detect "MAGIC":
   s32_Return = this->ReadFlash(XFL_DEVICE_INFO_MAGIC_LENGTH_V1, ou32_Address,
                                reinterpret_cast<uint8_t *>(orc_Data.acn_Magic)); //lint !e926 //cf. assertion above
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }

   s32_Return = memcmp(&orc_Data.acn_Magic[0], XFL_DEVICE_INFO_MAGIC_V1, XFL_DEVICE_INFO_MAGIC_LENGTH_V1);
   if (s32_Return != 0)
   {
      //try V2:
      s32_Return = this->ReadFlash(XFL_DEVICE_INFO_MAGIC_LENGTH_V2, ou32_Address,
                                   reinterpret_cast<uint8_t *>(orc_Data.acn_Magic)); //lint !e926 //cf. assertion above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      s32_Return = memcmp(&orc_Data.acn_Magic[0], XFL_DEVICE_INFO_MAGIC_V2, XFL_DEVICE_INFO_MAGIC_LENGTH_V2);
      if (s32_Return != 0)
      {
         return C_CONFIG;
      }
      q_IsV2 = true;
   }

   u16_Offset = static_cast<uint16_t>(u16_Offset + XFL_DEVICE_INFO_MAGIC_LENGTH_V1);
   u16_Offset += static_cast<uint16_t>((q_IsV2 == true) ? 1U : 0U);

   //read header information
   s32_Return = this->ReadFlash(2U, ou32_Address + u16_Offset, &orc_Data.u8_StructVersion);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }
   if ((orc_Data.u8_StructVersion != 1U) && (orc_Data.u8_StructVersion != 2U))
   {
      return C_CONFIG;
   }
   u16_Offset = static_cast<uint16_t>(u16_Offset + 2U);

   u8_VersionIndex = (q_IsV2 == true) ? 1U : 0U; //lookup index for array length tables

   if (orc_Data.ContainsDeviceID() == true)
   {
      u8_Size = C_XFLECUInformation::hau8_LENGTHS_DEVICE_ID[u8_VersionIndex];
      s32_Return = this->ReadFlash(u8_Size, ou32_Address + u16_Offset,
                                   reinterpret_cast<uint8_t *>(orc_Data.acn_DeviceID)); //lint !e926 //cf. assertion
                                                                                        // above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      u16_Offset = static_cast<uint16_t>(u16_Offset + u8_Size);
   }

   if (orc_Data.ContainsDateAndTime() == true)
   {
      u8_Size = C_XFLECUInformation::hau8_LENGTHS_DATE[u8_VersionIndex];
      s32_Return = this->ReadFlash(u8_Size, ou32_Address + u16_Offset,
                                   reinterpret_cast<uint8_t *>(orc_Data.acn_Date)); //lint !e926 //cf. assertion above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      u16_Offset = static_cast<uint16_t>(u16_Offset + u8_Size);

      u8_Size = C_XFLECUInformation::hau8_LENGTHS_TIME[u8_VersionIndex];
      s32_Return = this->ReadFlash(u8_Size, ou32_Address + u16_Offset,
                                   reinterpret_cast<uint8_t *>(orc_Data.acn_Time)); //lint !e926 //cf. assertion above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      u16_Offset = static_cast<uint16_t>(u16_Offset + u8_Size);
   }

   if (orc_Data.ContainsProjectName() == true)
   {
      u8_Size = C_XFLECUInformation::hau8_LENGTHS_PROJECT_NAME[u8_VersionIndex];
      s32_Return = this->ReadFlash(u8_Size, ou32_Address + u16_Offset,
                                   reinterpret_cast<uint8_t *>(orc_Data.acn_ProjectName)); //lint !e926 //cf. assertion
                                                                                           // above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      u16_Offset = static_cast<uint16_t>(u16_Offset + u8_Size);
   }

   if (orc_Data.ContainsProjectVersion() == true)
   {
      u8_Size = C_XFLECUInformation::hau8_LENGTHS_PROJECT_VERSION[u8_VersionIndex];
      s32_Return = this->ReadFlash(u8_Size, ou32_Address + u16_Offset,
                                   reinterpret_cast<uint8_t *>(orc_Data.acn_ProjectVersion)); //lint !e926 //cf.
                                                                                              // assertion
                                                                                              // above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
      u16_Offset = static_cast<uint16_t>(u16_Offset + u8_Size);
   }

   if (orc_Data.ContainsAdditionalInfo() == true)
   {
      s32_Return = this->ReadFlash(1U, ou32_Address + u16_Offset, &orc_Data.u8_LenAdditionalInfo);
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }

      u16_Offset = static_cast<uint16_t>(u16_Offset + 1U);
      s32_Return = this->ReadFlash(orc_Data.u8_LenAdditionalInfo, ou32_Address + u16_Offset,
                                   reinterpret_cast<uint8_t *>(&orc_Data.acn_AdditionalInfo[0])); //lint !e926 //cf.
                                                                                                  // assertion above
      if (s32_Return != C_NO_ERR)
      {
         return s32_Return;
      }
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_block_address service to read out information
            about all available blocks.

   get_block_address client implementation.
   Will read both, start and end addresses of all available blocks.
   The number of available blocks is detected through an OUT_OF_RANGE response.

   \param[out]    orc_ChecksumBlocks       array with defined checksum blocks

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetBlockAddressesAll(C_SclDynamicArray<C_XFLChecksumBlock> & orc_ChecksumBlocks)
{
   int32_t s32_Return;
   uint8_t u8_BlockIndex = 0U;
   C_XFLChecksumBlock c_Block;

   orc_ChecksumBlocks.SetLength(0);
   s32_Return = C_NO_ERR;
   while (s32_Return == C_NO_ERR)
   {
      s32_Return = this->GetBlockAddresses(u8_BlockIndex, c_Block);
      switch (s32_Return)
      {
      case C_NO_ERR:
         //block found
         orc_ChecksumBlocks.IncLength();
         orc_ChecksumBlocks[orc_ChecksumBlocks.GetHigh()] = c_Block;
         break;
      case C_NOACT:
         //was there an OUT_OF_RANGE response ?
         if (mu8_LastError == XFL_ERR_OUT_OF_RANGE_ERR)
         {
            return C_NO_ERR; //no more blocks -> finished
         }
         else
         {
            //do return the error code !
         }
         break;
      default:
         //do return the error code !
         break;
      }
      u8_BlockIndex++;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_block_address service.

   get_block_address client implementation.
   Will read both, start and end addresses of the specified block.

   Note:
   The "CHECKSUM_ERR" flags a block that is expected to be there but is invalid on the server.
   A "CHECKSUM_ERR" returned from the server is not reported to the application with "C_NOACT".
   Instead it is reported via "orc_ChecksumBlock.q_BlockDefinitionValid" being set to false.
   In this case this function returns C_NO_ERR.

   \param[in]     ou8_BlockNumber            number of block
   \param[out]    orc_ChecksumBlock          information about checksum block

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server (except for CHECKSUM_ERR)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetBlockAddresses(const uint8_t ou8_BlockNumber, C_XFLChecksumBlock & orc_ChecksumBlock)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   //read start address
   mc_CanWriteMessage.u8_DLC      = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_BLOCK_START_ADDRESS;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockNumber;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      orc_ChecksumBlock.u32_StartAddress = mh_AU8ToU32LE(&t_RXMsg.au8_Data[4]);

      //read end address
      mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_BLOCK_END_ADDRESS;
      m_SendMessageWithIDs();
      s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                     &t_RXMsg, 8);
      if (s32_Return == C_NO_ERR)
      {
         orc_ChecksumBlock.u32_EndAddress = mh_AU8ToU32LE(&t_RXMsg.au8_Data[4]);
      }
   }

   //error response with "CHECKSUM_ERR": invalid block
   if ((s32_Return == C_NOACT) && (mu8_LastError == XFL_ERR_CHECKSUM_ERR))
   {
      //values should not really matter -> defensive measure
      orc_ChecksumBlock.u32_StartAddress = XFL_CHK_BLOCK_END_INVALID;
      orc_ChecksumBlock.u32_EndAddress = XFL_CHK_BLOCK_END_INVALID;
      orc_ChecksumBlock.q_BlockDefinitionValid = false;
      s32_Return = C_NO_ERR;
   }
   else
   {
      orc_ChecksumBlock.q_BlockDefinitionValid = true;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_block_checksum service.

   get_block_checksum client implementation.

   ou8_ChecksumType parameter is not range-checked: So we are open to future expansions of the protocol.
   If the server can not handle it it will complain ...

   \param[in]     ou8_BlockNumber            number of block
   \param[in]     ou8_ChecksumType           type of checksum 0 -> read from EEPROM; 1 -> calculate and return
   \param[out]    oru32_Checksum             checksum

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetBlockChecksum(const uint8_t ou8_BlockNumber, const uint8_t ou8_ChecksumType,
                                        uint32_t & oru32_Checksum)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = 5U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_BLOCK_CHECKSUM;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockNumber;
   mc_CanWriteMessage.au8_Data[4] = ou8_ChecksumType;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CALC_CHECKSUM_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      oru32_Checksum = mh_AU8ToU32LE(&t_RXMsg.au8_Data[4]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_finger_print service.

   "low-level" set_finger_print client implementation.

   \param[in]     ou8_Index                  index of data to write
   \param[in]     orau8_Param                data to write

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetFingerPrint(const uint8_t ou8_Index, const uint8_t (&orau8_Param)[4])
{
   mc_CanWriteMessage.u8_DLC      = 8U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_FINGER_PRINT;
   mc_CanWriteMessage.au8_Data[3] = ou8_Index;
   mc_CanWriteMessage.au8_Data[4] = orau8_Param[0];
   mc_CanWriteMessage.au8_Data[5] = orau8_Param[1];
   mc_CanWriteMessage.au8_Data[6] = orau8_Param[2];
   mc_CanWriteMessage.au8_Data[7] = orau8_Param[3];

   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 7U, NULL,
                            8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_finger_print programming-time sub-service.

   set_finger_print programming-time client implementation.

   \param[in]     ou8_Hour      hour (0..23)
   \param[in]     ou8_Minute    minute (0..59)
   \param[in]     ou8_Second    second (0..59)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server                 \n
   C_RANGE            invalid time parameter
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetFingerPrintProgrammingTime(const uint8_t ou8_Hour, const uint8_t ou8_Minute,
                                                     const uint8_t ou8_Second)
{
   uint8_t au8_Data[4];

   if ((ou8_Hour > 23U) || (ou8_Minute > 59U) || (ou8_Second > 59U))
   {
      return C_RANGE;
   }

   au8_Data[0] = mh_DEC2BCD(ou8_Hour);
   au8_Data[1] = mh_DEC2BCD(ou8_Minute);
   au8_Data[2] = mh_DEC2BCD(ou8_Second);
   au8_Data[3] = 0U;
   return SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_TIME, au8_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_finger_print programming-date sub-service.

   set_finger_print programming-date client implementation.

   \param[in]     ou8_Year       year  (0..99)
   \param[in]     ou8_Month      month (1..12)
   \param[in]     ou8_Day        day   (1..31)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server                 \n
   C_RANGE            invalid date parameter
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetFingerPrintProgrammingDate(const uint8_t ou8_Year, const uint8_t ou8_Month,
                                                     const uint8_t ou8_Day)
{
   uint8_t au8_Data[4];

   if ((ou8_Month < 1U) || (ou8_Month > 12U) || (ou8_Day < 1U) || (ou8_Day > 31U) || (ou8_Year > 99U))
   {
      return C_RANGE;
   }

   au8_Data[0] = mh_DEC2BCD(ou8_Day);
   au8_Data[1] = mh_DEC2BCD(ou8_Month);
   au8_Data[2] = mh_DEC2BCD(ou8_Year);
   au8_Data[3] = 0U;
   return SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_PROGRAMMING_DATE, au8_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_finger_print checksum sub-service.

   set_finger_print checksum client implementation.

   \param[in]     ou32_CheckSum           checksum to write to server

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetFingerPrintChecksum(const uint32_t ou32_CheckSum)
{
   uint8_t au8_Data[4];

   au8_Data[0] = static_cast<uint8_t>(ou32_CheckSum);
   au8_Data[1] = static_cast<uint8_t>(ou32_CheckSum >> 8U);
   au8_Data[2] = static_cast<uint8_t>(ou32_CheckSum >> 16U);
   au8_Data[3] = static_cast<uint8_t>(ou32_CheckSum >> 24U);
   return SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_CHECKSUM, au8_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_finger_print user-name sub-service.

   set_finger_print user-name client implementation.
   If more than 16 characters are passed only the first 16 will be used.

   \param[in]     orc_UserName     user-name to write (max. 16 characters)

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetFingerPrintUserName(const C_SclString & orc_UserName)
{
   int32_t s32_Return;
   uint8_t au8_Data[17];
   uint8_t au8_Buffer[4];
   uint32_t u32_Length;

   u32_Length = orc_UserName.Length();

   if (u32_Length >= 16U)
   {
      au8_Data[16] = 0U; //terminate string
      (void)memcpy(&au8_Data[0], orc_UserName.c_str(), 16U);
   }
   else
   {
      (void)memset(&au8_Data[0], 0, 17U);
      (void)memcpy(&au8_Data[0], orc_UserName.c_str(), u32_Length);
   }

   //we can be sure to have enough data -> provide data through casting to avoid needing a local 4 byte buffer ...
   (void)memcpy(&au8_Buffer[0], &au8_Data[0], 4);
   s32_Return = SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_1, au8_Buffer);
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&au8_Buffer[0], &au8_Data[4], 4);
      s32_Return = SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_2, au8_Buffer);
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&au8_Buffer[0], &au8_Data[8], 4);
      s32_Return = SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_3, au8_Buffer);
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)memcpy(&au8_Buffer[0], &au8_Data[12], 4);
      s32_Return = SetFingerPrint(mu8_XFL_CMD_FINGER_PRINT_USERNAME_4, au8_Buffer);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_temp_bitrate service.

   set_temp_bitrate client implementation.

   \param[in]     ou32_TempBitrate      bitrate to switch to

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetTempBitrate(const uint32_t ou32_TempBitrate)
{
   mc_CanWriteMessage.u8_DLC      = 7U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_TEMP_BITRATE;
   mc_CanWriteMessage.au8_Data[3] = static_cast<uint8_t>(ou32_TempBitrate);
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou32_TempBitrate >> 8U);
   mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou32_TempBitrate >> 16U);
   mc_CanWriteMessage.au8_Data[6] = static_cast<uint8_t>(ou32_TempBitrate >> 24U);

   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 6U, NULL,
                            7);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_block_start_address and set_block_end_address services.

   set_block_start_address and set_block_end_address client implementation.

   \param[in]     ou8_BlockNumber     block number to set start address and end address for
   \param[in]     ou32_StartAddress   block start address
   \param[in]     ou32_EndAddress     block end address
   \param[in]     oq_StartAddressOnly true: only set start address, not end address

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetBlockAddresses(const uint8_t ou8_BlockNumber, const uint32_t ou32_StartAddress,
                                         const uint32_t ou32_EndAddress, const bool oq_StartAddressOnly)
{
   int32_t s32_Return;

   //write start address
   mc_CanWriteMessage.u8_DLC      = 8U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_BLOCK_START_ADDRESS;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockNumber;
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou32_StartAddress);
   mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou32_StartAddress >> 8U);
   mc_CanWriteMessage.au8_Data[6] = static_cast<uint8_t>(ou32_StartAddress >> 16U);
   mc_CanWriteMessage.au8_Data[7] = static_cast<uint8_t>(ou32_StartAddress >> 24U);

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 7U,
                                  NULL, 8);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }

   if (oq_StartAddressOnly == true)
   {
      return C_NO_ERR;
   }

   //write end address
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_BLOCK_END_ADDRESS;
   mc_CanWriteMessage.au8_Data[4] = static_cast<uint8_t>(ou32_EndAddress);
   mc_CanWriteMessage.au8_Data[5] = static_cast<uint8_t>(ou32_EndAddress >> 8U);
   mc_CanWriteMessage.au8_Data[6] = static_cast<uint8_t>(ou32_EndAddress >> 16U);
   mc_CanWriteMessage.au8_Data[7] = static_cast<uint8_t>(ou32_EndAddress >> 24U);
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 7U, NULL,
                            8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_block_checksum services.

   set_block_checksum client implementation.

   \param[in]      ou8_BlockNumber             block number
   \param[out]     oru32_CalculatedChecksum    checksum calculated by the server

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetBlockChecksum(const uint8_t ou8_BlockNumber, uint32_t & oru32_CalculatedChecksum)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC      = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_BLOCK_CHECKSUM;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockNumber;

   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CALC_CHECKSUM_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 8);
   if (s32_Return == C_NO_ERR)
   {
      oru32_CalculatedChecksum = mh_AU8ToU32LE(&t_RXMsg.au8_Data[4]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_xfl_exchange.

   set_xfl_exchange client implementation.

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetXflExchange(void)
{
   mc_CanWriteMessage.u8_DLC      = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_XFL_EXCHANGE;

   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_CHANGE_STATE_MS, mc_CanWriteMessage.au8_Data, 2U, NULL,
                            3);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_control_id service.

   get_control_id client implementation.

   \param[out]     oru32_ControlID       control ID read from server

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetControlID(uint32_t & oru32_ControlID)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 3U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_CONTROL_ID;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 2U,
                                  &t_RXMsg, 7);
   if (s32_Return == C_NO_ERR)
   {
      oru32_ControlID = mh_AU8ToU32LE(&t_RXMsg.au8_Data[3]);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of get_block_compare_mode service.

   get_block_compare_mode client implementation.

   \param[in]     ou8_BlockNumber        checksum block number
   \param[out]    orq_CheckAtStartup     true: check at startup configured
   \param[out]    orq_CheckAfterFlashing true: check after flashing configured

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::GetBlockCompareMode(const uint8_t ou8_BlockNumber, bool & orq_CheckAtStartup,
                                           bool & orq_CheckAfterFlashing)
{
   int32_t s32_Return;
   T_STWCAN_Msg_RX t_RXMsg;

   mc_CanWriteMessage.u8_DLC = 4U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_GET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_GET_BLOCK_COMPARE_MODE;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockNumber;
   m_SendMessageWithIDs();

   s32_Return = m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_READ_INFO_MS, mc_CanWriteMessage.au8_Data, 3U,
                                  &t_RXMsg, 5);
   if (s32_Return == C_NO_ERR)
   {
      //we ignore any bits except for 0 and 1 as they are not defined
      orq_CheckAtStartup     = ((t_RXMsg.au8_Data[4] & 0x01U) == 0x01U) ? true : false;
      orq_CheckAfterFlashing = ((t_RXMsg.au8_Data[4] & 0x02U) == 0x02U) ? true : false;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Implementation of set_block_compare_mode service.

   set_block_compare_mode client implementation.

   \param[in]     ou8_BlockNumber        checksum block number
   \param[in]     oq_CheckAtStartup      true: check at startup
   \param[in]     oq_CheckAfterFlashing  true: check after flashing

   \return
   C_NO_ERR           no errors                                  \n
   C_COM              no response from server                    \n
   C_NOACT            error response from server
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::SetBlockCompareMode(const uint8_t ou8_BlockNumber, const bool oq_CheckAtStartup,
                                           const bool oq_CheckAfterFlashing)
{
   mc_CanWriteMessage.u8_DLC = 5U;
   mc_CanWriteMessage.au8_Data[1] = mu8_XFL_CMD_GRP_SET_COMMAND;
   mc_CanWriteMessage.au8_Data[2] = mu8_XFL_CMD_SET_BLOCK_COMPARE_MODE;
   mc_CanWriteMessage.au8_Data[3] = ou8_BlockNumber;
   mc_CanWriteMessage.au8_Data[4] = (oq_CheckAtStartup == true) ? 0x01U : 0x00U;
   mc_CanWriteMessage.au8_Data[4] += ((oq_CheckAfterFlashing == true) ? 0x02U : 0x00U);
   m_SendMessageWithIDs();

   return m_WaitForResponse(mc_Config.u8_LocalID, mu16_TIMEOUT_WRITE_CONFIG_MS, mc_CanWriteMessage.au8_Data, 4U, NULL,
                            5);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Report progress of activity

   Called by some routines to report progress to the application.
   Override this function to adapt it to the application.
   Here: Default implementation; do nothing

   \param[in]     ou16_Progress1_1000       progress to report 0..1000
   \param[in]     orc_AdditionalText        text to report

   \return
   C_NO_ERR   continue operations
   else       abort operation (not necessarily honored by all calls)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_XFLProtocol::TRG_ReportProgress(const uint16_t ou16_Progress1_1000, const C_SclString & orc_AdditionalText)
{
   (void)orc_AdditionalText;
   (void)ou16_Progress1_1000;
   return C_NO_ERR; //do not abort !
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Report status of activity

   Called by some routines to report status to the application.
   Override this function to adapt it to the application.
   Here: Default implementation; do nothing

   \param[in]     orc_Text       text to report
   \param[in]     ou8_Type       type of status (see gu8_DL_REPORT_STATUS_TYPE_* constants)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::TRG_ReportStatus(const C_SclString & orc_Text, const uint8_t ou8_Type)
{
   (void)orc_Text;
   (void)ou8_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Active waiting

   Actively wait for a number of micro seconds

   \param[in]     ou32_NumUs            number of micro seconds to wait
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::TRG_WaitMicroSeconds(const uint32_t ou32_NumUs)
{
   TglDelayUs(ou32_NumUs);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Perform operating system message handling

   Prevents the application from freezing during waiting for replies from the server node.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_XFLProtocol::TRG_HandleSystemMessages(void)
{
   TglHandleSystemMessages();
}

//----------------------------------------------------------------------------------------------------------------------
