//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Sequences for system update.

   \class       stw::opensyde_core::C_OscSuSequences
   \brief       openSYDE: Sequences for system update.

   Provides functions to perform the flashing sequence.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUSEQUENCES_HPP
#define C_OSCSUSEQUENCES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscComFlashloaderInformation.hpp"
#include "C_OscComSequencesBase.hpp"
#include "C_OscSuSequencesNodeStates.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------

class C_OscSuSequences :
   public C_OscComSequencesBase
{
public:
   ///basic information about status of one openSYDE device
   class C_OsyDeviceInformation
   {
   public:
      stw::scl::C_SclString c_DeviceName;                                   ///< name of device
      std::vector<C_OscProtocolDriverOsy::C_FlashBlockInfo> c_Applications; ///< list of applications present on the
                                                                            // device
      C_OscComFlashloaderInformation c_MoreInformation;                     ///< collection of additional information
   };

   ///basic information about status of one STW Flashloader device
   class C_XflDeviceInformation
   {
   public:
      stw::diag_lib::C_XFLInformationFromServer c_BasicInformation; ///< basic information from server
      stw::diag_lib::C_XFLChecksumAreas c_ChecksumInformation;      ///< information about sector or block based
                                                                    // checksums
   };

   ///setting which files(s) to flash for one node and all flash parameters for each node
   class C_DoFlash
   {
   public:
      C_DoFlash(void);

      ///list of files to flash (keep size to 0 to not flash any files)
      std::vector<stw::scl::C_SclString> c_FilesToFlash;
      ///list of parameter files to write to NVM (keep size to 0 to not write any files)
      std::vector<stw::scl::C_SclString> c_FilesToWriteToNvm;
      ///optional list of names to allow (other than the device name itself)
      std::vector<stw::scl::C_SclString> c_OtherAcceptedDeviceNames;

      ///optional PEM file (empty string for no PEM file)
      stw::scl::C_SclString c_PemFile;

      /// Node configuration flags for security state
      bool q_SendSecurityEnabledState;
      bool q_SecurityEnabled;

      /// Node configuration flags for debugger state
      bool q_SendDebuggerEnabledState;
      bool q_DebuggerEnabled;
   };

   ///set of information used to identify one application
   class C_ApplicationProperties
   {
   public:
      bool operator == (const C_ApplicationProperties & orc_Source) const;

      stw::scl::C_SclString c_Name;
      stw::scl::C_SclString c_Version;
      stw::scl::C_SclString c_BuildDate;
      stw::scl::C_SclString c_BuildTime;
   };

   //constants for steps reported by "m_ReportProgress"
   //we use enums so the application can use static checkers to verify all are evaluated
   enum E_ProgressStep
   {
      eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_START = 0,
      eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR,
      eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_WARNING,
      eACTIVATE_FLASHLOADER_OSY_ECU_RESET_WARNING,
      eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR,
      eACTIVATE_FLASHLOADER_XFL_ECU_RESET_WARNING,
      eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR,
      eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START,
      eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR,
      eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR,
      eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START,
      eACTIVATE_FLASHLOADER_OSY_RECONNECT_WARNING,
      eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR,
      eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR,
      eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR,
      eACTIVATE_FLASHLOADER_ROUTING_START,
      eACTIVATE_FLASHLOADER_ROUTING_ERROR,
      eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR, // problem with available features of flashloader
      eACTIVATE_FLASHLOADER_FINISHED,

      eREAD_DEVICE_INFO_START, //supports aborting
      eREAD_DEVICE_INFO_OSY_START,
      eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR,
      eREAD_DEVICE_INFO_OSY_SET_SESSION_START,
      eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR,
      eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START,
      eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR,
      eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START,
      eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR,
      eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR,
      eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START,
      eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR,
      eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_START,
      eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR,
      eREAD_DEVICE_INFO_OSY_FINISHED,
      eREAD_DEVICE_INFO_XFL_START,
      eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR,
      eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START,
      eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR,
      eREAD_DEVICE_INFO_XFL_FINISHED,
      eREAD_DEVICE_INFO_FINISHED,

      eUPDATE_SYSTEM_START,
      eUPDATE_SYSTEM_OSY_NODE_START,
      eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR, // problem getting the available features
      eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START,
      eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR,
      eUPDATE_SYSTEM_OSY_RECONNECT_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK,
      eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START,
      eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE,
      eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR,

      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START,      //starting to write hex file
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START, //supports aborting
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START, //supports aborting
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED,

      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START,         //starting to write file based file
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START, //supports aborting
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START, //supports aborting
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_RESULT_STRING,
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED,

      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START,                   //reported once for each node (if there are > 0 NVM
                                                                 // files)
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR, // problem with available features of flashloader
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR,   //problem getting maximum block size from device
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_START,  //reported once for each file; supports aborting
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR,  //problem opening input file
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_START, //reported once for each file; supports aborting
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR, //problem writing file data to device
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED,    //reported for each finished file
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FINISHED,         //reported once for each node (if there are > 0 NVM files)

      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_START,
      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR, // problem with available features of flashloader
      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_FINISHED,

      eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_START,
      eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR, // problem with available features of
                                                                            // flashloader
      eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_FINISHED,

      eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_START,
      eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR, // problem with available features of
                                                                            // flashloader
      eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_FINISHED,

      eUPDATE_SYSTEM_OSY_NODE_FINISHED,
      eUPDATE_SYSTEM_ABORTED,
      eUPDATE_SYSTEM_FINISHED,

      eUPDATE_SYSTEM_XFL_NODE_START,
      eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START,
      eXFL_PROGRESS, //wrapped progress information from STW flashloader driver
      eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR,
      eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED,
      eUPDATE_SYSTEM_XFL_NODE_FINISHED,

      eRESET_SYSTEM_START,
      eRESET_SYSTEM_OSY_NODE_ERROR,
      eRESET_SYSTEM_OSY_ROUTED_NODE_ERROR,
      eRESET_SYSTEM_FINISHED
   };

   C_OscSuSequences(void);
   virtual ~C_OscSuSequences(void);

   static int32_t h_CreateTemporaryFolder(const std::vector<C_OscNode> & orc_Nodes,
                                          const std::vector<uint8_t> & orc_ActiveNodes,
                                          const stw::scl::C_SclString & orc_TargetPath,
                                          std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                          stw::scl::C_SclString * const opc_ErrorPath = NULL);
   static void h_CheckForChangedApplications(const std::vector<C_ApplicationProperties> & orc_ClientSideApplications,
                                             const std::vector<C_ApplicationProperties> & orc_ServerSideApplications,
                                             std::vector<uint8_t> & orc_ApplicationsPresentOnServer);

   int32_t ActivateFlashloader(const bool oq_FailOnFirstError = true);
   int32_t ReadDeviceInformation(const bool oq_FailOnFirstError = true);
   int32_t UpdateSystem(const std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                        const std::vector<uint32_t> & orc_NodesOrder);
   int32_t ResetSystem(void);

   virtual int32_t GetConnectStates(std::vector<C_OscSuSequencesNodeConnectStates> & orc_ConnectStatesNodes)
   const;
   virtual int32_t GetUpdateStates(std::vector<C_OscSuSequencesNodeUpdateStates> & orc_UpdateStatesNodes)
   const;

   static void h_FillDoFlashWithPemStates(const C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity,
                                          const C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger,
                                          C_OscSuSequences::C_DoFlash & orc_DoFlash);
   static void h_OpenSydeFlashloaderInformationToText(const C_OsyDeviceInformation & orc_Info,
                                                      stw::scl::C_SclStringList & orc_Text);
   static void h_StwFlashloaderInformationToText(const C_XflDeviceInformation & orc_Info,
                                                 stw::scl::C_SclStringList & orc_Text);

protected:
   //functions we use to report to application:
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result, const uint8_t ou8_Progress,
                                 const stw::scl::C_SclString & orc_Information);
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result, const uint8_t ou8_Progress,
                                 const C_OscProtocolDriverOsyNode & orc_Server,
                                 const stw::scl::C_SclString & orc_Information);

   virtual void m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                           const uint32_t ou32_NodeIndex);
   virtual void m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                      const uint32_t ou32_NodeIndex);

   //functions from base class that we implement to parse incoming reports:
   virtual int32_t m_XflReportProgress(const uint8_t ou8_Progress, const stw::scl::C_SclString & orc_Text);

   uint32_t mu32_CurrentNode; ///< index of currently addressed node within System Definition
   uint32_t mu32_CurrentFile;

private:
   //not implemented -> prevent copying
   C_OscSuSequences(const C_OscSuSequences & orc_Source);
   //not implemented -> prevent assignment
   C_OscSuSequences & operator = (const C_OscSuSequences & orc_Source); //lint !e1511 //we want to hide the base func.

   C_OscProtocolDriverOsyNode mc_CurrentNode; //node we currently deal with

   std::vector<C_OscSuSequencesNodeConnectStates> mc_ConnectStatesNodes;
   std::vector<C_OscSuSequencesNodeUpdateStates> mc_UpdateStatesNodes;

   int32_t m_FlashNodeOpenSydeHex(const std::vector<stw::scl::C_SclString> & orc_FilesToFlash,
                                  const std::vector<stw::scl::C_SclString> & orc_OtherAcceptedDeviceNames,
                                  const uint32_t ou32_RequestDownloadTimeout, const uint32_t ou32_TransferDataTimeout,
                                  bool & orq_SetProgrammingMode,
                                  std::vector<C_OscSuSequencesNodeHexFileStates> & orc_StateHexFiles);
   int32_t m_FlashOneFileOpenSydeHex(const stw::hex_file::C_HexDataDump & orc_HexDataDump,
                                     const uint32_t ou32_SignatureAddress, const uint32_t ou32_RequestDownloadTimeout,
                                     const uint32_t ou32_TransferDataTimeout,
                                     C_OscSuSequencesNodeHexFileStates & orc_StateHexFile);
   int32_t m_FlashNodeOpenSydeFile(const std::vector<stw::scl::C_SclString> & orc_FilesToFlash,
                                   const uint32_t ou32_RequestDownloadTimeout, const uint32_t ou32_TransferDataTimeout,
                                   const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                   bool & orq_SetProgrammingMode,
                                   std::vector<C_OscSuSequencesNodeOtherFileStates> & orc_StateOtherFiles);
   int32_t m_FlashOneFileOpenSydeFile(const stw::scl::C_SclString & orc_FileToFlash,
                                      const uint32_t ou32_RequestDownloadTimeout,
                                      const uint32_t ou32_TransferDataTimeout,
                                      const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                      C_OscSuSequencesNodeOtherFileStates & orc_StateOtherFile);
   int32_t m_WriteNvmOpenSyde(const std::vector<stw::scl::C_SclString> & orc_FilesToWrite,
                              const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                              const bool oq_SetProgrammingMode,
                              std::vector<C_OscSuSequencesNodePsiFileStates> & orc_StatePsiFiles);
   int32_t m_WritePemOpenSydeFile(const stw::scl::C_SclString & orc_FileToWrite,
                                  const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                  bool & orq_SetProgrammingMode, C_OscSuSequencesNodePemFileStates & orc_StatePemFile);
   int32_t m_WriteOpenSydeNodeStates(const C_OscSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                     const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                     bool & orq_SetProgrammingMode,
                                     C_OscSuSequencesNodePemFileStates & orc_StatePemFile);

   int32_t m_WriteFingerPrintOsy(void);

   int32_t m_FlashNodeXfl(const std::vector<stw::scl::C_SclString> & orc_FilesToFlash,
                          std::vector<C_OscSuSequencesNodeStwFlHexFileStates> & orc_StateHexFiles);

   int32_t m_ReadDeviceInformationOpenSyde(const uint8_t ou8_ProgressToReport, const uint32_t ou32_NodeIndex,
                                           C_OscSuSequencesNodeConnectStates & orc_NodeState);
   int32_t m_ReadDeviceInformationStwFlashloader(const uint8_t ou8_ProgressToReport, const uint32_t ou32_NodeIndex,
                                                 C_OscSuSequencesNodeConnectStates & orc_NodeState);

   bool m_IsNodeActive(const uint32_t ou32_NodeIndex, const uint32_t ou32_BusIndex,
                       C_OscNodeProperties::E_FlashLoaderProtocol & ore_ProtocolType,
                       C_OscProtocolDriverOsyNode & orc_NodeId) const;

   int32_t m_ReconnectToTargetServer(const bool oq_RestartRouting = false, const uint32_t ou32_NodeIndex = 0U);
   int32_t m_DisconnectFromTargetServer(const bool oq_DisconnectOnIp2IpRouting = true);

   uint32_t m_GetAdaptedTransferDataTimeout(const uint32_t ou32_DeviceTransferDataTimeout,
                                            const uint32_t ou32_MaxBlockLength, const uint8_t ou8_BusIdentifier) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
