//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Sequences for system update.

   \class       stw_opensyde_core::C_OSCSuSequences
   \brief       openSYDE: Sequences for system update.

   Provides functions to perform the flashing sequence.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUSEQUENCESH
#define C_OSCSUSEQUENCESH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCComSequencesBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------

class C_OSCSuSequences :
   public C_OSCComSequencesBase
{
public:
   ///basic information about status of one openSYDE device
   class C_OsyDeviceInformation
   {
   public:
      stw_scl::C_SCLString c_DeviceName;                                    ///< name of device
      std::vector<C_OSCProtocolDriverOsy::C_FlashBlockInfo> c_Applications; ///< list of applications present on the
                                                                            // device
      C_OSCComDriverFlash::C_FlashloaderInformation c_MoreInformation;      ///< collection of additional information
   };

   ///basic information about status of one STW Flashloader device
   class C_XflDeviceInformation
   {
   public:
      stw_diag_lib::C_XFLInformationFromServer c_BasicInformation; ///< basic information from server
      stw_diag_lib::C_XFLChecksumAreas c_ChecksumInformation;      ///< information about sector or block based
                                                                   // checksums
   };

   ///setting which files(s) to flash for one node and all flash parameters for each node
   class C_DoFlash
   {
   public:
      ///list of files to flash (keep size to 0 to not flash any files)
      std::vector<stw_scl::C_SCLString> c_FilesToFlash;
      ///list of parameter files to write to NVM (keep size to 0 to not write any files)
      std::vector<stw_scl::C_SCLString> c_FilesToWriteToNvm;
   };

   ///set of information used to identify one application
   class C_ApplicationProperties
   {
   public:
      bool operator == (const C_ApplicationProperties & orc_Source) const;

      stw_scl::C_SCLString c_Name;
      stw_scl::C_SCLString c_Version;
      stw_scl::C_SCLString c_BuildDate;
      stw_scl::C_SCLString c_BuildTime;
   };

   //constants for steps reported by "m_ReportProgress"
   //we use enums so the application can use static checkers to verify all all evaluated
   enum E_ProgressStep
   {
      eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_START = 0,
      eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_ERROR,
      eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_START,
      eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_ERROR,
      eACTIVATE_FLASHLOADER_XFL_ECU_RESET_START,
      eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR,
      eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START,
      eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR,
      eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR,
      eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START,
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
      eREAD_DEVICE_INFO_OSY_FINISHED,
      eREAD_DEVICE_INFO_XFL_START,
      eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR,
      eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START,
      eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR,
      eREAD_DEVICE_INFO_XFL_FINISHED,
      eREAD_DEVICE_INFO_FINISHED,

      eUPDATE_SYSTEM_START,
      eUPDATE_SYSTEM_OSY_NODE_START,
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
      eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED,

      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START,          //reported once for each node (if there are > 0 NVM files)
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_RECONNECT_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_READ_FEATURE_ERROR, // problem getting the available features
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR, // problem with available features of flashloader
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR,
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR, //problem getting maximum block size from device
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_START, //reported once for each file; supports aborting
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR, //problem opening input file
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_START, //reported once for each file; supports aborting
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR, //problem writing file data to device
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED,  //reported for each finished file
      eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FINISHED,       //reported once for each node (if there are > 0 NVM files)

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
      eRESET_SYSTEM_FINISHED
   };

   C_OSCSuSequences(void);
   virtual ~C_OSCSuSequences(void);

   static stw_types::sint32 h_CreateTemporaryFolder(const std::vector<C_OSCNode> & orc_Nodes,
                                                    const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                                    const stw_scl::C_SCLString & orc_TargetPath,
                                                    std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                                    stw_scl::C_SCLString * const opc_ErrorPath = NULL);
   static void h_CheckForChangedApplications(const std::vector<C_ApplicationProperties> & orc_ClientSideApplications,
                                             const std::vector<C_ApplicationProperties> & orc_ServerSideApplications,
                                             std::vector<stw_types::uint8> & orc_ApplicationsPresentOnServer);

   stw_types::sint32 ActivateFlashloader(const bool oq_FailOnFirstError = true);
   stw_types::sint32 ReadDeviceInformation(const bool oq_FailOnFirstError = true);
   stw_types::sint32 UpdateSystem(const std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                  const std::vector<stw_types::uint32> & orc_NodesOrder);
   stw_types::sint32 ResetSystem(void);

   static void h_OpenSydeFlashloaderInformationToText(const C_OsyDeviceInformation & orc_Info,
                                                      stw_scl::C_SCLStringList & orc_Text);
   static void h_StwFlashloaderInformationToText(const C_XflDeviceInformation & orc_Info,
                                                 stw_scl::C_SCLStringList & orc_Text);

protected:
   //functions we use to report to application:
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const stw_types::sint32 os32_Result,
                                 const stw_types::uint8 ou8_Progress, const stw_scl::C_SCLString & orc_Information);
   virtual bool m_ReportProgress(const E_ProgressStep oe_Step, const stw_types::sint32 os32_Result,
                                 const stw_types::uint8 ou8_Progress, const C_OSCProtocolDriverOsyNode & orc_Server,
                                 const stw_scl::C_SCLString & orc_Information);

   virtual void m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                           const stw_types::uint32 ou32_NodeIndex);
   virtual void m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                      const stw_types::uint32 ou32_NodeIndex);

   //functions from base class that we implement to parse incoming reports:
   virtual stw_types::sint32 m_XflReportProgress(const stw_types::uint8 ou8_Progress,
                                                 const stw_scl::C_SCLString & orc_Text);

   stw_types::uint32 mu32_CurrentNode; ///< index of currently addressed node within System Definition
   stw_types::uint32 mu32_CurrentFile;

private:
   C_OSCSuSequences(const C_OSCSuSequences & orc_Source);               //not implemented -> prevent copying
   C_OSCSuSequences & operator = (const C_OSCSuSequences & orc_Source); //not implemented -> prevent assignment

   C_OSCProtocolDriverOsyNode mc_CurrentNode; //node we currently deal with

   stw_types::sint32 m_FlashNodeOpenSydeHex(const std::vector<stw_scl::C_SCLString> & orc_FilesToFlash,
                                            const stw_types::uint32 ou32_RequestDownloadTimeout,
                                            const stw_types::uint32 ou32_TransferDataTimeout);
   stw_types::sint32 m_FlashOneFileOpenSydeHex(const stw_hex_file::C_HexDataDump & orc_HexDataDump,
                                               const stw_types::uint32 ou32_SignatureAddress,
                                               const stw_types::uint32 ou32_RequestDownloadTimeout,
                                               const stw_types::uint32 ou32_TransferDataTimeout);
   stw_types::sint32 m_FlashNodeOpenSydeFile(const std::vector<stw_scl::C_SCLString> & orc_FilesToFlash,
                                             const stw_types::uint32 ou32_RequestDownloadTimeout,
                                             const stw_types::uint32 ou32_TransferDataTimeout);
   stw_types::sint32 m_FlashOneFileOpenSydeFile(const stw_scl::C_SCLString & orc_FileToFlash,
                                                const stw_types::uint32 ou32_RequestDownloadTimeout,
                                                const stw_types::uint32 ou32_TransferDataTimeout);
   stw_types::sint32 m_WriteNvmOpenSyde(const std::vector<stw_scl::C_SCLString> & orc_FilesToWrite);

   stw_types::sint32 m_WriteFingerPrintOsy(void);

   stw_types::sint32 m_FlashNodeXfl(const std::vector<stw_scl::C_SCLString> & orc_FilesToFlash);

   stw_types::sint32 m_ReadDeviceInformationOpenSyde(const stw_types::uint8 ou8_ProgressToReport,
                                                     const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 m_ReadDeviceInformationStwFlashloader(const stw_types::uint8 ou8_ProgressToReport,
                                                           const stw_types::uint32 ou32_NodeIndex);

   bool m_IsNodeActive(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_BusIndex,
                       C_OSCNodeProperties::E_FlashLoaderProtocol & ore_ProtocolType,
                       C_OSCProtocolDriverOsyNode & orc_NodeId) const;

   stw_types::sint32 m_ReconnectToTargetServer(const bool oq_RestartRouting = false,
                                               const stw_types::uint32 ou32_NodeIndex = 0U);
   stw_types::sint32 m_DisconnectFromTargetServer(const bool oq_DisconnectOnIp2IpRouting = true);

   static stw_types::sint32 mh_CopyFile(const stw_scl::C_SCLString & orc_SourceFile,
                                        const stw_scl::C_SCLString & orc_TargetFile,
                                        stw_scl::C_SCLString * const opc_ErrorPath = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
