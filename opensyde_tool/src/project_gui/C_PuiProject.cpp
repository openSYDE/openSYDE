//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete project information (implementation)

   Complete project information.

   Note: For openSYDE_Test.pro there exists a simply copied dummy class of this class!

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDir>
#include <QWidget>
#include <QWindow>
#include <QPointer>
#include <QApplication>

#include "C_PuiProject.h"
#include "C_OSCAesFile.h"
#include "C_OSCZipFile.h"
#include "C_OSCProjectFiler.h"
#include "C_PuiSdHandler.h"
#include "CSCLString.h"
#include "TGLFile.h"
#include "stwerrors.h"
#include "C_Uti.h"
#include "C_UsHandler.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_PuiProject::mhc_TempFolder = "service_project_temp";
const QString C_PuiProject::mhc_ServiceProjectExtension = "syde_sp";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_PuiProject * C_PuiProject::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project

   \param[in]  oq_ForceSaveAll               Optional flag if all files should be saved
   \param[in]  oq_UseDeprecatedFileFormatV2  Flag to enable saving using the deprecated V2 file format

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
               no write access to file)
   C_RANGE    Path is empty
   C_NOACT    Could not create project directory
   C_COM      Bus sorting failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::Save(const bool oq_ForceSaveAll, const bool oq_UseDeprecatedFileFormatV2)
{
   sint32 s32_Retval;

   if (this->m_IsServiceModeProject() == false)
   {
      s32_Retval = this->m_Save(oq_ForceSaveAll, oq_UseDeprecatedFileFormatV2);
   }
   else
   {
      s32_Retval = this->m_SaveServiceModeProject(this->GetPath(), this->mc_Password);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save the current project encrypted with activated service to a specific path

   This function does not change the current opened project

   \param[in]       orc_FilePath     Path to save the service mode project
   \param[in]       orc_Password     Used password for service project encryption
   \param[in]       orc_ViewConfigs  Flags for view configuration which view function will
                                     be available in service project

   \retval   C_NO_ERR   Service project save with no problems
   \retval   C_RANGE    The view configuration is invalid
                        Path is empty
                        File extension does not match "syde_sp"
   \retval   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
                        no write access to file; could not open input file)
   \retval   C_NOACT    Could not create temporary project directory
                        could not add data to zip file (does the path to the file exist ?)
                        output file already exists or could not be written
   \retval   C_BUSY     Problems with deleting the temporary folder
   \retval   C_COM      Bus sorting failed
   \retval   C_WARN     No view or part of it is active
   \retval   C_CONFIG   at least one expected input file for the project generation is missing
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::SaveCurrentProjectForServiceMode(const QString & orc_FilePath, const QString & orc_Password,
                                                      const std::vector<std::array<bool, 3> > & orc_ViewConfigs)
{
   sint32 s32_Retval = C_RANGE;
   const uint32 u32_ViewCount = C_PuiSvHandler::h_GetInstance()->GetViewCount();
   const QFileInfo c_TargetFileInfo(orc_FilePath);

   QApplication::setOverrideCursor(Qt::WaitCursor);
   if ((orc_FilePath != "") &&
       (c_TargetFileInfo.suffix() == mhc_ServiceProjectExtension) &&
       (orc_ViewConfigs.size() == u32_ViewCount))
   {
      uint32 u32_ViewCounter;

      s32_Retval = C_WARN;

      // Adapt the system view configuration for the creation. At the end it will be reverted.
      C_PuiSvHandler::h_GetInstance()->SetServiceModeActive(true);
      for (u32_ViewCounter = 0U; u32_ViewCounter < u32_ViewCount; ++u32_ViewCounter)
      {
         const std::array<bool, 3> & rc_ViewConfig = orc_ViewConfigs[u32_ViewCounter];

         if ((rc_ViewConfig[0] == true) ||
             (rc_ViewConfig[1] == true) ||
             (rc_ViewConfig[2] == true))
         {
            // At least one screen of the view is active
            C_PuiSvHandler::h_GetInstance()->SetViewServiceModeActive(u32_ViewCounter, true);

            C_PuiSvHandler::h_GetInstance()->SetViewServiceModeSetupActive(u32_ViewCounter, rc_ViewConfig[0]);
            C_PuiSvHandler::h_GetInstance()->SetViewServiceModeUpdateActive(u32_ViewCounter, rc_ViewConfig[1]);
            C_PuiSvHandler::h_GetInstance()->SetViewServiceModeDashboardActive(u32_ViewCounter, rc_ViewConfig[2]);

            s32_Retval = C_NO_ERR;
         }
      }

      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = this->m_SaveServiceModeProject(orc_FilePath, orc_Password);
      }

      // Revert the system view configuration for the service mode in all cases
      C_PuiSvHandler::h_GetInstance()->SetServiceModeActive(false);
      for (u32_ViewCounter = 0U; u32_ViewCounter < u32_ViewCount; ++u32_ViewCounter)
      {
         C_PuiSvHandler::h_GetInstance()->SetViewServiceModeActive(u32_ViewCounter, false);
         C_PuiSvHandler::h_GetInstance()->SetViewServiceModeSetupActive(u32_ViewCounter, false);
         C_PuiSvHandler::h_GetInstance()->SetViewServiceModeUpdateActive(u32_ViewCounter, false);
         C_PuiSvHandler::h_GetInstance()->SetViewServiceModeDashboardActive(u32_ViewCounter, false);
      }
   }
   QApplication::restoreOverrideCursor();

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load project

   \param[in]  opu16_FileVersion    Optional storage for system definition file version

   \return
   C_RD_WR     Problems accessing file system (e.g. no read access to file)
   C_RANGE     A project file does not exist
   C_NOACT     A project file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    Content of a project file is invalid or incomplete
   C_COM       Device definition not found for project
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_CHECKSUM  password is incorrect
   C_BUSY      Problems with removing temporary folders
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::Load(uint16 * const opu16_FileVersion)
{
   sint32 s32_Retval;

   QApplication::setOverrideCursor(Qt::WaitCursor);

   if (this->m_IsServiceModeProject() == false)
   {
      s32_Retval = this->m_LoadProject(opu16_FileVersion);
   }
   else
   {
      s32_Retval = this->m_LoadServiceModeProject(this->mc_Password, opu16_FileVersion);
   }

   QApplication::restoreOverrideCursor();

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiProject::HasHashChanged(void) const
{
   const uint32 u32_NewHash = this->m_CalcHashProject();
   bool q_Changed = true;

   if (u32_NewHash == this->mu32_CalculatedProjectHash)
   {
      q_Changed = false;
   }

   return q_Changed;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set path of project (always absolute and with "/" as delimiters)

   \param[in]  orc_Path    Project path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::SetPath(const QString & orc_Path)
{
   if (orc_Path != "")
   {
      const QFileInfo c_File(orc_Path);
      mc_Path = QDir::cleanPath(c_File.absoluteFilePath());
   }
   else
   {
      mc_Path = orc_Path;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the current path for type of project

   The current password will be set here to an empty string in any case

   \retval   true   File is a service project with a configured password
   \retval   false  File is not a service project or a not encrypted password
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiProject::IsPasswordNecessary(void)
{
   bool q_Return = false;

   if (this->m_IsServiceModeProject() == true)
   {
      if (C_OSCZipFile::h_IsZipFile(this->GetPath().toStdString().c_str()) != C_NO_ERR)
      {
         // Not a zip file, so it is encrypted
         q_Return = true;
      }
   }

   this->mc_Password = "";

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the service project password

   Must be set before calling Load in case of a service project with a password

   \param[in]       orc_Password     Password for service project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::SetPassword(const QString & orc_Password)
{
   this->mc_Password = orc_Password;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current path

   \return
   Project file path, i.e. path of .syde file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiProject::GetPath(void) const
{
   return this->mc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current folder path without project name

   \return
   Path to directory of project file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiProject::GetFolderPath(void) const
{
   const QString c_Path = TGL_ExtractFilePath(this->mc_Path.toStdString().c_str()).c_str();

   return c_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current project name (name of *.syde file)

   \return
   name of project
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PuiProject::GetName(void) const
{
   QString c_Return = "";

   if (this->mc_Path != "")
   {
      const QFileInfo c_File(this->mc_Path);
      c_Return = c_File.completeBaseName();
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get information about project load switch.

   \return
   true     successfully loaded a provided project (switch to last known use case)
   false    loaded a project from recent project list (stay at main page)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiProject::GetSwitchUseCaseFlag() const
{
   return this->mq_SwitchToLastKnownUseCase;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiProject * C_PuiProject::h_GetInstance(void)
{
   if (C_PuiProject::mhpc_Singleton == NULL)
   {
      C_PuiProject::mhpc_Singleton = new C_PuiProject();
   }
   return C_PuiProject::mhpc_Singleton;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up singleton
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::h_Destroy(void)
{
   if (C_PuiProject::mhpc_Singleton != NULL)
   {
      delete (C_PuiProject::mhpc_Singleton);
      C_PuiProject::mhpc_Singleton = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get accumulated file size of all project files in byte

   \param[in]  orc_ProjectPath   Project path

   \return
   0    File not found
   Else Accumulated, relevant file sizes (Byte)
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 C_PuiProject::h_GetProjectSize(const QString & orc_ProjectPath)
{
   uint64 u64_Retval = 0ULL;
   QFileInfo c_ProjectFileInfo;

   c_ProjectFileInfo.setFile(orc_ProjectPath);
   if (c_ProjectFileInfo.exists() == true)
   {
      QString c_SystemDefinitionFilePath;
      QFileInfo c_SystemFileInfo;
      u64_Retval = c_ProjectFileInfo.size();

      C_PuiProject::h_AdaptProjectPathToSystemDefinition(orc_ProjectPath, c_SystemDefinitionFilePath);
      c_SystemFileInfo.setFile(c_SystemDefinitionFilePath);

      u64_Retval += static_cast<uint64>(c_SystemFileInfo.size());
   }
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Standard constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiProject::C_PuiProject(void) :
   C_OSCProject(),
   mu32_CalculatedProjectHash(),
   mq_SwitchToLastKnownUseCase(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Standard destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiProject::~C_PuiProject(void)
{
   //Clean up singleton
   try
   {
      C_PuiSvHandler::h_Destroy();
      C_PuiSdHandler::h_Destroy();
   }
   catch (...)
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare loading initial project

   Call before LoadInitialProject. If the return value equals C_CHECKSUM, set a password by calling SetPassword before
   calling LoadInitialProject.

   Loading Order:
      1. Provided argument
      2. If none: first project in recent projects list with existing .syde
      3. If failed: empty project (next recent projects are loaded afterwards)

  \return
   C_NO_ERR       Project prepared, no password necessary
   C_CHECKSUM     Password necessary, no real error
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::PrepareLoadInitialProject(void)
{
   sint32 s32_Return = C_NO_ERR;

   // Initial project from command line
   if (QApplication::arguments().size() > 1)
   {
      this->mq_SwitchToLastKnownUseCase = true;
      SetPath(QApplication::arguments().at(1));
   }
   // Search in recent projects for existing *.syde file
   else
   {
      const QStringList c_RecentProjects = C_UsHandler::h_GetInstance()->GetRecentProjects();
      this->mq_SwitchToLastKnownUseCase = false;
      SetPath("");
      for (sint32 s32_ItRecentProject = 0; s32_ItRecentProject < c_RecentProjects.count(); ++s32_ItRecentProject)
      {
         QFileInfo c_File;
         c_File.setFile(c_RecentProjects[s32_ItRecentProject]);
         if (c_File.exists() == true)
         {
            SetPath(c_RecentProjects[s32_ItRecentProject]);
            break;
         }
      }
   }

   if (this->IsPasswordNecessary() == true)
   {
      s32_Return = C_CHECKSUM;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load initial project

   PrepareLoadInitialProject must be called before calling this function.

   Loading Order:
      1. Provided argument
      2. If none: first project in recent projects list with existing .syde
      3. If failed: empty project (next recent projects are loaded afterwards)

   \param[out]  opu16_FileVersion   file version
   \param[out]  orc_LoadedProject   project that was tried to load (if load fails, this->mc_Path gets overwritten)

  \return
   C_RD_WR     Problems accessing file system (e.g. no read access to file)
   C_RANGE     A project file does not exist
   C_NOACT     A project file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    Content of a project file is invalid or incomplete
   C_COM       Device definition not found for project
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_CHECKSUM  password is incorrect
   C_BUSY      Problems with removing temporary folders
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::LoadInitialProject(uint16 * const opu16_FileVersion, QString & orc_LoadedProject)
{
   // Load it
   const sint32 s32_Error = this->Load(opu16_FileVersion);

   orc_LoadedProject = this->GetPath();

   if (s32_Error == C_NO_ERR)
   {
      //New recent project
      C_UsHandler::h_GetInstance()->AddToRecentProjects(this->GetPath());
      C_UsHandler::h_GetInstance()->Save();
   }
   else
   {
      // Load empty project on fail (User feedback for error and next try with recent projects is done by main window)
      this->LoadEmpty();

      // Always stay at main page when loading once failed
      this->mq_SwitchToLastKnownUseCase = false;
   }

   return s32_Error;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load empty project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::LoadEmpty(void)
{
   const C_OSCProject c_Tmp;

   //Default values
   this->mc_Path = "";
   this->c_Author = c_Tmp.c_Author;
   this->c_Editor = c_Author;
   this->c_CreationTime = c_Tmp.c_CreationTime;
   this->c_ModificationTime = c_Tmp.c_ModificationTime;
   this->c_Template = C_GtGetText::h_GetText("New project");
   this->c_Version = c_Tmp.c_Version;
   C_PuiSdHandler::h_GetInstance()->Clear();
   C_PuiSvHandler::h_GetInstance()->Clear();
   //Update hash
   this->mu32_CalculatedProjectHash = this->m_CalcHashProject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if project is never saved empty project

   \return
   True  Never saved
   False Has valid save location
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiProject::IsEmptyProject(void) const
{
   return (this->GetPath().compare("") == 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle all pending events to guarantee valid CRC check
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::h_HandlePendingEvents(void)
{
   QPointer<QWidget> const c_PreviousFocusWidget = QApplication::focusWidget();

   if (c_PreviousFocusWidget != NULL)
   {
      //Force focus change so any on focus change events (e.g. edit finished) will be processed before the CRC check
      c_PreviousFocusWidget->clearFocus();
      //Process focus change and handle all pending events which may cause project changes before checking the project
      // for changes
      QApplication::processEvents();

      //Restore original focus (if possible)
      if (c_PreviousFocusWidget != NULL)
      {
         c_PreviousFocusWidget->setFocus();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Automatically create sub-folder for system definition

   \param[in]  orc_ProjectPath            Project path
   \param[in]  orc_SystemDefintionPath    System definition path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::h_AdaptProjectPathToSystemDefinition(const QString & orc_ProjectPath,
                                                        QString & orc_SystemDefintionPath)
{
   const QFileInfo c_File(orc_ProjectPath);

   orc_SystemDefintionPath = c_File.path() + "/system_definition/" + c_File.completeBaseName() + ".syde_sysdef";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value of the project file

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiProject::m_CalcHashProject(void) const
{
   uint32 u32_Hash = 0xFFFFFFFFU;

   this->CalcHash(u32_Hash);

   return u32_Hash;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save project

   \param[in]  oq_ForceSaveAll               Optional flag if all files should be saved
   \param[in]  oq_UseDeprecatedFileFormatV2  Flag to enable saving using the deprecated V2 file format

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
               no write access to file)
   C_RANGE    Path is empty
   C_NOACT    Could not create project directory
   C_COM      Bus sorting failed
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::m_Save(const bool oq_ForceSaveAll, const bool oq_UseDeprecatedFileFormatV2)
{
   sint32 s32_Retval;
   const QFileInfo c_File(mc_Path);
   const QDir c_Directory(c_File.absolutePath());

   if (c_Directory.mkpath(".") == true)
   {
      C_PuiProject::h_HandlePendingEvents();
      s32_Retval = C_OSCProjectFiler::h_Save(*this,
                                             mc_Path.toStdString().c_str(),
                                             stw_opensyde_gui_logic::C_Uti::h_GetApplicationVersion(
                                                false).toStdString().c_str());
      if (s32_Retval == C_NO_ERR)
      {
         // save system definition only if it has changed
         if ((C_PuiSdHandler::h_GetInstance()->HasHashChanged() == true) || (oq_ForceSaveAll == true))
         {
            QString c_SystemDefintionPath;
            //Each format uses different folders!
            if (oq_UseDeprecatedFileFormatV2)
            {
               mh_AdaptProjectPathToSystemDefinitionV2(mc_Path, c_SystemDefintionPath);
            }
            else
            {
               h_AdaptProjectPathToSystemDefinition(mc_Path, c_SystemDefintionPath);
            }
            {
               const QFileInfo c_RefFile(c_SystemDefintionPath);
               const QDir c_Dir = c_RefFile.dir();
               //Create path (if necessary)
               if ((c_Dir.mkdir(".") == true) || (c_Dir.exists() == true))
               {
                  s32_Retval =
                     C_PuiSdHandler::h_GetInstance()->SaveToFile(
                        c_SystemDefintionPath.toStdString().c_str(), oq_UseDeprecatedFileFormatV2);
               }
               else
               {
                  s32_Retval = C_RD_WR;
               }
            }
         }

         if (s32_Retval == C_NO_ERR)
         {
            // save system views only if it has changed
            if ((C_PuiSvHandler::h_GetInstance()->HasHashChanged() == true) || (oq_ForceSaveAll == true))
            {
               QString c_SystemViewsPath;
               //Each format uses different folders!
               if (oq_UseDeprecatedFileFormatV2)
               {
                  mh_AdaptProjectPathToSystemViewsV1(mc_Path, c_SystemViewsPath);
               }
               else
               {
                  mh_AdaptProjectPathToSystemViews(mc_Path, c_SystemViewsPath);
               }
               {
                  const QFileInfo c_RefFile(c_SystemViewsPath);
                  const QDir c_Dir = c_RefFile.dir();
                  //Create path (if necessary)
                  if ((c_Dir.mkdir(".") == true) || (c_Dir.exists() == true))
                  {
                     s32_Retval = C_PuiSvHandler::h_GetInstance()->SaveToFile(
                        c_SystemViewsPath.toStdString().c_str(), oq_UseDeprecatedFileFormatV2);
                  }
                  else
                  {
                     s32_Retval = C_RD_WR;
                  }
               }
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Only update hash in non deprecated mode
         if (oq_UseDeprecatedFileFormatV2 == false)
         {
            //calculate the hash value and save it for comparing
            this->mu32_CalculatedProjectHash = this->m_CalcHashProject();
         }
      }
   }
   else
   {
      s32_Retval = C_NOACT;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save the current project encrypted with activated service to a specific path

   This function does not change the current opened project

   \param[in]       orc_FilePath     Path to save the service mode project
   \param[in]       orc_Password     Used password for service project encryption

   \retval   C_NO_ERR   Service project save with no problems
   \retval   C_RANGE    The view configuration is invalid
                        Path is empty
                        File extension does not match "syde_sp"
   \retval   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
                        no write access to file; could not open input file)
   \retval   C_NOACT    Could not create temporary project directory
                        could not add data to zip file (does the path to the file exist ?)
                        output file already exists or could not be written
   \retval   C_BUSY     Problems with deleting the temporary folder
   \retval   C_COM      Bus sorting failed
   \retval   C_CONFIG   at least one expected input file for the project generation is missing
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::m_SaveServiceModeProject(const QString & orc_FilePath, const QString & orc_Password)
{
   sint32 s32_Retval;

   const QFileInfo c_TargetFileInfo(orc_FilePath);
   const QString c_CurrentProjectPath = this->GetPath();
   const QString c_TargetFileName = c_TargetFileInfo.baseName();
   const QString c_ExePath = C_Uti::h_GetExePath();
   const QString c_TemporaryPath = c_ExePath + "/" + mhc_TempFolder + "/";
   const QString c_TemporaryProjectPath = c_TemporaryPath + c_TargetFileName + "/" + c_TargetFileName + ".syde";
   C_SCLString c_ErrorString;

   // Save the entire project to a temporary folder
   this->SetPath(c_TemporaryProjectPath);
   s32_Retval = this->Save(true, false);
   // Revert path configuration in any case
   this->SetPath(c_CurrentProjectPath);

   if (s32_Retval == C_NO_ERR)
   {
      std::vector<C_SCLString> c_AllFiles;

      C_Uti::h_GetAllFilePathsInFolder(c_TemporaryPath, c_AllFiles);

      // Create the encrypted zip file
      s32_Retval = C_OSCAesFile::h_CreateEncryptedZipFile(c_TemporaryPath.toStdString().c_str(),
                                                          c_AllFiles,
                                                          orc_FilePath.toStdString().c_str(),
                                                          orc_Password.toStdString().c_str(), &c_ErrorString);
   }

   //erase temporary folder:
   if (stw_tgl::TGL_RemoveDirectory(c_TemporaryPath.toStdString().c_str(), false) != 0)
   {
      // Do not overwrite any other error
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_BUSY;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load project

   \param[in]  opu16_FileVersion    Optional storage for system definition file version

   \return
   C_RD_WR  Problems accessing file system (e.g. no read access to file)
   C_RANGE  A project file does not exist
   C_NOACT  A project file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG Content of a project file is invalid or incomplete
   C_COM    Device definition not found for project
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::m_LoadProject(uint16 * const opu16_FileVersion)
{
   sint32 s32_Retval;

   if (this->mc_Path.compare("") == 0)
   {
      s32_Retval = C_NO_ERR;
      LoadEmpty();
   }
   else
   {
      //Load project file
      s32_Retval = C_OSCProjectFiler::h_Load(*this, this->mc_Path.toStdString().c_str());
      if (s32_Retval == C_NO_ERR)
      {
         QString c_SystemDefintionPath;
         //Try newest path
         h_AdaptProjectPathToSystemDefinition(this->mc_Path, c_SystemDefintionPath);
         {
            const QFileInfo c_FileInfoSysDef(c_SystemDefintionPath);
            //If this path does not work try the deprecated path
            if (c_FileInfoSysDef.exists() == false)
            {
               mh_AdaptProjectPathToSystemDefinitionV2(this->mc_Path, c_SystemDefintionPath);
            }
            //Load system definition
            s32_Retval = C_PuiSdHandler::h_GetInstance()->LoadFromFile(
               c_SystemDefintionPath.toStdString().c_str(), opu16_FileVersion);
            if (s32_Retval == C_NO_ERR)
            {
               QString c_SystemViewsPath;
               //Try newest path
               mh_AdaptProjectPathToSystemViews(this->mc_Path, c_SystemViewsPath);
               {
                  const QFileInfo c_FileInfoSysView(c_SystemViewsPath);
                  //If this path does not work try the deprecated path
                  if (c_FileInfoSysView.exists() == false)
                  {
                     mh_AdaptProjectPathToSystemViewsV1(this->mc_Path, c_SystemViewsPath);
                  }
                  //Load system views
                  s32_Retval = C_PuiSvHandler::h_GetInstance()->LoadFromFile(
                     c_SystemViewsPath.toStdString().c_str());
               }
            }
            else
            {
               if (s32_Retval == C_OVERFLOW)
               {
                  //remap "device not found" error code
                  s32_Retval = C_COM;
               }
            }
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //calculate the hash value and save it for comparing
      this->mu32_CalculatedProjectHash = this->m_CalcHashProject();
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load project

   \param[in]  orc_Password         Password for service project
   \param[in]  opu16_FileVersion    Optional storage for system definition file version

   \return
   C_RD_WR  Problems accessing file system (e.g. no read access to file)
   C_RANGE  A project file does not exist
   C_NOACT  A project file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG Content of a project file is invalid or incomplete
   C_COM    Device definition not found for project
   C_OVERFLOW  node in system definition references a device not part of the device definitions
   C_CHECKSUM  password is incorrect
   C_BUSY   Problems with removing temporary folders

*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::m_LoadServiceModeProject(const QString & orc_Password, stw_types::uint16 * const opu16_FileVersion)
{
   sint32 s32_Retval;
   const QString c_OriginalPath = this->GetPath();
   const QFileInfo c_FileInfo(this->GetPath());
   const QString c_ProjectName = c_FileInfo.baseName();
   const QString c_ExePath = C_Uti::h_GetExePath();
   const QString c_TemporaryPath = c_ExePath + "/" + mhc_TempFolder + "/";
   const QString c_TemporaryProjectPath = c_TemporaryPath + c_ProjectName + "/" + c_ProjectName + ".syde";
   C_SCLString c_ErrorString;

   // Decrypt the encrypted zip file
   s32_Retval = C_OSCAesFile::h_UnpackEncryptedZipFile(c_OriginalPath.toStdString().c_str(),
                                                       c_TemporaryPath.toStdString().c_str(),
                                                       orc_Password.toStdString().c_str(), &c_ErrorString);

   if (s32_Retval == C_NO_ERR)
   {
      this->SetPath(c_TemporaryProjectPath);
      s32_Retval = this->m_LoadProject(opu16_FileVersion);

      // Reset path
      this->SetPath(c_OriginalPath);
   }

   //erase temporary folder in any case:
   if (stw_tgl::TGL_RemoveDirectory(c_TemporaryPath.toStdString().c_str(), false) != 0)
   {
      // Do not overwrite any other error
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_BUSY;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the current path for type of project

   \retval   true   File is a service project (file extension "syde_sp")
   \retval   false  File is not a service project
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiProject::m_IsServiceModeProject(void) const
{
   const QFileInfo c_FileInfo(this->GetPath());
   bool q_Return;

   if (c_FileInfo.suffix() == mhc_ServiceProjectExtension)
   {
      q_Return = true;
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Automatically create sub-folder for system views

   \param[in]  orc_ProjectPath      Project path
   \param[in]  orc_SystemViewsPath  System views path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::mh_AdaptProjectPathToSystemViews(const QString & orc_ProjectPath, QString & orc_SystemViewsPath)
{
   const QFileInfo c_File(orc_ProjectPath);

   orc_SystemViewsPath = c_File.path() + "/system_views/" + c_File.completeBaseName() + ".syde_sysviews";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Automatically create sub-folder for system views (V1)

   \param[in]  orc_ProjectPath      Project path
   \param[in]  orc_SystemViewsPath  System views path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::mh_AdaptProjectPathToSystemViewsV1(const QString & orc_ProjectPath, QString & orc_SystemViewsPath)
{
   const QFileInfo c_File(orc_ProjectPath);

   orc_SystemViewsPath = c_File.path() + "/" + c_File.completeBaseName() + ".syde_sysviews";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Automatically create sub-folder for system definition (V2)

   \param[in]  orc_ProjectPath            Project path
   \param[in]  orc_SystemDefintionPath    System definition path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiProject::mh_AdaptProjectPathToSystemDefinitionV2(const QString & orc_ProjectPath,
                                                           QString & orc_SystemDefintionPath)
{
   const QFileInfo c_File(orc_ProjectPath);

   orc_SystemDefintionPath = c_File.path() + "/" + c_File.completeBaseName() + ".syde_sysdef";
}
