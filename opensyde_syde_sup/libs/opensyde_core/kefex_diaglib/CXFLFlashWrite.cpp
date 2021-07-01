//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implements the software update procedure based on the STW flashloader protocol.

   Download is seen from the side of the ECU (-> transfer PC to ECU).

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "DiagLib_config.h" //diaglib configuration

#include <string.h>
#include <limits.h> //for UCHAR_MAX

#include "stwtypes.h"
#include "stwerrors.h"
#include "DLLocalize.h"
#include "CXFLHexFile.h"
#include "CXFLProtocol.h"
#include "CXFLFlashWrite.h"
#include "CXFLActions.h"

#include "TGLFile.h"
#include "TGLUtils.h"
#include "TGLTime.h"

#include "CSCLString.h"
#include "CSCLDateTime.h"
#include "CSCLStringList.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_diag_lib;
using namespace stw_hex_file;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static const uint16 mu16_PROTOCOL_VERSION_3_00 = 0x3000U;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
void C_XFLFlashWrite::m_InitProtocol(const C_XFLFlashWriteParameters & orc_Params)
{
   CfgSetFlashId(orc_Params.u32_SendID, orc_Params.u32_ReceiveID);
   CfgSetXtdId(orc_Params.q_XtdID);
   CfgSetLocalId(mu8_ActualLocalID);
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLFlashWrite::m_Wakeup(const C_XFLWakeupParameters & orc_Params)
{
   sint32 s32_Return;

   s32_Return = PerformWakeup(orc_Params, &mu8_ActualLocalID);
   if (s32_Return == C_NO_ERR)
   {
      m_ReportVerboseStatus(TGL_LoadStr(STR_FM_CONNECTED_NODE) + C_SCLString::IntToStr(mu8_ActualLocalID));
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLFlashWrite::m_FlashingFinished(const E_XFLFlashFinishedAction oe_Action)
{
   uint32 u32_Action;
   sint32 s32_Return;
   E_XFLFlashFinishedAction e_Action;

   if (oe_Action == eXFL_FLASH_FINISHED_ACTION_ASK_USER)
   {
      s32_Return = TRG_UserInteraction(eXFL_USER_INTERACTION_REASON_FINISHED, TGL_LoadStr(STR_FM_FLASHING_FINISHED),
                                       u32_Action);
      if (s32_Return != C_NO_ERR)
      {
         u32_Action = static_cast<uint32>(eXFL_FLASH_FINISHED_ACTION_NONE);
      }

      e_Action = static_cast<E_XFLFlashFinishedAction>(u32_Action);
   }
   else
   {
      e_Action = oe_Action;
   }

   switch (e_Action)
   {
   case eXFL_FLASH_FINISHED_ACTION_NODE_RETURN:
      NodeReturn();
      break;
   case eXFL_FLASH_FINISHED_ACTION_NODE_RESET:
      NodeReset();
      break;
   case eXFL_FLASH_FINISHED_ACTION_NODE_SLEEP:
      NodeSleep();
      break;
   case eXFL_FLASH_FINISHED_ACTION_NET_START:
      NodeSleep(); //make sure CRC over CRCs is written !!
      TGL_Sleep(50U);
      NetStart();
      //wait for request to be sent out for sure (there's no response ...)
      TGL_Sleep(50U);
      break;
   case eXFL_FLASH_FINISHED_ACTION_NET_RESET:
      NodeSleep(); //make sure CRC over CRCs is written !!
      TGL_Sleep(50U);
      NetReset();
      //wait for request to be sent out for sure (there's no response ...)
      TGL_Sleep(50U);
      break;
   case eXFL_FLASH_FINISHED_ACTION_NONE:
      break;
   case eXFL_FLASH_FINISHED_ACTION_ASK_USER: //no break (already checked above)
   default:
      break;
   }
   this->TRG_ReportStatus(TGL_LoadStr(STR_FM_FINISHED), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
}

//----------------------------------------------------------------------------------------------------------------------
//opq_SectorBasedCRCsSupported -> For servers below protocol V3.00r0 we have to
//                                detect whether they support checksums hard-coded
//                                For newer servers this does not matter anyway.
sint32 C_XFLFlashWrite::m_GetVersionNumber(uint8 & oru8_Version, bool & orq_SectorBasedCRCsSupported,
                                           const bool oq_DivertStreamDesired)
{
   uint8 u8_DLC;
   uint8 au8_Version[5];
   uint32 u32_Version;
   sint32 s32_Return;
   C_SCLString c_Text;

   orq_SectorBasedCRCsSupported = false;
   s32_Return = GetVersionNumber(au8_Version, u8_DLC);
   if (s32_Return != C_NO_ERR)
   {
      c_Text = TGL_LoadStr(STR_FDL_VERSION_ERR) + C_XFLActions::XFLProtocolErrorToText(s32_Return, GetLastXFLError());
      TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return -1;
   }
   switch (u8_DLC)
   {
   case 1U:
      oru8_Version = 0U;
      break;
   case 5U:
      oru8_Version = 1U;
      u32_Version = static_cast<uint32>(((static_cast<uint16>(au8_Version[0]) - 48U) * 100U) +
                                        ((static_cast<uint16>(au8_Version[1]) - 48U) * 10U) +
                                        (static_cast<uint16>(au8_Version[2]) - 48U));
      if (u32_Version >= 202U)
      {
         orq_SectorBasedCRCsSupported = true;
      }
      if ((u32_Version < 203U) && (oq_DivertStreamDesired == true))
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_DIVERT_ERR_NO_SUPP), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
      break;
   default:
      oru8_Version = 2U;
      break;
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   write block based checksums to server

   Check server to find out how many blocks there are.
   Then request calculation of checksum for all defined blocks.
   This is used for updating block-based CRCs to EEPROM after flashing.

   We do NOT: Only request calculation for blocks that we have touched when writing.
   This would require a more complex logic: Match hex file content with defined blocks.

   \return
   C_NO_ERR     everything OK (checksum writing confirmed by server)
   else         return value of protocol class function
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::m_WriteBlockBasedChecksums(void)
{
   sint32 s32_Return;
   sint32 s32_Index;
   uint32 u32_CheckSum;

   SCLDynamicArray<C_XFLChecksumBlock> c_Blocks;

   s32_Return = GetBlockAddressesAll(c_Blocks);
   if (s32_Return != C_NO_ERR)
   {
      return s32_Return;
   }

   for (s32_Index = 0; s32_Index < c_Blocks.GetLength(); s32_Index++)
   {
      //for blocks with CRCs in EEPROM there should be no way for them to become invalid
      tgl_assert(c_Blocks[s32_Index].q_BlockDefinitionValid == true);

      if (c_Blocks[s32_Index].u32_EndAddress != XFL_CHK_BLOCK_END_INVALID)
      {
         s32_Return = SetBlockChecksum(static_cast<uint8>(s32_Index), u32_CheckSum);
         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write hex file to STW flashloader server node

   Write hex file to STW flashloader server node with the specified parameters.

   \param[in]     orc_Params     parameters

   \return
   C_NO_ERR     hex file written to target   \n
   else         could not write file to target
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::ExecuteWrite(const C_XFLFlashWriteParameters & orc_Params)
{
   sint32 s32_Return;
   uint8 u8_FlashloaderVersion;
   bool q_CRCsSupported = false;
   C_SCLString c_Text;
   uint32 u32_StartTime;
   uint32 u32_EndTime;
   C_SCLString c_DeviceID;
   uint8 u8_ChecksumType; //0 = none; 1 = sector based; 2 = block based EEPROM;
                          // 3 = block based flash; 4 = deactivated by user
   C_XFLFingerPrintSupportedIndexes c_FingerPrintIndexes;
   uint16 u16_ProtocolVersion;
   uint16 u16_NumSectors;

   mq_VerboseMode = orc_Params.q_VerboseMode;

   if (mc_Config.pc_CANDispatcher == NULL)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DISPATCHER_NA), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_CONFIG;
   }

   if (orc_Params.u16_Version != CXFLFLASHWRITE_VERSION)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_VERSION_MISMATCH),
                       gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_CONFIG;
   }

   mu8_ActualLocalID = orc_Params.c_WakeupConfig.u8_LocalID; //copied as it might change during the process when doing
                                                             // wakeup with SNR

   u32_StartTime = TGL_GetTickCount();

   TRG_ReportStatus("<<<CLRALL", gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

   if (TGL_FileExists(orc_Params.c_HexFile) == false)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_FILE_NOT_FOUND) + " (" + orc_Params.c_HexFile + ")",
                       gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_RD_WR;
   }

   m_InitProtocol(orc_Params);
   s32_Return = m_Wakeup(orc_Params.c_WakeupConfig);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_WAKEUP), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_NOACT;
   }

   if (orc_Params.q_XFLExchange == true)
   {
      s32_Return = SetXflExchange();
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_XFL_EXCHANGE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return C_NOACT;
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FM_XFL_EXCHANGE), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

      //STW Flashloader-Spec: set_xfl_exchange will cause the server to go back to sleep ...
      s32_Return = m_Wakeup(orc_Params.c_WakeupConfig);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_WAKEUP), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return C_NOACT;
      }
   }

   s32_Return = GetImplementationInformationProtocolVersion(u16_ProtocolVersion);
   if (s32_Return != C_NO_ERR)
   {
      //no response or error response: we have to assume we have a version <= 3.00r0
      TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_FL_PROTOCOL_VERSION), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      u16_ProtocolVersion = 0x0000U;
   }
   else
   {
      c_Text.PrintFormatted(TGL_LoadStr(STR_FM_FL_PROTOCOL_VERSION).c_str(),
                            static_cast<uint8>((u16_ProtocolVersion >> 12U) & 0x0FU),
                            static_cast<uint8>((u16_ProtocolVersion >> 8U) & 0x0FU),
                            static_cast<uint8>((u16_ProtocolVersion >> 4U) & 0x0FU),
                            static_cast<uint8>((u16_ProtocolVersion) & 0x0FU));
      TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }

   s32_Return = this->m_GetVersionNumber(u8_FlashloaderVersion, q_CRCsSupported, orc_Params.q_DivertStream);
   if (s32_Return != C_NO_ERR)
   {
      return C_NOACT;
   }

   u8_ChecksumType = 0U;         //default: no checksums supported
   c_FingerPrintIndexes.Clear(); //default: no finger printing supported
   mt_AvailableServices.Clear(); //default: no services available ...

   //not exactly correct, as older flashloader version have some services available
   //but for now we don't check for any of these here ...
   if (u16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00)
   {
      C_XFLFeaturesAvailable c_Features;

      s32_Return = GetImplementationInformationServices(mt_AvailableServices);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_READ_IMPL_INFO), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return C_NOACT;
      }

      //"parse" availabality of features:
      c_Features.DetermineFeatures(mt_AvailableServices, u16_ProtocolVersion);

      if (c_Features.q_SectorBasedCRCs == true)
      {
         u8_ChecksumType = 1U;
      }
      //Only either sector- or block-based checksums shall be implemented.
      //Just in case both are implemented: block-based wins ...
      if (c_Features.q_BlockBasedCRCsEEPROM == true)
      {
         u8_ChecksumType = 2U; //0 = none; 1 = sector based; 2 = block based
      }
      //and the best one is ...
      if (c_Features.q_BlockBasedCRCsFlash == true)
      {
         u8_ChecksumType = 3U;
      }

      if (c_Features.q_FingerPrint == true)
      {
         s32_Return = GetFingerPrintSupportedIndexes(c_FingerPrintIndexes);
         if (s32_Return != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_READ_FINGER_PRINT),
                             gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return C_NOACT;
         }
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_FINGER_PRINT_NA), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
         //keep defaults: no finger-print information ...
      }
   }
   else
   {
      if (q_CRCsSupported == true)
      {
         u8_ChecksumType = 1U; //only sector based CRCs available ...
      }
   }

   if (orc_Params.q_DivertStream == true)
   {
      s32_Return = DivertStreamOnOffBBB(true, orc_Params.u8_DivertTargetIndex, orc_Params.au8_DivertUserID);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FM_ERR_DIVERT_STREAM), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return C_NOACT;
      }
      //ESX2 babyboard B flashloader version 1.00 does not support CRCs
      //if the CRC mechanism is implemented this has to be handled via the version number
      u8_ChecksumType = 0U;
   }
   if (orc_Params.q_WriteCRCsIfSupported == false)
   {
      u8_ChecksumType = 4U; //user configuration: do not write ...
   }

   //print which checksum are going to be used ...
   switch (u8_ChecksumType)
   {
   case 0U: //not supported
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_CRCS_NO_SUPP), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      break;
   case 1U: //sector based
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_SBC_SUPPORTED), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      break;
   case 2U: //no break
   case 3U: //block based EEPROM or FLASH
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_BBC_SUPPORTED), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      break;
   case 4U: //deactivated
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_CS_DEACTIVATED), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      break;
   default:
      tgl_assert(false);
      break;
   }

   s32_Return = GetSectorCount(u16_NumSectors);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_SEC_COUNT_ERR), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_NOACT;
   }
   TRG_ReportStatus(TGL_LoadStr(STR_FDL_NUM_SECTORS) + " " + C_SCLString::IntToStr(u16_NumSectors),
                    gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

   s32_Return = GetDeviceID(u16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00, c_DeviceID);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FW_ERR_READ_DEV_ID), gu8_DL_REPORT_STATUS_TYPE_WARNING);
      c_DeviceID = "";
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FW_DEV_ID) + " \"" + c_DeviceID + "\"", gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }

   //hack for ESX2: new flashloader: Intel->7, old flashloader: Intel->6
   if (u16_ProtocolVersion < mu16_PROTOCOL_VERSION_3_00)
   {
      if ((u16_NumSectors == 6U) && (u8_FlashloaderVersion == 0U))
      {
         u16_NumSectors = 7U;
      }
   }
   mau8_SectorsToErase.SetLength(u16_NumSectors);

   s32_Return = m_FlashESXLokalID(c_DeviceID, c_FingerPrintIndexes, orc_Params, u16_ProtocolVersion);
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_FLASHING), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_NOACT;
   }

   //program was flashed
   //now write the CRCs for all previously erased sectors
   if (orc_Params.q_XFLExchange == false) //In XFL exchange mode updating the CRCs may fail as not all services
                                          // may be available in this mode.
                                          //cf. Flashloader specification chapter set_xfl_exchange
                                          //for a clean solution (e.g. perform the reset sequence as described there
                                          // before continuing with this block)
   {
      s32_Return = m_WriteFlashChecksums(u8_ChecksumType, orc_Params.c_WakeupConfig);
      if (s32_Return != C_NO_ERR)
      {
         return C_NOACT;
      }
   }

   if (orc_Params.q_DivertStream == true)
   {
      (void)DivertStreamOnOffBBB(false, orc_Params.u8_DivertTargetIndex, orc_Params.au8_DivertUserID);
      //ignore result; show flashing finished dialog in any case !
   }

   u32_EndTime = TGL_GetTickCount();
   c_Text.PrintFormatted("%s %d s", TGL_LoadStr(STR_FDL_TOTAL_TIME).c_str(), (u32_EndTime - u32_StartTime) / 1000U);
   m_ReportVerboseStatus(c_Text);

   m_FlashingFinished(orc_Params.e_FlashFinishedAction);
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   update resp. check flash content checksums

   To be called after the flash content was modified

   Exact action depends on flash CRC mode:
   * sector based CRCs: calculate CRCs for each flash sector
   * block based CRCs/EEPROM: calculate CRCs for each defined flash block
   * block based CRCs/flash: read back CRCs from server and let user know mismatches
   * deactivated or not supported: do nothing

   \param[in]   ou8_Mode          0 = not supported by server
                                  1 = sector based CRCs
                                  2 = block based CRCs stored in EEPROM
                                  3 = block based CRCs stored in flash
                                  4 = deacticated by user
   \param[in]   orc_WakeupConfig  configuration for wakeup (only required for mode = 1)

   \return
   C_NO_ERR           no errors
   else               trouble
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::m_WriteFlashChecksums(const uint8 ou8_Mode, const C_XFLWakeupParameters & orc_WakeupConfig)
{
   sint32 s32_Block;
   uint16 u16_Dummy;
   sint32 s32_Return = C_NO_ERR;
   C_XFLWakeupParameters c_WakeupParams;
   C_XFLChecksumAreas c_CRCs;

   switch (ou8_Mode)
   {
   case 1U: //sector based
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_WRITING_CRCS), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      for (s32_Block = 0; s32_Block < mau8_SectorsToErase.GetLength(); s32_Block++)
      {
         //Always write CRC for Boot Sector !
         //Reason: make sure the checksum over the flashloader is correct.
         //Strictly speaking this should be done when the flashloader is first put onto the ECU.
         //But we have to keep this here for compatibility reasons:
         //- for some ECUs the STW production procedure does not set the flashloader's checksum otherwise
         //- some ECU applications check for the correct checksum and fail if it is not OK
         if ((mau8_SectorsToErase[s32_Block] == 1U) || (s32_Block == 0))
         {
            s32_Return = SetSecCRC(static_cast<uint16>(s32_Block), u16_Dummy);
            if (s32_Return != C_NO_ERR)
            {
               TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_WRITING_CRCS), gu8_DL_REPORT_STATUS_TYPE_ERROR);
               s32_Return = C_NOACT;
            }
         }
      }
      if (s32_Return == C_NO_ERR)
      {
         NodeSleep(); //make sure CRC over CRCs is written (required on some older target implementations)
         //wake up again
         //we don't need to search again
         c_WakeupParams = orc_WakeupConfig;
         c_WakeupParams.q_SendResetRQ = false;
         c_WakeupParams.q_SendFLASHRequired = false;
         this->PerformWakeup(c_WakeupParams);
      }
      break;
   case 2U: //block based EEPROM
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_WRITING_CRCS), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      s32_Return = this->m_WriteBlockBasedChecksums();
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_WRITING_CRCS), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         s32_Return = C_NOACT;
      }
      break;
   case 3U: //block based flash
      //CRCs are already part of the hex file data
      //Read CRCs of all blocks so we can warn user if there is a mismatch
      //Helps to troubleshoot if the flashloader does not start the application due to CRC mismatch
      TRG_ReportStatus("Verifying flash block checksums ...", gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

      s32_Return = this->ReadServerBlockChecksumInformation(c_CRCs);
      if (s32_Return == C_NO_ERR) //information read, check the content ...
      {
         for (s32_Block = 0; s32_Block < c_CRCs.c_BlockConfig.GetLength(); s32_Block++)
         {
            if (c_CRCs.c_BlockConfig[s32_Block].q_BlockDefinitionValid == false)
            {
               //this is a hard error: invalid blocks will most likely cause the flashloader to not start the
               // application
               TRG_ReportStatus("Warning: flash checksum block " + C_SCLString::IntToStr(s32_Block) + " is invalid !",
                                gu8_DL_REPORT_STATUS_TYPE_WARNING);
               s32_Return = C_CONFIG;
            }
            else if (c_CRCs.c_Areas[s32_Block].u32_ChecksumEEP != c_CRCs.c_Areas[s32_Block].u32_ChecksumCalc)
            {
               //Mismatching CRCs are no hard errors as we might want to flash multiple files in sequence
               //So for the user this could just be an information.
               //So we do not return an error if we only have this type of information.
               TRG_ReportStatus("Information: flash checksum of block " + C_SCLString::IntToStr(s32_Block) +
                                " does not match the flash contents !", gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
            }
            else
            {
               //looks good ...
            }
         }
      }
      break;
   default:
      break;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   do a auto-detect of the sectors to erase

   The flash memory mapping is read from the server.

   Mode 1: hex file match:
   Automatically detect which sectors to erase based on:
   - the flash memory mapping on the server
   - the address area which is occupied in the hexfile

   Mode 2: all but protected:
   Flag all sectors to be erased except protected ones.

   \param[in]   orc_HexFile         hex-file to match against the sector mapping
   \param[in]   oq_AllButProtected  true: "all but protected" mode on
                                    false: "hex file match" mode on
   \param[in]   orc_FlashInfo       flash layout and timing information (as read with ::ReadFlashInformation)

   \return
   C_NO_ERR           no errors                                                                \n
   C_COM              could not read flash information from server
   C_CONFIG           flash information table reported from server inconsistent with reported  \n
                       number of sectors
   C_RANGE            hex file data occupies protected flash sector
   C_OVERFLOW         hex file data occupies memory not available on the server
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::m_SetAutoSectors(C_HexFile & orc_HexFile, const bool oq_AllButProtected,
                                         C_XFLFlashInformation & orc_FlashInfo)
{
   C_SCLString c_Text;
   const uint8 * pu8_NextLine;
   uint32 u32_StartAddress;
   uint32 u32_EndAddress;
   uint8 u8_Size;
   uint16 u16_Sector;
   sint32 s32_Index;
   sint32 s32_Return;
   bool q_FoundStartSector;
   bool q_FoundEndSector;
   C_XFLFlashSectors c_SectorTable;
   uint8 u8_Range;
   sint64 s64_Offset;

   //match hexfile to address areas
   orc_FlashInfo.ConvertToFlashSectorTable(c_SectorTable);

   //consistency check: at_SectorTable.Length must match total number of sectors !
   if (mau8_SectorsToErase.GetLength() != c_SectorTable.GetLength())
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_FL_IMPLEMENTATION),
                       gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_CONFIG;
   }

   if (oq_AllButProtected == true)
   {
      //flag all sectors except protected ones:
      for (s32_Index = 0; s32_Index < mau8_SectorsToErase.GetLength(); s32_Index++)
      {
         mau8_SectorsToErase[s32_Index] = (c_SectorTable[s32_Index].q_IsProtected == false) ? 1U : 0U;
      }
   }
   else
   {
      //mau8_SectorsToErase.Length contains the total (linear) number of sectors ...
      //also, the hexfile is already sorted by address
      orc_HexFile.LineInit();
      pu8_NextLine = orc_HexFile.NextBinData(u32_StartAddress, u8_Size);
      while (pu8_NextLine != NULL)
      {
         u32_EndAddress = u32_StartAddress + (static_cast<uint32>(u8_Size) - 1U);

         q_FoundStartSector = false;
         q_FoundEndSector = false;

         //Assumptions here:
         //- one hex-line does not span 3 or more sectors
         //- a hex line could partially lie in a physical and partially in an aliased sector
         //   however this seems to be a purely theoretical case and is ignored here

         //is it aliased ?
         if (orc_FlashInfo.c_Aliases.GetRangeOccupiedByAddress(u32_StartAddress, u8_Range) == C_NO_ERR)
         {
            //it is aliased -> get physical address
            s64_Offset = orc_FlashInfo.c_Aliases[u8_Range].GetOffset();
            if ((((static_cast<sint64>(u32_StartAddress) + s64_Offset) < 0LL) ||
                 ((static_cast<sint64>(u32_StartAddress) + s64_Offset) > 0xFFFFFFFFLL)) ||
                (((static_cast<sint64>(u32_EndAddress) + s64_Offset) < 0LL) ||
                 ((static_cast<sint64>(u32_EndAddress) + s64_Offset) > 0xFFFFFFFFLL)))
            {
               TRG_ReportStatus("Error: Aliasing would result in address overflow !", gu8_DL_REPORT_STATUS_TYPE_ERROR);
               return C_RANGE;
            }

            u32_StartAddress = static_cast<uint32>(static_cast<sint64>(u32_StartAddress) +
                                                   orc_FlashInfo.c_Aliases[u8_Range].GetOffset());
            u32_EndAddress   = static_cast<uint32>(static_cast<sint64>(u32_EndAddress) +
                                                   orc_FlashInfo.c_Aliases[u8_Range].GetOffset());
         }

         s32_Return = c_SectorTable.GetSectorOccupiedByAddress(u32_StartAddress, u16_Sector);
         if (s32_Return == C_NO_ERR)
         {
            mau8_SectorsToErase[u16_Sector] = 1U;
            if (c_SectorTable[u16_Sector].q_IsProtected == true)
            {
               TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_HEX_ON_PROTECTED_S), gu8_DL_REPORT_STATUS_TYPE_ERROR);
               return C_RANGE;
            }
            q_FoundStartSector = true;
         }

         s32_Return = c_SectorTable.GetSectorOccupiedByAddress(u32_EndAddress, u16_Sector);
         if (s32_Return == C_NO_ERR)
         {
            mau8_SectorsToErase[u16_Sector] = 1U;
            if (c_SectorTable[u16_Sector].q_IsProtected == true)
            {
               TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_HEX_ON_PROTECTED_S), gu8_DL_REPORT_STATUS_TYPE_ERROR);
               return C_RANGE;
            }
            q_FoundEndSector = true;
         }

         //did the line occupy any defined memory range ?
         if ((q_FoundStartSector == false) || (q_FoundEndSector == false))
         {
            c_Text.PrintFormatted(
               "Error: Hex file data occupies unavailable memory !\n Data from 0x%X to 0x%X does not fit into an available flash sector !",
               u32_StartAddress, u32_EndAddress);
            TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return C_OVERFLOW;
         }

         pu8_NextLine = orc_HexFile.NextBinData(u32_StartAddress, u8_Size);
      }
   }

   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLFlashWrite::m_SetSectorsToErase(C_HexFile & orc_HexFile, const C_SCLString & orc_DeviceID,
                                            const C_XFLFlashWriteParameters & orc_Params,
                                            C_XFLFlashInformation & orc_FlashInfo,
                                            const stw_types::uint16 ou16_ProtocolVersion)
{
   sint32 s32_Return;
   const uint16 u16_NumSectorsTotal = static_cast<uint16>(mau8_SectorsToErase.GetLength());

   for (sint32 s32_Sector = 0; s32_Sector < mau8_SectorsToErase.GetLength(); s32_Sector++)
   {
      mau8_SectorsToErase[s32_Sector] = 0U;
   }

   //do some plausibility checks:
   if (ou16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00)
   {
      switch (orc_Params.e_EraseMode)
      {
      case eXFL_ERASE_MODE_USER_DEFINED: //no break
      case eXFL_ERASE_MODE_AUTOMATIC:    //no break
         break;                          //valid
      case eXFL_ERASE_MODE_C_APPLICATION:
         //issue a warning, that this might not be a very good selection:
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_HINT_USE_AUTO_MODE), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
         break;                             //valid
      case eXFL_ERASE_MODE_CANOPEN_CONFIG:  //no break
      case eXFL_ERASE_MODE_IEC_APPLICATION: //no break
      case eXFL_ERASE_MODE_IEC_RUN_TIME_SYSTEM:
      default:
         //do not hint at using "C-Application" (we do not really want the user to use it)
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ONLY_F_AUTO_UD), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }
   else
   {
      if (orc_Params.e_EraseMode == eXFL_ERASE_MODE_AUTOMATIC)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_FILETYPE_AUTO_NA),
                          gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }

   switch (orc_Params.e_EraseMode)
   {
   case eXFL_ERASE_MODE_C_APPLICATION:
      if (ou16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00)
      {
         s32_Return = m_SetAutoSectors(orc_HexFile, true, orc_FlashInfo);
         if (s32_Return != C_NO_ERR)
         {
            return -1;
         }
      }
      else
      {
         for (uint16 u16_Sector = 1; u16_Sector < u16_NumSectorsTotal; u16_Sector++) //erase everything except sector 0
         {
            mau8_SectorsToErase[u16_Sector] = 1U;
         }
         if (orc_Params.q_DivertStream == true) //BBB -> don't erase parameter blocks
         {
            if (u16_NumSectorsTotal < 3U)
            {
               TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_C_APPL_NOT_POSSIBLE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
               return -1;
            }
            mau8_SectorsToErase[1] = 0U;
            mau8_SectorsToErase[2] = 0U;
         }
      }
      break;
   case eXFL_ERASE_MODE_CANOPEN_CONFIG:
      if (u16_NumSectorsTotal < 3U)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_CANOPEN_NOT_POSSIBLE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
      mau8_SectorsToErase[2] = 1U;
      if (orc_Params.q_DivertStream == true) //BBB -> param block 1 only
      {
         mau8_SectorsToErase[1] = 1U;
      }
      break;
   case eXFL_ERASE_MODE_IEC_APPLICATION:
      switch (u16_NumSectorsTotal)
      {
      case 7U: //512kB Intel Flash
         mau8_SectorsToErase[5] = 1U;
         mau8_SectorsToErase[6] = 1U;
         break;
      case 11U: //512kB AMD Flash
         mau8_SectorsToErase[7] = 1U;
         mau8_SectorsToErase[8] = 1U;
         mau8_SectorsToErase[9] = 1U;
         mau8_SectorsToErase[10] = 1U;
         break;
      case 19U: //1MB AMD Flash
         mau8_SectorsToErase[7] = 1U;
         mau8_SectorsToErase[8] = 1U;
         mau8_SectorsToErase[9] = 1U;
         mau8_SectorsToErase[10] = 1U;
         mau8_SectorsToErase[11] = 1U;
         mau8_SectorsToErase[12] = 1U;
         mau8_SectorsToErase[13] = 1U;
         mau8_SectorsToErase[14] = 1U;
         mau8_SectorsToErase[15] = 1U;
         mau8_SectorsToErase[16] = 1U;
         break;
      default:
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_IECAPPL_NOT_POSSIBLE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
      break;
   case eXFL_ERASE_MODE_IEC_RUN_TIME_SYSTEM:
      switch (u16_NumSectorsTotal)
      {
      case 7U:                       //512kB Intel Flash
         if (orc_DeviceID == "MICR") //ESX micro RTS is also in sectors 1 and 2
         {
            mau8_SectorsToErase[1] = 1U;
            mau8_SectorsToErase[2] = 1U;
         }
         mau8_SectorsToErase[3] = 1U;
         mau8_SectorsToErase[4] = 1U;
         break;
      case 11U: //512kB AMD Flash
         mau8_SectorsToErase[3] = 1U;
         mau8_SectorsToErase[4] = 1U;
         mau8_SectorsToErase[5] = 1U;
         mau8_SectorsToErase[6] = 1U;
         break;
      case 19U: //1MB AMD Flash
         mau8_SectorsToErase[3]  = 1U;
         mau8_SectorsToErase[4]  = 1U;
         mau8_SectorsToErase[5]  = 1U;
         mau8_SectorsToErase[6]  = 1U;
         mau8_SectorsToErase[17] = 1U;
         mau8_SectorsToErase[18] = 1U;
         break;
      default:
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_IECRTS_NOT_POSSIBLE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
      break;
   case eXFL_ERASE_MODE_USER_DEFINED:
      s32_Return = m_SetUserDefinedSectors(orc_Params.c_UserDefinedSectors);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_SEC_ERASE_INVALID), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
      break;
   case eXFL_ERASE_MODE_AUTOMATIC:
      s32_Return = m_SetAutoSectors(orc_HexFile, false, orc_FlashInfo);
      if (s32_Return != C_NO_ERR)
      {
         return -1;
      }
      break;
   default:
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_INVALID_HEX_FILE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return -1;
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLFlashWrite::m_SetUserDefinedSectors(const C_SCLString & orc_Sectors)
{
   uint16 u16_Index;
   uint16 u16_Value;
   uint16 u16_Value2;
   C_SCLString c_Help;
   C_SCLString c_Help2;
   C_SCLStringList c_Strings;
   charn * pcn_String;

   for (uint32 u32_Sector = 0; u32_Sector < static_cast<uint32>(mau8_SectorsToErase.GetLength()); u32_Sector++)
   {
      mau8_SectorsToErase[u32_Sector] = 0U; //first set all sectors to not being erased
   }

   //Code could be cleaned up by using C_SCLString::Tokenize.
   //This is quite risky, however as long as we don't have dedicated test cases.
   pcn_String = new charn[static_cast<uintn>(orc_Sectors.Length() + 1)];
   (void)memcpy(pcn_String, orc_Sectors.c_str(), orc_Sectors.Length());
   pcn_String[orc_Sectors.Length()] = '\0'; //terminate target string ...
   u16_Index = 0U;                          //index of last "start of string"
   for (uint32 u32_Sector = 0U; u32_Sector < orc_Sectors.Length(); u32_Sector++)
   {
      if (pcn_String[u32_Sector] == ',')
      {
         pcn_String[u32_Sector] = '\0'; //separate tokens by terminating with "0"

         c_Help = (static_cast<C_SCLString>(&pcn_String[u16_Index])).Trim();
         if (c_Help != "") //ignore "empty" commas
         {
            (void)c_Strings.Add(c_Help);
         }
         u16_Index = static_cast<uint16>(u32_Sector + 1);
      }
   }
   //get last token:
   c_Help = (static_cast<C_SCLString>(&pcn_String[u16_Index])).Trim();
   if (c_Help != "") //ignore "empty" commas
   {
      (void)c_Strings.Add(c_Help);
   }

   delete[] pcn_String;

   //now go through list of all tokens:
   for (uint32 u32_Sector = 0U; u32_Sector < c_Strings.GetCount(); u32_Sector++)
   {
      if (c_Strings.Strings[u32_Sector].Pos("-") != 0)
      {
         //it's a "from - to" area
         c_Help  = c_Strings.Strings[u32_Sector].SubString(1U, c_Strings.Strings[u32_Sector].Pos("-") - 1U).Trim();
         c_Help2 = c_Strings.Strings[u32_Sector].SubString(c_Strings.Strings[u32_Sector].Pos("-") + 1U,
                                                           c_Strings.Strings[u32_Sector].Length()).Trim();
         try
         {
            u16_Value  = static_cast<uint16>(c_Help.ToInt());
            u16_Value2 = static_cast<uint16>(c_Help2.ToInt());
         }
         catch (...)
         {
            return -1;
         }
         if ((u16_Value2 < u16_Value) || (u16_Value >= static_cast<uint16>(mau8_SectorsToErase.GetLength())))
         {
            TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_SEC_ERASE_INV2), gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return -1;
         }
         for (uint16 u16_IndexValue = u16_Value; u16_IndexValue <= u16_Value2; u16_IndexValue++)
         {
            mau8_SectorsToErase[u16_IndexValue] = 1U;
         }
      }
      else
      {
         //simple value
         try
         {
            u16_Value = static_cast<uint16>(c_Strings.Strings[u32_Sector].ToInt());
         }
         catch (...)
         {
            return -1;
         }
         if (u16_Value >= static_cast<uint16>(mau8_SectorsToErase.GetLength()))
         {
            TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_SEC_ERASE_INV2), gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return -1;
         }
         mau8_SectorsToErase[u16_Value] = 1U;
      }
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   write fingerprint data to server

   Write programming data and time, username.
   Set application checksum to "0" to signal that programming was started, but not finished.
   For finger-print sequence cf. the STW flashloader specification.

   \param[in]    orc_FingerPrintIndexes  flags signalling which finger print services are available

   \return
   C_NO_ERR    checksum calculated
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::m_WriteStartFingerPrint(const C_XFLFingerPrintSupportedIndexes & orc_FingerPrintIndexes)
{
   sint32 s32_Return;
   C_SCLString c_UserName;
   C_SCLDateTime c_Now;
   bool q_Return;

   c_Now = C_SCLDateTime::Now();
   if (orc_FingerPrintIndexes.q_ProgrammingDate == true)
   {
      s32_Return = SetFingerPrintProgrammingDate(static_cast<uint8>(c_Now.mu16_Year % 100U),
                                                 static_cast<uint8>(c_Now.mu16_Month),
                                                 static_cast<uint8>(c_Now.mu16_Day));
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_WR_PROG_DATA), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }

   if (orc_FingerPrintIndexes.q_ProgrammingTime == true)
   {
      s32_Return = SetFingerPrintProgrammingTime(static_cast<uint8>(c_Now.mu16_Hour),
                                                 static_cast<uint8>(c_Now.mu16_Minute),
                                                 static_cast<uint8>(c_Now.mu16_Second));
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_WR_PROG_TIME), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }

   if (orc_FingerPrintIndexes.q_UsernamePart1 == true)
   {
      q_Return = TGL_GetSystemUserName(c_UserName);
      if (q_Return != true)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_RD_USER_NAME), gu8_DL_REPORT_STATUS_TYPE_WARNING);
         c_UserName = "unknown";
      }
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_TXT_REC_USER_N_P1) + c_UserName + TGL_LoadStr(STR_FDL_TXT_REC_USER_N_P2),
                       gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      s32_Return = SetFingerPrintUserName(c_UserName);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_WR_USER_NAME), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }

   if (orc_FingerPrintIndexes.q_ApplicationCheckSum == true)
   {
      s32_Return = this->SetFingerPrintChecksum(0U);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_WR_APPL_CHECKS), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLFlashWrite::m_FlashESXLokalID(const C_SCLString & orc_DeviceID,
                                          const C_XFLFingerPrintSupportedIndexes & orc_FingerPrintIndexes,
                                          const C_XFLFlashWriteParameters & orc_Params,
                                          const stw_types::uint16 ou16_ProtocolVersion)
{
   sint32 s32_Return2;
   uint32 u32_LineCount = 0U;
   uint16 u16_NumSentNow;
   sint32 s32_Return;
   uint32 u32_NumBytes;
   uint32 u32_NumBytesFlashed = 0U;
   uint32 u32_NumBytesTotal;
   uint32 u32_NumLinesTotal;
   uint32 u32_Return;
   uint32 u32_OldTime;
   uint32 u32_OldTimeTotal;
   C_XFLHexFile c_HexFile;
   C_XFLCompanyID c_CompID;
   uint32 u32_NumSentProgress;
   C_XFLFlashInformation t_FlashInfo;
   uint32 u32_FileChecksum;

   uint32 u32_EraseTime;
   uint32 u32_FlashTime;
   const uint8 * pu8_Line;
   float32 f32_Rate;
   C_SCLString c_Text;

   C_XFLActions::CompIDStringToStruct(orc_Params.c_WakeupConfig.c_CompanyID, c_CompID);

   s32_Return = 0U;

   u32_Return = c_HexFile.LoadFromFile(orc_Params.c_HexFile.c_str());
   if (u32_Return != stw_hex_file::NO_ERR)
   {
      c_Text = c_HexFile.ErrorCodeToErrorText(u32_Return);
      switch (orc_Params.u8_IgnoreInvalidHexfileError)
      {
      case 0U:
         TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
         s32_Return = -1;
         break;
      case 1U:
         TRG_ReportStatus(c_Text + TGL_LoadStr(STR_FDL_TXT_IGNORE), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
         break;
      case 2U:
         if ((u32_Return & stw_hex_file::ERR_MASK) == stw_hex_file::WRN_RECORD_OVERLAY)
         {
            s32_Return = TRG_UserInteraction(eXFL_USER_INTERACTION_REASON_HEXFILE_OVERLAP, c_Text, u32_Return);
            if (s32_Return != C_NO_ERR)
            {
               s32_Return = -1;
            }
         }
         else
         {
            TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
            s32_Return = -1;
         }
         break;
      default:
         tgl_assert(false);
         s32_Return = -1;
         break;
      }
   }
   if (s32_Return != 0)
   {
      return -1;
   }

   //calculate checksum before doing anything else with the hex file
   s32_Return = c_HexFile.CalcFileChecksum(u32_FileChecksum);
   if (s32_Return != C_NO_ERR)
   {
      u32_FileChecksum = 0U;
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_WARN_COULD_NOT_CALC_APPL_CHK), gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }

   //device ID x-check:
   //perform device-id x-check
   s32_Return = m_DoDeviceIDXCheck(c_HexFile, orc_DeviceID, orc_Params, ou16_ProtocolVersion);
   if (s32_Return != 0)
   {
      //error message already in subfunction
      return s32_Return;
   }

   //read information about flash memory if available:
   if (ou16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00)
   {
      uint8 u8_RecordLength;
      uint8 u8_Granularity;

      s32_Return = ReadFlashInformation(t_FlashInfo, c_Text);
      if (s32_Return != C_NO_ERR)
      {
         //service is mandatory for flashloader protocol >= 3.0
         TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }

      //we can read the maximum hex record length from the server:
      s32_Return = GetImplementationInformationHexRecords(u8_RecordLength, u8_Granularity);
      if (s32_Return != C_NO_ERR)
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_RD_HEX_REC_INFO), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }

      m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_CHG_HEX_REC_LENGTH) +
                            C_SCLString::IntToStr(u8_RecordLength) + ")");

      u32_Return = c_HexFile.Optimize(u8_RecordLength);
      if (u32_Return != NO_ERR)
      {
         TRG_ReportStatus(c_HexFile.ErrorCodeToErrorText(u32_Return), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         return -1;
      }
   }
   else
   {
      if (orc_Params.u8_HexRecordLength != 0U)
      {
         m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_DO_CHG_HEX_REC_LENGTH) +
                               C_SCLString::IntToStr(orc_Params.u8_HexRecordLength));

         u32_Return = c_HexFile.Optimize(orc_Params.u8_HexRecordLength);
         if (u32_Return != static_cast<uint32>(C_NO_ERR))
         {
            TRG_ReportStatus(c_HexFile.ErrorCodeToErrorText(u32_Return), gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return -1;
         }
      }
   }

   s32_Return = m_SetSectorsToErase(c_HexFile, orc_DeviceID, orc_Params, t_FlashInfo, ou16_ProtocolVersion);
   if (s32_Return != 0)
   {
      //error message already in subfunction
      return s32_Return;
   }

   c_Text = TGL_LoadStr(STR_FDL_SEC_ERASE) + " ";
   for (sint32 s32_Sector = 0; s32_Sector < mau8_SectorsToErase.GetLength(); s32_Sector++)
   {
      if (mau8_SectorsToErase[s32_Sector] == 1U)
      {
         c_Text = c_Text + C_SCLString::IntToStr(s32_Sector) + " ";
      }
   }
   TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERASING), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   u32_OldTimeTotal = TGL_GetTickCount();

   for (sint32 s32_Sector = 0; s32_Sector < mau8_SectorsToErase.GetLength(); s32_Sector++)
   {
      if (mau8_SectorsToErase[s32_Sector] == 1U)
      {
         c_Text = TGL_LoadStr(STR_FDL_ERASING_SECTOR) + " " + C_SCLString::IntToStr(s32_Sector);
         TRG_ReportStatus("<<<CLRLINE", gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
         TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

         if (ou16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00)
         {
            //pass erase timeout read with ::ReadFlashInformation
            s32_Return = this->EraseSector(static_cast<uint16>(s32_Sector),
                                           t_FlashInfo.GetEraseTimeByLinearSectorNumber(static_cast<uint16>(s32_Sector)));
         }
         else
         {
            //use default timeout
            s32_Return = this->EraseSector(static_cast<uint16>(s32_Sector));
         }
         if (s32_Return != C_NO_ERR)
         {
            c_Text = TGL_LoadStr(STR_FDL_ERR_ERASE) + " " + C_SCLString::IntToStr(s32_Sector) +
                     ": " + C_XFLActions::XFLProtocolErrorToText(s32_Return, GetLastXFLError());
            TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return -1;
         }
      }
   }

   u32_EraseTime = TGL_GetTickCount() - u32_OldTimeTotal;
   c_Text.PrintFormatted("%s %d s", TGL_LoadStr(STR_FDL_ERASE_TIME).c_str(), u32_EraseTime / 1000);
   m_ReportVerboseStatus(c_Text.c_str());

   s32_Return = m_WriteStartFingerPrint(orc_FingerPrintIndexes);
   if (s32_Return != C_NO_ERR)
   {
      return -1;
   }

   //enter programming mode
   s32_Return = ProgFlash();
   if (s32_Return != C_NO_ERR)
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ENTER_PROG_MODE), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return -1;
   }

   {
      //get file information:
      sint32 s32_Size;

      s32_Size = TGL_FileSize(orc_Params.c_HexFile);
      if (s32_Size != -1)
      {
         m_ReportVerboseStatus(TGL_LoadStr(STR_FILESIZE) + " " + C_SCLString::IntToStr(s32_Size) +
                               TGL_LoadStr(STR_FDL_BYTES));
         TGL_FileAgeString(orc_Params.c_HexFile, c_Text);
         m_ReportVerboseStatus(TGL_LoadStr(STR_TIMESTAMP) + " " + c_Text);
      }
   }
   TRG_ReportStatus(TGL_LoadStr(STR_FM_WRITING), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   m_ReportVerboseStatus("");

   pu8_Line = c_HexFile.LineInit();
   if (pu8_Line != NULL)
   {
      u32_NumLinesTotal = c_HexFile.LineCount();
      u32_NumBytesTotal = c_HexFile.ByteCount();

      TRG_ReportProgress(0U, TGL_LoadStr(STR_FDL_TXT_WR_FLASH_DO));
      u32_OldTimeTotal = TGL_GetTickCount();

      u32_OldTime = TGL_GetTickCount();
      u32_NumSentProgress = 0U;

      for (;;)
      {
         //send 5 lines per call; bigger blocks might result in the callbacks not executed for a long time with slow
         // communication bitrates
         u16_NumSentNow = 5U;
         if (ou16_ProtocolVersion >= mu16_PROTOCOL_VERSION_3_00)
         {
            //pass erase timeout read with ::ReadFlashInformation
            //be a little conservative here: use the timeout required for the slowest flash IC
            s32_Return = SendMultiHexLine(u16_NumSentNow, orc_Params.u16_InterFrameDelayUs, u32_NumBytes, c_HexFile,
                                          t_FlashInfo.GetWriteTimeOfSlowestIC());
         }
         else
         {
            //use default timeout
            s32_Return = SendMultiHexLine(u16_NumSentNow, orc_Params.u16_InterFrameDelayUs, u32_NumBytes, c_HexFile);
         }
         u32_LineCount += u16_NumSentNow;
         switch (s32_Return)
         {
         case 1: //1 -> EOF reached
         case C_NO_ERR:
            c_Text = "";
            break;
         case C_CHECKSUM:
            c_Text.PrintFormatted("%s %d)", TGL_LoadStr(STR_FDL_ERR_WRITE_CHECKSUM).c_str(), u32_LineCount);
            break;
         default:
            c_Text = (TGL_LoadStr(STR_FDL_ERR_WR_FLASH_DO_PT1) +
                      C_XFLActions::XFLProtocolErrorToText(s32_Return, GetLastXFLError()) +
                      TGL_LoadStr(STR_FDL_ERR_WR_FLASH_DO_PT2) + C_SCLString::IntToStr(u32_LineCount)).c_str();
            break;
         }
         if (c_Text != "")
         {
            TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return -1;
         }

         //report progress as text:
         u32_NumBytesFlashed += u32_NumBytes;
         u32_NumSentProgress += u32_NumBytes;

         //do not update too often; might cause flickering in the application
         if (((TGL_GetTickCount() - u32_OldTime) > 750U) || (u32_LineCount == u32_NumLinesTotal))
         {
            const uint32 u32_Diff = TGL_GetTickCount() - u32_OldTime;

            c_Text.PrintFormatted("%s %d / %d", TGL_LoadStr(STR_FM_LINE).c_str(), u32_LineCount, u32_NumLinesTotal);
            TRG_ReportStatus("<<<CLRLINE", gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
            m_ReportVerboseStatus("<<<CLRLINE");
            TRG_ReportStatus(c_Text, gu8_DL_REPORT_STATUS_TYPE_INFORMATION);

            if (u32_Diff != 0U)
            {
               f32_Rate = ((((static_cast<float32>(u32_NumSentProgress)) / 1024.0F) /
                            (static_cast<float32>(u32_Diff))) * 1000.0F);
               c_Text.PrintFormatted("%s %d / %d at %2.2fkB/s", TGL_LoadStr(STR_FDL_BYTE).c_str(), u32_NumBytesFlashed,
                                     u32_NumBytesTotal, static_cast<float64>(f32_Rate));
            }
            else
            {
               c_Text.PrintFormatted("%s %d / %d", TGL_LoadStr(STR_FDL_BYTE).c_str(), u32_NumBytesFlashed,
                                     u32_NumBytesTotal);
            }
            m_ReportVerboseStatus(c_Text);

            u32_NumSentProgress = 0U;
            u32_OldTime = TGL_GetTickCount();
         }

         //report progress for e.g. progress bar update:
         s32_Return2 = TRG_ReportProgress(static_cast<uint16>((u32_LineCount * 1000U) / u32_NumLinesTotal),
                                          TGL_LoadStr(STR_FDL_TXT_WR_FLASH_DO));
         if (s32_Return2 != C_NO_ERR)
         {
            TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_USER_ABORT), gu8_DL_REPORT_STATUS_TYPE_ERROR);
            return -1;
         }
         if (s32_Return == 1)
         {
            break;
         }
      }

      u32_FlashTime = TGL_GetTickCount() - u32_OldTimeTotal;
      c_Text.PrintFormatted("%s %d s", TGL_LoadStr(STR_FDL_DOWNLOAD_TIME).c_str(), u32_FlashTime / 1000U);
      m_ReportVerboseStatus(c_Text);
      if (u32_FlashTime == 0U)
      {
         u32_FlashTime = 1U;
      }
      f32_Rate = (((static_cast<float32>(u32_NumBytesTotal) / 1024.0F) /
                   (static_cast<float32>(u32_FlashTime))) * 1000.0F);
      c_Text.PrintFormatted("%s %2.2fkB/s", TGL_LoadStr(STR_FDL_AVERAGE_RATE).c_str(), static_cast<float64>(f32_Rate));
      m_ReportVerboseStatus(c_Text);

      TRG_ReportProgress(1000U, TGL_LoadStr(STR_FDL_TXT_WR_FLASH_DO));
   }
   else
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_UNKNOWN_ERR), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return -1;
   }

   //calculate checksum over hex_file and write
   //result not critical -> just warning generated in sub-function
   (void)m_WriteFingerPrintCheckSum(orc_FingerPrintIndexes.q_ApplicationCheckSum, u32_FileChecksum);
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------

void C_XFLFlashWrite::m_ReportVerboseStatus(const C_SCLString & orc_StatusText)
{
   if (mq_VerboseMode == true)
   {
      TRG_ReportStatus(orc_StatusText, gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
   }
}

//----------------------------------------------------------------------------------------------------------------------

sint32 C_XFLFlashWrite::m_WriteFingerPrintCheckSum(const bool oq_FingerPrintAvailable, const uint32 ou32_Checksum)
{
   sint32 s32_Return;

   if (oq_FingerPrintAvailable == false)
   {
      return C_NO_ERR; //nothing to do ...
   }

   s32_Return = this->SetFingerPrintChecksum(ou32_Checksum);
   if (s32_Return != C_NO_ERR)
   {
      this->TRG_ReportStatus(TGL_LoadStr(STR_FDL_WARN_COULD_NOT_WR_APPL_CHK), gu8_DL_REPORT_STATUS_TYPE_WARNING);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   extract device ID from hex-file

   Scan through the hex-file and try to find the "device_info" structure.
   Then extract the device ID from it and return it.
   If the device_info structure with the device ID information is in the hexfile more than once, AND
    the device ID in at least one of the copies is different, we fail.

   \param[in]     ou32_StartAddress                address offset to start searching at
   \param[in]     oq_FailIfNotFoundAtStartAddress  true -> the ID must be EXACTLY at start address
   \param[in]     orc_HexFile                      hexfile potentially containing the device-ID
   \param[out]    orc_DeviceID                     device ID found in hex-file

   \return
   C_NO_ERR     everything OK (device ID in oc_DeviceID)
   C_NOACT      device-ID not found
   C_CONFIG     ambiguous device-IDs in hex-file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::m_ScanDeviceIDFromHexFile(const uint32 ou32_StartAddress,
                                                  const bool oq_FailIfNotFoundAtStartAddress,
                                                  C_XFLHexFile & orc_HexFile, C_SCLString & orc_DeviceID) const
{
   sint32 s32_Return;
   sint32 s32_Index;
   C_SCLString c_DeviceID = "";
   C_SCLString c_Help;

   SCLDynamicArray<C_XFLECUInformation> ac_InfoBlocks;
   s32_Return = orc_HexFile.GetECUInformationBlocks(ac_InfoBlocks, ou32_StartAddress, oq_FailIfNotFoundAtStartAddress,
                                                    oq_FailIfNotFoundAtStartAddress, true);
   if ((s32_Return != C_NO_ERR) || (ac_InfoBlocks.GetLength() == 0))
   {
      if ((oq_FailIfNotFoundAtStartAddress == false) && (ou32_StartAddress > 0U))
      {
         //retry from beginning:
         s32_Return = orc_HexFile.GetECUInformationBlocks(ac_InfoBlocks, 0x00000000UL, false, false, true);
         if ((s32_Return != C_NO_ERR) || (ac_InfoBlocks.GetLength() == 0))
         {
            return C_NOACT;
         }
      }
      else
      {
         return C_NOACT; //not even one block found :-(
      }
   }

   for (s32_Index = 0; s32_Index < ac_InfoBlocks.GetLength(); s32_Index++)
   {
      if (ac_InfoBlocks[s32_Index].ContainsDeviceID() == true)
      {
         c_Help = ac_InfoBlocks[s32_Index].GetDeviceID();
         if (c_DeviceID == "") //no device-ID seen yet ...
         {
            c_DeviceID = c_Help;
         }
         else
         {
            if (c_DeviceID != c_Help)
            {
               return C_CONFIG; //ambiguous device IDs !
            }
         }
      }
   }
   if (c_DeviceID != "")
   {
      orc_DeviceID = c_DeviceID;
      return C_NO_ERR;
   }
   return C_NOACT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   perform device ID x-check between hex-file and server's device ID

   Depending on configuration settings check whether the specified device ID matches
    the one in the hex-file.

   \param[in]     orc_HexFile           hexfile potentially containing the device-ID
   \param[in]     orc_DeviceID          device ID we expect to find in the hex-file
   \param[in]     orc_Params            flashloader parameters
   \param[in]     ou16_ProtocolVersion  protocol version as reported by server

   \return
   C_NO_ERR     everything OK (either device ID matches, override by user, not configured to be checked)
   C_CHECKSUM   device-ID does not match and no override (or other problems, cf. comments at parameter enums) -> fail
   C_CONFIG     at least one of the parameters is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::m_DoDeviceIDXCheck(C_XFLHexFile & orc_HexFile, const C_SCLString & orc_DeviceID,
                                           const C_XFLFlashWriteParameters & orc_Params,
                                           const uint16 ou16_ProtocolVersion)
{
   SCLDynamicArray<uint32> c_StartAddresses;
   uint32 u32_StartAddress = 0U;
   bool q_ExactAddressRequired;
   sint32 s32_Return;
   uint32 u32_Dummy;
   C_SCLString c_DeviceIDHexFile;
   C_SCLString c_Error;

   if ((orc_Params.e_DevTypeCheck > eXFL_DEV_TYPE_CHECK_GET_ADDRESS_THEN_FAIL) ||
       (orc_Params.e_DevTypeCheckGetIDFailedReaction > eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_ASK) ||
       (orc_Params.e_DevTypeCheckMatchIDFailedReaction > eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_ASK))
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_CFG_DIC), gu8_DL_REPORT_STATUS_TYPE_ERROR);
      return C_CONFIG;
   }

   switch (orc_Params.e_DevTypeCheck)
   {
   case eXFL_DEV_TYPE_CHECK_NONE:
      m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_ID_CHECK_PASSED));
      return C_NO_ERR; //finished here
   case eXFL_DEV_TYPE_CHECK_SCAN_HEX_FILE:
      m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_ID_OF_HEX_FILE));
      q_ExactAddressRequired = false;
      s32_Return = C_NO_ERR;
      break;
   case eXFL_DEV_TYPE_CHECK_GET_ADDRESS_THEN_SCAN:
      m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_ID_CHECKING));
      if (mt_AvailableServices.q_GetDeviceInfoAddress == true)
      {
         s32_Return = GetDeviceInfoAddresses(c_StartAddresses, ou16_ProtocolVersion);
         if ((s32_Return != C_NO_ERR) || (c_StartAddresses.GetLength() == 0))
         {
            m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_ID_CHECK_NODE_HEX));
         }
         else
         {
            //first one is used for this purpose:
            u32_StartAddress = c_StartAddresses[0];
         }
      }
      else
      {
         m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_ID_CHECK_ADDR_NA));
      }
      q_ExactAddressRequired = false;
      s32_Return = C_NO_ERR;
      break;
   case eXFL_DEV_TYPE_CHECK_GET_ADDRESS_THEN_FAIL:
      m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_TXT_ID_CHECKING));
      if (mt_AvailableServices.q_GetDeviceInfoAddress == true)
      {
         s32_Return = GetDeviceInfoAddresses(c_StartAddresses, ou16_ProtocolVersion);
         if ((s32_Return != C_NO_ERR) || (c_StartAddresses.GetLength() == 0))
         {
            TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_RD_NODE),
                             gu8_DL_REPORT_STATUS_TYPE_ERROR);
            s32_Return = C_CHECKSUM;
         }
         else
         {
            //first one is used for this purpose:
            u32_StartAddress = c_StartAddresses[0];
         }
      }
      else
      {
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_ID_ADDR_NA), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         s32_Return = C_CHECKSUM;
      }
      q_ExactAddressRequired = true;
      break;
   default:
      return C_CONFIG;
   }

   //get data from hex-file:
   if (s32_Return == C_NO_ERR) //maybe there was already an error getting the address with the ...._THEN_FAIL option
   {
      s32_Return = this->m_ScanDeviceIDFromHexFile(u32_StartAddress, q_ExactAddressRequired, orc_HexFile,
                                                   c_DeviceIDHexFile);
      switch (s32_Return)
      {
      case C_NO_ERR:
         break;
      case C_NOACT:
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_ID_NA_HEX), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         break;
      case C_CONFIG:
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_ID_AMBIG_HEX), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         break;
      default:
         TRG_ReportStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_ID_UNDEF), gu8_DL_REPORT_STATUS_TYPE_ERROR);
         break;
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      switch (orc_Params.e_DevTypeCheckGetIDFailedReaction)
      {
      case eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_IGNORE:
         m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_IGNOR));
         s32_Return = C_NO_ERR; //no check, as we don't have a device id ...
         break;
      case eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_FAIL:
         s32_Return =  C_CHECKSUM;
         break;
      case eXFL_DEV_TYPE_CHECK_GETID_FAIL_REACTION_ASK:
         s32_Return = TRG_UserInteraction(eXFL_USER_INTERACTION_REASON_GETID_FAIL,
                                          TGL_LoadStr(STR_FDL_TXT_ID_NA_CHECK_HEX_FILE), u32_Dummy);
         if (s32_Return != C_NO_ERR)
         {
            s32_Return = C_CONFIG;
         }
         break;
      default:
         s32_Return =  C_CONFIG;
         break;
      }
      return s32_Return;
   }

   //finally do the comparison:
   if (c_DeviceIDHexFile.Trim() == orc_DeviceID.Trim())
   {
      TRG_ReportStatus(TGL_LoadStr(STR_FDL_ID_CHECK_MATCH), gu8_DL_REPORT_STATUS_TYPE_INFORMATION);
      return C_NO_ERR;
   }

   c_Error = TGL_LoadStr(STR_FDL_ERR_ID_CHECK_MISMATCH_PT1) + c_DeviceIDHexFile.Trim() +
             TGL_LoadStr(STR_FDL_ERR_ID_CHECK_MISMATCH_PT2) + orc_DeviceID.Trim() + "\"";
   TRG_ReportStatus(c_Error, gu8_DL_REPORT_STATUS_TYPE_ERROR);

   switch (orc_Params.e_DevTypeCheckMatchIDFailedReaction)
   {
   case eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_IGNORE:
      m_ReportVerboseStatus(TGL_LoadStr(STR_FDL_ERR_ID_CHECK_IGNOR));
      s32_Return = C_NO_ERR;
      break;
   case eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_FAIL:
      s32_Return = C_CHECKSUM;
      break;
   case eXFL_DEV_TYPE_CHECK_MATCHID_FAIL_REACTION_ASK:
      s32_Return = TRG_UserInteraction(eXFL_USER_INTERACTION_REASON_MATCHID_FAIL, c_Error, u32_Dummy);
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_CONFIG;
      }
      break;
   default:
      s32_Return = C_CONFIG;
      break;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

C_XFLFlashWrite::~C_XFLFlashWrite(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default implementation of user interaction interface

   Default: keep empty and warn.
   Derived implementation should return C_NO_ERR to continue the ongoing process.

   \param[in]     oe_Reason               reason for user interaction
   \param[in]     orc_MessageText         message text to report
   \param[in]     oru32_AdditionalValue   additional information to report

   \return
   C_CONFIG     missing implementation in derived class -> abort process
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_XFLFlashWrite::TRG_UserInteraction(const E_XFLUserInteractionReason oe_Reason,
                                            const C_SCLString & orc_MessageText, uint32 & oru32_AdditionalValue)
{
   (void)oe_Reason;
   (void)orc_MessageText;
   (void)oru32_AdditionalValue;
   TRG_ReportStatus(
      "CXFLDownload configuration error: Configuration requires user interaction, but TRG_UserInteraction is not overloaded !",
      gu8_DL_REPORT_STATUS_TYPE_ERROR);
   return C_CONFIG;
}

//----------------------------------------------------------------------------------------------------------------------
