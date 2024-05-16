//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Sequences for basic system update.

   Provides functions to perform a very basic flash sequence: one HEX file for one openSYDE device via CAN

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.h"

#include "TglTime.hpp"
#include "C_SclDateTime.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscProtocolDriverOsyTpBase.hpp"
#include "C_OscUpdateUtil.hpp"
#include "C_OscBuSequences.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscBuSequences::C_OscBuSequences(void) :
   ms32_CanBitrate(125)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscBuSequences::~C_OscBuSequences()
{
   try
   {
      mc_TpCan.SetDispatcher(NULL);
   }
   catch (...)
   {
   }
   try
   {
      if (mc_CanDispatcher.DLL_Close() == C_NO_ERR)
      {
         osc_write_log_info("Teardown", "CAN DLL closed.");
      }
      else
      {
         osc_write_log_info("Teardown", "Failed to close CAN DLL.");
      }
   }
   catch (...)
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize transport protocol, openSYDE protocol driver and CAN with given parameters.

   \param[in]  orc_CanDllPath    Path to CAN DLL file
   \param[in]  os32_CanBitrate   CAN Bitrate in kBit/s
   \param[in]  ou8_NodeId        Server node ID

   \return
   C_NO_ERR    everything ok
   else        error occured, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscBuSequences::Init(const C_SclString & orc_CanDllPath, const int32_t os32_CanBitrate,
                               const uint8_t ou8_NodeId)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_LogActivity = "Initialization";

   m_ReportProgress(s32_Return, "Starting the initialization of CAN driver and protocol ...");

   const uint8_t u8_MINIMUM_PERCENTAGE = 0;
   m_ReportProgressPercentage(u8_MINIMUM_PERCENTAGE);

   osc_write_log_info(c_LogActivity, "CAN DLL path used: " + orc_CanDllPath);

   ms32_CanBitrate = os32_CanBitrate;

   mc_CanDispatcher.SetDLLName(orc_CanDllPath);
   s32_Return = mc_CanDispatcher.DLL_Open();
   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info(c_LogActivity, "CAN DLL loaded.");
      s32_Return = mc_CanDispatcher.CAN_Init(ms32_CanBitrate);
      if (s32_Return == C_NO_ERR)
      {
         osc_write_log_info(c_LogActivity, "CAN interface initialized.");
      }
      else
      {
         osc_write_log_error(c_LogActivity, "Could not initialize the CAN interface!");
      }
   }
   else
   {
      osc_write_log_error(c_LogActivity, "Could not load the CAN DLL!");
   }

   if (s32_Return == C_NO_ERR)
   {
      s32_Return = mc_TpCan.SetDispatcher(&mc_CanDispatcher);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity, "Setting CAN dispatcher for CAN transport protocol failed!");
      }
      else
      {
         s32_Return = mc_OsyProtocol.SetTransportProtocol(&mc_TpCan);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error(c_LogActivity,
                                "Setting CAN transport protocol to the openSYDE protocol driver failed!");
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      C_OscProtocolDriverOsyNode c_Client;
      C_OscProtocolDriverOsyNode c_Server;
      c_Client.u8_NodeIdentifier = 126;
      c_Client.u8_BusIdentifier = 0U;
      c_Server.u8_NodeIdentifier = ou8_NodeId;
      c_Server.u8_BusIdentifier = 0U;

      s32_Return = mc_OsyProtocol.SetNodeIdentifiers(c_Client, c_Server);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity, "Could not configure the node IDs! Is the server node ID within range?");
      }
   }

   m_ReportProgress(s32_Return, "Initialization finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activate flashloader of device.

   Use the request programming flag, but if it does not work go on to allow a manual reset of the device.
   Enter preprogramming session via broadcast for at least the value of u32_SCAN_TIME_MS.

   \param[in]  ou32_FlashloaderResetWaitTime    Flashloader reset wait time

   \return
   C_NO_ERR    flashloader activated
   else        error occured
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscBuSequences::ActivateFlashLoader(const uint32_t ou32_FlashloaderResetWaitTime)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_LogActivity = "Activate Flashloader";
   uint8_t u8_NumberCode = 0;
   const uint32_t u32_SCAN_TIME_MS = 5000U;
   uint32_t u32_WaitTime = ou32_FlashloaderResetWaitTime;

   m_ReportProgress(s32_Return, "Starting the flashloader activation ...");

   if (u32_WaitTime < u32_SCAN_TIME_MS)
   {
      // The scan time is necessary for the manual triggering of the nodes
      u32_WaitTime = u32_SCAN_TIME_MS;
   }

   s32_Return = mc_OsyProtocol.OsyRequestProgramming(&u8_NumberCode);
   if (s32_Return != C_NO_ERR)
   {
      //not a showstopper; user can still use the "manual reset" approach
      C_SclString c_Text;
      c_Text.PrintFormatted("Could not set the \"request programming\" flag: Failed with result %d. "
                            "You still have the chance to reset the device manually ...", s32_Return);
      m_ReportProgress(C_WARN, c_Text);
   }
   else
   {
      osc_write_log_info(c_LogActivity, "Succesfully set the \"request programming\" flag.");
   }

   //request "ResetToFlashloader"
   s32_Return = mc_OsyProtocol.OsyEcuReset(C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);
   if (s32_Return != C_NO_ERR)
   {
      //also not a showstopper; user can still use the "manual reset" approach
      C_SclString c_Text;
      osc_write_log_warning(c_LogActivity, "Could not request an ECU reset.");

      c_Text.PrintFormatted("You now have %d seconds time to turn on your target device ...",
                            u32_SCAN_TIME_MS / 1000);
      m_ReportProgress(C_WARN, c_Text);
   }
   else
   {
      osc_write_log_info(c_LogActivity, "ECU reset successful.");
   }

   // Always continue with broadcast. If previous steps did not work, user can do the manual reset while we broadcast.
   const uint32_t u32_StartTime = stw::tgl::TglGetTickCount();

   do
   {
      // openSYDE "DiagnosticSessionControl(PreProgramming)" broadcast
      s32_Return = mc_TpCan.BroadcastSendEnterPreProgrammingSession();
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity,
                             "Sending broadcast to enter preprogramming session failed with result " +
                             C_SclString::IntToStr(s32_Return));

         s32_Return = C_COM;
      }

      if (s32_Return != C_NO_ERR)
      {
         break;
      }

      TglSleep(5);
   }
   while (TglGetTickCount() < (u32_WaitTime + u32_StartTime));

   //Previous broadcasts might have caused responses placed in the receive queues of the device
   // specific driver instances. Dump them.
   (void)mc_CanDispatcher.DispatchIncoming();
   mc_TpCan.ClearDispatcherQueue();

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error(c_LogActivity, "Could not connect to the target device! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }

   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info(c_LogActivity, "Connection to target device established. Flashloader activated.");
   }

   m_ReportProgress(s32_Return, "Flashloader activation finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read device information

   \return
   C_NO_ERR    flashloader activated
   else        error occured
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscBuSequences::ReadDeviceInformation(void)
{
   int32_t s32_Return = C_NO_ERR;
   uint8_t u8_NumberCode;
   C_SclString c_DeviceName;
   C_OscComFlashloaderInformation c_Info;

   m_ReportProgress(s32_Return, "Starting to read the device information...");

   s32_Return = mc_OsyProtocol.OsyReadHardwareNumber(c_Info.u32_EcuArticleNumber, &u8_NumberCode);

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Article Number", "Could not read the device's article number! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadHardwareVersionNumber(c_Info.c_EcuHardwareVersionNumber, &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Hardware Version", "Could not read the device's hardware version number! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadDeviceName(c_DeviceName, &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Device Name", "Could not read the device's device name! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadProtocolVersion(c_Info.au8_ProtocolVersion, &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Protocol Version", "Could not read the device's protocol version! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadBootSoftwareIdentification(c_Info.au8_FlashloaderSoftwareVersion,
                                                                    &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Flashloader Versions",
                          "Could not read the device's flashloader implementation version! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadFlashloaderProtocolVersion(c_Info.au8_FlashloaderProtocolVersion,
                                                                    &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Flashloader Versions",
                          "Could not read the device's flashloader protocol version! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadFlashCount(c_Info.u32_FlashCount, &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Flash Count", "Could not read number of times the device has been flashed! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadApplicationSoftwareFingerprint(c_Info.au8_FlashFingerprintDate,
                                                                        c_Info.au8_FlashFingerprintTime,
                                                                        c_Info.c_FlashFingerprintUserName,
                                                                        &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Fingerprint",
                          "Could not read the device's application software fingerprint! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      s32_Return = mc_OsyProtocol.OsyReadListOfFeatures(c_Info.c_AvailableFeatures, &u8_NumberCode);
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("Read Features",
                          "Could not read the device's list of available features! Details: " +
                          C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
   }
   else
   {
      if (c_Info.c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == true)
      {
         s32_Return = mc_OsyProtocol.OsyReadMaxNumberOfBlockLength(c_Info.u16_MaxNumberOfBlockLength, &u8_NumberCode);
      }
      if (s32_Return != C_NO_ERR)
      {
         c_Info.u16_MaxNumberOfBlockLength = 0U;
         osc_write_log_error("Read MaxNumberOfBlockLength",
                             "Could not read the device's maximum number of block length! Details: " +
                             C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
      }
   }

   // read serial number after reading available features!
   if (s32_Return == C_NO_ERR)
   {
      if (c_Info.c_AvailableFeatures.q_ExtendedSerialNumberModeImplemented == false)
      {
         s32_Return = mc_OsyProtocol.OsyReadEcuSerialNumber(c_Info.c_SerialNumber, &u8_NumberCode);
      }
      else
      {
         s32_Return = mc_OsyProtocol.OsyReadEcuSerialNumberExt(c_Info.c_SerialNumber, &u8_NumberCode);
      }

      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Read Serial Number", "Could not read the device's serial number! Details: " +
                             C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
      }
   }

   m_ReportProgress(s32_Return, "Read device information finished.");
   const uint8_t u8_PROGRESS_PERCENTAGE = 2;
   m_ReportProgressPercentage(u8_PROGRESS_PERCENTAGE);

   if (s32_Return == C_NO_ERR)
   {
      m_ReportFlashloaderInformationRead(c_DeviceName, c_Info);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update node

   Read HEX file and send data to the device.

   \param[in]  orc_HexFilePath               Path to HEX file
   \param[in]  ou32_RequestDownloadTimeout   Maximum time in ms it can take to erase one continuous area in flash
   \param[in]  ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to flash

   \return
   C_NO_ERR    flashloader activated
   else        error occured
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscBuSequences::UpdateNode(const C_SclString & orc_HexFilePath, const uint32_t ou32_RequestDownloadTimeout,
                                     const uint32_t ou32_TransferDataTimeout)
{
   int32_t s32_Return = C_NO_ERR;
   C_SclString c_LogActivity;
   uint8_t u8_NumberCode;
   C_OscHexFile c_HexFile;
   uint32_t u32_SignatureBlockAddress = 0;
   const stw::hex_file::C_HexDataDump * pc_HexDump = NULL;

   m_ReportProgress(s32_Return, "Starting node update...");

   s32_Return = h_ReadHexFile(orc_HexFilePath, c_HexFile, u32_SignatureBlockAddress);

   if (s32_Return == C_NO_ERR)
   {
      uint32_t u32_Return;
      c_LogActivity = "Read HEX File";
      pc_HexDump = c_HexFile.GetDataDump(u32_Return);
      if (u32_Return != stw::hex_file::NO_ERR)
      {
         s32_Return = C_RD_WR;
         osc_write_log_error(c_LogActivity, "Could not get the HEX file split into handy chunks!");
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      c_LogActivity = "Programming Session";
      osc_write_log_info(c_LogActivity, "Activating programming session ...");

      //try to enter programming session
      s32_Return =
         mc_OsyProtocol.OsyDiagnosticSessionControl(C_OscProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PROGRAMMING,
                                                    &u8_NumberCode);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity, "Could not activate the programming session! Details: " +
                             C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //set security level 3; we want to flash ...
      const uint8_t u8_SECURITY_LEVEL = 3U;
      bool q_SecureMode;
      uint64_t u64_Seed;
      uint8_t ou8_SecurityAlgorithm;

      c_LogActivity = "Security Access";
      s32_Return = mc_OsyProtocol.OsySecurityAccessRequestSeed(u8_SECURITY_LEVEL, q_SecureMode, u64_Seed,
                                                               ou8_SecurityAlgorithm, &u8_NumberCode);

      if ((s32_Return == C_WARN) &&
          (u8_NumberCode == C_OscProtocolDriverOsy::hu8_NR_CODE_REQUIRED_TIME_DELAY_NOT_EXPIRED))
      {
         // Special case: The server must wait for a second to allow a request after a reset.
         // In some scenarios the normal waiting times after resets are not enough (this times are not calculated for
         // this security scenario), so wait in this case for another second and try again
         osc_write_log_warning(c_LogActivity, "The node was not ready for the Security Access request. Trying"
                               " after waiting for another second again.");

         stw::tgl::TglSleep(1000);

         s32_Return = mc_OsyProtocol.OsySecurityAccessRequestSeed(u8_SECURITY_LEVEL, q_SecureMode, u64_Seed,
                                                                  ou8_SecurityAlgorithm, &u8_NumberCode);
      }

      if (q_SecureMode == true)
      {
         osc_write_log_error(c_LogActivity, "Security request returned security is on. "
                             "No security support here. Use openSYDE GUI tool for this feature.");
         s32_Return = C_CONFIG;
      }
      else if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity, "Did not get a security seed from the target device! Details: " +
                             C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
      }
      else
      {
         //hard coded keys are the best keys
         const uint32_t u32_KEY = 23U; // fixed in UDS stack for non secure mode

         if (u64_Seed != 42U)
         {
            C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Received seed in non secure mode does not match the "
                                 "expected value, expected: 42, got %i", u64_Seed);
            osc_write_log_warning(c_LogActivity, c_Tmp.c_str());
         }

         s32_Return = mc_OsyProtocol.OsySecurityAccessSendKey(3U, u32_KEY, &u8_NumberCode);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error(c_LogActivity, "The target device did not access the security key! Details: " +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
         }
      }
   }

   if ((s32_Return == C_NO_ERR) && (pc_HexDump != NULL))
   {
      c_LogActivity = "Check Flash Memory";
      osc_write_log_info(c_LogActivity, "Session and security access set up.");
      osc_write_log_info(c_LogActivity, "Checking Flash memory availability ...");

      //do we have enough space for the hex file data ?
      for (uint16_t u16_Area = 0U; u16_Area < pc_HexDump->at_Blocks.GetLength(); u16_Area++)
      {
         s32_Return = mc_OsyProtocol.OsyCheckFlashMemoryAvailable(
            pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
            pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength(),
            &u8_NumberCode);
         if (s32_Return != C_NO_ERR)
         {
            C_SclString c_Error;
            c_Error.PrintFormatted("(Offset: 0x%08x Size: 0x%08x)", pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                                   pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength());
            osc_write_log_error(c_LogActivity,  "Could not get confirmation about flash memory availability " +
                                c_Error + "! Details: " +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //write fingerprint
      const C_SclDateTime c_Now = C_SclDateTime::Now();
      uint8_t au8_Date[3];
      uint8_t au8_Time[3];
      C_SclString c_UserName;
      bool q_Return;
      au8_Date[0] = static_cast<uint8_t>(c_Now.mu16_Year % 1000U);
      au8_Date[1] = static_cast<uint8_t>(c_Now.mu16_Month);
      au8_Date[2] = static_cast<uint8_t>(c_Now.mu16_Day);
      au8_Time[0] = static_cast<uint8_t>(c_Now.mu16_Hour);
      au8_Time[1] = static_cast<uint8_t>(c_Now.mu16_Minute);
      au8_Time[2] = static_cast<uint8_t>(c_Now.mu16_Second);

      c_LogActivity = "Write Fingerprint";
      osc_write_log_info(c_LogActivity, "Writing fingerprint ...");

      q_Return = TglGetSystemUserName(c_UserName);
      if (q_Return != true)
      {
         osc_write_log_warning(c_LogActivity, "Could not get the name of the current Windows user!");
         c_UserName = "unknown";
      }

      s32_Return =
         mc_OsyProtocol.OsyWriteApplicationSoftwareFingerprint(au8_Date, au8_Time, c_UserName, &u8_NumberCode);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity, "Could not set the SW fingerprint! Details: " +
                             C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
      }
   }

   if ((s32_Return == C_NO_ERR) && (pc_HexDump != NULL))
   {
      bool q_ErrorOccurred = false;
      c_LogActivity = "Flash";

      //flash all areas
      for (uint16_t u16_Area = 0U; u16_Area < pc_HexDump->at_Blocks.GetLength(); u16_Area++)
      {
         const int32_t s32_Size = pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength();
         uint32_t u32_MaxBlockLength;

         osc_write_log_info(c_LogActivity, "Preparing flashing ...");

         //set a proper timeout
         mc_OsyProtocol.SetTimeoutPolling(ou32_RequestDownloadTimeout);

         s32_Return = mc_OsyProtocol.OsyRequestDownload(pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                                                        pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength(),
                                                        u32_MaxBlockLength, &u8_NumberCode);
         if (s32_Return != C_NO_ERR)
         {
            C_SclString c_Error;
            c_Error.PrintFormatted("(Offset: 0x%08X Size: 0x%08X)", pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                                   pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength());
            osc_write_log_error(c_LogActivity, "Could not request download " + c_Error + "! Details: " +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
            q_ErrorOccurred = true;
         }
         else
         {
            //perform the actual transfer
            uint8_t u8_BlockSequenceCounter = 1U;
            int32_t s32_RemainingBytes = s32_Size;
            std::vector<uint8_t> c_Data;

            //set a proper timeout
            const uint32_t u32_AdaptedTimeout =
               C_OscUpdateUtil::h_GetAdaptedTransferDataTimeout(ou32_TransferDataTimeout, u32_MaxBlockLength,
                                                                ms32_CanBitrate);
            mc_OsyProtocol.SetTimeoutPolling(u32_AdaptedTimeout);

            while (s32_RemainingBytes > 0)
            {
               if (s32_RemainingBytes > static_cast<int32_t>(u32_MaxBlockLength))
               {
                  c_Data.resize(static_cast<int32_t>(u32_MaxBlockLength - 5U));
               }
               else
               {
                  c_Data.resize(s32_RemainingBytes);
               }

               (void)memcpy(
                  &c_Data[0],
                  &pc_HexDump->at_Blocks[u16_Area].au8_Data[s32_Size - s32_RemainingBytes],
                  c_Data.size());

               s32_Return = mc_OsyProtocol.OsyTransferData(u8_BlockSequenceCounter, c_Data, &u8_NumberCode);
               if (s32_Return == C_NO_ERR)
               {
                  C_SclString c_Text;
                  c_Text.PrintFormatted("Transferring area %02d/%02d  byte %08d/%08d",
                                        u16_Area + 1, pc_HexDump->at_Blocks.GetLength(),
                                        s32_Size - s32_RemainingBytes, s32_Size);
                  m_ReportProgress(s32_Return, c_Text);

                  const uint8_t u8_MAX_PERCENTAGE = 100;
                  const uint8_t u8_Percentage =
                     static_cast<uint8_t>((u8_MAX_PERCENTAGE * (s32_Size - s32_RemainingBytes)) / s32_Size);
                  m_ReportProgressPercentage(u8_Percentage);

                  s32_RemainingBytes -= static_cast<int32_t>(c_Data.size());
                  u8_BlockSequenceCounter = (u8_BlockSequenceCounter < 0xFFU) ? (u8_BlockSequenceCounter + 1U) : 0x00U;
               }
               else
               {
                  osc_write_log_error(c_LogActivity, "Could not transfer data. Details: " +
                                      C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                               u8_NumberCode));
                  break;
               }
            }
         }

         // reset timeout
         mc_OsyProtocol.ResetTimeoutPolling();

         if (s32_Return == C_NO_ERR)
         {
            C_SclString c_Text;
            c_Text.PrintFormatted("Transferring area %02d/%02d  byte %08d/%08d",
                                  u16_Area + 1, pc_HexDump->at_Blocks.GetLength(), s32_Size, s32_Size);

            const uint8_t u8_MAX_PERCENTAGE = 100;
            m_ReportProgressPercentage(u8_MAX_PERCENTAGE);

            m_ReportProgress(s32_Return, c_Text);
            m_ReportProgress(s32_Return, "Finished writing area, finalizing ...");

            if (u16_Area == (pc_HexDump->at_Blocks.GetLength() - 1))
            {
               osc_write_log_info(c_LogActivity,
                                  "This is the last area, we'll better check the signature as well ...");
               s32_Return = mc_OsyProtocol.OsyRequestTransferExitAddressBased(true, u32_SignatureBlockAddress,
                                                                              &u8_NumberCode);
            }
            else
            {
               s32_Return = mc_OsyProtocol.OsyRequestTransferExitAddressBased(false, 0U, &u8_NumberCode);
            }

            if (s32_Return != C_NO_ERR)
            {
               osc_write_log_error(c_LogActivity, "Could not finalize the area! Details: " +
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
               q_ErrorOccurred = true;
            }
         }

         if (q_ErrorOccurred == true)
         {
            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      c_LogActivity = "Update Node";
      osc_write_log_info(c_LogActivity, "Finished writing all data.");
   }

   m_ReportProgress(s32_Return, "Node update finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reset system

   \return
   C_NO_ERR   system reset
   C_NOACT    could not put request in Tx queue ...
   C_CONFIG   no transport protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscBuSequences::ResetSystem(void)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_LogActivity = "Reset System";

   m_ReportProgress(s32_Return, "Starting system reset... ");

   osc_write_log_info(c_LogActivity, "Resetting the target device ...");
   s32_Return = mc_OsyProtocol.OsyEcuReset();
   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error(c_LogActivity, "Could not reset the target device!");
   }
   else
   {
      Sleep(500); //wait a little to make sure the device has performed the reset
      osc_write_log_error(c_LogActivity, "Successfully sent reset to target device!");
   }

   m_ReportProgress(s32_Return, "System reset finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Read HEX file

   \param[in]   orc_HexFilePath              Path to HEX file
   \param[out]  orc_HexFile                  HEX file data structure
   \param[out]  oru32_SignatureBlockAddress  Signature block address

   \return
   C_NO_ERR    HEX file read
   C_RD_WR     Could not load file
   C_NOACT     No signature block found in HEX file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscBuSequences::h_ReadHexFile(const C_SclString & orc_HexFilePath, C_OscHexFile & orc_HexFile,
                                        uint32_t & oru32_SignatureBlockAddress)
{
   int32_t s32_Return = C_NO_ERR;
   uint32_t u32_Return;
   const C_SclString c_LogActivity = "Read HEX File";

   u32_Return = orc_HexFile.LoadFromFile(orc_HexFilePath.c_str());
   if (u32_Return != stw::hex_file::NO_ERR)
   {
      s32_Return = C_RD_WR;
      osc_write_log_error(c_LogActivity, "Could not open the HEX file \"" + orc_HexFilePath + "\"! Details: " +
                          orc_HexFile.ErrorCodeToErrorText(u32_Return));
   }
   else
   {
      C_SclDynamicArray<stw::diag_lib::C_XFLECUInformation> c_InfoBlocks;
      //no return value that can cause trouble for us
      (void)orc_HexFile.GetECUInformationBlocks(c_InfoBlocks, 0, false, false, false);

      osc_write_log_info(c_LogActivity, "Number of application information blocks in HEX file: " +
                         C_SclString::IntToStr(c_InfoBlocks.GetLength()));

      for (int32_t s32_Index = 0; s32_Index < c_InfoBlocks.GetLength(); s32_Index++)
      {
         C_SclStringList c_Lines;
         C_SclString c_Help;
         c_Help.PrintFormatted("%02d", s32_Index + 1);
         osc_write_log_info(c_LogActivity, "Application information block " + c_Help);

         c_InfoBlocks[s32_Index].AddInfoToList(c_Lines);
         for (uint32_t u32_Line = 0; u32_Line < c_Lines.GetCount(); u32_Line++)
         {
            osc_write_log_info(c_LogActivity, c_Lines.Strings[u32_Line]);
         }
      }

      s32_Return = orc_HexFile.GetSignatureBlockAddress(oru32_SignatureBlockAddress);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity, "Could not find a signature block in the HEX file data!");
      }
      else
      {
         C_SclString c_Text;

         c_Text.PrintFormatted("Signature block found at address 0x%08X.", oru32_SignatureBlockAddress);
         osc_write_log_info(c_LogActivity, c_Text);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Report progress in percentage

   Has only to be overridden if needed.

   \param[in]       ou8_ProgressInPercentage     Progress in percentage
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscBuSequences::m_ReportProgressPercentage(const uint8_t ou8_ProgressInPercentage)
{
   std::cout << "Progress in Percentage: " << static_cast<int32_t>(ou8_ProgressInPercentage) << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports some information about the current sequence

   To be overridden by application.
   Default implementation here: print to console and log file.

   \param[in]  os32_Result       Result of service
   \param[in]  orc_Information   Text information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscBuSequences::m_ReportProgress(const int32_t os32_Result, const C_SclString & orc_Information)
{
   std::cout << "Info: " << orc_Information.c_str() << " Result: " << os32_Result << std::endl;

   if (os32_Result == C_NO_ERR)
   {
      osc_write_log_info("Progress", orc_Information);
   }
   else if (os32_Result == C_WARN)
   {
      osc_write_log_warning("Progress", orc_Information);
   }
   else
   {
      osc_write_log_error("Progress", orc_Information);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports information read from openSYDE flashloader of node

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.
   Default implementation here: print read information to console and log file

   \param[in]  orc_DeviceName    Device name
   \param[in]  orc_Information   Information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscBuSequences::m_ReportFlashloaderInformationRead(const C_SclString & orc_DeviceName,
                                                          const C_OscComFlashloaderInformation & orc_Information)
{
   C_SclStringList c_Text;
   const C_SclStringList c_MoreInformation = orc_Information.FlashloaderInformationToText();

   c_Text.Clear();
   c_Text.Add("Device name: " + orc_DeviceName);
   c_Text.AddStrings(&c_MoreInformation);

   std::cout << "openSYDE device information read: " << "\n";
   for (uint32_t u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      std::cout << c_Text.Strings[u32_Line].c_str() << "\n";
      osc_write_log_info("Flashloader Info", c_Text.Strings[u32_Line]);
   }
}
