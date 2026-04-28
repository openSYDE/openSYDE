//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export communication stack settings of an openSYDE node.

   Create a .c and .h file providing entire communication stack configuration.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <map>

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_OscUtils.hpp"
#include "C_SclStringList.hpp"
#include "C_OscExportCommunicationStack.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscExportUti.hpp"
#include "C_OscExportDataPool.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return filename (without extension)

   Compose target file name for code generation.

   \param[in]  ou8_InterfaceIndex   index of interface
   \param[in]  ore_ProtocolType     protocol type (CL2, ECeS, ECoS)

   \return           assembled filename
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCommunicationStack::h_GetFileName(const uint8_t ou8_InterfaceIndex,
                                                         const C_OscCanProtocol::E_Type & ore_ProtocolType)
{
   // assemble filename
   // add Datapool name + protocol name + node index
   const C_SclString c_Text = "comm_" + mh_GetProtocolNameByType(ore_ProtocolType).LowerCase() + "_can" +
                              C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U);

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return name of configuration structure

   Compose name of configuration structure for code generation.

   \param[in]  ou8_InterfaceIndex   index of interface
   \param[in]  ore_ProtocolType     protocol type (CL2, ECeS, ECoS)

   \return   assembled configuration structure name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCommunicationStack::h_GetConfigurationName(const uint8_t ou8_InterfaceIndex,
                                                                  const C_OscCanProtocol::E_Type & ore_ProtocolType)
{
   const C_SclString c_Name = "gt_comm_" + mh_GetProtocolNameByType(ore_ProtocolType).LowerCase() + "_can" +
                              C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                              "_ProtocolConfiguration";

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert overall code structure version to process data communication code subversion.

   The overall code generation version decodes the format versions of parametrization (*_datapool.c/.h files)
   and process data communication code (comm_*.c/.h files).

   \param[in]  ou16_GenCodeVersion  Overall code format version

   \return version of comm code
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscExportCommunicationStack::h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion)
{
   uint16_t u16_Return = ou16_GenCodeVersion;

   switch (ou16_GenCodeVersion)
   {
   case 1: //same logic ...
   case 2: //same logic ...
   case 3:
      u16_Return = ou16_GenCodeVersion;
      break;
   case 4: //same value ...
   case 5:
   case 6:
      u16_Return = 3U;
      break;
   default:
      // should never occur...
      tgl_assert(false);
      break;
   }

   return u16_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create source files

   The caller must provide a valid path and node configuration.

   \param[in]  orc_Path                storage path for created files
   \param[in]  orc_Node                node configuration
   \param[in]  ou16_ApplicationIndex   index of programmable application within orc_Node.c_Applications
   \param[in]  ou8_InterfaceIndex      index of CAN interface
   \param[in]  ou32_DatapoolIndex      index of Datapool
   \param[in]  ore_Protocol            protocol type (CL2, ECeS, ECoS)
   \param[in]  orc_ExportToolInfo      information about calling executable (name + version)

   \return
   C_NO_ERR    Operation success
   C_NOACT     Application is not of type ePROGRAMMABLE_APPLICATION or has unknown code structure version
   C_RD_WR     Operation failure: cannot store files
   C_CONFIG    Protocol or Datapool not available in node for interface or application index out of range
               Message definition does not contain at least one message with signals
   C_RANGE     Application index out of range
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportCommunicationStack::h_CreateSourceCode(const C_SclString & orc_Path, const C_OscNode & orc_Node,
                                                          const uint16_t ou16_ApplicationIndex,
                                                          const uint8_t ou8_InterfaceIndex,
                                                          const uint32_t ou32_DatapoolIndex,
                                                          const C_OscCanProtocol::E_Type & ore_Protocol,
                                                          const C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Retval = C_NO_ERR;
   C_OscNodeApplication c_Application;

   if (ou16_ApplicationIndex < orc_Node.c_Applications.size())
   {
      c_Application = orc_Node.c_Applications[ou16_ApplicationIndex];
   }
   else
   {
      osc_write_log_error("Creating source code", "Application index " + C_SclString::IntToStr(ou16_ApplicationIndex) +
                          " out of range.");
      s32_Retval = C_RANGE;
   }

   if (s32_Retval == C_NO_ERR)
   {
      // make sure application is programmable
      if (c_Application.e_Type != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         osc_write_log_error("Creating source code",
                             "Did not generate code for application \"" + c_Application.c_Name +
                             "\" because application is not programmable.");
         s32_Retval = C_NOACT;
      }
      else
      {
         // make sure version is known
         if (c_Application.u16_GenCodeVersion > C_OscNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
         {
            osc_write_log_error("Creating source code",
                                "Did not generate code for application \"" + c_Application.c_Name +
                                "\" because code format version \"" +
                                C_SclString::IntToStr(c_Application.u16_GenCodeVersion) + "\" is unknown.");
            s32_Retval = C_NOACT;
         }
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      const C_OscCanProtocol * const pc_ComProtocol = orc_Node.GetCanProtocolConst(ore_Protocol, ou32_DatapoolIndex);
      const C_OscNodeDataPool * const pc_DataPool = orc_Node.GetComDataPoolConst(ore_Protocol, ou32_DatapoolIndex);

      if ((pc_ComProtocol != NULL) && (pc_DataPool != NULL))
      {
         //check whether there is at least one message with signals defined; otherwise fail; KFXTCSWRSCC_474
         bool q_NoSignals = true;
         const C_OscCanMessageContainer & rc_Messages = pc_ComProtocol->c_ComMessages[ou8_InterfaceIndex];

         for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < rc_Messages.c_TxMessages.size(); u16_MessageIndex++)
         {
            if (rc_Messages.c_TxMessages[u16_MessageIndex].c_Signals.size() > 0)
            {
               q_NoSignals = false;
               break;
            }
         }

         if (q_NoSignals == true)
         {
            for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < rc_Messages.c_RxMessages.size(); u16_MessageIndex++)
            {
               if (rc_Messages.c_RxMessages[u16_MessageIndex].c_Signals.size() > 0)
               {
                  q_NoSignals = false;
                  break;
               }
            }
         }

         if (q_NoSignals == false)
         {
            uint32_t u32_HashValue = 0U;
            //calculate hash value over the current state of the Datapool and protocol definitions
            pc_ComProtocol->CalcHash(u32_HashValue);
            pc_DataPool->CalcHash(u32_HashValue);
            const C_SclString c_ProjectId = C_SclString::IntToStr(u32_HashValue);

            // create header file
            s32_Retval = mh_CreateHeaderFile(orc_ExportToolInfo, orc_Path, c_Application, *pc_ComProtocol,
                                             ou8_InterfaceIndex, c_ProjectId);

            // create implementation file
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = mh_CreateImplementationFile(orc_ExportToolInfo, orc_Path, c_Application, *pc_ComProtocol,
                                                        *pc_DataPool, ou8_InterfaceIndex, c_ProjectId);
            }
         }
         else
         {
            osc_write_log_error("Creating source code",
                                "No messages with signals exist for specified communication protocol " +
                                mh_GetProtocolNameByType(
                                   ore_Protocol) + " for interface index " + C_SclString::IntToStr(ou8_InterfaceIndex));
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Creating source code",
                             "Protocol definition or Datapool does not exist for specified communication protocol " +
                             mh_GetProtocolNameByType(ore_Protocol));
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create header file

   \param[in]  orc_ExportToolInfo   information about calling executable (name + version)
   \param[in]  orc_Path             storage path for created file
   \param[in]  orc_Applicaton       application data
   \param[in]  orc_ComProtocol      communication protocol configuration
   \param[in]  ou8_InterfaceIndex   index of CAN interface
   \param[in]  orc_ProjectId        project id for consistency check

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportCommunicationStack::mh_CreateHeaderFile(const C_SclString & orc_ExportToolInfo,
                                                           const C_SclString & orc_Path,
                                                           const C_OscNodeApplication & orc_Applicaton,
                                                           const C_OscCanProtocol & orc_ComProtocol,
                                                           const uint8_t ou8_InterfaceIndex,
                                                           const C_SclString & orc_ProjectId)
{
   int32_t s32_Retval;
   C_SclStringList c_Data;

   c_Data.Clear();

   // add header
   mh_AddHeader(orc_ExportToolInfo, c_Data, ou8_InterfaceIndex, orc_ComProtocol.e_Type, mhq_IS_HEADER_FILE);

   // add includes
   c_Data.Append("");
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Includes"));
   c_Data.Append("#include \"stwtypes.h\"");
   c_Data.Append("#include \"osy_com_configuration.h\"");
   c_Data.Append("");
   C_OscExportUti::h_AddExternCeStart(c_Data);

   // add defines
   mh_AddDefines(c_Data, orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex], ou8_InterfaceIndex,
                 orc_ComProtocol.e_Type, orc_ProjectId, orc_Applicaton.u16_GenCodeVersion, mhq_IS_HEADER_FILE);

   // add types
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   // add global variables
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Global Variables"));
   c_Data.Append("///Stack configuration");
   c_Data.Append("extern const T_osy_com_protocol_configuration " +
                 h_GetConfigurationName(ou8_InterfaceIndex, orc_ComProtocol.e_Type) + ";");
   c_Data.Append("");

   // add function prototypes
   C_OscExportUti::h_AddProjIdFunctionPrototype(c_Data, mh_GetMagicName(orc_ProjectId, ou8_InterfaceIndex,
                                                                        orc_ComProtocol.e_Type));

   // add implementation
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Implementation"));
   C_OscExportUti::h_AddExternCeEnd(c_Data);
   c_Data.Append("#endif");

   // finally save all stuff into the file
   s32_Retval =
      C_OscExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(ou8_InterfaceIndex, orc_ComProtocol.e_Type), true);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create implementation file

   \param[in]  orc_ExportToolInfo   information about calling executable (name + version)
   \param[in]  orc_Path             storage path for created file
   \param[in]  orc_Applicaton       application data
   \param[in]  orc_ComProtocol      communication protocol configuration
   \param[in]  orc_DataPool         Datapool configuration
   \param[in]  ou8_InterfaceIndex   index of CAN interface
   \param[in]  orc_ProjectId        project id for consistency check

   \return
   C_NO_ERR    Operation success
   C_RD_WR     Operation failure: cannot store file
   C_CONFIG    Datapool not available for interface
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportCommunicationStack::mh_CreateImplementationFile(const C_SclString & orc_ExportToolInfo,
                                                                   const C_SclString & orc_Path,
                                                                   const C_OscNodeApplication & orc_Applicaton,
                                                                   const C_OscCanProtocol & orc_ComProtocol,
                                                                   const C_OscNodeDataPool & orc_DataPool,
                                                                   const uint8_t ou8_InterfaceIndex,
                                                                   const C_SclString & orc_ProjectId)
{
   int32_t s32_Retval;
   uint32_t u32_TxListIndex;
   uint32_t u32_RxListIndex;

   if ((C_OscCanProtocol::h_GetComListIndex(orc_DataPool, ou8_InterfaceIndex, true, u32_TxListIndex) == C_NO_ERR) &&
       (C_OscCanProtocol::h_GetComListIndex(orc_DataPool, ou8_InterfaceIndex, false, u32_RxListIndex) == C_NO_ERR))
   {
      C_SclStringList c_Data;
      const bool q_TxMessagesPresent =
         (orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex].c_TxMessages.size() > 0) ? true : false;
      const bool q_RxMessagesPresent =
         (orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex].c_RxMessages.size() > 0) ? true : false;
      const C_OscCanMessageContainer & rc_Messages = orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex];
      bool q_MessageWithoutSignalsPresent = false;

      for (uint16_t u16_Message = 0U; u16_Message < rc_Messages.c_TxMessages.size(); u16_Message++)
      {
         if (rc_Messages.c_TxMessages[u16_Message].c_Signals.size() == 0)
         {
            q_MessageWithoutSignalsPresent = true;
            break;
         }
      }

      if (q_MessageWithoutSignalsPresent == false)
      {
         for (uint16_t u16_Message = 0U; u16_Message < rc_Messages.c_RxMessages.size(); u16_Message++)
         {
            if (rc_Messages.c_RxMessages[u16_Message].c_Signals.size() == 0)
            {
               q_MessageWithoutSignalsPresent = true;
               break;
            }
         }
      }

      // add header
      mh_AddHeader(orc_ExportToolInfo, c_Data, ou8_InterfaceIndex, orc_ComProtocol.e_Type, mhq_IS_IMPLEMENTATION_FILE);

      // add includes
      mh_AddCeIncludes(c_Data, orc_DataPool, ou8_InterfaceIndex, orc_ComProtocol.e_Type,
                       ((q_RxMessagesPresent == false) || (q_TxMessagesPresent == false) ||
                        (q_MessageWithoutSignalsPresent == true)));

      // add defines
      mh_AddDefines(c_Data, orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex], ou8_InterfaceIndex,
                    orc_ComProtocol.e_Type, orc_ProjectId, orc_Applicaton.u16_GenCodeVersion,
                    mhq_IS_IMPLEMENTATION_FILE);

      // add types
      c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Types"));
      c_Data.Append("");

      // add module global variables and function prototypes
      mh_AddCeModuleGlobal(c_Data, orc_DataPool.q_IsSafety, orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex],
                           ou8_InterfaceIndex, orc_ComProtocol.e_Type, orc_Applicaton.u16_GenCodeVersion,
                           u32_TxListIndex, u32_RxListIndex);

      // add global variables
      mh_AddCeGlobalVariables(c_Data, orc_DataPool.c_Name, ou8_InterfaceIndex, orc_ComProtocol.e_Type,
                              q_TxMessagesPresent, q_RxMessagesPresent);

      // add implementation
      c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Implementation"));
      c_Data.Append("");

      // finally save all stuff into the file
      s32_Retval = C_OscExportUti::h_SaveToFile(c_Data, orc_Path,
                                                h_GetFileName(ou8_InterfaceIndex, orc_ComProtocol.e_Type), false);
   }
   else
   {
      osc_write_log_error("Creating source code",
                          "Datapool does not exist for specified communication protocol" +
                          mh_GetProtocolNameByType(orc_ComProtocol.e_Type) + " with interface index " +
                          C_SclString::IntToStr(ou8_InterfaceIndex));
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add file header

   \param[in]      orc_ExportToolInfo  information about calling executable (name + version)
   \param[in,out]  orc_Data            converted data to string list
   \param[in]      ou8_InterfaceIndex  index of interface
   \param[in]      ore_Protocol        protocol type (CL2, ECeS, ECoS)
   \param[in]      oq_FileType         .c or .h file selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddHeader(const C_SclString & orc_ExportToolInfo, C_SclStringList & orc_Data,
                                                 const uint8_t ou8_InterfaceIndex,
                                                 const C_OscCanProtocol::E_Type & ore_Protocol, const bool oq_FileType)
{
   orc_Data.Append(C_OscExportUti::h_GetHeaderSeparator());
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append(
         "   \\brief       openSYDE Communication stack data definition (Source file with constant definitions)");
      orc_Data.Append("");
      orc_Data.Append("   Defines the communication configuration for protocol type " +
                      mh_GetProtocolNameByType(ore_Protocol) + " on CAN interface " +
                      C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + ".");
      orc_Data.Append("");
      orc_Data.Append(C_OscExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   else
   {
      orc_Data.Append(
         "   \\brief       openSYDE Communication stack data definition (Header file with constant and global definitions)");
      orc_Data.Append("");
      orc_Data.Append(C_OscExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   orc_Data.Append("*/");
   orc_Data.Append(C_OscExportUti::h_GetHeaderSeparator());

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SclString c_HeaderGuard = h_GetFileName(ou8_InterfaceIndex, ore_Protocol).UpperCase() + "H";
      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add includes into C file

   \param[in,out]  orc_Data            converted data to string list
   \param[in]      orc_DataPool        name of Datapool
   \param[in]      ou8_InterfaceIndex  index of interface
   \param[in]      ore_Protocol        protocol type (CL2, ECeS, ECoS)
   \param[in]      oq_NullRequired     true: definition of NULL required in .c file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddCeIncludes(C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_DataPool,
                                                     const uint8_t ou8_InterfaceIndex,
                                                     const C_OscCanProtocol::E_Type & ore_Protocol,
                                                     const bool oq_NullRequired)
{
   const C_SclString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).LowerCase();

   orc_Data.Append("");
   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Includes"));

   if (oq_NullRequired == true)
   {
      orc_Data.Append("#include <stddef.h>");
      orc_Data.Append("");
   }

   orc_Data.Append("#include \"" + h_GetFileName(ou8_InterfaceIndex, ore_Protocol) + ".h\"");
   orc_Data.Append("");
   orc_Data.Append("#include \"osy_com_" + c_ProtocolName + "_driver.h\"");
   orc_Data.Append("");
   orc_Data.Append("#include \"" + C_OscExportDataPool::h_GetFileName(orc_DataPool) + ".h\"");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add defines

   \param[in,out]  orc_Data               converted data to string list
   \param[in]      orc_ComMessage         communication protocol message definition
   \param[in]      ou8_InterfaceIndex     index of interface
   \param[in]      ore_Protocol           protocol type (CL2, ECeS, ECoS)
   \param[in]      orc_ProjectId          project id for consistency check
   \param[in]      ou16_GenCodeVersion    version of structure (generate code as specified for this version)
   \param[in]      oq_FileType            .c or .h file selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddDefines(C_SclStringList & orc_Data,
                                                  const C_OscCanMessageContainer & orc_ComMessage,
                                                  const uint8_t ou8_InterfaceIndex,
                                                  const C_OscCanProtocol::E_Type & ore_Protocol,
                                                  const C_SclString & orc_ProjectId, const uint16_t ou16_GenCodeVersion,
                                                  const bool oq_FileType)
{
   const C_SclString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();
   const C_SclString c_MagicName = mh_GetMagicName(orc_ProjectId, ou8_InterfaceIndex, ore_Protocol);

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Defines"));

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      C_OscExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, true);
      orc_Data.Append("///Index of Tx messages");
      for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < orc_ComMessage.c_TxMessages.size(); u16_MessageIndex++)
      {
         const C_SclString c_Name = orc_ComMessage.c_TxMessages[u16_MessageIndex].c_Name.UpperCase();
         orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                         C_SclString::IntToStr(
                            static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_TX_MSG_INDEX_" + c_Name + " (" +
                         C_SclString::IntToStr(u16_MessageIndex) + "U)");
      }
      orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                      C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_TX_MSGS (" +
                      C_SclString::IntToStr(orc_ComMessage.c_TxMessages.size()) + "U)");
      orc_Data.Append("");

      orc_Data.Append("///Index of Rx messages");
      for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < orc_ComMessage.c_RxMessages.size(); u16_MessageIndex++)
      {
         const C_SclString c_Name = orc_ComMessage.c_RxMessages[u16_MessageIndex].c_Name.UpperCase();
         orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                         C_SclString::IntToStr(
                            static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_RX_MSG_INDEX_" + c_Name + " (" +
                         C_SclString::IntToStr(u16_MessageIndex) + "U)");
      }
      orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                      C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_RX_MSGS (" +
                      C_SclString::IntToStr(orc_ComMessage.c_RxMessages.size()) + "U)");
      orc_Data.Append("");

      if (ou16_GenCodeVersion >= 2U)
      {
         bool q_CommentAdded = false;
         // Tx MUX messages
         for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < orc_ComMessage.c_TxMessages.size(); u16_MessageIndex++)
         {
            const C_OscCanMessage & rc_Message = orc_ComMessage.c_TxMessages[u16_MessageIndex];

            if (rc_Message.IsMultiplexed() == true)
            {
               const C_SclString c_Name = rc_Message.c_Name.UpperCase();
               std::set<uint16_t> c_MultiplexerValues;
               std::set<uint16_t>::const_iterator c_ItValue;
               uint16_t u16_ValueIndex = 0;

               if (q_CommentAdded == false)
               {
                  // add comment only if a multiplexed message was found and only once
                  orc_Data.Append("///Index of Tx multiplexed messages");
                  q_CommentAdded = true;
               }

               // for every multiplexer value add one define
               rc_Message.GetMultiplexerValues(c_MultiplexerValues);
               for (c_ItValue = c_MultiplexerValues.begin(); c_ItValue != c_MultiplexerValues.end(); ++c_ItValue)
               {
                  orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                                  C_SclString::IntToStr(
                                     static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_TX_MSG_MUX_INDEX_" + c_Name +
                                  "_VALUE_" + C_SclString::IntToStr(*c_ItValue) + " (" +
                                  C_SclString::IntToStr(u16_ValueIndex) + "U)");
                  u16_ValueIndex++;
               }

               orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                               C_SclString::IntToStr(
                                  static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_TX_MSG_" + c_Name + "_NUM_MUX_MSGS" +
                               " (" +  C_SclString::IntToStr(c_MultiplexerValues.size()) + "U)");
               orc_Data.Append("");
            }
         }
         // Rx MUX messages
         q_CommentAdded = false;
         for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < orc_ComMessage.c_RxMessages.size(); u16_MessageIndex++)
         {
            const C_OscCanMessage & rc_Message = orc_ComMessage.c_RxMessages[u16_MessageIndex];

            if (rc_Message.IsMultiplexed() == true)
            {
               const C_SclString c_Name = rc_Message.c_Name.UpperCase();
               std::set<uint16_t> c_MultiplexerValues;
               std::set<uint16_t>::const_iterator c_ItValue;
               uint16_t u16_ValueIndex = 0;

               if (q_CommentAdded == false)
               {
                  // add comment only if a multiplexed message was found and only once
                  orc_Data.Append("///Index of Rx multiplexed messages");
                  q_CommentAdded = true;
               }

               // for every multiplexer value add one define
               rc_Message.GetMultiplexerValues(c_MultiplexerValues);
               for (c_ItValue = c_MultiplexerValues.begin(); c_ItValue != c_MultiplexerValues.end(); ++c_ItValue)
               {
                  orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                                  C_SclString::IntToStr(
                                     static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_RX_MSG_MUX_INDEX_" + c_Name +
                                  "_VALUE_" + C_SclString::IntToStr(*c_ItValue) + " (" +
                                  C_SclString::IntToStr(u16_ValueIndex) + "U)");
                  u16_ValueIndex++;
               }

               orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                               C_SclString::IntToStr(
                                  static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_RX_MSG_" + c_Name + "_NUM_MUX_MSGS" +
                               " (" +  C_SclString::IntToStr(c_MultiplexerValues.size()) + "U)");
               orc_Data.Append("");
            }
         }
      }
   }
   else
   {
      if (ou16_GenCodeVersion >= 2U)
      {
         orc_Data.Append("///check for correct version of structure definitions");
         orc_Data.Append(
            "#if OSY_COM_CONFIG_DEFINITION_VERSION != 0x" +
            C_SclString::IntToHex(static_cast<int64_t>(C_OscExportCommunicationStack::h_ConvertOverallCodeVersion(
                                                          ou16_GenCodeVersion)), 4U) + "U");
         orc_Data.Append("///if compilation fails here the openSYDE library version does not match the version of the "
                         "generated code");
         orc_Data.Append("static T_osy_non_existing_type_" + orc_ProjectId + " mt_Variable;");
         orc_Data.Append("#endif");
         orc_Data.Append("");
      }
      C_OscExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add module global variables and function prototypes

   Implementation file .C only.

   \param[in,out]  orc_Data               converted data to string list
   \param[in]      oq_SafeData            true: place status data in safe RAM
   \param[in]      orc_ComMessage         communication protocol message definition
   \param[in]      ou8_InterfaceIndex     index of interface
   \param[in]      ore_Protocol           protocol type (CL2, ECeS, ECoS)
   \param[in]      ou16_GenCodeVersion    version of structure (generate code as specified for this version)
   \param[in]      ou32_TxListIndex       Datapool list index for Tx messages
   \param[in]      ou32_RxListIndex       Datapool list index for Rx messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddCeModuleGlobal(C_SclStringList & orc_Data, const bool oq_SafeData,
                                                         const C_OscCanMessageContainer & orc_ComMessage,
                                                         const uint8_t ou8_InterfaceIndex,
                                                         const C_OscCanProtocol::E_Type & ore_Protocol,
                                                         const uint16_t ou16_GenCodeVersion,
                                                         const uint32_t ou32_TxListIndex,
                                                         const uint32_t ou32_RxListIndex)
{
   C_SclString c_SafeRamData;
   const C_SclString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();

   if (oq_SafeData == true)
   {
      //status data needs to be placed in safe memory as the driver deals with safety relevant data
      if (ou16_GenCodeVersion >= 3U)
      {
         c_SafeRamData = "OSY_DPA_SAFE_RAM_DATA_PRIVATE_ZERO ";
      }
      else
      {
         c_SafeRamData = "OSY_DPA_SAFE_RAM_DATA ";
      }
   }

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Module Global Variables"));
   orc_Data.Append("///message status");

   // Tx messages
   if (orc_ComMessage.c_TxMessages.size() > 0)
   {
      if (ou16_GenCodeVersion >= 2U)
      {
         orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_status mat_StatusTx[COMM_" +
                         c_ProtocolName + "_CAN" +
                         C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                         "_NUMBER_OF_TX_MSGS];");
      }
      else
      {
         orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_status_private mat_StatusTx[COMM_" +
                         c_ProtocolName + "_CAN" +
                         C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                         "_NUMBER_OF_TX_MSGS];");
      }
   }

   // Rx messages
   if (orc_ComMessage.c_RxMessages.size() > 0)
   {
      if (ou16_GenCodeVersion >= 2U)
      {
         orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_status mat_StatusRx[COMM_" +
                         c_ProtocolName + "_CAN" +
                         C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                         "_NUMBER_OF_RX_MSGS];");
      }
      else
      {
         orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_status_private mat_StatusRx[COMM_" +
                         c_ProtocolName + "_CAN" +
                         C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                         "_NUMBER_OF_RX_MSGS];");
      }
   }

   // mux messages (since V2)
   if (ou16_GenCodeVersion >= 2U)
   {
      orc_Data.Append("");
      orc_Data.Append("///mux status");
      // Tx mux messages
      if  (orc_ComMessage.c_TxMessages.size() > 0)
      {
         orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_mux_status mat_StatusMuxTxMessages[" +
                         C_SclString::IntToStr(mh_CountMuxMessages(orc_ComMessage.c_TxMessages)) + "];");
      }

      // Rx mux messages
      if (orc_ComMessage.c_RxMessages.size() > 0)
      {
         orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_mux_status mat_StatusMuxRxMessages[" +
                         C_SclString::IntToStr(mh_CountMuxMessages(orc_ComMessage.c_RxMessages)) + "];");
      }
   }
   orc_Data.Append("");

   if (orc_ComMessage.c_TxMessages.size() > 0)
   {
      orc_Data.Append("///Tx signal definitions");
      mh_AddSignalDefinitions(orc_Data, ou32_TxListIndex, orc_ComMessage.c_TxMessages, ou16_GenCodeVersion);
   }

   if (orc_ComMessage.c_RxMessages.size() > 0)
   {
      orc_Data.Append("///Rx signal definitions");
      mh_AddSignalDefinitions(orc_Data, ou32_RxListIndex, orc_ComMessage.c_RxMessages, ou16_GenCodeVersion);
   }

   if (orc_ComMessage.c_TxMessages.size() > 0)
   {
      orc_Data.Append("///Tx message definitions");
      if (ou16_GenCodeVersion >= 2U)
      {
         // add message mux definitions (since V2)
         mh_AddMessageMuxDefinitions(orc_Data, orc_ComMessage.c_TxMessages, "Tx");
      }
      mh_AddMessageDefinitions(orc_Data, ou8_InterfaceIndex, ore_Protocol, orc_ComMessage.c_TxMessages,
                               ou16_GenCodeVersion, true);
   }

   if (orc_ComMessage.c_RxMessages.size() > 0)
   {
      orc_Data.Append("///Rx message definitions");
      if (ou16_GenCodeVersion >= 2U)
      {
         // add message mux definitions (since V2)
         mh_AddMessageMuxDefinitions(orc_Data, orc_ComMessage.c_RxMessages, "Rx");
      }
      mh_AddMessageDefinitions(orc_Data, ou8_InterfaceIndex, ore_Protocol, orc_ComMessage.c_RxMessages,
                               ou16_GenCodeVersion, false);
   }

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add global variables

   \param[in,out]  orc_Data               converted data to string list
   \param[in]      orc_DataPoolName       name of Datapool
   \param[in]      ou8_InterfaceIndex     index of interface
   \param[in]      ore_Protocol           protocol type (CL2, ECeS, ECoS)
   \param[in]      oq_TxMessagesPresent   true: There is at least one Tx message defined
   \param[in]      oq_RxMessagesPresent   true: There is at least one Rx message defined
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddCeGlobalVariables(C_SclStringList & orc_Data,
                                                            const C_SclString & orc_DataPoolName,
                                                            const uint8_t ou8_InterfaceIndex,
                                                            const C_OscCanProtocol::E_Type & ore_Protocol,
                                                            const bool oq_TxMessagesPresent,
                                                            const bool oq_RxMessagesPresent)
{
   const C_SclString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Global Variables"));
   orc_Data.Append("///Stack configuration");
   orc_Data.Append("const T_osy_com_protocol_configuration " +
                   h_GetConfigurationName(ou8_InterfaceIndex, ore_Protocol) + " =");
   orc_Data.Append("{");
   orc_Data.Append("   " + C_SclString::IntToStr(ou8_InterfaceIndex) + "U,  ///< selected CAN channel");
   orc_Data.Append("   OSY_COM_PROTOCOL_TYPE_" + c_ProtocolName + ",  ///< protocol type for this stack"
                   );
   orc_Data.Append("   {");
   orc_Data.Append("      OSY_COM_CONFIG_MAGIC,  ///< identification of valid comm configuration");
   orc_Data.Append("      OSY_COM_CONFIG_DEFINITION_VERSION,  ///< configuration version");
   orc_Data.Append("      COMM_" + c_ProtocolName + "_CAN" +
                   C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                   "_NUMBER_OF_TX_MSGS,  ///< number of Tx messages in this configuration");
   orc_Data.Append("      COMM_" + c_ProtocolName + "_CAN" +
                   C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                   "_NUMBER_OF_RX_MSGS,  ///< number of Rx messages in this configuration");
   if (oq_TxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_MessagesTx[0],  ///< definition of Tx messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< definition of Tx messages (none defined)");
   }
   if (oq_RxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_MessagesRx[0],  ///< definition of Rx messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< definition of Rx messages (none defined)");
   }
   if (oq_TxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_StatusTx[0],  ///< status of Tx messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< status of Tx messages (none defined)");
   }
   if (oq_RxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_StatusRx[0],  ///< status of Rx messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< status of Rx messages (none defined)");
   }
   orc_Data.Append("      &gt_" + orc_DataPoolName + "_DataPool  ///< Datapool containing signal values");
   orc_Data.Append("   },");
   orc_Data.Append("   OSY_COM_" + c_ProtocolName + "_DRIVER  ///< pointer to protocol specific driver");
   orc_Data.Append("};");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add signal definition section to C file

   \param[in,out]  orc_Data               list of strings to add to
   \param[in]      ou32_SignalListIndex   index of Datapool list containing signals
   \param[in]      orc_Messages           definition of messages
   \param[in]      ou16_GenCodeVersion    version of structure (generate code as specified for this version)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddSignalDefinitions(C_SclStringList & orc_Data,
                                                            const uint32_t ou32_SignalListIndex,
                                                            const std::vector<C_OscCanMessage> & orc_Messages,
                                                            const uint16_t ou16_GenCodeVersion)
{
   for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < orc_Messages.size(); u16_MessageIndex++)
   {
      const C_OscCanMessage & rc_Message = orc_Messages[u16_MessageIndex];

      //Only create of signal table(s) for messages with signals.
      //For multiplexed messages we have at least one signal by definition; otherwise it could not be multiplexed.
      //So the check will cover both types of messages.
      if (rc_Message.c_Signals.size() > 0)
      {
         bool q_AddNonMux = false;
         if (ou16_GenCodeVersion >= 2U)
         {
            // check for mux messages
            uint32_t u32_MultiplexerIndex;
            if (rc_Message.IsMultiplexed(&u32_MultiplexerIndex) == true)
            {
               std::map< int32_t, std::vector<C_OscCanSignal> > c_MuxedSignalsPerValue;
               std::vector<C_OscCanSignal> c_NonMuxedSignals;

               // get signals grouped by mux value
               mh_GroupSignalsByMuxValue(rc_Message, u32_MultiplexerIndex, c_MuxedSignalsPerValue, c_NonMuxedSignals);

               // Special case: Multiplexed message without any multiplexed signals.
               if (c_MuxedSignalsPerValue.size() == 0) //no muxed signals ...
               {
                  const C_OscCanSignal & rc_MultiplexerSignal = rc_Message.c_Signals[u32_MultiplexerIndex];
                  std::vector<C_OscCanSignal> c_Signals;
                  c_Signals.push_back(rc_MultiplexerSignal);
                  //add non-muxed signals:
                  c_Signals.insert(c_Signals.end(), c_NonMuxedSignals.begin(), c_NonMuxedSignals.end());

                  orc_Data.Append("static const T_osy_com_signal_definition mat_" + rc_Message.c_Name + "[" +
                                  C_SclString::IntToStr(c_Signals.size()) + "] =");
                  orc_Data.Append("{");
                  // add signals
                  mh_ConvertSignalsToStrings(orc_Data, c_Signals, ou32_SignalListIndex, true);
                  orc_Data.Append("};");
                  orc_Data.Append("");
               }
               else
               {
                  std::map< int32_t, std::vector<C_OscCanSignal> >::const_iterator c_ItValue;

                  // add signal definitions to data string list
                  for (c_ItValue = c_MuxedSignalsPerValue.begin(); c_ItValue != c_MuxedSignalsPerValue.end();
                       ++c_ItValue)
                  {
                     orc_Data.Append("static const T_osy_com_signal_definition mat_" + rc_Message.c_Name + "Value" +
                                     C_SclString::IntToStr(c_ItValue->first) + "[" +
                                     C_SclString::IntToStr(c_ItValue->second.size() +
                                                           c_NonMuxedSignals.size()) + "] =");
                     orc_Data.Append("{");
                     // add multiplexer+multiplexed signals
                     mh_ConvertSignalsToStrings(orc_Data, c_MuxedSignalsPerValue[c_ItValue->first],
                                                ou32_SignalListIndex,
                                                (c_NonMuxedSignals.size() == 0));
                     // add non-muxed signals
                     mh_ConvertSignalsToStrings(orc_Data, c_NonMuxedSignals, ou32_SignalListIndex, true);
                     orc_Data.Append("};");
                     orc_Data.Append("");
                  }
               }
            }
            else
            {
               q_AddNonMux = true;
            }
         }
         else
         {
            q_AddNonMux = true;
         }

         // add messages without mux
         if (q_AddNonMux == true)
         {
            orc_Data.Append("static const T_osy_com_signal_definition mat_" + rc_Message.c_Name +
                            "[" + C_SclString::IntToStr(rc_Message.c_Signals.size()) + "] =");
            orc_Data.Append("{");
            mh_ConvertSignalsToStrings(orc_Data, rc_Message.c_Signals, ou32_SignalListIndex, true);
            orc_Data.Append("};");
            orc_Data.Append("");
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add message mux definition section to C file

   \param[in,out]  orc_Data         list of strings to add to
   \param[in]      orc_Messages     definition of messages
   \param[in]      orc_TxRxString   "Tx" or "Rx"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddMessageMuxDefinitions(C_SclStringList & orc_Data,
                                                                const std::vector<C_OscCanMessage> & orc_Messages,
                                                                const C_SclString & orc_TxRxString)
{
   const uint16_t u16_MessageCount = static_cast<uint16_t>(orc_Messages.size());

   orc_Data.Append("static const T_osy_com_message_mux_definition mat_MessagesMux" + orc_TxRxString + "[" +
                   C_SclString::IntToStr(mh_CountMuxMessages(orc_Messages)) + "] =");

   orc_Data.Append("{");

   for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < u16_MessageCount; u16_MessageIndex++)
   {
      C_SclString c_Text;
      const C_OscCanMessage & rc_Message = orc_Messages[u16_MessageIndex];
      uint32_t u32_MultiplexerIndex;

      // check for mux messages
      if (rc_Message.IsMultiplexed(&u32_MultiplexerIndex) == true)
      {
         // mux message: add entry for every mux value
         std::map< int32_t, std::vector<C_OscCanSignal> > c_MuxedSignalsPerValue;
         std::vector<C_OscCanSignal> c_NonMuxedSignals;

         // get signals grouped by mux value
         mh_GroupSignalsByMuxValue(rc_Message, u32_MultiplexerIndex, c_MuxedSignalsPerValue, c_NonMuxedSignals);

         // Special case: Multiplexed message without any multiplexed signals. Add dummy-mux value "0".
         if (c_MuxedSignalsPerValue.size() == 0) //no muxed signals ...
         {
            c_Text = "   { 0U, " + C_SclString::IntToStr(1U + c_NonMuxedSignals.size()) + "U, " +
                     "&mat_" + rc_Message.c_Name + "[0] }";
            if (u16_MessageIndex != (u16_MessageCount - 1U))
            {
               c_Text += ",";
            }
            orc_Data.Append(c_Text);
         }
         else
         {
            uint16_t u16_MuxCount = 0;
            std::map< int32_t, std::vector<C_OscCanSignal> >::const_iterator c_ItValue;

            // add signal definitions to data string list
            for (c_ItValue = c_MuxedSignalsPerValue.begin(); c_ItValue != c_MuxedSignalsPerValue.end(); ++c_ItValue)
            {
               c_Text = "   { " + C_SclString::IntToStr(c_ItValue->first) + "U, " +
                        C_SclString::IntToStr(c_ItValue->second.size() + c_NonMuxedSignals.size()) + "U, " +
                        "&mat_" + rc_Message.c_Name + "Value" + C_SclString::IntToStr(c_ItValue->first) + "[0] }";
               if ((u16_MessageIndex != (u16_MessageCount - 1U)) ||
                   (u16_MuxCount != static_cast<uint16_t>((c_MuxedSignalsPerValue.size() - 1))))
               {
                  c_Text += ",";
               }
               orc_Data.Append(c_Text);
               u16_MuxCount++;
            }
         }
      }
      else
      {
         //if number of signals is zero then add NULL instead of pointer to the signal table
         //muxed message cannot have zero signals so this only affects non-muxed messages
         const uint16_t u16_NumberOfSignals = static_cast<uint16_t>(rc_Message.c_Signals.size());
         C_SclString c_SignalTable;

         if (u16_NumberOfSignals == 0U)
         {
            c_SignalTable = "NULL";
         }
         else
         {
            c_SignalTable = "&mat_" + rc_Message.c_Name + "[0]";
         }

         // no mux message: add one entry
         c_Text = "   { 0U, " + C_SclString::IntToStr(u16_NumberOfSignals) + "U, " + c_SignalTable + " }";
         if (u16_MessageIndex != (u16_MessageCount - 1U))
         {
            c_Text += ",";
         }
         orc_Data.Append(c_Text);
      }
   }
   orc_Data.Append("};");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add message definition section to C file

   \param[in,out]  orc_Data               list of strings to add to
   \param[in]      ou8_InterfaceIndex     index of CAN interface
   \param[in]      ore_Protocol           protocol type (CL2, ECeS, ECoS)
   \param[in]      orc_Messages           definition of messages
   \param[in]      ou16_GenCodeVersion    version of structure (generate code as specified for this version)
   \param[in]      oq_Tx                  true: messages are Tx, false: messages are Rx
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_AddMessageDefinitions(C_SclStringList & orc_Data,
                                                             const uint8_t ou8_InterfaceIndex,
                                                             const C_OscCanProtocol::E_Type & ore_Protocol,
                                                             const std::vector<C_OscCanMessage> & orc_Messages,
                                                             const uint16_t ou16_GenCodeVersion, const bool oq_Tx)
{
   const C_SclString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();
   const uint16_t u16_MessageCount = static_cast<uint16_t>(orc_Messages.size());
   C_SclString c_Text;
   C_SclString c_TxRxString;
   uint16_t u16_MessageCountWithMuxOffset = 0; // necessary for V2 mux message count

   if (oq_Tx == true)
   {
      c_TxRxString = "Tx";
   }
   else
   {
      c_TxRxString = "Rx";
   }

   orc_Data.Append("static const T_osy_com_message_definition mat_Messages" + c_TxRxString + "[COMM_" +
                   c_ProtocolName + "_CAN" + C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                   "_NUMBER_OF_" + c_TxRxString.UpperCase() + "_MSGS] =");
   orc_Data.Append("{");

   for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < u16_MessageCount; u16_MessageIndex++)
   {
      const C_OscCanMessage & rc_Message = orc_Messages[u16_MessageIndex];
      const C_SclString c_Trigger = mh_GetTransmissionTriggerNameByType(rc_Message.e_TxMethod);
      uint16_t u16_DelayTime = 0U;
      uint32_t u32_MessageCounterGap = 0U;
      uint16_t u16_Dlc = rc_Message.u16_Dlc;

      if (ore_Protocol == C_OscCanProtocol::eECES)
      {
         u16_Dlc -= 2U; //Tool: message DLC; target: payload only
      }

      // differences of Tx and Rx
      if (oq_Tx == true)
      {
         //delay time not used for Rx messages and only applicable for change triggered transmissions
         u16_DelayTime = (rc_Message.e_TxMethod == C_OscCanMessage::eTX_METHOD_ON_CHANGE) ?
                         rc_Message.u16_DelayTimeMs : 0U;
      }
      else
      {
         // message counter gap is not relevant for Tx messages
         if (ore_Protocol == C_OscCanProtocol::eECES)
         {
            if (rc_Message.u32_CycleTimeMs != 0)
            {
               u32_MessageCounterGap = ((rc_Message.u32_TimeoutMs / rc_Message.u32_CycleTimeMs) - 1U);
            }
            else
            {
               u32_MessageCounterGap = 0U;
            }
         }
      }

      c_Text =
         "   { 0x" + C_SclString::IntToHex(static_cast<int64_t>(rc_Message.u32_CanId), 3U) + "U, " + // CAN ID
         C_SclString::IntToStr(rc_Message.q_IsExtended) + "U, " +                                    // extended flag
         C_SclString::IntToStr(u16_Dlc) + "U, " + c_Trigger + ", ";                                  // DLC

      // order of entries changed a bit since V2
      if (ou16_GenCodeVersion >= 2U)
      {
         c_Text +=  C_SclString::IntToStr(u32_MessageCounterGap) + "U, "; //message counter gap
      }
      else
      {
         c_Text += C_SclString::IntToStr(rc_Message.c_Signals.size()) + "U, "; // signal number
      }

      c_Text += C_SclString::IntToStr(u16_DelayTime) + "U, "; // minimum interval

      // maximum interval
      if (oq_Tx == true)
      {
         c_Text += C_SclString::IntToStr(rc_Message.u32_CycleTimeMs) + "U, ";
      }
      else
      {
         c_Text += C_SclString::IntToStr(rc_Message.u32_TimeoutMs) + "U, ";
      }

      // order of entries changed a bit since V2
      if (ou16_GenCodeVersion >= 2U)
      {
         std::set<uint16_t> c_MultiplexerValues;
         rc_Message.GetMultiplexerValues(c_MultiplexerValues);
         c_Text +=
            C_SclString::IntToStr(c_MultiplexerValues.size()) + "U, " + // number of mux messages resp. values
            "&mat_StatusMux" + c_TxRxString + "Messages[" +             // pointer to first element of mux status
            C_SclString::IntToStr(u16_MessageCountWithMuxOffset) + "], " +
            "&mat_MessagesMux" + c_TxRxString + "[" + // pointer to first element of mux definition
            C_SclString::IntToStr(u16_MessageCountWithMuxOffset) + "] }";

         if (c_MultiplexerValues.size() != 0)
         {
            u16_MessageCountWithMuxOffset += static_cast<uint16_t>(c_MultiplexerValues.size());
         }
         else
         {
            u16_MessageCountWithMuxOffset++;
         }
      }
      else
      {
         //if number of signals is zero then add NULL instead of pointer to the signal table
         const uint16_t u16_NumberOfSignals = static_cast<uint16_t>(rc_Message.c_Signals.size());
         C_SclString c_SignalTable;

         if (u16_NumberOfSignals == 0U)
         {
            c_SignalTable = "NULL";
         }
         else
         {
            c_SignalTable = "&mat_" + rc_Message.c_Name + "[0]";
         }

         c_Text += C_SclString::IntToStr(u32_MessageCounterGap) + "U, " + //message counter gap
                   c_SignalTable + " }";                                  // pointer to signals
      }

      if (u16_MessageIndex != (u16_MessageCount - 1U))
      {
         c_Text += ",";
      }

      // add as C comment: message name plus message comment if present
      c_Text +=         ("  /// " + C_OscUtils::h_NiceifyStringForCeComment(rc_Message.c_Name));
      if (rc_Message.c_Comment.IsEmpty() == false)
      {
         c_Text += (" (" + C_OscUtils::h_NiceifyStringForCeComment(rc_Message.c_Comment) + ")");
      }
      orc_Data.Append(c_Text);
   }

   orc_Data.Append("};");
   orc_Data.Append("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Count multiplex messages.

   If a message is not multiplexed, count it as one. If it is multiplexed count number of multiplexed messages
   corresponding to this message, i.e. number of multiplex values. If the message has a multiplexer signal and
   therefore is multiplexed, but has no multiplexed signal at all, also count with one.
   See KFXTCSWRSCC_380.

   \param[in]  orc_Messages   Original messages

   \return
   Number of multiplex messages
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscExportCommunicationStack::mh_CountMuxMessages(const std::vector<C_OscCanMessage> & orc_Messages)
{
   uint32_t u32_MessageNum = 0;

   for (uint16_t u16_MessageIndex = 0U; u16_MessageIndex < orc_Messages.size(); u16_MessageIndex++)
   {
      const C_OscCanMessage & rc_Message = orc_Messages[u16_MessageIndex];

      if (rc_Message.IsMultiplexed() == true)
      {
         std::set<uint16_t> c_MultiplexerValues;
         rc_Message.GetMultiplexerValues(c_MultiplexerValues);
         if (c_MultiplexerValues.size() > 0)
         {
            // every multiplex value corresponds to one mux message
            u32_MessageNum += static_cast<uint32_t>(c_MultiplexerValues.size());
         }
         else
         {
            // if no multiplexed signal exists it shall count like non-mux message
            ++u32_MessageNum;
         }
      }
      else
      {
         // count non-mux message with "1"
         ++u32_MessageNum;
      }
   }

   return u32_MessageNum;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string of protocol name by protocol type

   \param[in]  ore_Protocol   protocol type (CL2, ECeS, ECoS)

   \return
   protocol name as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCommunicationStack::mh_GetProtocolNameByType(const C_OscCanProtocol::E_Type & ore_Protocol)
{
   C_SclString c_Name;

   switch (ore_Protocol)
   {
   case C_OscCanProtocol::eLAYER2:
      c_Name = "cl2";
      break;
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
      c_Name = "ecos";
      break;
   case C_OscCanProtocol::eECES:
      c_Name = "eces";
      break;
   case C_OscCanProtocol::eCAN_OPEN:
      c_Name = "can open";
      break;
   case C_OscCanProtocol::eJ1939:
      c_Name = "j1939";
      break;
   default:
      tgl_assert(false);
      break;
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string of byte order by byte order type

   \param[in]  ore_ByteOrder  byte order type (Intel, Motorola)

   \return
   byte order as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCommunicationStack::mh_GetByteOrderNameByType(
   const C_OscCanSignal::E_ByteOrderType & ore_ByteOrder)
{
   C_SclString c_Name;

   switch (ore_ByteOrder)
   {
   case C_OscCanSignal::eBYTE_ORDER_INTEL:
      c_Name = "OSY_COM_BYTE_ORDER_LITTLE";
      break;
   case C_OscCanSignal::eBYTE_ORDER_MOTOROLA:
      c_Name = "OSY_COM_BYTE_ORDER_BIG";
      break;
   default:
      tgl_assert(false);
      break;
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get string of transmission trigger by transmission trigger type

   \param[in]  ore_Trigger    transmission trigger type (cyclic, change)

   \return
   transmission trigger as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCommunicationStack::mh_GetTransmissionTriggerNameByType(
   const C_OscCanMessage::E_TxMethodType & ore_Trigger)
{
   C_SclString c_Name;

   switch (ore_Trigger)
   {
   case C_OscCanMessage::eTX_METHOD_CYCLIC:
      c_Name = "OSY_COM_COMM_METHOD_CYCLIC";
      break;
   case C_OscCanMessage::eTX_METHOD_ON_CHANGE:
      c_Name = "OSY_COM_COMM_METHOD_ON_CHANGE";
      break;
   case C_OscCanMessage::eTX_METHOD_ON_EVENT:
      c_Name = "OSY_COM_COMM_METHOD_ON_EVENT";
      break;
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0:
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240:
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254:
   case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255:
   default:
      //not supported for COMM
      tgl_assert(false);
      break;
   }

   return c_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get magic name for consistency checks.

   \param[in]  orc_ProjectId        Project id for consistency check
   \param[in]  ou8_InterfaceIndex   index of CAN interface
   \param[in]  ore_Protocol         protocol type (CL2, ECeS, ECoS)

   \return
   Magic name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCommunicationStack::mh_GetMagicName(const C_SclString & orc_ProjectId,
                                                           const uint8_t ou8_InterfaceIndex,
                                                           const C_OscCanProtocol::E_Type & ore_Protocol)
{
   const C_SclString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();
   const C_SclString c_MagicName = "COMM_" + c_ProtocolName + "_CAN" +
                                   C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                                   "_PROJECT_ID_" + orc_ProjectId;

   return c_MagicName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility function for creating signal declarations

   \param[in,out]  orc_Data               converted data to string list
   \param[in]      orc_Signals            list of signals for creating signal declarations for
   \param[in]      ou32_SignalListIndex   index of Datapool list containing signals
   \param[in]      oq_RemoveLastComma     flag to handle comma (very last signals string should not end on comma)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_ConvertSignalsToStrings(C_SclStringList & orc_Data,
                                                               const std::vector<C_OscCanSignal> & orc_Signals,
                                                               const uint32_t ou32_SignalListIndex,
                                                               const bool oq_RemoveLastComma)
{
   const uint8_t u8_SignalCount = static_cast<uint8_t>(orc_Signals.size());

   for (uint8_t u8_SignalIndex = 0U; u8_SignalIndex < u8_SignalCount; u8_SignalIndex++)
   {
      const C_OscCanSignal c_Signal = orc_Signals[u8_SignalIndex];
      const C_SclString c_String = mh_GetByteOrderNameByType(c_Signal.e_ComByteOrder);
      C_SclString c_Text;

      c_Text = "   { " + c_String + ", " + C_SclString::IntToStr(c_Signal.u16_ComBitStart) + "U, " +
               C_SclString::IntToStr(c_Signal.u16_ComBitLength) + "U, " +
               C_SclString::IntToStr(ou32_SignalListIndex) + "U, " +
               C_SclString::IntToStr(c_Signal.u32_ComDataElementIndex) + "U }";
      if ((oq_RemoveLastComma == false) || (u8_SignalIndex != (u8_SignalCount - 1U)))
      {
         c_Text += ",";
      }
      orc_Data.Append(c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Group signals by multiplex value

   Sort all signals of the given message into vectors. There is one vector for each multiplex value and one separate
    vector holding all non-mux signals. The first entry in mux case is always the multiplexer signal itself.

   \param[in]   orc_Message              CAN message data for signal information
   \param[in]   ou32_MultiplexerIndex    Index of multiplexer signal in orc_Message
   \param[out]  orc_MuxedSignalsPerValue Signals sorted by multiplexer values (see description)
   \param[out]  orc_NonMuxedSignals      Non-multiplexed signals in the message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCommunicationStack::mh_GroupSignalsByMuxValue(const C_OscCanMessage & orc_Message,
                                                              const uint32_t ou32_MultiplexerIndex, std::map<int32_t,
                                                                                                             std::vector<C_OscCanSignal> > & orc_MuxedSignalsPerValue,
                                                              std::vector<C_OscCanSignal> & orc_NonMuxedSignals)
{
   if (ou32_MultiplexerIndex < orc_Message.c_Signals.size())
   {
      const C_OscCanSignal & rc_MultiplexerSignal = orc_Message.c_Signals[ou32_MultiplexerIndex];
      std::set<uint16_t> c_MultiplexValues;
      std::set<uint16_t>::const_iterator c_ItValue;

      orc_Message.GetMultiplexerValues(c_MultiplexValues);
      for (c_ItValue = c_MultiplexValues.begin(); c_ItValue != c_MultiplexValues.end(); ++c_ItValue)
      {
         // add multiplexer signal of every multiplexer-value-group; must be first entry!
         orc_MuxedSignalsPerValue[*c_ItValue].push_back(rc_MultiplexerSignal);
      }

      for (uint8_t u8_SignalIndex = 0U; u8_SignalIndex < static_cast<uint8_t>(orc_Message.c_Signals.size());
           u8_SignalIndex++)
      {
         const C_OscCanSignal & rc_Signal = orc_Message.c_Signals[u8_SignalIndex];
         switch (rc_Signal.e_MultiplexerType)
         {
         case C_OscCanSignal::eMUX_DEFAULT:
            orc_NonMuxedSignals.push_back(rc_Signal);
            break;
         case C_OscCanSignal::eMUX_MULTIPLEXER_SIGNAL:
            // signal was already added to every vector
            break;
         case C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL:
            // if key does not already exist something went really wrong
            tgl_assert(orc_MuxedSignalsPerValue.count(rc_Signal.u16_MultiplexValue) == 1);
            orc_MuxedSignalsPerValue[rc_Signal.u16_MultiplexValue].push_back(rc_Signal);
            break;
         default:
            tgl_assert(false);
            break;
         }
      }
   }
}
