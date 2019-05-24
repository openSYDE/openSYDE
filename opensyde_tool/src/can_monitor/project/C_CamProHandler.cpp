//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project data handler for can monitor (implementation)

   Project data handler for can monitor

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDir>

#include "C_Uti.h"
#include "TGLFile.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_UsHandler.h"
#include "C_OSCXMLParser.h"
#include "C_CamProHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_CamProHandlerFiler.h"
#include "cam_constants.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_CamProHandler C_CamProHandler::mhc_Instance;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all user configured messages

   \return
   All user configured messages
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_CamProMessageData> & C_CamProHandler::GetMessages(void) const
{
   return this->mc_Messages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get message

   \param[in] ou32_Index Message index

   \return
   NULL Message not found
   Else Valid message
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CamProMessageData * C_CamProHandler::GetMessageConst(const uint32 ou32_Index) const
{
   const C_CamProMessageData * pc_Retval = NULL;

   if (ou32_Index < this->mc_Messages.size())
   {
      pc_Retval = &this->mc_Messages[ou32_Index];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN DLL Path

   \return CAN DLL Path string (Peak/Vector/Custom)
*/
//----------------------------------------------------------------------------------------------------------------------
const QString C_CamProHandler::GetCANDllPath() const
{
   QString c_Return;

   switch (this->me_CANDllType)
   {
   case ePEAK:
      c_Return = mc_DLL_PATH_PEAK_CAM;
      break;
   case eVECTOR:
      c_Return = mc_DLL_PATH_VECTOR_CAM;
      break;
   case eOTHER:
      c_Return = this->mc_CustomCANDllPath;
      break;
   default:
      c_Return = mc_DLL_PATH_PEAK_CAM;
      break;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get custom CAN DLL Path

   \return CAN DLL Path string (custom)
*/
//----------------------------------------------------------------------------------------------------------------------
const QString C_CamProHandler::GetCustomCANDllPath() const
{
   return this->mc_CustomCANDllPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CAN Dll type. (PEAK = 0,Vector = 1, Other = 2)

   \return   CAN Dll type
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProHandler::E_CANDllType C_CamProHandler::GetCANDllType() const
{
   return this->me_CANDllType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all user configured filters.

   \return
   All configured filters.
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_CamProFilterData> & C_CamProHandler::GetFilters() const
{
   return this->mc_Filters;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Get disabled/enabled state of whole filter widget.

   \return
   true: enabled; false: disabled
*/
//----------------------------------------------------------------------------------------------------------------------
const bool & C_CamProHandler::GetFilterWidgetEnabled() const
{
   return this->mq_FiltersActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all configured databases.

   \return
   All configured databases.
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_CamProDatabaseData> & C_CamProHandler::GetDatabases() const
{
   return this->mc_Databases;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get logging data.

   \return
   logging data structure
*/
//----------------------------------------------------------------------------------------------------------------------
const C_CamProLoggingData & C_CamProHandler::GetLoggingData() const
{
   return this->mc_LoggingData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set all messages

   \param[in] orc_Messages All messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetMessages(const std::vector<C_CamProMessageData> & orc_Messages)
{
   this->mc_Messages = orc_Messages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message name value

   \param[in] ou32_Index Message index
   \param[in] orc_Name   New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetMessageName(const uint32 ou32_Index, const QString & orc_Name)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Messages.size())
   {
      C_CamProMessageData & rc_Message = this->mc_Messages[ou32_Index];
      rc_Message.c_Name = orc_Name.toStdString().c_str();
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message uint32 value

   \param[in] ou32_Index  Message index
   \param[in] oe_Selector Data specifier
   \param[in] ou32_Value  New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetMessageUint32Value(const uint32 ou32_Index,
                                              const C_CamProMessageData::E_GenericUint32DataSelector oe_Selector,
                                              const uint32 ou32_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Messages.size())
   {
      C_CamProMessageData & rc_Message = this->mc_Messages[ou32_Index];
      rc_Message.SetMessageUint32Value(oe_Selector, ou32_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message bool value

   \param[in] ou32_Index  Message index
   \param[in] oe_Selector Data specifier
   \param[in] oq_Value    New value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetMessageBoolValue(const uint32 ou32_Index,
                                            const C_CamProMessageData::E_GenericBoolDataSelector oe_Selector,
                                            const bool oq_Value)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Messages.size())
   {
      C_CamProMessageData & rc_Message = this->mc_Messages[ou32_Index];
      rc_Message.SetMessageBoolValue(oe_Selector, oq_Value);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message key related values

   \param[in] ou32_Index  Message index
   \param[in] orc_Key     New key (single character only)
   \param[in] ou32_Offset Key trigger offset (ms)

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetMessageKey(const uint32 ou32_Index, const QString & orc_Key, const uint32 ou32_Offset)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Messages.size())
   {
      C_CamProMessageData & rc_Message = this->mc_Messages[ou32_Index];
      rc_Message.SetMessageKey(orc_Key, ou32_Offset);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set CAN message data bytes

   \param[in] ou32_Index    Message index
   \param[in] orc_DataBytes New data byte content

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetMessageDataBytes(const uint32 ou32_Index, const std::vector<uint8> & orc_DataBytes)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Messages.size())
   {
      C_CamProMessageData & rc_Message = this->mc_Messages[ou32_Index];
      rc_Message.SetMessageDataBytes(orc_DataBytes);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set CAN DLL Path.

   \param[in]     orc_CANDllPath    New CAN DLL path.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetCustomCANDllPath(const QString & orc_CANDllPath)
{
   this->mc_CustomCANDllPath = orc_CANDllPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set DLL type. See get for type definition.

   \param[in]  oe_CANDllType  CAN DLL type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetCANDllType(const E_CANDllType oe_CANDllType)
{
   this->me_CANDllType = oe_CANDllType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set all filters.

   \param[in] orc_Filters   All filters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetFilters(const std::vector<C_CamProFilterData> & orc_Filters)
{
   this->mc_Filters = orc_Filters;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new filter configuration for already existing filter.

   \param[in]     ou32_Index        index of filter
   \param[in]     orc_NewFilter     new filter configuration

   \return
   C_NO_ERR    everything worked
   C_RANGE     specified index does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetFilter(const uint32 ou32_Index, const C_CamProFilterData & orc_NewFilter)
{
   sint32 s32_Return;

   if (ou32_Index < this->mc_Filters.size())
   {
      s32_Return = C_NO_ERR;
      this->mc_Filters[ou32_Index] = orc_NewFilter;
   }
   else
   {
      s32_Return = C_RANGE;
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable/enable filter.

   \param[in]     ou32_Index     index of filter
   \param[in]     oq_Enable      true: enable; false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetFilterEnabled(const uint32 ou32_Index, const bool oq_Enable)
{
   this->mc_Filters[ou32_Index].q_Enabled = oq_Enable;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Disable/enable whole filter widget.

   \param[in]     oq_Enable      true: enable; false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetFilterWidgetEnabled(const bool oq_Enable)
{
   this->mq_FiltersActive = oq_Enable;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set all databases

   \param[in] orc_Databases   All databases
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetDatabases(const std::vector<C_CamProDatabaseData> & orc_Databases)
{
   this->mc_Databases = orc_Databases;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set database name value

   \param[in] ou32_Index      database index
   \param[in] orc_NewName     New name

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetDatabaseName(const uint32 ou32_Index, const QString & orc_NewName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Databases.size())
   {
      C_CamProDatabaseData & rc_Database = this->mc_Databases[ou32_Index];
      rc_Database.c_Name = orc_NewName;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set database name value

   \param[in] ou32_Index         database index
   \param[in] os32_NewBusIndex   new bus index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SetDatabaseOsySysDefBus(const uint32 ou32_Index, const sint32 os32_NewBusIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_Index < this->mc_Databases.size())
   {
      C_CamProDatabaseData & rc_Database = this->mc_Databases[ou32_Index];
      rc_Database.s32_BusIndex = os32_NewBusIndex;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disable/enable database.

   \param[in]     ou32_Index     index of database
   \param[in]     oq_Enable      true: enable; false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetDatabaseEnabled(const uint32 ou32_Index, const bool oq_Enable)
{
   this->mc_Databases[ou32_Index].q_Enabled = oq_Enable;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging data.

   \param[in]     orc_LoggingData       logging data structure
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetLoggingData(const C_CamProLoggingData & orc_LoggingData)
{
   this->mc_LoggingData = orc_LoggingData;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging directory.

   \param[in]     orc_NewDir      new logging directory.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetLoggingDirectory(const QString & orc_NewDir)
{
   this->mc_LoggingData.c_Directory = orc_NewDir;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging file name.

   \param[in]     orc_NewName      new logging file name.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetLoggingFileName(const QString & orc_NewName)
{
   this->mc_LoggingData.c_FileName = orc_NewName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging enabled/disabled.

   \param[in]     oq_Enabled      Enabled/disabled flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetLoggingEnabled(const bool oq_Enabled)
{
   this->mc_LoggingData.q_Enabled = oq_Enabled;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging overwrite mode.

   \param[in]     oe_NewOverwriteMode        new overwrite mode (timestamp/ask/overwrite)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetLoggingOverwriteMode(const C_CamProLoggingData::E_OverwriteMode oe_NewOverwriteMode)
{
   this->mc_LoggingData.e_OverwriteMode = oe_NewOverwriteMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging file format.

   \param[in]     oe_NewFormat       logging file format (ASC/BLF)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::SetLoggingFormat(const C_CamProLoggingData::E_Format oe_NewFormat)
{
   this->mc_LoggingData.e_FileFormat = oe_NewFormat;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new message to existing ones

   \param[in] ou32_Index       New message index
   \param[in] orc_NewMessage   New message initial data
   \param[in] oq_AllowAdaption Optional flag to disable automatic adaption
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::InsertMessage(const uint32 ou32_Index, const C_CamProMessageData & orc_NewMessage,
                                    const bool oq_AllowAdaption)
{
   C_CamProMessageData c_MessageData = orc_NewMessage;

   if (oq_AllowAdaption == true)
   {
      //Only allow name adaptation if name is not necessary for database identification
      if (c_MessageData.c_DataBaseFilePath.IsEmpty())
      {
         c_MessageData.c_Name = C_Uti::h_GetUniqueName(this->m_GetAllMessageNames(), c_MessageData.c_Name);
      }
   }

   if (ou32_Index < this->mc_Messages.size())
   {
      this->mc_Messages.insert(this->mc_Messages.begin() + ou32_Index, c_MessageData);
   }
   else
   {
      this->mc_Messages.push_back(c_MessageData);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new filter to existing ones.

   \param[in]     orc_NewFilter     new filter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::AddFilter(const C_CamProFilterData & orc_NewFilter)
{
   this->mc_Filters.push_back(orc_NewFilter);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new database to existing ones.

   \param[in]     orc_NewDatabase     new database
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::AddDatabase(const C_CamProDatabaseData & orc_NewDatabase)
{
   this->mc_Databases.push_back(orc_NewDatabase);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Move message

   \param[in] ou32_StartIndex  Current message index
   \param[in] ou32_TargetIndex Desired message index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::MoveMessage(const uint32 ou32_StartIndex, const uint32 ou32_TargetIndex)
{
   sint32 s32_Retval;

   if ((ou32_StartIndex < this->mc_Messages.size()) && (ou32_TargetIndex < this->mc_Messages.size()))
   {
      const C_CamProMessageData c_Copy = this->mc_Messages[ou32_StartIndex];
      s32_Retval = C_NO_ERR;
      tgl_assert(this->DeleteMessage(ou32_StartIndex) == C_NO_ERR);
      this->InsertMessage(ou32_TargetIndex, c_Copy, false);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specified message

   \param[in] ou32_Index Message index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::DeleteMessage(const uint32 ou32_Index)
{
   sint32 s32_Retval;

   if (ou32_Index < this->mc_Messages.size())
   {
      s32_Retval = C_NO_ERR;
      this->mc_Messages.erase(this->mc_Messages.begin() + ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specified filter

   \param[in] ou32_Index   Filter index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::DeleteFilter(const uint32 ou32_Index)
{
   sint32 s32_Retval;

   if (ou32_Index < this->mc_Filters.size())
   {
      s32_Retval = C_NO_ERR;
      this->mc_Filters.erase(this->mc_Filters.begin() + ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specified database

   \param[in] ou32_Index   Database index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::DeleteDatabase(const uint32 ou32_Index)
{
   sint32 s32_Retval;

   if (ou32_Index < this->mc_Databases.size())
   {
      s32_Retval = C_NO_ERR;
      this->mc_Databases.erase(this->mc_Databases.begin() + ou32_Index);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all messages for the requested database file path

   \param[in]     orc_File                 Database file path to search for
   \param[in,out] opc_CompleteMessageNames Optional storage for found message names (may be not unique)
                                           -> includes database name
   \param[in,out] opc_MessageIndices       Optional storage for found message indices (ascending, unique)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::GetAllMessagesFromDatabase(const QString & orc_File, QStringList * const opc_CompleteMessageNames,
                                                 std::vector<uint32> * const opc_MessageIndices) const
{
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_Messages.size(); ++u32_ItMessage)
   {
      const C_CamProMessageData & rc_Message = this->mc_Messages[u32_ItMessage];
      //Check if same database
      if (orc_File.compare(rc_Message.c_DataBaseFilePath.c_str()) == 0)
      {
         //Match
         //Add
         if (opc_CompleteMessageNames != NULL)
         {
            const QString c_CompleteName = C_CamProHandler::h_GetCompleteMessageName(rc_Message);
            opc_CompleteMessageNames->append(c_CompleteName);
         }
         if (opc_MessageIndices != NULL)
         {
            opc_MessageIndices->push_back(u32_ItMessage);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the complete message name to display

   \param[in] orc_Message Message to get the name from

   \return
   Complete message name as expected
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamProHandler::h_GetCompleteMessageName(const C_CamProMessageData & orc_Message)
{
   QString c_Retval;

   if (orc_Message.c_DataBaseFilePath.IsEmpty() == true)
   {
      c_Retval = orc_Message.c_Name.c_str();
   }
   else
   {
      const QFileInfo c_Info(orc_Message.c_DataBaseFilePath.c_str());
      c_Retval = QString("%1::%2").arg(c_Info.baseName()).arg(orc_Message.c_Name.c_str());
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the current file path

   \return
   Current file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamProHandler::GetCurrentFilePath(void) const
{
   return this->mc_File;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current project directory (path of file without file name).

   Return path is always either absolute or empty.

   \return
   Current file directory.
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamProHandler::GetCurrentProjDir(void) const
{
   QString c_Return = "";

   if (this->mc_File != "")
   {
      c_Return = QFileInfo(this->mc_File).dir().absolutePath();
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load project

   Load project and store in information in our instance data.

   \param[in] orc_Path Path to system definition file

   \return
   C_NO_ERR    data read and placed into instance data
   C_RD_WR     problems accessing file system (e.g. no read access to file)
   C_RANGE     specified file does not exist (when loading)
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::LoadFromFile(const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;

      Q_EMIT (this->SigClearOldConfiguration());

      s32_Return = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = C_CamProHandlerFiler::h_Load(*this, c_XMLParser);
      }
      else
      {
         osc_write_log_error("Loading Project",
                             "File \"" + orc_Path + "\" could not be opened.");
         s32_Return = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading Project", "File \"" + orc_Path + "\" does not exist.");
      s32_Return = C_RANGE;
   }

   if (s32_Return == C_NO_ERR)
   {
      this->mc_File = orc_Path.c_str();

      // calculate hash
      this->mu32_FileHash = this->m_GetHash();

      // handle user settings
      C_UsHandler::h_GetInstance()->AddToRecentProjects(this->mc_File);
      C_UsHandler::h_GetInstance()->LoadActiveProject(this->mc_File);

      // emit signal last
      Q_EMIT (this->SigNewConfiguration());
   }
   else
   {
      // remove invalid project from recent projects
      C_UsHandler::h_GetInstance()->RemoveOfRecentProjects(orc_Path.c_str());
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project

   Save project to XML file.

   \param[in] orc_Path Path to project file

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (e.g. could not erase pre-existing file before saving)
   C_COM      could not create folder
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamProHandler::SaveToFile(const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Return = C_NO_ERR;
   QFileInfo c_File(orc_Path.c_str());
   QDir c_Directory(c_File.absolutePath());

   if (c_Directory.exists() == false)
   {
      if (c_Directory.mkpath(".") == false)
      {
         osc_write_log_error("Saving Project",
                             QString(
                                "Could not create folder for file \"" + c_File.absolutePath() +
                                "\".").toStdString().c_str());
         s32_Return = C_COM;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      if (TGL_FileExists(orc_Path) == true)
      {
         //erase it:
         sintn sn_Return;
         sn_Return = std::remove(orc_Path.c_str());
         if (sn_Return != 0)
         {
            osc_write_log_error("Saving project",
                                "Could not erase pre-existing file \"" + orc_Path + "\".");
            s32_Return = C_RD_WR;
         }
      }
      if (s32_Return == C_NO_ERR)
      {
         C_OSCXMLParser c_XMLParser;
         C_CamProHandlerFiler::h_Save(*this, c_XMLParser);

         s32_Return = c_XMLParser.SaveToFile(orc_Path);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Saving Project", "Could not write to file \"" + orc_Path + "\".");
            s32_Return = C_RD_WR;
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      this->mc_File = orc_Path.c_str();
      this->mu32_FileHash = this->m_GetHash();
      C_UsHandler::h_GetInstance()->AddToRecentProjects(this->mc_File);
      C_UsHandler::h_GetInstance()->ChangeActiveProjectName(this->mc_File);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if there are any changes

   \return
   True  Unsaved changes
   False No changes
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamProHandler::CheckChanges(void) const
{
   const uint32 u32_CurHas = this->m_GetHash();

   return u32_CurHas != this->mu32_FileHash;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all content

   \param[in] oq_UpdateUserSettings Flag to update the user settings (avoid Qt access in constructor)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::Clear(const bool oq_UpdateUserSettings)
{
   Q_EMIT (this->SigClearOldConfiguration());

   this->mc_Messages.clear();
   this->mc_Filters.clear();
   this->mq_FiltersActive = true;
   this->mc_Databases.clear();
   this->mc_File = "";
   this->me_CANDllType = ePEAK;
   this->mc_CustomCANDllPath = "";
   this->mc_LoggingData.Clear();

   this->mu32_FileHash = this->m_GetHash();
   if (oq_UpdateUserSettings)
   {
      C_UsHandler::h_GetInstance()->LoadActiveProject(this->mc_File);
   }

   //Do signal last
   Q_EMIT (this->SigNewConfiguration());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProHandler * C_CamProHandler::h_GetInstance(void)
{
   return &C_CamProHandler::mhc_Instance;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProHandler::C_CamProHandler(void) :
   mu32_FileHash(0UL),
   mq_FiltersActive(true),
   me_CANDllType(ePEAK)
{
   //init hash
   Clear(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get hash for current project

   \return
   possible return value(s) and description
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_CamProHandler::m_GetHash(void) const
{
   uint32 u32_Retval = 0xFFFFFFFFUL;

   m_CalcHash(u32_Retval);
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamProHandler::m_CalcHash(uint32 & oru32_HashValue) const
{
   // messages
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_Messages.size(); ++u32_ItMessage)
   {
      this->mc_Messages[u32_ItMessage].CalcHash(oru32_HashValue);
   }

   // settings
   // CAN DLL configuration
   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_CustomCANDllPath.toStdString().c_str(),
                                      this->mc_CustomCANDllPath.size(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->me_CANDllType, sizeof(this->me_CANDllType), oru32_HashValue);

   // filters
   for (uint32 u32_ItFilters = 0UL; u32_ItFilters < this->mc_Filters.size(); ++u32_ItFilters)
   {
      this->mc_Filters[u32_ItFilters].CalcHash(oru32_HashValue);
   }

   // databases
   for (uint32 u32_ItDatabases = 0UL; u32_ItDatabases < this->mc_Databases.size(); ++u32_ItDatabases)
   {
      this->mc_Databases[u32_ItDatabases].CalcHash(oru32_HashValue);
   }

   //logging
   this->mc_LoggingData.CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all current message names

   Warning: local use only

   \return
   All current message names
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<stw_scl::C_SCLString, bool> C_CamProHandler::m_GetAllMessageNames(void) const
{
   std::map<stw_scl::C_SCLString, bool> c_Retval;
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < this->mc_Messages.size(); ++u32_ItMessage)
   {
      const C_CamProMessageData & rc_Message = this->mc_Messages[u32_ItMessage];
      c_Retval[rc_Message.c_Name] = true;
   }
   return c_Retval;
}
