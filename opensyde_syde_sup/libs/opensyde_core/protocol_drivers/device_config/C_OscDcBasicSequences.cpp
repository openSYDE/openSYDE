//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for simple device configuration sequence

    Simple means beside others: CAN only, openSYDE flashloader only, no routing ...

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "TglTime.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscDcBasicSequences.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
using namespace stw::errors;
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
C_OscDcBasicSequences::C_OscDcBasicSequences(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDcBasicSequences::~C_OscDcBasicSequences()
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

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDcBasicSequences::Init(const C_SclString & orc_CanDllPath, const int32_t os32_CanBitrate)
{
   int32_t s32_Return = C_NO_ERR;

   const C_SclString c_LogActivity = "Initialization";

   m_ReportProgress(s32_Return, "Starting the initialization of CAN driver and protocol ... ");

   osc_write_log_info(c_LogActivity, "CAN DLL path used: " + orc_CanDllPath);

   mc_CanDispatcher.SetDLLName(orc_CanDllPath);
   s32_Return = mc_CanDispatcher.DLL_Open();
   if (s32_Return == C_NO_ERR)
   {
      osc_write_log_info(c_LogActivity, "CAN DLL loaded.");
      s32_Return = mc_CanDispatcher.CAN_Init(os32_CanBitrate);
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
      c_Client.u8_NodeIdentifier = 126;
      c_Client.u8_BusIdentifier = 0U;

      s32_Return = mc_TpCan.SetNodeIdentifiersForBroadcasts(c_Client);

      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_warning(c_LogActivity, "Could not configure the clients broadcast node ID.");
      }
   }

   m_ReportProgress(s32_Return, "Initialization finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Bring openSYDE devices in flashloader state

   Sequence:
   * send openSYDE CAN-TP broadcast "RequestProgramming"
   * send openSYDE broadcast "EcuReset"
   * send openSYDE "DiagnosticSessionControl(PreProgramming)" broadcast for 5 seconds every 5 milliseconds

   The com driver is expected to be initialized for broadcasting as it is done in Init().

   \param[in]  ou32_FlashloaderResetWaitTime    Flashloader reset wait time

   \return
   C_NO_ERR   Sequence finished
   else       error occurred
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDcBasicSequences::ScanEnterFlashloader(const uint32_t ou32_FlashloaderResetWaitTime)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_LogActivity = "Scan Activate Flashloader";
   const uint32_t u32_SCAN_TIME_MS = 5000U;
   uint32_t u32_WaitTime = ou32_FlashloaderResetWaitTime;

   std::vector<C_OscProtocolDriverOsyTpCan::C_BroadcastRequestProgrammingResults> c_Results;

   m_ReportProgress(s32_Return, "Starting the scan for flashloader activation ... ");

   if (u32_WaitTime < u32_SCAN_TIME_MS)
   {
      // The scan time is necessary for the manual triggering of the nodes
      u32_WaitTime = u32_SCAN_TIME_MS;
   }

   s32_Return = this->mc_TpCan.BroadcastRequestProgramming(c_Results);

   m_ReportProgress(C_NO_ERR, "Broadcasting \"request programming\" flag: " +
                    C_SclString::IntToStr(c_Results.size()) + " device(s) answered. ");

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error(c_LogActivity,
                          "openSYDE ECU reset broadcast failed with error: " + C_SclString::IntToStr(s32_Return));
   }
   else
   {
      // check results for request accepted flags
      for (uint32_t u32_ResponseIndex = 0U; u32_ResponseIndex < c_Results.size(); u32_ResponseIndex++)
      {
         if (c_Results[u32_ResponseIndex].q_RequestAccepted == false)
         {
            s32_Return = C_COM;
            break;
         }
      }
   }

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error(c_LogActivity,
                          "At least one reached device did not accept setting the \"request programming\" flag.");
   }
   else
   {
      //broadcast "ResetToFlashloader"
      s32_Return = mc_TpCan.BroadcastEcuReset(C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);

      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error(c_LogActivity,
                             "openSYDE request node reset failed with error: " + C_SclString::IntToStr(s32_Return));
      }
   }

   // Always continue with broadcast. If previous steps did not work, user can do the manual reset while we broadcast.
   const uint32_t u32_StartTime = stw::tgl::TglGetTickCount();

   // If no devices answered, give hint about "reset your device NOW"
   if (c_Results.size() == 0)
   {
      C_SclString c_Text;
      c_Text.PrintFormatted("You now have %d seconds time to turn on your target device ...",
                            u32_SCAN_TIME_MS / 1000);
      m_ReportProgress(C_WARN, c_Text);
   }

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

   m_ReportProgress(s32_Return, "Scan for flashloader activation finished. ");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Scan for connected openSYDE devices

   Assumptions:
   * All devices are in flashloader mode.
   * The com driver is initialized and has set up for broadcasting as it is done in Init()

   Sequence:
   * Broadcast "ReadSerialNumber"
   * For all nodes that have an unique ID: directed read device name

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDcBasicSequences::ScanGetInfo(void)
{
   int32_t s32_Return = C_NO_ERR;
   const C_SclString c_LogActivity = "Scan Device Info";

   std::vector<C_OscProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberResults> c_ReadSnResult;
   std::vector<C_OscProtocolDriverOsyTpCan::C_BroadcastReadEcuSerialNumberExtendedResults> c_ReadSnResultExt;

   m_ReportProgress(s32_Return, "Starting the scan for getting devices information ... ");

   // broadcast: "ReadSerialNumber"
   s32_Return = this->mc_TpCan.BroadcastReadSerialNumber(c_ReadSnResult, c_ReadSnResultExt);

   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error(c_LogActivity, "openSYDE serial number broadcast failed with error: " +
                          C_SclString::IntToStr(s32_Return));
   }
   else

   {
      uint32_t u32_ResultCounter;

      std::vector<uint32_t> c_UniqueIdIndices;
      std::vector<C_OscDcDeviceInformation> c_DeviceInfoResult;
      bool q_SecurityFeatureUsed = false;

      osc_write_log_info("Scan CAN for openSYDE devices",
                         "Sequence finished. Standard nodes found: " + C_SclString::IntToStr(c_ReadSnResult.size()));
      osc_write_log_info("Scan CAN for openSYDE devices",
                         "Sequence finished. Extended nodes found: " + C_SclString::IntToStr(c_ReadSnResultExt.size()));

      // Fill the result container with the standard results
      for (u32_ResultCounter = 0U; u32_ResultCounter < c_ReadSnResult.size(); ++u32_ResultCounter)
      {
         C_OscDcDeviceInformation c_DeviceInfo;
         c_DeviceInfo.SetSerialNumber(c_ReadSnResult[u32_ResultCounter].c_SerialNumber);
         c_DeviceInfo.SetNodeId(c_ReadSnResult[u32_ResultCounter].c_SenderId.u8_NodeIdentifier);
         c_DeviceInfoResult.push_back(c_DeviceInfo);
      }

      // Fill the result container with the extended results
      for (u32_ResultCounter = 0U; u32_ResultCounter < c_ReadSnResultExt.size(); ++u32_ResultCounter)
      {
         C_OscDcDeviceInformation c_DeviceInfo;
         // Serial number length is not necessary, it was already cross checked with the string
         c_DeviceInfo.SetSerialNumber(c_ReadSnResultExt[u32_ResultCounter].c_SerialNumber);
         c_DeviceInfo.SetNodeId(c_ReadSnResultExt[u32_ResultCounter].c_SenderId.u8_NodeIdentifier);
         c_DeviceInfo.SetExtendedInfo(c_ReadSnResultExt[u32_ResultCounter].u8_SubNodeId,
                                      c_ReadSnResultExt[u32_ResultCounter].q_SecurityActivated);
         //lint -e{514}  Using operator with a bool value was intended and is no accident
         q_SecurityFeatureUsed |= c_ReadSnResultExt[u32_ResultCounter].q_SecurityActivated;
         c_DeviceInfoResult.push_back(c_DeviceInfo);
      }

      // Search all unique ID's
      for (u32_ResultCounter = 0U; u32_ResultCounter < c_DeviceInfoResult.size(); ++u32_ResultCounter)
      {
         bool q_UniqueId = true;

         for (uint32_t u32_UniqueIdCheckCounter = 0U;
              u32_UniqueIdCheckCounter < c_DeviceInfoResult.size();
              ++u32_UniqueIdCheckCounter)
         {
            if ((u32_ResultCounter != u32_UniqueIdCheckCounter) &&
                (c_DeviceInfoResult[u32_ResultCounter].u8_NodeId ==
                 c_DeviceInfoResult[u32_UniqueIdCheckCounter].u8_NodeId))
            {
               q_UniqueId = false;
               if (q_SecurityFeatureUsed == true)
               {
                  // Special case: In case of at least one node with active security no broadcasts
                  // can be used by the configuration sequence. Therefore the node IDs must be unique for
                  // using direct communication.
                  s32_Return = C_CHECKSUM;

                  osc_write_log_error(c_LogActivity,
                                      "At least one node has the security feature activated and at least"
                                      " one node ID is not unique. Node ID: "  +
                                      C_SclString::IntToStr(c_DeviceInfoResult[u32_ResultCounter].u8_NodeId));
               }
               break;
            }
         }
         if (q_UniqueId == true)
         {
            c_UniqueIdIndices.push_back(u32_ResultCounter);
         }
      }

      tgl_assert((c_ReadSnResult.size() + c_ReadSnResultExt.size()) == c_DeviceInfoResult.size());

      if (s32_Return == C_NO_ERR)
      {
         C_OscProtocolDriverOsyNode c_CurSenderId;
         C_SclString c_Result;
         uint8_t u8_NumberCode = 0;
         uint32_t u32_UniqueIdIndicesCounter;
         C_OscProtocolDriverOsyNode c_Client;

         c_Client.u8_NodeIdentifier = 126;
         c_Client.u8_BusIdentifier = 0U;

         // Get the device names for all devices with unique ids
         for (u32_UniqueIdIndicesCounter = 0U;
              u32_UniqueIdIndicesCounter < c_UniqueIdIndices.size();
              ++u32_UniqueIdIndicesCounter)
         {
            // Get the correct sender id
            const uint32_t u32_DeviceInfoIndex = c_UniqueIdIndices[u32_UniqueIdIndicesCounter];

            // The first indexes are always the standard SNR results
            if (u32_DeviceInfoIndex < c_ReadSnResult.size())
            {
               c_CurSenderId = c_ReadSnResult[u32_DeviceInfoIndex].c_SenderId;
            }
            else
            {
               const uint32_t u32_ReadSnrResultExtIndex =
                  u32_DeviceInfoIndex - static_cast<uint32_t>(c_ReadSnResult.size());
               // all above must be the extended SNR results
               tgl_assert(u32_ReadSnrResultExtIndex < c_ReadSnResultExt.size());
               c_CurSenderId = c_ReadSnResultExt[u32_ReadSnrResultExtIndex].c_SenderId;
            }

            // set up temporary node IDs
            s32_Return = mc_OsyProtocol.SetNodeIdentifiers(c_Client, c_CurSenderId);
            if (s32_Return != C_NO_ERR)
            {
               osc_write_log_error(c_LogActivity,
                                   "Could not configure the node IDs! Is the server node ID within range?");
            }
            else
            {
               s32_Return = mc_OsyProtocol.OsyReadDeviceName(c_Result, &u8_NumberCode);
            }

            if (s32_Return != C_NO_ERR)
            {
               osc_write_log_error(c_LogActivity, "Could not read the device's device name! Details: " +
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NumberCode));
            }
            else
            {
               c_DeviceInfoResult[u32_DeviceInfoIndex].SetDeviceName(c_Result);
            }
         }
      }
      m_ReportProgress(s32_Return, "Found " + C_SclString::IntToStr(c_DeviceInfoResult.size()) + " device(s).");
      m_ReportDevicesInfoRead(c_DeviceInfoResult, q_SecurityFeatureUsed);
   }

   m_ReportProgress(s32_Return, "Scan for getting devices information finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Broadcast to reset devices

   Function to get all devices out of flashloader after calls to ScanEnterFlashloader() and ScanGetInfo().
   The com driver is expected to be initialized for broadcasting as it is done in Init().

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDcBasicSequences::ResetSystem(void)
{
   int32_t s32_Return = C_NO_ERR;

   m_ReportProgress(s32_Return, "Starting system reset broadcast...");

   s32_Return = mc_TpCan.BroadcastEcuReset(C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_KEY_OFF_ON);

   m_ReportProgress(s32_Return, "System reset broadcast finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Configure device

  \param[in]   ou8_CurrentNodeId   Node ID that the Node we want to configure currently has
  \param[in]   ou8_NewNodeId       Node ID we want to configure to Node with ou8_CurrentNodeId
  \param[in]   ou32_Bitrate        Bitrate we want to configure to Node with ou8_CurrentNodeId
  \param[in]   ou8_InterfaceIndex  Interface the target is connected

   \return
   C_NO_ERR    everything ok
   else        error occurred, see log file for details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscDcBasicSequences::ConfigureDevice(const uint8_t ou8_CurrentNodeId, const uint8_t ou8_NewNodeId,
                                               const uint32_t ou32_Bitrate, const uint8_t ou8_InterfaceIndex)
{
   int32_t s32_Return = C_NO_ERR;
   const C_OscProtocolDriverOsyNode c_ClientId(0, 126);
   const C_OscProtocolDriverOsyNode c_CurrentServerId(0, ou8_CurrentNodeId);

   m_ReportProgress(s32_Return, "Starting device configuration...");

   //set up protocol for communication (which node we want to configure)
   s32_Return = mc_OsyProtocol.SetNodeIdentifiers(c_ClientId, c_CurrentServerId);
   if (s32_Return != C_NO_ERR)
   {
      osc_write_log_error("ProtocolSetup",
                          "Could not configure Node IDs for transport protocol. Are the IDs in range?");
   }
   else
   {
      uint8_t u8_Nrc;
      s32_Return = mc_OsyProtocol.OsyDiagnosticSessionControl(
         C_OscProtocolDriverOsy::hu8_DIAGNOSTIC_SESSION_PROGRAMMING,
         &u8_Nrc);

      if (s32_Return == C_NO_ERR)
      {
         //set security level 1; we want to change ID and Bitrate...
         const uint8_t u8_SECURITY_LEVEL = 1U;
         bool q_SecureMode;
         uint64_t u64_Seed;
         uint8_t ou8_SecurityAlgorithm;
         C_SclString c_LogActivity;

         c_LogActivity = "Security Access";
         s32_Return = mc_OsyProtocol.OsySecurityAccessRequestSeed(u8_SECURITY_LEVEL, q_SecureMode, u64_Seed,
                                                                  ou8_SecurityAlgorithm, &u8_Nrc);

         if (q_SecureMode == true)
         {
            osc_write_log_error(c_LogActivity, "Security request returned security is on. "
                                "No security support here. Use openSYDE GUI tool for this feature.");
            s32_Return = C_CONFIG;
         }
         else if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error(c_LogActivity, "Did not get a security seed from the target device! Details: " +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_Nrc));
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

            s32_Return = mc_OsyProtocol.OsySecurityAccessSendKey(u8_SECURITY_LEVEL, u32_KEY, &u8_Nrc);
            if (s32_Return != C_NO_ERR)
            {
               osc_write_log_error(c_LogActivity, "The target device did not access the security key! Details: " +
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_Nrc));
            }
         }
      }
      //now we can start setting the Node ID and Bitrate
      if (s32_Return == C_NO_ERR)
      {
         const C_SclString c_LogActivity = "NodeConfiguration";
         const C_OscProtocolDriverOsyNode c_NewServerId(0, ou8_NewNodeId);
         C_SclString c_ProgressLogMsg = "";
         c_ProgressLogMsg.PrintFormatted(
            "Configuring Node ID \"%d\" to Node with current ID \"%d\" on Interface CAN %d.",
            ou8_NewNodeId, ou8_CurrentNodeId, ou8_InterfaceIndex + 1U);
         s32_Return = mc_OsyProtocol.OsySetNodeIdForChannel(0, ou8_InterfaceIndex, c_NewServerId, &u8_Nrc);
         m_ReportProgress(s32_Return, c_ProgressLogMsg);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error(c_LogActivity, "Could not set Node ID! Details: " +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_Nrc));
         }
         else
         {
            c_ProgressLogMsg = "";
            c_ProgressLogMsg.PrintFormatted(
               "Configuring Bitrate %d kbit/s to Node on Interface CAN %d.",
               ou32_Bitrate, ou8_InterfaceIndex + 1U);
            s32_Return = mc_OsyProtocol.OsySetBitrate(0, ou8_InterfaceIndex, ou32_Bitrate * 1000U,
                                                      &u8_Nrc);
            m_ReportProgress(s32_Return, c_ProgressLogMsg);

            if (s32_Return != C_NO_ERR)
            {
               osc_write_log_error(c_LogActivity, "Could not set Bitrate! Details: " +
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_Nrc));
            }
         }
      }
   }

   m_ReportProgress(s32_Return, "Device configuration finished.");

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility function to convert device information of found devices to string

   \param[in]  orc_DeviceInfoResult    Device information results
   \param[in]  oq_SecurityFeatureUsed  Security feature used for at least one node

   \return
   String with information
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscDcBasicSequences::h_DevicesInfoToString(
   const std::vector<C_OscDcDeviceInformation> & orc_DeviceInfoResult, const bool oq_SecurityFeatureUsed)
{
   C_SclString c_Information;

   c_Information = "Found " + C_SclString::IntToStr(orc_DeviceInfoResult.size()) + " device(s): \n";

   for (uint32_t u32_ItDevices = 0; u32_ItDevices < orc_DeviceInfoResult.size(); ++u32_ItDevices)
   {
      const C_OscDcDeviceInformation & rc_CurDevice = orc_DeviceInfoResult[u32_ItDevices];
      c_Information += " Device #" + C_SclString::IntToStr(u32_ItDevices + 1) + "\n";
      if (rc_CurDevice.q_DeviceNameValid == true)
      {
         c_Information += "   Device name: " + rc_CurDevice.c_DeviceName + "\n";
      }
      if (rc_CurDevice.q_NodeIdValid == true)
      {
         c_Information += "   Node ID: " + C_SclString::IntToStr(rc_CurDevice.u8_NodeId) + "\n";
      }
      if (rc_CurDevice.q_IpAddressValid == true)
      {
         c_Information += "   IP address: ";
         c_Information += C_SclString::IntToStr(rc_CurDevice.au8_IpAddress[0]) + ".";
         c_Information += C_SclString::IntToStr(rc_CurDevice.au8_IpAddress[1]) + ".";
         c_Information += C_SclString::IntToStr(rc_CurDevice.au8_IpAddress[2]) + ".";
         c_Information += C_SclString::IntToStr(rc_CurDevice.au8_IpAddress[3]) + "\n";
      }
      if (rc_CurDevice.c_SerialNumber.q_IsValid == true)
      {
         c_Information += "   Serial number: " + rc_CurDevice.c_SerialNumber.GetSerialNumberAsFormattedString() + "\n";
      }
      if (rc_CurDevice.q_ExtendedInfoValid == true)
      {
         c_Information += "   Sub node ID: " + C_SclString::IntToStr(rc_CurDevice.u8_SubNodeId) + "\n";
         c_Information += "   Security activated: ";
         c_Information += rc_CurDevice.q_SecurityActivated ? "yes" : "no";
         c_Information += "\n";
      }
   }

   if (orc_DeviceInfoResult.size() > 0)
   {
      c_Information += " Security feature used for at least one device: ";
      c_Information += oq_SecurityFeatureUsed ? "yes" : "no";
      c_Information += "\n";
   }

   return c_Information;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports some information about the current sequence

   To be overridden by application.
   Default implementation here: print to console and log file.

   \param[in]  os32_Result       Result of service
   \param[in]  orc_Information   Text information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcBasicSequences::m_ReportProgress(const int32_t os32_Result, const C_SclString & orc_Information)
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
/*! \brief  Reports device information read from found devices

   Called by ScanGetInfo() after it has scanned information from connected devices.
   Default implementation here: print read information to console and log file

   \param[in]  orc_DeviceInfoResult    Device information results
   \param[in]  oq_SecurityFeatureUsed  Security feature used for at least one node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcBasicSequences::m_ReportDevicesInfoRead(const std::vector<C_OscDcDeviceInformation> & orc_DeviceInfoResult,
                                                    const bool oq_SecurityFeatureUsed)
{
   std::cout << h_DevicesInfoToString(orc_DeviceInfoResult, oq_SecurityFeatureUsed).c_str() << std::endl;
}
