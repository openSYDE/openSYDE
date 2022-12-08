//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export CANopen configuration for CANopen manager

   Create a .c and .h file providing entire CANopen configuration.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <algorithm> //for sort

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscExportCanOpenConfig.hpp"
#include "C_OscExportUti.hpp"
#include "C_OscExportDataPool.hpp"
#include "TglUtils.hpp"
#include "C_OscLoggingHandler.hpp"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscExportCanOpenConciseEntry::C_OscExportCanOpenConciseEntry() :
   u16_Index(0),
   u8_SubIndex(0),
   c_Comment("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor: creates a ConciseEntry object with given parameters

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       orc_Payload        storage for payload
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscExportCanOpenConciseEntry::C_OscExportCanOpenConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                                               const std::vector<uint8_t> & orc_Payload,
                                                               const C_SclString & orc_Comment) :
   u16_Index(ou16_Index),
   u8_SubIndex(ou8_SubIndex),
   c_Payload(orc_Payload),
   c_Comment(orc_Comment)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscExportCanOpenConciseEntry::~C_OscExportCanOpenConciseEntry()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the given values on a existing ConciseEntry object

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       orc_Payload        storage for payload
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConciseEntry::SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                                     const std::vector<uint8_t> & orc_Payload,
                                                     const C_SclString & orc_Comment)
{
   u16_Index = ou16_Index;
   u8_SubIndex = ou8_SubIndex;
   c_Payload = orc_Payload;
   c_Comment = orc_Comment;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the given values on a existing ConciseEntry object (source: uint8 variable)

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       ou8_Value          value to set
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConciseEntry::SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                                     const uint8_t ou8_Value, const stw::scl::C_SclString & orc_Comment)
{
   std::vector<uint8_t> c_ThePayload;
   c_ThePayload.push_back(ou8_Value);

   this->SetConciseEntry(ou16_Index, ou8_SubIndex, c_ThePayload, orc_Comment);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the given values on a existing ConciseEntry object (source: uint16 variable)

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       ou16_Value         value to set
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConciseEntry::SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                                     const uint16_t ou16_Value,
                                                     const stw::scl::C_SclString & orc_Comment)
{
   std::vector<uint8_t> c_ThePayload;
   c_ThePayload.push_back(static_cast<uint8_t>(ou16_Value));
   c_ThePayload.push_back(static_cast<uint8_t>(ou16_Value >> 8U));

   this->SetConciseEntry(ou16_Index, ou8_SubIndex, c_ThePayload, orc_Comment);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the given values on a existing ConciseEntry object (source: uint32 variable)

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       ou32_Value         value to set
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConciseEntry::SetConciseEntry(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                                     const uint32_t ou32_Value,
                                                     const stw::scl::C_SclString & orc_Comment)
{
   std::vector<uint8_t> c_ThePayload;
   c_ThePayload.push_back(static_cast<uint8_t>(ou32_Value));
   c_ThePayload.push_back(static_cast<uint8_t>(ou32_Value >> 8U));
   c_ThePayload.push_back(static_cast<uint8_t>(ou32_Value >> 16U));
   c_ThePayload.push_back(static_cast<uint8_t>(ou32_Value >> 24U));

   this->SetConciseEntry(ou16_Index, ou8_SubIndex, c_ThePayload, orc_Comment);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Converts ConciseEntry data to string without comment

   \return string containing all data from ConciseEntry object
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCanOpenConciseEntry::GetConciseString(void) const
{
   const uint32_t u32_PayloadSize = static_cast<uint32_t>(this->c_Payload.size());
   C_SclString c_Retval = "0x" + C_SclString::IntToHex(static_cast<uint8_t>(this->u16_Index), 2U).UpperCase() + "U, " +
                          "0x" +
                          C_SclString::IntToHex(static_cast<uint8_t>(this->u16_Index >> 8U), 2U).UpperCase() + "U, " +
                          "0x" + C_SclString::IntToHex(this->u8_SubIndex, 2U).UpperCase() + "U, " +
                          "0x" + C_SclString::IntToHex(u32_PayloadSize, 2U).UpperCase() + "U, " +
                          "0x" + C_SclString::IntToHex(u32_PayloadSize >> 8U, 2U).UpperCase() + "U, " +
                          "0x" + C_SclString::IntToHex(u32_PayloadSize >> 16U, 2U).UpperCase() + "U, " +
                          "0x" + C_SclString::IntToHex(u32_PayloadSize >> 24U, 2U).UpperCase() + "U, ";

   C_SclString c_PayloadString;

   for (uint8_t u8_It = 0; u8_It < c_Payload.size(); ++u8_It)
   {
      c_PayloadString += "0x" + C_SclString::IntToHex(c_Payload[u8_It], 2).UpperCase() + "U, ";
   }
   //remove last comma + whitespace
   c_PayloadString.Delete(c_PayloadString.LastPos(","), 2U);
   c_Retval += c_PayloadString;

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Composes string which contains information about how many concise entries are listed in a concise array

   \param[in]       ou32_NumOfEntries     Detailed input parameter description

   \return Num of entries in bytes + comment
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCanOpenConciseEntry::h_GetNumOfEntriesString(const uint32_t ou32_NumOfEntries)
{
   return "0x" + C_SclString::IntToHex(static_cast<uint8_t>(ou32_NumOfEntries), 2U).UpperCase() + "U, " +
          "0x" + C_SclString::IntToHex(static_cast<uint8_t>(ou32_NumOfEntries >> 8U), 2U).UpperCase() + "U, " +
          "0x" + C_SclString::IntToHex(static_cast<uint8_t>(ou32_NumOfEntries >> 16U), 2U).UpperCase() + "U, " +
          "0x" + C_SclString::IntToHex(static_cast<uint8_t>(ou32_NumOfEntries >> 24U), 2U).UpperCase() + "U, " +
          "                                                 ///< Number of entries";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  For each CANopen Device we need to know the size of it's concise data array. This function calculates it.

   \param[in]       orc_ConciseEntries     vector of all concise entries

   \return array size
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscExportCanOpenConciseEntry::h_GetConciseArraySize(
   std::vector<C_OscExportCanOpenConciseEntry> & orc_ConciseEntries)
{
   uint32_t u32_Retval;
   uint32_t u32_PayloadByteCount = 0;

   //get count of all payload bytes for all entries
   for (uint32_t u32_EntryIt = 0; u32_EntryIt < orc_ConciseEntries.size(); ++u32_EntryIt)
   {
      u32_PayloadByteCount += static_cast<uint32_t>(orc_ConciseEntries[u32_EntryIt].c_Payload.size());
   }

   //formula:
   // 4[num_entries] + (num_of_entries * (2[index]+ 1[subindex] + 4[size_of_entry])) +
   // payload_byte_count_over_all_entries
   u32_Retval = static_cast<uint32_t>(4U + (orc_ConciseEntries.size() * (2U + 1U + 4U)) + u32_PayloadByteCount);

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return file name without extension.

   Detailed function description (optional). The function shall be described detailed if it is not described completely
   by short function description and parameter description.

   \param[in]  ou8_InterfaceIndex   index of interface

   \return     assembled file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCanOpenConfig::h_GetFileName(const uint8_t ou8_InterfaceIndex)
{
   //assemble file name
   //add interface index
   const C_SclString c_Text = "osco_man_config_can" +
                              C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U);

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert overall code structure version to process data communication code subversion.

   The overall code generation version decodes the format versions of parametrization (*_datapool.c/.h files)
   and process data communication code (comm_*.c/.h files).

   \param[in]       ou16_GenCodeVersion     Overall code format version

   \return version of code
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscExportCanOpenConfig::h_ConvertOverallCodeVersion(const uint16_t ou16_GenCodeVersion)
{
   uint16_t u16_Return;

   //Technically CANopen code is "compatible" with overall versions up to 6. So don't impose any artificial limits here.
   if ((ou16_GenCodeVersion > 0) && (ou16_GenCodeVersion <= 5U))
   {
      u16_Return = 1U;
   }
   else if ((ou16_GenCodeVersion > 0) && (ou16_GenCodeVersion <= 6U))
   {
      u16_Return = 2U;
   }
   else
   {
      // should never occur...
      tgl_assert(false);
      u16_Return = 0U;
   }

   return u16_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create source files

   Caller must provide valid node configuration and path

   \param[in]       orc_Path                 storage path for created files
   \param[in]       orc_Node                 node configuration
   \param[in]       ou16_ApplicationIndex    index of programmable application within orc_Node.c_Applications
   \param[in]       ou8_InterfaceIndex       index of CAN interface
   \param[in]       ou32_DatapoolIndex       index of datapool
   \param[in]       orc_ExportToolInfo       information about calling executable (name + version)

   \return
   C_NO_ERR    success
   C_NOACT     application is not of type ePROGRAMMABLE_APPLICATION or has unknown code structure version
   C_RD_WR     failure: cannot store files
   C_CONFIG    protocol or Datapool not available in node for interface or application index out of range
   C_RANGE     application index out of range

*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportCanOpenConfig::h_CreateSourceCode(const C_SclString & orc_Path, const C_OscNode & orc_Node,
                                                     const uint16_t ou16_ApplicationIndex,
                                                     const uint8_t ou8_InterfaceIndex,
                                                     const uint32_t ou32_DatapoolIndex,
                                                     const C_SclString & orc_ExportToolInfo)
{
   int32_t s32_Return = C_NO_ERR;
   C_OscNodeApplication c_Application;

   if (ou16_ApplicationIndex < orc_Node.c_Applications.size())
   {
      c_Application = orc_Node.c_Applications[ou16_ApplicationIndex];
   }
   else
   {
      osc_write_log_error("Creating source code", "Application index " + C_SclString::IntToStr(
                             ou16_ApplicationIndex) + "out of range.");
      s32_Return = C_RANGE;
   }

   if (s32_Return == C_NO_ERR)
   {
      //make sure application is programmable
      if (c_Application.e_Type != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         osc_write_log_error("Creating source code",
                             "Did not generate code for application \"" + c_Application.c_Name +
                             "\" because application is not programmable.");
         s32_Return = C_NOACT;
      }
      else
      {
         //make sure version is known
         if (c_Application.u16_GenCodeVersion > C_OscNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
         {
            osc_write_log_error("Creating source code",
                                "Did not generate code for application \"" + c_Application.c_Name +
                                "\" because code format version \"" +
                                C_SclString::IntToStr(c_Application.u16_GenCodeVersion) + "\" is unknown.");
            s32_Return = C_NOACT;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      const C_OscCanProtocol * const pc_Protocol = orc_Node.GetCanProtocolConst(C_OscCanProtocol::eCAN_OPEN,
                                                                                ou32_DatapoolIndex);
      const C_OscNodeDataPool * const pc_DataPool = orc_Node.GetComDataPoolConst(C_OscCanProtocol::eCAN_OPEN,
                                                                                 ou32_DatapoolIndex);

      //even if we have no signals there should be an "empty" DP
      if (pc_DataPool != NULL)
      {
         const C_OscCanMessageContainer & rc_Messages = pc_Protocol->c_ComMessages[ou8_InterfaceIndex];
         uint32_t u32_HashValue = 0U;
         //calculate hash value over the current state of the Datapool and protocol definitions
         pc_Protocol->CalcHash(u32_HashValue);
         pc_DataPool->CalcHash(u32_HashValue);
         const C_SclString c_ProjectId = C_SclString::IntToStr(u32_HashValue);

         //create header file
         s32_Return = mh_CreateHeaderFile(orc_ExportToolInfo, orc_Path, orc_Node, rc_Messages,
                                          c_Application, ou8_InterfaceIndex, c_ProjectId);

         if (s32_Return == C_NO_ERR)
         {
            //create implementation file
            s32_Return = mh_CreateImplementationFile(orc_ExportToolInfo, orc_Path, orc_Node, rc_Messages,
                                                     c_Application, *pc_DataPool, ou8_InterfaceIndex,
                                                     c_ProjectId);
         }
      }
      else
      {
         osc_write_log_error("Creating source code",
                             "Datapool does not exist for specified communication protocol CANopen");
         s32_Return = C_CONFIG;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  create .h file

   \param[in]       orc_ExportInfoTool    information about calling executable (name + version)
   \param[in]       orc_Path              storage path for created file
   \param[in]       orc_Node              node configuration
   \param[in]       orc_MsgContainer      communication protocol message definition
   \param[in]       orc_Application       application data
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       orc_ProjectId         project id for consistency check

   \return
   C_NO_ERR    success
   C_RD_WR     cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportCanOpenConfig::mh_CreateHeaderFile(const C_SclString & orc_ExportInfoTool,
                                                      const C_SclString & orc_Path, const C_OscNode & orc_Node,
                                                      const C_OscCanMessageContainer & orc_MsgContainer,
                                                      const C_OscNodeApplication & orc_Application,
                                                      const uint8_t ou8_InterfaceIndex,
                                                      const C_SclString & orc_ProjectId)
{
   int32_t s32_Return;
   C_SclStringList c_Data;

   c_Data.Clear();

   mh_AddHeader(orc_ExportInfoTool, c_Data, ou8_InterfaceIndex, mhq_IS_HEADER_FILE);

   //add includes
   c_Data.Append("");
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Includes"));
   c_Data.Append("#include \"osco_configuration.h\"");
   c_Data.Append("");
   C_OscExportUti::h_AddExternCeStart(c_Data);

   //add defines
   mh_AddDefines(c_Data, orc_Node, orc_MsgContainer, ou8_InterfaceIndex, orc_ProjectId,
                 orc_Application.u16_GenCodeVersion, mhq_IS_HEADER_FILE);

   //add types
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   //add global variables
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Global Variables"));
   c_Data.Append("///Stack configuration");
   c_Data.Append("extern const T_osco_man_manager_configuration gt_osco_man_can" +
                 C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_ProtocolConfiguration;");
   c_Data.Append("");

   //add function prototypes
   C_OscExportUti::h_AddProjIdFunctionPrototype(c_Data, mh_GetMagicName(orc_ProjectId, ou8_InterfaceIndex));

   //add implementation
   c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Implementation"));
   C_OscExportUti::h_AddExternCeEnd(c_Data);
   c_Data.Append("#endif");

   //save all this
   s32_Return = C_OscExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(ou8_InterfaceIndex), true);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create .c file

   \param[in]       orc_ExportInfoTool    information about calling executable (name + version)
   \param[in]       orc_Path              storage path for created file
   \param[in]       orc_Node              node configuration
   \param[in]       orc_MsgContainer      communication protocol message definition
   \param[in]       orc_Application       application data
   \param[in]       orc_Datapool          Datapool configuration
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       orc_ProjectId         project id for consistency check

   \return
   C_NO_ERR    success
   C_CONFIG    invalid configuration, aborted
   C_RD_WR     cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscExportCanOpenConfig::mh_CreateImplementationFile(const C_SclString & orc_ExportInfoTool,
                                                              const C_SclString & orc_Path, const C_OscNode & orc_Node,
                                                              const C_OscCanMessageContainer & orc_MsgContainer,
                                                              const C_OscNodeApplication & orc_Application,
                                                              const C_OscNodeDataPool & orc_Datapool,
                                                              const uint8_t ou8_InterfaceIndex,
                                                              const C_SclString & orc_ProjectId)
{
   int32_t s32_Return = C_CONFIG;

   C_SclStringList c_Data;

   uint32_t u32_TxListIndex;
   uint32_t u32_RxListIndex;

   //the signals lists are expected to exist even if there are no signals defined
   if ((C_OscCanProtocol::h_GetComListIndex(orc_Datapool, ou8_InterfaceIndex, true, u32_TxListIndex) == C_NO_ERR) &&
       (C_OscCanProtocol::h_GetComListIndex(orc_Datapool, ou8_InterfaceIndex, false, u32_RxListIndex) == C_NO_ERR))
   {
      c_Data.Clear();
      const C_OscNodeDataPoolList rc_DatapoolTxList = orc_Datapool.c_Lists[u32_TxListIndex];
      const C_OscNodeDataPoolList rc_DatapoolRxList = orc_Datapool.c_Lists[u32_RxListIndex];
      //add documentation header
      mh_AddHeader(orc_ExportInfoTool, c_Data, ou8_InterfaceIndex, mhq_IS_IMPLEMENTATION_FILE);

      //add includes
      //The Datapool will only exist if there is at least one signal, otherwise we will not refer to it.
      //We have signals if we have at least one active message (as we do not support messages without signals for
      // CANopen):
      mh_AddIncludes(c_Data, orc_Datapool, ou8_InterfaceIndex, orc_MsgContainer.ContainsAtLeastOneActiveMessage());

      //add defines
      mh_AddDefines(c_Data, orc_Node, orc_MsgContainer, ou8_InterfaceIndex, orc_ProjectId,
                    orc_Application.u16_GenCodeVersion, mhq_IS_IMPLEMENTATION_FILE);

      //add types
      c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Types"));
      c_Data.Append("");

      //add module global variables
      c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Module Global Variables"));
      c_Data.Append("");

      //add module global function prototypes
      c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
      c_Data.Append("");

      //add global variables
      mh_AddGlobalVariables(c_Data, orc_Datapool.c_Name, orc_Node, orc_MsgContainer, ou8_InterfaceIndex,
                            rc_DatapoolTxList, rc_DatapoolRxList, u32_TxListIndex, u32_RxListIndex, C_OscExportCanOpenConfig::h_ConvertOverallCodeVersion(
                               orc_Application.u16_GenCodeVersion));

      //add implementation
      c_Data.Append(C_OscExportUti::h_GetSectionSeparator("Implementation"));

      //save all this
      s32_Return = C_OscExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(ou8_InterfaceIndex), false);
   }
   else
   {
      osc_write_log_error("Creating source code",
                          "Datapool RX or TX signal list does not exist for specified communication protocol CANopen");
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add file documentation header

   \param[in]      orc_ExportToolInfo     information about calling executable (name + version)
   \param[in,out]  orc_Data               converted data to string list
   \param[in]      ou8_InterfaceIndex     index of interface
   \param[in]      oq_FileType            .c or .h file selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddHeader(const C_SclString & orc_ExportToolInfo, C_SclStringList & orc_Data,
                                            const uint8_t ou8_InterfaceIndex, const bool oq_FileType)
{
   orc_Data.Append(C_OscExportUti::h_GetHeaderSeparator());
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append(
         "   \\brief       openSYDE CANopen Manager data definition (Source file with constant definitions)");
      orc_Data.Append("");
      orc_Data.Append("   Defines the communication configuration for protocol type CANopen on CAN interface " +
                      C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + ".");
      orc_Data.Append("");
      orc_Data.Append(C_OscExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   else
   {
      orc_Data.Append(
         "   \\brief       openSYDE CANopen Manager data definition (Header file with constant and global definitions)");
      orc_Data.Append("");
      orc_Data.Append(C_OscExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   orc_Data.Append("*/");
   orc_Data.Append(C_OscExportUti::h_GetHeaderSeparator());

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SclString c_HeaderGuard = h_GetFileName(ou8_InterfaceIndex).UpperCase() + "H";
      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add includes into C file.

   \param[in,out]  orc_Data               converted data to string list
   \param[in]      orc_Datapool           name of datapool
   \param[in]      ou8_InterfaceIndex     index of interface
   \param[in]      oq_IncludeDataPool     include header for referenced data pool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddIncludes(C_SclStringList & orc_Data, const C_OscNodeDataPool & orc_Datapool,
                                              const uint8_t ou8_InterfaceIndex, const bool oq_IncludeDataPool)
{
   orc_Data.Append("");
   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Includes"));

   orc_Data.Append("#include <stddef.h>");
   orc_Data.Append("");

   orc_Data.Append("#include \"osco_man_config_can" + C_SclString::IntToStr(
                      static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + ".h\"");

   orc_Data.Append("#include \"stwtypes.h\"");
   orc_Data.Append("");

   if (oq_IncludeDataPool == true)
   {
      orc_Data.Append("#include \"" + C_OscExportDataPool::h_GetFileName(orc_Datapool) + ".h\"");
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add defines

   \param[in,out]  orc_Data              converted data to string list
   \param[in]      orc_Node              node configuration
   \param[in]      orc_MsgContainer      communication protocol message definition
   \param[in]      ou8_InterfaceIndex    index of interface
   \param[in]      orc_ProjectId         project id for consistency check
   \param[in]      ou16_GenCodeVersion   version of structure (generate code as specific for this version)
   \param[in]      oq_FileType           .c or .h file generated
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddDefines(C_SclStringList & orc_Data, const C_OscNode & orc_Node,
                                             const C_OscCanMessageContainer & orc_MsgContainer,
                                             const uint8_t ou8_InterfaceIndex, const C_SclString & orc_ProjectId,
                                             const uint16_t ou16_GenCodeVersion, const bool oq_FileType)
{
   const C_SclString c_MagicName = mh_GetMagicName(orc_ProjectId, ou8_InterfaceIndex);

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Defines"));

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      //needed for define name (see below)
      uint32_t u32_DevCnt = 0;
      uint32_t u32_TxPdoCnt = 0;
      uint32_t u32_RxPdoCnt = 0;
      const C_SclString c_InterfaceIndex = C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U);

      //get the CANopen manager info via interface index
      const std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_Iterator =
         orc_Node.c_CanOpenManagers.find(ou8_InterfaceIndex);
      //Sanity check: the manager for the specified interface should be available:
      tgl_assert(c_Iterator != orc_Node.c_CanOpenManagers.end());

      const C_OscCanOpenManagerInfo c_OscoManInfo = c_Iterator->second;
      std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_DeviceIt;

      orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_PROJECT_ID_" + orc_ProjectId +
                      " void osco_man_can" + c_InterfaceIndex + "_project_id_" + orc_ProjectId +
                      "(void) {}");
      orc_Data.Append("");

      for (c_DeviceIt = c_OscoManInfo.c_CanOpenDevices.begin(); c_DeviceIt != c_OscoManInfo.c_CanOpenDevices.end();
           ++c_DeviceIt)
      {
         orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_DEVICE" + C_SclString::IntToStr(
                            u32_DevCnt + 1U) + " (" + C_SclString::IntToStr(u32_DevCnt) + "U)");
         u32_DevCnt++;
      }

      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_DEVICES (" +
                      C_SclString::IntToStr(c_OscoManInfo.c_CanOpenDevices.size()) + "U)");
      orc_Data.Append("");
      orc_Data.Append("///Index of TX PDOs");

      for (uint16_t u16_MsgIndex = 0U; u16_MsgIndex < orc_MsgContainer.c_TxMessages.size(); ++u16_MsgIndex)
      {
         if (orc_MsgContainer.c_TxMessages[u16_MsgIndex].q_CanOpenManagerMessageActive == true)
         {
            const C_SclString c_MsgName = orc_MsgContainer.c_TxMessages[u16_MsgIndex].c_Name.UpperCase();

            //count active TX PDO(s) that contain signals
            if (orc_MsgContainer.c_TxMessages[u16_MsgIndex].c_Signals.size() > 0)
            {
               u32_TxPdoCnt++;
            }

            if (u32_TxPdoCnt > 0)
            {
               orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_TX_PDO_INDEX_" + c_MsgName + " (" + C_SclString::IntToStr(
                                  u32_TxPdoCnt - 1U) + "U)");
            }
         }
      }
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_TX_PDOS (" +
                      C_SclString::IntToStr(u32_TxPdoCnt) + "U)");

      //number of TX signals
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_TX_SIGNALS (" +
                      C_SclString::IntToStr(mh_GetTotalNumOfPdoSignals(orc_MsgContainer, true)) + "U)");
      orc_Data.Append("");
      orc_Data.Append("///Index of RX PDOs");

      for (uint16_t u16_MsgIndex = 0U; u16_MsgIndex < orc_MsgContainer.c_RxMessages.size(); ++u16_MsgIndex)
      {
         if (orc_MsgContainer.c_RxMessages[u16_MsgIndex].q_CanOpenManagerMessageActive == true)
         {
            const C_SclString c_MsgName = orc_MsgContainer.c_RxMessages[u16_MsgIndex].c_Name.UpperCase();

            //count active RX PDO(s) that contain signals
            if (orc_MsgContainer.c_RxMessages[u16_MsgIndex].c_Signals.size() > 0)
            {
               u32_RxPdoCnt++;
            }

            if (u32_RxPdoCnt > 0)
            {
               orc_Data.Append(
                  "#define OSCO_MAN_CAN" + c_InterfaceIndex + "_RX_PDO_INDEX_" + c_MsgName + " (" +
                  C_SclString::IntToStr(u32_RxPdoCnt - 1U) + "U)");
            }
         }
      }
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_RX_PDOS (" +
                      C_SclString::IntToStr(u32_RxPdoCnt) + "U)");

      //number of RX signals
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_RX_SIGNALS (" +
                      C_SclString::IntToStr(mh_GetTotalNumOfPdoSignals(orc_MsgContainer, false)) + "U)");
   }
   else
   {
      if (ou16_GenCodeVersion >= 1U)
      {
         orc_Data.Append("///check for correct version of structure definitions");
         orc_Data.Append("#if OSCO_MAN_CONFIG_DEFINITION_VERSION != 0x" +
                         C_SclString::IntToHex(static_cast<int64_t>(C_OscExportCanOpenConfig::
                                                                    h_ConvertOverallCodeVersion(
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
/*! \brief  Add global variables

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_DatapoolName      name of datapool which holds these PDO(s)
   \param[in]       orc_Node              node configuration
   \param[in]       orc_MsgContainer      communication protocol message definition
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       orc_DatapoolTxList    specific Datapool list
   \param[in]       orc_DatapoolRxList    specific Datapool list
   \param[in]       ou32_TxListIndex      index of Datapool list
   \param[in]       ou32_RxListIndex      index of Datapool list
   \param[in]       ou16_GenCodeVersion   converted version of code generation. Caller is responsible for conversion
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddGlobalVariables(C_SclStringList & orc_Data, const C_SclString & orc_DatapoolName,
                                                     const C_OscNode & orc_Node,
                                                     const C_OscCanMessageContainer & orc_MsgContainer,
                                                     const uint8_t ou8_InterfaceIndex,
                                                     const C_OscNodeDataPoolList & orc_DatapoolTxList,
                                                     const C_OscNodeDataPoolList & orc_DatapoolRxList,
                                                     const uint32_t ou32_TxListIndex, const uint32_t ou32_RxListIndex,
                                                     const uint16_t ou16_GenCodeVersion)
{
   uint32_t u32_ManagerInstance;
   uint16_t u16_MsgIt;
   bool q_AtLeastOneTxActiveWithSignals = false;
   bool q_AtLeastOneRxActiveWithSignals = false;

   //check if there are any active TX PDO(s)
   for (u16_MsgIt = 0; u16_MsgIt < orc_MsgContainer.c_TxMessages.size(); ++u16_MsgIt)
   {
      if ((orc_MsgContainer.c_TxMessages[u16_MsgIt].q_CanOpenManagerMessageActive == true) &&
          (orc_MsgContainer.c_TxMessages[u16_MsgIt].c_Signals.empty() == false))
      {
         q_AtLeastOneTxActiveWithSignals = true;
         break;
      }
   }

   //check if there are any active RX PDO(s) with signals
   for (u16_MsgIt = 0; u16_MsgIt < orc_MsgContainer.c_RxMessages.size(); ++u16_MsgIt)
   {
      if ((orc_MsgContainer.c_RxMessages[u16_MsgIt].q_CanOpenManagerMessageActive == true) &&
          (orc_MsgContainer.c_RxMessages[u16_MsgIt].c_Signals.empty() == false))
      {
         q_AtLeastOneRxActiveWithSignals = true;
         break;
      }
   }

   std::vector<uint32_t> c_ConcArraySizes;
   //get the CANopen manager info via interface index
   const std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_Iterator =
      orc_Node.c_CanOpenManagers.find(ou8_InterfaceIndex);
   const C_OscCanOpenManagerInfo c_ManInfo = c_Iterator->second;

   orc_Data.Append(C_OscExportUti::h_GetSectionSeparator("Global Variables"));
   orc_Data.Append("");

   if (q_AtLeastOneTxActiveWithSignals)
   {
      orc_Data.Append("///Tx signal definitions");
      mh_AddSignalDefinitions(orc_Data, orc_DatapoolTxList, ou32_TxListIndex, orc_MsgContainer.c_TxMessages);
   }
   if (q_AtLeastOneRxActiveWithSignals)
   {
      orc_Data.Append("///Rx signal definitions");
      mh_AddSignalDefinitions(orc_Data, orc_DatapoolRxList, ou32_RxListIndex, orc_MsgContainer.c_RxMessages);
   }

   //if list is empty or the list only contains INACTIVE PDO(s) entry is not generated
   if (q_AtLeastOneTxActiveWithSignals)
   {
      orc_Data.Append("///Tx PDO definitions");
      mh_AddPdoDefinitions(orc_Data, orc_MsgContainer.c_TxMessages, ou8_InterfaceIndex, true, true);
      orc_Data.Append("");
   }
   //if list is empty or the list only contains INACTIVE PDO(s) entry is not generated
   if (q_AtLeastOneRxActiveWithSignals)
   {
      orc_Data.Append("///Rx PDO definitions");
      mh_AddPdoDefinitions(orc_Data, orc_MsgContainer.c_RxMessages, ou8_InterfaceIndex, false, true);
      orc_Data.Append("");
   }

   orc_Data.Append("");

   //Add section with concise data for each CANopen manager sub-device
   c_ConcArraySizes = mh_AddDeviceSpecificConciseData(orc_Data, orc_Node, orc_MsgContainer, ou8_InterfaceIndex);

   //Add section with device settings
   mh_AddDeviceSettings(orc_Data, c_ConcArraySizes, c_ManInfo, ou8_InterfaceIndex, ou16_GenCodeVersion);

   //Add last section with CANopen manager configuration
   u32_ManagerInstance = static_cast<uint32_t>(std::distance(orc_Node.c_CanOpenManagers.begin(), c_Iterator));
   mh_AddManagerConfig(orc_Data, orc_DatapoolName, c_ManInfo, u32_ManagerInstance, ou8_InterfaceIndex,
                       q_AtLeastOneTxActiveWithSignals, q_AtLeastOneRxActiveWithSignals,
                       orc_MsgContainer.ContainsAtLeastOneActiveMessage(), ou16_GenCodeVersion);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add signal definition section to .c file

   \param[in,out]   orc_Data                 list of strings to add
   \param[in]       orc_DatapoolList         specific Datapool list
   \param[in]       ou32_SignalListIndex     index of Datapool list containing the signals
   \param[in]       orc_Messages             vector of messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddSignalDefinitions(C_SclStringList & orc_Data,
                                                       const C_OscNodeDataPoolList & orc_DatapoolList,
                                                       const uint32_t ou32_SignalListIndex,
                                                       const std::vector<C_OscCanMessage> & orc_Messages)
{
   for (uint16_t u16_MsgIndex = 0; u16_MsgIndex < orc_Messages.size(); ++u16_MsgIndex)
   {
      const C_OscCanMessage & rc_CurrentMsg = orc_Messages[u16_MsgIndex];

      //only compose array if PDO is active and contains signals
      if (rc_CurrentMsg.q_CanOpenManagerMessageActive == true)
      {
         const std::vector<C_OscCanSignal> & rc_CurrentSignalList = orc_Messages[u16_MsgIndex].c_Signals;
         if (rc_CurrentSignalList.size() > 0)
         {
            orc_Data.Append("static const T_osco_man_pdo_signal_definition mat_Pdo_" +
                            rc_CurrentMsg.c_Name + "[" + rc_CurrentSignalList.size() + "] =");
            orc_Data.Append("{");
            mh_ConvertSignalsToStrings(orc_Data, orc_DatapoolList, rc_CurrentMsg.c_Signals, ou32_SignalListIndex, true);
            orc_Data.Append("};");
         }
         orc_Data.Append("");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add message definition to C file

   \param[in,out]   orc_Data             converted data to string list
   \param[in]       orc_Messages         vector of messages
   \param[in]       ou8_InterfaceIndex   index of interface
   \param[in]       oq_IsTx              True: PDO is Tx
   \param[in]       oq_RemoveLastComma    True: comma is removed (signals string should not end on comma)

*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddPdoDefinitions(C_SclStringList & orc_Data,
                                                    const std::vector<C_OscCanMessage> & orc_Messages,
                                                    const uint8_t ou8_InterfaceIndex, const bool oq_IsTx,
                                                    const bool oq_RemoveLastComma)
{
   C_SclString c_Define;
   C_SclString c_TxOrRx;

   //generate define and part of struct name depending on TX or RX
   if (oq_IsTx)
   {
      c_Define = "[OSCO_MAN_CAN" + C_SclString::IntToStr(
         static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_TX_PDOS]";
      c_TxOrRx = "Tx";
   }
   else
   {
      c_Define = "[OSCO_MAN_CAN" + C_SclString::IntToStr(
         static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_RX_PDOS]";
      c_TxOrRx = "Rx";
   }

   orc_Data.Append("static const T_osco_man_pdo_definition mat_Pdo" + c_TxOrRx + "Definition" + c_Define + " =");
   orc_Data.Append("{");
   for (uint16_t u16_MsgIndex = 0; u16_MsgIndex < orc_Messages.size(); ++u16_MsgIndex)
   {
      //in this context message means PDO
      const C_OscCanMessage rc_CurrentMessage = orc_Messages[u16_MsgIndex];

      //only active PDO(s) with signals are considered
      if ((rc_CurrentMessage.q_CanOpenManagerMessageActive == true) &&
          (rc_CurrentMessage.c_Signals.empty() == false))
      {
         const C_SclString c_IdExt = rc_CurrentMessage.q_IsExtended ? "1U" : "0U";

         //EventTime depends on whether PDO is TX or RX
         const C_SclString c_EventTime =
            oq_IsTx ? C_SclString::IntToStr(rc_CurrentMessage.u32_CycleTimeMs) : C_SclString::IntToStr(
               rc_CurrentMessage.u32_TimeoutMs);

         const C_SclString c_StructName = "&mat_Pdo_" + rc_CurrentMessage.c_Name + "[0]";
         C_SclString c_TransmissionType;
         C_SclString c_Text;

         tgl_assert(rc_CurrentMessage.u16_DelayTimeMs <= 6553U);
         //we need a resolution of 100 micro-seconds on embedded side
         const uint16_t u16_InhibitTime = static_cast<uint16_t>(rc_CurrentMessage.u16_DelayTimeMs * 10U);

         //check transmission type, no version check needed here, because the structure remains the same.
         //if content derives from code version, generated code will cause compiler error
         if ((rc_CurrentMessage.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254) ||
             (rc_CurrentMessage.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255))
         {
            c_TransmissionType = "OSCO_MAN_MANAGER_COMM_METHOD_CYCLIC_ASYNC_MANUFA_SPEC";
         }
         else if (rc_CurrentMessage.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0)
         {
            c_TransmissionType = "OSCO_MAN_MANAGER_COMM_METHOD_SYNC_AFTER_NEXT_SYNC_AND_CHANGE";
         }
         else if (rc_CurrentMessage.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240)
         {
            c_TransmissionType = "OSCO_MAN_MANAGER_COMM_METHOD_SYNC_AFTER_X_SYNC + " +
                                 C_SclString::IntToStr(static_cast<uint32_t>(rc_CurrentMessage.
                                                                             u8_CanOpenTxMethodAdditionalInfo)) + "U";
         }
         else
         {
            //nothing to do here
         }

         //array contains: COB-ID, COB-ID Extension, COMM-Method, InhibitTime, EventTimer, NumSignals, pointer to
         //first element of array "signal definitions".
         //See code example in /doc/file_specifications/Generated_C_Code/CANopenManager_Definition
         c_Text = "   { 0x" + C_SclString::IntToHex(rc_CurrentMessage.u32_CanId, 3U) + "U, " + c_IdExt +
                  ", " + c_TransmissionType + ", " +
                  C_SclString::IntToStr(static_cast<uint32_t>(u16_InhibitTime)) + "U, " +
                  c_EventTime + "U, " + C_SclString::IntToStr(rc_CurrentMessage.c_Signals.size()) + "U, " +
                  c_StructName + " }";

         if ((oq_RemoveLastComma == false) || (u16_MsgIndex < (orc_Messages.size() - 1U)))
         {
            c_Text += ",";
         }
         orc_Data.Append(c_Text);
      }
   }
   orc_Data.Append("};");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds section with device specific concise data to C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_Node              node configuration
   \param[in]       orc_MsgContainer      communication protocol message definition
   \param[in]       ou8_InterfaceIndex    index of interface

   \return vector with size of concise array for every device
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_OscExportCanOpenConfig::mh_AddDeviceSpecificConciseData(C_SclStringList & orc_Data,
                                                                                const C_OscNode & orc_Node,
                                                                                const C_OscCanMessageContainer & orc_MsgContainer,
                                                                                const uint8_t ou8_InterfaceIndex)
{
   std::vector<uint32_t> c_Retval;
   //get the CANopen manager info via interface index
   const std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_Iterator =
      orc_Node.c_CanOpenManagers.find(ou8_InterfaceIndex);
   C_OscCanOpenManagerInfo c_ManInfo = c_Iterator->second;

   if (c_ManInfo.c_CanOpenDevices.size() > 0)
   {
      uint32_t u32_DevCnt = 1;

      std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_DeviceIt;

      //we collect and then generate concise data for each CANopen device
      for (c_DeviceIt = c_ManInfo.c_CanOpenDevices.begin(); c_DeviceIt != c_ManInfo.c_CanOpenDevices.end();
           ++c_DeviceIt)
      {
         uint32_t u32_ArraySize;
         C_SclString c_NumOfEntries;
         const C_OscCanOpenManagerDeviceInfo & rc_CurrentDevice = c_DeviceIt->second;
         const uint32_t u32_NodeIndex = c_DeviceIt->first.u32_NodeIndex;
         std::vector<C_OscExportCanOpenConciseEntry> c_ConciseEntries;

         mh_CollectDeviceSpecificConciseData(c_ConciseEntries, rc_CurrentDevice, c_ManInfo.u8_NodeIdValue,
                                             c_ManInfo.u32_SyncCyclePeriodUs, c_ManInfo.u32_SyncWindowLengthUs);
         //Tx Pdos (= Rx Pdos of device)
         mh_CollectPdoConciseData(c_ConciseEntries, rc_CurrentDevice, u32_NodeIndex, orc_MsgContainer, false);
         //Rx Pdos (= Tx Pdos of device)
         mh_CollectPdoConciseData(c_ConciseEntries, rc_CurrentDevice, u32_NodeIndex, orc_MsgContainer, true);

         u32_ArraySize = C_OscExportCanOpenConciseEntry::h_GetConciseArraySize(c_ConciseEntries);
         c_Retval.push_back(u32_ArraySize);

         orc_Data.Append("static const uint8 mau8_Device" + C_SclString::IntToStr(u32_DevCnt) +
                         "ConciseDcf[" + C_SclString::IntToStr(u32_ArraySize) + "] =");
         orc_Data.Append("{");

         //the number of all entries needs to be in first place
         c_NumOfEntries =
            C_OscExportCanOpenConciseEntry::h_GetNumOfEntriesString(static_cast<uint32_t>(c_ConciseEntries.size()));
         orc_Data.Append("   " + c_NumOfEntries);

         //unpack ConciseEntries and convert everything to string
         for (uint32_t u32_ConciseIt = 0; u32_ConciseIt < c_ConciseEntries.size(); ++u32_ConciseIt)
         {
            C_SclString c_Entry = c_ConciseEntries[u32_ConciseIt].GetConciseString();
            const uint32_t u32_PayloadSize = static_cast<uint32_t>(c_ConciseEntries[u32_ConciseIt].c_Payload.size());

            //add comma until last entry has reached
            if (u32_ConciseIt < static_cast<uint32_t>(c_ConciseEntries.size() - 1U))
            {
               c_Entry += ",";
            }
            else
            {
               c_Entry += " ";
            }

            //beautify with right indentation for comment
            switch (u32_PayloadSize)
            {
            case 1U:
               c_Entry += "                      ";
               break;
            case 2U:
               c_Entry += "               ";
               break;
            case 4U:
               c_Entry += " ";
               break;
            default:
               //nothing to do here
               break;
            }

            //add comment to end of line
            c_Entry += "///< " + c_ConciseEntries[u32_ConciseIt].c_Comment;
            orc_Data.Append("   " + c_Entry);
         }

         orc_Data.Append("};");
         //at this point the concise array for one device is finished
         u32_DevCnt++;
      }
      orc_Data.Append("");
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add section for device settings to C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_ConcSizes         vector with sizes of all concise data for each device
   \param[in]       orc_ManInfo           CANopen manager configuration
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       ou16_GenCodeVersion   converted version of code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddDeviceSettings(C_SclStringList & orc_Data,
                                                    const std::vector<uint32_t> & orc_ConcSizes,
                                                    const C_OscCanOpenManagerInfo & orc_ManInfo,
                                                    const uint8_t ou8_InterfaceIndex,
                                                    const uint16_t ou16_GenCodeVersion)
{
   if (orc_ManInfo.c_CanOpenDevices.size() > 0)
   {
      std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_DeviceIt;
      //this iterator is needed to remove the last comma in loop below
      std::map<C_OscCanInterfaceId,
               C_OscCanOpenManagerDeviceInfo>::const_iterator c_Abort = orc_ManInfo.c_CanOpenDevices.end();
      uint32_t u32_DevCnt = 1U;

      orc_Data.Append("static const T_osco_man_devices_settings mat_DeviceSettings[OSCO_MAN_CAN" +
                      C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_DEVICES] =");
      orc_Data.Append("{");
      --c_Abort;

      for (c_DeviceIt = orc_ManInfo.c_CanOpenDevices.begin(); c_DeviceIt != orc_ManInfo.c_CanOpenDevices.end();
           ++c_DeviceIt)
      {
         //u32_DevCount starts with one, but to access vector we need 0-based
         const uint32_t u32_Index = u32_DevCnt - 1U;
         const C_OscCanOpenManagerDeviceInfo rc_CurrentDevice = c_DeviceIt->second;
         const C_SclString c_OptionalDev = rc_CurrentDevice.q_DeviceOptional ? "1U" : "0U";
         const C_SclString c_NoInit = rc_CurrentDevice.q_NoInitialization ? "1U" : "0U";
         const C_SclString c_HbProducerTime = rc_CurrentDevice.q_EnableHeartbeatProducing ? C_SclString::IntToStr(
            static_cast<uint32_t>(rc_CurrentDevice.u16_HeartbeatProducerTimeMs)) + "U" : "0U";
         C_SclString c_Text;

         //in code version == 1, there is no FactorySetting
         if (ou16_GenCodeVersion < 2)
         {
            c_Text = "   { " + C_SclString::IntToStr(static_cast<uint32_t>(rc_CurrentDevice.u8_NodeIdValue)) + "U, " +
                     c_OptionalDev + ", " + c_NoInit + ", " + c_HbProducerTime + ", " +
                     C_SclString::IntToStr(orc_ConcSizes[u32_Index]) + "U, " + "&mau8_Device" +
                     C_SclString::IntToStr(u32_DevCnt) + "ConciseDcf[0] }";
         }
         else
         {
            C_SclString c_FactorySettingsSub;
            //FactorySettings = active -> write actual value into array
            if (rc_CurrentDevice.q_FactorySettingsActive == true)
            {
               c_FactorySettingsSub =
                  C_SclString::IntToStr(static_cast<uint32_t>(rc_CurrentDevice.u8_ResetNodeObjectDictionarySubIndex)) +
                  "U";
            }
            else
            {
               c_FactorySettingsSub = "0U"; //disabled
            }
            c_Text = "   { " + C_SclString::IntToStr(static_cast<uint32_t>(rc_CurrentDevice.u8_NodeIdValue)) + "U, " +
                     c_OptionalDev + ", " + c_NoInit + ", " + c_FactorySettingsSub + ", " + c_HbProducerTime + ", " +
                     C_SclString::IntToStr(orc_ConcSizes[u32_Index]) + "U, " + "&mau8_Device" +
                     C_SclString::IntToStr(u32_DevCnt) + "ConciseDcf[0] }";
         }

         if (c_DeviceIt != c_Abort)
         {
            c_Text += ",";
         }
         orc_Data.Append(c_Text);

         u32_DevCnt++;
      }
      orc_Data.Append("};");
      orc_Data.Append("");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds section with CANopen manager configuration to C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_DatapoolName      name of datapool which holds these PDO(s)
   \param[in]       orc_ManInfo           CANopen manager definition
   \param[in]       ou32_ManInstance      number of instance for CANopen manager
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       oq_TxExists           at least one TX PDO is existent
   \param[in]       oq_RxExists           at least one RX PDO is existent
   \param[in]       oq_ReferenceDataPool  true: reference DP; otherwise reference NULL
   \param[in]       ou16_GenCodeVersion   converted version of code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_AddManagerConfig(C_SclStringList & orc_Data, const C_SclString & orc_DatapoolName,
                                                   const C_OscCanOpenManagerInfo & orc_ManInfo,
                                                   const uint32_t ou32_ManInstance, const uint8_t ou8_InterfaceIndex,
                                                   const bool oq_TxExists, const bool oq_RxExists,
                                                   const bool oq_ReferenceDataPool, const uint16_t ou16_GenCodeVersion)
{
   orc_Data.Append("const T_osco_man_manager_configuration gt_osco_man_can" +
                   C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_ProtocolConfiguration =");
   orc_Data.Append("{");
   orc_Data.Append("   " + C_SclString::IntToStr(
                      static_cast<uint32_t>(ou8_InterfaceIndex)) + "U, ///< selected CAN channel");
   orc_Data.Append("   " + C_SclString::IntToStr(ou32_ManInstance) + "U, ///< CANopen Manager instance");
   orc_Data.Append("   {");

   //creates T_osco_man_manager_settings
   mh_CreateManSettings(orc_Data, orc_ManInfo, ou16_GenCodeVersion);

   orc_Data.Append("   },");
   orc_Data.Append("   OSCO_MAN_CAN" + C_SclString::IntToStr(
                      static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_DEVICES, ///< Number of Devices");

   if (orc_ManInfo.c_CanOpenDevices.size() > 0)
   {
      orc_Data.Append("   &mat_DeviceSettings[0],");
   }
   else
   {
      orc_Data.Append("   NULL,  ///< No devices assigned");
   }
   orc_Data.Append("   {");

   //creates T_osco_man_pdo_configuration
   mh_CreatePdoConfig(orc_Data, orc_DatapoolName, ou8_InterfaceIndex, oq_TxExists, oq_RxExists, oq_ReferenceDataPool);

   orc_Data.Append("   }");
   orc_Data.Append("};");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Given a configuration of a CANopen manager device, this function collects all necessary data to create
            concise data section except for the PDO data.

   \param[in,out]   orc_ConciseEntries   storage for all concise entries
   \param[in]       orc_DeviceInfo       CANopen manager device configuration
   \param[in]       ou8_ManagerId        CANopen manager node ID
   \param[in]       ou32_CyclePeriod     CANopen manager cycle period
   \param[in]       ou32_WindowLength    CANopen manager window length
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_CollectDeviceSpecificConciseData(
   std::vector<C_OscExportCanOpenConciseEntry> & orc_ConciseEntries,
   const C_OscCanOpenManagerDeviceInfo & orc_DeviceInfo, const uint8_t ou8_ManagerId, const uint32_t ou32_CyclePeriod,
   const uint32_t ou32_WindowLength)
{
   const C_OscCanOpenObjectDictionary c_Od = orc_DeviceInfo.c_EdsFileContent;
   const C_OscCanOpenObject * pc_OdObject;
   C_OscExportCanOpenConciseEntry c_Entry = C_OscExportCanOpenConciseEntry();
   C_SclString c_Comment;

   //SYNC (_538)
   pc_OdObject = c_Od.GetCanOpenObject(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_SYNC);

   //write if the objects exists and is writable:
   if ((pc_OdObject != NULL) && (pc_OdObject->IsWriteable() == true))
   {
      const uint32_t u32_VALUE = 0x80U;
      c_Entry.SetConciseEntry(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_SYNC, 0x00U, u32_VALUE, "SYNC Message");
      orc_ConciseEntries.push_back(c_Entry);
   }

   //COMMUNICATION CYCLE PERIOD (_539)
   pc_OdObject = c_Od.GetCanOpenObject(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_CYCLE_PERIOD);

   //write if the objects exists and is writable:
   if ((pc_OdObject != NULL) && (pc_OdObject->IsWriteable() == true))
   {
      c_Entry.SetConciseEntry(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_CYCLE_PERIOD, 0x00U, ou32_CyclePeriod,
                              "Communication Cycle Period in us");
      orc_ConciseEntries.push_back(c_Entry);
   }

   //SYNC WINDOW LENGTH (_540)
   pc_OdObject = c_Od.GetCanOpenObject(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_SYNC_WINDOW_LENGTH);

   //write if the objects exists and is writable:
   if ((pc_OdObject != NULL) && (pc_OdObject->IsWriteable() == true))
   {
      c_Entry.SetConciseEntry(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_SYNC_WINDOW_LENGTH, 0x00U, ou32_WindowLength,
                              "Sync Window Length in us");
      orc_ConciseEntries.push_back(c_Entry);
   }

   //EMCY (_526)
   pc_OdObject = c_Od.GetCanOpenObject(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_EMCY);

   //write if the objects exists and is writable:
   if ((pc_OdObject != NULL) && (pc_OdObject->IsWriteable() == true))
   {
      c_Entry.SetConciseEntry(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_EMCY, 0x00U,
                              static_cast<uint32_t>(orc_DeviceInfo.u8_NodeIdValue) + 0x80U,
                              "EMCY supported");
      orc_ConciseEntries.push_back(c_Entry);
   }

   //Heartbeat Consumer (_522)
   const uint8_t u8_NumHbConsumer = c_Od.GetNumHeartbeatConsumers();
   if (u8_NumHbConsumer > 0U) //at least one HB consumer supported ?
   {
      for (uint8_t u8_It = 1U; u8_It <= u8_NumHbConsumer; ++u8_It)
      {
         pc_OdObject = c_Od.GetCanOpenSubIndexObject(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_CONSUMER,
                                                     u8_It);
         if ((pc_OdObject != NULL) && (pc_OdObject->IsWriteable() == true))
         {
            uint32_t u32_Value;

            //for first consumer we write a specific value if HB consumption is activated, all others value = 0
            if ((u8_It == 1U) && (orc_DeviceInfo.q_EnableHeartbeatConsuming == true))
            {
               //object value: bit 0-15 HB time, 16-23 node id, 24-31 reserved with "0"
               u32_Value = orc_DeviceInfo.u16_HeartbeatConsumerTimeMs +
                           ((static_cast<uint32_t>(ou8_ManagerId) << 16U));
            }
            else
            {
               u32_Value = 0U;
            }

            c_Comment = "Set Heartbeat Consumer " + C_SclString::IntToStr(u8_It);
            c_Entry.SetConciseEntry(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_CONSUMER, u8_It, u32_Value,
                                    c_Comment);
            orc_ConciseEntries.push_back(c_Entry);
         }
      }
   }

   //Heartbeat Producer (_524)
   if (c_Od.IsHeartbeatProducerSupported() == true)
   {
      pc_OdObject = c_Od.GetCanOpenObject(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_PRODUCER);
      if ((pc_OdObject != NULL) && (pc_OdObject->IsWriteable() == true))
      {
         uint16_t u16_Value;

         if (orc_DeviceInfo.q_EnableHeartbeatProducing)
         {
            c_Comment = "Enable Heartbeat Producing";
            u16_Value = orc_DeviceInfo.u16_HeartbeatProducerTimeMs;
         }
         else
         {
            c_Comment = "Disable Heartbeat Producing";
            u16_Value = 0U;
         }
         c_Entry.SetConciseEntry(C_OscCanOpenObjectDictionary::hu16_OD_INDEX_HEARTBEAT_PRODUCER, 0x00U, u16_Value,
                                 c_Comment);
         orc_ConciseEntries.push_back(c_Entry);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Given a configuration of a CANopen manager device, this function collect all TX and RX PDO specific
            information

   \param[in,out]   orc_ConciseEntries   storage for all concise entries
   \param[in]       orc_DeviceInfo       CANopen manager device configuration
   \param[in]       ou32_NodeIndex       node index of current CANopen manager device (needed to match PDOs)
   \param[in]       orc_MsgContainer     communication protocol message definition
   \param[in]       oq_IsTx              True: collect data for TX PDOs (seen from device's perspective)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_CollectPdoConciseData(
   std::vector<C_OscExportCanOpenConciseEntry> & orc_ConciseEntries,
   const C_OscCanOpenManagerDeviceInfo & orc_DeviceInfo, const uint32_t ou32_NodeIndex,
   const C_OscCanMessageContainer & orc_MsgContainer, const bool oq_IsTx)
{
   //for device's TX PDOs we need to look at the manager's RX PDOs and vice versa
   std::vector<C_OscCanMessage> c_Pdos = oq_IsTx ? orc_MsgContainer.c_RxMessages : orc_MsgContainer.c_TxMessages;

   //iterate over RX or TX PDOS (depends on flag) of current device and push info into array
   for (uint16_t u16_PdoIndex = 0; u16_PdoIndex < c_Pdos.size(); ++u16_PdoIndex)
   {
      const C_OscCanMessage rc_CurrentPdo = c_Pdos[u16_PdoIndex];

      //is this PDO related to the device we generate concise data for ?
      if (rc_CurrentPdo.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex == ou32_NodeIndex)
      {
         uint16_t u16_ObjectIndex;
         int32_t s32_Return;
         C_SclString c_PdoCommentText;
         const C_OscCanOpenObjectDictionary c_OD = orc_DeviceInfo.c_EdsFileContent;
         C_OscExportCanOpenConciseEntry c_Entry = C_OscExportCanOpenConciseEntry();
         bool q_CobIdIsRo;

         //initialize variables that depend on PDO direction:
         if (oq_IsTx)
         {
            u16_ObjectIndex = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO;
            c_PdoCommentText = "TPDO ";
         }
         else
         {
            u16_ObjectIndex = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO;
            c_PdoCommentText = "RPDO ";
         }
         u16_ObjectIndex += rc_CurrentPdo.u16_CanOpenManagerPdoIndex;
         c_PdoCommentText +=
            C_SclString::IntToStr(static_cast<uint32_t>(rc_CurrentPdo.u16_CanOpenManagerPdoIndex) + 1U);

         //check if COB-ID is missing or RO
         s32_Return = c_OD.IsCobIdRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_CobIdIsRo);
         if (s32_Return != C_NO_ERR)
         {
            //if there is no entry we cannot assume it is writable
            q_CobIdIsRo = true;
         }

         if (q_CobIdIsRo == false)
         {
            //disable PDO before changing settings
            uint32_t u32_CanId = rc_CurrentPdo.u32_CanId | 0x80000000UL;

            if (oq_IsTx)
            {
               //set flag to define this COB as "non-RTR" (some devices do not support RTR and would complain otherwise)
               u32_CanId |= 0x40000000UL;
            }

            if (rc_CurrentPdo.q_IsExtended)
            {
               u32_CanId |= 0x20000000UL;
            }

            //Disable PDO
            c_Entry.SetConciseEntry(u16_ObjectIndex, 0x01, u32_CanId, "Disable " + c_PdoCommentText);
            orc_ConciseEntries.push_back(c_Entry);
         }

         //only continue writing concise entries if PDO is active (_521)
         if (rc_CurrentPdo.q_CanOpenManagerMessageActive == true)
         {
            bool q_SectionRo;
            //check if transmission type is missing or RO
            s32_Return = c_OD.IsTransmissionTypeRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRo);

            // _525
            if ((s32_Return == C_NO_ERR) && (q_SectionRo == false))
            {
               uint8_t u8_TxMethodType = 0U;
               switch (rc_CurrentPdo.e_TxMethod)
               {
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0:
                  u8_TxMethodType = 0x00U;
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240:
                  u8_TxMethodType = rc_CurrentPdo.u8_CanOpenTxMethodAdditionalInfo;
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254:
                  u8_TxMethodType = 0xFEU;
                  break;
               case C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255:
                  u8_TxMethodType = 0xFFU;
                  break;
               case C_OscCanMessage::eTX_METHOD_CYCLIC:
               case C_OscCanMessage::eTX_METHOD_ON_CHANGE:
               case C_OscCanMessage::eTX_METHOD_ON_EVENT:
               default:
                  //not supported for CANopen
                  tgl_assert(false);
                  break;
               }

               //Set transmission type
               c_Entry.SetConciseEntry(u16_ObjectIndex,
                                       C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE,
                                       u8_TxMethodType,
                                       "Set transmission type " + c_PdoCommentText);
               orc_ConciseEntries.push_back(c_Entry);
            }

            //check if inhibit time is missing or RO
            s32_Return = c_OD.IsInhibitTimeRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRo);

            if ((s32_Return == C_NO_ERR) && (q_SectionRo == false))
            {
               //resolution of 100 micro second is needed for CANopen stack
               const uint16_t u16_InhibitTimeMicro = static_cast<uint16_t>(rc_CurrentPdo.u16_DelayTimeMs * 10U);

               //Set inhibit time
               c_Entry.SetConciseEntry(u16_ObjectIndex, C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME,
                                       u16_InhibitTimeMicro,
                                       "Set inhibit time " + c_PdoCommentText);
               orc_ConciseEntries.push_back(c_Entry);
            }

            //check if event time is missing or RO
            s32_Return = c_OD.IsEventTimerRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRo);

            if ((s32_Return == C_NO_ERR) && (q_SectionRo == false))
            {
               uint16_t u16_EventTime;

               //out of range timeouts really should be prevented before trying to generate code:
               if (oq_IsTx == true)
               {
                  //Event-Time on TX PDOs = TX interval
                  tgl_assert(rc_CurrentPdo.u32_CycleTimeMs <= 0xFFFFU);
                  u16_EventTime = static_cast<uint16_t>(rc_CurrentPdo.u32_CycleTimeMs);
               }
               else
               {
                  //Event-Time on RX PDOs = timeout value
                  tgl_assert(rc_CurrentPdo.u32_TimeoutMs <= 0xFFFFU);
                  u16_EventTime = static_cast<uint16_t>(rc_CurrentPdo.u32_TimeoutMs);
               }

               //Set event time
               c_Entry.SetConciseEntry(u16_ObjectIndex, C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER,
                                       u16_EventTime, "Set event time " + c_PdoCommentText);
               orc_ConciseEntries.push_back(c_Entry);
            }

            //sub-index 0x06 is not used by RxPDOs
            if (oq_IsTx)
            {
               //check if sync start value is missing or RO (_525)
               s32_Return = c_OD.IsSyncStartRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRo);

               if ((s32_Return == C_NO_ERR) && (q_SectionRo == false))
               {
                  //explicitly write "0" if the device does support sub06
                  const uint8_t u8_SYNC_START_VALUE = 0U;
                  c_Entry.SetConciseEntry(u16_ObjectIndex,
                                          C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_SYNC_START_VALUE,
                                          u8_SYNC_START_VALUE, "Sync start value " + c_PdoCommentText);
                  orc_ConciseEntries.push_back(c_Entry);
               }
            }

            //for PDO mapping entries, we need to break the index pattern here
            if (oq_IsTx)
            {
               u16_ObjectIndex = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO_MAPPING;
            }
            else
            {
               u16_ObjectIndex = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO_MAPPING;
            }

            u16_ObjectIndex += rc_CurrentPdo.u16_CanOpenManagerPdoIndex;

            //check if PDO mapping is missing or RO
            s32_Return = c_OD.IsPdoMappingRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRo);

            //mapping can only be changed if the COB-ID is writable; otherwise the PDO is still active and cannot be
            // remapped
            if ((s32_Return == C_NO_ERR) && (q_SectionRo == false) && (q_CobIdIsRo == false))
            {
               //Clear PDO mapping
               c_Entry.SetConciseEntry(u16_ObjectIndex, 0x00U, static_cast<uint8_t>(0x0U),
                                       "Clear PDO mapping " + c_PdoCommentText);
               orc_ConciseEntries.push_back(c_Entry);

               if (rc_CurrentPdo.c_Signals.size() > 0)
               {
                  tgl_assert(rc_CurrentPdo.c_Signals.size() <= 127U);

                  //set mappings. To do so iterate over all signals of the current message
                  for (uint16_t u16_SigIt = 0; u16_SigIt < rc_CurrentPdo.c_Signals.size(); ++u16_SigIt)
                  {
                     const C_OscCanSignal & rc_CurrentSignal = rc_CurrentPdo.c_Signals[u16_SigIt];
                     uint32_t u32_Value;

                     tgl_assert(rc_CurrentSignal.u16_ComBitLength <= 255U);

                     //payload will contain 8bit signal length, 8bit sub-index, 16bit index (low byte), 16bit
                     // index (high byte)
                     u32_Value =
                        static_cast<uint8_t>(rc_CurrentSignal.u16_ComBitLength) +
                        (static_cast<uint32_t>(rc_CurrentSignal.u8_CanOpenManagerObjectDictionarySubIndex) << 8U) +
                        (static_cast<uint32_t>(rc_CurrentSignal.u16_CanOpenManagerObjectDictionaryIndex) << 16U);

                     //Set Mapping
                     c_Entry.SetConciseEntry(u16_ObjectIndex, static_cast<uint8_t>(u16_SigIt + 1U),
                                             u32_Value, "Set Mapping " + c_PdoCommentText);
                     orc_ConciseEntries.push_back(c_Entry);
                  }

                  //Set number of mappings
                  c_Entry.SetConciseEntry(u16_ObjectIndex, 0x00U, static_cast<uint8_t>(rc_CurrentPdo.c_Signals.size()),
                                          "Set number of mappings " + c_PdoCommentText);
                  orc_ConciseEntries.push_back(c_Entry);
               }
            }

            //return to "old" index pattern
            if (oq_IsTx)
            {
               u16_ObjectIndex = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO;
            }
            else
            {
               u16_ObjectIndex = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO;
            }

            u16_ObjectIndex += rc_CurrentPdo.u16_CanOpenManagerPdoIndex;

            //check if COB-ID is missing or RO
            if (q_CobIdIsRo == false)
            {
               uint32_t u32_CanId = rc_CurrentPdo.u32_CanId;

               if (oq_IsTx)
               {
                  //set flag to define this COB as "non-RTR" (some devices do not support RTR and would complain
                  // otherwise)
                  u32_CanId |= 0x40000000UL;
               }

               if (rc_CurrentPdo.q_IsExtended)
               {
                  u32_CanId |= 0x20000000UL;
               }

               //Set and enable COB-ID
               c_Entry.SetConciseEntry(u16_ObjectIndex, 0x01U, u32_CanId, "Set and enable COB-ID " + c_PdoCommentText);
               orc_ConciseEntries.push_back(c_Entry);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  creates sub-section for manager settings in manager configuration section in C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_ManInfo           CANopen manager definition
   \param[in]       ou16_GenCodeVersion   converted version of code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_CreateManSettings(C_SclStringList & orc_Data,
                                                    const C_OscCanOpenManagerInfo & orc_ManInfo,
                                                    const uint16_t ou16_GenCodeVersion)
{
   const uint16_t u16_HbProducerTime =
      orc_ManInfo.q_EnableHeartbeatProducing ? orc_ManInfo.u16_HeartbeatProducerTimeMs : 0U;
   const C_SclString c_Autostart = orc_ManInfo.q_AutostartCanOpenManager ? "1U" : "0U";

   C_SclString c_Define;

   orc_Data.Append("      " + C_SclString::IntToStr(
                      static_cast<uint32_t>(orc_ManInfo.u8_NodeIdValue)) + "U, ///< Manager Node ID");
   orc_Data.Append("      " + c_Autostart + ", ///< Autostart enabled");

   mh_GetDefineNmlStart(c_Define, orc_ManInfo.q_StartDevices, orc_ManInfo.q_NmtStartAll);
   orc_Data.Append("      " + c_Define + ",");

   mh_GetDefineNmtStop(c_Define, orc_ManInfo.e_NmtErrorBehaviour);
   orc_Data.Append("      " + c_Define + ",");

   orc_Data.Append("      " + C_SclString::IntToStr(
                      static_cast<uint32_t>(u16_HbProducerTime)) +
                   "U, ///< Heartbeat producer time [ms]");

   if (ou16_GenCodeVersion < 2)
   {
      orc_Data.Append("      " + C_SclString::IntToStr(static_cast<uint32_t>(orc_ManInfo.u16_GlobalSdoTimeoutMs)) +
                      "U ///< SDO timeout [ms]");
   }
   else
   {
      // _513
      uint32_t u32_SyncCyclePeriod = 0U;
      uint32_t u32_SyncWindowLength = 0U;

      //if "producing SYNC" is deactivated the cycle period and window length shall be initialized with "0U",
      //otherwise use the actual values (_513)
      if (orc_ManInfo.q_ProduceSyncMessage == true)
      {
         u32_SyncCyclePeriod = orc_ManInfo.u32_SyncCyclePeriodUs;
         u32_SyncWindowLength = orc_ManInfo.u32_SyncWindowLengthUs;
      }

      orc_Data.Append("      " + C_SclString::IntToStr(static_cast<uint32_t>(orc_ManInfo.u16_GlobalSdoTimeoutMs)) +
                      "U, ///< SDO timeout [ms]");
      orc_Data.Append("      " + C_SclString::IntToStr(
                         u32_SyncCyclePeriod) + "U, ///< SYNC Cycle Period in us");
      orc_Data.Append("      " + C_SclString::IntToStr(
                         u32_SyncWindowLength) + "U ///< SYNC Window length in us");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  creates sub-section for PDO configuration in manager configuration section in C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_DatapoolName      name of datapool which holds these PDO(s)
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       oq_TxExists           at least one TX PDO is existent
   \param[in]       oq_RxExists           at least one RX PDO is existent
   \param[in]       oq_ReferenceDataPool  true: reference DP; otherwise reference NULL
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_CreatePdoConfig(C_SclStringList & orc_Data, const C_SclString & orc_DatapoolName,
                                                  const uint8_t ou8_InterfaceIndex, const bool oq_TxExists,
                                                  const bool oq_RxExists, const bool oq_ReferenceDataPool)
{
   orc_Data.Append("      OSCO_MAN_CAN" + C_SclString::IntToStr(
                      static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_TX_PDOS,");
   orc_Data.Append("      OSCO_MAN_CAN" + C_SclString::IntToStr(
                      static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_RX_PDOS,");
   if (oq_TxExists)
   {
      orc_Data.Append("      &mat_PdoTxDefinition[0],");
   }
   else
   {
      orc_Data.Append("      NULL,");
   }

   if (oq_RxExists)
   {
      orc_Data.Append("      &mat_PdoRxDefinition[0],");
   }
   else
   {
      orc_Data.Append("      NULL,");
   }

   if (oq_ReferenceDataPool)
   {
      orc_Data.Append("      &gt_" + orc_DatapoolName + "_DataPool ///< Datapool containing signal values");
   }
   else
   {
      orc_Data.Append("      NULL ///< No signals defined, no Datapool reference");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert specific DataElement and Signal values to string

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_DatapoolList      specific Datapool list
   \param[in]       orc_Signals           list of signals
   \param[in]       ou32_SignalListIndex  index of Datapool list containing signals
   \param[in]       oq_RemoveLastComma    True: comma is removed (signals string should not end on comma)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_ConvertSignalsToStrings(C_SclStringList & orc_Data,
                                                          const C_OscNodeDataPoolList & orc_DatapoolList,
                                                          const std::vector<C_OscCanSignal> & orc_Signals,
                                                          const uint32_t ou32_SignalListIndex,
                                                          const bool oq_RemoveLastComma)
{
   std::vector<C_OscCanSignal> c_SignalsSorted(orc_Signals);
   std::sort(c_SignalsSorted.begin(), c_SignalsSorted.end());

   //get type of element via datapool_index, data_list_index, element_index
   for (uint16_t u16_SignalIndex = 0; u16_SignalIndex < c_SignalsSorted.size(); ++u16_SignalIndex)
   {
      const C_OscCanSignal c_Signal = c_SignalsSorted[u16_SignalIndex];
      const uint32_t u32_DataElementIndex = c_Signal.u32_ComDataElementIndex;
      const C_OscNodeDataPoolContent::E_Type e_ElementType =
         orc_DatapoolList.c_Elements[u32_DataElementIndex].c_Value.GetType();
      C_SclString c_Text;

      c_Text = "   { OSY_DPA_ELEMENT_TYPE_" + C_OscExportUti::h_GetElementTypeAsString(e_ElementType) + ", " +
               C_SclString::IntToStr(ou32_SignalListIndex) + "U, " + C_SclString::IntToStr(u32_DataElementIndex) +
               "U }";

      if ((oq_RemoveLastComma == false) || (u16_SignalIndex < (c_SignalsSorted.size() - 1U)))
      {
         c_Text += ",";
      }
      orc_Data.Append(c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get magic name for consistency check

   \param[in]  orc_ProjectId        Project id for consistency check
   \param[in]  ou8_InterfaceIndex   Index of interface

   \return Magic name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscExportCanOpenConfig::mh_GetMagicName(const C_SclString & orc_ProjectId,
                                                      const uint8_t ou8_InterfaceIndex)
{
   const C_SclString c_MagicName = "OSCO_MAN_CAN" +
                                   C_SclString::IntToStr(static_cast<uint32_t>(ou8_InterfaceIndex) + 1U) +
                                   "_PROJECT_ID_" + orc_ProjectId;

   return c_MagicName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Evaluate combination of the two given flags and return the according define string. For more information
            on the flags see C_OscCanOpenManagerInfo

   \param[in,out]   orc_String          storage for define string
   \param[in]       oq_StartDevices     flag if CANopen manager starts devices, or each own device
   \param[in]       oq_NmtStartAll      Starting devices using "NMT Start All" command

   \return Define string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_GetDefineNmlStart(C_SclString & orc_String, const bool oq_StartDevices,
                                                    const bool oq_NmtStartAll)
{
   if ((oq_StartDevices == true) && (oq_NmtStartAll == false))
   {
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_START_DEVICES_YES";
   }
   else if ((oq_StartDevices == false) && (oq_NmtStartAll == false))
   {
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_START_DEVICES_NO";
   }
   else if ((oq_StartDevices == true) && (oq_NmtStartAll == true))
   {
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_START_DEVICES_START_ALL";
   }
   else
   {
      //combination oq_StartDevices == false && oq_NmtStartAll == false) is not valid!
      tgl_assert(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generates define string based on NMT behavior of CANopen manager

   \param[in,out]   orc_String         storage for define string
   \param[in]       oe_NmtBehavior     NMT error behavior definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscExportCanOpenConfig::mh_GetDefineNmtStop(C_SclString & orc_String,
                                                   const C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType oe_NmtBehavior)
{
   switch (oe_NmtBehavior)
   {
   case C_OscCanOpenManagerInfo::eRESTART_ALL_DEVICES:
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_ERROR_RESET_ALL_NODES";
      break;
   case C_OscCanOpenManagerInfo::eRESTART_FAILURE_DEVICE:
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_ERROR_RESET_ERROR_NODE";
      break;
   case C_OscCanOpenManagerInfo::eSTOP_ALL_DEVICES:
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_ERROR_STOP_ALL";
      break;
   default:
      //should not happen
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns total number of all PDOs (RX or TX depends on flag)

   \param[in]       orc_MsgContainer     communication protocol message definition
   \param[in]       oq_IsTx              True: counts signals for TX PDOs

   \return total number of signals
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscExportCanOpenConfig::mh_GetTotalNumOfPdoSignals(const C_OscCanMessageContainer & orc_MsgContainer,
                                                              const bool oq_IsTx)
{
   uint32_t u32_Retval = 0;

   std::vector<C_OscCanMessage> c_Pdos = oq_IsTx ? orc_MsgContainer.c_TxMessages : orc_MsgContainer.c_RxMessages;

   for (uint16_t u16_MsgIndex = 0U; u16_MsgIndex < c_Pdos.size(); ++u16_MsgIndex)
   {
      //only count active PDO(s) which also contain at least one signal
      if ((c_Pdos[u16_MsgIndex].q_CanOpenManagerMessageActive == true) &&
          (c_Pdos[u16_MsgIndex].c_Signals.empty() == false))
      {
         u32_Retval += static_cast<uint32_t>(c_Pdos[u16_MsgIndex].c_Signals.size());
      }
   }

   return u32_Retval;
}
