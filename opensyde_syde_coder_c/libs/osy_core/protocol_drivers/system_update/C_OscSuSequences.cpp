//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Sequences for system update.

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <set>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscSuSequences.hpp"
#include "C_SclString.hpp"
#include "C_SclDateTime.hpp"
#include "C_SclChecksums.hpp"
#include "TglUtils.hpp"
#include "TglTime.hpp"
#include "TglFile.hpp"
#include "C_OsyHexFile.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscUtils.hpp"
#include "C_OscDataDealerNvmSafe.hpp"
#include "C_OscDiagProtocolOsy.hpp"
#include "C_OscSecurityPem.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::diag_lib;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequences::C_DoFlash::C_DoFlash(void) :
   q_SendSecurityEnabledState(false),
   q_SecurityEnabled(false),
   q_SendDebuggerEnabledState(false),
   q_DebuggerEnabled(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: check whether node is an active node on a specific bus

   Assumptions:
   * mpc_SystemDefinition and mc_ActiveNodes must be valid
   * node and bus index must be within the range of nodes contained in mpc_SystemDefinition

   \param[in]  ou32_NodeIndex     index of node within mpc_SystemDefinition
   \param[in]  ou32_BusIndex      index of bus within mpc_SystemDefinition
   \param[out] ore_ProtocolType   if found: type of flashloader protocol defined for node (otherwise: eFL_NONE)
   \param[out] orc_NodeId         if found: node id of the node on the specified bus

   \return
   true    node is active on specified bus
   false   node not active on specified bus
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSuSequences::m_IsNodeActive(const uint32_t ou32_NodeIndex, const uint32_t ou32_BusIndex,
                                      C_OscNodeProperties::E_FlashLoaderProtocol & ore_ProtocolType,
                                      C_OscProtocolDriverOsyNode & orc_NodeId) const
{
   bool q_Return = false;
   const C_OscNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[ou32_NodeIndex];

   ore_ProtocolType = C_OscNodeProperties::eFL_NONE;

   //is the node intends to be addressed at all (defined by parameter set in Init() function)
   if (this->mc_ActiveNodes[ou32_NodeIndex] == 1U)
   {
      for (uint16_t u16_Interface = 0U; u16_Interface < rc_Node.c_Properties.c_ComInterfaces.size(); u16_Interface++)
      {
         const C_OscNodeComInterfaceSettings & rc_Settings = rc_Node.c_Properties.c_ComInterfaces[u16_Interface];

         if ((rc_Settings.GetBusConnected() == true) && (rc_Settings.u32_BusIndex == ou32_BusIndex) &&
             (rc_Settings.q_IsUpdateEnabled == true))
         {
            //we cannot know for sure whether the target is in flashloader or application mode
            //prerequisite: a node is either
            //* a full openSYDE node (Flashloader + Application) or
            //* an STW Flashloader node
            //openSYDE node ?
            if (rc_Node.c_Properties.e_DiagnosticServer == C_OscNodeProperties::eDS_OPEN_SYDE)
            {
               tgl_assert(rc_Node.c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_OPEN_SYDE);
            }

            q_Return = true;
            ore_ProtocolType = rc_Node.c_Properties.e_FlashLoader;
            orc_NodeId.u8_NodeIdentifier = rc_Settings.u8_NodeId;
            orc_NodeId.u8_BusIdentifier = this->mpc_SystemDefinition->c_Buses[ou32_BusIndex].u8_BusId;
            break; //found the interface that is connected to the specified bus
         }
      }
   }
   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports some information about the current sequence

   To be overridden by application.
   Default implementation here: print to console.

   The public procedure functions report their progress using either this function or the polymorphous variation.
   If a procedure function finishes without problems it reports a progress of 100 before it returns.
   If it has problems the last progress reported can be < 100.
   Depending on the function it can report its whole process from "0 to 100" or individual parts
    (e.g.: SystemUpdate() will report from 0..100 for each node that is updated).
   The return value can be used to abort an ongoing sequence.
   However, not all calls to the function check the return value.

   \param[in]     oe_Step           Step of node configuration
   \param[in]     os32_Result       Result of service
   \param[in]     ou8_Progress      Progress of sequence in percentage
   \param[in]     orc_Information   Additional text information

   \return
   Flag for aborting sequence
   - true   abort sequence
   - false  continue sequence
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSuSequences::m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result,
                                        const uint8_t ou8_Progress, const C_SclString & orc_Information)
{
   std::cout << "Step: " << static_cast<uint32_t>(oe_Step) << " Result: " << os32_Result << " Progress: " <<
      static_cast<uint32_t>(ou8_Progress) << " Info: "  << orc_Information.c_str() << "\n";

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports some information about the current sequence for a specific server

   To be overridden by application.
   Default implementation here: print to console.

   For more details see description of the other m_ReportProgress() function

   \param[in]     oe_Step           Step of node configuration
   \param[in]     os32_Result       Result of service
   \param[in]     orc_Server        Affected node
   \param[in]     ou8_Progress      Progress of sequence in percentage (goes from 0..100 for each function)
   \param[in]     orc_Information   Additional text information

   \return
   Flag for aborting sequence
   - true   abort sequence
   - false  continue sequence
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSuSequences::m_ReportProgress(const E_ProgressStep oe_Step, const int32_t os32_Result,
                                        const uint8_t ou8_Progress, const C_OscProtocolDriverOsyNode & orc_Server,
                                        const C_SclString & orc_Information)
{
   std::cout << "Step: " << static_cast<uint32_t>(oe_Step) << " Result: " << os32_Result << " Progress: " <<
      static_cast<uint32_t>(ou8_Progress) << " Bus Id: " << static_cast<uint32_t>(orc_Server.u8_BusIdentifier) <<
      " Node Id: " << static_cast<uint32_t>(orc_Server.u8_NodeIdentifier) << " Info: " <<
      orc_Information.c_str() << "\n";

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports information read from openSYDE server node

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.
   Default implementation here: print read information to console

   \param[in]     orc_Info         Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSuSequences::m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                                  const uint32_t ou32_NodeIndex)
{
   C_SclStringList c_Text;

   h_OpenSydeFlashloaderInformationToText(orc_Info, c_Text);

   std::cout << "openSYDE device information found for node with index " << ou32_NodeIndex << "\n";
   for (uint32_t u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      std::cout << c_Text.Strings[u32_Line].c_str() << "\n";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports information read from STW flashloader server node

   Called by ReadDeviceInformation() after it has read information from an STW flashloader node.
   Default implementation here: print read information to console; checksum block or sector information is not printed

   \param[in]     orc_Info         Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSuSequences::m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                             const uint32_t ou32_NodeIndex)
{
   C_SclStringList c_Text;

   h_StwFlashloaderInformationToText(orc_Info, c_Text);

   std::cout << "STW Flashloader device information found for node with index " << ou32_NodeIndex << "\n";
   for (uint32_t u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      std::cout << c_Text.Strings[u32_Line].c_str() << "\n";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   report progress of STW Flashloader operation to application

   Report progress information from STW Flashloader driver via virtual function.

   \param[in]   ou8_Progress     progress of operation (from 0 to 100)
                                 progress invalid: 255
   \param[in]   orc_Text         textual information about progress

   \return
   C_NO_ERR    continue operation
   else        abort operation (not honored at each position)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_XflReportProgress(const uint8_t ou8_Progress, const C_SclString & orc_Text)
{
   int32_t s32_Return = C_NO_ERR;

   //add node information:
   const bool q_Return = this->m_ReportProgress(eXFL_PROGRESS, C_NO_ERR, ou8_Progress, mc_CurrentNode, orc_Text);

   if (q_Return == true)
   {
      s32_Return = C_BUSY; //abort
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Flash one openSYDE address based node

   Flash one or more files.

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * list of files must be > 0
   * files must be present and in Intel or Motorola hex format
   * node is active

   * Reports progress from 0..100 for the overall process
   * Reports 0..100 for each file being flashed

   \param[in]      orc_FilesToFlash              Files to flash
   \param[in]      orc_OtherAcceptedDeviceNames  Other accepted device names
   \param[in]      ou32_RequestDownloadTimeout   Maximum time in ms it can take to erase one continuous area in flash
   \param[in]      ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to flash
   \param[in,out]  orq_SetProgrammingMode        In: Flag if programming mode must be set.
                                                 Out: Flag if programming mode was set.
   \param[out]     orc_StateHexFiles             States of all handled hex files

   \return
   C_NO_ERR    flashed all files
   C_RD_WR     one of the files is not a valid Intel or Motorola hex file
               could not split up hex file in individual areas
   C_CONFIG    no signature block found in hex file
   C_COM       communication driver reported problem (details will be written to log file)
   C_NOACT     could not extract device name from hex file
   C_OVERFLOW  device name of device does not match name contained in hex file
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_FlashNodeOpenSydeHex(const std::vector<C_SclString> & orc_FilesToFlash,
                                                 const std::vector<C_SclString> & orc_OtherAcceptedDeviceNames,
                                                 const uint32_t ou32_RequestDownloadTimeout,
                                                 const uint32_t ou32_TransferDataTimeout, bool & orq_SetProgrammingMode,
                                                 std::vector<C_OscSuSequencesNodeHexFileStates> & orc_StateHexFiles)
{
   int32_t s32_Return = C_NO_ERR;
   uint32_t u32_Return;

   std::vector<uint32_t> c_SignatureAddresses(orc_FilesToFlash.size()); ///< addresses of signatures within hex files

   //C_OsyHexFile cannot be copied; so we cannot put it into a resizable vector
   //-> create instances manually
   std::vector<C_OsyHexFile *> c_Files(orc_FilesToFlash.size());
   for (uint32_t u32_File = 0U; u32_File < c_Files.size(); u32_File++)
   {
      c_Files[u32_File] = new C_OsyHexFile();
   }

   //try to open files to check whether we have valid hex files before we start messing with the target's flash memory:
   for (uint32_t u32_File = 0U; (u32_File < orc_FilesToFlash.size()) && (s32_Return == C_NO_ERR); u32_File++)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Opening HEX file " + orc_FilesToFlash[u32_File] + ".");
      u32_Return = c_Files[u32_File]->LoadFromFile(orc_FilesToFlash[u32_File].c_str());
      if (u32_Return != stw::hex_file::NO_ERR)
      {
         const C_SclString c_ErrorText = c_Files[u32_File]->ErrorCodeToErrorText(u32_Return);
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR, C_RD_WR, 0U, mc_CurrentNode,
                                "Opening HEX file. Reason: " + c_ErrorText + ".");

         orc_StateHexFiles[u32_File].e_FileLoaded = eSUSEQ_STATE_ERROR;
         s32_Return = C_RD_WR;
      }
      else
      {
         orc_StateHexFiles[u32_File].e_FileLoaded = eSUSEQ_STATE_NO_ERR;

         //try to find signature address in hex file:
         s32_Return = c_Files[u32_File]->GetSignatureBlockAddress(c_SignatureAddresses[u32_File]);
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR, s32_Return, 0U, mc_CurrentNode,
                                   "Could not find signature block in HEX file.");
            orc_StateHexFiles[u32_File].e_SignatureBlockAddressFromFileRead = eSUSEQ_STATE_ERROR;
            s32_Return = C_CONFIG;
         }
         else
         {
            orc_StateHexFiles[u32_File].e_SignatureBlockAddressFromFileRead = eSUSEQ_STATE_NO_ERR;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      C_SclString c_DeviceName;
      uint8_t u8_NrCode;

      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START, C_NO_ERR, 10U, mc_CurrentNode,
                             "X-checking device name of device against HEX file contents ...");
      //get target device name for comparison with PC-side files:
      s32_Return = this->mpc_ComDriver->SendOsyReadDeviceName(mc_CurrentNode, c_DeviceName, &u8_NrCode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR, s32_Return, 10U,
                                mc_CurrentNode, "Could not read device name from device. Details:" +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }

      if (s32_Return == C_NO_ERR)
      {
         //for all files check whether:
         //* device type in file matches target's device type
         for (uint32_t u32_File = 0U; (u32_File < orc_FilesToFlash.size()) && (s32_Return == C_NO_ERR); u32_File++)
         {
            C_SclString c_DeviceNameHexFile;

            //get device ID from hex file
            s32_Return = c_Files[u32_File]->ScanDeviceIdFromHexFile(c_DeviceNameHexFile);
            if (s32_Return != C_NO_ERR)
            {
               C_SclString c_ErrorText = "Could not read device name from file " + orc_FilesToFlash[u32_File] +
                                         ". Reason: ";
               if (s32_Return == C_NOACT)
               {
                  c_ErrorText += "Device name not found.";
               }
               else if (s32_Return == C_CONFIG)
               {
                  c_ErrorText += "Ambiguous device names found.";
               }
               else
               {
                  c_ErrorText += "Undefined error.";
               }

               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR, s32_Return, 10U,
                                      mc_CurrentNode, c_ErrorText);
               orc_StateHexFiles[u32_File].e_NodeNameCompared = eSUSEQ_STATE_ERROR;
               s32_Return = C_NOACT;
            }
            else
            {
               bool q_IsSame = false;
               //Check actual device name
               if (c_DeviceName.Trim().UpperCase() == c_DeviceNameHexFile.Trim().UpperCase())
               {
                  orc_StateHexFiles[u32_File].e_NodeNameCompared = eSUSEQ_STATE_NO_ERR;
                  q_IsSame = true;
               }
               else
               {
                  //Check other accepted names
                  for (uint32_t u32_ItName = 0UL;
                       (u32_ItName < orc_OtherAcceptedDeviceNames.size()) && (q_IsSame == false); ++u32_ItName)
                  {
                     if (orc_OtherAcceptedDeviceNames[u32_ItName].Trim().UpperCase() ==
                         c_DeviceNameHexFile.Trim().UpperCase())
                     {
                        orc_StateHexFiles[u32_File].e_NodeNameCompared = eSUSEQ_STATE_NO_ERR;
                        q_IsSame = true;
                     }
                  }
               }
               if (q_IsSame == false)
               {
                  const C_SclString c_ErrorText = "Device names of device and HEX file " + orc_FilesToFlash[u32_File] +
                                                  " do not match. Device reported: \"" +
                                                  c_DeviceName.Trim().UpperCase() +
                                                  "\". HEX file contains: \"" +
                                                  c_DeviceNameHexFile.Trim().UpperCase() + "\".";
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR, C_OVERFLOW, 10U,
                                         mc_CurrentNode, c_ErrorText);

                  orc_StateHexFiles[u32_File].e_NodeNameCompared = eSUSEQ_STATE_ERROR;
                  s32_Return = C_OVERFLOW;
               }
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //for all files check whether:
      //* address ranges contained in file are available on the target
      //we need to enter the programming session for that:
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START, C_NO_ERR, 20U, mc_CurrentNode,
                             "Checking memory availability ...");
      if (orq_SetProgrammingMode == true)
      {
         // In the hole update sequence, setting the programming mode only one time
         s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode);
         orq_SetProgrammingMode = false;
      }
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR, s32_Return, 20U, mc_CurrentNode,
                                "Could not activate programming session.");

         if (s32_Return != C_CHECKSUM)
         {
            s32_Return = C_COM;
         }
      }
      else
      {
         for (uint32_t u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            //do we have enough space for the hex file data ?
            const stw::hex_file::C_HexDataDump * const pc_HexDump = c_Files[u32_File]->GetDataDump(u32_Return);
            if (u32_Return != stw::hex_file::NO_ERR)
            {
               C_SclString c_ErrorText;
               c_ErrorText = "Could not split up HEX file data of file " + orc_FilesToFlash[u32_File] +
                             " into handy chunks. Reason: " + c_Files[u32_File]->ErrorCodeToErrorText(u32_Return);
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR, C_RD_WR, 20U, mc_CurrentNode,
                                      c_ErrorText);
               orc_StateHexFiles[u32_File].e_DataDumpFromFileRead = eSUSEQ_STATE_ERROR;
               s32_Return = C_RD_WR;
               break;
            }
            else
            {
               orc_StateHexFiles[u32_File].e_DataDumpFromFileRead = eSUSEQ_STATE_NO_ERR;

               for (uint16_t u16_Area = 0U; u16_Area < pc_HexDump->at_Blocks.GetLength(); u16_Area++)
               {
                  uint8_t u8_NrCode;
                  s32_Return =
                     this->mpc_ComDriver->SendOsyCheckFlashMemoryAvailable(
                        mc_CurrentNode,
                        pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                        pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength(),
                        &u8_NrCode);
                  if (s32_Return != C_NO_ERR)
                  {
                     C_SclString c_ErrorText;
                     c_ErrorText.PrintFormatted("Could not get confirmation about flash memory availability. (File: %s" \
                                                " Offset: 0x%08x Size: 0x%08x). Details: %s",
                                                orc_FilesToFlash[u32_File].c_str(),
                                                pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                                                pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength(),
                                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                         u8_NrCode).c_str());
                     (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK, s32_Return, 20U,
                                            mc_CurrentNode, c_ErrorText);
                     s32_Return = C_COM;
                     break;
                  }
               }

               orc_StateHexFiles[u32_File].e_AvailableFlashMemoryChecked =
                  (s32_Return == C_NO_ERR) ? eSUSEQ_STATE_NO_ERR : eSUSEQ_STATE_ERROR;
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //all prerequisites checked; commence the flashing ...
      //write fingerprint
      s32_Return = this->m_WriteFingerPrintOsy();
      if (s32_Return == C_NO_ERR)
      {
         //now do the real flashing ...
         for (uint32_t u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            const stw::hex_file::C_HexDataDump * const pc_HexDump = c_Files[u32_File]->GetDataDump(u32_Return);
            //we would not have gotten here if we could not get a decent dump ...
            tgl_assert(pc_HexDump != NULL);

            if (pc_HexDump != NULL)
            {
               // Save file index
               this->mu32_CurrentFile = u32_File;

               //lint -e{613}  //see assertion
               s32_Return = m_FlashOneFileOpenSydeHex(*pc_HexDump, c_SignatureAddresses[u32_File],
                                                      ou32_RequestDownloadTimeout, ou32_TransferDataTimeout,
                                                      orc_StateHexFiles[u32_File]);
               if (s32_Return != C_NO_ERR)
               {
                  //error report is already in subfunction
                  break;
               }
            }
         }
      }
   }

   //clean up hex file instances:
   for (uint32_t u32_File = 0U; u32_File < c_Files.size(); u32_File++)
   {
      delete c_Files[u32_File];
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Flash one hex file to openSYDE address based node

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * driver is correctly set up
   * target is in programming session with security level 3 active
   * file was checked to be OK (valid hex file; signature present)
   * file matches the target device name

   Reports progress from 0..100

   \param[in]      orc_HexDataDump               Dump of file to flash
   \param[in]      ou32_SignatureAddress         address of signature block within hex file
   \param[in]      ou32_RequestDownloadTimeout   Maximum time in ms it can take to erase one continuous area in flash
   \param[in]      ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to flash
   \param[in,out]  orc_StateHexFile              State of hex file

   \return
   C_NO_ERR   file flashed
   C_COM      communication driver reported problem (details will be written to log file)
   C_BUSY     procedure aborted by user (as returned by m_ReportProgress)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_FlashOneFileOpenSydeHex(const stw::hex_file::C_HexDataDump & orc_HexDataDump,
                                                    const uint32_t ou32_SignatureAddress,
                                                    const uint32_t ou32_RequestDownloadTimeout,
                                                    const uint32_t ou32_TransferDataTimeout,
                                                    C_OscSuSequencesNodeHexFileStates & orc_StateHexFile)
{
   int32_t s32_Return = C_NO_ERR;
   uint32_t u32_TotalNumberOfBytes = 0U;
   uint32_t u32_TotalNumberOfBytesFlashed = 0U;

   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START, C_NO_ERR, 0U, mc_CurrentNode,
                          "Flashing HEX file ...");

   //get total number of bytes for progress calculations:
   for (int32_t s32_Area = 0U; s32_Area < orc_HexDataDump.at_Blocks.GetLength(); s32_Area++)
   {
      u32_TotalNumberOfBytes += static_cast<uint32_t>(orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength());
   }

   //flash all areas
   for (int32_t s32_Area = 0U; s32_Area < orc_HexDataDump.at_Blocks.GetLength(); s32_Area++)
   {
      const uint32_t u32_AreaSize = orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength();
      uint32_t u32_MaxBlockLength = 0U;
      uint8_t u8_NrCode;
      bool q_Abort;

      //calculate progress percentage:
      // (we just need a rough approximation; so integer calculation will suffice)
      tgl_assert(u32_TotalNumberOfBytes != 0U); //prerequisite for function: non-empty hex file
      // Prevent an overflow when file is bigger than 43MB
      //lint -e{414}  //see assertion
      uint8_t u8_ProgressPercentage =
         static_cast<uint8_t>((static_cast<uint64_t>(u32_TotalNumberOfBytesFlashed) * 100ULL) /
                              static_cast<uint64_t>(u32_TotalNumberOfBytes));
      q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START, C_NO_ERR, u8_ProgressPercentage,
                                 mc_CurrentNode,
                                 "Erasing flash memory for area " + C_SclString::IntToStr(s32_Area + 1) + "...");
      if (q_Abort == true)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_ABORTED, C_NO_ERR, u8_ProgressPercentage,
                                mc_CurrentNode, "Flashing HEX file aborted.");

         s32_Return = C_BUSY;
      }
      else
      {
         //set a proper timeout
         (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, ou32_RequestDownloadTimeout);

         s32_Return = this->mpc_ComDriver->SendOsyRequestDownload(
            mc_CurrentNode,
            orc_HexDataDump.at_Blocks[s32_Area].u32_AddressOffset,
            orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength(),
            u32_MaxBlockLength, &u8_NrCode);

         if (s32_Return != C_NO_ERR)
         {
            C_SclString c_Error;
            c_Error.PrintFormatted("Erasing flash memory for area %d failed (Offset: 0x%08X Size: 0x%08X). Details: %s",
                                   s32_Area + 1,
                                   orc_HexDataDump.at_Blocks[s32_Area].u32_AddressOffset,
                                   orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength(),
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                            u8_NrCode).c_str());
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR, s32_Return,
                                   u8_ProgressPercentage, mc_CurrentNode, c_Error);

            orc_StateHexFile.e_RequestDownloadSent = eSUSEQ_STATE_ERROR;
            s32_Return = C_COM;
         }
         else
         {
            orc_StateHexFile.e_RequestDownloadSent = eSUSEQ_STATE_NO_ERR;
         }
      }
      if (s32_Return == C_NO_ERR)
      {
         //perform the actual transfer
         uint8_t u8_BlockSequenceCounter = 1U;
         uint32_t u32_RemainingBytes = u32_AreaSize;
         std::vector<uint8_t> c_Data;
         const uint32_t u32_AdaptedTransferDataTimeout = m_GetAdaptedTransferDataTimeout(ou32_TransferDataTimeout,
                                                                                         u32_MaxBlockLength,
                                                                                         mc_CurrentNode.u8_BusIdentifier);

         //set a proper timeout
         (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, u32_AdaptedTransferDataTimeout);

         while (u32_RemainingBytes > 0U)
         {
            C_SclString c_Text;
            c_Text.PrintFormatted("Writing data for area %02d/%02d  byte %08d/%08d ...",
                                  s32_Area + 1, orc_HexDataDump.at_Blocks.GetLength(),
                                  u32_AreaSize - u32_RemainingBytes, u32_AreaSize);
            q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START, C_NO_ERR,
                                       u8_ProgressPercentage, mc_CurrentNode, c_Text);
            if (q_Abort == true)
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_ABORTED, C_NO_ERR, u8_ProgressPercentage,
                                      mc_CurrentNode, "Flashing HEX file aborted.");

               s32_Return = C_BUSY;
            }
            else
            {
               //subtract 4 bytes from the reported size; compensated for an issue in older server implementations
               // reporting an incorrect size; #62305
               if (u32_RemainingBytes > (u32_MaxBlockLength - 4U))
               {
                  c_Data.resize(static_cast<size_t>(u32_MaxBlockLength) - 4U);
               }
               else
               {
                  c_Data.resize(u32_RemainingBytes);
               }

               (void)memcpy(&c_Data[0],
                            &orc_HexDataDump.at_Blocks[s32_Area].au8_Data[static_cast<int32_t>(u32_AreaSize -
                                                                                               u32_RemainingBytes)],
                            c_Data.size());

               s32_Return = this->mpc_ComDriver->SendOsyTransferData(mc_CurrentNode, u8_BlockSequenceCounter, c_Data,
                                                                     &u8_NrCode);
               if (s32_Return == C_NO_ERR)
               {
                  u32_RemainingBytes -= static_cast<uint32_t>(c_Data.size());
                  u8_BlockSequenceCounter = (u8_BlockSequenceCounter < 0xFFU) ? (u8_BlockSequenceCounter + 1U) : 0x00U;
                  u32_TotalNumberOfBytesFlashed += static_cast<uint32_t>(c_Data.size());
                  // Prevent an overflow when file is bigger than 43MB
                  //lint -e{414}  //see assertion at initial assignment
                  u8_ProgressPercentage =
                     static_cast<uint8_t>((static_cast<uint64_t>(u32_TotalNumberOfBytesFlashed) * 100ULL) /
                                          static_cast<uint64_t>(u32_TotalNumberOfBytes));
               }
               else
               {
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR, s32_Return,
                                         u8_ProgressPercentage, mc_CurrentNode, "Could not write data. Details: " +
                                         C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                  u8_NrCode));
                  s32_Return = C_COM;
               }
            }
            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }

         orc_StateHexFile.e_AllTransferDataSent = (s32_Return == C_NO_ERR) ? eSUSEQ_STATE_NO_ERR : eSUSEQ_STATE_ERROR;
      }

      // Reset the timeout. The services with the specific timeouts are finished.
      (void)this->mpc_ComDriver->OsyResetPollingTimeout(mc_CurrentNode);

      if (s32_Return == C_NO_ERR)
      {
         //area transferred ...
         //report "final" status:
         C_SclString c_Text;
         c_Text.PrintFormatted("Writing data for area %02d/%02d  byte %08d/%08d ...",
                               s32_Area + 1, orc_HexDataDump.at_Blocks.GetLength(), u32_AreaSize, u32_AreaSize);
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START, C_NO_ERR,
                                u8_ProgressPercentage, mc_CurrentNode, c_Text);

         //if it's the last area we need to check the signature
         if (s32_Area == (orc_HexDataDump.at_Blocks.GetLength() - 1))
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START, C_NO_ERR,
                                   u8_ProgressPercentage, mc_CurrentNode,
                                   "Finalizing the final area and checking the signature ...");
            s32_Return = this->mpc_ComDriver->SendOsyRequestTransferExitAddressBased(mc_CurrentNode, true,
                                                                                     ou32_SignatureAddress, &u8_NrCode);
         }
         else
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START, C_NO_ERR,
                                   u8_ProgressPercentage, mc_CurrentNode, "Finalizing the area ...");
            s32_Return = this->mpc_ComDriver->SendOsyRequestTransferExitAddressBased(mc_CurrentNode, false, 0U,
                                                                                     &u8_NrCode);
         }

         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR, s32_Return,
                                   u8_ProgressPercentage, mc_CurrentNode, "Could not finalize the area. Details: " +
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
            orc_StateHexFile.e_RequestTransferAddressExitSent = eSUSEQ_STATE_ERROR;
            s32_Return = C_COM;
         }
         else
         {
            orc_StateHexFile.e_RequestTransferAddressExitSent = eSUSEQ_STATE_NO_ERR;
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         break;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED, C_NO_ERR, 100U, mc_CurrentNode,
                             "Flashing HEX file finished.");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Flash one openSYDE file based node

   Writes one or more files.

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * list of files must be > 0
   * files must be present on client side file system
   * node is active

   * Reports progress from 0..100 for the overall process
   * Reports 0..100 for each file being flashed

   \param[in]      orc_FilesToFlash              Files to write
   \param[in]      ou32_RequestDownloadTimeout   Maximum time in ms it can take to prepare one file on the target file
   \param[in]      ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to the target
                                                  file
   \param[in]      orc_ProtocolFeatures          available protocol features
   \param[in,out]  orq_SetProgrammingMode        In: Flag if programming mode must be set.
                                                 Out: Flag if programming mode was set.
   \param[out]     orc_StateOtherFiles           States of all handled files

   \return
   C_NO_ERR    flashed all files

   C_RD_WR     one of the files is not a valid Intel or Motorola hex file
               could not split up hex file in individual areas
   C_CONFIG    no signature block found in hex file
   C_COM       communication driver reported problem (details will be written to log file)
   C_NOACT     could not extract device name from hex file
   C_OVERFLOW  device name of device does not match name contained in hex file
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_FlashNodeOpenSydeFile(const std::vector<C_SclString> & orc_FilesToFlash,
                                                  const uint32_t ou32_RequestDownloadTimeout,
                                                  const uint32_t ou32_TransferDataTimeout,
                                                  const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                                  bool & orq_SetProgrammingMode,
                                                  std::vector<C_OscSuSequencesNodeOtherFileStates> & orc_StateOtherFiles)
{
   int32_t s32_Return = C_NO_ERR;

   //start the actual transfers
   //we need to enter the programming session for that:
   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START, C_NO_ERR, 20U, mc_CurrentNode,
                          "Checking memory availability ...");
   if (orq_SetProgrammingMode == true)
   {
      // In the hole update sequence, setting the programming mode only one time
      s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode);
      orq_SetProgrammingMode = false;
   }
   if (s32_Return != C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR, s32_Return, 20U, mc_CurrentNode,
                             "Could not activate programming session.");
      if (s32_Return != C_CHECKSUM)
      {
         s32_Return = C_COM;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //all prerequisites checked; commence the flashing ...
      //write fingerprint
      s32_Return = this->m_WriteFingerPrintOsy();
      if (s32_Return == C_NO_ERR)
      {
         //now do the real flashing ...
         for (uint32_t u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            // Save file index
            this->mu32_CurrentFile = u32_File;

            s32_Return = m_FlashOneFileOpenSydeFile(orc_FilesToFlash[u32_File], ou32_RequestDownloadTimeout,
                                                    ou32_TransferDataTimeout, orc_ProtocolFeatures,
                                                    orc_StateOtherFiles[u32_File]);
            if (s32_Return != C_NO_ERR)
            {
               //error report is already in subfunction
               break;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Flash one file to openSYDE file based node

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * driver is correctly set up
   * target is in programming session with security level 3 active
   * file was checked to be present

   Reports progress from 0..100

   When reporting the file name to the target only the file name of the file will be transferred.
   (i.e. path information will be removed).

   \param[in]     orc_FileToFlash               File to write
   \param[in]     ou32_SignatureAddress         address of signature block within hex file
   \param[in]     ou32_RequestDownloadTimeout   Maximum time in ms it can take to prepare one file on the target file
   \param[in]     ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to the target
                                                 file
   \param[in]     orc_ProtocolFeatures          available protocol features
   \param[in,out] orc_StateOtherFile            State of file

   \return
   C_NO_ERR   file flashed
   C_COM      communication driver reported problem (details will be written to log file)
   C_RD_WR    could not read from input file
   C_BUSY     procedure aborted by user (as returned by m_ReportProgress)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_FlashOneFileOpenSydeFile(const C_SclString & orc_FileToFlash,
                                                     const uint32_t ou32_RequestDownloadTimeout,
                                                     const uint32_t ou32_TransferDataTimeout,
                                                     const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                                     C_OscSuSequencesNodeOtherFileStates & orc_StateOtherFile)
{
   int32_t s32_Return = C_NO_ERR;
   int32_t s32_FileApiReturn; //2GB file limit will be fine for us
   uint32_t u32_TotalNumberOfBytes = 0U;

   std::FILE * pc_File;
   uint32_t u32_MaxBlockLength = 0U;
   uint8_t u8_NrCode;
   uint32_t u32_TransferCrc = 0xFFFFFFFFU;
   uint8_t u8_ProgressPercentage = 0U;

   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START, C_NO_ERR, 0U, mc_CurrentNode,
                          "Transferring file ...");
   pc_File = std::fopen(orc_FileToFlash.c_str(), "rb");
   if (pc_File == NULL)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      //get total number of bytes for progress calculations:
      s32_FileApiReturn = std::fseek(pc_File, 0, SEEK_END);
      if (s32_FileApiReturn != 0)
      {
         s32_Return = C_RD_WR;
         (void)std::fclose(pc_File);
         pc_File = NULL;
      }
      else
      {
         s32_FileApiReturn = std::ftell(pc_File);
         if (s32_FileApiReturn < 0)
         {
            s32_Return = C_RD_WR;
            (void)std::fclose(pc_File);
            pc_File = NULL;
         }
         else
         {
            //s32_FileApiReturn already checked for negative value (so we can only flash files up to 2GB)
            u32_TotalNumberOfBytes = static_cast<uint32_t>(s32_FileApiReturn);
            std::rewind(pc_File); //start over ...
         }
      }
   }

   //flash file
   if (s32_Return == C_NO_ERR)
   {
      const bool q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START, C_NO_ERR, 0U,
                                            mc_CurrentNode,
                                            "Preparing file system for file \"" +
                                            TglExtractFileName(orc_FileToFlash) + "\"...");

      orc_StateOtherFile.e_FileLoaded = eSUSEQ_STATE_NO_ERR;

      if (q_Abort == true)
      {
         s32_Return = C_BUSY;
      }
      else
      {
         //set a proper timeout
         (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, ou32_RequestDownloadTimeout);

         s32_Return = this->mpc_ComDriver->SendOsyRequestFileTransfer(
            mc_CurrentNode, TglExtractFileName(orc_FileToFlash), u32_TotalNumberOfBytes, u32_MaxBlockLength,
            &u8_NrCode);

         if (s32_Return != C_NO_ERR)
         {
            C_SclString c_Error;
            c_Error.PrintFormatted("Preparing file system for file \"%s\" failed. Details: %s",
                                   TglExtractFileName(orc_FileToFlash).c_str(),
                                   C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                            u8_NrCode).c_str());
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR, s32_Return,
                                   0U, mc_CurrentNode, c_Error);

            orc_StateOtherFile.e_RequestFileTransferSent = eSUSEQ_STATE_ERROR;
            s32_Return = C_COM;
         }
         else
         {
            orc_StateOtherFile.e_RequestFileTransferSent = eSUSEQ_STATE_NO_ERR;
         }
      }

      if ((s32_Return != C_NO_ERR) && (pc_File != NULL))
      {
         (void)std::fclose(pc_File);
         pc_File = NULL;
      }
   }
   else
   {
      orc_StateOtherFile.e_FileLoaded = eSUSEQ_STATE_ERROR;
   }

   if (s32_Return == C_NO_ERR)
   {
      //perform the actual transfer
      uint8_t u8_BlockSequenceCounter = 1U;
      uint32_t u32_RemainingBytes = u32_TotalNumberOfBytes;
      std::vector<uint8_t> c_Data;
      uint32_t u32_TotalNumberOfBytesFlashed = 0U;
      const uint32_t u32_AdaptedTransferDataTimeout = m_GetAdaptedTransferDataTimeout(ou32_TransferDataTimeout,
                                                                                      u32_MaxBlockLength,
                                                                                      mc_CurrentNode.u8_BusIdentifier);

      //set a proper timeout
      (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, u32_AdaptedTransferDataTimeout);

      while (u32_RemainingBytes > 0U)
      {
         C_SclString c_Text;
         bool q_Abort;
         tgl_assert(u32_TotalNumberOfBytes != 0U); //prerequisite for function: non-empty hex file
         // Prevent an overflow when file is bigger than 43MB
         //lint -e{414}  //see assertion
         u8_ProgressPercentage = static_cast<uint8_t>((static_cast<uint64_t>(u32_TotalNumberOfBytesFlashed) * 100ULL) /
                                                      static_cast<uint64_t>(u32_TotalNumberOfBytes));

         c_Text.PrintFormatted("Writing data byte %08d/%08d ...", u32_TotalNumberOfBytesFlashed,
                               u32_TotalNumberOfBytes);
         q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START, C_NO_ERR,
                                    u8_ProgressPercentage, mc_CurrentNode, c_Text);
         if (q_Abort == true)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_ABORTED, C_NO_ERR, u8_ProgressPercentage,
                                   mc_CurrentNode, "Flashing file aborted.");
            s32_Return = C_BUSY;
         }
         else
         {
            //subtract 4 bytes from the reported size; compensated for an issue in older server implementations
            // reporting an incorrect size; #62305
            if (u32_RemainingBytes > (u32_MaxBlockLength - 4U))
            {
               c_Data.resize(static_cast<size_t>(u32_MaxBlockLength) - 4U);
            }
            else
            {
               c_Data.resize(u32_RemainingBytes);
            }

            //lint -e{668}  //file cannot be NULL if we get here
            s32_FileApiReturn =
               static_cast<int32_t>(std::fread(&c_Data[0], 1U, static_cast<size_t>(c_Data.size()), pc_File));
            if (s32_FileApiReturn != static_cast<int32_t>(c_Data.size()))
            {
               //it's not ideal that we have to abort in the middle of the procedure
               //an alternative would be to read in the file before starting
               //But the files might get large and the client might be limited in RAM.
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR,
                                      s32_Return, u8_ProgressPercentage, mc_CurrentNode,
                                      "Could not read from input file.");
               s32_Return = C_RD_WR;
            }
            else
            {
               s32_Return = this->mpc_ComDriver->SendOsyTransferData(mc_CurrentNode, u8_BlockSequenceCounter, c_Data,
                                                                     &u8_NrCode);
               if (s32_Return == C_NO_ERR)
               {
                  //update continuous CRC:
                  C_SclChecksums::CalcCRC32(&c_Data[0], static_cast<uint32_t>(c_Data.size()), u32_TransferCrc);

                  u32_RemainingBytes -= static_cast<uint32_t>(c_Data.size());
                  u8_BlockSequenceCounter =
                     (u8_BlockSequenceCounter < 0xFFU) ? (u8_BlockSequenceCounter + 1U) : 0x00U;
                  u32_TotalNumberOfBytesFlashed += static_cast<uint32_t>(c_Data.size());
               }
               else
               {
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR,
                                         s32_Return, u8_ProgressPercentage, mc_CurrentNode,
                                         "Could not write data. Details: " +
                                         C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                  u8_NrCode));
                  s32_Return = C_COM;
               }
            }
         }
         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }

      orc_StateOtherFile.e_AllTransferDataSent = (s32_Return == C_NO_ERR) ? eSUSEQ_STATE_NO_ERR : eSUSEQ_STATE_ERROR;

      // File is not necessary anymore. Close it.
      if (pc_File != NULL)
      {
         (void)std::fclose(pc_File);
         pc_File = NULL;
      }
   }

   // Reset the timeout. The services with the specific timeouts are finished.
   (void)this->mpc_ComDriver->OsyResetPollingTimeout(mc_CurrentNode);

   if (s32_Return == C_NO_ERR)
   {
      //area transferred ...
      //report "final" status:
      C_SclString c_Text;
      bool q_RejectedByTargetLayer = false;
      c_Text.PrintFormatted("Writing data byte %08d/%08d ...", u32_TotalNumberOfBytes,
                            u32_TotalNumberOfBytes);
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START, C_NO_ERR,
                             u8_ProgressPercentage, mc_CurrentNode, c_Text);

      //finalize transfer:
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START, C_NO_ERR,
                             u8_ProgressPercentage, mc_CurrentNode,
                             "Finalizing the transfer and checking the transfer CRC ...");
      //finalize transfer CRC:
      u32_TransferCrc ^= 0xFFFFFFFFU;

      s32_Return =
         this->mpc_ComDriver->SendOsyRequestTransferExitFileBased(mc_CurrentNode, u32_TransferCrc, &u8_NrCode);
      if (s32_Return != C_NO_ERR)
      {
         orc_StateOtherFile.e_RequestTransferFileExitSent = eSUSEQ_STATE_ERROR;
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR, s32_Return,
                                u8_ProgressPercentage, mc_CurrentNode, "Could not finalize the transfer. Details: " +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         if ((s32_Return == C_WARN) && (u8_NrCode == C_OscProtocolDriverOsy::hu8_NR_CODE_GENERAL_PROGRAMMING_FAILURE))
         {
            q_RejectedByTargetLayer = true;
         }
         s32_Return = C_COM;
      }
      else
      {
         orc_StateOtherFile.e_RequestTransferFileExitSent = eSUSEQ_STATE_NO_ERR;
      }

      //if the target supports reading the "FileBasedTransferExitResult" and the result of "TransferExit" was either
      // "OK" or "rejected by target layer" then use that service to read string information about result
      if (orc_ProtocolFeatures.q_FileBasedTransferExitResultAvailable == true)
      {
         C_SclString c_TransferExitResult;
         if ((s32_Return == C_NO_ERR) || (q_RejectedByTargetLayer == true))
         {
            //do not overwrite s32_Return; this function shall still fail if finalize failed
            int32_t s32_LocalReturn;
            s32_LocalReturn = this->mpc_ComDriver->SendOsyRequestFileBasedTransferExitResult(
               mc_CurrentNode, c_TransferExitResult, &u8_NrCode);
            if (s32_LocalReturn != C_NO_ERR)
            {
               orc_StateOtherFile.e_RequestTransferFileExitResultSent = eSUSEQ_STATE_ERROR;
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR, s32_LocalReturn,
                                      u8_ProgressPercentage, mc_CurrentNode,
                                      "Could not read exit result text. Details: " +
                                      C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_LocalReturn,
                                                                                               u8_NrCode));
               s32_Return = C_COM;
               c_TransferExitResult = "unkown (reading information failed)";
            }
            else
            {
               orc_StateOtherFile.e_RequestTransferFileExitResultSent = eSUSEQ_STATE_NO_ERR;
            }
         }
         else
         {
            orc_StateOtherFile.e_RequestTransferFileExitResultSent = eSUSEQ_STATE_NOT_EXECUTED;
            c_TransferExitResult = "unkown (reading information not supported by server device)";
         }
         m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_RESULT_STRING, C_NO_ERR, 100U, mc_CurrentNode,
                          "Result of file transfer: \"" + c_TransferExitResult + "\"");
      }
      else
      {
         orc_StateOtherFile.e_RequestTransferFileExitResultSent = eSUSEQ_STATE_NOT_NEEDED;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED, C_NO_ERR, 100U, mc_CurrentNode,
                             "Flashing file finished.");
   }

   return s32_Return;
} //lint !e593 //pc_File is closed in all paths; looks like a false positive

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write one or more NVM parameter set file(s) to openSYDE node

   Files to be written must be in valid openSYDE .psi_syde file format

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * list of files must be > 0
   * node is active

   * Reports progress from 0..100 for the overall process

   Sequence performed:
   * check server for MaxNumberOfBlockLength
   * set received MaxNumberOfBlockLength in installed TP
   * for all files to write
   ** open file
   ** write file content to device

   Strategy for setting up required "DataDealer":
    To set up the DataDealerNvm we need to set up a DiagProtocol; but we only have a ProtocolDriverOsy
    So we create a DiagProtocol and copy over the known settings from the already up-and-running ProtocolDriverOsy.

   \param[in]      orc_FilesToWrite              Files to write to NVM
   \param[in]      orc_ProtocolFeatures          Information about available protocol features
   \param[in]      oq_SetProgrammingMode         In: Flag if programming mode must be set.
   \param[out]     orc_StatePsiFiles             States of all handled psi files

   \return
   C_NO_ERR    all files were written
   C_RD_WR     one of the files is not a valid .psi_syde file or does not exist
   C_CONFIG    one of the files contains data for zero or more than one device (expected: data for exactly one device)
   C_DEFAULT   one of the files is present but checksum is invalid
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
   C_COM       communication driver reported problem (details will be written to log file)
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
   C_RANGE     At least one feature of the openSYDE Flashloader is not available for NVM writing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_WriteNvmOpenSyde(const std::vector<C_SclString> & orc_FilesToWrite,
                                             const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                             const bool oq_SetProgrammingMode,
                                             std::vector<C_OscSuSequencesNodePsiFileStates> & orc_StatePsiFiles)
{
   int32_t s32_Return = C_NO_ERR;

   //Get pointer to OSY protocol driver provided by comm driver:
   C_OscProtocolDriverOsyTpBase * const pc_TransportProtocol = mpc_ComDriver->GetOsyTransportProtocol(mu32_CurrentNode);

   tgl_assert(pc_TransportProtocol != NULL);
   if (pc_TransportProtocol != NULL)
   {
      uint16_t u16_MaxBlockLength = 0U;
      C_OscProtocolDriverOsyNode c_Client;
      C_OscProtocolDriverOsyNode c_Server;
      C_OscDiagProtocolOsy c_DiagProtocol;
      C_OscNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[mu32_CurrentNode];

      //get node-IDs from ProtocolDriver and set in DiagProtocol:
      pc_TransportProtocol->GetNodeIdentifiers(c_Client, c_Server);
      s32_Return = c_DiagProtocol.SetNodeIdentifiers(c_Client, c_Server);
      tgl_assert(s32_Return == C_NO_ERR);
      //Set transport protocol in DiagProtocol:
      s32_Return = c_DiagProtocol.SetTransportProtocol(pc_TransportProtocol);
      tgl_assert(s32_Return == C_NO_ERR);

      //set up DataDealer:
      C_OscDataDealerNvmSafe c_Dealer(&rc_Node, mu32_CurrentNode, &c_DiagProtocol);

      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Writing parameter set image files ...");

      //do we have the required features ?
      if ((orc_ProtocolFeatures.q_FlashloaderCanWriteToNvm == false) ||
          (orc_ProtocolFeatures.q_MaxNumberOfBlockLengthAvailable == false))
      {
         s32_Return = C_RANGE;
         // Both features are necessary to write NVM files to flashloader
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR, s32_Return, 5U,
                                mc_CurrentNode,
                                "The node has not the Flashloader feature to write a Parameter Set Image file.");
      }

      if (s32_Return == C_NO_ERR)
      {
         if (oq_SetProgrammingMode == true)
         {
            // In the hole update sequence, setting the programming mode only one time
            s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode);
            // Last step with this security level in this sequence
         }
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR, s32_Return, 5U, mc_CurrentNode,
                                   "Could not activate programming session.");
            if (s32_Return != C_CHECKSUM)
            {
               s32_Return = C_COM;
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         //check server for MaxNumberOfBlockLength
         s32_Return = c_DiagProtocol.OsyReadMaxNumberOfBlockLength(u16_MaxBlockLength);
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR, s32_Return, 5U, mc_CurrentNode,
                                   "Could not get max number of block length from device.");
            //this information is not available in older Flashloaders
            //the application is responsible to not try this sequence for those servers
            s32_Return = C_COM;
         }
      }
      if (s32_Return == C_NO_ERR)
      {
         //set received MaxNumberOfBlockLength in installed TP
         c_DiagProtocol.SetMaxServiceSize(u16_MaxBlockLength);

         for (uint16_t u16_File = 0U; u16_File < orc_FilesToWrite.size(); u16_File++)
         {
            C_OscSuSequencesNodePsiFileStates & rc_State = orc_StatePsiFiles[u16_File];
            //logic: split 80% (between 10% and 90%) by the number of files
            const float32_t f32_Percent = (((80.0F / static_cast<float32_t>(orc_FilesToWrite.size()))) *
                                           static_cast<float32_t>(u16_File));
            const uint8_t u8_Percent = 10U + static_cast<uint8_t>(f32_Percent);
            bool q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_START, C_NO_ERR, u8_Percent,
                                            mc_CurrentNode,
                                            "Reading parameter set image file \"" + orc_FilesToWrite[u16_File] +
                                            "\"...");
            if (q_Abort == true)
            {
               s32_Return = C_BUSY;
            }
            else
            {
               s32_Return = c_Dealer.NvmSafeReadFileWithCrc(orc_FilesToWrite[u16_File]);
               if (s32_Return != C_NO_ERR)
               {
                  rc_State.e_FileLoaded = eSUSEQ_STATE_ERROR;
                  if (s32_Return == C_CHECKSUM)
                  {
                     // Remap to have a unique return value for this case
                     s32_Return = C_DEFAULT;
                  }
               }
               else
               {
                  rc_State.e_FileLoaded = eSUSEQ_STATE_NO_ERR;
               }
            }
            if (s32_Return != C_NO_ERR)
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR, s32_Return, u8_Percent,
                                      mc_CurrentNode,
                                      "Could not read parameter set image file \"" + orc_FilesToWrite[u16_File] +
                                      "\"!");
            }
            else
            {
               q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_START, C_NO_ERR,
                                          u8_Percent, mc_CurrentNode,
                                          "Writing data of parameter set image file \"" +
                                          orc_FilesToWrite[u16_File] + "\" to device ...");
               if (q_Abort == true)
               {
                  s32_Return = C_BUSY;
               }
               else
               {
                  int32_t s32_ResultDetail;
                  s32_Return = c_Dealer.NvmSafeWriteParameterSetFile(orc_FilesToWrite[u16_File], s32_ResultDetail);
               }
               if (s32_Return != C_NO_ERR)
               {
                  rc_State.e_PsiFileWrote = eSUSEQ_STATE_ERROR;
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR, s32_Return, u8_Percent,
                                         mc_CurrentNode,
                                         "Could not write data of parameter set image file \"" +
                                         orc_FilesToWrite[u16_File] + "\" to device !");
                  //C_OVERFLOW, C_CONFIG: would be a systematic error in this implementation
                  //rest: C_COM
                  s32_Return = C_COM;
               }
               else
               {
                  rc_State.e_PsiFileWrote = eSUSEQ_STATE_NO_ERR;
               }
            }
            if (s32_Return == C_NO_ERR)
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED, s32_Return, u8_Percent,
                                      mc_CurrentNode, "Writing parameter set image file to device finished.");
            }
            else
            {
               break;
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FINISHED, s32_Return, 100U, mc_CurrentNode,
                                "Writing parameter set image file(s) to device finished.");
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write one PEM file to openSYDE node

   File to be written must be in valid .pem file format

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * orc_FileToWrite must be a valid file path
   * node is active

   * Reports progress from 0..100 for the overall process

   \param[in]      orc_FileToWrite               PEM file to write
   \param[in]      orc_ProtocolFeatures          Information about available protocol features
   \param[in,out]  orq_SetProgrammingMode        In: Flag if programming mode must be set.
                                                 Out: Flag if programming mode was set.
   \param[out]     orc_StatePemFile              State of PEM file

   \return
   C_NO_ERR    file was written or nothing to do
   C_RD_WR     the file is not a valid .pem file or does not exist
   C_WARN      the file is present but key details of PEM file could not be extracted
   C_COM       communication driver reported problem (details will be written to log file)
   C_RANGE     At least one feature of the openSYDE Flashloader is not available for NVM writing
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_WritePemOpenSydeFile(const stw::scl::C_SclString & orc_FileToWrite,
                                                 const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures,
                                                 bool & orq_SetProgrammingMode,
                                                 C_OscSuSequencesNodePemFileStates & orc_StatePemFile)
{
   int32_t s32_Return;

   if (orc_FileToWrite != "")
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Writing PEM file ...");

      if (orc_ProtocolFeatures.q_SupportsSecurity == false)
      {
         s32_Return = C_RANGE;
         // Security feature is necessary to write PEM file to flashloader
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR, s32_Return, 5U,
                                mc_CurrentNode,
                                "The node has not the Flashloader features to support security and to write PEM files.");
      }
      else
      {
         C_OscSecurityPem c_PemFile;
         std::string c_ErrorMessage;

         s32_Return = c_PemFile.LoadFromFile(orc_FileToWrite.c_str(), c_ErrorMessage);

         if (s32_Return == C_NO_ERR)
         {
            const std::vector<uint8_t> c_PubKeyDecoded = c_PemFile.GetKeyInfo().GetPubKeyTextDecoded();
            std::vector<uint8_t> c_PubKeyModulus;
            std::vector<uint8_t> c_PubKeyExponent;

            orc_StatePemFile.e_FileLoaded = eSUSEQ_STATE_NO_ERR;

            s32_Return = C_OscSecurityPem::h_ExtractModulusAndExponent(c_PubKeyDecoded, c_PubKeyModulus,
                                                                       c_PubKeyExponent,
                                                                       c_ErrorMessage);

            if (s32_Return == C_NO_ERR)
            {
               orc_StatePemFile.e_PemFileExtracted = eSUSEQ_STATE_NO_ERR;

               if (orq_SetProgrammingMode == true)
               {
                  // In the hole update sequence, setting the programming mode only one time
                  const uint8_t u8_SECURITY_LEVEL = 1U;
                  s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode, &u8_SECURITY_LEVEL);
                  orq_SetProgrammingMode = false;
               }
               if (s32_Return != C_NO_ERR)
               {
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR, s32_Return, 10U,
                                         mc_CurrentNode,
                                         "Could not activate programming session.");
                  if (s32_Return != C_CHECKSUM)
                  {
                     s32_Return = C_COM;
                  }
               }
            }
            else
            {
               orc_StatePemFile.e_PemFileExtracted = eSUSEQ_STATE_ERROR;
            }

            if (s32_Return == C_NO_ERR)
            {
               const std::vector<uint8_t> c_KeySerialNumber = c_PemFile.GetKeyInfo().GetPubKeySerialNumber();
               uint8_t u8_NrCode;

               s32_Return = this->mpc_ComDriver->SendOsyWriteSecurityKey(this->mc_CurrentNode, c_PubKeyModulus,
                                                                         c_PubKeyExponent,
                                                                         c_KeySerialNumber,
                                                                         &u8_NrCode);

               if (s32_Return != C_NO_ERR)
               {
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR, s32_Return,
                                         75U, mc_CurrentNode,
                                         "Could not write security key. Details: " +
                                         C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                  u8_NrCode));
                  orc_StatePemFile.e_SecurityKeySent = eSUSEQ_STATE_ERROR;
                  s32_Return = C_COM;
               }
               else
               {
                  orc_StatePemFile.e_SecurityKeySent = eSUSEQ_STATE_NO_ERR;
               }
            }
            else
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR, s32_Return,
                                      50U, mc_CurrentNode,
                                      "Could not load PEM file. Details: " + c_ErrorMessage);
               s32_Return = C_WARN;
            }
         }
         else
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR, s32_Return,
                                   25U, mc_CurrentNode,
                                   "Could not extract security key from decoded public key. Details: " +
                                   c_ErrorMessage);
            orc_StatePemFile.e_FileLoaded = eSUSEQ_STATE_ERROR;
            s32_Return = C_RD_WR;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_FINISHED, s32_Return, 100U, mc_CurrentNode,
                                "Writing PEM file to device finished.");
      }
   }
   else
   {
      // Nothing to do, no error
      orc_StatePemFile.e_FileLoaded = eSUSEQ_STATE_NOT_NEEDED;
      orc_StatePemFile.e_SecurityKeySent = eSUSEQ_STATE_NOT_NEEDED;
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write device states to openSYDE node

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * node is active

   \param[in]      orc_ApplicationsToWrite       Update configuration with all states for sending or not sending
   \param[in]      orc_ProtocolFeatures          Information about available protocol features
   \param[in,out]  orq_SetProgrammingMode        In: Flag if programming mode must be set.
                                                 Out: Flag if programming mode was set.
   \param[out]     orc_StatePemFile              State of PEM file and its associated handled states

   \return
   C_NO_ERR    all states were written or no write process was necessary
   C_COM       communication driver reported problem (details will be written to log file)
   C_RANGE     At least one feature of the openSYDE Flashloader is not available for state writing
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_WriteOpenSydeNodeStates(const C_OscSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                                    const C_OscProtocolDriverOsy::C_ListOfFeatures & orc_ProtocolFeatures, bool & orq_SetProgrammingMode,
                                                    C_OscSuSequencesNodePemFileStates & orc_StatePemFile)
{
   int32_t s32_Return = C_NO_ERR;
   const uint8_t u8_SECURITY_LEVEL = 1U;

   // Security state
   if (orc_ApplicationsToWrite.q_SendSecurityEnabledState == true)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Writing security activation ...");

      if (orc_ProtocolFeatures.q_SupportsSecurity == true)
      {
         if (orq_SetProgrammingMode == true)
         {
            // In the hole update sequence, setting the programming mode only one time
            s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode, &u8_SECURITY_LEVEL);
            orq_SetProgrammingMode = false;
         }
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR, s32_Return, 10U,
                                   mc_CurrentNode,
                                   "Could not activate programming session.");
            if (s32_Return != C_CHECKSUM)
            {
               s32_Return = C_COM;
            }
         }

         if (s32_Return == C_NO_ERR)
         {
            uint8_t u8_NrCode;
            // Only RSA 1024 supported at the moment, so 0 for security algorithm
            s32_Return = this->mpc_ComDriver->SendOsyWriteSecurityActivation(this->mc_CurrentNode,
                                                                             orc_ApplicationsToWrite.q_SecurityEnabled,
                                                                             0U, &u8_NrCode);

            if (s32_Return == C_NO_ERR)
            {
               orc_StatePemFile.e_SecurityFlagSent = eSUSEQ_STATE_NO_ERR;
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_FINISHED, s32_Return, 100U,
                                      mc_CurrentNode, "Writing security activation to device finished.");
            }
            else
            {
               orc_StatePemFile.e_SecurityFlagSent = eSUSEQ_STATE_ERROR;
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR, s32_Return,
                                      50U, mc_CurrentNode,
                                      "Could not write security activation. Details: " +
                                      C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                               u8_NrCode));
               s32_Return = C_COM;
            }
         }
      }
      else
      {
         // Security feature is necessary to write security activation state to flashloader
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR, s32_Return, 5U,
                                mc_CurrentNode,
                                "The node has not the Flashloader features to support security.");

         s32_Return = C_RANGE;
      }
   }
   else
   {
      orc_StatePemFile.e_SecurityFlagSent = eSUSEQ_STATE_NOT_NEEDED;
   }

   // Debugger state
   if (orc_ApplicationsToWrite.q_SendDebuggerEnabledState == true)
   {
      if (s32_Return == C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_START, C_NO_ERR, 0U, mc_CurrentNode,
                                "Writing debugger state ...");

         if (((orc_ProtocolFeatures.q_SupportsDebuggerOn == true) &&
              (orc_ApplicationsToWrite.q_DebuggerEnabled == true)) ||
             ((orc_ProtocolFeatures.q_SupportsDebuggerOff == true) &&
              (orc_ApplicationsToWrite.q_DebuggerEnabled == false)))
         {
            if (orq_SetProgrammingMode == true)
            {
               // In the hole update sequence, setting the programming mode only one time
               s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode, &u8_SECURITY_LEVEL);
               // Last time setting of ProgrammingMode in this sequence
            }
            if (s32_Return != C_NO_ERR)
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR, s32_Return, 10U,
                                      mc_CurrentNode,
                                      "Could not activate programming session.");
               if (s32_Return != C_CHECKSUM)
               {
                  s32_Return = C_COM;
               }
            }

            if (s32_Return == C_NO_ERR)
            {
               uint8_t u8_NrCode;
               s32_Return = this->mpc_ComDriver->SendOsyWriteDebuggerEnabled(this->mc_CurrentNode,
                                                                             orc_ApplicationsToWrite.q_DebuggerEnabled,
                                                                             &u8_NrCode);

               if (s32_Return == C_NO_ERR)
               {
                  orc_StatePemFile.e_DebuggerFlagSent = eSUSEQ_STATE_NO_ERR;
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_FINISHED, s32_Return, 100U,
                                         mc_CurrentNode, "Writing debugger state to device finished.");
               }
               else
               {
                  orc_StatePemFile.e_DebuggerFlagSent = eSUSEQ_STATE_ERROR;
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR, s32_Return,
                                         50U, mc_CurrentNode,
                                         "Could not write debugger state. Details: " +
                                         C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                  u8_NrCode));
                  s32_Return = C_COM;
               }
            }
         }
         else
         {
            // Debugger state change feature is necessary to write debugger state to flashloader
            if (orc_ApplicationsToWrite.q_DebuggerEnabled == true)
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR, s32_Return,
                                      5U,
                                      mc_CurrentNode,
                                      "The node has not the Flashloader feature to enable the debugger.");
            }
            else
            {
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR, s32_Return,
                                      5U,
                                      mc_CurrentNode,
                                      "The node has not the Flashloader feature to disable the debugger.");
            }
            s32_Return = C_RANGE;
         }
      }
   }
   else
   {
      orc_StatePemFile.e_DebuggerFlagSent = eSUSEQ_STATE_NOT_NEEDED;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write openSYDE flashing fingerprint

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * server node must be in programming session

   * Reports progress as 30

   \return
   C_NO_ERR    flashed all files
   C_COM       error result from device (see log for details)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_WriteFingerPrintOsy(void)
{
   //all prerequisites checked; commence the flashing ...
   //write fingerprint
   const C_SclDateTime c_Now = C_SclDateTime::Now();
   uint8_t au8_Date[3];
   uint8_t au8_Time[3];
   C_SclString c_UserName;
   bool q_Return;
   uint8_t u8_NrCode;
   int32_t s32_Return;

   au8_Date[0] = static_cast<uint8_t>(c_Now.mu16_Year % 1000U);
   au8_Date[1] = static_cast<uint8_t>(c_Now.mu16_Month);
   au8_Date[2] = static_cast<uint8_t>(c_Now.mu16_Day);
   au8_Time[0] = static_cast<uint8_t>(c_Now.mu16_Hour);
   au8_Time[1] = static_cast<uint8_t>(c_Now.mu16_Minute);
   au8_Time[2] = static_cast<uint8_t>(c_Now.mu16_Second);

   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START, C_NO_ERR, 30U, mc_CurrentNode,
                          "Writing fingerprint ...");
   q_Return = stw::tgl::TglGetSystemUserName(c_UserName);
   if (q_Return != true)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE, C_WARN, 30U, mc_CurrentNode,
                             "Could not get user name from system. Using \"unknown\".");
      c_UserName = "unknown";
   }

   s32_Return = this->mpc_ComDriver->SendOsyWriteApplicationSoftwareFingerprint(mc_CurrentNode, au8_Date, au8_Time,
                                                                                c_UserName, &u8_NrCode);
   if (s32_Return != C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR, s32_Return, 30U, mc_CurrentNode,
                             "Could not write fingerprint. Details:" +
                             C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
      s32_Return = C_COM;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Flash one STW Flashloader based node

   Flash one or more files.

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * server node must be in Flashloader mode
   * list of files must be > 0
   * files must be present and in Intel or Motorola hex format
   * node is active

   * Reports progress from 0..100 for the overall process
   * Reports 0..100 for each file being flashed

   \param[in]     orc_FilesToFlash              Files to flash
   \param[out]    orc_StateHexFiles             States of all handled hex files

   \return
   C_NO_ERR    flashed all files
   C_COM       error flashing (see log for details)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_FlashNodeXfl(const std::vector<C_SclString> & orc_FilesToFlash,
                                         std::vector<C_OscSuSequencesNodeStwFlHexFileStates> & orc_StateHexFiles)
{
   int32_t s32_Return = C_NO_ERR;

   (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_START, C_NO_ERR, 0U, mc_CurrentNode,
                          "Flashing node ...");

   for (uint32_t u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
   {
      // Save file index
      this->mu32_CurrentFile = u32_File;

      (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Flashing HEX file ...");

      // Loading errors can not be detected on this layer separated from the sending process
      orc_StateHexFiles[u32_File].e_FileLoaded = eSUSEQ_STATE_NOT_NEEDED;

      s32_Return = this->mpc_ComDriver->SendStwDoFlash(mc_CurrentNode, orc_FilesToFlash[u32_File]);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR, C_NO_ERR, 100U, mc_CurrentNode,
                                "Flashing HEX file finished.");

         orc_StateHexFiles[u32_File].e_FileSent = eSUSEQ_STATE_ERROR;
         s32_Return = C_COM;
         break;
      }
      else
      {
         orc_StateHexFiles[u32_File].e_FileSent = eSUSEQ_STATE_NO_ERR;
      }

      (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED, C_NO_ERR, 100U, mc_CurrentNode,
                             "Flashing HEX file finished.");
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_FINISHED, C_NO_ERR, 100U, mc_CurrentNode,
                             "Flashing node finished.");
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information of openSYDE server node

   Utility function used to read openSYDE information from server node and report it.
   See documentation of ReadDeviceInformation() for full description.

   \param[in]     ou8_ProgressToReport    progress value to report when invoking virtual reporting function
   \param[in]     ou32_NodeIndex          Index of node within mpc_SystemDefinition
   \param[in,out] orc_NodeState           Connect states of node

   \return
   C_COM      communication driver reported problem (details will be written to log file)
   C_CHECKSUM Security related error (something went wrong while handshaking with the server)
   C_NO_ERR   information read
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_ReadDeviceInformationOpenSyde(const uint8_t ou8_ProgressToReport,
                                                          const uint32_t ou32_NodeIndex,
                                                          C_OscSuSequencesNodeConnectStates & orc_NodeState)
{
   C_OsyDeviceInformation c_Info;
   uint8_t u8_NrCode;
   int32_t s32_Return;

   // If connected via Ethernet we might need to reconnect (in case we ran into the session timeout)
   s32_Return = this->m_ReconnectToTargetServer();

   if (s32_Return != C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR, s32_Return, ou8_ProgressToReport, mc_CurrentNode,
                             "Could not reconnect to node");
      if (s32_Return != C_CHECKSUM)
      {
         s32_Return = C_COM;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //node is active and flashable directly on the bus that the client is connected to
      //to prevent any session timeout issues request the pre-programming session and activate security
      // level 1
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_SET_SESSION_START, C_NO_ERR, ou8_ProgressToReport, mc_CurrentNode,
                             "Activating PreProgramming session ...");
      s32_Return = this->mpc_ComDriver->SendOsySetPreProgrammingMode(mc_CurrentNode, false, &u8_NrCode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR, s32_Return, ou8_ProgressToReport,
                                mc_CurrentNode,
                                "Error activating PreProgramming session. Details:" +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));

         if (s32_Return != C_CHECKSUM)
         {
            s32_Return = C_COM;
         }
         else
         {
            // By this error we know security is necessary and failed
            orc_NodeState.q_AuthenticationNecessarySet = true;
            orc_NodeState.q_AuthenticationNecessary = true;
            orc_NodeState.q_AuthenticationError = true;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //get device name
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START, C_NO_ERR, ou8_ProgressToReport, mc_CurrentNode,
                             "Reading device name ...");
      s32_Return = this->mpc_ComDriver->SendOsyReadDeviceName(mc_CurrentNode, c_Info.c_DeviceName, &u8_NrCode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR, s32_Return, ou8_ProgressToReport,
                                mc_CurrentNode,
                                "Error reading device name. Details:" +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }
   }

   //get information about all flash blocks
   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START, C_NO_ERR, ou8_ProgressToReport, mc_CurrentNode,
                             "Reading flash block information ...");

      //we need security level 1 for that:
      s32_Return = this->mpc_ComDriver->SendOsySetSecurityLevel(mc_CurrentNode, 1U, &u8_NrCode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR, s32_Return,
                                ou8_ProgressToReport, mc_CurrentNode,
                                "Error setting security level for reading flash block information. Details:" +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         if (s32_Return != C_CHECKSUM)
         {
            s32_Return = C_COM;
         }
      }
      else
      {
         const C_OscNode & rc_CurNode = this->mpc_SystemDefinition->c_Nodes[ou32_NodeIndex];
         //this information is only available for address based devices
         tgl_assert(rc_CurNode.u32_SubDeviceIndex < rc_CurNode.pc_DeviceDefinition->c_SubDevices.size());
         if (rc_CurNode.pc_DeviceDefinition->c_SubDevices[rc_CurNode.u32_SubDeviceIndex].
             q_FlashloaderOpenSydeIsFileBased
             ==
             false)
         {
            s32_Return = this->mpc_ComDriver->SendOsyReadAllFlashBlockData(mc_CurrentNode, c_Info.c_Applications,
                                                                           &u8_NrCode);
            if (s32_Return != C_NO_ERR)
            {
               (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR, s32_Return, ou8_ProgressToReport,
                                      mc_CurrentNode, "Error reading flash block information. Details:" +
                                      C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
               s32_Return = C_COM;
            }
         }
         else
         {
            c_Info.c_Applications.resize(0);
         }
      }
   }

   //get rest of information
   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START, C_NO_ERR, ou8_ProgressToReport,
                             mc_CurrentNode, "Reading even more information ...");
      s32_Return = this->mpc_ComDriver->SendOsyReadInformationFromFlashloader(mc_CurrentNode,
                                                                              c_Info.c_MoreInformation,
                                                                              &u8_NrCode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR, s32_Return, ou8_ProgressToReport,
                                mc_CurrentNode, "Error reading even more information. Details:" +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }
      else
      {
         orc_NodeState.c_AvailableFeatures = c_Info.c_MoreInformation.c_AvailableFeatures;
      }
   }

   if ((s32_Return == C_NO_ERR) &&
       (c_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsSecurity == true))
   {
      // Check for activated security
      // By calling SendOsySetPreProgrammingMode on the beginning we know it is successful, but not if security is
      // activated
      uint8_t u8_Algorithm;
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_START, C_NO_ERR,
                             ou8_ProgressToReport,
                             mc_CurrentNode, "Reading security activation state ...");
      s32_Return = this->mpc_ComDriver->SendOsyReadSecurityActivation(mc_CurrentNode,
                                                                      orc_NodeState.q_AuthenticationNecessary,
                                                                      u8_Algorithm,
                                                                      &u8_NrCode);

      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR, s32_Return,
                                ou8_ProgressToReport,
                                mc_CurrentNode, "Error reading security activation state. Details:" +
                                C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }
      else
      {
         orc_NodeState.q_AuthenticationNecessarySet = true;
      }
   }
   else
   {
      orc_NodeState.q_AuthenticationNecessarySet = true;
   }

   (void)this->m_DisconnectFromTargetServer();

   if (s32_Return == C_NO_ERR)
   {
      //report findings to application:
      this->m_ReportOpenSydeFlashloaderInformationRead(c_Info, ou32_NodeIndex);
      orc_NodeState.e_InformationRead = eSUSEQ_STATE_NO_ERR;
   }
   else
   {
      orc_NodeState.e_InformationRead = eSUSEQ_STATE_ERROR;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information of STW flashloader server node

   Utility function used to read STW flashloader information from server node and report it.
   See documentation of ReadDeviceInformation() for full description.

   \param[in]     ou8_ProgressToReport    progress value to report when invoking virtual reporting function
   \param[in]     ou32_NodeIndex          Index of node within mpc_SystemDefinition
   \param[in,out] orc_NodeState           Connect states of node

   \return
   C_COM      communication driver reported problem (details will be written to log file)
   C_NO_ERR   information read
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_ReadDeviceInformationStwFlashloader(const uint8_t ou8_ProgressToReport,
                                                                const uint32_t ou32_NodeIndex,
                                                                C_OscSuSequencesNodeConnectStates & orc_NodeState)
{
   int32_t s32_Return;
   C_XflDeviceInformation c_Information;

   //bring device into wakeup state ...
   s32_Return = this->mpc_ComDriver->SendStwWakeupLocalId(mc_CurrentNode, NULL);
   if (s32_Return != C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR, s32_Return, ou8_ProgressToReport, mc_CurrentNode,
                             "Could not perform node wakeup.");
      s32_Return = C_COM;
   }
   else
   {
      //read everything we can get ...
      (void)m_ReportProgress(eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START, C_NO_ERR, ou8_ProgressToReport,
                             mc_CurrentNode, "Reading information from node ...");
      s32_Return = this->mpc_ComDriver->SendStwReadDeviceInformation(mc_CurrentNode, c_Information.c_BasicInformation,
                                                                     c_Information.c_ChecksumInformation);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR, s32_Return, ou8_ProgressToReport,
                                mc_CurrentNode, "Could not read information from node.");

         s32_Return = C_COM;
      }
      else
      {
         //report findings to application:
         this->m_ReportStwFlashloaderInformationRead(c_Information, ou32_NodeIndex);
      }
   }

   orc_NodeState.q_AuthenticationNecessarySet = true;
   orc_NodeState.e_InformationRead = (s32_Return == C_NO_ERR) ? eSUSEQ_STATE_NO_ERR : eSUSEQ_STATE_ERROR;

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Set up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequences::C_OscSuSequences(void) :
   C_OscComSequencesBase(true, true),
   mu32_CurrentNode(0U),
   mu32_CurrentFile(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   constructor

   Tear down class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscSuSequences::~C_OscSuSequences(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy referenced files to a temporary folder

   Copy all files to be flashed and/or written to NVM to a "temp" folder.
   Ensures a clearly defined state during the whole system update procedure.
   To be called optionally before starting the procedure with "ActivateFlashloader()".

   * Can be called before "Init()" as all required information is passed as parameters.

   * All referenced files must be present.
   * Validity is not checked (will be done at UpdateSystem()).
   * Will erase a pre-existing folder completely
   * Will create the target folder (recursively if it needs to be)
   * Files for address based targets and NVM files will be renamed to prevent possible conflicts
   * Flash files for file based devices will not be renamed (as they need to be unique anyway)
   * Does not report any progress via m_ReportProgress().

   \param[in]     orc_Nodes               List of nodes (part of system definition)
   \param[in]     orc_ActiveNodes         Vector of flags for each node in orc_Nodes
                                          - true: file path(s) for the target are expected to be present
                                          - false: file path(s) for the target are not expected to be present
   \param[in]     orc_TargetPath          Path to place files in (with trailing path separator)
   \param[in,out] orc_ApplicationsToWrite in: Paths to files to copy
                                          out: Paths to resulting files copied and renamed by this function
                                          if the function returns an error this parameter will not be modified
   \param[in,out] opc_ErrorPath           Optional pointer to store path error details (current: which file did fail)

   \return
   C_NO_ERR    files copied
   C_OVERFLOW  size of orc_ApplicationsToWrite is not the same as the size of nodes in orc_Nodes
               size of orc_ActiveNodes is not the same as the size of nodes in orc_Nodes
   C_NOACT     orc_ApplicationsToWrite has non-empty list of files for node that was not set as active in orc_ActiveNodes
               size of files in orc_ApplicationsToWrite[node] differs from the
                 number of applications of the node in orc_Nodes (for an active and address based node)
   C_RANGE     Flash or NVM file referenced by orc_ApplicationsToWrite does not exist
               orc_TargetPath does not end in "\" or "/"
   C_BUSY      could not erase pre-existing target path (note: can result in partially erased target path)
   C_RD_WR     could not copy file
   C_TIMEOUT   could not create target directory
   C_CONFIG    at least one file based node has at least two identical named files (independent of character case)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::h_CreateTemporaryFolder(const std::vector<C_OscNode> & orc_Nodes,
                                                  const std::vector<uint8_t> & orc_ActiveNodes,
                                                  const C_SclString & orc_TargetPath,
                                                  std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                                  C_SclString * const opc_ErrorPath)
{
   int32_t s32_Return = C_NO_ERR;

   std::vector<C_DoFlash> c_NodesToFlashNewPaths = orc_ApplicationsToWrite;
   std::vector<C_SclString> c_NodeTargetPaths;

   const char_t cn_LastCharacter = orc_TargetPath[orc_TargetPath.Length()];

   if ((cn_LastCharacter != '\\') && (cn_LastCharacter != '/'))
   {
      s32_Return = C_RANGE;

      if (opc_ErrorPath != NULL)
      {
         *opc_ErrorPath = orc_TargetPath;
      }
   }
   else
   {
      if ((orc_ApplicationsToWrite.size() != orc_Nodes.size()) || (orc_ActiveNodes.size() != orc_Nodes.size()))
      {
         s32_Return = C_OVERFLOW;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      //consistent configuration ?
      for (uint16_t u16_Node = 0U; u16_Node < orc_Nodes.size(); u16_Node++)
      {
         tgl_assert(orc_Nodes[u16_Node].pc_DeviceDefinition != NULL);

         //node inactive but files defined ?
         if ((orc_ActiveNodes[u16_Node] == 0U) &&
             ((orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size() != 0) ||
              (orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm.size() != 0) ||
              (orc_ApplicationsToWrite[u16_Node].c_PemFile != "")))
         {
            //file(s) defined for inactive node -> cry
            s32_Return = C_NOACT;
         }

         if (s32_Return == C_NO_ERR)
         {
            //do all files exist ?
            if (orc_ActiveNodes[u16_Node] == 1U)
            {
               //files for flash:
               for (uint16_t u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size();
                    u16_File++)
               {
                  const C_SclString c_File = orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File];
                  if (TglFileExists(c_File) == false)
                  {
                     s32_Return = C_RANGE;
                     if (opc_ErrorPath != NULL)
                     {
                        *opc_ErrorPath = c_File;
                     }
                     break;
                  }
               }
               //files for NVM:
               for (uint16_t u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm.size();
                    u16_File++)
               {
                  const C_SclString c_File = orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm[u16_File];
                  if (TglFileExists(c_File) == false)
                  {
                     s32_Return = C_RANGE;
                     if (opc_ErrorPath != NULL)
                     {
                        *opc_ErrorPath = c_File;
                     }
                     break;
                  }
               }
               // PEM file
               if (orc_ApplicationsToWrite[u16_Node].c_PemFile != "")
               {
                  const C_SclString c_File = orc_ApplicationsToWrite[u16_Node].c_PemFile;
                  if (TglFileExists(c_File) == false)
                  {
                     s32_Return = C_RANGE;
                     if (opc_ErrorPath != NULL)
                     {
                        *opc_ErrorPath = c_File;
                     }
                  }
               }
            }
         }

         if (s32_Return == C_NO_ERR)
         {
            // Special case: File based nodes shall have unique file names
            tgl_assert(
               orc_Nodes[u16_Node].u32_SubDeviceIndex < orc_Nodes[u16_Node].pc_DeviceDefinition->c_SubDevices.size());
            if (orc_Nodes[u16_Node].pc_DeviceDefinition->c_SubDevices[orc_Nodes[u16_Node].u32_SubDeviceIndex].
                q_FlashloaderOpenSydeIsFileBased == true)
            {
               std::vector<C_SclString> c_Files = orc_ApplicationsToWrite[u16_Node].c_FilesToFlash;
               //convert all file names to lower case to detect conflicts in the file system
               //also: remove paths
               for (uint16_t u16_File = 0U; u16_File < c_Files.size(); u16_File++)
               {
                  c_Files[u16_File] = TglExtractFileName(c_Files[u16_File]).LowerCase();
               }
               //get same names next to each other:
               std::sort(c_Files.begin(), c_Files.end());
               //remove duplicates:
               c_Files.erase(std::unique(c_Files.begin(), c_Files.end()), c_Files.end());
               if (c_Files.size() != orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size())
               {
                  //we have less files than originally; so there must have been dupes
                  s32_Return = C_CONFIG;
               }
            }
         }

         if (s32_Return != C_NO_ERR)
         {
            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //erase target path if it exists:
      if (TglDirectoryExists(orc_TargetPath) == true)
      {
         s32_Return = TglRemoveDirectory(orc_TargetPath, false);
         if (s32_Return != 0)
         {
            s32_Return = C_BUSY;
            if (opc_ErrorPath != NULL)
            {
               *opc_ErrorPath = orc_TargetPath;
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //create target folder (from bottom-up if required):
      s32_Return = C_OscUtils::h_CreateFolderRecursively(orc_TargetPath);
      if (s32_Return != C_NO_ERR)
      {
         if (opc_ErrorPath != NULL)
         {
            *opc_ErrorPath = orc_TargetPath;
         }
         s32_Return = C_TIMEOUT;
      }

      if (s32_Return == C_NO_ERR)
      {
         // Creating sub folders for each node in the target path
         c_NodeTargetPaths.resize(orc_Nodes.size(), "");

         for (uint16_t u16_Node = 0U; u16_Node < orc_Nodes.size(); u16_Node++)
         {
            if (orc_ActiveNodes[u16_Node] == 1U)
            {
               c_NodeTargetPaths[u16_Node] =
                  TglFileIncludeTrailingDelimiter(orc_TargetPath + C_OscUtils::h_NiceifyStringForFileName(
                                                     orc_Nodes[u16_Node].c_Properties.c_Name));

               s32_Return = TglCreateDirectory(c_NodeTargetPaths[u16_Node]);
               if (s32_Return != 0)
               {
                  if (opc_ErrorPath != NULL)
                  {
                     *opc_ErrorPath = c_NodeTargetPaths[u16_Node];
                  }
                  s32_Return = C_TIMEOUT;
                  break;
               }
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //copy files
      for (uint16_t u16_Node = 0U; u16_Node < orc_Nodes.size(); u16_Node++)
      {
         if (orc_ActiveNodes[u16_Node] == 1U)
         {
            //flash files
            for (uint16_t u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size(); u16_File++)
            {
               //get source file name
               const C_SclString c_SourceFileName = orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File];
               //compose target file name
               C_SclString c_TargetFileName;

               tgl_assert(
                  orc_Nodes[u16_Node].u32_SubDeviceIndex <
                  orc_Nodes[u16_Node].pc_DeviceDefinition->c_SubDevices.size());
               if (orc_Nodes[u16_Node].pc_DeviceDefinition->c_SubDevices[orc_Nodes[u16_Node].u32_SubDeviceIndex].
                   q_FlashloaderOpenSydeIsFileBased == true)
               {
                  // File based nodes need the unchanged file name and must be unique
                  c_TargetFileName = c_NodeTargetPaths[u16_Node] +
                                     TglExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File]);
               }
               else
               {
                  // The original source file name is not relevant and does not need to be unique
                  // Add an counter to the target file name
                  c_TargetFileName = c_NodeTargetPaths[u16_Node] +
                                     C_SclString::IntToStr(static_cast<uint32_t>(u16_File) + 1U) + "_" +
                                     TglExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File]);
               }

               //copy file
               s32_Return = C_OscUtils::h_CopyFile(c_SourceFileName, c_TargetFileName, opc_ErrorPath);
               if (s32_Return == C_NO_ERR)
               {
                  c_NodesToFlashNewPaths[u16_Node].c_FilesToFlash[u16_File] = c_TargetFileName;
               }
               else
               {
                  break;
               }
            }
            //NVM files
            for (uint16_t u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm.size();
                 u16_File++)
            {
               //get source file name
               const C_SclString c_SourceFileName = orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm[u16_File];
               //compose target file name
               // The original file name is not relevant does not need to be unique
               // Add an counter to the target file name
               const C_SclString c_TargetFileName =
                  c_NodeTargetPaths[u16_Node] +
                  C_SclString::IntToStr(static_cast<uint32_t>(u16_File) + 1U) + "_" +
                  TglExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm[u16_File]);

               //copy file
               s32_Return = C_OscUtils::h_CopyFile(c_SourceFileName, c_TargetFileName, opc_ErrorPath);
               if (s32_Return == C_NO_ERR)
               {
                  c_NodesToFlashNewPaths[u16_Node].c_FilesToWriteToNvm[u16_File] = c_TargetFileName;
               }
               else
               {
                  break;
               }
            }

            //PEM file
            if (c_NodesToFlashNewPaths[u16_Node].c_PemFile != "")
            {
               //get source file name
               const C_SclString c_SourceFileName = orc_ApplicationsToWrite[u16_Node].c_PemFile;
               //compose target file name
               const C_SclString c_TargetFileName =
                  c_NodeTargetPaths[u16_Node] +
                  TglExtractFileName(orc_ApplicationsToWrite[u16_Node].c_PemFile);

               //copy file
               s32_Return = C_OscUtils::h_CopyFile(c_SourceFileName, c_TargetFileName, opc_ErrorPath);
               if (s32_Return == C_NO_ERR)
               {
                  c_NodesToFlashNewPaths[u16_Node].c_PemFile = c_TargetFileName;
               }
               else
               {
                  break;
               }
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //copy over new paths to return to caller:
      orc_ApplicationsToWrite = c_NodesToFlashNewPaths;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether application on client side match applications on devices

   Compares a list of application properties to a second list of application properties.
   The function will report which elements of the first list are present in the second list.

   Use case:
   Check which applications need to be flashed.

   Workflow:
   * Read application properties from servers (e.g. using ReadDeviceInformation())
   * Parse application properties from client side hex files
        (e.g. using C_OsyHexFile::ScanApplicationInformationBlockFromHexFile())
   * call this function
   * set up parameter for UpdateSystem() based on the results of this function

   \param[in]     orc_ClientSideApplications       properties of client side applications
   \param[in]     orc_ServerSideApplications       properties of server side applications
   \param[out]    orc_ApplicationsPresentOnServer   size matches size of orc_ClientSideApplications
                                                    0: application not contained in orc_ServerSideApplications
                                                    1: application contained in orc_ServerSideApplications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSuSequences::h_CheckForChangedApplications(
   const std::vector<C_ApplicationProperties> & orc_ClientSideApplications,
   const std::vector<C_ApplicationProperties> & orc_ServerSideApplications,
   std::vector<uint8_t> & orc_ApplicationsPresentOnServer)
{
   orc_ApplicationsPresentOnServer.resize(orc_ClientSideApplications.size());
   for (uint32_t u32_ClientApplIndex = 0U; u32_ClientApplIndex < orc_ClientSideApplications.size();
        u32_ClientApplIndex++)
   {
      orc_ApplicationsPresentOnServer[u32_ClientApplIndex] = 0U;
      for (uint32_t u32_ServerApplIndex = 0U; u32_ServerApplIndex < orc_ServerSideApplications.size();
           u32_ServerApplIndex++)
      {
         if (orc_ServerSideApplications[u32_ServerApplIndex] == orc_ClientSideApplications[u32_ClientApplIndex])
         {
            orc_ApplicationsPresentOnServer[u32_ClientApplIndex] = 1U;
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialization of the protocol

   Enter Flashloader for all nodes
   Note: STW Flashloader nodes can only be end-points, not routers

   Prerequisite:
   * class members and communication has been set up with Init()

   Rough sequence:
   For local bus:
   * openSYDE nodes:
   ** send "RequestProgramming" to all nodes
   ** send "EcuReset" to all nodes
   ** if connected via CAN: send "EnterPreProgrammingSession" as broadcast (for a few seconds in short intervals)
   * STW Flashloader nodes (CAN only):
   ** send all configured reset request messages
   ** send "FLASH" (for a few seconds in short intervals;
      in parallel to the openSYDE "EnterPreProgrammingSession" broadcasts
   For confirmation:
   * use simple read service for all nodes that are expected to be present (as "ping")

   Then recurse for all buses that can be reached through the first bus:
   * set up routing and perform sequence bus-by-bus

   Result:
   * all nodes defined to be present on all buses are in flashloader mode

   \param[in]   oq_FailOnFirstError   true: abort all further communication if connecting to one device fails
                                      false: try to continue with other devices in this case

   \return
   C_NO_ERR   flashloaders on all nodes activated
   C_CONFIG   mpc_SystemDefinition is NULL (Init() not called)
   C_COM      communication driver reported problem (details will be written to log file)
   C_WARN     activation for at least one device failed (see log for details)
   C_CHECKSUM Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::ActivateFlashloader(const bool oq_FailOnFirstError)
{
   int32_t s32_Return = C_NO_ERR;
   bool q_AtLeastOneError = false;
   const uint32_t u32_SCAN_TIME_MS = 5000U;
   const uint32_t u32_INTERVAL_TESTER_PRESENT = 1000U;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint8_t u8_NrCode;

      // Prepare only nodes on the local bus in the first step ...

      //check prerequisites (should have been checked at Init() already)
      tgl_assert(this->mpc_SystemDefinition->c_Nodes.size() == this->mc_ActiveNodes.size());
      tgl_assert(this->mu32_ActiveBusIndex < this->mpc_SystemDefinition->c_Buses.size());

      // send openSYDE CAN-TP "RequestProgramming" to each node
      (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_START, C_NO_ERR, 0U,
                             "Sending openSYDE Flashloader activation requests ...");

      this->mpc_ComDriver->ClearDispatcherQueue();

      // Reset timeout flags
      this->mc_TimeoutNodes.clear();
      this->mc_TimeoutNodes.resize(this->mc_ActiveNodes.size(), 0U);

      // Reset states
      this->mc_ConnectStatesNodes.clear();
      this->mc_ConnectStatesNodes.resize(this->mc_ActiveNodes.size());

      // First set the request programming flag
      for (uint16_t u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
      {
         C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
         const bool q_IsActive = m_IsNodeActive(u16_Node, this->mu32_ActiveBusIndex, e_ProtocolType, mc_CurrentNode);

         if (q_IsActive == true)
         {
            s32_Return = this->mpc_ComDriver->IsRoutingNecessary(u16_Node);

            // Continue with nodes without routing
            if (s32_Return == C_NOACT)
            {
               // STW Flashloader does not have a request programming flag
               if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
               {
                  s32_Return = this->mpc_ComDriver->ReConnectNode(mc_CurrentNode);

                  if (s32_Return != C_NO_ERR)
                  {
                     // In this case, the node did not connect.
                     // Errors in case of not reachable nodes will be handled when
                     // the state eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START was set the first time to have
                     // the same process sequence like using CAN.
                     // Therefore do not abort and send only a warning as progress information.
                     // This can only happen in case of Ethernet. The node is not connected, so the next
                     // request did not make any sense to send and will be skipped till the
                     // "EnterPreProgrammingSession" broadcasts. By skipping the other request,
                     // further sending errors will be avoided which would occur a "real" sending error.
                     (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_RECONNECT_WARNING, s32_Return,
                                            10U, mc_CurrentNode,
                                            "Device does not respond to RequestProgramming request.");

                     s32_Return = C_NO_ERR;
                  }
                  else
                  {
                     // Set the request programming flag to bring the server into flashloader
                     s32_Return = this->mpc_ComDriver->SendOsyRequestProgramming(mc_CurrentNode);

                     this->mpc_ComDriver->DisconnectNode(mc_CurrentNode);

                     if (s32_Return != C_NO_ERR)
                     {
                        // In this case, the node does not respond and maybe the next step can help to bring
                        // it into the flashloader by sending EnterPreProgrammingSession as broadcast
                        // Therefore do not abort and send only a warning as progress information.
                        (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_WARNING, s32_Return,
                                               10U, mc_CurrentNode,
                                               "Device does not respond to RequestProgramming request.");

                        s32_Return = C_NO_ERR;
                     }
                  }
               }
               else
               {
                  // No error, nothing to do for STW Flashloader nodes here, reset the error code
                  s32_Return = C_NO_ERR;
               }
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         // Sending the reset commandos
         for (uint16_t u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
         {
            C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
            const bool q_IsActive = m_IsNodeActive(u16_Node, this->mu32_ActiveBusIndex, e_ProtocolType, mc_CurrentNode);

            if (q_IsActive == true)
            {
               s32_Return = this->mpc_ComDriver->IsRoutingNecessary(u16_Node);

               // Continue with nodes without routing
               if (s32_Return == C_NOACT)
               {
                  if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
                  {
                     s32_Return = this->mpc_ComDriver->ReConnectNode(mc_CurrentNode);

                     if (s32_Return != C_NO_ERR)
                     {
                        // In this case, the node did not connect.
                        // Errors in case of not reachable nodes will be handled when
                        // the state eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START was set the first time to have
                        // the same process sequence like using CAN.
                        // Therefore do not abort and send only a warning as progress information.
                        // This can only happen in case of Ethernet. The node is not connected, so the next
                        // request did not make any sense to send and will be skipped till the
                        // "EnterPreProgrammingSession" broadcasts. By skipping the other request,
                        // further sending errors will be avoided which would occur a "real" sending error.
                        (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_RECONNECT_WARNING, s32_Return,
                                               15U, mc_CurrentNode,
                                               "Device does not respond to RequestProgramming request.");

                        s32_Return = C_NO_ERR;
                     }
                     else
                     {
                        s32_Return = this->mpc_ComDriver->SendOsyEcuReset(
                           mc_CurrentNode,
                           C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);

                        this->mpc_ComDriver->DisconnectNode(mc_CurrentNode);

                        if (s32_Return != C_NO_ERR)
                        {
                           // In this case, the node will not get the request, maybe the next step can help to bring
                           // it into the flashloader by sending EnterPreProgrammingSession as broadcast
                           // Therefore do not abort and send only a warning as progress information.
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_ECU_RESET_WARNING, s32_Return,
                                                  10U, mc_CurrentNode,
                                                  "EcuReset request for device failed.");

                           s32_Return = C_NO_ERR;
                        }
                     }
                  }
                  else
                  {
                     //STW Flashloader node
                     s32_Return = this->mpc_ComDriver->SendStwRequestNodeReset(this->mc_CurrentNode);
                     if (s32_Return != C_NO_ERR)
                     {
                        if (s32_Return == C_NOACT)
                        {
                           // No reset message configured. The next step can help to bring
                           // it into the flashloader by sending STW Flashloader "FLASH"
                           // Therefore do not abort and send only a warning as progress information.
                           // All other errors are errors which can not be "healed" by the next step
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_ECU_RESET_WARNING, s32_Return, 15U,
                                                  mc_CurrentNode,
                                                  "No EcuReset message configured.");

                           s32_Return = C_NO_ERR;
                        }
                        else
                        {
                           // All other errors are errors which can not be "healed" by the next step
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR, s32_Return, 15U,
                                                  mc_CurrentNode,
                                                  "Sending EcuReset request failed.");

                           s32_Return = C_COM;
                        }
                     }
                  }
               }
            }
         }
      }

      //send "EnterPreProgrammingSession" as broadcast (for a few seconds in short intervals)
      if (s32_Return == C_NO_ERR)
      {
         //but only if we are communicating via CAN bus; keeping the target in the flashloader via the time window
         // is not possible via Ethernet; it really has to react to RequestProgramming
         if (this->mpc_SystemDefinition->c_Buses[this->mu32_ActiveBusIndex].e_Type == C_OscSystemBus::eCAN)
         {
            const uint32_t u32_StartTime = stw::tgl::TglGetTickCount();
            uint32_t u32_WaitTime = this->GetMinimumFlashloaderResetWaitTime(C_OscComDriverFlash::eNO_CHANGES_CAN);

            if (u32_WaitTime < u32_SCAN_TIME_MS)
            {
               // The scan time is necessary for the manual triggering of the nodes
               u32_WaitTime = u32_SCAN_TIME_MS;
            }

            (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START, C_NO_ERR, 20U,
                                   "Broadcasting enter Flashloader request ...");
            do
            {
               if (this->mq_OpenSydeDevicesActive == true)
               {
                  // openSYDE "DiagnosticSessionControl(PreProgramming)" broadcast
                  s32_Return = this->mpc_ComDriver->SendOsyCanBroadcastEnterPreProgrammingSession();
                  if (s32_Return != C_NO_ERR)
                  {
                     (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR, s32_Return, 20U,
                                            "EnterPreProgramming broadcast failed.");
                     s32_Return = C_COM;
                  }
               }

               if ((this->mq_StwFlashloaderDevicesActiveOnLocalBus == true) &&
                   (s32_Return == C_NO_ERR))
               {
                  // send STW Flashloader "FLASH" in parallel:
                  s32_Return = this->mpc_ComDriver->SendStwSendFlash(this->mc_StwFlashloaderDeviceOnLocalBus);
                  if (s32_Return != C_NO_ERR)
                  {
                     (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR, s32_Return, 20U,
                                            "Send \"FLASH\" broadcast failed.");
                     s32_Return = C_COM;
                  }
               }

               if (s32_Return != C_NO_ERR)
               {
                  break;
               }

               stw::tgl::TglSleep(5);
            }
            while (stw::tgl::TglGetTickCount() < (u32_WaitTime + u32_StartTime));
         }
         else
         {
            //Ethernet. Give the targets the minimum reset time to reset and initialize their Ethernet interfaces ...
            TglSleep(this->GetMinimumFlashloaderResetWaitTime(C_OscComDriverFlash::eNO_CHANGES_ETHERNET));
         }
      }

      //Previous broadcasts might have caused responses placed in the receive queues of the device
      // specific driver instances. Dump them.
      this->mpc_ComDriver->ClearDispatcherQueue();

      //use simple read service for all nodes that are expected to be present (as "ping")
      if (s32_Return == C_NO_ERR)
      {
         (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START, C_NO_ERR, 30U,
                                "Checking node states ...");

         for (uint16_t u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
         {
            C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;

            const bool q_Return =
               m_IsNodeActive(u16_Node, this->mu32_ActiveBusIndex, e_ProtocolType, mc_CurrentNode);

            if (q_Return == true)
            {
               (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START, C_NO_ERR, 30U, mc_CurrentNode,
                                      "Checking node state ...");

               s32_Return = this->mpc_ComDriver->IsRoutingNecessary(u16_Node);

               // Continue with nodes without routing
               if (s32_Return == C_NOACT)
               {
                  if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
                  {
                     //if connected via Ethernet we need to reconnect as the reset will break the active TCP
                     // connection
                     s32_Return = this->mpc_ComDriver->ReConnectNode(mc_CurrentNode);

                     if (s32_Return != C_NO_ERR)
                     {
                        (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR, s32_Return, 30U,
                                               mc_CurrentNode,
                                               "Could not reconnect to node");

                        // Node is not reachable
                        this->mc_TimeoutNodes[u16_Node] = static_cast<uint8_t>(s32_Return == C_BUSY);

                        s32_Return = C_COM;
                     }
                     if (s32_Return == C_NO_ERR)
                     {
                        //If we are on Ethernet the node is in flashloader but not in programming mode yet
                        // we need to send one EnterPreProgramming request to get it there
                        //If we're on CAN one more of this services does not hurt as well :-)
                        //We want to confirm the device is in flashloader anyway.
                        //So we use this service (it will fail if the node is in the application
                        // as there is no "PreProgramming" session there.
                        s32_Return =
                           this->mpc_ComDriver->SendOsySetPreProgrammingMode(mc_CurrentNode, true, &u8_NrCode);
                        if (s32_Return != C_NO_ERR)
                        {
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR, s32_Return, 30U,
                                                  mc_CurrentNode, "Request to set active session failed. Details:" +
                                                  C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                           u8_NrCode));

                           // Node is not reachable
                           this->mc_TimeoutNodes[u16_Node] = static_cast<uint8_t>(s32_Return == C_TIMEOUT);

                           if (s32_Return != C_CHECKSUM)
                           {
                              s32_Return = C_COM;
                           }
                           else
                           {
                              this->mc_ConnectStatesNodes[u16_Node].q_AuthenticationError = true;
                           }
                        }
                        else
                        {
                           //looks good ...
                           //node is active and flashable directly on the bus that the client is connected to
                        }
                     }

                     this->mpc_ComDriver->DisconnectNode(mc_CurrentNode);
                  }
                  else
                  {
                     //STW Flashloader node
                     //it should be in Sleep state now. The only service that we can use is wakeup.
                     //will not cause any problems as the node will go back to sleep if another node is addressed
                     // later
                     // on
                     s32_Return = this->mpc_ComDriver->SendStwWakeupLocalId(mc_CurrentNode, NULL);
                     if (s32_Return != C_NO_ERR)
                     {
                        (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR, s32_Return, 30U,
                                               mc_CurrentNode,
                                               "Could not perform node wakeup.");

                        // Node is not reachable
                        this->mc_TimeoutNodes[u16_Node] = 1U;

                        s32_Return = C_COM;
                     }
                  }
                  if (s32_Return != C_NO_ERR)
                  {
                     q_AtLeastOneError = true;
                     if (oq_FailOnFirstError == true)
                     {
                        break;
                     }
                     s32_Return = C_NO_ERR;
                  }
               }
            }

            if (this->mc_TimeoutNodes[u16_Node] == 1U)
            {
               this->mc_ConnectStatesNodes[u16_Node].q_Timeout = true;
            }
         }
      }

      // Activate all nodes with routing
      if (s32_Return == C_NO_ERR)
      {
         for (uint16_t u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
         {
            C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
            uint32_t u32_BusIndex;

            if (this->mpc_ComDriver->GetBusIndexOfRoutingNode(u16_Node, u32_BusIndex) == C_NO_ERR)
            {
               const bool q_IsActive = m_IsNodeActive(u16_Node, u32_BusIndex, e_ProtocolType, mc_CurrentNode);

               if (q_IsActive == true)
               {
                  const bool q_IsNodeReachable = this->m_IsNodeReachable(u16_Node);

                  (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START, C_NO_ERR, 30U, mc_CurrentNode,
                                         "Checking node state ...");

                  if (q_IsNodeReachable == true)
                  {
                     uint32_t u32_ErrorIndex = 0U;

                     (void)m_ReportProgress(eACTIVATE_FLASHLOADER_ROUTING_START, C_NO_ERR, 40U, mc_CurrentNode,
                                            "Starting routing for node ...");

                     s32_Return = this->mpc_ComDriver->StartRouting(u16_Node, &u32_ErrorIndex);

                     if (s32_Return == C_NO_ERR)
                     {
                        if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
                        {
                           // Set the request programming flag to bring the server into flashloader
                           s32_Return = this->mpc_ComDriver->SendOsyRequestProgramming(mc_CurrentNode);

                           // Reset the server
                           if (s32_Return == C_NO_ERR)
                           {
                              s32_Return = this->mpc_ComDriver->SendOsyEcuReset(
                                 mc_CurrentNode,
                                 C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);

                              (void)this->m_DisconnectFromTargetServer();

                              if (s32_Return != C_NO_ERR)
                              {
                                 (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR, s32_Return,
                                                        50U, mc_CurrentNode,
                                                        "EcuReset request for routing device failed.");
                                 s32_Return = C_COM;
                              }
                           }
                           else
                           {
                              (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR, s32_Return,
                                                     50U, mc_CurrentNode,
                                                     "RequestProgramming request for routing device failed.");

                              // Node is not reachable
                              this->mc_TimeoutNodes[u16_Node] = static_cast<uint8_t>(s32_Return == C_TIMEOUT);

                              s32_Return = C_COM;
                           }

                           if (s32_Return == C_NO_ERR)
                           {
                              uint32_t u32_StartTime;
                              uint32_t u32_CurrentTime;
                              uint32_t u32_LastSentTesterPresent;
                              uint32_t u32_WaitTime;
                              C_OscSystemBus::E_Type e_TargetInterfaceType = C_OscSystemBus::eCAN;
                              C_OscComDriverFlash::E_MinimumFlashloaderResetWaitTimeType e_WaitType;
                              stw::opensyde_core::C_OscProtocolDriverOsyNode c_LastRouter;

                              // Check what interface is used of target (not the local bus)
                              tgl_assert(this->mpc_ComDriver->GetRoutingTargetInterfaceType(
                                            u16_Node, e_TargetInterfaceType) == C_NO_ERR);
                              if (e_TargetInterfaceType == C_OscSystemBus::eCAN)
                              {
                                 e_WaitType = C_OscComDriverFlash::eNO_CHANGES_CAN;
                              }
                              else
                              {
                                 e_WaitType = C_OscComDriverFlash::eNO_CHANGES_ETHERNET;
                              }

                              tgl_assert(this->GetMinimumFlashloaderResetWaitTime(e_WaitType,
                                                                                  this->mc_CurrentNode,
                                                                                  u32_WaitTime) == C_NO_ERR);

                              tgl_assert(this->mpc_ComDriver->GetServerIdOfLastRouter(u16_Node,
                                                                                      c_LastRouter) == C_NO_ERR);

                              u32_StartTime = stw::tgl::TglGetTickCount();
                              u32_LastSentTesterPresent = u32_StartTime;

                              // Give the targets some time to reset and initialize their interfaces
                              // and wait the minimum time
                              do
                              {
                                 u32_CurrentTime = stw::tgl::TglGetTickCount();

                                 // Tester present is only necessary in case of CAN. In case of Ethernet the entire
                                 // routing will be restarted anyway by m_ReconnectToTargetServer
                                 if ((e_TargetInterfaceType == C_OscSystemBus::eCAN) &&
                                     (u32_CurrentTime > (u32_LastSentTesterPresent + u32_INTERVAL_TESTER_PRESENT)))
                                 {
                                    // We need TesterPresent to keep the routing connection alive
                                    // Send it to the last routing point
                                    s32_Return = this->mpc_ComDriver->SendTesterPresent(c_LastRouter);

                                    if (s32_Return != C_NO_ERR)
                                    {
                                       (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR, s32_Return,
                                                              50U,
                                                              this->mc_CurrentNode,
                                                              "Sending Tester Present to router node failed.");
                                       s32_Return = C_COM;
                                       break;
                                    }

                                    u32_LastSentTesterPresent = u32_CurrentTime;
                                 }

                                 TglSleep(5);
                              }
                              while (u32_CurrentTime < (u32_WaitTime + u32_StartTime));

                              if (s32_Return == C_NO_ERR)
                              {
                                 s32_Return = this->m_ReconnectToTargetServer(true, u16_Node);
                              }
                           }

                           if (s32_Return == C_NO_ERR)
                           {
                              //If we are on Ethernet the node is in flashloader but not in programming mode yet
                              // we need to send one EnterPreProgramming request to get it there
                              //If we're on CAN one more of this services does not hurt as well :-)
                              //We want to confirm the device is in flashloader anyway.
                              //So we use this service (it will fail if the node is in the application
                              // as there is no "PreProgramming" session there.
                              s32_Return =
                                 this->mpc_ComDriver->SendOsySetPreProgrammingMode(mc_CurrentNode, true, &u8_NrCode);

                              if (s32_Return != C_NO_ERR)
                              {
                                 (void)m_ReportProgress(
                                    eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR, s32_Return, 50U,
                                    mc_CurrentNode,
                                    "Request to set active session for routing device failed. Details:" +
                                    C_OscProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));

                                 // Node is not reachable
                                 this->mc_TimeoutNodes[u16_Node] = static_cast<uint8_t>(s32_Return == C_TIMEOUT);

                                 if (s32_Return != C_CHECKSUM)
                                 {
                                    s32_Return = C_COM;
                                 }
                                 else
                                 {
                                    this->mc_ConnectStatesNodes[u16_Node].q_AuthenticationError = true;
                                 }
                              }

                              (void)this->m_DisconnectFromTargetServer(false);
                           }
                        }
                        else
                        {
                           //STW Flashloader node
                           s32_Return = this->mpc_ComDriver->SendStwRequestNodeReset(this->mc_CurrentNode);
                           if (s32_Return != C_NO_ERR)
                           {
                              (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR, s32_Return, 50U,
                                                     mc_CurrentNode,
                                                     "Sending request failed.");

                              s32_Return = C_COM;
                           }

                           if (s32_Return == C_NO_ERR)
                           {
                              const uint32_t u32_StartTime = stw::tgl::TglGetTickCount();
                              uint32_t u32_WaitTime;

                              // Get the minimum wait time of the concrete routing node
                              tgl_assert(this->GetMinimumFlashloaderResetWaitTime(
                                            C_OscComDriverFlash::eNO_CHANGES_CAN,
                                            this->mc_CurrentNode,
                                            u32_WaitTime) == C_NO_ERR);

                              do
                              {
                                 s32_Return = this->mpc_ComDriver->SendStwSendFlash(this->mc_CurrentNode);

                                 if (s32_Return != C_NO_ERR)
                                 {
                                    (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR, s32_Return, 50U,
                                                           mc_CurrentNode,
                                                           "Send \"FLASH\" broadcast failed.");
                                    s32_Return = C_COM;
                                    break;
                                 }

                                 stw::tgl::TglSleep(5);
                              }
                              while (stw::tgl::TglGetTickCount() < (u32_WaitTime + u32_StartTime));

                              if (s32_Return == C_NO_ERR)
                              {
                                 //it should be in Sleep state now. The only service that we can use is wakeup.
                                 //will not cause any problems as the node will go back to sleep if another node is
                                 // addressed later on
                                 s32_Return = this->mpc_ComDriver->SendStwWakeupLocalId(mc_CurrentNode, NULL);

                                 if (s32_Return != C_NO_ERR)
                                 {
                                    (void)m_ReportProgress(
                                       eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR, s32_Return, 50U,
                                       mc_CurrentNode, "Could not perform node wakeup for routing device.");

                                    // Node is not reachable
                                    this->mc_TimeoutNodes[u16_Node] = 1U;
                                 }
                              }
                           }
                        }
                     }
                     else
                     {
                        if (s32_Return == C_NOACT)
                        {
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR, s32_Return,
                                                  50U, mc_CurrentNode,
                                                  "Starting routing for node failed due to not capable node");
                        }
                        else
                        {
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_ROUTING_ERROR, s32_Return, 50U,
                                                  mc_CurrentNode,
                                                  "Starting routing for node failed");
                        }
                        osc_write_log_error(
                           "Activate Flashloader",
                           "Activate Flashloader: Start of routing for node (" + C_SclString::IntToStr(
                              u16_Node) + ") failed with error code: " +
                           C_OscLoggingHandler::h_StwError(s32_Return));

                        // Routing node and target node is not reachable
                        this->mc_TimeoutNodes[u16_Node] = static_cast<uint8_t>(s32_Return == C_TIMEOUT);
                        this->mc_TimeoutNodes[u32_ErrorIndex] = static_cast<uint8_t>(s32_Return == C_TIMEOUT);

                        this->mc_ConnectStatesNodes[u16_Node].q_CouldNotReachedDueToRoutingError = true;

                        if ((s32_Return != C_CONFIG) &&
                            (s32_Return != C_RANGE))
                        {
                           this->mc_ConnectStatesNodes[u32_ErrorIndex].q_RoutingNodeError = true;
                        }

                        if (s32_Return == C_CHECKSUM)
                        {
                           this->mc_ConnectStatesNodes[u32_ErrorIndex].q_AuthenticationError = true;
                        }
                     }

                     // Stop routing always to clean up
                     this->mpc_ComDriver->StopRouting(u16_Node);
                  }
                  else
                  {
                     // At least one node is not reachable to get to the current node
                     s32_Return = C_TIMEOUT;

                     (void)m_ReportProgress(eACTIVATE_FLASHLOADER_ROUTING_ERROR, s32_Return, 50U,
                                            mc_CurrentNode, "Starting routing for node failed");
                     osc_write_log_error(
                        "Activate Flashloader",
                        "Activate Flashloader: Start of routing for node (" + C_SclString::IntToStr(
                           u16_Node) + ") failed due to not available node on route with error code: " +
                        C_OscLoggingHandler::h_StwError(s32_Return));
                  }

                  if (s32_Return != C_NO_ERR)
                  {
                     q_AtLeastOneError = true;
                     if (oq_FailOnFirstError == true)
                     {
                        break;
                     }
                  }
               }
            }

            if (this->mc_TimeoutNodes[u16_Node] == 1U)
            {
               this->mc_ConnectStatesNodes[u16_Node].q_Timeout = true;
            }
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eACTIVATE_FLASHLOADER_FINISHED, C_NO_ERR, 100U, "Flashloader activated on all devices.");
   }

   if (q_AtLeastOneError == true)
   {
      s32_Return = C_WARN;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information of server nodes

   Prerequisite:
   All nodes marked as active in mc_ActiveNodes are present and are in flashloader mode
   i.e. the system is in the state after a successful call to ActivateFlashloader()

   For the API it would be better to first read all information from STW Flashloader nodes and return it and then from
   openSYDE nodes in another function or vice versa.
   But for routing it makes sense to set it up sequentially and handle the system bus by bus.

   So this function reports found information via virtual functions:
   It calls either
   * m_ReportStwFlashloaderInformationRead or
   * m_ReportOpenSydeFlashloaderInformationRead

   The sequence for routing is the same as in "ActivateFlashloader".

   Services for openSYDE:
   * service readDataByIdentifier(DeviceName)
   * service readFlashBlockData
   * one fixed application info block for the flashloader
   * ReadDataById(BootSwIdentification)
   * ReadDataById(ApplicationSwFingerprint)
   * ReadDataById(ECUSerialNumber)
   * ReadDataById(SystemSupplierECUHWNumber)
   * ReadDataById(SystemSupplierECUHWVersion)
   * ReadDataById(ProtocolVersion)
   * ReadDataById(FlashloaderProtocolVersion)
   * ReadDataById(FlashCount)

   Services for STW Flashloader
   * service get_device_id
   * services get_device_info_address and subsequent read_flash
   * <V3: not supported
   * >V3 <V3.03: one application info block supported
   * >=V3.03: up to 32 application info blocks supported
   * Use "C_XFLActions::Read*" functions; basically most of the stuff that WinFlash displays in the "Configure Node"
      dialog

   \param[in]   oq_FailOnFirstError   true: abort all further communication if communication with one device fails
                                      false: try to continue with other devices in this case

   \return
   C_CONFIG   mpc_SystemDefinition is NULL (Init() not called)
   C_BUSY     aborted by user
   C_WARN     reading failed for at least one node
   C_NO_ERR   information read
   C_COM      communication driver reported error (details will be written to log file)
   C_CHECKSUM Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::ReadDeviceInformation(const bool oq_FailOnFirstError)
{
   int32_t s32_Return = C_NO_ERR;
   bool q_AtLeastOneError = false;
   bool q_AtLeastOneAuthentificationError = false;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      if (this->mc_ConnectStatesNodes.size() == 0)
      {
         // In case of not used ActivateFlashloader function
         this->mc_ConnectStatesNodes.resize(this->mc_ActiveNodes.size());
      }

      for (uint16_t u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
      {
         //set progress based on node index:
         const uint8_t u8_Progress =
            static_cast<uint8_t>(((static_cast<size_t>(u16_Node) + 1U) * 100U) /
                                 (this->mpc_SystemDefinition->c_Nodes.size() + 1U));
         const bool q_Abort = m_ReportProgress(eREAD_DEVICE_INFO_START, C_NO_ERR, u8_Progress,
                                               "Reading device information from device ...");
         if (q_Abort == true)
         {
            s32_Return = C_BUSY;
         }
         else
         {
            const bool q_IsNodeReachable = this->m_IsNodeReachable(u16_Node);

            if (q_IsNodeReachable == true)
            {
               uint32_t u32_BusIndex;
               bool q_RoutingActivated = false;

               s32_Return = this->mpc_ComDriver->GetBusIndexOfRoutingNode(u16_Node, u32_BusIndex);

               if (s32_Return == C_NOACT)
               {
                  // No routing necessary
                  u32_BusIndex = this->mu32_ActiveBusIndex;
                  s32_Return = C_NO_ERR;
               }
               else if (s32_Return == C_NO_ERR)
               {
                  // Routing necessary
                  s32_Return = this->mpc_ComDriver->StartRouting(u16_Node);
                  q_RoutingActivated = true;
               }
               else
               {
                  // Error case
               }

               if (s32_Return == C_NO_ERR)
               {
                  C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
                  const bool q_Return = m_IsNodeActive(u16_Node, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
                  if (q_Return == true)
                  {
                     if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
                     {
                        (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_START, C_NO_ERR, u8_Progress, mc_CurrentNode,
                                               "Reading openSYDE device information ...");
                        s32_Return = this->m_ReadDeviceInformationOpenSyde(u8_Progress, u16_Node,
                                                                           this->mc_ConnectStatesNodes[u16_Node]);

                        if (s32_Return == C_NO_ERR)
                        {
                           (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FINISHED, C_NO_ERR, u8_Progress,
                                                  mc_CurrentNode,
                                                  "openSYDE device information read.");
                        }
                     }
                     else
                     {
                        (void)m_ReportProgress(eREAD_DEVICE_INFO_XFL_START, C_NO_ERR, u8_Progress, mc_CurrentNode,
                                               "Reading STW Flashloader device information ...");
                        s32_Return = this->m_ReadDeviceInformationStwFlashloader(u8_Progress, u16_Node,
                                                                                 this->mc_ConnectStatesNodes[u16_Node]);

                        if (s32_Return == C_NO_ERR)
                        {
                           (void)m_ReportProgress(eREAD_DEVICE_INFO_XFL_FINISHED, C_NO_ERR, u8_Progress,
                                                  mc_CurrentNode,
                                                  "STW Flashloader device information read.");
                        }
                     }
                  } //end node active
               }

               if (q_RoutingActivated == true)
               {
                  // Stop routing always to clean up
                  this->mpc_ComDriver->StopRouting(u16_Node);
               }
            }
            else
            {
               // At least one node is not reachable to get to the current node
               s32_Return = C_TIMEOUT;

               osc_write_log_warning("Read device information",
                                     "Read device information: Node (" + C_SclString::IntToStr(u16_Node) +
                                     ") is not reachable due to previous timeout.");
            }
         }
         if (s32_Return != C_NO_ERR)
         {
            if (s32_Return == C_CHECKSUM)
            {
               q_AtLeastOneAuthentificationError = true;
            }

            q_AtLeastOneError = true;
            if (oq_FailOnFirstError == true)
            {
               break;
            }
            s32_Return = C_NO_ERR;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_FINISHED, C_NO_ERR, 100U,
                             "Device information read from all devices.");
   }

   if (q_AtLeastOneError == true)
   {
      if (q_AtLeastOneAuthentificationError == true)
      {
         s32_Return = C_CHECKSUM;
      }
      else
      {
         s32_Return = C_WARN;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read information of server nodes

   Prerequisite:
   All nodes marked as active in mc_ActiveNodes are present and are in flashloader mode
   i.e. the system is in the state after a successful call to ActivateFlashloader()

   Flash all configured files to defined devices.

   Sequence:
   * for all nodes on local bus:
   ** flash all configured applications for all nodes on bus
   ** write all defined NVM parameter set files to NVM
   * for all buses that can be reached via routing
   ** set up routing
   ** for all nodes on bus:
   *** flash all configured applications
   *** write all defined NVM parameter set files to NVM

   Progress report:
   * Reports 0..100 for the overall process
   * Reports 0..100 for each file of each individual node being flashed

   \param[in]  orc_ApplicationsToWrite   list of files to flash per node; must have the same size as the system
                                          definition contains nodes
   \param[in]  orc_NodesOrder            Vector with node update order (index is update position, value is node index)

   \return
   C_NO_ERR    flashed all files
   C_CONFIG    mpc_SystemDefinition is NULL (Init() not called)
               parameter writing: one of the files contains data for zero or more than one device
                (expected: data for exactly one device)
   C_OVERFLOW  size of orc_ApplicationsToWrite is not the same as the size of nodes in mpc_SystemDefinition
               for address based targets: device name of device does not match name contained in hex file
   C_NOACT     orc_ApplicationsToWrite has non-empty list of flash or NVM files for node that was not set as active
               orc_ApplicationsToWrite has non-empty list of flash or NVM files for node that has no position in the
                update order
               orc_ApplicationsToWrite has non-empty list of flash or NVM files for node that has more than one position
                in the update order
               for address based targets: could not extract device name from hex file
   C_RD_WR     file referenced by orc_ApplicationsToWrite does not exist
               for address based targets: one of the flash files is not a valid Intel or Motorola hex file
               for address based targets: could not split up flash file into individual memory areas
               parameter writing: one of the files is not a valid .psi_syde file or does not exist
   C_CONFIG    for address based targets: no signature block found in flash hex file
               for STW Flashloader targets: NVM files are defined (not supported by STW Flashloader)
   C_COM       communication driver reported problem (details will be written to log file)
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
   C_DEFAULT   parameter writing: one of the files is present but checksum is invalid
   C_WARN      The file is present but key details of PEM file could not be extracted
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
   C_RANGE     At least one feature of the openSYDE Flashloader is not available for NVM writing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::UpdateSystem(const std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                       const std::vector<uint32_t> & orc_NodesOrder)
{
   int32_t s32_Return = C_NO_ERR;

   this->mu32_CurrentNode = 0U;
   this->mu32_CurrentFile = 0U;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else if (orc_ApplicationsToWrite.size() != this->mpc_SystemDefinition->c_Nodes.size())
   {
      s32_Return = C_OVERFLOW;
   }
   else
   {
      uint32_t u32_NodesToFlash = 0U;

      // Reset states
      this->mc_UpdateStatesNodes.clear();
      this->mc_UpdateStatesNodes.resize(this->mc_ActiveNodes.size());

      //are all nodes supposed to get flashed active ?
      for (uint16_t u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
      {
         C_OscSuSequencesNodeUpdateStates & rc_State = this->mc_UpdateStatesNodes[u16_Node];
         // Prepare the node states for its files
         if (this->mpc_SystemDefinition->c_Nodes[u16_Node].c_Properties.e_FlashLoader ==
             C_OscNodeProperties::eFL_OPEN_SYDE)
         {
            const C_OscDeviceDefinition * const pc_DeviceDefinition =
               this->mpc_SystemDefinition->c_Nodes[u16_Node].pc_DeviceDefinition;
            const uint32_t u32_SubDeviceIndex =
               this->mpc_SystemDefinition->c_Nodes[u16_Node].u32_SubDeviceIndex;

            tgl_assert(pc_DeviceDefinition != NULL);
            if (pc_DeviceDefinition != NULL)
            {
               if (pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased == false)
               {
                  rc_State.c_StateHexFiles.resize(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size());
               }
               else
               {
                  rc_State.c_StateOtherFiles.resize(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size());
               }
            }

            // PSI files
            rc_State.c_StatePsiFiles.resize(orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm.size());
         }
         else if (this->mpc_SystemDefinition->c_Nodes[u16_Node].c_Properties.e_FlashLoader ==
                  C_OscNodeProperties::eFL_STW)
         {
            // STW Flashloader
            rc_State.c_StateStwFlHexFiles.resize(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size());
         }
         else
         {
            // Nothing to do
         }

         if ((orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size() != 0) ||
             (orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm.size() != 0) ||
             (orc_ApplicationsToWrite[u16_Node].c_PemFile != ""))
         {
            if (this->mc_ActiveNodes[u16_Node] == false)
            {
               osc_write_log_error("System Update",
                                   "File(s) to flash or write to NVM configured for node " + C_SclString::IntToStr(
                                      u16_Node) + " which is not marked as active !");
               s32_Return = C_NOACT;
            }
            else
            {
               uint32_t u32_PositionCounter;

               ++u32_NodesToFlash;

               // Does the node have a position ?
               s32_Return = C_NOACT;
               for (u32_PositionCounter = 0U; u32_PositionCounter < orc_NodesOrder.size(); ++u32_PositionCounter)
               {
                  if (orc_NodesOrder[u32_PositionCounter] == static_cast<uint32_t>(u16_Node))
                  {
                     s32_Return = C_NO_ERR;
                     break;
                  }
               }

               if (s32_Return == C_NO_ERR)
               {
                  //do all files exist ?
                  //files for flash:
                  for (uint32_t u32_File = 0U; u32_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size();
                       u32_File++)
                  {
                     // Getting the correct state variable
                     C_OscSuSequencesNodeFileStates * pc_FileState = NULL;
                     if (rc_State.c_StateHexFiles.size() > 0)
                     {
                        pc_FileState = &rc_State.c_StateHexFiles[u32_File];
                     }
                     else if (rc_State.c_StateOtherFiles.size() > 0)
                     {
                        pc_FileState = &rc_State.c_StateOtherFiles[u32_File];
                     }
                     else if (rc_State.c_StateStwFlHexFiles.size() > 0)
                     {
                        pc_FileState = &rc_State.c_StateStwFlHexFiles[u32_File];
                     }
                     else
                     {
                        tgl_assert(false);
                     }

                     if (pc_FileState != NULL)
                     {
                        // Save the file name
                        pc_FileState->c_FileName =
                           TglExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u32_File]);
                     }

                     if (TglFileExists(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u32_File]) == false)
                     {
                        osc_write_log_error("System Update", "Could not find file \"" +
                                            orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u32_File] + "\" !");
                        if (pc_FileState != NULL)
                        {
                           pc_FileState->e_FileExists = eSUSEQ_STATE_ERROR;
                        }
                        s32_Return = C_RD_WR;
                        break;
                     }
                     else
                     {
                        if (pc_FileState != NULL)
                        {
                           pc_FileState->e_FileExists = eSUSEQ_STATE_NO_ERR;
                        }
                     }
                  }
                  //files for Nvm:
                  for (uint32_t u32_File = 0U; u32_File < orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm.size();
                       u32_File++)
                  {
                     // Save the file name
                     rc_State.c_StatePsiFiles[u32_File].c_FileName =
                        TglExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm[u32_File]);

                     if (TglFileExists(orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm[u32_File]) == false)
                     {
                        osc_write_log_error("System Update", "Could not find file \"" +
                                            orc_ApplicationsToWrite[u16_Node].c_FilesToWriteToNvm[u32_File] + "\" !");
                        rc_State.c_StatePsiFiles[u32_File].e_FileExists = eSUSEQ_STATE_ERROR;
                        s32_Return = C_RD_WR;
                        break;
                     }
                     else
                     {
                        rc_State.c_StatePsiFiles[u32_File].e_FileExists = eSUSEQ_STATE_NO_ERR;
                     }
                  }
                  // PEM file
                  if (orc_ApplicationsToWrite[u16_Node].c_PemFile != "")
                  {
                     rc_State.c_StatePemFile.c_FileName = TglExtractFileName(
                        orc_ApplicationsToWrite[u16_Node].c_PemFile);
                     if (TglFileExists(orc_ApplicationsToWrite[u16_Node].c_PemFile) == false)
                     {
                        osc_write_log_error("System Update", "Could not find file \"" +
                                            orc_ApplicationsToWrite[u16_Node].c_PemFile + "\" !");
                        rc_State.c_StatePemFile.e_FileExists = eSUSEQ_STATE_ERROR;
                        s32_Return = C_RD_WR;
                     }
                     else
                     {
                        rc_State.c_StatePemFile.e_FileExists = eSUSEQ_STATE_NO_ERR;
                     }
                  }
                  else
                  {
                     // No PEM file exists
                     rc_State.c_StatePemFile.c_FileName = "";
                     rc_State.c_StatePemFile.e_FileExists = eSUSEQ_STATE_NOT_NEEDED;
                     rc_State.c_StatePemFile.e_FileLoaded = eSUSEQ_STATE_NOT_NEEDED;
                     rc_State.c_StatePemFile.e_PemFileExtracted = eSUSEQ_STATE_NOT_NEEDED;
                     rc_State.c_StatePemFile.e_SecurityKeySent = eSUSEQ_STATE_NOT_NEEDED;
                     rc_State.c_StatePemFile.e_SecurityFlagSent = eSUSEQ_STATE_NOT_NEEDED;
                     rc_State.c_StatePemFile.e_DebuggerFlagSent = eSUSEQ_STATE_NOT_NEEDED;
                  }
               }
            }
            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         // Check the order configuration
         // One node shall have maximum one position
         // The number of positions must match with number of nodes to flash
         if (orc_NodesOrder.size() == u32_NodesToFlash)
         {
            uint32_t u32_OrderPos;
            uint32_t u32_OrderPosCompare;

            for (u32_OrderPos = 0U; u32_OrderPos < orc_NodesOrder.size(); ++u32_OrderPos)
            {
               for (u32_OrderPosCompare = 0U; u32_OrderPosCompare < orc_NodesOrder.size(); ++u32_OrderPosCompare)
               {
                  if ((u32_OrderPos != u32_OrderPosCompare) &&
                      (orc_NodesOrder[u32_OrderPos] == orc_NodesOrder[u32_OrderPosCompare]))
                  {
                     // Node has more than one position
                     s32_Return = C_NOACT;
                     break;
                  }
               }

               if (s32_Return != C_NO_ERR)
               {
                  break;
               }
            }
         }
         else
         {
            s32_Return = C_NOACT;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_START, C_NO_ERR, 0U, "Starting System Update ...");
      for (uint32_t u32_Position = 0U; u32_Position < orc_NodesOrder.size(); u32_Position++)
      {
         const uint32_t u32_NodeIndex = orc_NodesOrder[u32_Position];
         C_OscSuSequencesNodeUpdateStates & rc_NodeUpdateStates = this->mc_UpdateStatesNodes[u32_NodeIndex];

         // Save node index
         this->mu32_CurrentNode = u32_NodeIndex;

         //flash openSYDE nodes
         if ((orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash.size() > 0) ||
             (orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToWriteToNvm.size() > 0) ||
             (orc_ApplicationsToWrite[u32_NodeIndex].c_PemFile != ""))
         {
            uint32_t u32_BusIndex;
            bool q_RoutingActivated = false;

            s32_Return = this->mpc_ComDriver->GetBusIndexOfRoutingNode(u32_NodeIndex, u32_BusIndex);

            if (s32_Return == C_NOACT)
            {
               // No routing necessary
               u32_BusIndex = this->mu32_ActiveBusIndex;
               s32_Return = C_NO_ERR;
            }
            else if (s32_Return == C_NO_ERR)
            {
               uint32_t u32_ErrorIndex = 0U;
               // Routing necessary
               s32_Return = this->mpc_ComDriver->StartRouting(u32_NodeIndex, &u32_ErrorIndex);

               if (s32_Return != C_NO_ERR)
               {
                  // Handle routing specific errors
                  this->mc_ConnectStatesNodes[u32_NodeIndex].q_CouldNotReachedDueToRoutingError = true;
                  if ((s32_Return != C_CONFIG) &&
                      (s32_Return != C_RANGE))
                  {
                     this->mc_ConnectStatesNodes[u32_ErrorIndex].q_RoutingNodeError = true;
                  }

                  if (s32_Return == C_CHECKSUM)
                  {
                     this->mc_ConnectStatesNodes[u32_ErrorIndex].q_AuthenticationError = true;
                  }
               }

               q_RoutingActivated = true;
            }
            else
            {
               // Error case
            }

            if (s32_Return == C_NO_ERR)
            {
               C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;

               const bool q_Return = m_IsNodeActive(u32_NodeIndex, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
               if (q_Return == true)
               {
                  if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
                  {
                     const C_OscDeviceDefinition * const pc_DeviceDefinition =
                        this->mpc_SystemDefinition->c_Nodes[u32_NodeIndex].pc_DeviceDefinition;
                     const uint32_t u32_SubDeviceIndex =
                        this->mpc_SystemDefinition->c_Nodes[u32_NodeIndex].u32_SubDeviceIndex;
                     (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_START, C_NO_ERR, 10U, mc_CurrentNode,
                                            "Starting device update ...");
                     tgl_assert(pc_DeviceDefinition != NULL);
                     if (pc_DeviceDefinition != NULL)
                     {
                        C_OscProtocolDriverOsy::C_ListOfFeatures c_AvailableFeatures;
                        bool q_SetProgrammingMode = true;

                        //if connected via Ethernet we might need to reconnect (in case we ran into the session timeout)
                        s32_Return = this->m_ReconnectToTargetServer();
                        if (s32_Return != C_NO_ERR)
                        {
                           (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_RECONNECT_ERROR, s32_Return, 10U, mc_CurrentNode,
                                                  "Could not reconnect to node");
                           if (s32_Return != C_CHECKSUM)
                           {
                              s32_Return = C_COM;
                           }
                        }

                        if (s32_Return == C_NO_ERR)
                        {
                           //check which protocol features are available
                           s32_Return = this->mpc_ComDriver->SendOsyReadListOfFeatures(this->mc_CurrentNode,
                                                                                       c_AvailableFeatures);
                           if (s32_Return != C_NO_ERR)
                           {
                              (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR, s32_Return, 10U,
                                                     mc_CurrentNode,
                                                     "Could not read available openSYDE Flashloader features.");
                              s32_Return = C_COM;
                           }
                        }

                        //files to flash ?
                        if ((s32_Return == C_NO_ERR) &&
                            (orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash.size() > 0))
                        {
                           tgl_assert(u32_SubDeviceIndex < pc_DeviceDefinition->c_SubDevices.size());
                           //address based or file based ?
                           if (pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeIsFileBased ==
                               false)
                           {
                              s32_Return = m_FlashNodeOpenSydeHex(
                                 orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash,
                                 orc_ApplicationsToWrite[u32_NodeIndex].c_OtherAcceptedDeviceNames,
                                 pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].u32_FlashloaderOpenSydeRequestDownloadTimeout,
                                 pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].u32_FlashloaderOpenSydeTransferDataTimeout,
                                 q_SetProgrammingMode,
                                 rc_NodeUpdateStates.c_StateHexFiles);
                           }
                           else
                           {
                              s32_Return = m_FlashNodeOpenSydeFile(
                                 orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash,
                                 pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].u32_FlashloaderOpenSydeRequestDownloadTimeout,
                                 pc_DeviceDefinition->c_SubDevices[u32_SubDeviceIndex].u32_FlashloaderOpenSydeTransferDataTimeout,
                                 c_AvailableFeatures,
                                 q_SetProgrammingMode,
                                 rc_NodeUpdateStates.c_StateOtherFiles);
                           }
                        }

                        //files to write to Nvm ?
                        if ((s32_Return == C_NO_ERR) &&
                            (orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToWriteToNvm.size() > 0))
                        {
                           s32_Return =
                              m_WriteNvmOpenSyde(orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToWriteToNvm,
                                                 c_AvailableFeatures,
                                                 q_SetProgrammingMode,
                                                 rc_NodeUpdateStates.c_StatePsiFiles);
                        }

                        // Special case: An other security level is necessary for the next steps.
                        // The next step must set the programming mode with the other security level again
                        q_SetProgrammingMode = true;

                        // PEM file to write?
                        if (s32_Return == C_NO_ERR)
                        {
                           s32_Return = m_WritePemOpenSydeFile(orc_ApplicationsToWrite[u32_NodeIndex].c_PemFile,
                                                               c_AvailableFeatures,
                                                               q_SetProgrammingMode,
                                                               rc_NodeUpdateStates.c_StatePemFile);
                        }

                        // States to write?
                        if (s32_Return == C_NO_ERR)
                        {
                           s32_Return = m_WriteOpenSydeNodeStates(orc_ApplicationsToWrite[u32_NodeIndex],
                                                                  c_AvailableFeatures,
                                                                  q_SetProgrammingMode,
                                                                  rc_NodeUpdateStates.c_StatePemFile);
                        }

                        (void)this->m_DisconnectFromTargetServer();
                     }
                     if (s32_Return == C_NO_ERR)
                     {
                        (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FINISHED, C_NO_ERR, 100U, mc_CurrentNode,
                                               "Finishing device update ...");
                     }
                  }
                  else
                  {
                     //flash STW Flashloader nodes
                     s32_Return = m_FlashNodeXfl(orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash,
                                                 rc_NodeUpdateStates.c_StateStwFlHexFiles);

                     if ((s32_Return == C_NO_ERR) &&
                         ((orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToWriteToNvm.size() > 0) ||
                          (orc_ApplicationsToWrite[u32_NodeIndex].c_PemFile != "")))
                     {
                        //writing to NVM and PEM files are not supported by STW Flashloader
                        s32_Return = C_CONFIG;
                     }
                  }
               }
            }

            if (q_RoutingActivated == true)
            {
               // Stop routing always to clean up
               this->mpc_ComDriver->StopRouting(u32_NodeIndex);
            }
         }
         if (s32_Return != C_NO_ERR)
         {
            switch (s32_Return)
            {
            case C_TIMEOUT:
               rc_NodeUpdateStates.q_Timeout = true;
               break;
            case C_CHECKSUM:
               rc_NodeUpdateStates.q_AuthenticationError = true;
               break;
            default:
               // Nothing to do
               break;
            }

            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_FINISHED, C_NO_ERR, 100U, "Finished System Update.");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reset all devices in the system

   Reset all nodes backwards from the tiniest twigs up to the mighty trunk
   For STW Flashloader we can send a NET Reset

   Sequence:
   * for all openSYDE nodes on local bus:
   ** for openSYDE nodes: send ECU-Reset requests (do not use broadcast as we cannot route it)
   ** for STW Flashloader nodes: send NET-Reset (broadcast)

   Progress report:
   * Reports 0..100 for the overall process

   \return
   C_NO_ERR    reset requests were sent out to all nodes
   C_CONFIG    mpc_SystemDefinition is NULL (Init() not called)
   C_COM       communication driver reported problem (details will be written to log file)
   C_CHECKSUM  Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::ResetSystem(void)
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      const uint32_t u32_LongestRoute = this->mpc_ComDriver->GetRoutingPointMaximum();
      int32_t s32_RouteSizeCounter;
      std::set<uint32_t> c_XflResetSentForBus;

      (void)m_ReportProgress(eRESET_SYSTEM_START, C_NO_ERR, 0U, "Resetting System ...");

      // Reset the system beginning with the longest route.
      // Avoiding conflicts with resetting a node, which is necessary for an other route
      for (s32_RouteSizeCounter = static_cast<int32_t>(u32_LongestRoute);
           s32_RouteSizeCounter >= 0; --s32_RouteSizeCounter)
      {
         for (uint32_t u32_Node = 0U; u32_Node < this->mpc_SystemDefinition->c_Nodes.size(); u32_Node++)
         {
            const bool q_IsNodeReachable = this->m_IsNodeReachable(u32_Node);

            if (q_IsNodeReachable == true)
            {
               bool q_Return;
               const uint32_t u32_RoutePointCount = this->mpc_ComDriver->GetRoutingPointCount(u32_Node, q_Return);

               if ((static_cast<int32_t>(u32_RoutePointCount) == s32_RouteSizeCounter) &&
                   (q_Return == true))
               {
                  uint32_t u32_BusIndex;
                  bool q_RoutingActivated = false;

                  s32_Return = this->mpc_ComDriver->GetBusIndexOfRoutingNode(u32_Node, u32_BusIndex);

                  if (s32_Return == C_NOACT)
                  {
                     // No routing necessary
                     u32_BusIndex = this->mu32_ActiveBusIndex;
                     s32_Return = C_NO_ERR;
                  }
                  else if (s32_Return == C_NO_ERR)
                  {
                     // Routing necessary
                     s32_Return = this->mpc_ComDriver->StartRouting(u32_Node);
                     q_RoutingActivated = true;
                  }
                  else
                  {
                     // Error case
                  }

                  if (s32_Return == C_NO_ERR)
                  {
                     //reset nodes
                     C_OscNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
                     q_Return = m_IsNodeActive(u32_Node, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
                     if (q_Return == true)
                     {
                        if (e_ProtocolType == C_OscNodeProperties::eFL_OPEN_SYDE)
                        {
                           s32_Return = this->m_ReconnectToTargetServer();

                           if (s32_Return == C_NO_ERR)
                           {
                              s32_Return = this->mpc_ComDriver->SendOsyEcuReset(
                                 mc_CurrentNode,
                                 C_OscProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_KEY_OFF_ON);

                              (void)this->m_DisconnectFromTargetServer();
                           }
                        }
                        else
                        {
                           //send reset broadcast (only required once per network):
                           if (c_XflResetSentForBus.find(u32_BusIndex) == c_XflResetSentForBus.end())
                           {
                              // In case of routing, we need a concrete device for the reset
                              s32_Return = this->mpc_ComDriver->SendStwNetReset(this->mc_CurrentNode);
                              c_XflResetSentForBus.insert(u32_BusIndex);
                           }
                        }
                     }
                     if (s32_Return != C_NO_ERR)
                     {
                        // 0 equals the local bus
                        if (s32_RouteSizeCounter == 0)
                        {
                           (void)m_ReportProgress(eRESET_SYSTEM_OSY_NODE_ERROR, s32_Return, 0U, mc_CurrentNode,
                                                  "Could not reset routed node.");
                        }
                        else
                        {
                           (void)m_ReportProgress(eRESET_SYSTEM_OSY_ROUTED_NODE_ERROR, s32_Return, 0U, mc_CurrentNode,
                                                  "Could not reset routed node.");
                        }
                        s32_Return = C_COM;
                        break;
                     }
                  }

                  if (q_RoutingActivated == true)
                  {
                     // Reset command does not send a response. Need a little wait time to guarantee that
                     // the routing nodes really send the reset command
                     TglSleep(20);

                     // Stop routing always because of cleaning up the tp and legacy routing dispatcher
                     this->mpc_ComDriver->StopRouting(u32_Node);
                  }
               }
            }
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eRESET_SYSTEM_FINISHED, C_NO_ERR, 100U, "System was reset.");
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node states of the connect process

   The connect process involves the both steps:
   * ActivateFlashloader
   * ReadDeviceInformation

   \param[out]      orc_ConnectStatesNodes   Detailed output parameter description

   \retval   C_NO_ERR   States returned
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::GetConnectStates(std::vector<C_OscSuSequencesNodeConnectStates> & orc_ConnectStatesNodes)
const
{
   orc_ConnectStatesNodes = this->mc_ConnectStatesNodes;
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node states of the connect process

   The connect process involves the both steps:
   * ActivateFlashloader
   * ReadDeviceInformation

   \param[out]      orc_UpdateStatesNodes   Detailed output parameter description

   \retval   C_NO_ERR   States returned
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::GetUpdateStates(std::vector<C_OscSuSequencesNodeUpdateStates> & orc_UpdateStatesNodes) const
{
   orc_UpdateStatesNodes = this->mc_UpdateStatesNodes;
   return C_NO_ERR;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert information read from openSYDE server node to string list

   Can be used by the application for a no-frills approach to get a textual representation of the information
   reported by m_ReportOpenSydeFlashloaderInformationRead.

   \param[in]     orc_Info         openSYDE server node information
   \param[out]    orc_Text         textual representation of read information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSuSequences::h_OpenSydeFlashloaderInformationToText(const C_OsyDeviceInformation & orc_Info,
                                                              C_SclStringList & orc_Text)
{
   C_SclString c_Line;

   orc_Text.Clear();
   orc_Text.Add("Device name: " + orc_Info.c_DeviceName);
   orc_Text.Add("Number of applications: " + C_SclString::IntToStr(orc_Info.c_Applications.size()));
   for (uint8_t u8_Application = 0U; u8_Application < orc_Info.c_Applications.size(); u8_Application++)
   {
      orc_Text.Add("Application " + C_SclString::IntToStr(u8_Application));
      orc_Text.Add(" Name: " + orc_Info.c_Applications[u8_Application].c_ApplicationName);
      orc_Text.Add(" Version: " + orc_Info.c_Applications[u8_Application].c_ApplicationVersion);
      orc_Text.Add(" Build date: " + orc_Info.c_Applications[u8_Application].c_BuildDate);
      orc_Text.Add(" Build time: " + orc_Info.c_Applications[u8_Application].c_BuildTime);
      orc_Text.Add(" Block start address: 0x" +
                   C_SclString::IntToHex(static_cast<int64_t>(orc_Info.c_Applications[u8_Application].
                                                              u32_BlockStartAddress), 8U));
      orc_Text.Add(" Block end address: 0x" +
                   C_SclString::IntToHex(static_cast<int64_t>(orc_Info.c_Applications[u8_Application].
                                                              u32_BlockEndAddress), 8U));
      orc_Text.Add(static_cast<C_SclString>(" Signature valid: ") +
                   ((orc_Info.c_Applications[u8_Application].u8_SignatureValid == 0) ? "yes" : "no"));
      orc_Text.Add(" Additional information: " + orc_Info.c_Applications[u8_Application].c_AdditionalInformation);
   }

   c_Line.PrintFormatted("Flashloader software version: V%d.%02dr%d",
                         orc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[0],
                         orc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[1],
                         orc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[2]);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Flashloader protocol version: V%d.%02dr%d",
                         orc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[0],
                         orc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[1],
                         orc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[2]);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Protocol version: V%d.%02dr%d", orc_Info.c_MoreInformation.au8_ProtocolVersion[0],
                         orc_Info.c_MoreInformation.au8_ProtocolVersion[1],
                         orc_Info.c_MoreInformation.au8_ProtocolVersion[2]);
   orc_Text.Add(c_Line);
   orc_Text.Add("Flash count: " + C_SclString::IntToStr(orc_Info.c_MoreInformation.u32_FlashCount));
   c_Line = "Device serial number: " + orc_Info.c_MoreInformation.GetEcuSerialNumber() + " " +
            orc_Info.c_MoreInformation.GetEcuSerialNumberFormatDescription();
   orc_Text.Add(c_Line);
   orc_Text.Add("Device article number: " + C_SclString::IntToStr(orc_Info.c_MoreInformation.u32_EcuArticleNumber));
   orc_Text.Add("Device article version: " + orc_Info.c_MoreInformation.c_EcuHardwareVersionNumber);
   c_Line.PrintFormatted("Flash fingerprint date: %02d-%02d-%02d (yy-mm-dd)",
                         orc_Info.c_MoreInformation.au8_FlashFingerprintDate[0],
                         orc_Info.c_MoreInformation.au8_FlashFingerprintDate[1],
                         orc_Info.c_MoreInformation.au8_FlashFingerprintDate[2]);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Flash fingerprint time: %02d:%02d:%02d",
                         orc_Info.c_MoreInformation.au8_FlashFingerprintTime[0],
                         orc_Info.c_MoreInformation.au8_FlashFingerprintTime[1],
                         orc_Info.c_MoreInformation.au8_FlashFingerprintTime[2]);
   orc_Text.Add(c_Line);
   orc_Text.Add("Flash fingerprint username: " + orc_Info.c_MoreInformation.c_FlashFingerprintUserName);
   c_Line.PrintFormatted("NVM writing available: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_FlashloaderCanWriteToNvm == true) ? 1 : 0);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Security supported: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsSecurity == true) ? 1 : 0);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Disabling debugger supported: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsDebuggerOff == true) ? 1 : 0);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Enabling debugger supported: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_SupportsDebuggerOn == true) ? 1 : 0);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Maximum block size information available: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable ==
                          true) ? 1 : 0);
   orc_Text.Add(c_Line);
   if (orc_Info.c_MoreInformation.c_AvailableFeatures.q_MaxNumberOfBlockLengthAvailable == true)
   {
      c_Line.PrintFormatted("Maximum block size: %d", orc_Info.c_MoreInformation.u16_MaxNumberOfBlockLength);
      orc_Text.Add(c_Line);
   }
   c_Line.PrintFormatted("Ethernet2Ethernet routing supported: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_EthernetToEthernetRoutingSupported ==
                          true) ? 1 : 0);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("FileBasedTransferExitResult available: %d",
                         (orc_Info.c_MoreInformation.c_AvailableFeatures.q_FileBasedTransferExitResultAvailable ==
                          true) ? 1 : 0);
   orc_Text.Add(c_Line);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert information read from STW Flashloader server node to string list

   Can be used by the application for a no-frills approach to get a textual representation of the information
   reported by m_ReportStwFlashloaderInformationRead.
   Checksum block or sector information is not added to the text.

   \param[in]     orc_Info         STW Flashloader server node information
   \param[out]    orc_Text         textual representation of read information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSuSequences::h_StwFlashloaderInformationToText(const C_XflDeviceInformation & orc_Info,
                                                         C_SclStringList & orc_Text)
{
   C_SclString c_Line;

   orc_Text.Clear();

   if (orc_Info.c_BasicInformation.q_DeviceIDValid == true)
   {
      orc_Text.Add("Device name: " + orc_Info.c_BasicInformation.c_DeviceId);
   }
   else
   {
      orc_Text.Add("Device name: unknown");
   }

   if (orc_Info.c_BasicInformation.q_DeviceInfoAddressesValid == true)
   {
      tgl_assert(orc_Info.c_BasicInformation.c_DeviceInfoAddresses.GetLength() ==
                 orc_Info.c_BasicInformation.c_DeviceInfoBlocks.GetLength());
      tgl_assert(orc_Info.c_BasicInformation.c_DeviceInfoAddresses.GetLength() ==
                 orc_Info.c_BasicInformation.c_DeviceInfoBlocksValid.GetLength());

      orc_Text.Add("Number of applications: " +
                   C_SclString::IntToStr(orc_Info.c_BasicInformation.c_DeviceInfoAddresses.GetLength()));
      for (uint8_t u8_Application = 0U; u8_Application < orc_Info.c_BasicInformation.c_DeviceInfoAddresses.GetLength();
           u8_Application++)
      {
         orc_Text.Add("Application " + C_SclString::IntToStr(u8_Application));
         orc_Text.Add(" Device info address: 0x" +
                      C_SclString::IntToHex(static_cast<int64_t>(orc_Info.c_BasicInformation.c_DeviceInfoAddresses[
                                                                    u8_Application]), 8));
         orc_Info.c_BasicInformation.c_DeviceInfoBlocks[u8_Application].AddInfoToList(orc_Text);
         orc_Text.Add("\n");
      }
   }
   else
   {
      orc_Text.Add("Number of applications: unknown");
   }

   if (orc_Info.c_BasicInformation.q_FlashloaderVersionValid == true)
   {
      c_Line = orc_Info.c_BasicInformation.c_FlashloaderVersion;
   }
   else
   {
      c_Line = "unknown";
   }
   orc_Text.Add("Flashloader software version: " + c_Line);

   if (orc_Info.c_BasicInformation.q_ProtocolVersionValid == true)
   {
      c_Line.PrintFormatted("V%x.%x%xr%x\n",
                            static_cast<uint8_t>((orc_Info.c_BasicInformation.u16_ProtocolVersion >> 12U) & 0x0FU),
                            static_cast<uint8_t>((orc_Info.c_BasicInformation.u16_ProtocolVersion >> 8U) & 0x0FU),
                            static_cast<uint8_t>((orc_Info.c_BasicInformation.u16_ProtocolVersion >> 4U) & 0x0FU),
                            static_cast<uint8_t>((orc_Info.c_BasicInformation.u16_ProtocolVersion) & 0x0FU));
   }
   else
   {
      c_Line = "unknown (< V3.00r0)";
   }
   orc_Text.Add("Protocol version: " + c_Line);

   if (orc_Info.c_BasicInformation.q_EraseCountValid == true)
   {
      c_Line = C_SclString::IntToStr(orc_Info.c_BasicInformation.u32_EraseCount);
   }
   else
   {
      c_Line = "unknown";
   }
   orc_Text.Add("Flash count: " + c_Line);

   if (orc_Info.c_BasicInformation.q_SerialNumberValid == true)
   {
      c_Line = orc_Info.c_BasicInformation.c_SerialNumber;
   }
   else
   {
      c_Line = "unknown";
   }
   orc_Text.Add("Device serial number: " + c_Line);

   //finger print data:
   if (orc_Info.c_BasicInformation.c_AvailableFeatures.q_FingerPrint == false)
   {
      orc_Text.Add("Flash fingerprint: not supported by this device");
   }
   else
   {
      if (orc_Info.c_BasicInformation.c_FingerPrintData.q_SupportedIndexesValid == false)
      {
         orc_Text.Add("Flash fingerprint: unknown");
      }
      else
      {
         if ((orc_Info.c_BasicInformation.c_FingerPrintData.c_SupportedIndexes.q_ProgrammingDate == true) &&
             (orc_Info.c_BasicInformation.c_FingerPrintData.q_ProgrammingDateValid == true))
         {
            c_Line.PrintFormatted("%02d-%02d-%02d (yy-mm-dd)",
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u8_ProgrammingDateYear,
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u8_ProgrammingDateMonth,
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u8_ProgrammingDateDay);
         }
         else
         {
            c_Line = "unknown";
         }
         orc_Text.Add("Flash fingerprint date: " + c_Line);

         if ((orc_Info.c_BasicInformation.c_FingerPrintData.c_SupportedIndexes.q_ProgrammingTime == true) &&
             (orc_Info.c_BasicInformation.c_FingerPrintData.q_ProgrammingTimeValid == true))
         {
            c_Line.PrintFormatted("%02d:%02d:%02d",
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u8_ProgrammingTimeHour,
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u8_ProgrammingTimeMinute,
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u8_ProgrammingTimeSecond);
         }
         else
         {
            c_Line = "unknown";
         }
         orc_Text.Add("Flash fingerprint time: " + c_Line);

         if ((orc_Info.c_BasicInformation.c_FingerPrintData.c_SupportedIndexes.q_UsernamePart1 == true) &&
             (orc_Info.c_BasicInformation.c_FingerPrintData.q_UserNameValid == true))
         {
            c_Line = orc_Info.c_BasicInformation.c_FingerPrintData.c_UserName;
         }
         else
         {
            c_Line = "unknown";
         }
         orc_Text.Add("Flash fingerprint username: " + c_Line);

         if ((orc_Info.c_BasicInformation.c_FingerPrintData.c_SupportedIndexes.q_ApplicationCheckSum == true) &&
             (orc_Info.c_BasicInformation.c_FingerPrintData.q_ChecksumValid == true))
         {
            c_Line.PrintFormatted("Flash fingerprint checksum: 0x%08x",
                                  orc_Info.c_BasicInformation.c_FingerPrintData.u32_Checksum);
         }
         else
         {
            c_Line = "Flash fingerprint checksum: unknown";
         }
         orc_Text.Add(c_Line);
      }
   }

   orc_Text.Add("Flash memory information:");

   if (orc_Info.c_BasicInformation.q_SectorCountValid == true)
   {
      c_Line = C_SclString::IntToStr(orc_Info.c_BasicInformation.u16_SectorCount);
   }
   else
   {
      c_Line = "unknown";
   }
   orc_Text.Add(" Number of flash sectors: " + c_Line);

   if (orc_Info.c_BasicInformation.q_FlashloaderVersionValid == true)
   {
      int32_t s32_Index;
      C_XFLFlashSectors c_SectorTable;
      uint32_t u32_TotalSize = 0U;
      uint32_t u32_TotalSizeWriteable = 0U;

      const C_XFLFlashInformation & rc_FlashInfo = orc_Info.c_BasicInformation.c_FlashMappingInformation;

      orc_Text.Add(" Flash memory details:");

      //print whole flash information data:
      //first raw information:
      if (rc_FlashInfo.c_ProtectedSectors.GetLength() == 0)
      {
         orc_Text.Add("  No protected sectors.");
      }
      for (s32_Index = 0; s32_Index < rc_FlashInfo.c_ProtectedSectors.GetLength(); s32_Index++)
      {
         c_Line.PrintFormatted("  Protected sector: IC %03d, Sector %05d",
                               rc_FlashInfo.c_ProtectedSectors[s32_Index].u8_ICIndex,
                               rc_FlashInfo.c_ProtectedSectors[s32_Index].u16_SectorNumber);
         orc_Text.Add(c_Line);
      }

      orc_Text.Add("  Number of flash ICs: " + C_SclString::IntToStr(rc_FlashInfo.c_ICs.GetLength()));

      for (s32_Index = 0; s32_Index < rc_FlashInfo.c_ICs.GetLength(); s32_Index++)
      {
         c_Line.PrintFormatted(
            "  IC %03d: size: 0x%08X; sec0: 0x%08X; regions: 0x%02X; erasetime: %ums; progtime: %ums",
            s32_Index,
            rc_FlashInfo.c_ICs[s32_Index].u32_TotalSize,
            rc_FlashInfo.c_ICs[s32_Index].u32_Sector0Offset,
            rc_FlashInfo.c_ICs[s32_Index].c_Regions.GetLength(),
            rc_FlashInfo.c_ICs[s32_Index].u32_SectorEraseTime,
            rc_FlashInfo.c_ICs[s32_Index].u32_ProgrammingTime);
         orc_Text.Add(c_Line);
         for (int32_t s32_RegionIndex = 0; s32_RegionIndex < rc_FlashInfo.c_ICs[s32_Index].c_Regions.GetLength();
              s32_RegionIndex++)
         {
            c_Line.PrintFormatted("  IC %03d, region 0x%02X: blocksize: 0x%08X; numblocks: 0x%04X",
                                  s32_Index,
                                  static_cast<int32_t>(s32_RegionIndex),
                                  static_cast<int32_t>(rc_FlashInfo.c_ICs[s32_Index].c_Regions[s32_RegionIndex].
                                                       u32_BlockSize),
                                  rc_FlashInfo.c_ICs[s32_Index].c_Regions[s32_RegionIndex].u16_NumBlocks);
            orc_Text.Add(c_Line);
         }
      }

      //then linearized information
      //match hexfile to address areas
      orc_Text.Add("  Linear sector map:\n");

      rc_FlashInfo.ConvertToFlashSectorTable(c_SectorTable);

      for (s32_Index = 0; s32_Index < c_SectorTable.GetLength(); s32_Index++)
      {
         uint32_t u32_SectorSize;

         //was there a gap before this sector:
         if (s32_Index > 0)
         {
            if (c_SectorTable[s32_Index].u32_LowestAddress > (c_SectorTable[s32_Index - 1].u32_HighestAddress + 1))
            {
               //gap of at least one byte !
               c_Line.PrintFormatted("  Gap: Start: 0x%08X; Size: 0x%08X",
                                     c_SectorTable[s32_Index - 1].u32_HighestAddress + 1,
                                     c_SectorTable[s32_Index].u32_LowestAddress -
                                     (c_SectorTable[s32_Index - 1].u32_HighestAddress + 1));
               orc_Text.Add(c_Line);
            }
         }

         u32_SectorSize = (c_SectorTable[s32_Index].u32_HighestAddress - c_SectorTable[s32_Index].u32_LowestAddress) +
                          1;
         u32_TotalSize += u32_SectorSize;
         if (c_SectorTable[s32_Index].q_IsProtected == false)
         {
            u32_TotalSizeWriteable += u32_SectorSize;
         }

         c_Line.PrintFormatted("  Sector %05d: Start: 0x%08X; Size: 0x%08X; IC: %03d; Read: yes; Write: %s",
                               s32_Index,
                               c_SectorTable[s32_Index].u32_LowestAddress,
                               u32_SectorSize,
                               c_SectorTable[s32_Index].u8_ICIndex,
                               (c_SectorTable[s32_Index].q_IsProtected == true) ? "no" : "yes");
         orc_Text.Add(c_Line);
      }
      orc_Text.Add("\n");
      c_Line.PrintFormatted("  Total size: 0x%08X", u32_TotalSize);
      orc_Text.Add(c_Line);
      c_Line.PrintFormatted("  Total non-protected size: 0x%08X", u32_TotalSizeWriteable);
      orc_Text.Add(c_Line);

      //Aliased memory
      if (rc_FlashInfo.c_Aliases.GetLength() != 0)
      {
         orc_Text.Add("\n");
         orc_Text.Add("  Number of aliased memory regions: " +
                      C_SclString::IntToStr(rc_FlashInfo.c_Aliases.GetLength()));
         for (s32_Index = 0; s32_Index < rc_FlashInfo.c_Aliases.GetLength(); s32_Index++)
         {
            c_Line.PrintFormatted("  Aliased region %03d: Physical: 0x%08X; Aliased: 0x%08X; Size: 0x%08X",
                                  s32_Index,
                                  rc_FlashInfo.c_Aliases[s32_Index].u32_PhysicalAddress,
                                  rc_FlashInfo.c_Aliases[s32_Index].u32_AliasedAddress,
                                  rc_FlashInfo.c_Aliases[s32_Index].u32_Size);
            orc_Text.Add(c_Line);
         }
      }
   }
   else
   {
      orc_Text.Add(" Flash memory details: not available");
   }

   if (orc_Info.c_BasicInformation.q_ImplementationInformationHexFileInformationValid == true)
   {
      orc_Text.Add(" HEX file: Maximum record length: " +
                   C_SclString::IntToStr(orc_Info.c_BasicInformation.u8_ImplementationInformationMaxHexRecordLength));
      orc_Text.Add(" HEX file: Granularity: " +
                   C_SclString::IntToStr(orc_Info.c_BasicInformation.u8_ImplementationInformationHexRecordGranularity));
   }
   else
   {
      orc_Text.Add(" HEX file: Maximum record length: unknown");
      orc_Text.Add(" HEX file: Granularity: unknown");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare two instances

   \param[in]   orc_Source   instance to compare *this against

   \return
   true:   orc_Source == (*this)
   false:  orc_Source != (*this)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscSuSequences::C_ApplicationProperties::operator ==(const C_ApplicationProperties & orc_Source) const
{
   return((this->c_Name == orc_Source.c_Name) &&
          (this->c_Version == orc_Source.c_Version) &&
          (this->c_BuildDate == orc_Source.c_BuildDate) &&
          (this->c_BuildTime == orc_Source.c_BuildTime));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnects the current server

   Checking if the target node is really connected to Ethernet.
   It is only necessary in case of ETH -> ETH routing and
   not in case of ETH -> CAN routing! In case of ETH -> CAN routing the target and
   restarted device is not connected to Ethernet and the Ethernet routing device must not be
   reconnected to hold the routing configuration active.

   \param[in]     oq_RestartRouting    In case of ETH -> ETH routing the routing must be restarted if necessary
   \param[in]     ou32_NodeIndex       Node index (only used if oq_RestartRouting is true)

   \return
   C_NO_ERR   re-connected or no reconnect necessary
   C_BUSY     could not re-connect to node
   C_RANGE    node not found or no openSYDE protocol installed
   C_COM      communication driver reported error (details will be written to log file)
   C_CHECKSUM Security related error (something went wrong while handshaking with the server)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_ReconnectToTargetServer(const bool oq_RestartRouting, const uint32_t ou32_NodeIndex)
{
   uint32_t u32_BusCounter;
   int32_t s32_Return = C_NO_ERR;

   for (u32_BusCounter = 0U; u32_BusCounter < this->mpc_SystemDefinition->c_Buses.size(); ++u32_BusCounter)
   {
      if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].u8_BusId == this->mc_CurrentNode.u8_BusIdentifier)
      {
         if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].e_Type == C_OscSystemBus::eETHERNET)
         {
            if (oq_RestartRouting == true)
            {
               if (this->mc_CurrentNode.u8_BusIdentifier != this->mpc_ComDriver->GetClientId().u8_BusIdentifier)
               {
                  // Restart routing when the routing is still necessary. Only a problem when using
                  // Ethernet to Ethernet routing a target is connected to Ethernet and not CAN.
                  // The routing connection on the last router server will be dropped by reseting the target server.
                  s32_Return = this->mpc_ComDriver->StartRouting(ou32_NodeIndex);
               }
            }

            if (s32_Return == C_NO_ERR)
            {
               s32_Return = this->mpc_ComDriver->ReConnectNode(mc_CurrentNode);
            }
         }
         break;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnects the current server

   \param[in]  oq_DisconnectOnIp2IpRouting    Flag if a disconnect in case of Ethernet to Ethernet routing shall be
                                              executed. If the routing is still necessary after this call, the
                                              disconnect shall not be executed.

   \return
   C_NO_ERR   disconnected or no reconnect necessary
   C_NOACT    could not disconnect to node
   C_RANGE    node not found or no openSYDE protocol installed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSuSequences::m_DisconnectFromTargetServer(const bool oq_DisconnectOnIp2IpRouting)
{
   uint32_t u32_BusCounter;
   int32_t s32_Return = C_NO_ERR;

   // Disconnect the current server
   for (u32_BusCounter = 0U; u32_BusCounter < this->mpc_SystemDefinition->c_Buses.size(); ++u32_BusCounter)
   {
      if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].u8_BusId == this->mc_CurrentNode.u8_BusIdentifier)
      {
         if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].e_Type == C_OscSystemBus::eETHERNET)
         {
            if (this->mc_CurrentNode.u8_BusIdentifier == this->mpc_ComDriver->GetClientId().u8_BusIdentifier)
            {
               s32_Return = this->mpc_ComDriver->DisconnectNode(mc_CurrentNode);
            }
            else if (oq_DisconnectOnIp2IpRouting == true)
            {
               uint32_t u32_NodeIndex;

               this->mpc_ComDriver->GetNodeIndex(this->mc_CurrentNode, u32_NodeIndex);
               this->mpc_ComDriver->StopRouting(u32_NodeIndex);
            }
            else
            {
               // Nothing to do
            }
         }
         break;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns an adapted device transfer data timeout time for compensating a potential higher bus load

   The device specific time does only cover the needed time of the device itself, but not the real communication
   time. In case of a high bus load this time can vary a lot.

   Information for the calculation:
   * Adding the result of the calculation as offset to the original time
   * Assuming we have 7 bytes per CAN frame
   * The lowest supported bitrate is 100kB/s
   * -> 2ms for each CAN message roughly results in the number of messages for each block * 2ms
   * Assuming a high "Alien" busload of 80% we need to multiply by 5
   -> (((ou32_MaxBlockLength / 7) * 2ms) * 5) + original device timeout time

   In a routing scenario when considering the transfer time we need to consider the slowest possible bus.
   As we support routing from Ethernet to CAN, but not the other way around this means that the last bus
    (i.e. the one connected to the target device) is the slowest (for our purpose we can assume CAN is always slower
     than Ethernet).
    However there is one scenario not covered by this: PC <-> CAN@LowSpeed <-> CAN@HighSpeed <-> Device.
    So there is room for further improvement.

   \param[in]       ou32_DeviceTransferDataTimeout     Device specific timeout time as base
   \param[in]       ou32_MaxBlockLength                Maximum number of bytes of each block
   \param[in]       ou8_BusIdentifier                  Bus identifier of the bus the server is connected to

   \return
   Calculated device transfer data timeout
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscSuSequences::m_GetAdaptedTransferDataTimeout(const uint32_t ou32_DeviceTransferDataTimeout,
                                                           const uint32_t ou32_MaxBlockLength,
                                                           const uint8_t ou8_BusIdentifier) const
{
   uint32_t u32_AdaptedTime = ou32_DeviceTransferDataTimeout;
   uint32_t u32_BusCounter;
   bool q_IsCan = false;
   uint32_t u32_Bitrate = 0U;

   // Search the matching bus for the bus identifier
   for (u32_BusCounter = 0U; u32_BusCounter < this->mpc_SystemDefinition->c_Buses.size(); ++u32_BusCounter)
   {
      const C_OscSystemBus & rc_Bus = this->mpc_SystemDefinition->c_Buses[u32_BusCounter];

      if (ou8_BusIdentifier == rc_Bus.u8_BusId)
      {
         if (rc_Bus.e_Type == C_OscSystemBus::eCAN)
         {
            q_IsCan = true;
            u32_Bitrate = static_cast<uint32_t>(rc_Bus.u64_BitRate / 1000ULL);
         }
         break;
      }
   }

   // In case of Ethernet no offset is necessary
   if (q_IsCan == true)
   {
      const uint32_t u32_Offset = (ou32_MaxBlockLength * 10U) / 7U;

      tgl_assert(u32_Bitrate != 0U);

      if (u32_Bitrate == 0U)
      {
         //this should not happen ... assume the slowest we support (100kBit/s)
         u32_Bitrate = 100U;
      }

      // Scale the offset to the expected bitrate on the bus in relation to the slowest bitrate of 100 kBit/s
      u32_AdaptedTime += (u32_Offset * 100U) / u32_Bitrate;
   }

   osc_write_log_info("Update Node",
                      "Used \"transferdatatimeout\" by adding an offset for compensating a "
                      "potential high bus load: " +
                      C_SclString::IntToStr(u32_AdaptedTime) + "ms");

   return u32_AdaptedTime;
}
