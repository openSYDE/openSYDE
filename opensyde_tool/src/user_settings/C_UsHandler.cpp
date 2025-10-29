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
#include "C_GtGetText.hpp"
#include "stwerrors.hpp"
#include "C_UsHandler.hpp"
#include "C_Uti.hpp"
#include "constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint8_t C_UsHandler::mhu8_MAX_RECENT_PROJECTS = 42;
const QPoint C_UsHandler::mhc_DEFAULT_VIEW_POS = QPoint(0, 0);
const int32_t C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL = 100;

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

   \param[in]  orc_ActiveProject    Current active project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::ChangeActiveProjectName(const QString & orc_ActiveProject)
{
   this->mc_ActualProject = orc_ActiveProject;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads all information of ini file for the current project

   The common information will be loaded too.

   \param[in]  orc_ActiveProject    Current active project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::LoadActiveProject(const QString & orc_ActiveProject)
{
   this->ClearMaps();

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
   mc_Lang = C_GtGetText::h_GetText("American english");
   this->mq_PerformanceMeasurementActive = false;

   this->mc_ScreenPos = QPoint(50, 50);
   this->mc_AppSize = QSize(1000, 700);
   this->mq_AppMaximized = true;
   this->mu32_ScreenIndex = 0;

   this->mc_SdTopologyToolboxPos = QPoint(-1, -1);
   this->mc_SdTopologyToolboxSize = QSize(600, 400);
   this->mq_SdTopologyToolboxMaximized = true;

   this->ms32_ProjLastKnownMode = 0;

   this->mc_ProjSdTopologyViewPos = C_UsHandler::mhc_DEFAULT_VIEW_POS;
   this->ms32_ProjSdTopologyViewZoom = C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL;

   this->ms32_SysDefSubMode = 0;
   this->mu32_SysDefIndex = 0U;
   this->mu32_SysDefFlag = 0U;

   this->ms32_SysViewSubMode = 0;
   this->mu32_SysViewIndex = 0U;
   this->mu32_SysViewFlag = 0U;

   this->ms32_SysDefNodeEditTabIndex = 0;
   this->ms32_SysDefBusEditTabIndex = 0;

   this->mc_RecentColors.fill(QColor(255, 255, 255, 255), 6);

   ms32_ScreenshotGifSucessTimeout = 3000;
   mc_PathHandlingSelection = "";
   mc_SkipTspImportSelection = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get language

   \return
   Language value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLanguage(void) const
{
   return this->mc_Lang;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get performance measurement active flag

   \return
   performance measurement active flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetPerformanceActive() const
{
   return this->mq_PerformanceMeasurementActive;
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
/*! \brief  Get recent application screen index (for multi screen setup)

   \return
   Screen index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_UsHandler::GetAppScreenIndex(void) const
{
   return this->mu32_ScreenIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get recent screen position

   \return
   Recent screen position
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_UsHandler::GetSdTopologyToolboxPos(void) const
{
   return this->mc_SdTopologyToolboxPos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get recent application size

   \return
   Recent application size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_UsHandler::GetSdTopologyToolboxSize(void) const
{
   return this->mc_SdTopologyToolboxSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application maximizing flag

   \return
   Flag for showing application maximized
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetSdTopologyToolboxMaximized(void) const
{
   return this->mq_SdTopologyToolboxMaximized;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get navigation bar size

   \return
   Current navigation bar size
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetNaviBarSize(void) const
{
   return this->ms32_NaviBarSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get navigation bar node section size

   \return
   Current navigation bar node section size
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetNaviBarNodeSectionSize(void) const
{
   return this->ms32_NaviBarNodeSectionSize;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node edit splitter x position value

   \return
   Current node edit splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdNodeEditSplitterHorizontal(void) const
{
   return this->ms32_SdNodeEditSplitterHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC screen splitter x position value

   \return
   Current HALC screen splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdNodeEditHalcSplitterHorizontal(void) const
{
   return this->ms32_SdNodEditHalcSplitterHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get HALC screen splitter x position value

   \return
   Current HALC screen splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdNodeEditCoManagerSplitterHorizontal(void) const
{
   return this->ms32_SdNodEditCoManagerSplitterHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data logger screen splitter x position value

   \return
   Current data logger screen splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdNodeEditDataLoggerSplitterHorizontal() const
{
   return this->ms32_SdNodeEditDataLoggerSplitterHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bus edit tree splitter x position value

   \return
   Current bus edit tree splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdBusEditTreeSplitterHorizontal(void) const
{
   return this->ms32_SdBusEditTreeSplitterHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bus edit tree splitter x position 2 value

   \return
   Current bus edit tree splitter x position 2 value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdBusEditTreeSplitterHorizontal2() const
{
   return this->ms32_SdBusEditTreeSplitterHorizontal2;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get bus edit layout splitter x position value

   \return
   Current bus edit layout splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetSdBusEditLayoutSplitterHorizontal(void) const
{
   return this->ms32_SdBusEditLayoutSplitterHorizontal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get maximum number of recent projects entries

   \return
   Maximum number of recent projects entries
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_UsHandler::h_GetMaxRecentProjects(void)
{
   return C_UsHandler::mhu8_MAX_RECENT_PROJECTS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current save as path

   \return
   Current save as path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetCurrentSaveAsPath(void) const
{
   return this->mc_CurrentSaveAsPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get recent colors as vector to color picker

   \return
   Recent colors as vector to color picker
*/
//----------------------------------------------------------------------------------------------------------------------
QVector<QColor> C_UsHandler::GetRecentColors(void) const
{
   return this->mc_RecentColors;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get next recent color button number to color picker

   \return
   Next recent color button number to color picker
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetNextRecentColorButtonNumber(void) const
{
   return this->ms32_NextRecentColorButtonNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get screenshot GIF Play timeout

   \return
   Screenshot GIF play timer
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetScreenshotGifSucessTimeout() const
{
   return this->ms32_ScreenshotGifSucessTimeout;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the currently selected option for path handling

   \retval   Relative
   \retval   Absolute
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetPathHandlingSelection() const
{
   return this->mc_PathHandlingSelection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get TSP shortcut usage

   \return
   bool

   \retval   true    TSP shortcut dialog will not appear after adding a new node (Import is skipped always)
   \retval   false   TSP shortcut dialog will appear after adding a new node
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetSkipTspSelection() const
{
   return this->mc_SkipTspImportSelection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get most recent folder

   Extract folder of last recent projects or return default folder,
   should be valid in all cases

   \param[out]  orc_Str    Storage for most recent folder
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

   \param[out]  orc_Folders   Storage for recent folders
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
/*! \brief   Get language list

   \param[out]  orc_List   Storage for language list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::h_GetLanguages(QStringList & orc_List)
{
   orc_List.clear();
   orc_List.append(C_GtGetText::h_GetText("American english"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known project mode (SD/SC/Main)

   long description of function within several lines

   \return
   mode (SD/SC/MAIN)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetProjLastMode() const
{
   return this->ms32_ProjLastKnownMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get zoom value of view

   A project shall be set in the constructor to get a non default value

   \return
   Zoom value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetProjSdTopologyViewZoom(void) const
{
   return this->ms32_ProjSdTopologyViewZoom;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get pos value of viewport

   A project shall be set in the constructor to get a non default value

   \return
   Pos value
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_UsHandler::GetProjSdTopologyViewPos(void) const
{
   return this->mc_ProjSdTopologyViewPos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known TSP path

   \return
   Last known TSP path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownTspPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownTspPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known code export path

   \return
   Last known code export path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownCodeExportPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownCodeExportPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known export path (e.g. DBC file)

   \return
   Full path of last known export folder
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownExportPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownExportPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known full path of RTF file export

   \return
   Full path of last known RTF file export folder
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownRtfPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownRtfPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known company name of RTF file export

   \return
   Last known company name of RTF file export
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownRtfCompanyName(void) const
{
   return this->mc_ProjSdTopologyLastKnownRtfCompanyName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known company logo path of RTF file export

   \return
   Full path of last known company logo path of RTF file export
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownRtfCompanyLogoPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownRtfCompanyLogoPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known HALC definition file path

   \return
   Last known HALC definition file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownHalcDefPath(void) const
{
   return this->mc_LastKnownHalcDefPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known HALC import file path

   \return
   Last known HALC import file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownHalcImportPath(void) const
{
   return this->mc_LastKnownHalcImportPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known HALC export file path

   \return
   Last known HALC export file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownHalcExportPath(void) const
{
   return this->mc_LastKnownHalcExportPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known service project file path

   \return
   Last known service project file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownServiceProjectPath() const
{
   return this->mc_LastKnownServiceProjectPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known RAMView project import file path

   \return
   Last known RAMView project import file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownRamViewProjectPath() const
{
   return this->mc_LastKnownRamViewProjectPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known J1939 catalog import file path

   \return
   Last known J1939 catalog import file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownJ1939CatalogPath() const
{
   return this->mc_LastKnownJ1939CatalogPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known CSV Export file path

   \return
   Last known CSV Export file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownCsvExportPath() const
{
   return this->mc_LastKnownServiceProjectPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known Pem file path

   \return
   Last known Pem file path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownAddPemFilePath() const
{
   return this->mc_LastKnownAddPemFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Update Pem  File Paths

   \return
   std::vector<QString>
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_UsHandler::GetLastKnownUpdatePemFilePaths() const
{
   return this->mc_PemFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known public pem file path

   \return
   Last known public pem file

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownPublicPemFilePath() const
{
   return this->mc_LastKnownPublicPemFilePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known pem file password

   \return
   Last known pem file password

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownPemFilePassword() const
{
   return this->mc_LastKnownPemFilePassword;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known secure certificate package file path

   \return
   Last known secure certificate package

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownSecureCertificatePackagePath() const
{
   return this->mc_LastKnownSecureCertificatePackagePath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known private key path (PEM file for secure update)

   \return
   Last known private key path

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownPrivateKeyPath() const
{
   return this->mc_LastKnownPrivateKeyPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known Add PEM file state

   \return
   last known Add PEM file state

*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetLastKnownAddPemFileState() const
{
   return this->mq_LastKnownAddPemFileState;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known secure update config state

   \return
   last known secure update config state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetLastKnownSecureUpdateConfigState() const
{
   return this->mq_LastKnownSecureUpdateConfigState;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known import path

   \return
   Last known import path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownImportPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownImportPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last known can open path

   \return
   Last known CAN open path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownCanOpenEdsPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownCanOpenEdsPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last known import path of an device-definition file

   \return
   Last known import path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetProjSdTopologyLastKnownDeviceDefPath(void) const
{
   return this->mc_ProjSdTopologyLastKnownDeviceDefPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system definition node user settings

   \param[in]  orc_NodeName   Project system definition node name (identifier)

   \return
   Project system definition node user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsNode C_UsHandler::GetProjSdNode(const QString & orc_NodeName) const
{
   return this->mc_ProjSdNode.value(orc_NodeName, C_UsNode());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system definition bus user settings

   \param[in]  orc_BusName    Project system definition bus name (identifier)

   \return
   Project system definition bus user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsCommunication C_UsHandler::GetProjSdBus(const QString & orc_BusName) const
{
   return this->mc_ProjSdBus.value(orc_BusName, C_UsCommunication());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system view user settings

   \param[in]  orc_ViewName   Project system view name (identifier)

   \return
   Project system view user settings
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsSystemView C_UsHandler::GetProjSvSetupView(const QString & orc_ViewName) const
{
   return this->mc_ProjSvSetupView.value(orc_ViewName, C_UsSystemView(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL,
                                                                      C_UsHandler::mhc_DEFAULT_VIEW_POS));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system definition node keys internal structure

   \return
   Project system definition node keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsHandler::GetProjSdNodeKeysInternal(void) const
{
   return this->mc_ProjSdNode.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system definition bus keys internal structure

   \return
   Project system definition bus keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsHandler::GetProjSdBusKeysInternal(void) const
{
   return this->mc_ProjSdBus.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get project system view setup view keys internal structure

   \return
   Project system view setup view keys internal structure
*/
//----------------------------------------------------------------------------------------------------------------------
const QList<QString> C_UsHandler::GetProjSvSetupViewKeysInternal(void) const
{
   return this->mc_ProjSvSetupView.keys();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last parameters for system definition and system view screens

   \param[out]  ors32_SysDefSubMode    Last sub mode of system definition
   \param[out]  oru32_SysDefIndex      Last index of system definition
   \param[out]  oru32_SysDefFlag       Last flag value of system definition
   \param[out]  ors32_SysViewSubMode   Last sub mode of system view
   \param[out]  oru32_SysViewIndex     Last index of system view
   \param[out]  oru32_SysViewFlag      Last flag value of system view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::GetProjLastScreenMode(int32_t & ors32_SysDefSubMode, uint32_t & oru32_SysDefIndex,
                                        uint32_t & oru32_SysDefFlag, int32_t & ors32_SysViewSubMode,
                                        uint32_t & oru32_SysViewIndex, uint32_t & oru32_SysViewFlag) const
{
   ors32_SysDefSubMode = this->ms32_SysDefSubMode;
   oru32_SysDefIndex = this->mu32_SysDefIndex;
   oru32_SysDefFlag = this->mu32_SysDefFlag;

   ors32_SysViewSubMode = this->ms32_SysViewSubMode;
   oru32_SysViewIndex = this->mu32_SysViewIndex;
   oru32_SysViewFlag = this->mu32_SysViewFlag;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last index for system definition node edit tab

   \return     Tab index of node edit
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetProjLastSysDefNodeTabIndex(void) const
{
   return this->ms32_SysDefNodeEditTabIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last index for system definition bus edit tab

   \return     Tab index of bus edit
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetProjLastSysDefBusTabIndex(void) const
{
   return this->ms32_SysDefBusEditTabIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get view permissions for specified view

   \param[in]       orc_ViewName     name of view

   \return
   array of booleans with permission flags
*/
//----------------------------------------------------------------------------------------------------------------------
std::array<bool, 3> C_UsHandler::GetViewPermissions(const QString & orc_ViewName)
{
   std::array<bool, 3> c_Retval = {false, false, false};
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      const C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);

      c_Retval[0] = rc_View.GetSetupPermission();
      c_Retval[1] = rc_View.GetUpdatePermission();
      c_Retval[2] = rc_View.GetDashboardPermission();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set language

   \param[in]  orc_Lang    New language value

   \return
   C_NO_ERR: exists
   C_RANGE:  does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::SetLanguage(const QString & orc_Lang)
{
   const int32_t s32_Retval = h_CheckLanguageExists(orc_Lang);

   if (s32_Retval == C_NO_ERR)
   {
      this->mc_Lang = orc_Lang;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set performance measurement active flag

   \param[in]  oq_Active   New performance measurement active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetPerformanceActive(const bool oq_Active)
{
   this->mq_PerformanceMeasurementActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set complete recent projects list

   \param[in]  orc_New  New recent projects list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetRecentProjects(const QStringList & orc_New)
{
   this->mc_RecentProjects = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent screen position

   \param[in]  orc_New  Updated screen position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetScreenPos(const QPoint & orc_New)
{
   this->mc_ScreenPos = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent screen position

   \param[in]  orc_New  Updated screen position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetAppSize(const QSize & orc_New)
{
   this->mc_AppSize = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application maximizing flag

   \param[in]  oq_New   Updated application maximizing flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetAppMaximized(const bool oq_New)
{
   this->mq_AppMaximized = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set recent application screen index (for multi screen setup)

   \param[in]  ou32_New    New screen index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetAppScreenIndex(const uint32_t ou32_New)
{
   this->mu32_ScreenIndex = ou32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent screen position

   \param[in]  orc_New  Updated screen position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdTopologyToolboxPos(const QPoint & orc_New)
{
   this->mc_SdTopologyToolboxPos = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent screen position

   \param[in]  orc_New  Updated screen position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdTopologyToolboxSize(const QSize & orc_New)
{
   this->mc_SdTopologyToolboxSize = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set navigation bar size

   \param[in]  os32_Value  New navigation bar size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetNaviBarSize(const int32_t os32_Value)
{
   this->ms32_NaviBarSize = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set navigation bar node section size

   \param[in]  os32_Value  New navigation bar node section size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetNaviBarNodeSectionSize(const int32_t os32_Value)
{
   this->ms32_NaviBarNodeSectionSize = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set node edit splitter x position value

   \param[in]  os32_Value  New node edit splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdNodeEditSplitterHorizontal(const int32_t os32_Value)
{
   this->ms32_SdNodeEditSplitterHorizontal = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node edit HALC screen splitter x position value

   \param[in]  os32_Value  New splitter position x value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdNodeEditHalcSplitterHorizontal(const int32_t os32_Value)
{
   this->ms32_SdNodEditHalcSplitterHorizontal = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node edit HALC screen splitter x position value

   \param[in]  os32_Value  New splitter position x value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdNodeEditCoManagerSplitterHorizontal(const int32_t os32_Value)
{
   this->ms32_SdNodEditCoManagerSplitterHorizontal = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node edit data logger screen splitter x position value

   \param[in]  os32_Value  New splitter position x value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdNodeEditDataLoggerSplitterHorizontal(const int32_t os32_Value)
{
   this->ms32_SdNodeEditDataLoggerSplitterHorizontal = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set bus edit tree splitter x position value

   \param[in]  os32_Value  New bus edit tree splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdBusEditTreeSplitterHorizontal(const int32_t os32_Value)
{
   this->ms32_SdBusEditTreeSplitterHorizontal = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set bus edit tree splitter x position 2 value

   \param[in]  os32_Value  New bus edit tree splitter x position 2 value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdBusEditTreeSplitterHorizontal2(const int32_t os32_Value)
{
   this->ms32_SdBusEditTreeSplitterHorizontal2 = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set bus edit layout splitter x position value

   \param[in]  os32_Value  New bus edit layout splitter x position value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdBusEditLayoutSplitterHorizontal(const int32_t os32_Value)
{
   this->ms32_SdBusEditLayoutSplitterHorizontal = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set application maximizing flag

   \param[in]  orq_New  Updated application maximizing falg
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSdTopologyToolboxMaximized(const bool & orq_New)
{
   this->mq_SdTopologyToolboxMaximized = orq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current save as path

   \param[in]  orc_Value   Current save as path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetCurrentSaveAsPath(const QString & orc_Value)
{
   this->mc_CurrentSaveAsPath = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set recent colors as vector from color picker

   \param[in]  orc_RecentColorsVector  Recent colors as vector from color picker
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetRecentColors(const QVector<QColor> & orc_RecentColorsVector)
{
   this->mc_RecentColors = orc_RecentColorsVector;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set next recent color button number from color picker

   \param[in]  os32_NextRecentColorButtonNumber  Next recent color button number from color picker
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetNextRecentColorButtonNumber(const int32_t os32_NextRecentColorButtonNumber)
{
   this->ms32_NextRecentColorButtonNumber = os32_NextRecentColorButtonNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set flag whether TSP Import on adding a Node shall be skipped always

   \param[in]       orc_Selection     can be "Ask User" or "Skip"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetSkipTspSelection(const QString & orc_Selection)
{
   this->mc_SkipTspImportSelection = orc_Selection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set flag for the currently selected option for the path handling

   \param[in]       orc_Selection     can be "Ask User", "Relative" or "Absolute"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetPathHandlingSelection(const QString & orc_Selection)
{
   this->mc_PathHandlingSelection = orc_Selection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add path & file to recent projects list

   \param[in]  orc_Str  Path and file name
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
   if (this->mc_RecentProjects.count() > C_UsHandler::mhu8_MAX_RECENT_PROJECTS)
   {
      this->mc_RecentProjects.pop_back();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove path & file of recent projects list

   \param[in]  orc_Str  Path and file name
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
/*! \brief  Set Screenshot GIF play timeout

   \param[in]       os32_ScreenshotGifSucessTimer     GIF Play timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetScreenshotGifSucessTimeout(const int32_t os32_ScreenshotGifSucessTimer)
{
   this->ms32_ScreenshotGifSucessTimeout = os32_ScreenshotGifSucessTimer;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last mode of project (SD/SC/Main)

   long description of function within several lines

   \param[in]  os32_New    New last mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjLastMode(const int32_t os32_New)
{
   this->ms32_ProjLastKnownMode = os32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the project specific zoom value for the view

   A project shall be set in the constructor to save a value

   \param[in]  os32_New  New zoom value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyViewZoom(const int32_t os32_New)
{
   this->ms32_ProjSdTopologyViewZoom = os32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the project specific position for the viewport

   A project shall be set in the constructor to save a value

   \param[in]  orc_New  New zoom value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyViewPos(const QPoint & orc_New)
{
   this->mc_ProjSdTopologyViewPos = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known TSP path

   \param[in]  orc_New  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownTspPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownTspPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known code export path

   \param[in]  orc_New  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownCodeExportPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownCodeExportPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known full path of export (e.g. DBC file)

   \param[in]  orc_New  new full folder path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownExportPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownExportPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known full path of RTF file export

   \param[in]  orc_New  new full folder path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownRtfPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownRtfPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known company name of RTF file export

   \param[in]  orc_New  new company name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownRtfCompanyName(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownRtfCompanyName = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known full logo path of RTF file export

   \param[in]  orc_New  new full folder path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownRtfCompanyLogoPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownRtfCompanyLogoPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known HALC definition file path

   \param[in]  orc_NewPath    Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownHalcDefPath(const QString & orc_NewPath)
{
   this->mc_LastKnownHalcDefPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known HALC definition file path

   \param[in]  orc_NewPath    Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownHalcImportPath(const QString & orc_NewPath)
{
   this->mc_LastKnownHalcImportPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known HALC definition file path

   \param[in]  orc_NewPath    Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownHalcExportPath(const QString & orc_NewPath)
{
   this->mc_LastKnownHalcExportPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known service project file path

   \param[in]  orc_NewPath    service project file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownServiceProjectPath(const QString & orc_NewPath)
{
   this->mc_LastKnownServiceProjectPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known service project file path

   \param[in]  orc_NewPath    service project file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownRamViewProjectPath(const QString & orc_NewPath)
{
   this->mc_LastKnownRamViewProjectPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known J1939 catalog file path

   \param[in]  orc_NewPath    J1939 catalog file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownJ1939CatalogPath(const QString & orc_NewPath)
{
   this->mc_LastKnownJ1939CatalogPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known CSV Export file path

   \param[in]  orc_NewPath    CSV Export file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownCsvExportPath(const QString & orc_NewPath)
{
   this->mc_LastKnownServiceProjectPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Pem file path

   \param[in]  orc_NewPath    Pem file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownAddPemFilePath(const QString & orc_NewPath)
{
   this->mc_LastKnownAddPemFilePath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Update Pem File Paths

   \param[in]  orc_PemFilePaths   vector of pem file paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownUpdatePemFilePaths(const QStringList & orc_PemFilePaths)
{
   this->mc_PemFilePaths = orc_PemFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known public pem file path

   \param[in]  orc_NewPath    PEM file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownPublicPemFilePath(const QString & orc_NewPath)
{
   this->mc_LastKnownPublicPemFilePath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known pem file password

   \param[in]  orc_Password    pem file password
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownPemFilePassword(const QString & orc_Password)
{
   this->mc_LastKnownPemFilePassword = orc_Password;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Add pem file state

   \param[in]  orq_IsEnabled    Add pem file state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownAddPemFileState(const bool & orq_IsEnabled)
{
   this->mq_LastKnownAddPemFileState = orq_IsEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known secure update config state

   \param[in]  orq_IsEnabled    secure update config state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownSecureUpdateConfigState(const bool & orq_IsEnabled)
{
   this->mq_LastKnownSecureUpdateConfigState = orq_IsEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known secure certificate package path

   \param[in]  orc_NewPath    secure certificate package path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownSecureCertificatePackagePath(const QString & orc_NewPath)
{
   this->mc_LastKnownSecureCertificatePackagePath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known private key file path (PEM file for secure update)

   \param[in]  orc_NewPath    PEM file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownPrivateKeyPath(const QString & orc_NewPath)
{
   this->mc_LastKnownPrivateKeyPath = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known import path

   \param[in]  orc_New  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownImportPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownImportPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known CAN open path

   \param[in]  orc_New  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownCanOpenEdsPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownCanOpenEdsPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known import path of a device-definition file

   \param[in]  orc_New  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdTopologyLastKnownDeviceDefPath(const QString & orc_New)
{
   this->mc_ProjSdTopologyLastKnownDeviceDefPath = orc_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node selected data pool name

   \param[in]  orc_NodeName      Project system definition node name (identifier)
   \param[in]  orc_DatapoolName  Selected data pool name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedDatapoolName(const QString & orc_NodeName, const QString & orc_DatapoolName)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedDatapoolName(orc_DatapoolName);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedDatapoolName(orc_DatapoolName);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node selected protocol

   \param[in]  orc_NodeName   Project system definition node name (identifier)
   \param[in]  oe_Protocol    Selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedProtocol(const QString & orc_NodeName,
                                                const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Protocol)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedProtocol(oe_Protocol);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedProtocol(oe_Protocol);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node selected protocol

   \param[in]  orc_NodeName            Project system definition node name (identifier)
   \param[in]  ou32_SelectedInterface  Selected CAN interface
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedInterface(const QString & orc_NodeName, const uint32_t ou32_SelectedInterface)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedInterface(ou32_SelectedInterface);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedInterface(ou32_SelectedInterface);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool expanded list names

   \param[in]  orc_NodeName      Project system definition node name (identifier)
   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Expanded list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolOpenListNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                     const std::vector<QString> & orc_New)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolOpenListNames(orc_DatapoolName, orc_New);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolOpenListNames(orc_DatapoolName, orc_New);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool selected list names

   \param[in]  orc_NodeName      Project system definition node name (identifier)
   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Selected list names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolSelectedListNames(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                         const std::vector<QString> & orc_New)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolSelectedListNames(orc_DatapoolName, orc_New);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolSelectedListNames(orc_DatapoolName, orc_New);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool selected variable names

   \param[in]  orc_NodeName      Project system definition node name (identifier)
   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_New           Selected variable names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolSelectedVariableNames(const QString & orc_NodeName,
                                                             const QString & orc_DatapoolName,
                                                             const std::vector<QString> & orc_New)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolSelectedVariableNames(orc_DatapoolName, orc_New);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolSelectedVariableNames(orc_DatapoolName, orc_New);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node datapool comm overview column width

   \param[in]  orc_NodeName      Node name
   \param[in]  orc_DatapoolName  Datapool name
   \param[in]  orc_ListName      List name
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolCommMessageOverviewColumnWidth(const QString & orc_NodeName,
                                                                      const QString & orc_DatapoolName,
                                                                      const QString & orc_ListName,
                                                                      const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolCommMessageOverviewColumnWidth(orc_DatapoolName,
                                                                                              orc_ListName,
                                                                                              orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolCommMessageOverviewColumnWidth(orc_DatapoolName, orc_ListName, orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set proj sd node datapool comm signal overview column width

   \param[in]  orc_NodeName      Node name
   \param[in]  orc_DatapoolName  Datapool name
   \param[in]  orc_ListName      List name
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolCommSignalOverviewColumnWidth(const QString & orc_NodeName,
                                                                     const QString & orc_DatapoolName,
                                                                     const QString & orc_ListName,
                                                                     const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolCommSignalOverviewColumnWidth(orc_DatapoolName,
                                                                                             orc_ListName,
                                                                                             orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolCommSignalOverviewColumnWidth(orc_DatapoolName, orc_ListName, orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool list selected message

   \param[in]  orc_NodeName               Project system definition node name (identifier)
   \param[in]  orc_DatapoolName           Project system definition node datapool name (identifier)
   \param[in]  orc_ListName               Project system definition node datapool list name (identifier)
   \param[in]  oe_SelectedProtocol        Type of selected protocol
   \param[in]  oq_MessageSelected         Set flag if there is a selected message
   \param[in]  orc_SelectedMessageName    Selected message name if any
   \param[in]  oq_SignalSelected          Flag if signal selected
   \param[in]  orc_SelectedSignalName     Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolListSelectedMessage(const QString & orc_NodeName,
                                                           const QString & orc_DatapoolName,
                                                           const QString & orc_ListName, const stw::opensyde_core::C_OscCanProtocol::E_Type
                                                           oe_SelectedProtocol, const bool oq_MessageSelected,
                                                           const QString & orc_SelectedMessageName,
                                                           const bool oq_SignalSelected,
                                                           const QString & orc_SelectedSignalName)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolListSelectedMessage(orc_DatapoolName, orc_ListName,
                                                                                   oe_SelectedProtocol,
                                                                                   oq_MessageSelected,
                                                                                   orc_SelectedMessageName,
                                                                                   oq_SignalSelected,
                                                                                   orc_SelectedSignalName);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolListSelectedMessage(orc_DatapoolName, orc_ListName, oe_SelectedProtocol, oq_MessageSelected,
                                            orc_SelectedMessageName, oq_SignalSelected, orc_SelectedSignalName);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition node datapool list column widths

   \param[in]  orc_NodeName      Project system definition node name (identifier)
   \param[in]  orc_DatapoolName  Project system definition node datapool name (identifier)
   \param[in]  orc_ListName      Project system definition node datapool list name (identifier)
   \param[in]  orc_ColumnWidths  Last known column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeDatapoolListColumnSizes(const QString & orc_NodeName, const QString & orc_DatapoolName,
                                                       const QString & orc_ListName,
                                                       const std::vector<int32_t> & orc_ColumnWidths)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetDatapoolListColumnSizes(orc_DatapoolName, orc_ListName,
                                                                               orc_ColumnWidths);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetDatapoolListColumnSizes(orc_DatapoolName, orc_ListName, orc_ColumnWidths);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node CANopen overview column width

   \param[in]  orc_NodeName   Node name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeCanOpenOverviewColumnWidth(const QString & orc_NodeName,
                                                          const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetCanOpenOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetCanOpenOverviewColumnWidth(orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node CANopen PDO overview column width

   \param[in]  orc_NodeName   Node name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeCanOpenPdoOverviewColumnWidth(const QString & orc_NodeName,
                                                             const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetCanOpenPdoOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetCanOpenPdoOverviewColumnWidth(orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected CANopen manager

   \param[in]  orc_NodeName   Node name
   \param[in]  oru8_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedCanOpenManager(const QString & orc_NodeName, const uint8_t & oru8_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedCanOpenManager(oru8_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedCanOpenManager(oru8_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected CANopen device

   \param[in]  orc_NodeName      Node name
   \param[in]  oru8_Number       Interface number
   \param[in]  orc_Value         Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedCanOpenDevice(const QString & orc_NodeName, const uint8_t & oru8_Number,
                                                     const QString & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedCanOpenDevice(oru8_Number, orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedCanOpenDevice(oru8_Number, orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected CANopen device use case

   \param[in]  orc_NodeName   Node name
   \param[in]  oru32_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedCanOpenDeviceUseCaseIndex(const QString & orc_NodeName,
                                                                 const uint32_t & oru32_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedCanOpenDeviceUseCaseIndex(oru32_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedCanOpenDeviceUseCaseIndex(oru32_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected CANopen device use case

   \param[in]  orc_NodeName      Node name
   \param[in]  orc_Interfaces    Expanded interfaces
   \param[in]  orc_Devices       Expanded devices
   \param[in]  orc_Device        Expanded device
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeExpandedCanOpenTree(const QString & orc_NodeName, const std::map<uint8_t,
                                                                                                bool> & orc_Interfaces,
                                                   const std::map<uint8_t,
                                                                  bool> & orc_Devices, const std::map<std::pair<uint8_t,
                                                                                                                std::pair<uint8_t,
                                                                                                                          stw::scl::C_SclString> >,
                                                                                                      bool> & orc_Device)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetExpandedCanOpenManager(orc_Interfaces);
      this->mc_ProjSdNode.operator [](orc_NodeName).SetExpandedCanOpenDevices(orc_Devices);
      this->mc_ProjSdNode.operator [](orc_NodeName).SetExpandedCanOpenDevice(orc_Device);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetExpandedCanOpenManager(orc_Interfaces);
      c_Node.SetExpandedCanOpenDevices(orc_Devices);
      c_Node.SetExpandedCanOpenDevice(orc_Device);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node is use case or interface selected

   \param[in]  orc_NodeName               Node name
   \param[in]  orq_IsUseCaseSelected      Selected or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeCanOpenSelectedUseCaseOrInterface(const QString & orc_NodeName,
                                                                 const bool & orq_IsUseCaseSelected)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetCanOpenSelectedUseCaseOrInterface(orq_IsUseCaseSelected);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetCanOpenSelectedUseCaseOrInterface(orq_IsUseCaseSelected);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node halc overview column width

   \param[in]  orc_NodeName   Node name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeHalcOverviewColumnWidth(const QString & orc_NodeName,
                                                       const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetHalcOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetHalcOverviewColumnWidth(orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node HALC parameter configuration table column width

   \param[in]  orc_NodeName   Node name
   \param[in]  orc_Value      Column widths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeHalcConfigColumnWidth(const QString & orc_NodeName,
                                                     const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetHalcConfigColumnWidth(orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetHalcConfigColumnWidth(orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected HALC domain

   \param[in]  orc_NodeName   Node name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedHalcDomain(const QString & orc_NodeName, const QString & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedHalcDomain(orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedHalcDomain(orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected HALC channel

   \param[in]  orc_NodeName   Node name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedHalcChannel(const QString & orc_NodeName, const QString & orc_Value)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedHalcChannel(orc_Value);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedHalcChannel(orc_Value);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node selected DataLogger LogJob Index

   \param[in]  orc_NodeName   Node name
   \param[in]  os32_LogJobIndex      LogJob Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeSelectedDataLoggerLogJobIndex(const QString & orc_NodeName,
                                                             const int32_t os32_LogJobIndex)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetSelectedDataLoggerLogJobIndex(os32_LogJobIndex);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetSelectedDataLoggerLogJobIndex(os32_LogJobIndex);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition node DataLogger Overview widget selected

   \param[in]  orc_NodeName   Node name
   \param[in]  oq_IsOverviewWidgetSelected      IsOverviewWidgetSelected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdNodeIsOverviewWidgetSelected(const QString & orc_NodeName,
                                                        const bool oq_IsOverviewWidgetSelected)
{
   if (this->mc_ProjSdNode.contains(orc_NodeName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdNode.operator [](orc_NodeName).SetIsOverviewWidgetSelected(oq_IsOverviewWidgetSelected);
   }
   else
   {
      C_UsNode c_Node;
      c_Node.SetIsOverviewWidgetSelected(true);
      this->mc_ProjSdNode.insert(orc_NodeName, c_Node);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system definition bus last selected message

   \param[in]  orc_BusName                Project system definition bus name (identifier)
   \param[in]  oe_SelectedProtocol        Currently selected protocol
   \param[in]  oq_MessageSelected         Set flag if there is a selected message
   \param[in]  orc_SelectedMessageName    Selected message name if any
   \param[in]  oq_SignalSelected          Flag if signal selected
   \param[in]  orc_SelectedSignalName     Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdBusSelectedMessage(const QString & orc_BusName,
                                              const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                                              const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                              const bool oq_SignalSelected, const QString & orc_SelectedSignalName)
{
   if (this->mc_ProjSdBus.contains(orc_BusName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdBus.operator [](orc_BusName).SetMessageSelected(oe_SelectedProtocol, oq_MessageSelected,
                                                                     orc_SelectedMessageName, oq_SignalSelected,
                                                                     orc_SelectedSignalName);
   }
   else
   {
      C_UsCommunication c_Bus;
      c_Bus.SetMessageSelected(oe_SelectedProtocol, oq_MessageSelected, orc_SelectedMessageName, oq_SignalSelected,
                               orc_SelectedSignalName);
      this->mc_ProjSdBus.insert(orc_BusName, c_Bus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system definition bus comm overview column width

   \param[in]  orc_BusName    Bus name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdBusCommMessageOverviewColumnWidth(const QString & orc_BusName,
                                                             const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdBus.contains(orc_BusName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdBus.operator [](orc_BusName).SetMessageOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsCommunication c_Bus;
      c_Bus.SetMessageOverviewColumnWidth(orc_Value);
      this->mc_ProjSdBus.insert(orc_BusName, c_Bus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set proj sd bus comm signal overview column width

   \param[in]  orc_BusName    Bus name
   \param[in]  orc_Value      Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSdBusCommSignalOverviewColumnWidth(const QString & orc_BusName,
                                                            const std::vector<int32_t> & orc_Value)
{
   if (this->mc_ProjSdBus.contains(orc_BusName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSdBus.operator [](orc_BusName).SetSignalOverviewColumnWidth(orc_Value);
   }
   else
   {
      C_UsCommunication c_Bus;
      c_Bus.SetSignalOverviewColumnWidth(orc_Value);
      this->mc_ProjSdBus.insert(orc_BusName, c_Bus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the status for the navigation expansion state

   \param[in]  orc_ViewName                  Project system view name (identifier)
   \param[in]  oq_NavigationExpandedStatus   New navigation expansion state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvNavigationExpandedStatus(const QString & orc_ViewName,
                                                    const bool oq_NavigationExpandedStatus)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSvSetupView.operator [](orc_ViewName).SetNavigationExpandedStatus(oq_NavigationExpandedStatus);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetNavigationExpandedStatus(oq_NavigationExpandedStatus);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view setup view zoom

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  os32_New       Project system view setup view zoom
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvSetupViewZoom(const QString & orc_ViewName, const int32_t os32_New)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSvSetupView.operator [](orc_ViewName).s32_SetupViewZoom = os32_New;
   }
   else
   {
      this->mc_ProjSvSetupView.insert(orc_ViewName, C_UsSystemView(os32_New, C_UsHandler::mhc_DEFAULT_VIEW_POS));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view setup view position

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_New        Project system view setup view position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvSetupViewPos(const QString & orc_ViewName, const QPoint & orc_New)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      this->mc_ProjSvSetupView.operator [](orc_ViewName).c_SetupViewPos = orc_New;
   }
   else
   {
      this->mc_ProjSvSetupView.insert(orc_ViewName, C_UsSystemView(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, orc_New));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view update scene zoom level

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  os32_New       New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateViewZoom(const QString & orc_ViewName, const int32_t os32_New)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.s32_UpdateViewZoom = os32_New;
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.s32_UpdateViewZoom = os32_New;
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view update scene position

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_New        New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateViewPos(const QString & orc_ViewName, const QPoint & orc_New)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.c_UpdateViewPos = orc_New;
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.c_UpdateViewPos = orc_New;
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view parameter export settings

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_Path       Selected path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvParamExport(const QString & orc_ViewName, const QString & orc_Path)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.c_ParamExportPath = orc_Path;
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.c_ParamExportPath = orc_Path;
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view parameter import settings

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_Path       Selected path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvParamImport(const QString & orc_ViewName, const QString & orc_Path)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.c_ParamImportPath = orc_Path;
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.c_ParamImportPath = orc_Path;
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view parameter record settings

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_Path       Selected path
   \param[in]  orc_FileName   Selected file name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvParamRecord(const QString & orc_ViewName, const QString & orc_Path,
                                       const QString & orc_FileName)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.c_ParamRecordPath = orc_Path;
      rc_View.c_ParamRecordFileName = orc_FileName;
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.c_ParamRecordPath = orc_Path;
      c_View.c_ParamRecordFileName = orc_FileName;
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add project system view update data rate per node

   \param[in]  orc_ViewName               Project system view name (identifier)
   \param[in]  orc_NodeName               Node name
   \param[in]  ou32_Checksum              Checksum
   \param[in]  of64_DataRateBytesPerMs    Data rate bytes per ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::AddProjSvNodeUpdateDataRate(const QString & orc_ViewName, const QString & orc_NodeName,
                                              const uint32_t ou32_Checksum, const float64_t of64_DataRateBytesPerMs)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.AddNodeUpdateDataRate(orc_NodeName, ou32_Checksum, of64_DataRateBytesPerMs);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.AddNodeUpdateDataRate(orc_NodeName, ou32_Checksum, of64_DataRateBytesPerMs);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view update splitter X value

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  os32_Value     New update splitter X value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateSplitterHorizontal(const QString & orc_ViewName, const int32_t os32_Value)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetUpdateSplitterHorizontal(os32_Value);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetUpdateSplitterHorizontal(os32_Value);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view update horizontal splitter Y value

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  os32_Value     New horizontal update splitter Y value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateHorizontalSplitterVertical(const QString & orc_ViewName, const int32_t os32_Value)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetUpdateHorizontalSplitterVertical(os32_Value);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetUpdateHorizontalSplitterVertical(os32_Value);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view update progress log settings

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_Position   Progress log position
   \param[in]  orc_Size       Progress log size
   \param[in]  orq_Maximized  Progress log maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateProgressLog(const QString & orc_ViewName, const QPoint & orc_Position,
                                             const QSize & orc_Size, const bool & orq_Maximized)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetUpdateProgressLogPos(orc_Position);
      rc_View.SetUpdateProgressLogSize(orc_Size);
      rc_View.SetUpdateProgressLogMaximized(orq_Maximized);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetUpdateProgressLogPos(orc_Position);
      c_View.SetUpdateProgressLogSize(orc_Size);
      c_View.SetUpdateProgressLogMaximized(orq_Maximized);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set style type of project system view update summary widget

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  oq_BigVisible  true: big summary widget; false: small summary widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateSummaryBig(const QString & orc_ViewName, const bool oq_BigVisible)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetUpdateSummaryBig(oq_BigVisible);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetUpdateSummaryBig(oq_BigVisible);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set visibility of empty optional sections

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  oq_Visible     true: visible; false: invisible
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateEmptyOptionalSectionsVisible(const QString & orc_ViewName, const bool oq_Visible)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetUpdateEmptyOptionalSectionsVisible(oq_Visible);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetUpdateEmptyOptionalSectionsVisible(oq_Visible);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set project system view update package sections expand flags

   \param[in]  orc_ViewName            Project system view name (identifier)
   \param[in]  orc_NodeName            Node name (identifier)
   \param[in]  orc_SectionsExpanded    New values (true: expand, false: collapse)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvUpdateSectionsExpandedFlags(const QString & orc_ViewName, const QString & orc_NodeName,
                                                       const QVector<bool> & orc_SectionsExpanded)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetNodeSectionsExpanded(orc_NodeName, orc_SectionsExpanded);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetNodeSectionsExpanded(orc_NodeName, orc_SectionsExpanded);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view dashboard toolbox settings

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  orc_Position   Toolbox position
   \param[in]  orc_Size       Toolbox size
   \param[in]  orq_Maximized  Toolbox maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvDashboardToolbox(const QString & orc_ViewName, const QPoint & orc_Position,
                                            const QSize & orc_Size, const bool & orq_Maximized)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetDashboardToolboxPos(orc_Position);
      rc_View.SetDashboardToolboxSize(orc_Size);
      rc_View.SetDashboardToolboxMaximized(orq_Maximized);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetDashboardToolboxPos(orc_Position);
      c_View.SetDashboardToolboxSize(orc_Size);
      c_View.SetDashboardToolboxMaximized(orq_Maximized);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view dashboard selected tab index

   \param[in]  orc_ViewName   Project system view name (identifier)
   \param[in]  os32_Index     Dashboard selected tab index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvDashboardSelectedTabIndex(const QString & orc_ViewName, const int32_t os32_Index)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetDashboardSelectedTabIndex(os32_Index);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetDashboardSelectedTabIndex(os32_Index);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view dashboard integrated tab index

   \param[in]  orc_ViewName         Project system view name (identifier)
   \param[in]  orc_DashboardName    Dashboard name (identifier)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvDashboardMainTab(const QString & orc_ViewName, const QString & orc_DashboardName)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetDashboardMainTab(orc_DashboardName);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetDashboardMainTab(orc_DashboardName);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view dashboard torn off window data

   \param[in]  orc_ViewName               Project system view name (identifier)
   \param[in]  orc_DashboardName          Dashboard name (identifier)
   \param[in]  orc_Position               Torn off window position
   \param[in]  orc_Size                   Torn off window size
   \param[in]  oq_TornOffWindowMinimized  Torn off window minimized flag
   \param[in]  oq_TornOffWindowMaximized  Torn off window maximized flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvDashboardTearOffPosition(const QString & orc_ViewName, const QString & orc_DashboardName,
                                                    const QPoint & orc_Position, const QSize & orc_Size,
                                                    const bool oq_TornOffWindowMinimized,
                                                    const bool oq_TornOffWindowMaximized)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetDashboardTearOffPosition(orc_DashboardName, orc_Position, orc_Size, oq_TornOffWindowMinimized,
                                          oq_TornOffWindowMaximized);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetDashboardTearOffPosition(orc_DashboardName, orc_Position, orc_Size, oq_TornOffWindowMinimized,
                                         oq_TornOffWindowMaximized);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set project system view dashboard scene position and zoom

   \param[in]  orc_ViewName         Project system view name (identifier)
   \param[in]  orc_DashboardName    Dashboard name (identifier)
   \param[in]  orc_Position         Scene position
   \param[in]  os32_Zoom            Scene zoom
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjSvDashboardScenePositionAndZoom(const QString & orc_ViewName,
                                                         const QString & orc_DashboardName, const QPoint & orc_Position,
                                                         const int32_t os32_Zoom)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      //Do not insert as this will replace all currently known user settings for this item
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);
      rc_View.SetProjSvDashboardScenePositionAndZoom(orc_DashboardName, orc_Position, os32_Zoom);
   }
   else
   {
      C_UsSystemView c_View(C_UsHandler::mhs32_DEFAULT_ZOOM_LEVEL, C_UsHandler::mhc_DEFAULT_VIEW_POS);
      c_View.SetProjSvDashboardScenePositionAndZoom(orc_DashboardName, orc_Position, os32_Zoom);
      this->mc_ProjSvSetupView.insert(orc_ViewName, c_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the last parameters for system definition and system view screens

   \param[in]  os32_SysDefSubMode   Last sub mode of system definition
   \param[in]  ou32_SysDefIndex     Last index of system definition
   \param[in]  ou32_SysDefFlag      Last flag value of system definition
   \param[in]  os32_SysViewSubMode  Last sub mode of system view
   \param[in]  ou32_SysViewIndex    Last index of system view
   \param[in]  ou32_SysViewFlag     Last flag value of system view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjLastScreenMode(const int32_t os32_SysDefSubMode, const uint32_t ou32_SysDefIndex,
                                        const uint32_t ou32_SysDefFlag, const int32_t os32_SysViewSubMode,
                                        const uint32_t ou32_SysViewIndex, const uint32_t ou32_SysViewFlag)
{
   this->ms32_SysDefSubMode = os32_SysDefSubMode;
   this->mu32_SysDefIndex = ou32_SysDefIndex;
   this->mu32_SysDefFlag = ou32_SysDefFlag;

   this->ms32_SysViewSubMode = os32_SysViewSubMode;
   this->mu32_SysViewIndex = ou32_SysViewIndex;
   this->mu32_SysViewFlag = ou32_SysViewFlag;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last index for system definition node edit tab

   \param[out]  os32_SysDefNodeEditTabIndex   Tab index of node edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjLastSysDefNodeTabIndex(const int32_t os32_SysDefNodeEditTabIndex)
{
   this->ms32_SysDefNodeEditTabIndex = os32_SysDefNodeEditTabIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get the last index for system definition bus edit tabs

   \param[out]  os32_SysDefBusEditTabIndex    Tab index of bus edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetProjLastSysDefBusTabIndex(const int32_t os32_SysDefBusEditTabIndex)
{
   this->ms32_SysDefBusEditTabIndex = os32_SysDefBusEditTabIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the view user settings

   \param[in]  orc_SourceViewName   Source view name
   \param[in]  orc_TargetViewName   Target view name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::CopyProjSvSettings(const QString & orc_SourceViewName, const QString & orc_TargetViewName)
{
   if (this->mc_ProjSvSetupView.contains(orc_SourceViewName) == true)
   {
      const C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_SourceViewName);
      this->mc_ProjSvSetupView.insert(orc_TargetViewName, rc_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets permissions for setup, update, dashboard for specified view

   \param[in]       orc_ViewName     View name
   \param[in]      orc_Permissions   Array of permissions for Setup, Update, Dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetViewPermission(const QString & orc_ViewName, std::array<bool, 3> & orc_Permissions)
{
   if (this->mc_ProjSvSetupView.contains(orc_ViewName) == true)
   {
      C_UsSystemView & rc_View = this->mc_ProjSvSetupView.operator [](orc_ViewName);

      rc_View.SetSetupPermission(orc_Permissions[0]);
      rc_View.SetUpdatePermission(orc_Permissions[1]);
      rc_View.SetDashboardPermission(orc_Permissions[2]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clears saved maps in case of a new project

   A new project has no views, nodes or busses
   An other project could be have a different amount of views, nodes or busses
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::ClearMaps(void)
{
   this->mc_ProjSvSetupView.clear();
   this->mc_ProjSdNode.clear();
   this->mc_ProjSdBus.clear();
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
/*! \brief   Check if string valid language

   \param[in]  orc_Str  Language

   \return
   C_NO_ERR: exists
   C_RANGE:  does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::h_CheckLanguageExists(const QString & orc_Str)
{
   QStringList c_List;
   int32_t s32_Retval = C_RANGE;

   C_UsHandler::h_GetLanguages(c_List);
   for (uint8_t u8_It = 0; u8_It < c_List.count(); ++u8_It)
   {
      if (orc_Str.compare(c_List.at(u8_It)) == 0)
      {
         s32_Retval = C_NO_ERR;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get parent folder of path with or without file

   \param[in]   orc_CompletePath                Complete path
   \param[out]  orc_Parent                      Parent folder
   \param[in]   orq_CompletePathContainsFile    Indicator if complete path contains a file (filenames can't be handled automatically
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
   mc_IniPathAndName(C_Uti::h_GetExePath() + "/User/user_settings.ini"),
   mc_ActualProject(""),
   mc_DefaultProjectsFolder(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/openSYDE/Projects"),
   ms32_SdNodeEditSplitterHorizontal(1000),
   ms32_SdNodEditHalcSplitterHorizontal(400),
   ms32_SdNodEditCoManagerSplitterHorizontal(400),
   ms32_SdNodeEditDataLoggerSplitterHorizontal(400),
   ms32_SdBusEditTreeSplitterHorizontal(0),
   ms32_SdBusEditLayoutSplitterHorizontal(0)
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
