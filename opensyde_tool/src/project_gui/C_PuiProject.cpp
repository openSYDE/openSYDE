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
sint32 C_PuiProject::Load(uint16 * const opu16_FileVersion)
{
   sint32 s32_Retval;

   QApplication::setOverrideCursor(Qt::WaitCursor);
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
   C_PuiSvHandler::h_Destroy();
   C_PuiSdHandler::h_Destroy();
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
/*! \brief   Load initial project

   Loading Order:
      1. Provided argument
      2. If none: first project in recent projects list with existing .syde
      3. If failed: empty project (next recent projects are loaded afterwards)

   \param[out]  opu16_FileVersion   file version
   \param[out]  orc_LoadedProject   project that was tried to load (if load fails, this->mc_Path gets overwritten)

  \return
   C_RD_WR  Problems accessing file system (e.g. no read access to file)
   C_RANGE  A project file does not exist
   C_NOACT  A project file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG Content of a project file is invalid or incomplete
   C_COM    Device definition not found for project
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiProject::LoadInitialProject(uint16 * const opu16_FileVersion, QString & orc_LoadedProject)
{
   sint32 s32_Error;

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

   // Load it
   s32_Error = this->Load(opu16_FileVersion);
   orc_LoadedProject = this->GetPath();

   if (s32_Error == C_NO_ERR)
   {
      //New recent project
      C_UsHandler::h_GetInstance()->AddToRecentProjects(this->GetPath());
      C_UsHandler::h_GetInstance()->Save();
   }
   else
   {
      // Make sure to remove project from recent projects
      C_UsHandler::h_GetInstance()->RemoveOfRecentProjects(this->GetPath());
      C_UsHandler::h_GetInstance()->Save();

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
