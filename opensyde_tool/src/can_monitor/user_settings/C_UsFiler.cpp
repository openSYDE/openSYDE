//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle save'n load for user settings (implementation)

   Handle save'n load for user settings

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>
#include <QDir>
#include "stwerrors.h"
#include "C_Uti.h"
#include "C_UsFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui_logic;
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const std::string C_UsFiler::mhc_TRACE_COL_WIDTH_IDENTIFIER = "TraceColWidths";
const std::string C_UsFiler::mhc_TRACE_COL_POSITION_IDENTIFIER = "TraceColPositions";
const std::string C_UsFiler::mhc_MESSAGE_GENERATOR_IDENTIFIER = "MessageGeneratorColWidths";
const std::string C_UsFiler::mhc_MESSAGE_GENERATOR_SIGNALS_IDENTIFIER = "MessageSignalsColWidths";
const std::string C_UsFiler::mhc_MESSAGE_GENERATOR_SELECTION_IDENTIFIER = "MessageSelection";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsFiler::C_UsFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user setting to default ini file

   \param[in] orc_UserSettings  User settings to save
   \param[in] orc_Path          File path
   \param[in] orc_ActiveProject Actual project to save project specific settings
                                Empty string results in saving no informations

   \return
   C_NO_ERR: OK
   C_RANGE:  Parameter invalid
   C_NOACT:  File open failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_UsFiler::h_Save(const C_UsHandler & orc_UserSettings, const QString & orc_Path,
                         const QString & orc_ActiveProject)
{
   sint32 s32_Retval;

   if (orc_Path.compare("") != 0)
   {
      s32_Retval = C_NO_ERR;
      {
         //Helper to seperate path and file name
         const QFileInfo c_File(orc_Path);
         //Check if directory exists
         const QDir c_Dir(c_File.path());
         if (c_Dir.exists() == false)
         {
            c_Dir.mkpath(".");
         }
      }
      try
      {
         //Parse ini
         C_SCLIniFile c_Ini(orc_Path.toStdString().c_str());
         mh_SaveRecentProjects(orc_UserSettings, c_Ini);
         mh_SaveProjectIndependentSection(orc_UserSettings, c_Ini);
         mh_SaveProjectDependentSection(orc_UserSettings, c_Ini, orc_ActiveProject);
      }
      catch (...)
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all values of ini file

   If ini not existing set default values.

   \param[in,out] orc_UserSettings  User settings to load
   \param[in]     orc_Path          File path
   \param[in]     orc_ActiveProject Actual project to load project specific settings.
                                    Empty string results in default values

   \return
   C_NO_ERR: OK
   C_RANGE:  Parameter invalid
   C_NOACT:  File open failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_UsFiler::h_Load(C_UsHandler & orc_UserSettings, const QString & orc_Path, const QString & orc_ActiveProject)
{
   sint32 s32_Retval;

   if (orc_Path.compare("") != 0)
   {
      try
      {
         C_SCLIniFile c_Ini(orc_Path.toStdString().c_str());
         s32_Retval = C_NO_ERR;

         orc_UserSettings.SetDefault();
         if (orc_ActiveProject == "")
         {
            // load recent projects only if no active project is given
            // (else it was already added to RecentProjects and hence a call to LoadRecentProjects would override it)
            mh_LoadRecentProjects(orc_UserSettings, c_Ini);
         }
         mh_LoadProjectIndependentSection(orc_UserSettings, c_Ini);
         mh_LoadProjectDependentSection(orc_UserSettings, c_Ini, orc_ActiveProject);
      }
      catch (...)
      {
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save recent projects part of user settings

   \param[in]     orc_UserSettings User settings
   \param[in,out] orc_Ini          Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveRecentProjects(const C_UsHandler & orc_UserSettings, C_SCLIniFile & orc_Ini)
{
   const QStringList c_List = orc_UserSettings.GetRecentProjects();

   // clear recent projects section (the ini file can only add keys and does not delete keys that do not exist anymore)
   if (orc_Ini.SectionExists("RecentProjects") == true)
   {
      orc_Ini.EraseSection("RecentProjects");
   }

   //Recent projects
   for (uint8 u8_It = 0; u8_It < c_List.count(); ++u8_It)
   {
      orc_Ini.WriteString("RecentProjects", C_SCLString::IntToStr(
                             u8_It), c_List.at(u8_It).toStdString().c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project independent part of user settings

   \param[in]     orc_UserSettings User settings
   \param[in,out] orc_Ini          Ini handler
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveProjectIndependentSection(const C_UsHandler & orc_UserSettings, C_SCLIniFile & orc_Ini)
{
   //Screen position
   orc_Ini.WriteInteger("Screen", "Position_x", orc_UserSettings.GetScreenPos().x());
   orc_Ini.WriteInteger("Screen", "Position_y", orc_UserSettings.GetScreenPos().y());

   // Application size
   orc_Ini.WriteInteger("Screen", "Size_width", orc_UserSettings.GetAppSize().width());
   orc_Ini.WriteInteger("Screen", "Size_height", orc_UserSettings.GetAppSize().height());

   // Application maximizing flag
   orc_Ini.WriteBool("Screen", "Size_maximized", orc_UserSettings.GetAppMaximized());

   //Buttons
   orc_Ini.WriteBool("Buttons", "Hex", orc_UserSettings.GetButtonHexActive());
   orc_Ini.WriteBool("Buttons", "TimeStamp", orc_UserSettings.GetButtonRelativeTimeStampActive());
   orc_Ini.WriteBool("Buttons", "Unique", orc_UserSettings.GetButtonUniqueViewActive());

   //Trace settings
   orc_Ini.WriteBool("Trace_Settings", "TimeStampAbsoluteTimeOfDay",
                     orc_UserSettings.GetTraceSettingDisplayTimestampAbsoluteTimeOfDay());
   orc_Ini.WriteInteger("Trace_Settings", "TraceBufferSize", orc_UserSettings.GetTraceSettingBufferSize());

   //Protocol
   orc_Ini.WriteInteger("Protocol", "Value", orc_UserSettings.GetSelectedProtocolIndex());

   // Message generator splitter
   orc_Ini.WriteInteger("Layout", "MessageGenSplitter_y", orc_UserSettings.GetSplitterMessageGenY());
   orc_Ini.WriteBool("Layout", "MessageGen_expanded", orc_UserSettings.GetMessageGenIsExpanded());

   // Settings splitter
   orc_Ini.WriteInteger("Layout", "SettingsSplitter_x", orc_UserSettings.GetSplitterSettingsX());
   orc_Ini.WriteBool("Layout", "Settings_expanded", orc_UserSettings.GetSettingsAreExpanded());

   // Messages signals splitter
   orc_Ini.WriteInteger("Layout", "MessagesSignalsSplitter_x", orc_UserSettings.GetSplitterMesSigX());

   // Settings expanded collapsed
   orc_Ini.WriteBool("Settings", "DatabaseExpanded", orc_UserSettings.GetWiDatabaseExpanded());
   orc_Ini.WriteBool("Settings", "DllExpanded", orc_UserSettings.GetWiDllConfigExpanded());
   orc_Ini.WriteBool("Settings", "FilterExpanded", orc_UserSettings.GetWiFilterExpanded());
   orc_Ini.WriteBool("Settings", "LoggingExpanded", orc_UserSettings.GetWiLoggingExpanded());
   orc_Ini.WriteInteger("Settings", "PopOpenSection", static_cast<sint32>(orc_UserSettings.GetPopOpenSection()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project dependent part of user settings

   \param[in]     orc_UserSettings User settings
   \param[in,out] orc_Ini          Ini handler
   \param[in] orc_ActiveProject    Actual project to save project specific settings
                                   Empty string results in saving no information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveProjectDependentSection(const C_UsHandler & orc_UserSettings, C_SCLIniFile & orc_Ini,
                                               const QString & orc_ActiveProject)
{
   if (orc_ActiveProject != "")
   {
      // project specific settings

      // message generator
      mh_SaveColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_TRACE_COL_WIDTH_IDENTIFIER,
                     orc_UserSettings.GetTraceColWidths());
      mh_SaveColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_TRACE_COL_POSITION_IDENTIFIER,
                     orc_UserSettings.GetTraceColPositions());
      mh_SaveColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_MESSAGE_GENERATOR_IDENTIFIER,
                     orc_UserSettings.GetMessageColWidths());
      mh_SaveColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_MESSAGE_GENERATOR_SIGNALS_IDENTIFIER,
                     orc_UserSettings.GetSignalsColWidths());
      mh_SaveColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_MESSAGE_GENERATOR_SELECTION_IDENTIFIER,
                     orc_UserSettings.GetSelectedMessages());

      // settings section
      orc_Ini.WriteString(orc_ActiveProject.toStdString().c_str(), "LastKnownDatabasePath",
                          orc_UserSettings.GetLastKnownDatabasePath().toStdString().c_str());
   }

   //lint -e1764 This function is necessary for future use so keep interface as necessary
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI recent projects section

   \param[in,out] orc_UserSettings User settings
   \param[in,out] orc_Ini          Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadRecentProjects(C_UsHandler & orc_UserSettings, C_SCLIniFile & orc_Ini)
{
   QStringList c_List;
   QString c_Cur;

   //Recent projects
   c_List.clear();
   for (uint8 u8_It = 0; u8_It < C_UsHandler::h_GetMaxRecentProjects(); ++u8_It)
   {
      c_Cur =
         orc_Ini.ReadString("RecentProjects", C_SCLString::IntToStr(u8_It), "").c_str();
      if (c_Cur.compare("") != 0)
      {
         QFileInfo c_File;
         if (c_Cur.startsWith(".") == true)
         {
            c_File.setFile(C_Uti::h_GetExePath() + c_Cur);
         }
         else
         {
            c_File.setFile(c_Cur);
         }
         if (c_File.exists() == true)
         {
            c_List.append(c_File.absoluteFilePath());
         }
      }
   }
   orc_UserSettings.SetRecentProjects(c_List);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI project independent section

   \param[in,out] orc_UserSettings User settings
   \param[in,out] orc_Ini          Current ini
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadProjectIndependentSection(C_UsHandler & orc_UserSettings, C_SCLIniFile & orc_Ini)
{
   QPoint c_Pos;
   QSize c_Size;
   bool q_Flag;
   sint32 s32_Value;

   // Screen position
   c_Pos.setX(orc_Ini.ReadInteger("Screen", "Position_x", 50));
   c_Pos.setY(orc_Ini.ReadInteger("Screen", "Position_y", 50));
   orc_UserSettings.SetScreenPos(c_Pos);

   // Application size
   c_Size.setWidth(orc_Ini.ReadInteger("Screen", "Size_width", 1000));
   c_Size.setHeight(orc_Ini.ReadInteger("Screen", "Size_height", 700));
   orc_UserSettings.SetAppSize(c_Size);

   // Application maximizing flag
   q_Flag = orc_Ini.ReadBool("Screen", "Size_maximized", true);
   orc_UserSettings.SetAppMaximized(q_Flag);

   // Buttons
   q_Flag = orc_Ini.ReadBool("Buttons", "Hex", true);
   orc_UserSettings.SetButtonHexActive(q_Flag);
   q_Flag = orc_Ini.ReadBool("Buttons", "TimeStamp", false);
   orc_UserSettings.SetButtonRelativeTimeStampActive(q_Flag);
   q_Flag = orc_Ini.ReadBool("Buttons", "Unique", true);
   orc_UserSettings.SetButtonUniqueViewActive(q_Flag);

   //Trace settings
   q_Flag = orc_Ini.ReadBool("Trace_Settings", "TimeStampAbsoluteTimeOfDay", false);
   orc_UserSettings.SetTraceSettingDisplayTimestampAbsoluteTimeOfDay(q_Flag);
   s32_Value = orc_Ini.ReadInteger("Trace_Settings", "TraceBufferSize", 1000);
   orc_UserSettings.SetTraceSettingBufferSize(static_cast<uint32>(s32_Value));

   // Protocol
   s32_Value = orc_Ini.ReadInteger("Protocol", "Value", 0);
   orc_UserSettings.SetSelectedProtocolIndex(s32_Value);

   // Message Generator splitter
   s32_Value = orc_Ini.ReadInteger("Layout", "MessageGenSplitter_y", 348);
   orc_UserSettings.SetSplitterMessageGenY(s32_Value);
   q_Flag = orc_Ini.ReadBool("Layout", "MessageGen_expanded", true);
   orc_UserSettings.SetMessageGenIsExpanded(q_Flag);

   // Settings splitter
   s32_Value = orc_Ini.ReadInteger("Layout", "SettingsSplitter_x", 0);
   orc_UserSettings.SetSplitterSettingsX(s32_Value);
   q_Flag = orc_Ini.ReadBool("Layout", "Settings_expanded", true);
   orc_UserSettings.SetSettingsAreExpanded(q_Flag);

   // Messages signals splitter
   s32_Value = orc_Ini.ReadInteger("Layout", "MessagesSignalsSplitter_x", 1005);
   orc_UserSettings.SetSplitterMesSigX(s32_Value);

   // Settings expanded collapsed
   q_Flag = orc_Ini.ReadBool("Settings", "DatabaseExpanded", true);
   orc_UserSettings.SetWiDatabaseExpanded(q_Flag);
   q_Flag = orc_Ini.ReadBool("Settings", "DllExpanded", true);
   orc_UserSettings.SetWiDllConfigExpanded(q_Flag);
   q_Flag = orc_Ini.ReadBool("Settings", "FilterExpanded", true);
   orc_UserSettings.SetWiFilterExpanded(q_Flag);
   q_Flag = orc_Ini.ReadBool("Settings", "LoggingExpanded", true);
   orc_UserSettings.SetWiLoggingExpanded(q_Flag);
   s32_Value = orc_Ini.ReadInteger("Settings", "PopOpenSection",
                                   static_cast<sint32>(C_UsHandler::E_SettingsSubSection::eNONE));
   orc_UserSettings.SetPopOpenSection(static_cast< C_UsHandler::E_SettingsSubSection>(s32_Value));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load INI project independent section

   \param[in,out] orc_UserSettings  User settings
   \param[in,out] orc_Ini           Current ini
   \param[in]     orc_ActiveProject Actual project to load project specific settings.
                                    Empty string results in default values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadProjectDependentSection(C_UsHandler & orc_UserSettings, C_SCLIniFile & orc_Ini,
                                               const QString & orc_ActiveProject)
{
   if (orc_ActiveProject != "")
   {
      std::vector<sint32> c_Columns;
      // project specific settings

      // message generator
      mh_LoadColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_TRACE_COL_WIDTH_IDENTIFIER, c_Columns);
      orc_UserSettings.SetTraceColWidths(c_Columns);
      mh_LoadColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_TRACE_COL_POSITION_IDENTIFIER, c_Columns);
      orc_UserSettings.SetTraceColPositions(c_Columns);
      mh_LoadColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_MESSAGE_GENERATOR_IDENTIFIER, c_Columns);
      orc_UserSettings.SetMessageColWidths(c_Columns);
      mh_LoadColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_MESSAGE_GENERATOR_SIGNALS_IDENTIFIER,
                     c_Columns);
      orc_UserSettings.SetSignalsColWidths(c_Columns);
      mh_LoadColumns(orc_Ini,
                     orc_ActiveProject.toStdString().c_str(), C_UsFiler::mhc_MESSAGE_GENERATOR_SELECTION_IDENTIFIER,
                     c_Columns);
      orc_UserSettings.SetSelectedMessages(c_Columns);

      // settings section
      orc_UserSettings.SetLastKnownDatabasePath(orc_Ini.ReadString(orc_ActiveProject.toStdString().c_str(),
                                                                   "LastKnownDatabasePath", "").c_str());
   }
   else
   {
      // Default values in case of new project
      orc_UserSettings.SetLastKnownDatabasePath("");
   }
   //lint -e1764 This function is necessary for future use so keep interface as necessary
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save columns

   \param[in,out] orc_Ini                Current ini
   \param[in]     orc_SectionName        Section name
   \param[in]     orc_IdentifierBaseName Identifier base name
   \param[in]     orc_Columns            Columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_SaveColumns(C_SCLIniFile & orc_Ini, const C_SCLString & orc_SectionName,
                               const std::string & orc_IdentifierBaseName, const std::vector<sint32> & orc_Columns)
{
   const QString c_CountId = static_cast<QString>("%1_count").arg(orc_IdentifierBaseName.c_str());

   orc_Ini.WriteInteger(orc_SectionName.c_str(), c_CountId.toStdString().c_str(), orc_Columns.size());
   for (uint32 u32_ItCol = 0UL; u32_ItCol < orc_Columns.size(); ++u32_ItCol)
   {
      const QString c_ItemId = static_cast<QString>("%1_%2").arg(orc_IdentifierBaseName.c_str()).arg(u32_ItCol);
      orc_Ini.WriteInteger(orc_SectionName.c_str(), c_ItemId.toStdString().c_str(), orc_Columns[u32_ItCol]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load columns

   \param[in,out] orc_Ini                Current ini
   \param[in]     orc_SectionName        Section name
   \param[in]     orc_IdentifierBaseName Identifier base name
   \param[in,out] orc_Columns            Columns
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsFiler::mh_LoadColumns(C_SCLIniFile & orc_Ini, const C_SCLString & orc_SectionName,
                               const std::string & orc_IdentifierBaseName, std::vector<sint32> & orc_Columns)
{
   const QString c_CountId = static_cast<QString>("%1_count").arg(orc_IdentifierBaseName.c_str());
   const sint32 s32_Count = orc_Ini.ReadInteger(orc_SectionName.c_str(), c_CountId.toStdString().c_str(), 0);

   orc_Columns.clear();
   orc_Columns.reserve(s32_Count);
   for (sint32 s32_ItCol = 0L; s32_ItCol < s32_Count; ++s32_ItCol)
   {
      const QString c_ItemId = static_cast<QString>("%1_%2").arg(orc_IdentifierBaseName.c_str()).arg(s32_ItCol);
      const sint32 s32_Value = orc_Ini.ReadInteger(orc_SectionName.c_str(), c_ItemId.toStdString().c_str(), 50);
      orc_Columns.push_back(s32_Value);
   }
}
