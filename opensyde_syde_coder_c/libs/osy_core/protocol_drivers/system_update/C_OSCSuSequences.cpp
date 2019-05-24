//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Sequences for system update.

   For details cf. documentation in .h file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <fstream>
#include <iterator>
#include <set>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCSuSequences.h"
#include "CSCLString.h"
#include "CSCLDateTime.h"
#include "CSCLChecksums.h"
#include "TGLUtils.h"
#include "TGLTime.h"
#include "TGLFile.h"
#include "C_OsyHexFile.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_diag_lib;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Utility: check whether node is an active node on a specific bus

   Assumptions:
   * mpc_SystemDefinition and mc_ActiveNodes must be valid
   * node and bus index must be within the range of nodes contained in mpc_SystemDefinition

   \param[in]  ou32_NodeIndex     index of node within mpc_SystemDefinition
   \param[in]  ou32_BusIndex      index of bus within mpc_SystemDefinition
   \param[out] ore_ProtocolType   if found: type of flashloader protocol defined for node
   \param[out] orc_NodeId         if found: node id of the node on the specified bus

   \return
   true    node is active on specified bus
   false   node not active on specified bus
*/
//-----------------------------------------------------------------------------
bool C_OSCSuSequences::m_IsNodeActive(const uint32 ou32_NodeIndex, const uint32 ou32_BusIndex,
                                      C_OSCNodeProperties::E_FlashLoaderProtocol & ore_ProtocolType,
                                      C_OSCProtocolDriverOsyNode & orc_NodeId) const
{
   bool q_Return = false;
   const C_OSCNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[ou32_NodeIndex];

   //is the node intends to be addressed at all (defined by parameter set in Init() function)
   if (this->mc_ActiveNodes[ou32_NodeIndex] == 1U)
   {
      for (uint16 u16_Interface = 0U; u16_Interface < rc_Node.c_Properties.c_ComInterfaces.size(); u16_Interface++)
      {
         const C_OSCNodeComInterfaceSettings & rc_Settings = rc_Node.c_Properties.c_ComInterfaces[u16_Interface];

         if ((rc_Settings.q_IsBusConnected == true) && (rc_Settings.u32_BusIndex == ou32_BusIndex) &&
             (rc_Settings.q_IsUpdateEnabled == true))
         {
            //we cannot know for sure whether the target is in flashloader or application mode
            //prerequisite: a node is either
            //* a full openSYDE node (Flashloader + Application) or
            //* an STW Flashloader node
            //openSYDE node ?
            if (rc_Node.c_Properties.e_DiagnosticServer == C_OSCNodeProperties::eDS_OPEN_SYDE)
            {
               tgl_assert(rc_Node.c_Properties.e_FlashLoader == C_OSCNodeProperties::eFL_OPEN_SYDE);
            }

            q_Return = true;
            ore_ProtocolType = rc_Node.c_Properties.e_FlashLoader;
            orc_NodeId.u8_NodeIdentifier = rc_Settings.u8_NodeID;
            orc_NodeId.u8_BusIdentifier = this->mpc_SystemDefinition->c_Buses[ou32_BusIndex].u8_BusID;
            break; //found the interface that is connected to the specified bus
         }
      }
   }
   return q_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Utility: check whether node is directly required in update

   Assumptions:
   * mpc_SystemDefinition and mc_ActiveNodes must be valid
   * node and bus index must be within the range of nodes contained in mpc_SystemDefinition

   \param[in] ou32_NodeIndex     index of node within mpc_SystemDefinition

   \return
   true    node is directly required for update
   false   node not directly required for update (might still be necessary for routing!)
*/
//-----------------------------------------------------------------------------
bool C_OSCSuSequences::m_IsNodeRequired(const uint32 ou32_NodeIndex) const
{
   bool q_Return;
   const C_OSCNode & rc_Node = this->mpc_SystemDefinition->c_Nodes[ou32_NodeIndex];

   if (rc_Node.c_Applications.size() > 0UL)
   {
      q_Return = true;
   }
   else
   {
      q_Return = false;
   }
   return q_Return;
}

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
bool C_OSCSuSequences::m_ReportProgress(const E_ProgressStep oe_Step, const sint32 os32_Result,
                                        const uint8 ou8_Progress, const C_SCLString & orc_Information)
{
   std::cout << "Step: " << static_cast<uint32>(oe_Step) << " Result: " << os32_Result << " Progress: " <<
      static_cast<uint32>(ou8_Progress) << " Info: "  << orc_Information.c_str() << "\n";

   return false;
}

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
bool C_OSCSuSequences::m_ReportProgress(const E_ProgressStep oe_Step, const sint32 os32_Result,
                                        const uint8 ou8_Progress, const C_OSCProtocolDriverOsyNode & orc_Server,
                                        const C_SCLString & orc_Information)
{
   std::cout << "Step: " << static_cast<uint32>(oe_Step) << " Result: " << os32_Result << " Progress: " <<
      static_cast<uint32>(ou8_Progress) << " Bus Id: " << static_cast<uint32>(orc_Server.u8_BusIdentifier) <<
      " Node Id: " << static_cast<uint32>(orc_Server.u8_NodeIdentifier) << " Info: " <<
      orc_Information.c_str() << "\n";

   return false;
}

//-----------------------------------------------------------------------------
/*! \brief   Reports information read from openSYDE server node

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.
   Default implementation here: print read information to console

   \param[in]     orc_Info         Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//-----------------------------------------------------------------------------
void C_OSCSuSequences::m_ReportOpenSydeFlashloaderInformationRead(const C_OsyDeviceInformation & orc_Info,
                                                                  const uint32 ou32_NodeIndex)
{
   C_SCLStringList c_Text;

   h_OpenSydeFlashloaderInformationToText(orc_Info, c_Text);

   std::cout << "openSYDE device information found for node with index " << ou32_NodeIndex << "\n";
   for (uint32 u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      std::cout << c_Text.Strings[u32_Line].c_str() << "\n";
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Reports information read from STW flashloader server node

   Called by ReadDeviceInformation() after it has read information from an STW flashloader node.
   Default implementation here: print read information to console; checksum block or sector information is not printed

   \param[in]     orc_Info         Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//-----------------------------------------------------------------------------
void C_OSCSuSequences::m_ReportStwFlashloaderInformationRead(const C_XflDeviceInformation & orc_Info,
                                                             const uint32 ou32_NodeIndex)
{
   C_SCLStringList c_Text;

   h_StwFlashloaderInformationToText(orc_Info, c_Text);

   std::cout << "STW Flashloader device information found for node with index " << ou32_NodeIndex << "\n";
   for (uint32 u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      std::cout << c_Text.Strings[u32_Line].c_str() << "\n";
   }
}

//-----------------------------------------------------------------------------
/*! \brief   report progress of STW Flashloader operation to application

   Report progress information from STW Flashloader driver via virtual function.

   \param[in]   ou8_Progress     progress of operation (from 0 to 100)
                                 progress invalid: 255
   \param[in]   orc_Text         textual information about progress

   \return
   C_NO_ERR    continue operation
   else        abort operation (not honored at each position)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_XflReportProgress(const uint8 ou8_Progress, const C_SCLString & orc_Text)
{
   sint32 s32_Return = C_NO_ERR;

   //add node information:
   bool q_Return = this->m_ReportProgress(eXFL_PROGRESS, C_NO_ERR, ou8_Progress, mc_CurrentNode, orc_Text);

   if (q_Return == true)
   {
      s32_Return = C_BUSY; //abort
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
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

   \param[in]     orc_FilesToFlash              Files to flash
   \param[in]     ou32_RequestDownloadTimeout   Maximum time in ms it can take to erase one continuous area in flash
   \param[in]     ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to flash

   \return
   C_NO_ERR    flashed all files
   C_RD_WR     one of the files is not a valid Intel or Motorola hex file
               could not split up hex file in individual areas
   C_CONFIG    no signature block found in hex file
   C_COM       communication driver reported problem
   C_NOACT     could not extract device name from hex file
   C_OVERFLOW  device name of device does not match name contained in hex file
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_FlashNodeOpenSydeHex(const std::vector<C_SCLString> & orc_FilesToFlash,
                                                const uint32 ou32_RequestDownloadTimeout,
                                                const uint32 ou32_TransferDataTimeout)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_Return;

   std::vector<uint32> c_SignatureAddresses(orc_FilesToFlash.size()); ///< addresses of signatures within hex files

   //C_OsyHexFile cannot be copied; so we cannot put it into a resizable vector
   //-> create instances manually
   std::vector<C_OsyHexFile *> c_Files(orc_FilesToFlash.size());
   for (uint32 u32_File = 0U; u32_File < c_Files.size(); u32_File++)
   {
      c_Files[u32_File] = new C_OsyHexFile();
   }

   //try to open files to check whether we have valid hex files before we start messing with the target's flash memory:
   for (uint32 u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Opening HEX file " + orc_FilesToFlash[u32_File] + ".");
      u32_Return = c_Files[u32_File]->LoadFromFile(orc_FilesToFlash[u32_File].c_str());
      if (u32_Return != stw_hex_file::NO_ERR)
      {
         C_SCLString c_ErrorText = c_Files[u32_File]->ErrorCodeToErrorText(u32_Return);
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR, C_RD_WR, 0U, mc_CurrentNode,
                                "Opening HEX file. Reason: " + c_ErrorText + ".");
         s32_Return = C_RD_WR;
      }
      else
      {
         //try to find signature address in hex file:
         s32_Return = c_Files[u32_File]->GetSignatureBlockAddress(c_SignatureAddresses[u32_File]);
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR, s32_Return, 0U, mc_CurrentNode,
                                   "Could not find signature block in HEX file.");
            s32_Return = C_CONFIG;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      C_SCLString c_DeviceName;
      uint8 u8_NrCode;

      //if connected via Ethernet we might need to reconnect (in case we ran into the session timeout)
      s32_Return = this->m_ReconnectToTargetServer();
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_RECONNECT_ERROR, s32_Return, 10U, mc_CurrentNode,
                                "Could not reconnect to node");
         s32_Return = C_COM;
      }
      if (s32_Return == C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START, C_NO_ERR, 10U, mc_CurrentNode,
                                "X-checking device name of device against HEX file contents ...");
         //get target device name for comparison with PC-side files:
         s32_Return = this->mpc_ComDriver->SendOsyReadDeviceName(mc_CurrentNode, c_DeviceName, &u8_NrCode);
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR, s32_Return, 10U,
                                   mc_CurrentNode, "Could not read device name from device. Details:" +
                                   C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
            s32_Return = C_COM;
         }
      }
      if (s32_Return == C_NO_ERR)
      {
         //for all files check whether:
         //* device type in file matches target's device type
         for (uint32 u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            C_SCLString c_DeviceNameHexFile;

            //get device ID from hex file
            s32_Return = c_Files[u32_File]->ScanDeviceIdFromHexFile(c_DeviceNameHexFile);
            if (s32_Return != C_NO_ERR)
            {
               C_SCLString c_ErrorText = "Could not read device name from file " + orc_FilesToFlash[u32_File] +
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
               s32_Return = C_NOACT;
            }
            else
            {
               if (c_DeviceName.Trim() != c_DeviceNameHexFile.Trim())
               {
                  C_SCLString c_ErrorText = "Device names of device and HEX file " + orc_FilesToFlash[u32_File] +
                                            " do not match. Device reported: \"" + c_DeviceName.Trim() +
                                            "\". HEX file contains: \"" +
                                            c_DeviceNameHexFile.Trim() + "\".";
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR, C_OVERFLOW, 10U,
                                         mc_CurrentNode, c_ErrorText);
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
      s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR, s32_Return, 20U, mc_CurrentNode,
                                "Could not activate programming session.");
         s32_Return = C_COM;
      }
      else
      {
         for (uint32 u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            //do we have enough space for the hex file data ?
            const stw_hex_file::C_HexDataDump * const pc_HexDump = c_Files[u32_File]->GetDataDump(u32_Return);
            if (u32_Return != stw_hex_file::NO_ERR)
            {
               C_SCLString c_ErrorText;
               c_ErrorText = "Could not split up HEX file data of file " + orc_FilesToFlash[u32_File] +
                             " into handy chunks. Reason: " + c_Files[u32_File]->ErrorCodeToErrorText(u32_Return);
               (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR, C_RD_WR, 20U, mc_CurrentNode,
                                      c_ErrorText);
               s32_Return = C_RD_WR;
            }
            else
            {
               for (uint16 u16_Area = 0U; u16_Area < pc_HexDump->at_Blocks.GetLength(); u16_Area++)
               {
                  uint8 u8_NrCode;
                  s32_Return =
                     this->mpc_ComDriver->SendOsyCheckFlashMemoryAvailable(
                        mc_CurrentNode,
                        pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                        pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength(),
                        &u8_NrCode);
                  if (s32_Return != C_NO_ERR)
                  {
                     C_SCLString c_ErrorText;
                     c_ErrorText.PrintFormatted("Could not get confirmation about flash memory availability. (File: %s" \
                                                " Offset: 0x%08x Size: 0x%08x). Details: %s",
                                                orc_FilesToFlash[u32_File].c_str(),
                                                pc_HexDump->at_Blocks[u16_Area].u32_AddressOffset,
                                                pc_HexDump->at_Blocks[u16_Area].au8_Data.GetLength(),
                                                C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                         u8_NrCode).c_str());
                     (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK, s32_Return, 20U,
                                            mc_CurrentNode, c_ErrorText);
                     s32_Return = C_COM;
                     break;
                  }
               }
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
         for (uint32 u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            const stw_hex_file::C_HexDataDump * const pc_HexDump = c_Files[u32_File]->GetDataDump(u32_Return);
            //we would not have gotten here if we could not get a decent dump ...
            tgl_assert(pc_HexDump != NULL);

            // Save file index
            this->mu32_CurrentFile = u32_File;

            //lint -e{613}  //see assertion
            s32_Return = m_FlashOneFileOpenSydeHex(*pc_HexDump, c_SignatureAddresses[u32_File],
                                                   ou32_RequestDownloadTimeout, ou32_TransferDataTimeout);
            if (s32_Return != C_NO_ERR)
            {
               //error report is already in subfunction
               break;
            }
         }
      }
   }

   this->m_DisconnectFromTargetServer();

   //clean up hex file instances:
   for (uint32 u32_File = 0U; u32_File < c_Files.size(); u32_File++)
   {
      delete c_Files[u32_File];
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Flash one hex file to openSYDE address based node

   Assumptions/prerequisites (not explicitly checked by this function):
   * mc_CurrentNode contains ID of node to work with
   * driver is correctly set up
   * target is in programming session with security level 3 active
   * file was checked to be OK (valid hex file; signature present)
   * file matches the target device name

   Reports progress from 0..100

   \param[in]     orc_HexDataDump               Dump of file to flash
   \param[in]     ou32_SignatureAddress         address of signature block within hex file
   \param[in]     ou32_RequestDownloadTimeout   Maximum time in ms it can take to erase one continuous area in flash
   \param[in]     ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to flash

   \return
   C_NO_ERR   file flashed
   C_COM      communication driver reported problem
   C_BUSY     procedure aborted by user (as returned by m_ReportProgress)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_FlashOneFileOpenSydeHex(const stw_hex_file::C_HexDataDump & orc_HexDataDump,
                                                   const uint32 ou32_SignatureAddress,
                                                   const uint32 ou32_RequestDownloadTimeout,
                                                   const uint32 ou32_TransferDataTimeout)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_TotalNumberOfBytes = 0U;
   uint32 u32_TotalNumberOfBytesFlashed = 0U;

   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START, C_NO_ERR, 0U, mc_CurrentNode,
                          "Flashing HEX file ...");

   //get total number of bytes for progress calculations:
   for (sint32 s32_Area = 0U; s32_Area < orc_HexDataDump.at_Blocks.GetLength(); s32_Area++)
   {
      u32_TotalNumberOfBytes += static_cast<uint32>(orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength());
   }

   //flash all areas
   for (sint32 s32_Area = 0U; s32_Area < orc_HexDataDump.at_Blocks.GetLength(); s32_Area++)
   {
      const uint32 u32_AreaSize = orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength();
      uint32 u32_MaxBlockLength = 0U;
      uint8 u8_NrCode;
      bool q_Abort;

      //calculate progress percentage:
      // (we just need a rough approximation; so integer calculation will suffice)
      tgl_assert(u32_TotalNumberOfBytes != 0U); //prerequisite for function: non-empty hex file
      //lint -e{414}  //see assertion
      uint8 u8_ProgressPercentage = static_cast<uint8>((u32_TotalNumberOfBytesFlashed * 100U) / u32_TotalNumberOfBytes);
      q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START, C_NO_ERR, u8_ProgressPercentage,
                                 mc_CurrentNode,
                                 "Erasing flash memory for area " + C_SCLString::IntToStr(s32_Area + 1) + "...");
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
            C_SCLString c_Error;
            c_Error.PrintFormatted("Erasing flash memory for area %d failed (Offset: 0x%08X Size: %08X). Details: %s",
                                   s32_Area + 1,
                                   orc_HexDataDump.at_Blocks[s32_Area].u32_AddressOffset,
                                   orc_HexDataDump.at_Blocks[s32_Area].au8_Data.GetLength(),
                                   C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                            u8_NrCode).c_str());
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERRROR, s32_Return,
                                   u8_ProgressPercentage, mc_CurrentNode, c_Error);
            s32_Return = C_COM;
         }
      }
      if (s32_Return == C_NO_ERR)
      {
         //perform the actual transfer
         uint8 u8_BlockSequenceCounter = 1U;
         uint32 u32_RemainingBytes = u32_AreaSize;
         std::vector<uint8> c_Data;

         //set a proper timeout
         (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, ou32_TransferDataTimeout);

         while (u32_RemainingBytes > 0U)
         {
            C_SCLString c_Text;
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
               if (u32_RemainingBytes > (u32_MaxBlockLength - 5U))
               {
                  c_Data.resize(static_cast<size_t>(u32_MaxBlockLength - 5U));
               }
               else
               {
                  c_Data.resize(u32_RemainingBytes);
               }

               (void)memcpy(&c_Data[0],
                            &orc_HexDataDump.at_Blocks[s32_Area].au8_Data[static_cast<sint32>(u32_AreaSize -
                                                                                              u32_RemainingBytes)],
                            c_Data.size());

               s32_Return = this->mpc_ComDriver->SendOsyTransferData(mc_CurrentNode, u8_BlockSequenceCounter, c_Data,
                                                                     &u8_NrCode);
               if (s32_Return == C_NO_ERR)
               {
                  u32_RemainingBytes -= c_Data.size();
                  u8_BlockSequenceCounter = (u8_BlockSequenceCounter < 0xFFU) ? (u8_BlockSequenceCounter + 1U) : 0x00U;
                  u32_TotalNumberOfBytesFlashed += c_Data.size();
                  //lint -e{414}  //see assertion at initial assignment
                  u8_ProgressPercentage = static_cast<uint8>((u32_TotalNumberOfBytesFlashed * 100U) /
                                                             u32_TotalNumberOfBytes);
               }
               else
               {
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR, s32_Return,
                                         u8_ProgressPercentage, mc_CurrentNode, "Could not write data. Details: " +
                                         C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                  u8_NrCode));
                  s32_Return = C_COM;
               }
            }
            if (s32_Return != C_NO_ERR)
            {
               break;
            }
         }
      }

      // Reset the timeout. The services with the specific timeouts are finished.
      (void)this->mpc_ComDriver->OsyResetPollingTimeout(mc_CurrentNode);

      if (s32_Return == C_NO_ERR)
      {
         //area transferred ...
         //report "final" status:
         C_SCLString c_Text;
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
                                   C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
            s32_Return = C_COM;
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

//-----------------------------------------------------------------------------
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

   \param[in]     orc_FilesToFlash              Files to write
   \param[in]     ou32_RequestDownloadTimeout   Maximum time in ms it can take to prepare one file on the target file
   \param[in]     ou32_TransferDataTimeout      Maximum time in ms it can take to write up to 4kB of data to the target
                                                 file

   \return
   C_NO_ERR    flashed all files

xx   C_RD_WR     one of the files is not a valid Intel or Motorola hex file
xx               could not split up hex file in individual areas
xx   C_CONFIG    no signature block found in hex file

   C_COM       communication driver reported problem
xx   C_NOACT     could not extract device name from hex file
xx   C_OVERFLOW  device name of device does not match name contained in hex file
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_FlashNodeOpenSydeFile(const std::vector<C_SCLString> & orc_FilesToFlash,
                                                 const uint32 ou32_RequestDownloadTimeout,
                                                 const uint32 ou32_TransferDataTimeout)
{
   sint32 s32_Return;

   //if connected via Ethernet we might need to reconnect (in case we ran into the session timeout)
   s32_Return = this->m_ReconnectToTargetServer();
   if (s32_Return != C_NO_ERR)
   {
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_RECONNECT_ERROR, s32_Return, 10U, mc_CurrentNode,
                             "Could not reconnect to node");
      s32_Return = C_COM;
   }

   if (s32_Return == C_NO_ERR)
   {
      //start the actual transfers
      //we need to enter the programming session for that:
      (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START, C_NO_ERR, 20U, mc_CurrentNode,
                             "Checking memory availability ...");
      s32_Return = this->mpc_ComDriver->SendOsySetProgrammingMode(mc_CurrentNode);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR, s32_Return, 20U, mc_CurrentNode,
                                "Could not activate programming session.");
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
         for (uint32 u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
         {
            // Save file index
            this->mu32_CurrentFile = u32_File;

            s32_Return = m_FlashOneFileOpenSydeFile(orc_FilesToFlash[u32_File],
                                                    ou32_RequestDownloadTimeout, ou32_TransferDataTimeout);
            if (s32_Return != C_NO_ERR)
            {
               //error report is already in subfunction
               break;
            }
         }
      }
   }

   this->m_DisconnectFromTargetServer();

   return s32_Return;
}

//-----------------------------------------------------------------------------
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

   \return
   C_NO_ERR   file flashed
   C_COM      communication driver reported problem
   C_RD_WR    could not read from input file
   C_BUSY     procedure aborted by user (as returned by m_ReportProgress)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_FlashOneFileOpenSydeFile(const C_SCLString & orc_FileToFlash,
                                                    const uint32 ou32_RequestDownloadTimeout,
                                                    const uint32 ou32_TransferDataTimeout)
{
   sint32 s32_Return = C_NO_ERR;
   sintn sn_Return;
   uint32 u32_TotalNumberOfBytes = 0U;
   uint32 u32_TotalNumberOfBytesFlashed = 0U;
   FILE * pt_File;
   uint32 u32_MaxBlockLength = 0U;
   uint8 u8_NrCode;
   uint32 u32_TransferCrc = 0xFFFFFFFFU;
   uint8 u8_ProgressPercentage = 0U;

   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START, C_NO_ERR, 0U, mc_CurrentNode,
                          "Transferring file ...");
   pt_File = fopen(orc_FileToFlash.c_str(), "rb");
   if (pt_File == NULL)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      //get total number of bytes for progress calculations:
      sn_Return = std::fseek(pt_File, 0, SEEK_END);
      if (sn_Return != 0)
      {
         s32_Return = C_RD_WR;
         (void)fclose(pt_File);
         pt_File = NULL;
      }
      else
      {
         sn_Return = std::ftell(pt_File);
         if (sn_Return < 0)
         {
            s32_Return = C_RD_WR;
            (void)fclose(pt_File);
            pt_File = NULL;
         }
         else
         {
            //sn_Return already checked for negative value (so we can only flash files up to 2GB)
            u32_TotalNumberOfBytes = static_cast<uint32>(sn_Return);
            std::rewind(pt_File); //start over ...
         }
      }
   }

   //flash file
   if (s32_Return == C_NO_ERR)
   {
      const bool q_Abort = m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START, C_NO_ERR, 0U,
                                            mc_CurrentNode,
                                            "Preparing file system for file \"" +
                                            TGL_ExtractFileName(orc_FileToFlash) + "\"...");
      if (q_Abort == true)
      {
         s32_Return = C_BUSY;
      }
      else
      {
         //set a proper timeout
         (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, ou32_RequestDownloadTimeout);

         s32_Return = this->mpc_ComDriver->SendOsyRequestFileTransfer(
            mc_CurrentNode, TGL_ExtractFileName(orc_FileToFlash), u32_TotalNumberOfBytes, u32_MaxBlockLength,
            &u8_NrCode);

         if (s32_Return != C_NO_ERR)
         {
            C_SCLString c_Error;
            c_Error.PrintFormatted("Preparing file system for file \"%s\" failed. Details: %s",
                                   TGL_ExtractFileName(orc_FileToFlash).c_str(),
                                   C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                            u8_NrCode).c_str());
            (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERRROR, s32_Return,
                                   0U, mc_CurrentNode, c_Error);
            s32_Return = C_COM;
         }
      }

      if ((s32_Return != C_NO_ERR) && (pt_File != NULL))
      {
         (void)fclose(pt_File);
         pt_File = NULL;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      //perform the actual transfer
      uint8 u8_BlockSequenceCounter = 1U;
      uint32 u32_RemainingBytes = u32_TotalNumberOfBytes;
      std::vector<uint8> c_Data;

      //set a proper timeout
      (void)this->mpc_ComDriver->OsySetPollingTimeout(mc_CurrentNode, ou32_TransferDataTimeout);

      while (u32_RemainingBytes > 0U)
      {
         C_SCLString c_Text;
         bool q_Abort;
         tgl_assert(u32_TotalNumberOfBytes != 0U); //prerequisite for function: non-empty hex file
         //lint -e{414}  //see assertion
         u8_ProgressPercentage = static_cast<uint8>((u32_TotalNumberOfBytesFlashed * 100U) /
                                                    u32_TotalNumberOfBytes);

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
            if (u32_RemainingBytes > (u32_MaxBlockLength - 5U))
            {
               c_Data.resize(static_cast<size_t>(u32_MaxBlockLength - 5U));
            }
            else
            {
               c_Data.resize(u32_RemainingBytes);
            }

            //lint -e{449,668}  //file cannot be NULL if we get here
            sn_Return = fread(&c_Data[0], 1U, c_Data.size(), pt_File);
            if (sn_Return != static_cast<sintn>(c_Data.size()))
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
                  C_SCLChecksums::CalcCRC32(&c_Data[0], c_Data.size(), u32_TransferCrc);

                  u32_RemainingBytes -= c_Data.size();
                  u8_BlockSequenceCounter =
                     (u8_BlockSequenceCounter < 0xFFU) ? (u8_BlockSequenceCounter + 1U) : 0x00U;
                  u32_TotalNumberOfBytesFlashed += c_Data.size();
               }
               else
               {
                  (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR,
                                         s32_Return, u8_ProgressPercentage, mc_CurrentNode,
                                         "Could not write data. Details: " +
                                         C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
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

      // File is not necessary anymore. Close it.
      if (pt_File != NULL)
      {
         (void)fclose(pt_File);
      }
   }

   // Reset the timeout. The services with the specific timeouts are finished.
   (void)this->mpc_ComDriver->OsyResetPollingTimeout(mc_CurrentNode);

   if (s32_Return == C_NO_ERR)
   {
      //area transferred ...
      //report "final" status:
      C_SCLString c_Text;
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
         (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR, s32_Return,
                                u8_ProgressPercentage, mc_CurrentNode, "Could not finalize the transfer. Details: " +
                                C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      (void) m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED, C_NO_ERR, 100U, mc_CurrentNode,
                              "Flashing file finished.");
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_WriteFingerPrintOsy(void)
{
   //all prerequisites checked; commence the flashing ...
   //write fingerprint
   const C_SCLDateTime c_Now = C_SCLDateTime::Now();
   uint8 au8_Date[3];
   uint8 au8_Time[3];
   C_SCLString c_UserName;
   bool q_Return;
   uint8 u8_NrCode;
   sint32 s32_Return;

   au8_Date[0] = static_cast<uint8>(c_Now.mu16_Year % 1000U);
   au8_Date[1] = static_cast<uint8>(c_Now.mu16_Month);
   au8_Date[2] = static_cast<uint8>(c_Now.mu16_Day);
   au8_Time[0] = static_cast<uint8>(c_Now.mu16_Hour);
   au8_Time[1] = static_cast<uint8>(c_Now.mu16_Minute);
   au8_Time[2] = static_cast<uint8>(c_Now.mu16_Second);

   (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START, C_NO_ERR, 30U, mc_CurrentNode,
                          "Writing fingerprint ...");
   q_Return = stw_tgl::TGL_GetSystemUserName(c_UserName);
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
                             C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
      s32_Return = C_COM;
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
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

   \return
   C_NO_ERR    flashed all files
   C_COM       error flashing (see log for details)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_FlashNodeXfl(const std::vector<C_SCLString> & orc_FilesToFlash)
{
   sint32 s32_Return = C_NO_ERR;

   (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_START, C_NO_ERR, 0U, mc_CurrentNode,
                          "Flashing node ...");

   for (uint32 u32_File = 0U; u32_File < orc_FilesToFlash.size(); u32_File++)
   {
      // Save file index
      this->mu32_CurrentFile = u32_File;

      (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START, C_NO_ERR, 0U, mc_CurrentNode,
                             "Flashing HEX file ...");

      s32_Return = this->mpc_ComDriver->SendStwDoFlash(mc_CurrentNode, orc_FilesToFlash[u32_File]);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR, C_NO_ERR, 100U, mc_CurrentNode,
                                "Flashing HEX file finished.");
         s32_Return = C_COM;
         break;
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

//-----------------------------------------------------------------------------
/*! \brief   Read information of openSYDE server node

   Utility function used to read openSYDE information from server node and report it.
   See documentation of ReadDeviceInformation() for full description.

   \param[in]   ou8_ProgressToReport    progress value to report when invoking virtual reporting function
   \param[in]   ou32_NodeIndex          Index of node within mpc_SystemDefinition

   \return
   C_COM      communication driver reported problem
   C_NO_ERR   information read
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_ReadDeviceInformationOpenSyde(const uint8 ou8_ProgressToReport, const uint32 ou32_NodeIndex)
{
   C_OsyDeviceInformation c_Info;
   uint8 u8_NrCode;
   sint32 s32_Return;

   // If connected via Ethernet we might need to reconnect (in case we ran into the session timeout)
   s32_Return = this->m_ReconnectToTargetServer();

   if (s32_Return != C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_RECONNECT_ERRROR, s32_Return, ou8_ProgressToReport, mc_CurrentNode,
                             "Could not reconnect to node");
      s32_Return = C_COM;
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
                                C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
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
                                C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }
   }

   //get information about all flash blocks
   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START, C_NO_ERR, ou8_ProgressToReport, mc_CurrentNode,
                             "Reading flash block information ...");

      //we need security level 1 for that:
      s32_Return = this->mpc_ComDriver->SendOsySetSecurityLevel(mc_CurrentNode, 1U);
      if (s32_Return != C_NO_ERR)
      {
         (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR, s32_Return,
                                ou8_ProgressToReport, mc_CurrentNode,
                                "Error setting security level for reading flash block information.");
         s32_Return = C_COM;
      }
      else
      {
         //this information is only available for address based devices
         if (this->mpc_SystemDefinition->c_Nodes[ou32_NodeIndex].pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased
             ==
             false)
         {
            s32_Return = this->mpc_ComDriver->SendOsyReadAllFlashBlockData(mc_CurrentNode, c_Info.c_Applications,
                                                                           &u8_NrCode);
            if (s32_Return != C_NO_ERR)
            {
               (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR, s32_Return, ou8_ProgressToReport,
                                      mc_CurrentNode, "Error reading flash block information. Details:" +
                                      C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
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
                                C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return, u8_NrCode));
         s32_Return = C_COM;
      }
   }

   this->m_DisconnectFromTargetServer();

   if (s32_Return == C_NO_ERR)
   {
      //report findings to application:
      this->m_ReportOpenSydeFlashloaderInformationRead(c_Info, ou32_NodeIndex);
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Read information of STW flashloader server node

   Utility function used to read STW flashloader information from server node and report it.
   See documentation of ReadDeviceInformation() for full description.

   \param[in]   ou8_ProgressToReport    progress value to report when invoking virtual reporting function
   \param[in]   ou32_NodeIndex          Index of node within mpc_SystemDefinition

   \return
   C_COM      communication driver reported problem
   C_NO_ERR   information read
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_ReadDeviceInformationStwFlashloader(const uint8 ou8_ProgressToReport,
                                                               const uint32 ou32_NodeIndex)
{
   sint32 s32_Return;
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

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   constructor

   Set up class
*/
//-----------------------------------------------------------------------------
C_OSCSuSequences::C_OSCSuSequences(void) :
   C_OSCComSequencesBase(true),
   mu32_CurrentNode(0U),
   mu32_CurrentFile(0U)
{
}

//-----------------------------------------------------------------------------
/*! \brief   constructor

   Tear down class
*/
//-----------------------------------------------------------------------------
C_OSCSuSequences::~C_OSCSuSequences(void)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Copy referenced files to a temporary folder

   Copy all files to be flashed to a "temp" folder.
   Ensures a clearly defined state during the whole flashing procedure.
   To be called optionally before starting the procedure with "ActivateFlashloader()".

   * Can be called before "Init()" as all required information is passed as parameters.

   * All referenced files must be present (except for file-based targets).
   * Validity is not checked (will be done at UpdateSystem()).
   * Will erase a pre-existing folder completely
   * Will create the target folder (recursively if it needs to be)
   * Will rename the files to prevent possible conflicts.
   ** Used pattern: <device_name>_<application_name>.<extension>

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
               size of files in orc_ApplicationsToWrite[node] is higher than the
                 number of applications of the node in orc_Nodes (for an active and address based node)
   C_RANGE     file referenced by orc_ApplicationsToWrite does not exist
               orc_TargetPath does not end in "\" or "/"
   C_BUSY      could not erase pre-existing target path (note: can result in partially erased target path)
   C_RD_WR     could not copy file
   C_TIMEOUT   could not create target directory
   C_CONFIG    at least one file based node has at least two identical named files
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::h_CreateTemporaryFolder(const std::vector<C_OSCNode> & orc_Nodes,
                                                 const std::vector<uint8> & orc_ActiveNodes,
                                                 const C_SCLString & orc_TargetPath,
                                                 std::vector<C_DoFlash> & orc_ApplicationsToWrite,
                                                 C_SCLString * const opc_ErrorPath)
{
   sint32 s32_Return = C_NO_ERR;

   std::vector<C_DoFlash> c_NodesToFlashNewPaths = orc_ApplicationsToWrite;
   std::vector<C_SCLString> c_NodeTargetPaths;

   if ((orc_TargetPath[orc_TargetPath.Length()] != '\\') &&
       (orc_TargetPath[orc_TargetPath.Length()] != '/'))
   {
      s32_Return = C_RANGE;
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
      for (uint16 u16_Node = 0U; u16_Node < orc_Nodes.size(); u16_Node++)
      {
         tgl_assert(orc_Nodes[u16_Node].pc_DeviceDefinition != NULL);

         if ((orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size() != 0) && (orc_ActiveNodes[u16_Node] == 0U))
         {
            //files defined for inactive node -> cry
            s32_Return = C_NOACT;
         }
         if ((orc_ActiveNodes[u16_Node] == 1U) &&
             (orc_Nodes[u16_Node].pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == false) &&
             (orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size() > orc_Nodes[u16_Node].c_Applications.size()))
         {
            //more files for node than applications
            // Check is not relevant for file based servers
            s32_Return = C_NOACT;
         }

         if (s32_Return == C_NO_ERR)
         {
            //do all files exist ?
            if (orc_ActiveNodes[u16_Node] == 1U)
            {
               for (uint16 u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size();
                    u16_File++)
               {
                  const C_SCLString c_File = orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File];
                  if (TGL_FileExists(c_File) == false)
                  {
                     s32_Return = C_RANGE;
                     break;
                  }
               }
            }
         }

         if (s32_Return == C_NO_ERR)
         {
            // Special case: File based nodes shall have unique file names
            if (orc_Nodes[u16_Node].pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == true)
            {
               uint16 u16_File;

               for (u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size(); ++u16_File)
               {
                  uint16 u16_FileCompare;

                  for (u16_FileCompare = u16_File + 1U;
                       u16_FileCompare < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size();
                       ++u16_FileCompare)
                  {
                     if (TGL_ExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File]) ==
                         TGL_ExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_FileCompare]))
                     {
                        s32_Return = C_CONFIG;
                        break;
                     }
                  }

                  if (s32_Return != C_NO_ERR)
                  {
                     break;
                  }
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
      if (TGL_DirectoryExists(orc_TargetPath) == true)
      {
         s32_Return = TGL_RemoveDirectory(orc_TargetPath, false);
         if (s32_Return != 0)
         {
            s32_Return = C_BUSY;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //create target folder (from bottom-up if required):
      s32_Return = C_OSCUtils::h_CreateFolderRecursively(orc_TargetPath);
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

         for (uint16 u16_Node = 0U; u16_Node < orc_Nodes.size(); u16_Node++)
         {
            if (orc_ActiveNodes[u16_Node] == 1U)
            {
               c_NodeTargetPaths[u16_Node] = TGL_FileIncludeTrailingDelimiter(
                  orc_TargetPath + orc_Nodes[u16_Node].c_Properties.c_Name);

               s32_Return = TGL_CreateDirectory(c_NodeTargetPaths[u16_Node]);
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
      for (uint16 u16_Node = 0U; u16_Node < orc_Nodes.size(); u16_Node++)
      {
         if (orc_ActiveNodes[u16_Node] == 1U)
         {
            for (uint16 u16_File = 0U; u16_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size(); u16_File++)
            {
               //get source file name
               const C_SCLString c_SourceFileName = orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File];
               //compose target file name
               C_SCLString c_TargetFileName;

               if (orc_Nodes[u16_Node].pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == true)
               {
                  // File based nodes need the unchanged file name and must be unique
                  c_TargetFileName = c_NodeTargetPaths[u16_Node] +
                                     TGL_ExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File]);
               }
               else
               {
                  // The original source file name is not relevant for address based nodes and must not be unique
                  // Add an counter to the target file name
                  c_TargetFileName = c_NodeTargetPaths[u16_Node] +
                                     C_SCLString::IntToStr(static_cast<uintn>(u16_File) + 1) + "_" +
                                     TGL_ExtractFileName(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u16_File]);
               }

               //copy file
               std::fstream c_Input(c_SourceFileName.c_str(), std::fstream::in | std::fstream::binary);
               if (c_Input.fail() == true)
               {
                  osc_write_log_error("Copying file", "Could not read \"" + c_SourceFileName + "\".");
                  s32_Return = C_RD_WR;
                  if (opc_ErrorPath != NULL)
                  {
                     *opc_ErrorPath = c_SourceFileName;
                  }
               }
               else
               {
                  c_Input << &std::noskipws;

                  std::istream_iterator<uint8> c_Begin(c_Input);
                  std::istream_iterator<uint8> c_End;

                  std::fstream c_Output(
                     c_TargetFileName.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
                  if (c_Output.fail() == true)
                  {
                     osc_write_log_error("Copying file", "Could not write \"" + c_TargetFileName + "\".");
                     s32_Return = C_RD_WR;
                     if (opc_ErrorPath != NULL)
                     {
                        *opc_ErrorPath = c_TargetFileName;
                     }
                  }
                  else
                  {
                     std::ostream_iterator<uint8> c_Begin2(c_Output);
                     try
                     {
                        std::copy(c_Begin, c_End, c_Begin2);
                     }
                     catch (...)
                     {
                        osc_write_log_error("Copying file", "Could not write stream of \"" + c_TargetFileName + "\".");
                        s32_Return = C_RD_WR;
                        if (opc_ErrorPath != NULL)
                        {
                           *opc_ErrorPath = c_TargetFileName;
                        }
                     }
                  }
               }
               if (s32_Return == C_NO_ERR)
               {
                  c_NodesToFlashNewPaths[u16_Node].c_FilesToFlash[u16_File] = c_TargetFileName;
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

//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void C_OSCSuSequences::h_CheckForChangedApplications(
   const std::vector<C_ApplicationProperties> & orc_ClientSideApplications,
   const std::vector<C_ApplicationProperties> & orc_ServerSideApplications,
   std::vector<stw_types::uint8> & orc_ApplicationsPresentOnServer)
{
   orc_ApplicationsPresentOnServer.resize(orc_ClientSideApplications.size());
   for (uint32 u32_ClientApplIndex = 0U; u32_ClientApplIndex < orc_ClientSideApplications.size(); u32_ClientApplIndex++)
   {
      orc_ApplicationsPresentOnServer[u32_ClientApplIndex] = 0U;
      for (uint32 u32_ServerApplIndex = 0U; u32_ServerApplIndex < orc_ServerSideApplications.size();
           u32_ServerApplIndex++)
      {
         if (orc_ServerSideApplications[u32_ClientApplIndex] == orc_ClientSideApplications[u32_ClientApplIndex])
         {
            orc_ApplicationsPresentOnServer[u32_ClientApplIndex] = 1U;
            break;
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Initialization of the protocol

   Enter Flashloader for all nodes
   Note: STW Flashloader nodes can only be end-points, not routers

   Prerequisite:
   * class members and communication has been set up with Init()

   Rough sequence:
   For local bus:
   * openSYDE nodes:
   ** send "RequestProgramming" as broadcast
   ** send "EcuReset" as broadcast
   ** send "EnterPreProgrammingSession" as broadcast (for a few seconds in short intervals)
   * STW Flashloader nodes:
   ** send all configured reset request messages
   ** if connected via CAN: send "FLASH" (for a few seconds in short intervals;
                            in parallel to the openSYDE "EnterPreProgrammingSession" service
   For confirmation:
   * use simple read service for all nodes that are expected to be present (as "ping")

   Then recurse for all buses that can be reached through the first bus.

   Different for routed target buses:
   * address nodes individually; do not use broadcasts (except for the STW-Flashloader "FLASH" message)

   Result:
   * all nodes on all buses are in flashloader mode

   \param[in]   oq_FailOnFirstError   true: abort all further communication if connecting to one device fails
                                      false: try to continue with other devices in this case

   \return
   C_NO_ERR   flashloaders on all nodes activated
   C_CONFIG   mpc_SystemDefinition is NULL (Init() not called)
   C_COM      communication driver reported problem
   C_WARN     activation for at least one device failed (see log for details)
*/
//-----------------------------------------------------------------------------
stw_types::sint32 C_OSCSuSequences::ActivateFlashloader(const bool oq_FailOnFirstError)
{
   sint32 s32_Return = C_NO_ERR;
   bool q_AtLeastOneError = false;
   const uint32 u32_SCAN_TIME_MS = 5000U;
   const uint32 u32_SCAN_TIME_ROUTING_MS = 1000U;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      uint8 u8_NrCode;

      // Prepare only nodes on the local bus in the first step ...

      //check prerequisites (should have been checked at Init() already)
      tgl_assert(this->mpc_SystemDefinition->c_Nodes.size() == this->mc_ActiveNodes.size());
      tgl_assert(this->mu32_ActiveBusIndex < this->mpc_SystemDefinition->c_Buses.size());

      // send openSYDE CAN-TP broadcast "RequestProgramming"
      (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_START, C_NO_ERR, 0U,
                             "Broadcasting openSYDE Flashloader activation request ...");

      this->mpc_ComDriver->ClearDispatcherQueue();

      // Reset timeout flags
      this->mc_TimeoutNodes.clear();
      this->mc_TimeoutNodes.resize(this->mc_ActiveNodes.size(), 0U);

      if (this->mq_OpenSydeDevicesActive == true)
      {
         bool q_RequestProblem;
         s32_Return = this->mpc_ComDriver->SendOsyBroadcastRequestProgramming(q_RequestProblem);

         if ((s32_Return != C_NO_ERR) || (q_RequestProblem == true))
         {
            (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_ERROR, s32_Return, 0U,
                                   "RequestProgramming request failed.");
            s32_Return = C_COM;
         }
         else
         {
            // send openSYDE broadcast "EcuReset"
            (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_START, C_NO_ERR, 10U,
                                   "Broadcasting openSYDE EcuReset request ...");
            s32_Return = this->mpc_ComDriver->SendOsyBroadcastEcuReset(
               C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);
            if (s32_Return != C_NO_ERR)
            {
               (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_BC_ECU_RESET_ERROR, s32_Return, 10U,
                                      "EcuReset request failed.");
               s32_Return = C_COM;
            }
         }
      }

      if ((this->mq_StwFlashloaderDevicesActiveOnLocalBus == true) &&
          (s32_Return == C_NO_ERR))
      {
         //send reset requests for all STW Flashloader nodes
         (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_ECU_RESET_START, C_NO_ERR, 15U,
                                "Sending STW Flashloader reset requests ...");
         s32_Return = this->mpc_ComDriver->SendStwRequestNodeReset();
         if (s32_Return != C_NO_ERR)
         {
            (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR, s32_Return, 15U,
                                   "Sending request failed.");
         }
      }

      //send "EnterPreProgrammingSession" as broadcast (for a few seconds in short intervals)
      if (s32_Return == C_NO_ERR)
      {
         //but only if we are communicating via CAN bus; keeping the target in the flashloader via the time window
         // is not possible via Ethernet; it really has to react to RequestProgramming
         if (this->mpc_SystemDefinition->c_Buses[this->mu32_ActiveBusIndex].e_Type == C_OSCSystemBus::eCAN)
         {
            uint32 u32_StartTime = stw_tgl::TGL_GetTickCount();
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

               stw_tgl::TGL_Sleep(5);
            }
            while (stw_tgl::TGL_GetTickCount() < (u32_SCAN_TIME_MS + u32_StartTime));
         }
         else
         {
            //Ethernet. Give the targets some time to reset and initialize their Ethernet interfaces ...
            TGL_Sleep(5500U);
         }
      }

      //use simple read service for all nodes that are expected to be present (as "ping")
      if (s32_Return == C_NO_ERR)
      {
         (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START, C_NO_ERR, 30U,
                                "Checking node states ...");

         for (uint16 u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
         {
            C_OSCNodeProperties::E_FlashLoaderProtocol e_ProtocolType;

            const bool q_Return =
               m_IsNodeActive(u16_Node, this->mu32_ActiveBusIndex, e_ProtocolType, mc_CurrentNode);

            if (q_Return == true)
            {
               const bool q_Return2 = m_IsNodeRequired(u16_Node);

               if (q_Return2 == true)
               {
                  (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START, C_NO_ERR, 30U, mc_CurrentNode,
                                         "Checking node state ...");

                  s32_Return = this->mpc_ComDriver->IsRoutingNecessary(u16_Node);

                  // Continue with nodes without routing
                  if (s32_Return == C_NOACT)
                  {
                     s32_Return = C_NO_ERR;

                     if (e_ProtocolType == C_OSCNodeProperties::eFL_OPEN_SYDE)
                     {
                        //if connected via Ethernet we need to reconnect as the reset will break the active TCP
                        // connection
                        s32_Return = this->mpc_ComDriver->ReConnectNode(mc_CurrentNode);

                        if (s32_Return != C_NO_ERR)
                        {
                           (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERRROR, s32_Return, 30U,
                                                  mc_CurrentNode,
                                                  "Could not reconnect to node");

                           // Node is not reachable
                           this->mc_TimeoutNodes[u16_Node] = static_cast<uint8>(s32_Return == C_BUSY);

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
                                                     C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(s32_Return,
                                                                                                              u8_NrCode));

                              // Node is not reachable
                              this->mc_TimeoutNodes[u16_Node] = static_cast<uint8>(s32_Return == C_TIMEOUT);

                              s32_Return = C_COM;
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
            }
         }
      }

      // Activate all nodes with routing
      if (s32_Return == C_NO_ERR)
      {
         for (uint16 u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
         {
            C_OSCNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
            uint32 u32_BusIndex;

            if (this->mpc_ComDriver->GetBusIndexOfRoutingNode(u16_Node, u32_BusIndex) == C_NO_ERR)
            {
               const bool q_Return =
                  m_IsNodeActive(u16_Node, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
               const bool q_Return2 =
                  m_IsNodeRequired(u16_Node);

               if ((q_Return == true) &&
                   (q_Return2 == true))
               {
                  const bool q_IsNodeReachable = this->mq_IsNodeReachable(u16_Node);

                  (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START, C_NO_ERR, 30U, mc_CurrentNode,
                                         "Checking node state ...");

                  if (q_IsNodeReachable == true)
                  {
                     uint32 u32_ErrorIndex = 0U;

                     (void)m_ReportProgress(eACTIVATE_FLASHLOADER_ROUTING_START, C_NO_ERR, 50U, mc_CurrentNode,
                                            "Starting routing for node ...");

                     s32_Return = this->mpc_ComDriver->StartRouting(u16_Node, &u32_ErrorIndex);

                     if (s32_Return == C_NO_ERR)
                     {
                        if (e_ProtocolType == C_OSCNodeProperties::eFL_OPEN_SYDE)
                        {
                           // Set the request programming flag to bring the server into flashloader
                           s32_Return = this->mpc_ComDriver->SendOsyRequestProgramming(mc_CurrentNode);

                           // Reset the server
                           if (s32_Return == C_NO_ERR)
                           {
                              s32_Return = this->mpc_ComDriver->SendOsyEcuReset(mc_CurrentNode,
                                                                                C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_RESET_TO_FLASHLOADER);

                              this->m_DisconnectFromTargetServer();
                           }
                           else
                           {
                              (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_BC_REQUEST_PROGRAMMING_ERROR, s32_Return,
                                                     50U, mc_CurrentNode,
                                                     "RequestProgramming request for routing device failed.");

                              // Node is not reachable
                              this->mc_TimeoutNodes[u16_Node] = static_cast<uint8>(s32_Return == C_TIMEOUT);

                              s32_Return = C_COM;
                           }

                           if (s32_Return == C_NO_ERR)
                           {
                              // TODO: How long ? We might need TesterPresent to keep the routing connection alive
                              //Ethernet. Give the targets some time to reset and initialize their Ethernet interfaces
                              // ...
                              TGL_Sleep(2000U);

                              s32_Return = this->m_ReconnectToTargetServer(true, u16_Node);
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
                                 (void)m_ReportProgress(eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR, s32_Return, 50U,
                                                        mc_CurrentNode,
                                                        "Request to set active session for routing device failed. Details:" +
                                                        C_OSCProtocolDriverOsy::h_GetOpenSydeServiceErrorDetails(
                                                           s32_Return,
                                                           u8_NrCode));

                                 // Node is not reachable
                                 this->mc_TimeoutNodes[u16_Node] = static_cast<uint8>(s32_Return == C_TIMEOUT);

                                 s32_Return = C_COM;
                              }

                              this->m_DisconnectFromTargetServer(false);
                           }
                        }
                        else
                        {
                           //STW Flashloader node
                           s32_Return = this->mpc_ComDriver->SendStwRequestNodeReset(this->mc_CurrentNode);
                           if (s32_Return != C_NO_ERR)
                           {
                              (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR, s32_Return, 15U,
                                                     mc_CurrentNode,
                                                     "Sending request failed.");
                           }

                           if (s32_Return == C_NO_ERR)
                           {
                              uint32 u32_StartTime = stw_tgl::TGL_GetTickCount();
                              do
                              {
                                 s32_Return = this->mpc_ComDriver->SendStwSendFlash(this->mc_CurrentNode);

                                 if (s32_Return != C_NO_ERR)
                                 {
                                    (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR, s32_Return, 20U,
                                                           mc_CurrentNode,
                                                           "Send \"FLASH\" broadcast failed.");
                                    s32_Return = C_COM;
                                    break;
                                 }

                                 stw_tgl::TGL_Sleep(5);
                              }
                              while (stw_tgl::TGL_GetTickCount() < (u32_SCAN_TIME_ROUTING_MS + u32_StartTime));

                              if (s32_Return == C_NO_ERR)
                              {
                                 //it should be in Sleep state now. The only service that we can use is wakeup.
                                 //will not cause any problems as the node will go back to sleep if another node is
                                 // addressed later on
                                 s32_Return = this->mpc_ComDriver->SendStwWakeupLocalId(mc_CurrentNode, NULL);

                                 if (s32_Return != C_NO_ERR)
                                 {
                                    (void)m_ReportProgress(eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR, s32_Return, 50U,
                                                           mc_CurrentNode,
                                                           "Could not perform node wakeup for routing device.");

                                    // Node is not reachable
                                    this->mc_TimeoutNodes[u16_Node] = 1U;
                                 }
                              }
                           }
                        }
                     }
                     else
                     {
                        (void)m_ReportProgress(eACTIVATE_FLASHLOADER_ROUTING_ERROR, s32_Return, 50U,
                                               mc_CurrentNode,
                                               "Starting routing for node failed");
                        osc_write_log_error("Activate Flashloader",
                                            "Activate Flashloader: Start of routing for node (" + C_SCLString::IntToStr(
                                               u16_Node) + ") failed with error code: " +
                                            C_OSCLoggingHandler::h_StwError(s32_Return));

                        // Routing node and target node is not reachable
                        this->mc_TimeoutNodes[u16_Node] = static_cast<uint8>(s32_Return == C_TIMEOUT);
                        this->mc_TimeoutNodes[u32_ErrorIndex] = static_cast<uint8>(s32_Return == C_TIMEOUT);
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
                     osc_write_log_error("Activate Flashloader",
                                         "Activate Flashloader: Start of routing for node (" + C_SCLString::IntToStr(
                                            u16_Node) + ") failed due to not available node on route with error code: " +
                                         C_OSCLoggingHandler::h_StwError(s32_Return));
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

//-----------------------------------------------------------------------------
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
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::ReadDeviceInformation(const bool oq_FailOnFirstError)
{
   sint32 s32_Return = C_NO_ERR;
   bool q_AtLeastOneError = false;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      for (uint16 u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
      {
         C_OSCNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
         //set progress based on node index:
         const uint8 u8_Progress =
            static_cast<uint8>((((static_cast<uintn>(u16_Node)) + 1U) * 100U) /
                               (this->mpc_SystemDefinition->c_Nodes.size() + 1U));
         const bool q_Abort = m_ReportProgress(eREAD_DEVICE_INFO_START, C_NO_ERR, u8_Progress,
                                               "Reading device information from device ...");
         if (q_Abort == true)
         {
            s32_Return = C_BUSY;
         }
         else
         {
            const bool q_IsNodeReachable = this->mq_IsNodeReachable(u16_Node);

            if (q_IsNodeReachable == true)
            {
               uint32 u32_BusIndex;
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
                  bool q_Return;
                  q_Return = m_IsNodeActive(u16_Node, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
                  if (q_Return == true)
                  {
                     q_Return = m_IsNodeRequired(u16_Node);
                     if (q_Return == true)
                     {
                        if (e_ProtocolType == C_OSCNodeProperties::eFL_OPEN_SYDE)
                        {
                           (void)m_ReportProgress(eREAD_DEVICE_INFO_OSY_START, C_NO_ERR, u8_Progress, mc_CurrentNode,
                                                  "Reading openSYDE device information ...");
                           s32_Return = this->m_ReadDeviceInformationOpenSyde(u8_Progress, u16_Node);

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
                           s32_Return = this->m_ReadDeviceInformationStwFlashloader(u8_Progress, u16_Node);

                           if (s32_Return == C_NO_ERR)
                           {
                              (void)m_ReportProgress(eREAD_DEVICE_INFO_XFL_FINISHED, C_NO_ERR, u8_Progress,
                                                     mc_CurrentNode,
                                                     "STW Flashloader device information read.");
                           }
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
                                     "Read device information: Node (" + C_SCLString::IntToStr(u16_Node) +
                                     ") is not reachable due to previous timeout.");
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

   if (s32_Return == C_NO_ERR)
   {
      (void)m_ReportProgress(eREAD_DEVICE_INFO_FINISHED, C_NO_ERR, 100U,
                             "Device information read from all devices.");
   }

   if (q_AtLeastOneError == true)
   {
      s32_Return = C_WARN;
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*! \brief   Read information of server nodes

   Prerequisite:
   All nodes marked as active in mc_ActiveNodes are present and are in flashloader mode
   i.e. the system is in the state after a successful call to ActivateFlashloader()

   Flash all configured files to defined devices.

   Sequence:
   * for all nodes on local bus:
   ** flash all configured applications for all nodes on bus
   * for all buses that can be reached via routing
   ** set up routing
   ** flash all configured applications for all nodes on bus

   Progress report:
   * Reports 0..100 for the overall process
   * Reports 0..100 for each file of each individual node being flashed

   \param[in]  orc_ApplicationsToWrite   list of files to flash per node; must have the same size as the system definition
                                         contains nodes
   \param[in]  orc_NodesOrder            Vector with node update order (index is update position, value is node index)

   \return
   C_NO_ERR    flashed all files
   C_CONFIG    mpc_SystemDefinition is NULL (Init() not called)
   C_OVERFLOW  size of orc_ApplicationsToWrite is not the same as the size of nodes in mpc_SystemDefinition
               for address based targets: device name of device does not match name contained in hex file
   C_NOACT     orc_ApplicationsToWrite has non-empty list of files for node that was not set as active
               orc_ApplicationsToWrite has non-empty list of files for node that has no position in the update order
               orc_ApplicationsToWrite has non-empty list of files for node that has more than one position in the update order
               for address based targets: could not extract device name from hex file
   C_RD_WR     file referenced by orc_ApplicationsToWrite does not exist
               for address based targets: one of the files is not a valid Intel or Motorola hex file
               for address based targets: could not split up hex file in individual areas
   C_CONFIG    for address based targets: no signature block found in hex file
   C_COM       communication driver reported problem
   C_BUSY      procedure aborted by user (as returned by m_ReportProgress)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::UpdateSystem(const std::vector<C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                      const std::vector<uint32> & orc_NodesOrder)
{
   sint32 s32_Return = C_NO_ERR;

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
      uint32 u32_NodesToFlash = 0U;

      //are all nodes supposed to get flashed active ?
      for (uint16 u16_Node = 0U; u16_Node < this->mpc_SystemDefinition->c_Nodes.size(); u16_Node++)
      {
         if (orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size() != 0)
         {
            if (this->mc_ActiveNodes[u16_Node] == false)
            {
               osc_write_log_error("System Update",
                                   "File(s) to flash configured for node " + C_SCLString::IntToStr(
                                      u16_Node) + " which is not marked as active !");
               s32_Return = C_NOACT;
            }
            else
            {
               uint32 u32_PositionCounter;

               ++u32_NodesToFlash;

               // Has the node a position
               s32_Return = C_NOACT;
               for (u32_PositionCounter = 0U; u32_PositionCounter < orc_NodesOrder.size(); ++u32_PositionCounter)
               {
                  if (orc_NodesOrder[u32_PositionCounter] == static_cast<uint32>(u16_Node))
                  {
                     s32_Return = C_NO_ERR;
                     break;
                  }
               }

               if (s32_Return == C_NO_ERR)
               {
                  //do all files exist ?
                  for (uint32 u32_File = 0U; u32_File < orc_ApplicationsToWrite[u16_Node].c_FilesToFlash.size();
                       u32_File++)
                  {
                     if (TGL_FileExists(orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u32_File]) == false)
                     {
                        osc_write_log_error("System Update", "Could not find file \"" +
                                            orc_ApplicationsToWrite[u16_Node].c_FilesToFlash[u32_File] + "\" !");
                        s32_Return = C_RD_WR;
                        break;
                     }
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
            uint32 u32_OrderPos;
            uint32 u32_OrderPosCompare;

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
      for (uint32 u32_Position = 0U; u32_Position < orc_NodesOrder.size(); u32_Position++)
      {
         const uint32 u32_NodeIndex = orc_NodesOrder[u32_Position];

         // Save node index
         this->mu32_CurrentNode = u32_NodeIndex;

         //flash openSYDE nodes
         if (orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash.size() > 0)
         {
            C_OSCNodeProperties::E_FlashLoaderProtocol e_ProtocolType;
            uint32 u32_BusIndex;
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
               // Routing necessary
               s32_Return = this->mpc_ComDriver->StartRouting(u32_NodeIndex);
               q_RoutingActivated = true;
            }
            else
            {
               // Error case
            }

            if (s32_Return == C_NO_ERR)
            {
               const bool q_Return = m_IsNodeActive(u32_NodeIndex, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
               if (q_Return == true)
               {
                  if (e_ProtocolType == C_OSCNodeProperties::eFL_OPEN_SYDE)
                  {
                     const C_OSCDeviceDefinition * const pc_DeviceDefinition =
                        this->mpc_SystemDefinition->c_Nodes[u32_NodeIndex].pc_DeviceDefinition;
                     (void)m_ReportProgress(eUPDATE_SYSTEM_OSY_NODE_START, C_NO_ERR, 10U, mc_CurrentNode,
                                            "Starting device update ...");
                     tgl_assert(pc_DeviceDefinition != NULL);
                     if (pc_DeviceDefinition != NULL)
                     {
                        //address based or file based ?
                        if (pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == false)
                        {
                           s32_Return = m_FlashNodeOpenSydeHex(
                              orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash,
                              pc_DeviceDefinition->u32_FlashloaderOpenSydeRequestDownloadTimeout,
                              pc_DeviceDefinition->u32_FlashloaderOpenSydeTransferDataTimeout);
                        }
                        else
                        {
                           s32_Return = m_FlashNodeOpenSydeFile(
                              orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash,
                              pc_DeviceDefinition->u32_FlashloaderOpenSydeRequestDownloadTimeout,
                              pc_DeviceDefinition->u32_FlashloaderOpenSydeTransferDataTimeout);
                        }
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
                     s32_Return = m_FlashNodeXfl(orc_ApplicationsToWrite[u32_NodeIndex].c_FilesToFlash);
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

//-----------------------------------------------------------------------------
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
   C_COM       communication driver reported problem
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::ResetSystem(void)
{
   sint32 s32_Return = C_NO_ERR;

   if (this->mpc_SystemDefinition == NULL)
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      const uint32 u32_LongestRoute = this->mpc_ComDriver->GetRoutingPointMaximum();
      sint32 s32_RouteSizeCounter;
      std::set<uint32> c_XflResetSentForBus;

      (void)m_ReportProgress(eRESET_SYSTEM_START, C_NO_ERR, 0U, "Resetting System ...");

      // Reset the system beginning with the longest route.
      // Avoiding conflicts with resetting a node, which is necessary for an other route
      for (s32_RouteSizeCounter = static_cast<sint32>(u32_LongestRoute);
           s32_RouteSizeCounter >= 0; --s32_RouteSizeCounter)
      {
         for (uint32 u32_Node = 0U; u32_Node < this->mpc_SystemDefinition->c_Nodes.size(); u32_Node++)
         {
            const bool q_IsNodeReachable = this->mq_IsNodeReachable(u32_Node);

            if (q_IsNodeReachable == true)
            {
               bool q_Return;
               const uint32 u32_RoutePointCount = this->mpc_ComDriver->GetRoutingPointCount(u32_Node, q_Return);

               if ((static_cast<sint32>(u32_RoutePointCount) == s32_RouteSizeCounter) &&
                   (q_Return == true))
               {
                  uint32 u32_BusIndex;
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
                     C_OSCNodeProperties::E_FlashLoaderProtocol e_ProtocolType;

                     q_Return = m_IsNodeActive(u32_Node, u32_BusIndex, e_ProtocolType, mc_CurrentNode);
                     if (q_Return == true)
                     {
                        if (e_ProtocolType == C_OSCNodeProperties::eFL_OPEN_SYDE)
                        {
                           s32_Return = this->m_ReconnectToTargetServer();

                           if (s32_Return == C_NO_ERR)
                           {
                              s32_Return = this->mpc_ComDriver->SendOsyEcuReset(mc_CurrentNode,
                                                                                C_OSCProtocolDriverOsyTpBase::hu8_OSY_RESET_TYPE_KEY_OFF_ON);

                              this->m_DisconnectFromTargetServer();
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
                        (void)m_ReportProgress(eRESET_SYSTEM_OSY_NODE_ERROR, s32_Return, 0U, mc_CurrentNode,
                                               "Could not reset node.");
                        s32_Return = C_COM;
                        break;
                     }
                  }

                  if (q_RoutingActivated == true)
                  {
                     // Reset command does not send a response. Need a little wait time to guarantee that
                     // the routing nodes really send the reset command
                     TGL_Sleep(20);

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

//-----------------------------------------------------------------------------
/*! \brief   Convert information read from openSYDE server node to string list

   Can be used by the application for a no-frills approach to get a textual representation of the information
   reported by m_ReportOpenSydeFlashloaderInformationRead.

   \param[in]     orc_Info         openSYDE server node information
   \param[out]    orc_Text         textual representation of read information
*/
//-----------------------------------------------------------------------------
void C_OSCSuSequences::h_OpenSydeFlashloaderInformationToText(const C_OsyDeviceInformation & orc_Info,
                                                              C_SCLStringList & orc_Text)
{
   C_SCLString c_Line;

   orc_Text.Clear();
   orc_Text.Add("Device name: " + orc_Info.c_DeviceName);
   orc_Text.Add("Number of applications: " + C_SCLString::IntToStr(orc_Info.c_Applications.size()));
   for (uint8 u8_Application = 0U; u8_Application < orc_Info.c_Applications.size(); u8_Application++)
   {
      orc_Text.Add("Application " + C_SCLString::IntToStr(u8_Application));
      orc_Text.Add(" Name: " + orc_Info.c_Applications[u8_Application].c_ApplicationName);
      orc_Text.Add(" Version: " + orc_Info.c_Applications[u8_Application].c_ApplicationVersion);
      orc_Text.Add(" Build date: " + orc_Info.c_Applications[u8_Application].c_BuildDate);
      orc_Text.Add(" Build time: " + orc_Info.c_Applications[u8_Application].c_BuildTime);
      orc_Text.Add(" Block start address: 0x" +
                   C_SCLString::IntToHex(static_cast<sint64>(orc_Info.c_Applications[u8_Application].
                                                             u32_BlockStartAddress), 8U));
      orc_Text.Add(" Block end address: 0x" +
                   C_SCLString::IntToHex(static_cast<sint64>(orc_Info.c_Applications[u8_Application].
                                                             u32_BlockEndAddress), 8U));
      orc_Text.Add(C_SCLString(" Signature valid: ") +
                   ((orc_Info.c_Applications[u8_Application].u8_SignatureValid == 0) ? "yes" : "no"));
      orc_Text.Add(" Additional information: " + orc_Info.c_Applications[u8_Application].c_AdditionalInformation);
   }

   c_Line.PrintFormatted("Flashloader software version: V%d.%02dr%d",
                         orc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[0],
                         orc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[1],
                         orc_Info.c_MoreInformation.au8_FlashloaderSoftwareVersion[2]);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Flashloader protocol version: V%d.%02dr%d",
                         orc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[1],
                         orc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[1],
                         orc_Info.c_MoreInformation.au8_FlashloaderProtocolVersion[2]);
   orc_Text.Add(c_Line);
   c_Line.PrintFormatted("Protocol version: V%d.%02dr%d", orc_Info.c_MoreInformation.au8_ProtocolVersion[0],
                         orc_Info.c_MoreInformation.au8_ProtocolVersion[1],
                         orc_Info.c_MoreInformation.au8_ProtocolVersion[2]);
   orc_Text.Add(c_Line);
   orc_Text.Add("Flash count: " + C_SCLString::IntToStr(orc_Info.c_MoreInformation.u32_FlashCount));
   c_Line.PrintFormatted("Device serial number: %02X.%02X%02X%02X.%02X%02X",
                         orc_Info.c_MoreInformation.au8_EcuSerialNumber[0],
                         orc_Info.c_MoreInformation.au8_EcuSerialNumber[1],
                         orc_Info.c_MoreInformation.au8_EcuSerialNumber[2],
                         orc_Info.c_MoreInformation.au8_EcuSerialNumber[3],
                         orc_Info.c_MoreInformation.au8_EcuSerialNumber[4],
                         orc_Info.c_MoreInformation.au8_EcuSerialNumber[5]);
   orc_Text.Add(c_Line);
   orc_Text.Add("Device article number: " + C_SCLString::IntToStr(orc_Info.c_MoreInformation.u32_EcuArticleNumber));
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
}

//-----------------------------------------------------------------------------
/*! \brief   Convert information read from STW Flashloader server node to string list

   Can be used by the application for a no-frills approach to get a textual representation of the information
   reported by m_ReportStwFlashloaderInformationRead.
   Checksum block or sector information is not added to the text.

   \param[in]     orc_Info         STW Flashloader server node information
   \param[out]    orc_Text         textual representation of read information
*/
//-----------------------------------------------------------------------------
void C_OSCSuSequences::h_StwFlashloaderInformationToText(const C_XflDeviceInformation & orc_Info,
                                                         C_SCLStringList & orc_Text)
{
   C_SCLString c_Line;

   orc_Text.Clear();

   if (orc_Info.c_BasicInformation.q_DeviceIDValid == true)
   {
      orc_Text.Add("Device name: " + orc_Info.c_BasicInformation.c_DeviceID);
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
                   C_SCLString::IntToStr(orc_Info.c_BasicInformation.c_DeviceInfoAddresses.GetLength()));
      for (uint8 u8_Application = 0U; u8_Application < orc_Info.c_BasicInformation.c_DeviceInfoAddresses.GetLength();
           u8_Application++)
      {
         orc_Text.Add("Application " + C_SCLString::IntToStr(u8_Application));
         orc_Text.Add(" Device info address: 0x" +
                      C_SCLString::IntToHex(static_cast<sint64>(orc_Info.c_BasicInformation.c_DeviceInfoAddresses[
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
                            static_cast<uint8>((orc_Info.c_BasicInformation.u16_ProtocolVersion >> 12) & 0x0FU),
                            static_cast<uint8>((orc_Info.c_BasicInformation.u16_ProtocolVersion >> 8) & 0x0FU),
                            static_cast<uint8>((orc_Info.c_BasicInformation.u16_ProtocolVersion >> 4) & 0x0FU),
                            static_cast<uint8>((orc_Info.c_BasicInformation.u16_ProtocolVersion) & 0x0FU));
   }
   else
   {
      c_Line = "unknown (< V3.00r0)";
   }
   orc_Text.Add("Protocol version: " + c_Line);

   if (orc_Info.c_BasicInformation.q_EraseCountValid == true)
   {
      c_Line = C_SCLString::IntToStr(orc_Info.c_BasicInformation.u32_EraseCount);
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
                                  static_cast<uintn>(orc_Info.c_BasicInformation.c_FingerPrintData.u32_Checksum));
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
      c_Line = C_SCLString::IntToStr(orc_Info.c_BasicInformation.u16_SectorCount);
   }
   else
   {
      c_Line = "unknown";
   }
   orc_Text.Add(" Number of flash sectors: " + c_Line);

   if (orc_Info.c_BasicInformation.q_FlashloaderVersionValid == true)
   {
      sint32 s32_Index;
      C_XFLFlashSectors c_SectorTable;
      uint32 u32_TotalSize = 0U;
      uint32 u32_TotalSizeWriteable = 0U;
      uint32 u32_SectorSize;

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

      orc_Text.Add("  Number of flash ICs: " + C_SCLString::IntToStr(rc_FlashInfo.c_ICs.GetLength()));

      for (s32_Index = 0; s32_Index < rc_FlashInfo.c_ICs.GetLength(); s32_Index++)
      {
         c_Line.PrintFormatted(
            "  IC %03d: size: 0x%08X; sec0: 0x%08X; regions: 0x%02X; erasetime: %ums; progtime: %ums",
            static_cast<sintn>(s32_Index),
            static_cast<uintn>(rc_FlashInfo.c_ICs[s32_Index].u32_TotalSize),
            static_cast<uintn>(rc_FlashInfo.c_ICs[s32_Index].u32_Sector0Offset),
            static_cast<uintn>(rc_FlashInfo.c_ICs[s32_Index].c_Regions.GetLength()),
            static_cast<uintn>(rc_FlashInfo.c_ICs[s32_Index].u32_SectorEraseTime),
            static_cast<uintn>(rc_FlashInfo.c_ICs[s32_Index].u32_ProgrammingTime));
         orc_Text.Add(c_Line);
         for (sint32 s32_RegionIndex = 0; s32_RegionIndex < rc_FlashInfo.c_ICs[s32_Index].c_Regions.GetLength();
              s32_RegionIndex++)
         {
            c_Line.PrintFormatted("  IC %03d, region 0x%02X: blocksize: 0x%08X; numblocks: 0x%04X",
                                  static_cast<sintn>(s32_Index),
                                  static_cast<uintn>(s32_RegionIndex),
                                  static_cast<uintn>(rc_FlashInfo.c_ICs[s32_Index].c_Regions[s32_RegionIndex].
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
         //was there a gap before this sector:
         if (s32_Index > 0)
         {
            if (c_SectorTable[s32_Index].u32_LowestAddress > (c_SectorTable[s32_Index - 1].u32_HighestAddress + 1))
            {
               //gap of at least one byte !
               c_Line.PrintFormatted("  Gap: Start: 0x%08X; Size: 0x%08X",
                                     static_cast<uintn>(c_SectorTable[s32_Index - 1].u32_HighestAddress + 1),
                                     static_cast<uintn>(c_SectorTable[s32_Index].u32_LowestAddress -
                                                        (c_SectorTable[s32_Index - 1].u32_HighestAddress + 1)));
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
                               static_cast<sintn>(s32_Index),
                               static_cast<uintn>(c_SectorTable[s32_Index].u32_LowestAddress),
                               static_cast<uintn>(u32_SectorSize),
                               c_SectorTable[s32_Index].u8_ICIndex,
                               (c_SectorTable[s32_Index].q_IsProtected == true) ? "no" : "yes");
         orc_Text.Add(c_Line);
      }
      orc_Text.Add("\n");
      c_Line.PrintFormatted("  Total size: 0x%08X", static_cast<uintn>(u32_TotalSize));
      orc_Text.Add(c_Line);
      c_Line.PrintFormatted("  Total non-protected size: 0x%08X", static_cast<uintn>(u32_TotalSizeWriteable));
      orc_Text.Add(c_Line);

      //Aliased memory
      if (rc_FlashInfo.c_Aliases.GetLength() != 0)
      {
         orc_Text.Add("\n");
         orc_Text.Add("  Number of aliased memory regions: " +
                      C_SCLString::IntToStr(rc_FlashInfo.c_Aliases.GetLength()));
         for (s32_Index = 0; s32_Index < rc_FlashInfo.c_Aliases.GetLength(); s32_Index++)
         {
            c_Line.PrintFormatted("  Aliased region %03d: Physical: 0x%08X; Aliased: 0x%08X; Size: 0x%08X",
                                  static_cast<sintn>(s32_Index),
                                  static_cast<uintn>(rc_FlashInfo.c_Aliases[s32_Index].u32_PhysicalAddress),
                                  static_cast<uintn>(rc_FlashInfo.c_Aliases[s32_Index].u32_AliasedAddress),
                                  static_cast<uintn>(rc_FlashInfo.c_Aliases[s32_Index].u32_Size));
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
                   C_SCLString::IntToStr(orc_Info.c_BasicInformation.u8_ImplementationInformationMaxHexRecordLength));
      orc_Text.Add(" HEX file: Granularity: " +
                   C_SCLString::IntToStr(orc_Info.c_BasicInformation.u8_ImplementationInformationHexRecordGranularity));
   }
   else
   {
      orc_Text.Add(" HEX file: Maximum record length: unknown");
      orc_Text.Add(" HEX file: Granularity: unknown");
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Compare two instances

   \param[in]   orc_Source   instance to compare *this against

   \return
   true:   orc_Source == (*this)
   false:  orc_Source != (*this)
*/
//-----------------------------------------------------------------------------
bool C_OSCSuSequences::C_ApplicationProperties::operator ==(const C_ApplicationProperties & orc_Source) const
{
   return((this->c_Name == orc_Source.c_Name) &&
          (this->c_Version == orc_Source.c_Version) &&
          (this->c_BuildDate == orc_Source.c_BuildDate) &&
          (this->c_BuildTime == orc_Source.c_BuildTime));
}

//-----------------------------------------------------------------------------
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
   C_COM      communication driver reported error
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_ReconnectToTargetServer(const bool oq_RestartRouting, const uint32 ou32_NodeIndex)
{
   uint32 u32_BusCounter;
   sint32 s32_Return = C_NO_ERR;

   for (u32_BusCounter = 0U; u32_BusCounter < this->mpc_SystemDefinition->c_Buses.size(); ++u32_BusCounter)
   {
      if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].u8_BusID == this->mc_CurrentNode.u8_BusIdentifier)
      {
         if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].e_Type == C_OSCSystemBus::eETHERNET)
         {
            if ((oq_RestartRouting == true) &&
                (this->mc_CurrentNode.u8_BusIdentifier != this->mpc_ComDriver->GetClientId().u8_BusIdentifier))
            {
               // Restart routing when the routing is still necessary. Only a problem when using
               // Ethernet to Ethernet routing a target is connected to Ethernet and not CAN.
               // The routing connection on the last router server will be dropped by reseting the target server.
               s32_Return = this->mpc_ComDriver->StartRouting(ou32_NodeIndex);
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

//-----------------------------------------------------------------------------
/*! \brief   Disconnects the current server

   \param[in]  oq_DisconnectOnIp2IpRouting    Flag if a disconnect in case of Ethernet to Ethernet routing shall be
                                              executed. If the routing is still necessary after this call, the
                                              disconnect shall not be executed.

   \return
   C_NO_ERR   disconnected or no reconnect necessary
   C_NOACT    could not disconnect to node
   C_RANGE    node not found or no openSYDE protocol installed
*/
//-----------------------------------------------------------------------------
sint32 C_OSCSuSequences::m_DisconnectFromTargetServer(const bool oq_DisconnectOnIp2IpRouting)
{
   uint32 u32_BusCounter;
   sint32 s32_Return = C_NO_ERR;

   // Disconnect the current server
   for (u32_BusCounter = 0U; u32_BusCounter < this->mpc_SystemDefinition->c_Buses.size(); ++u32_BusCounter)
   {
      if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].u8_BusID == this->mc_CurrentNode.u8_BusIdentifier)
      {
         if (this->mpc_SystemDefinition->c_Buses[u32_BusCounter].e_Type == C_OSCSystemBus::eETHERNET)
         {
            if (this->mc_CurrentNode.u8_BusIdentifier == this->mpc_ComDriver->GetClientId().u8_BusIdentifier)
            {
               s32_Return = this->mpc_ComDriver->DisconnectNode(mc_CurrentNode);
            }
            else if (oq_DisconnectOnIp2IpRouting == true)
            {
               uint32 u32_NodeIndex;

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
