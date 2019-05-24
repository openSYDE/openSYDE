//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export communication stack settings of a openSYDE node.

   Create a .c and .h file providing entire communication stack configuration.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_OSCUtils.h"
#include "CSCLStringList.h"
#include "C_OSCExportCommunicationStack.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Constructor

   Initialize all elements with default values
*/
//-----------------------------------------------------------------------------
C_OSCExportCommunicationStack::C_OSCExportCommunicationStack(void)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//-----------------------------------------------------------------------------
C_OSCExportCommunicationStack::~C_OSCExportCommunicationStack(void)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Return filename (without extension)

   Compose target file name for code generation.

   \param[in]  ou8_InterfaceIndex      index of interface
   \param[in]  ore_ProtocolType        protocol type (CL2, ECeS, ECoS)
   \return           assembled filename
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportCommunicationStack::h_GetFileName(const uint8 ou8_InterfaceIndex,
                                                         const C_OSCCanProtocol::E_Type & ore_ProtocolType)
{
   // assemble filename
   // add data pool name + protocol name + node index
   const C_SCLString c_Text = "comm_" + mh_GetProtocolNameByType(ore_ProtocolType).LowerCase() + "_can" +
                              C_SCLString::IntToStr(ou8_InterfaceIndex + 1L);

   return c_Text;
}

//-----------------------------------------------------------------------------
/*! \brief   Return name of configuration structure

   Compose name of configuration structure for code generation.

   \param[in]  ou8_InterfaceIndex      index of interface
   \param[in]  ore_ProtocolType        protocol type (CL2, ECeS, ECoS)

   \return   assembled configuration structure name
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportCommunicationStack::h_GetConfigurationName(const uint8 ou8_InterfaceIndex,
                                                                  const C_OSCCanProtocol::E_Type & ore_ProtocolType)
{
   const C_SCLString c_Name = "gt_comm_" + mh_GetProtocolNameByType(ore_ProtocolType).LowerCase() + "_can" +
                              C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_ProtocolConfiguration";

   return c_Name;
}

//-----------------------------------------------------------------------------
/*! \brief   Create source files

   The caller must provide a valid path and node configuration.

   \param[in] orc_Path                 storage path for created files
   \param[in] orc_Node                 node configuration
   \param[in] ou8_InterfaceIndex       index of interface
   \param[in] ore_Protocol             protocol type (CL2, ECeS, ECoS)

   \return
   C_NO_ERR    Operation success
   C_RD_WR     Operation failure: cannot store files
   C_CONFIG    Protocol or data pool not available in node for interface
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportCommunicationStack::h_CreateSourceCode(const C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                         const uint8 ou8_InterfaceIndex,
                                                         const C_OSCCanProtocol::E_Type & ore_Protocol)
{
   sint32 s32_Retval;
   const C_OSCCanProtocol * const pc_ComProtocol = orc_Node.GetCANProtocolConst(ore_Protocol);
   const C_OSCNodeDataPool * const pc_DataPool = orc_Node.GetComDataPoolConst(ore_Protocol);

   if ((pc_ComProtocol != NULL) && (pc_DataPool != NULL))
   {
      uint32 u32_HashValue = 0U;
      //calculate hash value over the current state of the data pool and protocol definitions
      pc_ComProtocol->CalcHash(u32_HashValue);
      pc_DataPool->CalcHash(u32_HashValue);
      const C_SCLString c_ProjectId = C_SCLString::IntToStr(u32_HashValue);

      // create header file
      s32_Retval = mh_CreateHeaderFile(orc_Path, *pc_ComProtocol, ou8_InterfaceIndex, ore_Protocol, c_ProjectId);

      // create implementation file
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_CreateImplementationFile(orc_Path, *pc_ComProtocol, *pc_DataPool, ou8_InterfaceIndex,
                                                  ore_Protocol, c_ProjectId);
      }
   }
   else
   {
      osc_write_log_error("Creating source code",
                          "Protocol definition or data pool does not exist for specified communication protocol" +
                          mh_GetProtocolNameByType(ore_Protocol));
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Create header file

   \param[in] orc_Path                 storage path for created file
   \param[in] orc_ComProtocol          communication protocol configuration
   \param[in] ou8_InterfaceIndex       index of interface
   \param[in] ore_Protocol             protocol type (CL2, ECeS, ECoS)
   \param[in] orc_ProjectId            project id for consistency check

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportCommunicationStack::mh_CreateHeaderFile(const C_SCLString & orc_Path,
                                                          const C_OSCCanProtocol & orc_ComProtocol,
                                                          const uint8 ou8_InterfaceIndex,
                                                          const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                          const C_SCLString & orc_ProjectId)
{
   sint32 s32_Retval;
   C_SCLStringList c_Data;

   c_Data.Clear();

   // add header
   mh_AddHeader(c_Data, ou8_InterfaceIndex, ore_Protocol, mhq_IS_HEADER_FILE);

   // add includes
   c_Data.Append(
      "\n/* -- Includes ------------------------------------------------------------------------------------------------------ */");
   c_Data.Append("#include \"stwtypes.h\"");
   c_Data.Append("#include \"osy_com_configuration.h\"\n");
   c_Data.Append("#ifdef __cplusplus");
   c_Data.Append("extern \"C\" {");
   c_Data.Append("#endif\n");

   // add defines
   mh_AddDefines(c_Data, orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex], ou8_InterfaceIndex,
                 ore_Protocol, orc_ProjectId, mhq_IS_HEADER_FILE);

   // add types
   c_Data.Append(
      "/* -- Types --------------------------------------------------------------------------------------------------------- */\n");

   // add global variables
   c_Data.Append(
      "/* -- Global Variables ---------------------------------------------------------------------------------------------- */");
   c_Data.Append("///Stack configuration");
   c_Data.Append("extern const T_osy_com_protocol_configuration " +
                 h_GetConfigurationName(ou8_InterfaceIndex, ore_Protocol) + ";");
   c_Data.Append("");

   // add function prototypes
   mh_AddFunctionPrototypes(c_Data, ou8_InterfaceIndex, ore_Protocol, orc_ProjectId);

   // add implementation
   c_Data.Append(
      "/* -- Implementation ------------------------------------------------------------------------------------------------ */");
   c_Data.Append("#ifdef __cplusplus");
   c_Data.Append("} /* end of extern \"C\" */");
   c_Data.Append("#endif");
   c_Data.Append("");
   c_Data.Append("#endif");

   // finally save all stuff into the file
   s32_Retval = mh_SaveToFile(c_Data, orc_Path, ou8_InterfaceIndex, ore_Protocol, mhq_IS_HEADER_FILE);
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Create implementation file

   \param[in] orc_Path                 storage path for created file
   \param[in] orc_ComProtocol          communication protocol configuration
   \param[in] orc_DataPool             data pool configuration
   \param[in] ou8_InterfaceIndex       index of interface
   \param[in] ore_Protocol             protocol type (CL2, ECeS, ECoS)
   \param[in] orc_ProjectId            project id for consistency check

   \return
   C_NO_ERR    Operation success
   C_RD_WR     Operation failure: cannot store file
   C_CONFIG    data pool not available for interface
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportCommunicationStack::mh_CreateImplementationFile(const C_SCLString & orc_Path,
                                                                  const C_OSCCanProtocol & orc_ComProtocol,
                                                                  const C_OSCNodeDataPool & orc_DataPool,
                                                                  const uint8 ou8_InterfaceIndex,
                                                                  const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                                  const C_SCLString & orc_ProjectId)
{
   sint32 s32_Retval;
   uint32 u32_TxListIndex;
   uint32 u32_RxListIndex;

   if ((C_OSCCanProtocol::h_GetComListIndex(orc_DataPool, ou8_InterfaceIndex, true, u32_TxListIndex) == C_NO_ERR) &&
       (C_OSCCanProtocol::h_GetComListIndex(orc_DataPool, ou8_InterfaceIndex, false, u32_RxListIndex) == C_NO_ERR))
   {
      C_SCLStringList c_Data;
      const bool q_TxMessagesPresent =
         (orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex].c_TxMessages.size() > 0) ? true : false;
      const bool q_RxMessagesPresent =
         (orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex].c_RxMessages.size() > 0) ? true : false;

      // add header
      mh_AddHeader(c_Data, ou8_InterfaceIndex, ore_Protocol, mhq_IS_IMPLEMENTATION_FILE);

      // add includes
      mh_AddCIncludes(c_Data, orc_DataPool.c_Name, ou8_InterfaceIndex, ore_Protocol,
                      ((q_RxMessagesPresent == false) || (q_TxMessagesPresent == false)));

      // add defines
      mh_AddDefines(c_Data, orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex], ou8_InterfaceIndex,
                    ore_Protocol, orc_ProjectId, mhq_IS_IMPLEMENTATION_FILE);

      // add types
      c_Data.Append(
         "/* -- Types --------------------------------------------------------------------------------------------------------- */\n");

      // add module global variables and function prototypes
      mh_AddCModuleGlobal(c_Data, orc_DataPool.q_IsSafety, orc_ComProtocol.c_ComMessages[ou8_InterfaceIndex],
                          ou8_InterfaceIndex, ore_Protocol, u32_TxListIndex, u32_RxListIndex);

      // add global variables
      mh_AddCGlobalVariables(c_Data, orc_DataPool.c_Name, ou8_InterfaceIndex, ore_Protocol,
                             q_TxMessagesPresent, q_RxMessagesPresent);

      // add implementation
      c_Data.Append(
         "/* -- Implementation ------------------------------------------------------------------------------------------------ */");
      c_Data.Append("");

      // finally save all stuff into the file
      s32_Retval = mh_SaveToFile(c_Data, orc_Path, ou8_InterfaceIndex, ore_Protocol, mhq_IS_IMPLEMENTATION_FILE);
   }
   else
   {
      osc_write_log_error("Creating source code",
                          "Data pool does not exist for specified communication protocol" +
                          mh_GetProtocolNameByType(ore_Protocol) + " with interface index " +
                          C_SCLString::IntToStr(ou8_InterfaceIndex));
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Add file header

   \param[in,out] orc_Data             converted data to string list
   \param[in]     ou8_InterfaceIndex   index of interface
   \param[in]     ore_Protocol         protocol type (CL2, ECeS, ECoS)
   \param[in]     oq_FileType          .c or .h file selected
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddHeader(C_SCLStringList & orc_Data, const uint8 ou8_InterfaceIndex,
                                                 const C_OSCCanProtocol::E_Type & ore_Protocol, const bool oq_FileType)
{
   orc_Data.Append(
      "//----------------------------------------------------------------------------------------------------------------------");
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append(
         "   \\brief       openSYDE Communication stack data definition (Source file with constant definitions)");
      orc_Data.Append("");
      orc_Data.Append("   Defines the communication configuration for protocol type " +
                      mh_GetProtocolNameByType(ore_Protocol) + " on CAN interface " +
                      C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + ".");
   }
   else
   {
      orc_Data.Append(
         "   \\brief       openSYDE Communication stack data definition (Header file with constant and global definitions)");
   }
   orc_Data.Append("*/");
   orc_Data.Append(
      "//----------------------------------------------------------------------------------------------------------------------");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SCLString c_HeaderGuard = "COMM_" + mh_GetProtocolNameByType(ore_Protocol).UpperCase() + "_CAN" +
                                        C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "H";
      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Add includes into C file

   \param[in,out] orc_Data             converted data to string list
   \param[in]     orc_DataPoolName     name of data pool
   \param[in]     ou8_InterfaceIndex   index of interface
   \param[in]     ore_Protocol         protocol type (CL2, ECeS, ECoS)
   \param[in]     oq_NullRequired      true: definition of NULL required in .c file
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddCIncludes(C_SCLStringList & orc_Data, const C_SCLString & orc_DataPoolName,
                                                    const uint8 ou8_InterfaceIndex,
                                                    const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                    const bool oq_NullRequired)
{
   const C_SCLString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).LowerCase();

   orc_Data.Append(
      "\n/* -- Includes ------------------------------------------------------------------------------------------------------ */");

   if (oq_NullRequired == true)
   {
      orc_Data.Append("#include <stddef.h>");
      orc_Data.Append("");
   }

   orc_Data.Append("#include \"comm_" + c_ProtocolName + "_can" + C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) +
                   ".h\"\n");
   orc_Data.Append("#include \"osy_com_" + c_ProtocolName + "_driver.h\"\n");
   orc_Data.Append("#include \"" + orc_DataPoolName.LowerCase() + "_data_pool.h\"\n");
}

//-----------------------------------------------------------------------------
/*! \brief   Add defines

   \param[in,out] orc_Data              converted data to string list
   \param[in]     orc_ComMessage        communication protocol message definition
   \param[in]     ou8_InterfaceIndex    index of interface
   \param[in]     ore_Protocol          protocol type (CL2, ECeS, ECoS)
   \param[in]     orc_ProjectId         project id for consistency check
   \param[in]     oq_FileType           .c or .h file selected
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddDefines(C_SCLStringList & orc_Data,
                                                  const C_OSCCanMessageContainer & orc_ComMessage,
                                                  const uint8 ou8_InterfaceIndex,
                                                  const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                  const C_SCLString & orc_ProjectId, const bool oq_FileType)
{
   const C_SCLString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();
   const C_SCLString c_MagicName = "COMM_" + c_ProtocolName + "_CAN" +
                                   C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_PROJECT_ID_" + orc_ProjectId;

   orc_Data.Append(
      "/* -- Defines ------------------------------------------------------------------------------------------------------- */");

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
      orc_Data.Append("#define " + c_MagicName + " void " + c_MagicName.LowerCase() + "(void) {}");
      orc_Data.Append("");

      orc_Data.Append("///Index of TX messages");
      for (uint16 u16_MessageIndex = 0U; u16_MessageIndex < orc_ComMessage.c_TxMessages.size(); u16_MessageIndex++)
      {
         const C_SCLString c_Name = orc_ComMessage.c_TxMessages[u16_MessageIndex].c_Name.UpperCase();
         orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                         C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_TX_MSG_INDEX_" + c_Name + " (" +
                         C_SCLString::IntToStr(u16_MessageIndex) + "U)");
      }
      orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                      C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_NUMBER_OF_TX_MSGS (" +
                      C_SCLString::IntToStr(orc_ComMessage.c_TxMessages.size()) + "U)\n");

      orc_Data.Append("///Index of RX messages");
      for (uint16 u16_MessageIndex = 0U; u16_MessageIndex < orc_ComMessage.c_RxMessages.size(); u16_MessageIndex++)
      {
         const C_SCLString c_Name = orc_ComMessage.c_RxMessages[u16_MessageIndex].c_Name.UpperCase();
         orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                         C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_RX_MSG_INDEX_" + c_Name + " (" +
                         C_SCLString::IntToStr(u16_MessageIndex) + "U)");
      }
      orc_Data.Append("#define COMM_" + c_ProtocolName + "_CAN" +
                      C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_NUMBER_OF_RX_MSGS (" +
                      C_SCLString::IntToStr(orc_ComMessage.c_RxMessages.size()) + "U)\n");
   }
   else
   {
      orc_Data.Append("///ensure file consistency (if compilation fails here the .h file does not match this .c file)");
      orc_Data.Append(c_MagicName);
      orc_Data.Append("");
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Add function prototypes.

   \param[in,out] orc_Data              converted data to string list
   \param[in]     ou8_InterfaceIndex    index of interface
   \param[in]     ore_Protocol          protocol type (CL2, ECeS, ECoS)
   \param[in]     orc_ProjectId         project id for consistency check
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddFunctionPrototypes(C_SCLStringList & orc_Data, const uint8 ou8_InterfaceIndex,
                                                             const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                             const C_SCLString & orc_ProjectId)
{
   const C_SCLString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol).UpperCase();
   const C_SCLString c_MagicName = "COMM_" + c_ProtocolName + "_CAN" +
                                   C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_PROJECT_ID_" + orc_ProjectId;

   orc_Data.Append(
      "/* -- Function Prototypes ------------------------------------------------------------------------------------------- */");
   orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
   orc_Data.Append("extern void " + c_MagicName.LowerCase() + "(void);");
   orc_Data.Append("");
}

//-----------------------------------------------------------------------------
/*! \brief   Add module global variables and function prototypes

   Implementation file .C only.

   \param[in,out] orc_Data              converted data to string list
   \param[in]     oq_SafeData           true: place status data in safe RAM
   \param[in]     orc_ComMessage        communication protocol message definition
   \param[in]     ou8_InterfaceIndex    index of interface
   \param[in]     ore_Protocol          protocol type (CL2, ECeS, ECoS)
   \param[in]     ou32_TxListIndex      data pool list index for Tx messages
   \param[in]     ou32_RxListIndex      data pool list index for Rx messages
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddCModuleGlobal(C_SCLStringList & orc_Data, const bool oq_SafeData,
                                                        const C_OSCCanMessageContainer & orc_ComMessage,
                                                        const uint8 ou8_InterfaceIndex,
                                                        const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                        const uint32 ou32_TxListIndex, const uint32 ou32_RxListIndex)
{
   C_SCLString c_SafeRamData;
   C_SCLString c_Text;
   const C_SCLString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol);

   if (oq_SafeData == true)
   {
      //status data needs to be placed in safe memory as the driver deals with safety relevant data
      c_SafeRamData = "OSY_DPA_SAFE_RAM_DATA ";
   }

   orc_Data.Append(
      "/* -- Module Global Variables --------------------------------------------------------------------------------------- */");
   orc_Data.Append("///private message status");
   if (orc_ComMessage.c_TxMessages.size() > 0)
   {
      orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_status_private mat_StatusTx[COMM_" +
                      c_ProtocolName.UpperCase() + "_CAN" +
                      C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_NUMBER_OF_TX_MSGS];");
   }
   if (orc_ComMessage.c_RxMessages.size() > 0)
   {
      orc_Data.Append("static " + c_SafeRamData + "T_osy_com_message_status_private mat_StatusRx[COMM_" +
                      c_ProtocolName.UpperCase() + "_CAN" +
                      C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) + "_NUMBER_OF_RX_MSGS];");
   }

   orc_Data.Append("///Tx signal definitions");
   mh_AddSignalDefinitions(ou32_TxListIndex, orc_ComMessage.c_TxMessages, orc_Data);

   orc_Data.Append("///Rx signal definitions");
   mh_AddSignalDefinitions(ou32_RxListIndex, orc_ComMessage.c_RxMessages, orc_Data);

   if (orc_ComMessage.c_TxMessages.size() > 0)
   {
      const uint16 u16_MessageCount = static_cast<uint16>(orc_ComMessage.c_TxMessages.size());
      orc_Data.Append("///Tx message definitions");
      orc_Data.Append("static const T_osy_com_message_definition mat_MessagesTx[COMM_" +
                      c_ProtocolName.UpperCase() + "_CAN" + C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) +
                      "_NUMBER_OF_TX_MSGS] =");
      orc_Data.Append("{");
      for (uint16 u16_MessageIndex = 0U; u16_MessageIndex < u16_MessageCount; u16_MessageIndex++)
      {
         const C_OSCCanMessage & rc_Message = orc_ComMessage.c_TxMessages[u16_MessageIndex];
         const C_SCLString c_Trigger = mh_GetTransmissionTriggerNameByType(rc_Message.e_TxMethod);
         //delay time is only applicable for change triggered transmissions
         const uint16 u16_DelayTime = (rc_Message.e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE) ?
                                      rc_Message.u16_DelayTimeMs : 0U;
         uint16 u16_Dlc = rc_Message.u16_Dlc;

         if (ore_Protocol == C_OSCCanProtocol::eECES)
         {
            u16_Dlc -= 2U; //Tool: message DLC; target: payload only
         }

         c_Text = "   { 0x" +
                  C_SCLString::IntToHex(static_cast<sint64>(rc_Message.u32_CanId), 3U) + "U, " +
                  C_SCLString::IntToStr(rc_Message.q_IsExtended) + "U, " +
                  C_SCLString::IntToStr(u16_Dlc) + "U, " + c_Trigger + ", " +
                  C_SCLString::IntToStr(rc_Message.c_Signals.size()) + "U, " +
                  C_SCLString::IntToStr(u16_DelayTime) + "U, " +
                  C_SCLString::IntToStr(rc_Message.u32_CycleTimeMs) + "U, " +
                  "0U, &mat_" + rc_Message.c_Name + "[0] }"; //message counter gap not relevant for TX messages
         if (u16_MessageIndex != (u16_MessageCount - 1U))
         {
            c_Text += ",";
         }
         c_Text += ("  ///" + C_OSCUtils::h_NiceifyStringForCComment(rc_Message.c_Comment));
         orc_Data.Append(c_Text);
      }
      orc_Data.Append("};");
      orc_Data.Append("");
   }

   if (orc_ComMessage.c_RxMessages.size() > 0)
   {
      const uint16 u16_MessageCount = static_cast<uint16>(orc_ComMessage.c_RxMessages.size());
      orc_Data.Append("///Rx message definitions");
      orc_Data.Append("static const T_osy_com_message_definition mat_MessagesRx[COMM_" +
                      c_ProtocolName.UpperCase() + "_CAN" + C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) +
                      "_NUMBER_OF_RX_MSGS] =");
      orc_Data.Append("{");
      for (uint16 u16_MessageIndex = 0U; u16_MessageIndex < u16_MessageCount; u16_MessageIndex++)
      {
         const C_OSCCanMessage & rc_Message = orc_ComMessage.c_RxMessages[u16_MessageIndex];
         const C_SCLString c_Trigger = mh_GetTransmissionTriggerNameByType(rc_Message.e_TxMethod);
         uint32 u32_MessageCounterGap = 0U;
         uint16 u16_Dlc = rc_Message.u16_Dlc;

         if (ore_Protocol == C_OSCCanProtocol::eECES)
         {
            if (rc_Message.u32_CycleTimeMs != 0)
            {
               u32_MessageCounterGap = ((rc_Message.u32_TimeoutMs / rc_Message.u32_CycleTimeMs) - 1U);
            }
            else
            {
               u32_MessageCounterGap = 0U;
            }
            u16_Dlc -= 2U; //Tool: message DLC; target: payload only
         }

         c_Text = "   { 0x" +
                  C_SCLString::IntToHex(static_cast<sint64>(rc_Message.u32_CanId), 3U) + "U, " +
                  C_SCLString::IntToStr(rc_Message.q_IsExtended) + "U, " +
                  C_SCLString::IntToStr(u16_Dlc) + "U, " + c_Trigger + ", " +
                  C_SCLString::IntToStr(rc_Message.c_Signals.size()) + "U, " +
                  "0U, " + //delay time not used for TX messages
                  C_SCLString::IntToStr(rc_Message.u32_TimeoutMs) + "U, " +
                  C_SCLString::IntToStr(u32_MessageCounterGap) + "U, &mat_" + rc_Message.c_Name +
                  "[0] }";
         if (u16_MessageIndex != (u16_MessageCount - 1U))
         {
            c_Text += ",";
         }
         c_Text += ("  ///" + C_OSCUtils::h_NiceifyStringForCComment(rc_Message.c_Comment));
         orc_Data.Append(c_Text);
      }
      orc_Data.Append("};");
      orc_Data.Append("");
   }

   orc_Data.Append(
      "/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */\n");
}

//-----------------------------------------------------------------------------
/*! \brief   Add global variables

   \param[in,out] orc_Data              converted data to string list
   \param[in]     orc_DataPoolName      name of data pool
   \param[in]     orc_ComMessage        communication protocol message definition
   \param[in]     ou8_InterfaceIndex    index of interface
   \param[in]     ore_Protocol          protocol type (CL2, ECeS, ECoS)
   \param[in]     oq_TxMessagesPresent  true: There is at least one TX message defined
   \param[in]     oq_RxMessagesPresent  true: There is at least one RX message defined
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddCGlobalVariables(C_SCLStringList & orc_Data,
                                                           const C_SCLString & orc_DataPoolName,
                                                           const uint8 ou8_InterfaceIndex,
                                                           const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                           const bool oq_TxMessagesPresent,
                                                           const bool oq_RxMessagesPresent)
{
   const C_SCLString c_ProtocolName = mh_GetProtocolNameByType(ore_Protocol);

   orc_Data.Append(
      "/* -- Global Variables ---------------------------------------------------------------------------------------------- */");
   orc_Data.Append("///Stack configuration");
   orc_Data.Append("const T_osy_com_protocol_configuration " +
                   h_GetConfigurationName(ou8_InterfaceIndex, ore_Protocol) + " =");
   orc_Data.Append("{");
   orc_Data.Append("   " + C_SCLString::IntToStr(ou8_InterfaceIndex) + "U,  ///< selected CAN channel");
   orc_Data.Append("   OSY_COM_PROTOCOL_TYPE_" + c_ProtocolName.UpperCase() + ",  ///< protocol type for this stack"
                   );
   orc_Data.Append("   {");
   orc_Data.Append("      OSY_COM_CONFIG_MAGIC,  ///< identification of valid comm configuration");
   orc_Data.Append("      OSY_COM_CONFIG_DEFINITION_VERSION,  ///< configuration version");
   orc_Data.Append("      COMM_" + c_ProtocolName.UpperCase() + "_CAN" +
                   C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) +
                   "_NUMBER_OF_TX_MSGS,  ///< number of TX messages in this configuration");
   orc_Data.Append("      COMM_" + c_ProtocolName.UpperCase() + "_CAN" +
                   C_SCLString::IntToStr(ou8_InterfaceIndex + 1L) +
                   "_NUMBER_OF_RX_MSGS,  ///< number of RX messages in this configuration");
   if (oq_TxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_MessagesTx[0],  ///< definition of TX messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< definition of TX messages (none defined)");
   }
   if (oq_RxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_MessagesRx[0],  ///< definition of RX messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< definition of RX messages (none defined)");
   }
   if (oq_TxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_StatusTx[0],  ///< private status of TX messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< private status of TX messages (none defined)");
   }
   if (oq_RxMessagesPresent == true)
   {
      orc_Data.Append("      &mat_StatusRx[0],  ///< private status of RX messages");
   }
   else
   {
      orc_Data.Append("      NULL,  ///< private status of RX messages (none defined)");
   }
   orc_Data.Append("      &gt_" + orc_DataPoolName +
                   "_DataPool  ///< data pool containing signal values");
   orc_Data.Append("   },");
   orc_Data.Append("   OSY_COM_" + c_ProtocolName.UpperCase() + "_DRIVER  ///< pointer to protocol specific driver");
   orc_Data.Append("};");
   orc_Data.Append("");
}

//-----------------------------------------------------------------------------
/*! \brief   Store assembled data in file

   \param[in] orc_Data                 converted data to string list
   \param[in] orc_Path                 storage path for created file
   \param[in] ou8_InterfaceIndex       index of interface
   \param[in] ore_Protocol             protocol type (CL2, ECeS, ECoS)
   \param[in] oq_FileType              .c or .h file selected

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store file
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportCommunicationStack::mh_SaveToFile(C_SCLStringList & orc_Data, const C_SCLString & orc_Path,
                                                    const uint8 ou8_InterfaceIndex,
                                                    const C_OSCCanProtocol::E_Type & ore_Protocol,
                                                    const bool oq_FileType)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_PathAndFilename;
   C_SCLString c_FileName;

   // assemble filename
   c_FileName = h_GetFileName(ou8_InterfaceIndex, ore_Protocol);

   // assemble path and filename
   // add path + add filename + extension
   c_PathAndFilename = orc_Path + "/" + c_FileName;

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      c_PathAndFilename += ".h";
   }
   else
   {
      c_PathAndFilename += ".c";
   }

   // store into file
   try
   {
      orc_Data.SaveToFile(c_PathAndFilename);
   }
   catch (...)
   {
      osc_write_log_error("Creating source code", "Could not write to file \"" + c_PathAndFilename + "\"");
      s32_Retval = C_RD_WR;
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Get string of protocol name by protocol type

   \param[in]  ore_Protocol            protocol type (CL2, ECeS, ECoS)

   \return
   protocol name as string
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportCommunicationStack::mh_GetProtocolNameByType(const C_OSCCanProtocol::E_Type & ore_Protocol)
{
   C_SCLString c_Name;

   switch (ore_Protocol)
   {
   case C_OSCCanProtocol::eLAYER2:
      c_Name = "cl2";
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      c_Name = "ecos";
      break;
   case C_OSCCanProtocol::eECES:
      c_Name = "eces";
      break;
   default:
      break;
   }

   return c_Name;
}

//-----------------------------------------------------------------------------
/*! \brief   Get string of byte order by byte order type

   \param[in]  ore_ByteOrder           byte order type (Intel, Motorola)

   \return
   byte order as string
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportCommunicationStack::mh_GetByteOrderNameByType(
   const C_OSCCanSignal::E_ByteOrderType & ore_ByteOrder)
{
   C_SCLString c_Name;

   switch (ore_ByteOrder)
   {
   case C_OSCCanSignal::eBYTE_ORDER_INTEL:
      c_Name = "OSY_COM_BYTE_ORDER_LITTLE";
      break;
   case C_OSCCanSignal::eBYTE_ORDER_MOTOROLA:
      c_Name = "OSY_COM_BYTE_ORDER_BIG";
      break;
   default:
      break;
   }

   return c_Name;
}

//-----------------------------------------------------------------------------
/*! \brief   Get string of transmission trigger by transmission trigger type

   \param[in]  ore_Trigger           transmission trigger type (cyclic, change)

   \return
   transmission trigger as string
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCExportCommunicationStack::mh_GetTransmissionTriggerNameByType(
   const C_OSCCanMessage::E_TxMethodType & ore_Trigger)
{
   C_SCLString c_Name;

   switch (ore_Trigger)
   {
   case C_OSCCanMessage::eTX_METHOD_CYCLIC:
      c_Name = "OSY_COM_COMM_METHOD_CYCLIC";
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_CHANGE:
      c_Name = "OSY_COM_COMM_METHOD_ON_CHANGE";
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_EVENT:
      c_Name = "OSY_COM_COMM_METHOD_ON_EVENT";
      break;
   default:
      break;
   }

   return c_Name;
}

//-----------------------------------------------------------------------------
/*! \brief   Add signal definition section to C file

   \param[in]      ou32_SignalListIndex   index of data pool list containing signals
   \param[in]      orc_Messages           definition of messages
   \param[in,out]  orc_Data               list of strings to add to
*/
//-----------------------------------------------------------------------------
void C_OSCExportCommunicationStack::mh_AddSignalDefinitions(const uint32 ou32_SignalListIndex,
                                                            const std::vector<C_OSCCanMessage> & orc_Messages,
                                                            C_SCLStringList & orc_Data)
{
   for (uint16 u16_MessageIndex = 0U; u16_MessageIndex < orc_Messages.size(); u16_MessageIndex++)
   {
      const uint8 u8_SignalCount = static_cast<uint8>(orc_Messages[u16_MessageIndex].c_Signals.size());
      orc_Data.Append("static const T_osy_com_signal_definition mat_" + orc_Messages[u16_MessageIndex].c_Name +
                      "[" + C_SCLString::IntToStr(
                         u8_SignalCount) + "] =");
      orc_Data.Append("{");
      for (uint8 u8_SignalIndex = 0U; u8_SignalIndex < u8_SignalCount; u8_SignalIndex++)
      {
         const C_OSCCanSignal c_Signal = orc_Messages[u16_MessageIndex].c_Signals[u8_SignalIndex];
         const C_SCLString c_String = mh_GetByteOrderNameByType(c_Signal.e_ComByteOrder);
         C_SCLString c_Text;

         c_Text = "   { " + c_String + ", " + C_SCLString::IntToStr(c_Signal.u16_ComBitStart) + "U, " +
                  C_SCLString::IntToStr(c_Signal.u16_ComBitLength) + "U, " +
                  C_SCLString::IntToStr(ou32_SignalListIndex) + "U, " +
                  C_SCLString::IntToStr(c_Signal.u32_ComDataElementIndex) + "U }";
         if (u8_SignalIndex != (u8_SignalCount - 1U))
         {
            c_Text += ",";
         }
         orc_Data.Append(c_Text);
      }
      orc_Data.Append("};");
      orc_Data.Append("");
   }
}
