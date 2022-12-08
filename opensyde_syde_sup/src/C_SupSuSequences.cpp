//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SYDEsup integration class for system update sequences (implementation)

   This class is made for reimplementing the report methods for better console output.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SupSuSequences.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SydeSup.hpp"
#include "stwerrors.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SupSuSequences::C_SupSuSequences(void)
{
   mq_Quiet = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set quiet parameter.

   quiet == true: print to console
   quiet == false: do not print to console

   Log file should always contain everything.

   \param[in]     orq_Quiet      true: quiet, false: not quiet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupSuSequences::SetQuiet(const bool & orq_Quiet)
{
   this->mq_Quiet = orq_Quiet;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getter to return information about active openSYDE devices

   Important: Call ClearActiveOsyDeviceInformation(...) and ReadDeviceInformation(...) first.

   \param[out]  orc_Indexes   absolute index of elements of returned vector in list of all devices

   \return
   list of active openSYDE devices (server side)
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<stw::opensyde_core::C_OscSuSequences::C_OsyDeviceInformation> & C_SupSuSequences::
GetActiveOsyDeviceInformation(std::vector<uint16_t> & orc_Indexes) const
{
   orc_Indexes = mc_OsyDeviceInformationIndexes;
   return this->mc_ActiveOsyDeviceInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getter to return information about active STW Flashloader devices

   Important: Call ClearActiveOsyDeviceInformation(...) and ReadDeviceInformation(...) first.

   \param[out]  orc_Indexes   absolute index of elements of returned vector in list of all devices

   \return
   list of active STW Flashloader devices (server side)
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<stw::opensyde_core::C_OscSuSequences::C_XflDeviceInformation> & C_SupSuSequences::
GetActiveXflDeviceInformation(std::vector<uint16_t> & orc_Indexes) const
{
   orc_Indexes = mc_XflDeviceInformationIndexes;
   return this->mc_ActiveXflDeviceInformation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Empties vectors of active devices

  Important: Call before ReadDeviceInformation(...)

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupSuSequences::ClearActiveDeviceInformation()
{
   this->mc_OsyDeviceInformationIndexes.clear();
   this->mc_XflDeviceInformationIndexes.clear();
   this->mc_ActiveOsyDeviceInformation.clear();
   this->mc_ActiveXflDeviceInformation.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports some information about the current sequence

   Override method.
   Here: Give information to logging engine.

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
bool C_SupSuSequences::m_ReportProgress(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step,
                                        const int32_t os32_Result, const uint8_t ou8_Progress,
                                        const stw::scl::C_SclString & orc_Information)
{
   (void) ou8_Progress; // progress numbers not interesting for console application
   const bool q_Error = m_CheckErrorCase(oe_Step);
   C_SclString c_Text = "";

   c_Text += m_GetStepName(oe_Step) + ":  ";

   // show result only in error case
   if (q_Error == true)
   {
      c_Text += "Result: " + C_SclString::IntToStr(os32_Result);
   }

   c_Text += " " + orc_Information;

   // write to log file and console
   this->m_WriteLog(c_Text, q_Error);

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports some information about the current sequence for a specific server

   Override method.
   Here: Give information to logging engine.

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
bool C_SupSuSequences::m_ReportProgress(const stw::opensyde_core::C_OscSuSequences::E_ProgressStep oe_Step,
                                        const int32_t os32_Result, const uint8_t ou8_Progress,
                                        const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_Server,
                                        const stw::scl::C_SclString & orc_Information)
{
   (void) ou8_Progress; // progress numbers not interesting for console application
   bool q_PrintLine = true;

   // suppress often occurring lines
   if ((oe_Step == C_OscSuSequences::eXFL_PROGRESS) && (os32_Result == static_cast<int32_t>(C_NO_ERR)))
   {
      if ((orc_Information == "Information: Sending FLASH request ...") ||
          (orc_Information == "Information: <<<CLRALL") ||
          (orc_Information == "Information: <<<CLRLINE") ||
          (orc_Information == "Information: Writing to Flash ..."))
      {
         q_PrintLine = false;
      }
   }

   if (q_PrintLine == true)
   {
      const bool q_Error = m_CheckErrorCase(oe_Step);
      C_SclString c_Text = "";

      c_Text += m_GetStepName(oe_Step) + ":  ";

      // show result only in error case
      if (q_Error == true)
      {
         c_Text += "Result: " + C_SclString::IntToStr(os32_Result);
      }

      c_Text += " Bus Id: " + C_SclString::IntToStr(orc_Server.u8_BusIdentifier);
      c_Text += " Node Id: " + C_SclString::IntToStr(orc_Server.u8_NodeIdentifier);
      c_Text += " " + orc_Information;

      // write to log file and console
      this->m_WriteLog(c_Text, q_Error);
   }

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports information read from openSYDE server node

   Override method.
   Here: Write to log and remember information.

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.

   \param[in]     orc_Info         Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupSuSequences::m_ReportOpenSydeFlashloaderInformationRead(
   const stw::opensyde_core::C_OscSuSequences::C_OsyDeviceInformation & orc_Info, const uint32_t ou32_NodeIndex)
{
   C_SclStringList c_Text;
   C_SclString c_Message = "openSYDE device information found for node with index " +
                           C_SclString::IntToStr(ou32_NodeIndex) + "\n";

   h_OpenSydeFlashloaderInformationToText(orc_Info, c_Text);
   for (uint32_t u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      c_Message += c_Text.Strings[u32_Line] + "\n";
   }

   this->m_WriteLog(c_Message);

   // store current device information of openSYDE node
   this->mc_OsyDeviceInformationIndexes.push_back(ou32_NodeIndex);
   this->mc_ActiveOsyDeviceInformation.push_back(orc_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reports information read from STW flashloader server node

   Override method.
   Here: Write to log and remember information.

   Called by ReadDeviceInformation() after it has read information from an STW flashloader node.

   \param[in]     orc_Info         Information read from node
   \param[in]     ou32_NodeIndex   Index of node within mpc_SystemDefinition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupSuSequences::m_ReportStwFlashloaderInformationRead(
   const stw::opensyde_core::C_OscSuSequences::C_XflDeviceInformation & orc_Info, const uint32_t ou32_NodeIndex)
{
   C_SclStringList c_Text;
   C_SclString c_Message = "STW Flashloader device information found for node with index " +
                           C_SclString::IntToStr(ou32_NodeIndex) + "\n";

   h_StwFlashloaderInformationToText(orc_Info, c_Text);
   for (uint32_t u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      c_Message += c_Text.Strings[u32_Line] + "\n";
   }

   this->m_WriteLog(c_Message);

   // store current device information of openSYDE node
   this->mc_XflDeviceInformationIndexes.push_back(ou32_NodeIndex);
   this->mc_ActiveXflDeviceInformation.push_back(orc_Info);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if step belongs to an error case.

   Check step to differentiate between error and information.

   \param[in]     oe_Step         Progress step
   \param[in]     orc_Text        Information text

   \return
   true: is error
   false: is no error (i.e. information)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SupSuSequences::m_CheckErrorCase(const C_OscSuSequences::E_ProgressStep oe_Step) const
{
   bool q_Return = false;

   // Decide if error or info
   switch (oe_Step)
   {
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_ROUTING_ERROR:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_RECONNECT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR:
   case C_OscSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
   case C_OscSuSequences::eRESET_SYSTEM_OSY_NODE_ERROR:
   case C_OscSuSequences::eRESET_SYSTEM_OSY_ROUTED_NODE_ERROR:
      q_Return = true;
      break;
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_START:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_WARNING:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_ECU_RESET_WARNING:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_XFL_ECU_RESET_WARNING:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_RECONNECT_WARNING:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_ROUTING_START:
   case C_OscSuSequences::eACTIVATE_FLASHLOADER_FINISHED:
   case C_OscSuSequences::eREAD_DEVICE_INFO_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_SET_SESSION_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_OSY_FINISHED:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START:
   case C_OscSuSequences::eREAD_DEVICE_INFO_XFL_FINISHED:
   case C_OscSuSequences::eREAD_DEVICE_INFO_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_RESULT_STRING:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_OSY_NODE_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_ABORTED:
   case C_OscSuSequences::eUPDATE_SYSTEM_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_XFL_NODE_START:
   case C_OscSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START:
   case C_OscSuSequences::eXFL_PROGRESS:
   case C_OscSuSequences::eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED:
   case C_OscSuSequences::eUPDATE_SYSTEM_XFL_NODE_FINISHED:
   case C_OscSuSequences::eRESET_SYSTEM_START:
   case C_OscSuSequences::eRESET_SYSTEM_FINISHED:
   default:
      break;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the name of the current step

   \param[in]     oe_Step         Step of node configuration

   \return
   Name of the specific step
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SupSuSequences::m_GetStepName(const E_ProgressStep oe_Step) const
{
   C_SclString c_Text;

   switch (oe_Step)
   {
   case eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_START:
      c_Text = "Activate Flashloader - Request programming start";
      break;
   case eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_WARNING:
      c_Text = "Activate Flashloader - Request programming warning";
      break;
   case eACTIVATE_FLASHLOADER_OSY_REQUEST_PROGRAMMING_ERROR:
      c_Text = "Activate Flashloader - Request programming error";
      break;
   case eACTIVATE_FLASHLOADER_OSY_ECU_RESET_WARNING:
      c_Text = "Activate Flashloader - ECU reset warning";
      break;
   case eACTIVATE_FLASHLOADER_OSY_ECU_RESET_ERROR:
      c_Text = "Activate Flashloader - ECU reset error";
      break;
   case eACTIVATE_FLASHLOADER_XFL_ECU_RESET_WARNING:
      c_Text = "Activate Flashloader - No ECU reset message configured";
      break;
   case eACTIVATE_FLASHLOADER_XFL_ECU_RESET_ERROR:
      c_Text = "Activate Flashloader - Sending ECU reset requests error";
      break;
   case eACTIVATE_FLASHLOADER_OSY_XFL_BC_ENTER_FLASHLOADER_START:
      c_Text = "Activate Flashloader - Broadcast enter Flashloader start";
      break;
   case eACTIVATE_FLASHLOADER_OSY_BC_ENTER_PRE_PROGRAMMING_ERROR:
      c_Text = "Activate Flashloader - Broadcast enter pre programming error";
      break;
   case eACTIVATE_FLASHLOADER_XFL_BC_FLASH_ERROR:
      c_Text = "Activate Flashloader - Broadcast \"FLASH\" error";
      break;
   case eACTIVATE_FLASHLOADER_OSY_XFL_BC_PING_START:
      c_Text = "Activate Flashloader - Ping devices start";
      break;
   case eACTIVATE_FLASHLOADER_OSY_RECONNECT_WARNING:
      c_Text = "Activate Flashloader - Reconnect warning";
      break;
   case eACTIVATE_FLASHLOADER_OSY_RECONNECT_ERROR:
      c_Text = "Activate Flashloader - Reconnect error";
      break;
   case eACTIVATE_FLASHLOADER_OSY_SET_SESSION_ERROR:
      c_Text = "Activate Flashloader - Set session error";
      break;
   case eACTIVATE_FLASHLOADER_XFL_WAKEUP_ERROR:
      c_Text = "Activate Flashloader - Perform wakeup error";
      break;
   case eACTIVATE_FLASHLOADER_ROUTING_START:
      c_Text = "Activate Flashloader - Start routing";
      break;
   case eACTIVATE_FLASHLOADER_ROUTING_ERROR:
      c_Text = "Activate Flashloader - Error on starting routing";
      break;
   case eACTIVATE_FLASHLOADER_ROUTING_AVAILABLE_FEATURE_ERROR:
      c_Text = "Activate Flashloader - Problem with available features of flashloader";
      break;
   case eACTIVATE_FLASHLOADER_FINISHED:
      c_Text = "Activate Flashloader - Finished";
      break;
   case eREAD_DEVICE_INFO_START:
      c_Text = "Read Device Information - Start";
      break;
   case eREAD_DEVICE_INFO_OSY_RECONNECT_ERROR:
      c_Text = "Read Device Information - Error on reconnecting";
      break;
   case eREAD_DEVICE_INFO_OSY_SET_SESSION_START:
      c_Text = "Read Device Information - Set session start";
      break;
   case eREAD_DEVICE_INFO_OSY_SET_SESSION_ERROR:
      c_Text = "Read Device Information - Set session error";
      break;
   case eREAD_DEVICE_INFO_OSY_DEVICE_NAME_START:
      c_Text = "Read Device Information - Device name start";
      break;
   case eREAD_DEVICE_INFO_OSY_DEVICE_NAME_ERROR:
      c_Text = "Read Device Information - Device name error";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_START:
      c_Text = "Read Device Information - Flash blocks start";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_SECURITY_ERROR:
      c_Text = "Read Device Information - Flash blocks security error";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASH_BLOCKS_ERROR:
      c_Text = "Read Device Information - Flash blocks error";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_START:
      c_Text = "Read Device Information - Flashloader information start";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_INFO_ERROR:
      c_Text = "Read Device Information - Flashloader information error";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_START:
      c_Text = "Read Device Information - Flashloader security activation state start";
      break;
   case eREAD_DEVICE_INFO_OSY_FLASHLOADER_CHECK_SECURITY_ACTIVATION_ERROR:
      c_Text = "Read Device Information - Flashloader security activation state error";
      break;
   case eREAD_DEVICE_INFO_FINISHED:
      c_Text = "Read Device Information - Finished";
      break;
   case eREAD_DEVICE_INFO_OSY_START:
      c_Text = "Read openSYDE Device Information - Started";
      break;
   case eREAD_DEVICE_INFO_OSY_FINISHED:
      c_Text = "Read openSYDE Device Information - Finished";
      break;
   case eREAD_DEVICE_INFO_XFL_START:
      c_Text = "Read STW Flashloader Device Information - Started";
      break;
   case eREAD_DEVICE_INFO_XFL_FINISHED:
      c_Text = "Read STW Flashloader Device Information - Finished";
      break;
   case eREAD_DEVICE_INFO_XFL_WAKEUP_ERROR:
      c_Text = "Read Device Information - Connection with STW Flashloader device failed";
      break;
   case eREAD_DEVICE_INFO_XFL_READING_INFORMATION_START:
      c_Text = "Read Device Information - Start";
      break;
   case eREAD_DEVICE_INFO_XFL_READING_INFORMATION_ERROR:
      c_Text = "Read Device Information - Could not read information from STW Flashloader device";
      break;
   case eUPDATE_SYSTEM_START:
      c_Text = "Update System - Start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_START:
      c_Text = "Update System - Node start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_READ_FEATURE_ERROR:
      c_Text = "Update System - Problem with getting nodes available features";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINISHED:
      c_Text = "Update System - Node finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_START:
      c_Text = "Update System - Node open HEX file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_HEX_OPEN_ERROR:
      c_Text = "Update System - Node open HEX file error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_HEX_SIGNATURE_ERROR:
      c_Text = "Update System - Node HEX file signature error";
      break;
   case eUPDATE_SYSTEM_OSY_RECONNECT_ERROR:
      c_Text = "Update System - Error on reconnecting";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_START:
      c_Text = "Update System - Node check device name start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_COMM_ERROR:
      c_Text = "Update System - Node check device name communication error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_FILE_ERROR:
      c_Text = "Update System - Node check device name file error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_DEVICE_NAME_MATCH_ERROR:
      c_Text = "Update System - Node check device name match error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_START:
      c_Text = "Update System - Node check memory start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_SESSION_ERROR:
      c_Text = "Update System - Node check memory session error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_FILE_ERROR:
      c_Text = "Update System - Node check memory file error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_CHECK_MEMORY_NOT_OK:
      c_Text = "Update System - Node check memory not ok";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_START:
      c_Text = "Update System - Node fingerprint start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_NAME_NOT_READABLE:
      c_Text = "Update System - Node fingerprint name is not readable";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FINGERPRINT_ERROR:
      c_Text = "Update System - Node fingerprint error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_START:
      c_Text = "Update System - Node flash of HEX file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_START:
      c_Text = "Update System - Node flash area of HEX file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_ERASE_ERROR:
      c_Text = "Update System - Node flash area of HEX file erase error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_START:
      c_Text = "Update System - Node flash area of HEX file transfer start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_TRANSFER_ERROR:
      c_Text = "Update System - Node flash area of HEX file transfer error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_START:
      c_Text = "Update System - Node flash area of HEX file exit start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_FINAL_START:
      c_Text = "Update System - Node flash area of HEX file exit final start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_AREA_EXIT_ERROR:
      c_Text = "Update System - Node flash area of HEX file exit error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_HEX_FINISHED:
      c_Text = "Update System - Node flash of HEX file finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_START:
      c_Text = "Update System - Node flash of file system file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_START:
      c_Text = "Update System - Preparation of Node flash area of file system file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_PREPARE_ERROR:
      c_Text = "Update System - Preparation of Node flash area of file system file erase error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_START:
      c_Text = "Update System - Node flash area of file system file transfer start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_TRANSFER_ERROR:
      c_Text = "Update System - Node flash area of file system file transfer error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_START:
      c_Text = "Update System - Node flash area of file system file exit start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_EXIT_ERROR:
      c_Text = "Update System - Node flash area of file system file exit error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_RESULT_STRING:
      c_Text = "Update System - Node flash area of file result text";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_FLASH_FILE_FINISHED:
      c_Text = "Update System - Node flash of file system file finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_START:
      c_Text = "Update System - Node NVM write start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = "Update System - Node NVM write available feature error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_SESSION_ERROR:
      c_Text = "Update System - Node NVM write session error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_MAX_SIZE_ERROR:
      c_Text = "Update System - Node NVM write read back of maximum block length error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_START:
      c_Text = "Update System - Node NVM write read back of parameter set image file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_OPEN_FILE_ERROR:
      c_Text = "Update System - Node NVM write read back of parameter set image file error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_START:
      c_Text = "Update System - Node NVM write write of parameter set image file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_WRITE_FILE_ERROR:
      c_Text = "Update System - Node NVM write write of parameter set image file error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FILE_FINISHED:
      c_Text = "Update System - Node NVM write of parameter set image file finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_NVM_WRITE_FINISHED:
      c_Text = "Update System - Node NVM write of parameter set image files finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_START:
      c_Text = "Update System: Node write of PEM file start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = "Update System: Node write of PEM file available feature error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SESSION_ERROR:
      c_Text = "Update System: Node write of PEM file session error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_OPEN_FILE_ERROR:
      c_Text = "Update System: Node write of PEM file open file error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_EXTRACT_KEY_ERROR:
      c_Text = "Update System: Node write of PEM file extract key error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_SEND_ERROR:
      c_Text = "Update System: Node write of PEM file send error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_PEM_FILE_WRITE_FINISHED:
      c_Text = "Update System: Node write of PEM file finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_START:
      c_Text = "Update System: Node write of security activation state start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = "Update System: Node write of security activation state available feature error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SESSION_ERROR:
      c_Text = "Update System: Node write of security activation state session error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_SEND_ERROR:
      c_Text = "Update System: Node write of security activation state send error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_SECURITY_WRITE_FINISHED:
      c_Text = "Update System: Node write of security activation state finished";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_START:
      c_Text = "Update System: Node write of debugger activation state start";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_AVAILABLE_FEATURE_ERROR:
      c_Text = "Update System: Node write of debugger activation state available feature error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SESSION_ERROR:
      c_Text = "Update System: Node write of debugger activation state session error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_SEND_ERROR:
      c_Text = "Update System: Node write of debugger activation state send error";
      break;
   case eUPDATE_SYSTEM_OSY_NODE_STATE_DEBUGGER_WRITE_FINISHED:
      c_Text = "Update System: Node write of debugger activation state finished";
      break;
   case eUPDATE_SYSTEM_ABORTED:
      c_Text = "Update System - Aborted";
      break;
   case eUPDATE_SYSTEM_FINISHED:
      c_Text = "Update System - Finished";
      break;
   case eUPDATE_SYSTEM_XFL_NODE_START:
      c_Text = "Update System - Node flash start";
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FINISHED:
      c_Text = "Update System - Node flash finished";
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_START:
      c_Text = "Update System - Node flash of HEX file start";
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_ERROR:
      c_Text = "Update System - Node flash of HEX file error";
      break;
   case eXFL_PROGRESS: //wrapped progress information from STW flashloader driver
      c_Text = "Update System - STW Flashloader status";
      break;
   case eUPDATE_SYSTEM_XFL_NODE_FLASH_HEX_FINISHED:
      c_Text = "Update System - Node flash of HEX file finished";
      break;
   case eRESET_SYSTEM_START:
      c_Text = "Reset System - Start";
      break;
   case eRESET_SYSTEM_OSY_NODE_ERROR:
      c_Text = "Reset System - Node error";
      break;
   case eRESET_SYSTEM_OSY_ROUTED_NODE_ERROR:
      c_Text = "Reset System - Routed node error";
      break;
   case eRESET_SYSTEM_FINISHED:
      c_Text = "Reset System - Finished";
      break;
   default:
      c_Text = "Unknown Step - How did you get here?";
      break;
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write to log file and print to console.

   Write detailed information with logging engine to log file but
   only print concise details to console.

   \param[in]     orc_Text        Information text
   \param[in]     orq_Error       True: log as error; false: log as information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SupSuSequences::m_WriteLog(const stw::scl::C_SclString & orc_Text, const bool & orq_IsError) const
{
   C_SydeSup::h_WriteLog("Report Progress", orc_Text, orq_IsError, mq_Quiet);
}
