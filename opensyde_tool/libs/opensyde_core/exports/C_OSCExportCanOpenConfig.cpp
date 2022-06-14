//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export CANopen configuration for CANopen manager

   Create a .c and .h file providing entire CANopen configuration.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCExportCanOpenConfig.h"
#include "C_OSCExportUti.h"
#include "C_OSCExportDataPool.h"
#include "TGLUtils.h"
#include "C_OSCLoggingHandler.h"
/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

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
C_OSCExportCanOpenConciseEntry::C_OSCExportCanOpenConciseEntry() :
   u16_Index(0),
   u8_SubIndex(0),
   u32_PayloadSize(0),
   c_Comment("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor: creates a ConciseEntry object with given parameters

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       ou32_PayloadSize   size of payload (4 bytes)
   \param[in]       orc_Payload        storage for payload
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCExportCanOpenConciseEntry::C_OSCExportCanOpenConciseEntry(const uint16 ou16_Index, const uint8 ou8_SubIndex,
                                                               const uint32 ou32_PayloadSize,
                                                               const std::vector<uint8> & orc_Payload,
                                                               const C_SCLString & orc_Comment)
{
   u16_Index = ou16_Index;
   u8_SubIndex = ou8_SubIndex;
   u32_PayloadSize = ou32_PayloadSize;
   c_Payload = orc_Payload;
   c_Comment = orc_Comment;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCExportCanOpenConciseEntry::~C_OSCExportCanOpenConciseEntry()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the given values on a existing ConciseEntry object

   \param[in]       ou16_Index         CANopen object index (2 bytes)
   \param[in]       ou8_SubIndex       CANopen object subindex (1 byte)
   \param[in]       ou32_PayloadSize   size of payload (4 bytes)
   \param[in]       orc_Payload        storage for payload
   \param[in]       orc_Comment        comment for code generation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConciseEntry::SetConciseEntry(const uint16 ou16_Index, const uint8 ou8_SubIndex,
                                                     const uint32 ou32_PayloadSize,
                                                     const std::vector<uint8> & orc_Payload,
                                                     const C_SCLString & orc_Comment)
{
   u16_Index = ou16_Index;
   u8_SubIndex = ou8_SubIndex;
   u32_PayloadSize = ou32_PayloadSize;
   c_Payload = orc_Payload;
   c_Comment = orc_Comment;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Converts ConciseEntry data to string without comment

   \return string containing all data from ConciseEntry object
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportCanOpenConciseEntry::GetConciseString(void) const
{
   C_SCLString c_Retval = "0x" + C_SCLString::IntToHex(static_cast<uint8>(this->u16_Index), 2U).UpperCase() + "U, " +
                          "0x" +
                          C_SCLString::IntToHex(static_cast<uint8>(this->u16_Index >> 8U), 2U).UpperCase() + "U, " +
                          "0x" + C_SCLString::IntToHex(this->u8_SubIndex, 2U).UpperCase() + "U, " +
                          "0x" + C_SCLString::IntToHex(this->u32_PayloadSize, 2U).UpperCase() + "U, " +
                          "0x" + C_SCLString::IntToHex(this->u32_PayloadSize >> 8U, 2U).UpperCase() + "U, " +
                          "0x" + C_SCLString::IntToHex(this->u32_PayloadSize >> 16U, 2U).UpperCase() + "U, " +
                          "0x" + C_SCLString::IntToHex(this->u32_PayloadSize >> 24U, 2U).UpperCase() + "U, ";

   C_SCLString c_PayloadString;

   for (uint8 u8_It = 0; u8_It < c_Payload.size(); ++u8_It)
   {
      c_PayloadString += "0x" + C_SCLString::IntToHex(c_Payload[u8_It], 2).UpperCase() + "U, ";
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
C_SCLString C_OSCExportCanOpenConciseEntry::h_GetNumOfEntriesString(const uint32 ou32_NumOfEntries)
{
   return "0x" + C_SCLString::IntToHex(static_cast<uint8>(ou32_NumOfEntries), 2U).UpperCase() + "U, " +
          "0x" + C_SCLString::IntToHex(static_cast<uint8>(ou32_NumOfEntries >> 8U), 2U).UpperCase() + "U, " +
          "0x" + C_SCLString::IntToHex(static_cast<uint8>(ou32_NumOfEntries >> 16U), 2U).UpperCase() + "U, " +
          "0x" + C_SCLString::IntToHex(static_cast<uint8>(ou32_NumOfEntries >> 24U), 2U).UpperCase() + "U, " +
          "                                                 ///< Number of entries";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  For each CANopen Device we need to know the size of it's concise data array. This function calculates it.

   \param[in]       orc_ConciseEntries     vector of all concise entries

   \return array size
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCExportCanOpenConciseEntry::h_GetConciseArraySize(
   std::vector<C_OSCExportCanOpenConciseEntry> & orc_ConciseEntries)
{
   uint32 u32_Retval;
   uint32 u32_PayloadByteCount = 0;

   //get count of all payload bytes for all entries
   for (uint32 u32_EntryIt = 0; u32_EntryIt < orc_ConciseEntries.size(); ++u32_EntryIt)
   {
      u32_PayloadByteCount += orc_ConciseEntries[u32_EntryIt].c_Payload.size();
   }

   //formula:
   // 4[num_entries] + (num_of_entries * (2[index]+ 1[subindex] + 4[size_of_entry])) +
   // payload_byte_count_over_all_entries
   u32_Retval = static_cast<uint32>(4U + (orc_ConciseEntries.size() * (2U + 1U + 4U)) + u32_PayloadByteCount);

   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return file name without extension.

   Detailed function description (optional). The function shall be described detailed if it is not described completely
   by short function description and parameter description.

   \param[in]  ou8_InterfaceIndex   index of interface

   \return     assembled file name5
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportCanOpenConfig::h_GetFileName(const uint8 ou8_InterfaceIndex)
{
   //assemble file name
   //add interface index
   const C_SCLString c_Text = "osco_man_config_can" +
                              C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U);

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
uint16 C_OSCExportCanOpenConfig::h_ConvertOverallCodeVersion(const uint16 ou16_GenCodeVersion)
{
   uint16 u16_Return;

   //Technically CANopen code is "compatible" with overall versions up to 5. So don't impose any artificial limits here.
   if ((ou16_GenCodeVersion > 0) && (ou16_GenCodeVersion <= 5U))
   {
      u16_Return = 1U;
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
sint32 C_OSCExportCanOpenConfig::h_CreateSourceCode(const C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                    const uint16 ou16_ApplicationIndex, const uint8 ou8_InterfaceIndex,
                                                    const uint32 ou32_DatapoolIndex,
                                                    const C_SCLString & orc_ExportToolInfo)
{
   sint32 s32_Return = C_NO_ERR;
   C_OSCNodeApplication c_Application;

   if (ou16_ApplicationIndex < orc_Node.c_Applications.size())
   {
      c_Application = orc_Node.c_Applications[ou16_ApplicationIndex];
   }
   else
   {
      osc_write_log_error("Creating source code", "Application index " + C_SCLString::IntToStr(
                             ou16_ApplicationIndex) + "out of range.");
      s32_Return = C_RANGE;
   }

   if (s32_Return == C_NO_ERR)
   {
      //make sure application is programmable
      if (c_Application.e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         osc_write_log_error("Creating source code",
                             "Did not generate code for application \"" + c_Application.c_Name +
                             "\" because application is not programmable.");
         s32_Return = C_NOACT;
      }
      else
      {
         //make sure version is known
         if (c_Application.u16_GenCodeVersion > C_OSCNodeApplication::hu16_HIGHEST_KNOWN_CODE_VERSION)
         {
            osc_write_log_error("Creating source code",
                                "Did not generate code for application \"" + c_Application.c_Name +
                                "\" because code format version \"" +
                                C_SCLString::IntToStr(c_Application.u16_GenCodeVersion) + "\" is unknown.");
            s32_Return = C_NOACT;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      const C_OSCCanProtocol * const pc_Protocol = orc_Node.GetCANProtocolConst(C_OSCCanProtocol::eCAN_OPEN,
                                                                                ou32_DatapoolIndex);
      const C_OSCNodeDataPool * const pc_DataPool = orc_Node.GetComDataPoolConst(C_OSCCanProtocol::eCAN_OPEN,
                                                                                 ou32_DatapoolIndex);

      if (pc_DataPool != NULL)
      {
         bool q_NoSignals = true;
         const C_OSCCanMessageContainer & rc_Messages = pc_Protocol->c_ComMessages[ou8_InterfaceIndex];

         for (uint16 u16_MsgIt = 0U; u16_MsgIt < rc_Messages.c_TxMessages.size(); ++u16_MsgIt)
         {
            if (rc_Messages.c_TxMessages[u16_MsgIt].c_Signals.size() > 0)
            {
               q_NoSignals = false;
               break;
            }
         }

         //no Tx signals, but maybe we have some Rx
         if (q_NoSignals == true)
         {
            for (uint16 u16_MsgIt = 0U; u16_MsgIt < rc_Messages.c_RxMessages.size(); ++u16_MsgIt)
            {
               if (rc_Messages.c_RxMessages[u16_MsgIt].c_Signals.size() > 0)
               {
                  q_NoSignals = false;
                  break;
               }
            }
         }

         //we have some signals, continue
         if (q_NoSignals == false)
         {
            uint32 u32_HashValue = 0U;
            //calculate hash value over the current state of the Datapool and protocol definitions
            pc_Protocol->CalcHash(u32_HashValue);
            pc_DataPool->CalcHash(u32_HashValue);
            const C_SCLString c_ProjectId = C_SCLString::IntToStr(u32_HashValue);

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
                                "No messages with signals exist for specified communication protocol CANopen for interface index " +
                                C_SCLString::IntToStr(
                                   ou8_InterfaceIndex));
            s32_Return = C_CONFIG;
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
sint32 C_OSCExportCanOpenConfig::mh_CreateHeaderFile(const C_SCLString & orc_ExportInfoTool,
                                                     const C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                     const C_OSCCanMessageContainer & orc_MsgContainer,
                                                     const C_OSCNodeApplication & orc_Application,
                                                     const uint8 ou8_InterfaceIndex, const C_SCLString & orc_ProjectId)
{
   sint32 s32_Return;
   C_SCLStringList c_Data;

   c_Data.Clear();

   mh_AddHeader(orc_ExportInfoTool, c_Data, ou8_InterfaceIndex, mhq_IS_HEADER_FILE);

   //add includes
   c_Data.Append("");
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Includes"));
   c_Data.Append("#include \"osco_configuration.h\"");
   c_Data.Append("");
   C_OSCExportUti::h_AddExternCStart(c_Data);

   //add defines
   mh_AddDefines(c_Data, orc_Node, orc_MsgContainer, ou8_InterfaceIndex, orc_ProjectId,
                 orc_Application.u16_GenCodeVersion, mhq_IS_HEADER_FILE);

   //add types
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));
   c_Data.Append("");

   //add global variables
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));
   c_Data.Append("///Stack configuration");
   c_Data.Append("extern const T_osco_man_manager_configuration gt_osco_man_can" +
                 C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_ProtocolConfiguration;");
   c_Data.Append("");

   //add function prototypes
   C_OSCExportUti::h_AddProjIdFunctionPrototype(c_Data, mh_GetMagicName(orc_ProjectId, ou8_InterfaceIndex));

   //add implementation
   c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));
   C_OSCExportUti::h_AddExternCEnd(c_Data);
   c_Data.Append("#endif");

   //save all this
   s32_Return = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(ou8_InterfaceIndex), true);

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
   C_RD_WR     cannot store file
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportCanOpenConfig::mh_CreateImplementationFile(const C_SCLString & orc_ExportInfoTool,
                                                             const C_SCLString & orc_Path, const C_OSCNode & orc_Node,
                                                             const C_OSCCanMessageContainer & orc_MsgContainer,
                                                             const C_OSCNodeApplication & orc_Application,
                                                             const C_OSCNodeDataPool & orc_Datapool,
                                                             const uint8 ou8_InterfaceIndex,
                                                             const C_SCLString & orc_ProjectId)
{
   sint32 s32_Return = C_NO_ERR;

   C_SCLStringList c_Data;

   uint32 u32_TxListIndex;
   uint32 u32_RxListIndex;

   if ((C_OSCCanProtocol::h_GetComListIndex(orc_Datapool, ou8_InterfaceIndex, true, u32_TxListIndex) == C_NO_ERR) &&
       (C_OSCCanProtocol::h_GetComListIndex(orc_Datapool, ou8_InterfaceIndex, false, u32_RxListIndex) == C_NO_ERR))
   {
      c_Data.Clear();
      const C_OSCNodeDataPoolList rc_DatapoolTxList = orc_Datapool.c_Lists[u32_TxListIndex];
      const C_OSCNodeDataPoolList rc_DatapoolRxList = orc_Datapool.c_Lists[u32_RxListIndex];
      //add documentation header
      mh_AddHeader(orc_ExportInfoTool, c_Data, ou8_InterfaceIndex, mhq_IS_IMPLEMENTATION_FILE);

      //add includes
      mh_AddIncludes(c_Data, orc_Datapool, ou8_InterfaceIndex, true);

      //add defines
      mh_AddDefines(c_Data, orc_Node, orc_MsgContainer, ou8_InterfaceIndex, orc_ProjectId,
                    orc_Application.u16_GenCodeVersion, mhq_IS_IMPLEMENTATION_FILE);

      //add types
      c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Types"));
      c_Data.Append("");

      //add module global variables
      c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Variables"));
      c_Data.Append("");

      //add module global function prototypes
      c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
      c_Data.Append("");

      //add global variables
      mh_AddGlobalVariables(c_Data, orc_Datapool.c_Name, orc_Node, orc_MsgContainer, ou8_InterfaceIndex,
                            rc_DatapoolTxList, rc_DatapoolRxList, u32_TxListIndex, u32_RxListIndex);

      //add implementation
      c_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Implementation"));

      //save all this
      s32_Return = C_OSCExportUti::h_SaveToFile(c_Data, orc_Path, h_GetFileName(ou8_InterfaceIndex), false);
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
void C_OSCExportCanOpenConfig::mh_AddHeader(const C_SCLString & orc_ExportToolInfo, C_SCLStringList & orc_Data,
                                            const uint8 ou8_InterfaceIndex, const bool oq_FileType)
{
   orc_Data.Append(C_OSCExportUti::h_GetHeaderSeparator());
   orc_Data.Append("/*!");
   orc_Data.Append("   \\file");
   if (oq_FileType == mhq_IS_IMPLEMENTATION_FILE)
   {
      orc_Data.Append(
         "   \\brief       openSYDE CANopen Manager data definition (Source file with constant definitions)");
      orc_Data.Append("");
      orc_Data.Append("   Defines the communication configuration for protocol type CANopen on CAN interface " +
                      C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U) + ".");
      orc_Data.Append("");
      orc_Data.Append(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   else
   {
      orc_Data.Append(
         "   \\brief       openSYDE CANopen Manager data definition (Header file with constant and global definitions)");
      orc_Data.Append("");
      orc_Data.Append(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   }
   orc_Data.Append("*/");
   orc_Data.Append(C_OSCExportUti::h_GetHeaderSeparator());

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      const C_SCLString c_HeaderGuard = h_GetFileName(ou8_InterfaceIndex).UpperCase() + "H";
      orc_Data.Append("#ifndef " + c_HeaderGuard);
      orc_Data.Append("#define " + c_HeaderGuard);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add includes into C file.

   \param[in,out]  orc_Data               converted data to string list
   \param[in]      orc_Datapool           name of datapool
   \param[in]      ou8_InterfaceIndex     index of interface
   \param[in]      oq_NullRequired        true: definition of NULL required in c-file.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_AddIncludes(C_SCLStringList & orc_Data, const C_OSCNodeDataPool & orc_Datapool,
                                              const uint8 ou8_InterfaceIndex, const bool oq_NullRequired)
{
   orc_Data.Append("");
   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Includes"));

   if (oq_NullRequired == true)
   {
      orc_Data.Append("#include <stddef.h>");
      orc_Data.Append("");
   }

   orc_Data.Append("#include \"osco_man_config_can" + C_SCLString::IntToStr(
                      static_cast<uint32>(ou8_InterfaceIndex) + 1U) + ".h\"");

   orc_Data.Append("#include \"stwtypes.h\"");
   orc_Data.Append("");
   orc_Data.Append("#include \"" + C_OSCExportDataPool::h_GetFileName(orc_Datapool) + ".h\"");
   orc_Data.Append("");
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
void C_OSCExportCanOpenConfig::mh_AddDefines(C_SCLStringList & orc_Data, const C_OSCNode & orc_Node,
                                             const C_OSCCanMessageContainer & orc_MsgContainer,
                                             const uint8 ou8_InterfaceIndex, const C_SCLString & orc_ProjectId,
                                             const uint16 ou16_GenCodeVersion, const bool oq_FileType)
{
   const C_SCLString c_MagicName = mh_GetMagicName(orc_ProjectId, ou8_InterfaceIndex);

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Defines"));

   if (oq_FileType == mhq_IS_HEADER_FILE)
   {
      //needed for define name (see below)
      uint32 u32_DevCnt = 0;
      const C_SCLString c_InterfaceIndex = C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U);

      //get the CANopen manager info via interface index
      const std::map<uint8, C_OSCCanOpenManagerInfo>::const_iterator c_Iterator =
         orc_Node.c_CanOpenManagers.find(ou8_InterfaceIndex);
      const C_OSCCanOpenManagerInfo c_OscoManInfo = c_Iterator->second;
      std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_DeviceIt;

      orc_Data.Append("///unique ID to ensure consistency between .h and .c files");
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_PROJECT_ID_" + orc_ProjectId +
                      " void osco_man_can" + c_InterfaceIndex + "_project_id_" + orc_ProjectId +
                      "(void) {}");
      orc_Data.Append("");

      for (c_DeviceIt = c_OscoManInfo.c_CanOpenDevices.begin(); c_DeviceIt != c_OscoManInfo.c_CanOpenDevices.end();
           ++c_DeviceIt)
      {
         orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_DEVICE" + C_SCLString::IntToStr(
                            u32_DevCnt + 1U) + " (" + C_SCLString::IntToStr(u32_DevCnt) + "U)");
         u32_DevCnt++;
      }

      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_DEVICES (" +
                      C_SCLString::IntToStr(c_OscoManInfo.c_CanOpenDevices.size()) + "U)");
      orc_Data.Append("");
      orc_Data.Append("///Index of TX PDOs");

      for (uint16 u16_MsgIndex = 0U; u16_MsgIndex < orc_MsgContainer.c_TxMessages.size(); ++u16_MsgIndex)
      {
         const C_SCLString c_MsgName = orc_MsgContainer.c_TxMessages[u16_MsgIndex].c_Name.UpperCase();
         orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_TX_PDO_INDEX_" + c_MsgName + " (" + C_SCLString::IntToStr(
                            u16_MsgIndex) + "U)");
      }
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_TX_PDOS (" +
                      C_SCLString::IntToStr(orc_MsgContainer.c_TxMessages.size()) + "U)");

      //number of TX signals
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_TX_SIGNALS (" +
                      C_SCLString::IntToStr(mh_GetTotalNumOfPdoSignals(orc_MsgContainer, true)) + "U)");
      orc_Data.Append("");
      orc_Data.Append("///Index of RX PDOs");

      for (uint16 u16_MsgIndex = 0U; u16_MsgIndex < orc_MsgContainer.c_RxMessages.size(); ++u16_MsgIndex)
      {
         const C_SCLString c_MsgName = orc_MsgContainer.c_RxMessages[u16_MsgIndex].c_Name.UpperCase();
         orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_RX_PDO_INDEX_" + c_MsgName + " (" + C_SCLString::IntToStr(
                            u16_MsgIndex) + "U)");
      }
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_RX_PDOS (" +
                      C_SCLString::IntToStr(orc_MsgContainer.c_RxMessages.size()) + "U)");

      //number of RX signals
      orc_Data.Append("#define OSCO_MAN_CAN" + c_InterfaceIndex + "_NUMBER_OF_RX_SIGNALS (" +
                      C_SCLString::IntToStr(mh_GetTotalNumOfPdoSignals(orc_MsgContainer, false)) + "U)");
   }
   else
   {
      if (ou16_GenCodeVersion >= 1U)
      {
         orc_Data.Append("///check for correct version of structure definitions");
         orc_Data.Append("#if OSCO_MAN_CONFIG_DEFINITION_VERSION != 0x" +
                         C_SCLString::IntToHex(static_cast<sint64>(C_OSCExportCanOpenConfig::h_ConvertOverallCodeVersion(
                                                                      ou16_GenCodeVersion)), 4U) + "U");
         orc_Data.Append("///if compilation fails here the openSYDE library version does not match the version of the "
                         "generated code");
         orc_Data.Append("static T_osy_non_existing_type_" + orc_ProjectId + " mt_Variable;");
         orc_Data.Append("#endif");
         orc_Data.Append("");
      }
      C_OSCExportUti::h_AddProjectIdDef(orc_Data, c_MagicName, false);
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
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_AddGlobalVariables(C_SCLStringList & orc_Data, const C_SCLString & orc_DatapoolName,
                                                     const C_OSCNode & orc_Node,
                                                     const C_OSCCanMessageContainer & orc_MsgContainer,
                                                     const uint8 ou8_InterfaceIndex,
                                                     const C_OSCNodeDataPoolList & orc_DatapoolTxList,
                                                     const C_OSCNodeDataPoolList & orc_DatapoolRxList,
                                                     const stw_types::uint32 ou32_TxListIndex,
                                                     const stw_types::uint32 ou32_RxListIndex)
{
   uint32 u32_ManagerInstance;
   const bool q_TxExists = (orc_MsgContainer.c_TxMessages.size() > 0) ? true : false;
   const bool q_RxExists = (orc_MsgContainer.c_RxMessages.size() > 0) ? true : false;

   std::vector<uint32> c_ConcArraySizes;
   //get the CANopen manager info via interface index
   const std::map<uint8, C_OSCCanOpenManagerInfo>::const_iterator c_Iterator =
      orc_Node.c_CanOpenManagers.find(ou8_InterfaceIndex);
   const C_OSCCanOpenManagerInfo c_ManInfo = c_Iterator->second;

   orc_Data.Append(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));
   orc_Data.Append("");

   if (q_TxExists)
   {
      orc_Data.Append("///Tx signal definitions");
      mh_AddSignalDefinitions(orc_Data, orc_DatapoolTxList, ou32_TxListIndex, orc_MsgContainer.c_TxMessages);
   }
   if (q_RxExists)
   {
      orc_Data.Append("///Rx signal definitions");
      mh_AddSignalDefinitions(orc_Data, orc_DatapoolRxList, ou32_RxListIndex, orc_MsgContainer.c_RxMessages);
   }

   if (q_TxExists)
   {
      orc_Data.Append("///Tx PDO definitions");
      mh_AddPdoDefinitions(orc_Data, orc_MsgContainer.c_TxMessages, ou8_InterfaceIndex, true, true);
   }
   orc_Data.Append("");
   if (q_RxExists)
   {
      orc_Data.Append("///Rx PDO definitions");
      mh_AddPdoDefinitions(orc_Data, orc_MsgContainer.c_RxMessages, ou8_InterfaceIndex, false, true);
   }

   orc_Data.Append("");

   //Add section with concise data for each CANopen manager sub-device
   c_ConcArraySizes = mh_AddDeviceSpecificConciseData(orc_Data, orc_Node, orc_MsgContainer, ou8_InterfaceIndex);

   orc_Data.Append("");

   //Add section with device settings
   mh_AddDeviceSettings(orc_Data, c_ConcArraySizes, c_ManInfo, ou8_InterfaceIndex, true);

   orc_Data.Append("");

   //Add last section with CANopen manager configuration
   u32_ManagerInstance = std::distance(orc_Node.c_CanOpenManagers.begin(), c_Iterator);
   mh_AddManagerConfig(orc_Data, orc_DatapoolName, c_ManInfo, u32_ManagerInstance, ou8_InterfaceIndex, q_TxExists,
                       q_RxExists);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add signal definition section to .c file

   \param[in,out]   orc_Data                 list of strings to add
   \param[in]       orc_DatapoolList         specific Datapool list
   \param[in]       ou32_SignalListIndex     index of Datapool list containing the signals
   \param[in]       orc_Messages             vector of messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_AddSignalDefinitions(C_SCLStringList & orc_Data,
                                                       const C_OSCNodeDataPoolList & orc_DatapoolList,
                                                       const uint32 ou32_SignalListIndex,
                                                       const std::vector<C_OSCCanMessage> & orc_Messages)
{
   for (uint16 u16_MsgIndex = 0; u16_MsgIndex < orc_Messages.size(); ++u16_MsgIndex)
   {
      const C_OSCCanMessage & rc_CurrentMsg = orc_Messages[u16_MsgIndex];
      const std::vector<C_OSCCanSignal> & rc_CurrentSignalList = orc_Messages[u16_MsgIndex].c_Signals;
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add message definition to C file

   \param[in,out]   orc_Data             converted data to string list
   \param[in]       orc_Messages         vector of messages
   \param[in]       ou8_InterfaceIndex   index of interface
   \param[in]       oq_IsTx              True: PDO is Tx
   \param[in]       oq_RemoveLastComma    True: comma is removed (signals string should not end on comma)

*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_AddPdoDefinitions(C_SCLStringList & orc_Data,
                                                    const std::vector<C_OSCCanMessage> & orc_Messages,
                                                    const uint8 ou8_InterfaceIndex, const bool oq_IsTx,
                                                    const bool oq_RemoveLastComma)
{
   C_SCLString c_Define;
   C_SCLString c_TxOrRx;

   //generate define and part of struct name depending on TX or RX
   if (oq_IsTx)
   {
      c_Define = "[OSCO_MAN_CAN" + C_SCLString::IntToStr(
         static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_TX_PDOS]";
      c_TxOrRx = "Tx";
   }
   else
   {
      c_Define = "[OSCO_MAN_CAN" + C_SCLString::IntToStr(
         static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_RX_PDOS]";
      c_TxOrRx = "Rx";
   }

   orc_Data.Append("static const T_osco_man_pdo_definition mat_Pdo" + c_TxOrRx + "Definition" + c_Define + " =");
   orc_Data.Append("{");
   for (uint16 u16_MsgIndex = 0; u16_MsgIndex < orc_Messages.size(); ++u16_MsgIndex)
   {
      //in this context message means PDO
      const C_OSCCanMessage rc_CurrentMessage = orc_Messages[u16_MsgIndex];
      const C_SCLString c_IdExt = rc_CurrentMessage.q_IsExtended ? "1U" : "0U";
      //EventTime depends on whether PDO is TX or RX
      const C_SCLString c_EventTime =
         oq_IsTx ? C_SCLString::IntToStr(rc_CurrentMessage.u32_CycleTimeMs) : C_SCLString::IntToStr(
            rc_CurrentMessage.u32_TimeoutMs);

      //if signal list of current signal is empty, instead of adding pointer to PDO struct to array, the pointer shall
      // be NULL
      const C_SCLString c_StructName = rc_CurrentMessage.c_Signals.empty() ? "NULL" :
                                       "&mat_Pdo_" + rc_CurrentMessage.c_Name + "[0]";
      C_SCLString c_Text;

      tgl_assert(rc_CurrentMessage.u16_DelayTimeMs <= 6553U);
      //we need a resolution of 100 micro-seconds on embedded side
      const uint16 u16_InhibitTime = static_cast<uint16>(rc_CurrentMessage.u16_DelayTimeMs * 10U);

      //array contains: COB-ID, COB-ID Extension, COMM-Method, InhibitTime, EventTimer, NumSignals, pointer to
      //first element of array "signal definitions".
      //See code example in /doc/file_specifications/Generated_C_Code/CANopenManager_Definition
      c_Text = "   { 0x" + C_SCLString::IntToHex(rc_CurrentMessage.u32_CanId, 3U) + "U, " + c_IdExt +
               ", OSCO_MAN_MANAGER_COMM_METHOD_CYCLIC_ASYNC_MANUFA_SPEC, " +
               C_SCLString::IntToStr(static_cast<uint32>(u16_InhibitTime)) + "U, " +
               c_EventTime + "U, " + C_SCLString::IntToStr(rc_CurrentMessage.c_Signals.size()) + "U, " +
               c_StructName + " }";

      if ((oq_RemoveLastComma == false) || (u16_MsgIndex < (orc_Messages.size() - 1U)))
      {
         c_Text += ",";
      }
      orc_Data.Append(c_Text);
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
std::vector<uint32> C_OSCExportCanOpenConfig::mh_AddDeviceSpecificConciseData(C_SCLStringList & orc_Data,
                                                                              const C_OSCNode & orc_Node,
                                                                              const C_OSCCanMessageContainer & orc_MsgContainer,
                                                                              const uint8 ou8_InterfaceIndex)
{
   std::vector<uint32> c_Retval;
   uint32 u32_DevCnt = 1;

   //get the CANopen manager info via interface index
   const std::map<uint8, C_OSCCanOpenManagerInfo>::const_iterator c_Iterator =
      orc_Node.c_CanOpenManagers.find(ou8_InterfaceIndex);
   C_OSCCanOpenManagerInfo c_ManInfo = c_Iterator->second;

   std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_DeviceIt;

   //we collect and then generate concise data for each CANopen device
   for (c_DeviceIt = c_ManInfo.c_CanOpenDevices.begin(); c_DeviceIt != c_ManInfo.c_CanOpenDevices.end(); ++c_DeviceIt)
   {
      uint32 u32_ArraySize;
      C_SCLString c_NumOfEntries;
      const C_OSCCanOpenManagerDeviceInfo & rc_CurrentDevice = c_DeviceIt->second;
      const uint32 u32_NodeIndex = c_DeviceIt->first.u32_NodeIndex;
      std::vector<C_OSCExportCanOpenConciseEntry> c_ConciseEntries;

      mh_CollectDeviceSpecificConciseData(c_ConciseEntries, rc_CurrentDevice);
      //Tx Pdos (= Rx Pdos of device)
      mh_CollectPdoConciseData(c_ConciseEntries, rc_CurrentDevice, u32_NodeIndex, orc_MsgContainer, true);
      //Rx Pdos (= Tx Pdos of device)
      mh_CollectPdoConciseData(c_ConciseEntries, rc_CurrentDevice, u32_NodeIndex, orc_MsgContainer, false);

      u32_ArraySize = C_OSCExportCanOpenConciseEntry::h_GetConciseArraySize(c_ConciseEntries);
      c_Retval.push_back(u32_ArraySize);

      orc_Data.Append("static const uint8 mau8_Device" + C_SCLString::IntToStr(u32_DevCnt) +
                      "ConciseDcf[" + C_SCLString::IntToStr(u32_ArraySize) + "] =");
      orc_Data.Append("{");

      //the number of all entries needs to be in first place
      c_NumOfEntries = C_OSCExportCanOpenConciseEntry::h_GetNumOfEntriesString(
         c_ConciseEntries.size());
      orc_Data.Append("   " + c_NumOfEntries);

      //unpack ConciseEntries and convert everything to string
      for (uint32 u32_ConciseIt = 0; u32_ConciseIt < c_ConciseEntries.size(); ++u32_ConciseIt)
      {
         C_SCLString c_Entry = c_ConciseEntries[u32_ConciseIt].GetConciseString();

         //add comma until last entry has reached
         if (u32_ConciseIt < static_cast<uint32>(c_ConciseEntries.size() - 1U))
         {
            c_Entry += ",";
         }
         else
         {
            c_Entry += " ";
         }

         //beautify with right indentation for comment
         switch (c_ConciseEntries[u32_ConciseIt].u32_PayloadSize)
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

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add section for device settings to C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_ConcSizes         vector with sizes of all concise data for each device
   \param[in]       orc_ManInfo           CANopen manager configuration
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       oq_RemoveLastComma    True: last comma gets removed.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_AddDeviceSettings(C_SCLStringList & orc_Data,
                                                    const std::vector<uint32> & orc_ConcSizes,
                                                    const C_OSCCanOpenManagerInfo & orc_ManInfo,
                                                    const uint8 ou8_InterfaceIndex, const bool oq_RemoveLastComma)
{
   std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_DeviceIt;
   //this iterator is needed to remove the last comma in loop below
   std::map<C_OSCCanInterfaceId,
            C_OSCCanOpenManagerDeviceInfo>::const_iterator c_Abort = orc_ManInfo.c_CanOpenDevices.end();
   uint32 u32_DevCnt = 1U;

   orc_Data.Append("static const T_osco_man_devices_settings mat_DeviceSettings[OSCO_MAN_CAN" +
                   C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_DEVICES] =");
   orc_Data.Append("{");

   tgl_assert(orc_ManInfo.c_CanOpenDevices.size() == orc_ConcSizes.size());
   --c_Abort;

   for (c_DeviceIt = orc_ManInfo.c_CanOpenDevices.begin(); c_DeviceIt != orc_ManInfo.c_CanOpenDevices.end();
        ++c_DeviceIt)
   {
      //u32_DevCount starts with one, but to access vector we need 0-based
      const uint32 u32_Index = u32_DevCnt - 1U;
      const C_OSCCanOpenManagerDeviceInfo rc_CurrentDevice = c_DeviceIt->second;
      const C_SCLString c_OptionalDev = rc_CurrentDevice.q_DeviceOptional ? "1U" : "0U";
      const C_SCLString c_NoInit = rc_CurrentDevice.q_NoInitialization ? "1U" : "0U";
      C_SCLString c_Text;

      c_Text = "   { " + C_SCLString::IntToStr(static_cast<uint32>(rc_CurrentDevice.u8_NodeIDValue)) + "U, " +
               c_OptionalDev + ", " + c_NoInit + ", " +
               C_SCLString::IntToStr(static_cast<uint32>(rc_CurrentDevice.u16_HeartbeatProducerTimeMs)) + "U, " +
               C_SCLString::IntToStr(orc_ConcSizes[u32_Index]) + "U, " + "&mau8_Device" +
               C_SCLString::IntToStr(u32_DevCnt) + "ConciseDcf[0] }";

      if ((oq_RemoveLastComma == false) || (c_DeviceIt != c_Abort))
      {
         c_Text += ",";
      }
      orc_Data.Append(c_Text);

      u32_DevCnt++;
   }
   orc_Data.Append("};");
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
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_AddManagerConfig(C_SCLStringList & orc_Data, const C_SCLString & orc_DatapoolName,
                                                   const C_OSCCanOpenManagerInfo & orc_ManInfo,
                                                   const uint32 ou32_ManInstance, const uint8 ou8_InterfaceIndex,
                                                   const bool oq_TxExists, const bool oq_RxExists)
{
   orc_Data.Append("const T_osco_man_manager_configuration gt_osco_man_can" +
                   C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_ProtocolConfiguration =");
   orc_Data.Append("{");
   orc_Data.Append("   " + C_SCLString::IntToStr(
                      static_cast<uint32>(ou8_InterfaceIndex)) + "U, ///< selected CAN channel");
   orc_Data.Append("   " + C_SCLString::IntToStr(ou32_ManInstance) + "U, ///< CANopen Manager instance");
   orc_Data.Append("   {");

   //creates T_osco_man_manager_settings
   mh_CreateManSettings(orc_Data, orc_ManInfo);

   orc_Data.Append("   },");
   orc_Data.Append("   OSCO_MAN_CAN" + C_SCLString::IntToStr(
                      static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_DEVICES, ///< Number of Devices");
   orc_Data.Append("   &mat_DeviceSettings[0],");
   orc_Data.Append("   {");

   //creates T_osco_man_pdo_configuration
   mh_CreatePdoConfig(orc_Data, orc_DatapoolName, ou8_InterfaceIndex, oq_TxExists, oq_RxExists);

   orc_Data.Append("   }");
   orc_Data.Append("};");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Given a configuration of a CANopen manager device, this function collects all necessary data to create
            concise data section.

   \param[in,out]   orc_ConciseEntries   storage for all concise entries
   \param[in]       orc_DeviceInfo       CANopen manager device configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_CollectDeviceSpecificConciseData(
   std::vector<C_OSCExportCanOpenConciseEntry> & orc_ConciseEntries,
   const C_OSCCanOpenManagerDeviceInfo & orc_DeviceInfo)
{
   const C_OSCCanOpenObjectDictionary c_OD = orc_DeviceInfo.c_EDSFileContent;
   C_OSCExportCanOpenConciseEntry c_Entry = C_OSCExportCanOpenConciseEntry();
   C_SCLString c_Comment;

   std::vector<uint8> c_Payload;

   if (orc_DeviceInfo.q_FactorySettingsActive)
   {
      mh_GetRestoreValueComment(c_Comment, orc_DeviceInfo.u8_ResetNodeObjectDictionarySubIndex);

      c_Payload.clear();
      c_Payload.resize(4U);
      //set "load":
      c_Payload[0] = 0x6CU;
      c_Payload[1] = 0x6FU;
      c_Payload[2] = 0x61U;
      c_Payload[3] = 0x64U;

      c_Entry.SetConciseEntry(mhu16_OD_INDEX_RESTORE_PARAMETERS, orc_DeviceInfo.u8_ResetNodeObjectDictionarySubIndex,
                              c_Payload.size(), c_Payload, c_Comment);
      orc_ConciseEntries.push_back(c_Entry);
   }

   //Heartbeat Consumer
   if (orc_DeviceInfo.q_EnableHeartbeatConsuming)
   {
      const uint8 u8_NumHBConsumer = c_OD.GetNumHeartbeatConsumers();
      if (u8_NumHBConsumer > 0)
      {
         c_Comment = "Set Heartbeat Consumer";
         for (uint8 u8_It = 0; u8_It < u8_NumHBConsumer; ++u8_It)
         {
            c_Payload.clear();
            c_Payload.resize(4U);
            //for first consumer we write a specific value, all others value = 0
            if (u8_It < 1)
            {
               //object value: bit 0-15 HB time, 16-23 node id, 24-31 reserved with "0"
               c_Payload[0] = static_cast<uint8>(orc_DeviceInfo.u16_HeartbeatConsumerTimeMs);
               c_Payload[1] = static_cast<uint8>(orc_DeviceInfo.u16_HeartbeatConsumerTimeMs >> 8U);
               c_Payload[2] = orc_DeviceInfo.u8_NodeIDValue;
               c_Payload[3] = 0x00U;
            }
            else
            {
               c_Payload[0] = 0x00U;
               c_Payload[1] = 0x00U;
               c_Payload[2] = 0x00U;
               c_Payload[3] = 0x00U;
            }

            c_Entry.SetConciseEntry(mhu16_OD_INDEX_HEARTBEAT_CONSUMER, u8_It, c_Payload.size(), c_Payload, c_Comment);
         }
      }
   }

   //Heartbeat Producer
   c_Payload.clear();
   c_Payload.resize(2U);
   if (orc_DeviceInfo.q_EnableHeartbeatProducing)
   {
      c_Comment = "Enable Heartbeat Producing";
      c_Payload[0] = static_cast<uint8>(orc_DeviceInfo.u16_HeartbeatProducerTimeMs);
      c_Payload[1] = static_cast<uint8>(orc_DeviceInfo.u16_HeartbeatProducerTimeMs >> 8U);
   }
   else
   {
      c_Comment = "Disable Heartbeat Producing";
      c_Payload[0] = 0x00U;
      c_Payload[1] = 0x00U;
   }
   c_Entry.SetConciseEntry(mhu16_OD_INDEX_HEARTBEAT_PRODUCER, 0x00U, c_Payload.size(), c_Payload, c_Comment);
   orc_ConciseEntries.push_back(c_Entry);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Given a configuration of a CANopen manager device, this function collect all TX and RX PDO specific
            information

   \param[in,out]   orc_ConciseEntries   storage for all concise entries
   \param[in]       orc_DeviceInfo       CANopen manager device configuration
   \param[in]       ou32_NodeIndex       node index of current CANopen manager device (needed to match PDOs)
   \param[in]       orc_MsgContainer     communication protocol message definition
   \param[in]       oq_IsTx              True: collect data for TX PDOs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_CollectPdoConciseData(
   std::vector<C_OSCExportCanOpenConciseEntry> & orc_ConciseEntries,
   const C_OSCCanOpenManagerDeviceInfo & orc_DeviceInfo, const uint32 ou32_NodeIndex,
   const C_OSCCanMessageContainer & orc_MsgContainer, const bool oq_IsTx)
{
   std::vector<C_OSCCanMessage> c_Pdos = oq_IsTx ? orc_MsgContainer.c_TxMessages : orc_MsgContainer.c_RxMessages;

   //iterate over RX or TX PDOS (depends on flag) of current device and push info into array
   for (uint16 u16_PdoIndex = 0; u16_PdoIndex < c_Pdos.size(); ++u16_PdoIndex)
   {
      uint16 u16_ObjectIndex;
      const C_OSCCanMessage rc_CurrentPdo = c_Pdos[u16_PdoIndex];

      if (rc_CurrentPdo.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex == ou32_NodeIndex)
      {
         uint32 u32_Return;
         uint32 u32_CanId;
         bool q_SectionRO;
         std::vector<uint8> c_Payload;
         const C_OSCCanOpenObjectDictionary c_OD = orc_DeviceInfo.c_EDSFileContent;
         C_OSCExportCanOpenConciseEntry c_Entry = C_OSCExportCanOpenConciseEntry();

         //we need to "swap" the TX/RX perspective here: "c_Pdos" contains the PDOs from the manager's perspective
         // for the concise data we need to take the device's perspective
         if (oq_IsTx)
         {
            u16_ObjectIndex = mhu16_OD_INDEX_FIRST_RX_PDO;
         }
         else
         {
            u16_ObjectIndex = mhu16_OD_INDEX_FIRST_TX_PDO;
         }
         u16_ObjectIndex += rc_CurrentPdo.u16_CanOpenManagerPdoIndex;

         //disable PDO before changing settings
         u32_CanId = rc_CurrentPdo.u32_CanId | 0x80000000UL;

         if (rc_CurrentPdo.q_IsExtended)
         {
            u32_CanId = u32_CanId | 0x20000000UL;
         }

         c_Payload.clear();
         //COB-ID is a uint32 value
         c_Payload.resize(4U);
         c_Payload[0] = static_cast<uint8>(u32_CanId);
         c_Payload[1] = static_cast<uint8>(u32_CanId >> 8U);
         c_Payload[2] = static_cast<uint8>(u32_CanId >> 16U);
         c_Payload[3] = static_cast<uint8>(u32_CanId >> 24U);

         //Disable PDO
         c_Entry.SetConciseEntry(u16_ObjectIndex, 0x01, c_Payload.size(), c_Payload, "Disable PDO");
         orc_ConciseEntries.push_back(c_Entry);

         //check if transmission type is missing or RO
         u32_Return = c_OD.IsTransmissionTypeRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRO);

         if ((u32_Return == C_NO_ERR) && (q_SectionRO == false))
         {
            uint8 u8_TxMethodType = 0;
            switch (rc_CurrentPdo.e_TxMethod)
            {
            case C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_254:
               u8_TxMethodType = 0xFEU;
               break;
            case C_OSCCanMessage::eTX_METHOD_CAN_OPEN_TYPE_255:
               u8_TxMethodType = 0xFFU;
               break;
            case C_OSCCanMessage::eTX_METHOD_CYCLIC:
            case C_OSCCanMessage::eTX_METHOD_ON_CHANGE:
            case C_OSCCanMessage::eTX_METHOD_ON_EVENT:
            default:
               //not supported for CANopen
               tgl_assert(false);
               break;
            }

            c_Payload.clear();
            //TxMethodType information is 1 byte
            c_Payload.resize(1U);
            c_Payload[0] = u8_TxMethodType;

            //Set transmission type
            c_Entry.SetConciseEntry(u16_ObjectIndex, 0x02U, c_Payload.size(), c_Payload, "Set transmission type");
            orc_ConciseEntries.push_back(c_Entry);
         }

         //check if inhibit time is missing or RO
         u32_Return = c_OD.IsInhibitTimeRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRO);

         if ((u32_Return == C_NO_ERR) && (q_SectionRO == false))
         {
            //resolution of 100 micro second is needed for CANopen stack
            const uint16 u16_InhibitTimeMicro = static_cast<uint16>(rc_CurrentPdo.u16_DelayTimeMs * 10U);

            c_Payload.clear();
            //Inhibit-time information is 2 byte long
            c_Payload.resize(2U);
            c_Payload[0] = static_cast<uint8>(u16_InhibitTimeMicro);
            c_Payload[1] = static_cast<uint8>(u16_InhibitTimeMicro >> 8U);

            //Set inhibit time
            c_Entry.SetConciseEntry(u16_ObjectIndex, 0x03U, c_Payload.size(), c_Payload, "Set inhibit time");
            orc_ConciseEntries.push_back(c_Entry);
         }

         //check if event time is missing or RO
         u32_Return = c_OD.IsEventTimerRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRO);

         if ((u32_Return == C_NO_ERR) && (q_SectionRO == false))
         {
            //out of range timeouts really should be prevented before trying to generate code:
            tgl_assert(rc_CurrentPdo.u32_TimeoutMs <= 0xFFFFU);
            c_Payload.clear();
            //Event-Time on RX PDOs = C_OSCCanMessage::u32_TimeOutMs
            c_Payload.resize(2U);
            c_Payload[0] = static_cast<uint8>(rc_CurrentPdo.u32_TimeoutMs);
            c_Payload[1] = static_cast<uint8>(rc_CurrentPdo.u32_TimeoutMs >> 8U);

            //Set event time
            c_Entry.SetConciseEntry(u16_ObjectIndex, 0x05U, c_Payload.size(), c_Payload, "Set event time");
            orc_ConciseEntries.push_back(c_Entry);
         }

         //for PDO mapping entries, we need to break the index pattern here
         //again: TX/RX needs to be swapped to change perspective ...
         if (oq_IsTx)
         {
            u16_ObjectIndex = mhu16_OD_INDEX_FIRST_RX_PDO_MAPPING;
         }
         else
         {
            u16_ObjectIndex = mhu16_OD_INDEX_FIRST_TX_PDO_MAPPING;
         }

         u16_ObjectIndex += rc_CurrentPdo.u16_CanOpenManagerPdoIndex;

         //check if PDO mapping is missing or RO
         u32_Return = c_OD.IsPDOMappingRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRO);

         if ((u32_Return == C_NO_ERR) && (q_SectionRO == false))
         {
            c_Payload.clear();
            c_Payload.resize(1U);
            c_Payload[0] = 0x00U;

            //Clear PDO mapping
            c_Entry.SetConciseEntry(u16_ObjectIndex, 0x00U, c_Payload.size(), c_Payload, "Clear pdo mapping");
            orc_ConciseEntries.push_back(c_Entry);

            if (rc_CurrentPdo.c_Signals.size() > 0)
            {
               tgl_assert(rc_CurrentPdo.c_Signals.size() <= 127U);

               //set mappings. To do so iterate over all signals of the current message
               for (uint16 u16_SigIt = 0; u16_SigIt < rc_CurrentPdo.c_Signals.size(); ++u16_SigIt)
               {
                  const C_OSCCanSignal & rc_CurrentSignal = rc_CurrentPdo.c_Signals[u16_SigIt];

                  tgl_assert(rc_CurrentSignal.u16_ComBitLength <= 255U);

                  //payload will contain 8bit signal length, 8bit sub-index, 16bit index (low byte), 16bit index(high
                  // byte)
                  c_Payload.clear();
                  c_Payload.resize(4U);

                  c_Payload[0] = static_cast<uint8>(rc_CurrentSignal.u16_ComBitLength);
                  c_Payload[1] = rc_CurrentSignal.u8_CanOpenManagerObjectDictionarySubIndex;
                  c_Payload[2] = static_cast<uint8>(rc_CurrentSignal.u16_CanOpenManagerObjectDictionaryIndex);
                  c_Payload[3] = static_cast<uint8>(rc_CurrentSignal.u16_CanOpenManagerObjectDictionaryIndex >> 8U);

                  //Set Mapping
                  c_Entry.SetConciseEntry(u16_ObjectIndex, static_cast<uint8>(u16_SigIt + 1U),
                                          c_Payload.size(), c_Payload, "Set Mapping");
                  orc_ConciseEntries.push_back(c_Entry);
               }

               c_Payload.clear();
               c_Payload.resize(1U);
               c_Payload[0] = static_cast<uint8>(rc_CurrentPdo.c_Signals.size());

               //Set number of mappings
               c_Entry.SetConciseEntry(u16_ObjectIndex, 0x00U, c_Payload.size(), c_Payload, "Set number of mappings");
               orc_ConciseEntries.push_back(c_Entry);
            }
         }

         //return to "old" index pattern
         if (oq_IsTx)
         {
            u16_ObjectIndex = mhu16_OD_INDEX_FIRST_RX_PDO;
         }
         else
         {
            u16_ObjectIndex = mhu16_OD_INDEX_FIRST_TX_PDO;
         }

         u16_ObjectIndex += rc_CurrentPdo.u16_CanOpenManagerPdoIndex;

         //check if COB-ID is missing or RO
         u32_Return = c_OD.IsCobIdRo(rc_CurrentPdo.u16_CanOpenManagerPdoIndex, oq_IsTx, q_SectionRO);

         if ((u32_Return == C_NO_ERR) && (q_SectionRO == false))
         {
            u32_CanId = rc_CurrentPdo.u32_CanId;

            if (rc_CurrentPdo.q_IsExtended)
            {
               u32_CanId = u32_CanId | 0x20000000UL;
            }

            c_Payload.clear();
            //COB-ID is a uint32 value
            c_Payload.resize(4U);
            c_Payload[0] = static_cast<uint8>(u32_CanId);
            c_Payload[1] = static_cast<uint8>(u32_CanId >> 8U);
            c_Payload[2] = static_cast<uint8>(u32_CanId >> 16U);
            c_Payload[3] = static_cast<uint8>(u32_CanId >> 24U);

            //Set and enable COB-ID
            c_Entry.SetConciseEntry(u16_ObjectIndex, 0x01U, c_Payload.size(), c_Payload, "Set and enable COB-ID");
            orc_ConciseEntries.push_back(c_Entry);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  creates sub-section for manager settings in manager configuration section in C file

   \param[in,out]   orc_Data        converted data to string list
   \param[in]       orc_ManInfo     CANopen manager definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_CreateManSettings(C_SCLStringList & orc_Data,
                                                    const C_OSCCanOpenManagerInfo & orc_ManInfo)
{
   const C_SCLString c_Autostart = orc_ManInfo.q_AutostartCanOpenManager ? "1U" : "0U";
   C_SCLString c_Define;

   orc_Data.Append("      " + C_SCLString::IntToStr(
                      static_cast<uint32>(orc_ManInfo.u8_NodeIDValue)) + "U, ///< Manager Node ID");
   orc_Data.Append("      " + c_Autostart + ", ///<  Autostart enabled");

   mh_GetDefineNMTStart(c_Define, orc_ManInfo.q_StartDevices, orc_ManInfo.q_NMTStartAll);
   orc_Data.Append("      " + c_Define + ",");

   mh_GetDefineNMTStop(c_Define, orc_ManInfo.e_NMTErrorBehaviour);
   orc_Data.Append("      " + c_Define + ",");

   orc_Data.Append("      " + C_SCLString::IntToStr(
                      static_cast<uint32>(orc_ManInfo.u16_HeartbeatProducerTimeMs)) +
                   "U, ///< Heartbeat producer time");
   orc_Data.Append("      " + C_SCLString::IntToStr(static_cast<uint32>(orc_ManInfo.u16_GlobalSDOTimeoutMs)) +
                   "U, ///< SDO timeout");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  creates sub-section for PDO configuration in manager configuration section in C file

   \param[in,out]   orc_Data              converted data to string list
   \param[in]       orc_DatapoolName      name of datapool which holds these PDO(s)
   \param[in]       ou8_InterfaceIndex    index of interface
   \param[in]       oq_TxExists           at least one TX PDO is existent
   \param[in]       oq_RxExists           at least one RX PDO is existent
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_CreatePdoConfig(C_SCLStringList & orc_Data, const C_SCLString & orc_DatapoolName,
                                                  const uint8 ou8_InterfaceIndex, const bool oq_TxExists,
                                                  const bool oq_RxExists)
{
   orc_Data.Append("      OSCO_MAN_CAN" + C_SCLString::IntToStr(
                      static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_TX_PDOS,");
   orc_Data.Append("      OSCO_MAN_CAN" + C_SCLString::IntToStr(
                      static_cast<uint32>(ou8_InterfaceIndex) + 1U) + "_NUMBER_OF_RX_PDOS,");
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

   orc_Data.Append("      &gt_" + orc_DatapoolName + "_DataPool ///< Datapool containing signal values");
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
void C_OSCExportCanOpenConfig::mh_ConvertSignalsToStrings(C_SCLStringList & orc_Data,
                                                          const C_OSCNodeDataPoolList & orc_DatapoolList,
                                                          const std::vector<C_OSCCanSignal> & orc_Signals,
                                                          const uint32 ou32_SignalListIndex,
                                                          const bool oq_RemoveLastComma)
{
   //get type of element via datapool_index, data_list_index, element_index
   for (uint16 u16_SignalIndex = 0; u16_SignalIndex < orc_Signals.size(); ++u16_SignalIndex)
   {
      const C_OSCCanSignal c_Signal = orc_Signals[u16_SignalIndex];
      const uint32 u32_DataElementIndex = c_Signal.u32_ComDataElementIndex;
      const C_OSCNodeDataPoolContent::E_Type e_ElementType =
         orc_DatapoolList.c_Elements[u32_DataElementIndex].c_Value.GetType();
      C_SCLString c_Text;

      c_Text = "   { OSY_DPA_ELEMENT_TYPE_" + C_OSCExportUti::h_GetElementTypeAsString(e_ElementType) + ", " +
               C_SCLString::IntToStr(ou32_SignalListIndex) + "U, " + C_SCLString::IntToStr(u32_DataElementIndex) +
               "U }";

      if ((oq_RemoveLastComma == false) || (u16_SignalIndex < (orc_Signals.size() - 1U)))
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
C_SCLString C_OSCExportCanOpenConfig::mh_GetMagicName(const C_SCLString & orc_ProjectId, const uint8 ou8_InterfaceIndex)
{
   const C_SCLString c_MagicName = "OSCO_MAN_CAN" +
                                   C_SCLString::IntToStr(static_cast<uint32>(ou8_InterfaceIndex) + 1U) +
                                   "_PROJECT_ID_" + orc_ProjectId;

   return c_MagicName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Evaluate combination of the two given flags and return the according define string. For more information
            on the flags see C_OSCCanOpenManagerInfo

   \param[in,out]   orc_String          storage for define string
   \param[in]       oq_StartDevices     flag if CANopen manager starts devices, or each own device
   \param[in]       oq_NMTStartAll      Starting devices using "NMT Start All" command

   \return Define string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_GetDefineNMTStart(C_SCLString & orc_String, const bool oq_StartDevices,
                                                    const bool oq_NMTStartAll)
{
   if ((oq_StartDevices == true) && (oq_NMTStartAll == false))
   {
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_START_DEVICES_YES";
   }
   else if ((oq_StartDevices == false) && (oq_NMTStartAll == false))
   {
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_START_DEVICES_NO";
   }
   else if ((oq_StartDevices == true) && (oq_NMTStartAll == true))
   {
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_START_DEVICES_START_ALL";
   }
   else
   {
      //combination oq_StartDevices == false && oq_NMTStartAll == false) is not valid!
      tgl_assert(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generates define string based on NMT behavior of CANopen manager

   \param[in,out]   orc_String         storage for define string
   \param[in]       oe_NMTBehavior     NMT error behavior definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_GetDefineNMTStop(C_SCLString & orc_String,
                                                   const C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType oe_NMTBehavior)
{
   switch (oe_NMTBehavior)
   {
   case C_OSCCanOpenManagerInfo::eRESTART_ALL_DEVICES:
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_ERROR_RESET_ALL_NODES";
      break;
   case C_OSCCanOpenManagerInfo::eRESTART_FAILURE_DEVICE:
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_ERROR_RESET_ERROR_NODE";
      break;
   case C_OSCCanOpenManagerInfo::eSTOP_ALL_DEVICES:
      orc_String = "OSCO_MAN_MANAGER_SETTINGS_NMT_ERROR_STOP_ALL";
      break;
   default:
      //should not happen
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Evaluates C_OSCCanOpenManagerDeviceInfo::u8_ResetNodeObjectDictionarySubIndex and composes comment for
            concise entry

   \param[in,out]   orc_Comment   storage for comment value
   \param[in]       ou8_Value     enum value to evaluate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCExportCanOpenConfig::mh_GetRestoreValueComment(C_SCLString & orc_Comment, const uint8 ou8_Value)
{
   switch (ou8_Value)
   {
   case 0U:
      orc_Comment = "Restore All Parameters";
      break;
   case 1U:
      orc_Comment = "Restore Communication Related Parameters";
      break;
   case 2U:
      orc_Comment = "Restore Application related Parameters";
      break;
   default:
      orc_Comment = "Restore Factory Defaults";
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
uint32 C_OSCExportCanOpenConfig::mh_GetTotalNumOfPdoSignals(const C_OSCCanMessageContainer & orc_MsgContainer,
                                                            const bool oq_IsTx)
{
   uint32 u32_Retval = 0;

   std::vector<C_OSCCanMessage> c_Pdos = oq_IsTx ? orc_MsgContainer.c_TxMessages : orc_MsgContainer.c_RxMessages;

   for (uint16 u16_MsgIndex = 0U; u16_MsgIndex < c_Pdos.size(); ++u16_MsgIndex)
   {
      u32_Retval += c_Pdos[u16_MsgIndex].c_Signals.size();
   }

   return u32_Retval;
}
