//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle user settings data (implementation)

   This class handles all actions concerning user settings.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include "C_UsFiler.hpp"
#include "stwerrors.hpp"
#include "C_UsHandler.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint8_t C_UsHandler::mhu8_NAX_RECENT_PROJECTS = 42;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_UsHandler * C_UsHandler::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler * C_UsHandler::h_GetInstance(void)
{
   if (C_UsHandler::mhpc_Singleton == NULL)
   {
      C_UsHandler::mhpc_Singleton = new C_UsHandler();
   }
   return C_UsHandler::mhpc_Singleton;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up singleton
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::h_Destroy(void)
{
   if (C_UsHandler::mhpc_Singleton != NULL)
   {
      delete (C_UsHandler::mhpc_Singleton);
      C_UsHandler::mhpc_Singleton = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the path of the current project

   \param[in]     orc_ActiveProject         Current active project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::ChangeActiveProjectName(const QString & orc_ActiveProject)
{
   this->mc_ActualProject = orc_ActiveProject;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all information of ini file for the current project

   The common information will be loaded too.

   \param[in]     orc_ActiveProject         Current active project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::LoadActiveProject(const QString & orc_ActiveProject)
{
   this->mc_ActualProject = orc_ActiveProject;

   C_UsFiler::h_Load(*this, this->mc_IniPathAndName, mc_ActualProject);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set default values

   Language = American english
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetDefault(void)
{
   this->mc_ScreenPos = QPoint(50, 50);
   this->mc_AppSize = QSize(1000, 700);
   this->mq_AppMaximized = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get recent project list

   \return
   Recent project list
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_UsHandler::GetRecentProjects(void) const
{
   return this->mc_RecentProjects;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get recent screen position

   \return
   Recent screen position
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_UsHandler::GetScreenPos(void) const
{
   return this->mc_ScreenPos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get recent application size

   \return
   Recent application size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_UsHandler::GetAppSize(void) const
{
   return this->mc_AppSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application maximizing flag

   \return
   Flag for showing application maximized
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetAppMaximized(void) const
{
   return this->mq_AppMaximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known button "hex" state

   \return
   Last known button "hex" state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetButtonHexActive(void) const
{
   return this->mq_ButtonHexActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known button "time stamp" state

   \return
   Last known button "time stamp" state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetButtonRelativeTimeStampActive(void) const
{
   return this->mq_ButtonRelativeTimeStampActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known button "unique" state

   \return
   Last known button "unique" state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetButtonUniqueViewActive(void) const
{
   return this->mq_ButtonUniqueViewActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known timestamp absolute mode

   \return
   Last known setting "timestamp absolute time of day" state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetTraceSettingDisplayTimestampAbsoluteTimeOfDay(void) const
{
   return this->mq_TraceSettingDisplayTimestampAbsoluteTimeOfDay;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known trace buffer size

   \return
   Last known setting "trace buffer size" state
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_UsHandler::GetTraceSettingBufferSize(void) const
{
   return this->mu32_TraceSettingBufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known selected protocol

   \return
   Last known selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSelectedProtocolIndex(void) const
{
   return this->ms32_SelectedProtocolIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get maximum number of recent projects entries

   \return
   Maximum number of recent projects entries
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_UsHandler::h_GetMaxRecentProjects(void)
{
   return C_UsHandler::mhu8_NAX_RECENT_PROJECTS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known database path.

   \return
   Last known database path.
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownDatabasePath() const
{
   return this->mc_LastKnownDatabasePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get settings-splitter x position value

   \return
   Current settings-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSplitterSettingsHorizontal(void) const
{
   return this->ms32_SplitterSettingsHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get settings-splitter state.

   \return
   true: settings are expanded
   false: settings are collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetSettingsAreExpanded(void) const
{
   return this->mq_SettingsAreExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get message-generator-splitter x position value

   \return
   Current message-generator-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSplitterMessageGenVertical(void) const
{
   return this->ms32_SplitterMessageGenVertical;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get message-generator-splitter state.

   \return
   true: message generator is expanded
   false: message generator is collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetMessageGenIsExpanded(void) const
{
   return this->mq_MessageGenIsExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get messages-signals-splitter x position value

   \return
   Current messages-signals-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSplitterMesSigHorizontal() const
{
   return this->ms32_SplitterMesSigHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known selected message indices

   \return
   Last known selected message indices
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsHandler::GetSelectedMessages(void) const
{
   return this->mc_SelectedMessages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known trace tree column widths

   \return
   Last known trace tree column widths
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsHandler::GetTraceColWidths(void) const
{
   return this->mc_TraceColWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known trace tree column positions

   \return
   Last known trace tree column positions
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsHandler::GetTraceColPositions(void) const
{
   return this->mc_TraceColPosition;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known message generator table column widths

   \return
   Last known message generator table column widths
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsHandler::GetMessageColWidths(void) const
{
   return this->mc_MessageGenColWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known message generator signals table column widths

   \return
   Last known message generator signals table column widths
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> & C_UsHandler::GetSignalsColWidths(void) const
{
   return this->mc_SignalsColWidth;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get database widget expanded flag.

   \return
   true: expanded, false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetWiDatabaseExpanded() const
{
   return this->mq_WiDatabaseExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get DLL configuration widget expanded flag.

   \return
   true: expanded, false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetWiDllConfigExpanded() const
{
   return this->mq_WiDllConfigExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get filter widget expanded flag.

   \return
   true: expanded, false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetWiFilterExpanded() const
{
   return this->mq_WiFilterExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get logging widget expanded flag.

   \return
   true: expanded, false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetWiLoggingExpanded() const
{
   return this->mq_WiLoggingExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get identification of settings subsection with open popup in collapsed state.

   \return
   section that was opened in collapsed mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::E_SettingsSubSection C_UsHandler::GetPopOpenSection(void) const
{
   return this->me_PopOpenSection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get most recent folder

   Extract folder of last recent projects or return default folder,
   should be valid in all cases

   \param[out] orc_Str Storage for most recent folder
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::GetMostRecentFolder(QString & orc_Str) const
{
   bool q_Exists;

   if (this->mc_RecentProjects.count() <= 0)
   {
      q_Exists = false;
   }
   else
   {
      const QFileInfo c_FileInfo(this->mc_RecentProjects[0]);
      QDir c_Dir(c_FileInfo.path());
      q_Exists = c_Dir.exists();
      if (q_Exists == true)
      {
         q_Exists = c_Dir.cdUp();
         if (q_Exists == true)
         {
            orc_Str = c_Dir.path();
         }
      }
   }
   if (q_Exists == false)
   {
      orc_Str = mc_DefaultProjectsFolder;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all recently opened parent folders

   Extract parent folder of recently opened projects or return default folder,
   should be valid in all cases

   \param[out] orc_Folders Storage for recent folders
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::GetRecentFolders(QStringList & orc_Folders) const
{
   QString c_Cur;
   QString c_CurFolder;
   bool q_Exists;

   orc_Folders.clear();
   for (uint8_t u8_It = 0; u8_It < this->mc_RecentProjects.size(); ++u8_It)
   {
      c_Cur = this->mc_RecentProjects.at(u8_It);
      //Extract parent
      if (C_UsHandler::h_GetParentFolder(c_Cur, c_CurFolder, true) == C_NO_ERR)
      {
         //Check if parent already in list
         q_Exists = false;
         for (uint8_t u8_It2 = 0; u8_It2 < orc_Folders.size(); ++u8_It2)
         {
            c_Cur = orc_Folders.at(u8_It2);
            if (c_Cur.compare(c_CurFolder) == 0)
            {
               q_Exists = true;
            }
         }
         if (q_Exists != true)
         {
            //Add
            orc_Folders.append(c_CurFolder);
         }
      }
   }
   if (orc_Folders.size() <= 0)
   {
      orc_Folders.append(mc_DefaultProjectsFolder);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set complete recent projects list

   \param[in] orc_New New recent projects list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetRecentProjects(const QStringList & orc_New)
{
   this->mc_RecentProjects = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent screen position

   \param[in] orc_New Updated screen position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetScreenPos(const QPoint & orc_New)
{
   this->mc_ScreenPos = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent screen position

   \param[in] orc_New Updated screen position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetAppSize(const QSize & orc_New)
{
   this->mc_AppSize = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application maximizing flag

   \param[in] oq_New Updated application maximizing flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetAppMaximized(const bool oq_New)
{
   this->mq_AppMaximized = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known button "hex" state

   \param[in] oq_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetButtonHexActive(const bool oq_New)
{
   this->mq_ButtonHexActive = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known button "time stamp" state

   \param[in] oq_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetButtonRelativeTimeStampActive(const bool oq_New)
{
   this->mq_ButtonRelativeTimeStampActive = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known button "unique" state

   \param[in] oq_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetButtonUniqueViewActive(const bool oq_New)
{
   this->mq_ButtonUniqueViewActive = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known timestamp absolute mode

   \param[in] oq_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetTraceSettingDisplayTimestampAbsoluteTimeOfDay(const bool oq_New)
{
   this->mq_TraceSettingDisplayTimestampAbsoluteTimeOfDay = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known trace buffer size

   \param[in] ou32_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetTraceSettingBufferSize(const uint32_t ou32_New)
{
   this->mu32_TraceSettingBufferSize = ou32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known selected protocol index

   \param[in] os32_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSelectedProtocolIndex(const int32_t os32_New)
{
   this->ms32_SelectedProtocolIndex = os32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current save as path

   \param[in] orc_Value Current save as path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetCurrentSaveAsPath(const QString & orc_Value)
{
   this->mc_CurrentSaveAsPath = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known database path.

   \param[in]     orc_Value      Last known database path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownDatabasePath(const QString & orc_Value)
{
   this->mc_LastKnownDatabasePath = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set settings-splitter x position value

   \param[in]   os32_New     New settings-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSplitterSettingsHorizontal(const int32_t os32_New)
{
   this->ms32_SplitterSettingsHorizontal = os32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set settings-splitter x position value

   \param[in]   oq_New     New settings-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSettingsAreExpanded(const bool oq_New)
{
   this->mq_SettingsAreExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message-generator-splitter x position value

   \param[in]   os32_New     New message-generator-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSplitterMessageGenVertical(const int32_t os32_New)
{
   this->ms32_SplitterMessageGenVertical = os32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message-generator-splitter x position value

   \param[in]   oq_New     New message-generator-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetMessageGenIsExpanded(const bool oq_New)
{
   this->mq_MessageGenIsExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set messages-signals-splitter x position value

   \param[in]   os32_New     New messages-signals-splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSplitterMesSigHorizontal(const int32_t os32_New)
{
   this->ms32_SplitterMesSigHorizontal = os32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known selected message indices

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSelectedMessages(const std::vector<int32_t> & orc_Value)
{
   this->mc_SelectedMessages = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known trace tree column widths

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetTraceColWidths(const std::vector<int32_t> & orc_Value)
{
   this->mc_TraceColWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known trace tree column positions

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetTraceColPositions(const std::vector<int32_t> & orc_Value)
{
   this->mc_TraceColPosition = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known message generator table column widths

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetMessageColWidths(const std::vector<int32_t> & orc_Value)
{
   this->mc_MessageGenColWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known message generator signals table column widths

   \param[in] orc_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSignalsColWidths(const std::vector<int32_t> & orc_Value)
{
   this->mc_SignalsColWidth = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set database widget expanded.

   \param[in]     oq_New        expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiDatabaseExpanded(const bool oq_New)
{
   this->mq_WiDatabaseExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set DLL configuration widget expanded.

   \param[in]     oq_New        expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiDllConfigExpanded(const bool oq_New)
{
   this->mq_WiDllConfigExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set filter widget expanded.

   \param[in]     oq_New        expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiFilterExpanded(const bool oq_New)
{
   this->mq_WiFilterExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set logging widget expanded.

   \param[in]     oq_New        expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiLoggingExpanded(const bool oq_New)
{
   this->mq_WiLoggingExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set identification of settings subsection with open popup in collapsed state.

   \param[in]     oe_PopOpenSection     section that was opened in collapsed mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetPopOpenSection(const E_SettingsSubSection oe_PopOpenSection)
{
   this->me_PopOpenSection = oe_PopOpenSection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add path & file to recent projects list

   \param[in] orc_Str Path and file name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::AddToRecentProjects(const QString & orc_Str)
{
   QString c_Copy = orc_Str;

   //Always use same notation
   c_Copy.replace(QChar('\\'), QChar('/'));
   //Check if new -> else erase then add again (Should be at top)
   RemoveOfRecentProjects(c_Copy);
   //Add
   this->mc_RecentProjects.prepend(c_Copy.toStdString().c_str());
   if (this->mc_RecentProjects.count() > C_UsHandler::mhu8_NAX_RECENT_PROJECTS)
   {
      this->mc_RecentProjects.pop_back();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove path & file of recent projects list

   \param[in] orc_Str Path and file name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::RemoveOfRecentProjects(const QString & orc_Str)
{
   this->mc_RecentProjects.removeAll(orc_Str);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear recent project list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::ClearRecentProjects(void)
{
   this->mc_RecentProjects.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user setting to default ini file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::Save(void) const
{
   C_UsFiler::h_Save(*this, mc_IniPathAndName, mc_ActualProject);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parent folder of path with or without file

   \param[in]  orc_CompletePath             Complete path
   \param[out] orc_Parent                   Parent folder
   \param[in]  orq_CompletePathContainsFile Indicator if complete path contains a file (filenames can't be handled automatically
                                               as there is no difference to a folder name,
                                               e.g. in Windows you can name a folder "MyFolder.MyExtension")

   \return
   C_NO_ERR: Parent valid
   C_RANGE:  Parent invalid = No parent found
             Sources: Input empty
                Path does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::h_GetParentFolder(const QString & orc_CompletePath, QString & orc_Parent,
                                       const bool & orq_CompletePathContainsFile)
{
   int32_t s32_Retval;

   if (orc_CompletePath.compare("") == 0)
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      QString c_Path;

      //RemoveFile
      if (orq_CompletePathContainsFile == true)
      {
         const QFileInfo c_File(orc_CompletePath);
         c_Path = c_File.absoluteDir().absolutePath();
      }
      else
      {
         c_Path = orc_CompletePath;
      }
      //GetParent
      {
         QDir c_Dir(c_Path);

         if (c_Dir.cdUp() == true)
         {
            s32_Retval = C_NO_ERR;
            orc_Parent = c_Dir.path();
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Load currently set values
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::C_UsHandler(void) :
   mc_IniPathAndName(C_Uti::h_GetExePath() + "/User/can_monitor_user_settings.ini"),
   mc_ActualProject(""),
   mc_DefaultProjectsFolder(static_cast<QString>(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                                                 "/openSYDE/Projects")),
   mq_ButtonHexActive(false),
   mq_ButtonRelativeTimeStampActive(false),
   mq_ButtonUniqueViewActive(false),
   mq_TraceSettingDisplayTimestampAbsoluteTimeOfDay(false),
   mu32_TraceSettingBufferSize(1000U),
   ms32_SelectedProtocolIndex(0),
   ms32_SplitterSettingsHorizontal(0),
   ms32_SplitterMessageGenVertical(0),
   mq_SettingsAreExpanded(true),
   mq_MessageGenIsExpanded(true),
   ms32_SplitterMesSigHorizontal(0),
   mq_WiDatabaseExpanded(true),
   mq_WiDllConfigExpanded(true),
   mq_WiFilterExpanded(true),
   mq_WiLoggingExpanded(true),
   me_PopOpenSection(E_SettingsSubSection::eNONE)
{
   // Load all project independent information
   C_UsFiler::h_Load(*this, mc_IniPathAndName, "");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Destructor

   Save currently set values
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::~C_UsHandler()
{
   this->Save();
}
