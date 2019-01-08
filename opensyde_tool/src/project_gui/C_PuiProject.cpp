//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Complete project information (implementation)

   Complete project information.

   Note: For openSYDE_Test.pro there exists a simply copied dummy class of this class!

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QDir>
#include <QWidget>
#include <QWindow>
#include <QApplication>

#include "C_PuiProject.h"
#include "C_OSCProjectFiler.h"
#include "C_PuiSdHandler.h"
#include "CSCLString.h"
#include "TGLFile.h"
#include "stwerrors.h"
#include "C_Uti.h"
#include "C_UsHandler.h"
#include "C_PopErrorHandling.h"
#include "C_GtGetText.h"
#include "C_PuiSvHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
C_PuiProject * C_PuiProject::mhpc_Singleton = NULL;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Save project

   \param[in] oq_ForceSaveAll Optional flag if all files should be saved

   \return
   C_NO_ERR   data saved
   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
               no write access to file)
   C_RANGE    Path is empty
   C_NOACT    Could not create project directory
   C_COM      Bus sorting failed

   \created     15.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiProject::Save(const bool oq_ForceSaveAll)
{
   sint32 s32_Retval;
   QFileInfo c_File(mc_Path);
   QDir c_Directory(c_File.absolutePath());

   if (c_Directory.mkpath(".") == true)
   {
      C_PuiProject::h_HandlePendingEvents();
      //lint -e{40} Defined by project file
      s32_Retval = C_OSCProjectFiler::h_Save(*this, mc_Path.toStdString().c_str(), APPLICATION_VERSION);
      if (s32_Retval == C_NO_ERR)
      {
         // save system definition only if it has changed
         if ((C_PuiSdHandler::h_GetInstance()->HasHashChanged() == true) || (oq_ForceSaveAll == true))
         {
            QString c_SystemDefintionPath;
            h_AdaptProjectPathToSystemDefinition(mc_Path, c_SystemDefintionPath);
            {
               stw_scl::C_SCLString c_Path;
               //Create path
               c_Path = TGL_ExtractFilePath(c_SystemDefintionPath.toStdString().c_str());
               TGL_CreateDirectory(c_Path);
            }
            s32_Retval = C_PuiSdHandler::h_GetInstance()->SaveToFile(c_SystemDefintionPath.toStdString().c_str());
         }

         if (s32_Retval == C_NO_ERR)
         {
            QString c_SystemViewsPath;
            mh_AdaptProjectPathToSystemViews(mc_Path, c_SystemViewsPath);
            {
               stw_scl::C_SCLString c_Path;
               //Create path
               c_Path = TGL_ExtractFilePath(c_SystemViewsPath.toStdString().c_str());
               TGL_CreateDirectory(c_Path);
            }
            s32_Retval = C_PuiSvHandler::h_GetInstance()->SaveToFile(c_SystemViewsPath.toStdString().c_str());
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //calculate the hash value and save it for comparing
         this->mu32_CalculatedProjectHash = this->m_CalcHashProject();
      }
   }
   else
   {
      s32_Retval = C_NOACT;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load project

   \param[in]   orc_Path   Project path

   \return
   C_RD_WR  Problems accessing file system (e.g. no read access to file)
   C_RANGE  A project file does not exist
   C_NOACT  A project file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG Content of a project file is invalid or incomplete
   C_COM    Device definition not found for project
   C_OVERFLOW  node in system definition references a device not part of the device definitions

   \created     15.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_PuiProject::Load(void)
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
      s32_Retval = C_OSCProjectFiler::h_Load(*this, this->mc_Path.toStdString().c_str());
      if (s32_Retval == C_NO_ERR)
      {
         QString c_SystemDefintionPath;
         h_AdaptProjectPathToSystemDefinition(this->mc_Path, c_SystemDefintionPath);
         s32_Retval = C_PuiSdHandler::h_GetInstance()->LoadFromFile(c_SystemDefintionPath.toStdString().c_str());
         if (s32_Retval == C_NO_ERR)
         {
            QString c_SystemViewsPath;
            mh_AdaptProjectPathToSystemViews(this->mc_Path, c_SystemViewsPath);
            s32_Retval = C_PuiSvHandler::h_GetInstance()->LoadFromFile(c_SystemViewsPath.toStdString().c_str());
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
   if (s32_Retval == C_NO_ERR)
   {
      //calculate the hash value and save it for comparing
      this->mu32_CalculatedProjectHash = this->m_CalcHashProject();
   }
   QApplication::restoreOverrideCursor();
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Compares the last saved hash value against the actual hash

   \return
   false    Hash has not changed
   true     Hash has changed

   \created     10.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Set path of project (always absolute and with "/" as delimiters)

   \param[in]   orc_Path   Project path

   \created     16.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiProject::SetPath(const QString & orc_Path)
{
   if (orc_Path != "")
   {
      QFileInfo c_File(orc_Path);
      mc_Path = QDir::cleanPath(c_File.absoluteFilePath());
   }
   else
   {
      mc_Path = orc_Path;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get default path

   \created     16.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiProject::h_GetDefaultPath(void)
{
   return C_Uti::h_GetExePath() + "/Examples/ESX-3CM/ESX-3CM.syde";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current path

   \created     16.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_PuiProject::GetPath(void) const
{
   return this->mc_Path;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current folder path without project name

   \created     21.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QString C_PuiProject::GetFolderPath(void) const
{
   const QString c_Path = TGL_ExtractFilePath(this->mc_Path.toStdString().c_str()).c_str();

   return c_Path;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get information about project load switch.

   \return
   true     successfully loaded a provided project (switch to last known use case)
   false    loaded a project from recent project list (stay at main page)

   \created     08.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
bool C_PuiProject::GetSwitchUseCaseFlag() const
{
   return this->mq_SwitchToLastKnownUseCase;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get singleton (Create if necessary)

   \return
   Pointer to singleton

   \created     06.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiProject * C_PuiProject::h_GetInstance(void)
{
   if (C_PuiProject::mhpc_Singleton == NULL)
   {
      C_PuiProject::mhpc_Singleton = new C_PuiProject();
   }
   return C_PuiProject::mhpc_Singleton;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clean up singleton

   \created     06.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiProject::h_Destroy(void)
{
   if (C_PuiProject::mhpc_Singleton != NULL)
   {
      delete (C_PuiProject::mhpc_Singleton);
      C_PuiProject::mhpc_Singleton = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get accumulated file size of all project files in byte

   \param[in] orc_ProjectPath Project path

   \return
   0    File not found
   Else Accumulated, relevant file sizes (Byte)

   \created     15.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Standard constructor

   \param[in]   orc_Path   Project path

   \created     15.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiProject::C_PuiProject(void) :
   C_OSCProject(),
   mu32_CalculatedProjectHash(),
   mq_SwitchToLastKnownUseCase(true)
{
   m_InitialProjectLoad();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Standard destructor

   \created     15.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiProject::~C_PuiProject(void)
{
   //Clean up singleton
   C_PuiSvHandler::h_Destroy();
   C_PuiSdHandler::h_Destroy();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value of the project file

   Start value is 0xFFFFFFFF

   \return
   Calculated hash value

   \created     30.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_PuiProject::m_CalcHashProject(void) const
{
   uint32 u32_Hash = 0xFFFFFFFFU;

   this->CalcHash(u32_Hash);

   return u32_Hash;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load empty project

   \created     17.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiProject::LoadEmpty(void)
{
   C_OSCProject c_Tmp;

   //Default values
   this->mc_Path = "";
   this->c_Author = c_Tmp.c_Author;
   this->c_CreationTime = c_Tmp.c_CreationTime;
   this->c_ModificationTime = c_Tmp.c_ModificationTime;
   this->c_Template = C_GtGetText::h_GetText("New project");
   this->c_Version = c_Tmp.c_Version;
   C_PuiSdHandler::h_GetInstance()->Clear();
   C_PuiSvHandler::h_GetInstance()->Clear();
   //Update hash
   this->mu32_CalculatedProjectHash = this->m_CalcHashProject();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if project is never saved empty project

   \return
   True  Never saved
   False Has valid save location

   \created     13.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiProject::IsEmptyProject(void) const
{
   return (this->GetPath().compare("") == 0);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle all pending events to guarantee valid CRC check

   \created     07.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiProject::h_HandlePendingEvents(void)
{
   QWidget * const pc_PreviousFocusWidget = QApplication::focusWidget();

   if (pc_PreviousFocusWidget != NULL)
   {
      //At least two focus changes so we don't accidentally set the focus to the same widget
      uint32 u32_Counter = 0;
      const QWidgetList c_WidgetList = QApplication::allWidgets();
      //Set focus to this so any on focus change events (e.g. edit finished) will be processed before the CRC check
      for (QWidgetList::const_iterator c_It = c_WidgetList.begin(); (c_It != c_WidgetList.end()) && (u32_Counter < 2);
           ++c_It)
      {
         QWidget * const pc_Widget = (*c_It);
         if (pc_Widget != NULL)
         {
            pc_Widget->setFocus(Qt::MouseFocusReason);
            ++u32_Counter;
         }
      }
      //Process focus change and handle all pending events which may cause project changes before checking the project
      // for changes
      QApplication::processEvents();
      //Restore original focus
      pc_PreviousFocusWidget->setFocus();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Automatically create sub-folder for system definition

   \param[in]   orc_ProjectPath         Project path
   \param[in]   orc_SystemDefintionPath System definition path

   \created     15.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiProject::h_AdaptProjectPathToSystemDefinition(const QString & orc_ProjectPath,
                                                        QString & orc_SystemDefintionPath)
{
   QFileInfo c_File;

   c_File.setFile(orc_ProjectPath);

   orc_SystemDefintionPath = c_File.path() + "/" + c_File.baseName() + ".syde_sysdef";
}
//-----------------------------------------------------------------------------
/*!
   \brief   Automatically create sub-folder for system views

   \param[in]   orc_ProjectPath     Project path
   \param[in]   orc_SystemViewsPath System views path

   \created     22.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiProject::mh_AdaptProjectPathToSystemViews(const QString & orc_ProjectPath, QString & orc_SystemViewsPath)
{
   QFileInfo c_File;

   c_File.setFile(orc_ProjectPath);

   orc_SystemViewsPath = c_File.path() + "/" + c_File.baseName() + ".syde_sysviews";
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load initial project and handle occurring errors

   Loading Order:
      1. Provided argument
      2. If none: first project in recent projects list with existing .syde
      3. If failed: next project in recent projects list with existing .syde
      4. If failed: empty project

   \created     08.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_PuiProject::m_InitialProjectLoad()
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

   // load it (first try)
   s32_Error = this->Load();
   C_PopErrorHandling::mh_ProjectLoadErr(s32_Error, this->GetPath(), NULL);

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
      const QStringList c_RecentProjects = C_UsHandler::h_GetInstance()->GetRecentProjects();

      // On loading fail try next available recent project (where "available" here means: *.syde exists)
      this->mq_SwitchToLastKnownUseCase = false; // stay at main page when loading once failed
      this->SetPath("");

      for (sint32 s32_ItRecentProject = 0; s32_ItRecentProject < c_RecentProjects.count(); ++s32_ItRecentProject)
      {
         QFileInfo c_File;
         c_File.setFile(c_RecentProjects[s32_ItRecentProject]);
         if (c_File.exists() == true)
         {
            this->SetPath(c_RecentProjects[s32_ItRecentProject]);
            break;
         }
      }

      // load it (second try)
      s32_Error = this->Load();
      C_PopErrorHandling::mh_ProjectLoadErr(s32_Error, this->GetPath(), NULL);

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
         // load empty project if this second try also went wrong
         this->LoadEmpty();
      }
   }
}
