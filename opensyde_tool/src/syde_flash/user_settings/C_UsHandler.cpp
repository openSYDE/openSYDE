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
#include "fla_constants.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

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
/*! \brief   Set default values

   Language = American english
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetDefault(void)
{
   this->mc_ScreenPos = QPoint(50, 50);
   this->mc_AppSize = QSize(1000, 700);
   this->mq_AppMaximized = true;
   this->mu32_ScreenIndex = 0;
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
/*! \brief   Get progress widget expanded flag.

   \return
   true: expanded, false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetWiProgressExpanded() const
{
   return this->mq_WiProgressExpanded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get advanced settings widget expanded flag.

   \return
   true: expanded, false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_UsHandler::GetWiAdvSettExpanded() const
{
   return this->mq_WiAdvSettExpanded;
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
/*! \brief   Get identification of settings subsection with open popup in collapsed state.

   \return
   section that was opened in collapsed mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::E_SettingsSubSection C_UsHandler::GetPopOpenSection() const
{
   return this->me_PopOpenSection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Node ID from Properties Widget

   \return
   Node ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetPropNodeId() const
{
   return this->ms32_PropNodeId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Bitrate from Properties Widget

   \return
   Bitrate
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetPropBitrate() const
{
   return this->ms32_PropBitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get Interface Index from Node Configuration Dialog

   \return
   Interface Index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetNodeCfgInterfaceIndex() const
{
   return this->ms32_NodeCfgInterfaceIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Flashloader Reset Wait Time

   \return
   Current Flashloader Reset Wait Time value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetFlashloaderResetWaitTime(void) const
{
   return this->ms32_FlashloaderResetWaitTime;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Request Download Timeout

   \return
   Current Request Download Timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetRequestDownloadTimeout(void) const
{
   return this->ms32_RequestDownloadTimeout;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get Transfer Data Timeout

   \return
   Current Transfer Data Timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_UsHandler::GetTransferDataTimeout(void) const
{
   return this->ms32_TransferDataTimeout;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Update Hex File Paths

   \return
   std::vector<QString>
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_UsHandler::GetLastKnownUpdateHexFilePaths() const
{
   return this->mc_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Update Hex File Paths as relative or absolute

   \return
   std::vector<QString>
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_UsHandler::GetHexFilePathsAsRelativeOrAbsolute() const
{
   return this->mc_HexFilePathsAsRelativeOrAbsolute;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get last know hex file location

   \return
   Last known hex file location
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetLastKnownUpdateHexFileLocation() const
{
   return this->mc_LastKnownUpdateHexFileLocation;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CAN DLL type

   \return
   CAN DLL type
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::E_CanDllType C_UsHandler::GetCanDllType() const
{
   return this->me_CanDllType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CAN DLL Path

   \return CAN DLL Path string (Peak/Vector/Custom)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetCanDllPath() const
{
   QString c_Return;

   switch (this->me_CanDllType)
   {
   case ePEAK:
      c_Return = mc_DLL_PATH_PEAK_FLA;
      break;
   case eVECTOR:
      c_Return = mc_DLL_PATH_VECTOR_FLA;
      break;
   case eOTHER:
      c_Return = this->mc_CustomCanDllPath;
      break;
   default:
      c_Return = mc_DLL_PATH_PEAK_FLA;
      break;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get custom CAN DLL path

   \return
   CAN DLL path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_UsHandler::GetCustomCanDllPath() const
{
   return this->mc_CustomCanDllPath;
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
/*! \brief  Set recent application screen index (for multi screen setup)

   \param[in]  ou32_New    New screen index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetAppScreenIndex(const uint32_t ou32_New)
{
   this->mu32_ScreenIndex = ou32_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set settings-splitter x position value

   \param[in]  os32_New    New settings-splitter x position value
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
/*! \brief   Set progress widget expanded.

   \param[in]  oq_New   expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiProgressExpanded(const bool oq_New)
{
   this->mq_WiProgressExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set advanced settings widget expanded.

   \param[in]  oq_New   expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiAdvSettExpanded(const bool oq_New)
{
   this->mq_WiAdvSettExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set DLL configuration widget expanded.

   \param[in]  oq_New   expanded flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetWiDllConfigExpanded(const bool oq_New)
{
   this->mq_WiDllConfigExpanded = oq_New;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set identification of settings subsection with open popup in collapsed state.

   \param[in]  oe_PopOpenSection    section that was opened in collapsed mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetPopOpenSection(const C_UsHandler::E_SettingsSubSection oe_PopOpenSection)
{
   this->me_PopOpenSection = oe_PopOpenSection;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Node ID from Properties widget

   \param[in]  os32_NodeId    Node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetPropNodeId(const int32_t os32_NodeId)
{
   this->ms32_PropNodeId = os32_NodeId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Bitrate from Properties widget

   \param[in]  os32_Bitrate   Bitrate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetPropBitrate(const int32_t os32_Bitrate)
{
   this->ms32_PropBitrate = os32_Bitrate;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Interface Index for connected interface in Node Configuration Dialog

   \param[in]  os32_InterfaceIndex   Interface Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetNodeCfgInterfaceIndex(const int32_t os32_InterfaceIndex)
{
   this->ms32_NodeCfgInterfaceIndex = os32_InterfaceIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Flashloader Reset Wait Time

   \param[in]  ors32_NewValue   NewValue
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetFlashloaderResetWaitTime(const int32_t & ors32_NewValue)
{
   this->ms32_FlashloaderResetWaitTime = ors32_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Request Download Timeout

   \param[in]  ors32_NewValue   NewValue
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetRequestDownloadTimeout(const int32_t & ors32_NewValue)
{
   this->ms32_RequestDownloadTimeout = ors32_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Transfer Data Timeout

   \param[in]  ors32_NewValue   NewValue
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetTransferDataTimeout(const int32_t & ors32_NewValue)
{
   this->ms32_TransferDataTimeout = ors32_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Update Hex File Paths

   \param[in]  orc_HexFilePaths   vector of hex file paths
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownUpdateHexFilePaths(const QStringList & orc_HexFilePaths)
{
   this->mc_HexFilePaths = orc_HexFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last known Update Hex File Paths as relative or absolute

   \param[in]  orc_HexFilePathsAsRelativeOrAbsolute   vector of hex file paths as relative or absolute
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetHexFilePathsAsRelativeOrAbsolute(const QStringList & orc_HexFilePathsAsRelativeOrAbsolute)
{
   this->mc_HexFilePathsAsRelativeOrAbsolute = orc_HexFilePathsAsRelativeOrAbsolute;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set last known Hex file location

   \param[in]  orc_NewPath    Hex file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetLastKnownUpdateHexFileLocation(const QString & orc_NewPath)
{
   this->mc_LastKnownUpdateHexFileLocation = orc_NewPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CAN DLL type

   \param[in]  oe_NewValue    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetCanDllType(const C_UsHandler::E_CanDllType oe_NewValue)
{
   this->me_CanDllType = oe_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CAN DLL path

   \param[in]  orc_NewValue   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::SetCustomCanDllPath(const QString & orc_NewValue)
{
   this->mc_CustomCanDllPath = orc_NewValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save all user setting to default ini file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsHandler::Save(void) const
{
   C_UsFiler::h_Save(*this, mc_IniPathAndName);
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
   mc_IniPathAndName(C_Uti::h_GetExePath() + "/User/syde_flash_user_settings.ini"),
   ms32_SplitterSettingsHorizontal(0),
   mq_SettingsAreExpanded(true),
   mq_WiProgressExpanded(true),
   mq_WiDllConfigExpanded(true),
   me_PopOpenSection(E_SettingsSubSection::eNONE),
   me_CanDllType(C_UsHandler::ePEAK)
{
   // Load all project independent information
   C_UsFiler::h_Load(*this, mc_IniPathAndName);
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
