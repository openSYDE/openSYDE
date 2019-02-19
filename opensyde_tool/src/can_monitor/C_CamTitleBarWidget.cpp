//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for title, about and help elements
 (implementation)

   Widget for title, about and help elements


   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPixmap>
#include <QApplication>

#include "C_Uti.h"
#include "stwerrors.h"

#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_NagAboutDialog.h"
#include "C_OSCLoggingHandler.h"
#include "C_CamMosFilterPopup.h"
#include "C_HeHandler.h"
#include "C_CamProHandler.h"
#include "C_CamTitleBarWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "C_UsHandler.h"
#include "C_CamUti.h"

#include "ui_C_CamTitleBarWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
//C_GtGetText does not work here (executed before main)
const QString C_CamTitleBarWidget::mhc_FILTER = QString("openSYDE Parameter Set File (*%1)").arg(".syde_cam");
const QString C_CamTitleBarWidget::mhc_NORECENTPROJECT = "No recent project found. Click here to browse.";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamTitleBarWidget::C_CamTitleBarWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamTitleBarWidget),
   mpc_Menu(new C_CamOgeMuRecentProjects)
{
   QPixmap c_ImgLogo;

   this->mpc_Ui->setupUi(this);
   this->SetBackgroundColor(10);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_Frame, "HasColor8Background", true);

   // initialize static names
   this->InitStaticNames();

   //load STW logo
   c_ImgLogo.load("://images/STW_Logo_Dark.png");
   c_ImgLogo = c_ImgLogo.scaled((c_ImgLogo.width() / 18), (c_ImgLogo.height() / 18),
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
   this->mpc_Ui->pc_LogoLabel->setPixmap(c_ImgLogo);

   // button icons
   this->mpc_Ui->pc_PushButtonHelp->setText("");
   this->mpc_Ui->pc_PushButtonHelp->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonHelp->SetSvg("://images/IconHelp.svg");
   this->mpc_Ui->pc_PushButtonHelp->SetIconOnly(true);

   this->mpc_Ui->pc_PushButtonNew->setIconSize(QSize(22, 22));
   this->mpc_Ui->pc_PushButtonSave->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonSaveAs->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_ToolButtonLoad->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonNew->SetSvg("://images/IconAddNewProj.svg");
   this->mpc_Ui->pc_PushButtonSave->SetSvg("://images/IconSave.svg");
   this->mpc_Ui->pc_PushButtonSaveAs->SetSvg("://images/IconProjSaveAs.svg");
   this->mpc_Ui->pc_ToolButtonLoad->SetSvg("://images/IconProjOpen.svg");

   // button style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ToolButtonLoad, "MenuOpen", false);

   // fill menu with data
   this->UpdateRecentProjectsAndWindowTitle();

   // set menu to button
   this->mpc_Ui->pc_ToolButtonLoad->setMenu(this->mpc_Menu);
   this->mpc_Ui->pc_ToolButtonLoad->setPopupMode(QToolButton::MenuButtonPopup); // for two-button-style

   connect(this->mpc_Menu, &QMenu::aboutToShow, this, &C_CamTitleBarWidget::m_OnAboutToShow);
   connect(this->mpc_Menu, &QMenu::aboutToHide, this, &C_CamTitleBarWidget::m_OnAboutToHide);

   connect(this->mpc_Menu, &QMenu::triggered, this, &C_CamTitleBarWidget::m_OnRecentProjectSelected);
   connect(this->mpc_Ui->pc_PushButtonNew, &QPushButton::clicked, this, &C_CamTitleBarWidget::m_NewConfig);
   connect(this->mpc_Ui->pc_PushButtonSaveAs, &QPushButton::clicked, this, &C_CamTitleBarWidget::SaveAsConfig);
   connect(this->mpc_Ui->pc_PushButtonSave, &QPushButton::clicked, this, &C_CamTitleBarWidget::SaveConfig);
   connect(this->mpc_Ui->pc_PushButtonAbout, &QPushButton::clicked, this, &C_CamTitleBarWidget::m_ShowAbout);
   connect(this->mpc_Ui->pc_PushButtonHelp, &QPushButton::clicked, this, &C_CamTitleBarWidget::m_TriggerHelp);
   connect(this->mpc_Ui->pc_ToolButtonLoad, &QToolButton::clicked, this, &C_CamTitleBarWidget::m_OnOpenProjectClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamTitleBarWidget::~C_CamTitleBarWidget()
{
   delete this->mpc_Ui;
   delete this->mpc_Menu;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     23.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PushButtonAbout->setText(C_GtGetText::h_GetText("About"));
   this->mpc_Ui->pc_PushButtonNew->setText(C_GtGetText::h_GetText("New Project"));
   this->mpc_Ui->pc_ToolButtonLoad->setText(C_GtGetText::h_GetText("Open Project"));
   this->mpc_Ui->pc_PushButtonSaveAs->setText(C_GtGetText::h_GetText("Save Project As"));
   this->mpc_Ui->pc_PushButtonSave->setText(C_GtGetText::h_GetText("Save Project"));

   //tooltips
   this->mpc_Ui->pc_PushButtonHelp->SetToolTipInformation(C_GtGetText::h_GetText("Help"),
                                                          C_GtGetText::h_GetText("Open user manual for help."));
   this->mpc_Ui->pc_PushButtonAbout->SetToolTipInformation(C_GtGetText::h_GetText("About"),
                                                           C_GtGetText::h_GetText(
                                                              "Show information about openSYDE CAN Monitor."));

   this->mpc_Ui->pc_ToolButtonLoad->SetToolTipInformation(C_GtGetText::h_GetText("Open Project"),
                                                          C_GtGetText::h_GetText("Browse to open existing project."));
   this->mpc_Ui->pc_PushButtonNew->SetToolTipInformation(C_GtGetText::h_GetText("New Project"),
                                                         C_GtGetText::h_GetText("Create new empty project."));
   this->mpc_Ui->pc_PushButtonSave->SetToolTipInformation(C_GtGetText::h_GetText("Save Project"),
                                                          C_GtGetText::h_GetText("Save Project data to file."));
   this->mpc_Ui->pc_PushButtonSaveAs->SetToolTipInformation(C_GtGetText::h_GetText("Save Project as"),
                                                            C_GtGetText::h_GetText(
                                                               "Choose name and location to save the project."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update recent projects menu and window title.

   Clear recent projects menu and add all known recent projects again.
   Furthermore set window title to new opened project

   \created     18.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::UpdateRecentProjectsAndWindowTitle(void)
{
   QString c_NewTitle;
   const QStringList & rc_RecentProjects = C_UsHandler::h_GetInstance()->GetRecentProjects();
   const QFileInfo c_ActualProject(C_CamProHandler::h_GetInstance()->GetCurrentFilePath());

   this->mpc_Menu->clear();

   // only show last 10 recent projects
   for (sint32 s32_Pos = 0; (s32_Pos < rc_RecentProjects.size()) && (s32_Pos < 10); ++s32_Pos)
   {
      this->mpc_Menu->addAction(rc_RecentProjects[s32_Pos]);
   }

   // show browse action if no recent project was found
   if (this->mpc_Menu->actions().size() == 0)
   {
      this->mpc_Menu->addAction(C_GtGetText::h_GetText(mhc_NORECENTPROJECT.toStdString().c_str()));
   }

   // update window title (window title of this widget is connected to main window)
   if (c_ActualProject.baseName() == "")
   {
      c_NewTitle = QString("openSYDE CAN Monitor - New project");
   }
   else
   {
      c_NewTitle = QString("openSYDE CAN Monitor - %1 (%2)").arg(c_ActualProject.baseName()).arg(
         c_ActualProject.absoluteFilePath());
   }
   //Send title changed signal to update main window title (this would only set child window title)
   Q_EMIT (this->windowTitleChanged(c_NewTitle));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle project comparison

   \return
   True  Continue
   False Abort

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_CamTitleBarWidget::HandleProjectComparison(void)
{
   bool q_Continue = true;

   if (C_CamProHandler::h_GetInstance()->CheckChanges())
   {
      C_OgeWiCustomMessage::E_Outputs e_Output;
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Unsaved changes"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you want to save the project changes?"));
      c_Message.SetOKButtonText(C_GtGetText::h_GetText("Save"));
      c_Message.SetNOButtonText(C_GtGetText::h_GetText("Don't Save"));
      c_Message.ShowCancelButton();
      e_Output = c_Message.Execute();
      switch (e_Output)
      {
      case C_OgeWiCustomMessage::eYES:
         q_Continue = true;
         SaveConfig();
         break;
      case C_OgeWiCustomMessage::eNO:
         q_Continue = true;
         break;
      case C_OgeWiCustomMessage::eCANCEL:
         q_Continue = false;
         break;
      default:
         break;
      }
   }
   return q_Continue;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle save configuration action

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::SaveConfig(void)
{
   const QString c_FilePath = C_CamProHandler::h_GetInstance()->GetCurrentFilePath();

   //Check if save as necessary
   if (c_FilePath.isEmpty())
   {
      SaveAsConfig();
   }
   else
   {
      m_DoSaveToFileAction(c_FilePath);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle save configuration as action

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::SaveAsConfig(void)
{
   QString c_PreviousDir = C_CamProHandler::h_GetInstance()->GetCurrentProjDir();
   QString c_FileName;

   if (c_PreviousDir.isEmpty())
   {
      c_PreviousDir = C_Uti::h_GetExePath();
   }
   c_FileName = C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("Save can monitor configuration"),
                                               c_PreviousDir, C_CamTitleBarWidget::mhc_FILTER, "");
   if (c_FileName.isEmpty() == false)
   {
      // remap database paths (before save!)
      m_RemapOnSaveAs(c_FileName);

      // save to file
      m_DoSaveToFileAction(c_FileName);

      // update GUI
      Q_EMIT (this->SigSavedAsNew());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle load configuration action

   \param[in]  orc_FilePath   path of project file to load

   \return
   see C_CamProHandler::LoadFromFile for return values

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_CamTitleBarWidget::LoadConfig(const QString & orc_FilePath)
{
   sint32 s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);
   s32_Return = C_CamProHandler::h_GetInstance()->LoadFromFile(orc_FilePath.toStdString().c_str());
   QApplication::restoreOverrideCursor();

   if (s32_Return != C_NO_ERR)
   {
      const QString c_Log = C_Uti::h_GetLink(
         C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str(), mc_STYLE_GUIDE_COLOR_LINK,
         QString("file:\\\\\\") + C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str());
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project load"));
      c_Message.SetDescription(QString(C_GtGetText::h_GetText("For more details see log file %1")).arg(c_Log));

      //Update log file
      C_OSCLoggingHandler::h_Flush();

      //Show message
      c_Message.Execute();
   }

   // update recent projects menu
   this->UpdateRecentProjectsAndWindowTitle();

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show about screen

   \created     02.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_ShowAbout(void)
{
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   new C_NagAboutDialog(*c_New, "openSYDE CAN Monitor", ":/images/CAN_Monitor_logo.png", 20);

   //Resize
   c_New->SetSize(QSize(650, 350));

   c_New->exec();

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger help

   \created     04.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_TriggerHelp(void)
{
   //lint -e{10,48,64,746,1013,1055} Will be defined via moc compiler, PC lint unable to handle this construct
   stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(this->metaObject()->className());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle new configuration action

   \created     26.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_NewConfig(void)
{
   if (HandleProjectComparison())
   {
      // Let all modules save their specific user settings before saving to file
      Q_EMIT (this->SigPrepareForSave());
      // save user settings to remember all recently opened projects
      C_UsHandler::h_GetInstance()->Save();

      // clear project
      C_CamProHandler::h_GetInstance()->Clear();

      // update window title
      this->UpdateRecentProjectsAndWindowTitle();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle save to file action with valid file path

   \param[in] orc_File Selected file path

   \created     27.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_DoSaveToFileAction(const QString & orc_File)
{
   sint32 s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);
   s32_Return = C_CamProHandler::h_GetInstance()->SaveToFile(orc_File.toStdString().c_str());
   QApplication::restoreOverrideCursor();

   if (s32_Return != C_NO_ERR)
   {
      const QString c_Log = C_Uti::h_GetLink(
         C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str(), mc_STYLE_GUIDE_COLOR_LINK,
         QString("file:\\\\\\") + C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str());
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project save"));
      c_Message.SetDescription(QString(C_GtGetText::h_GetText("For more details see log file %1")).arg(c_Log));

      //Update log file
      C_OSCLoggingHandler::h_Flush();

      //Show message
      c_Message.Execute();
   }

   // update recent projects menu
   this->UpdateRecentProjectsAndWindowTitle();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of open project button.

   Do unsaved changes check, open file dialog and trigger load.

   \created     18.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnOpenProjectClicked(void)
{
   if (HandleProjectComparison() == true)
   {
      const QString c_FullFilePath = QFileDialog::getOpenFileName(this,
                                                                  C_GtGetText::h_GetText(
                                                                     "Select opeSYDE CAN monitor project file"),
                                                                  C_Uti::h_GetExePath(),
                                                                  C_CamTitleBarWidget::mhc_FILTER,
                                                                  NULL);
      if (c_FullFilePath != "")
      {
         // Let all modules save their specific user settings before saving to file
         Q_EMIT (this->SigPrepareForSave());
         // save user settings to not lose project dependent user settings
         C_UsHandler::h_GetInstance()->Save();

         // load configuration
         this->LoadConfig(c_FullFilePath);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of action triggered in recent-projects-menu.

   \param[in]     opc_Action         selected action in menu

   \created     18.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnRecentProjectSelected(const QAction * const opc_Action)
{
   const QString c_Path = opc_Action->text(); // remember path before project comparison

   // open browse dialog if no recent project was found
   if (c_Path == mhc_NORECENTPROJECT)
   {
      this->m_OnOpenProjectClicked();
   }
   else
   {
      if (HandleProjectComparison() == true)
      {
         // Let all modules save their specific user settings before saving to file
         Q_EMIT (this->SigPrepareForSave());
         // save user settings to not lose project dependent user settings
         C_UsHandler::h_GetInstance()->Save();

         // load configuration
         this->LoadConfig(c_Path);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remap database paths to new location.

   Following convention is used:
   prior path was        relative                               absolute
                         |                                        |
                        relative to new location possible?        |
                         |                |                       |
                        yes               no                      |
                         |                |                       |
   make new path        relative          absolute             absolute

   \param[in]  orc_NewFileName   new save as path

   \created     12.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_RemapOnSaveAs(const QString & orc_NewFileName) const
{
   // iterate over all databases
   std::vector<C_CamProDatabaseData> c_Databases = C_CamProHandler::h_GetInstance()->GetDatabases();
   for (uint32 u32_Pos = 0; u32_Pos < c_Databases.size(); u32_Pos++)
   {
      C_CamProDatabaseData & rc_Database = c_Databases[u32_Pos];
      const QFileInfo c_Info(rc_Database.c_Name);
      QString c_NewName = C_CamUti::h_GetAbsPathFromProj(rc_Database.c_Name);

      // if path was relative before try to make it relative to new project path
      if (c_Info.isRelative() == true)
      {
         QDir c_Dir = QFileInfo(orc_NewFileName).dir();
         c_NewName = c_Dir.relativeFilePath(c_NewName);
      }

      // set new path
      C_CamProHandler::h_GetInstance()->SetDatabaseName(u32_Pos, c_NewName);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change color of left side of tool button on menu show.

   We need a style sheet property here, because only the menu button can have an own ":open" style;
   the rest of the button ignores this flag and uses pressed color.

   \created     01.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnAboutToShow(void) const
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ToolButtonLoad, "MenuOpen", true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change color of left side of tool button on menu hide.

   \created     01.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnAboutToHide(void) const
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ToolButtonLoad, "MenuOpen", false);
}
