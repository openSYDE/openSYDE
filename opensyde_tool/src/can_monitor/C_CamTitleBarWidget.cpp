//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for title, about and help elements
 (implementation)

   Widget for title, about and help elements

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPixmap>
#include <QApplication>

#include "C_Uti.hpp"
#include "stwerrors.hpp"

#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_NagAboutDialog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_CamMosFilterPopup.hpp"
#include "C_HeHandler.hpp"
#include "C_CamProHandler.hpp"
#include "C_CamTitleBarWidget.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_UsHandler.hpp"
#include "C_CamUti.hpp"

#include "ui_C_CamTitleBarWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
//C_GtGetText does not work here (executed before main)
const QString C_CamTitleBarWidget::mhc_FILTER = "openSYDE CAN Monitor Project (*.syde_cam)";
const QString C_CamTitleBarWidget::mhc_NORECENTPROJECT = "No recent project found. Click here to browse.";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
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
   this->mpc_Ui->pc_PushButtonNew->setIconSize(QSize(22, 22));
   this->mpc_Ui->pc_PushButtonSave->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonSaveAs->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_ToolButtonLoad->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonHelp->setIconSize(QSize(24, 24));
   this->mpc_Ui->pc_PushButtonNew->SetSvg("://images/IconAddNewProj.svg");
   this->mpc_Ui->pc_PushButtonSave->SetSvg("://images/IconSave.svg");
   this->mpc_Ui->pc_PushButtonSaveAs->SetSvg("://images/IconProjSaveAs.svg");
   this->mpc_Ui->pc_ToolButtonLoad->SetSvg("://images/IconProjOpen.svg");
   this->mpc_Ui->pc_PushButtonHelp->SetSvg("://images/IconHelp.svg");
   this->mpc_Ui->pc_PushButtonHelp->setText("");
   this->mpc_Ui->pc_PushButtonHelp->SetIconOnly(true);

   // button style
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ToolButtonLoad, "MenuOpen", false);

   // fill menu with data
   this->UpdateRecentProjectsAndWindowTitle();

   // set menu to button
   this->mpc_Ui->pc_ToolButtonLoad->setMenu(this->mpc_Menu);
   this->mpc_Ui->pc_ToolButtonLoad->setPopupMode(QToolButton::MenuButtonPopup); // for two-button-style

   connect(this->mpc_Menu, &QMenu::aboutToShow, this, &C_CamTitleBarWidget::m_OnAboutToShowMenu);
   connect(this->mpc_Menu, &QMenu::aboutToHide, this, &C_CamTitleBarWidget::m_OnAboutToHideMenu);

   connect(this->mpc_Menu, &QMenu::triggered, this, &C_CamTitleBarWidget::m_OnRecentProjectSelected);
   connect(this->mpc_Ui->pc_PushButtonNew, &QPushButton::clicked, this, &C_CamTitleBarWidget::m_NewConfig);
   connect(this->mpc_Ui->pc_PushButtonSaveAs, &QPushButton::clicked, this, &C_CamTitleBarWidget::SaveAsConfig);
   connect(this->mpc_Ui->pc_PushButtonSave, &QPushButton::clicked, this, &C_CamTitleBarWidget::SaveConfig);
   connect(this->mpc_Ui->pc_PushButtonAbout, &QPushButton::clicked, this, &C_CamTitleBarWidget::m_ShowAbout);
   connect(this->mpc_Ui->pc_PushButtonHelp, &QPushButton::clicked, this, &C_CamTitleBarWidget::m_TriggerHelp);
   connect(this->mpc_Ui->pc_ToolButtonLoad, &QToolButton::clicked, this, &C_CamTitleBarWidget::m_OnOpenProjectClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamTitleBarWidget::~C_CamTitleBarWidget()
{
   delete this->mpc_Ui;
   delete this->mpc_Menu;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::InitStaticNames(void) const
{
   // initialize button texts
   this->mpc_Ui->pc_PushButtonAbout->setText(C_GtGetText::h_GetText("About")); // no icon therefore no icon-only mode
   this->m_SetButtonsText(false);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update recent projects menu and window title.

   Clear recent projects menu and add all known recent projects again.
   Furthermore set window title to new opened project
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::UpdateRecentProjectsAndWindowTitle(void)
{
   QString c_NewTitle;
   const QStringList & rc_RecentProjects = C_UsHandler::h_GetInstance()->GetRecentProjects();
   const QFileInfo c_ActualProject(C_CamProHandler::h_GetInstance()->GetCurrentFilePath());

   this->mpc_Menu->clear();

   // only show last 10 recent projects
   for (int32_t s32_Pos = 0; (s32_Pos < rc_RecentProjects.size()) && (s32_Pos < 10); ++s32_Pos)
   {
      this->mpc_Menu->addAction(rc_RecentProjects[s32_Pos]);
   }

   // show browse action if no recent project was found
   if (this->mpc_Menu->actions().size() == 0)
   {
      this->mpc_Menu->addAction(C_GtGetText::h_GetText(mhc_NORECENTPROJECT.toStdString().c_str()));
   }

   // update window title (window title of this widget is connected to main window)
   if (c_ActualProject.completeBaseName() == "")
   {
      c_NewTitle = static_cast<QString>("openSYDE CAN Monitor - New project");
   }
   else
   {
      c_NewTitle = static_cast<QString>("openSYDE CAN Monitor - %1 (%2)").arg(c_ActualProject.completeBaseName()).arg(
         c_ActualProject.absoluteFilePath());
   }
   //Send title changed signal to update main window title (this would only set child window title)
   Q_EMIT (this->windowTitleChanged(c_NewTitle));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle project comparison

   \return
   True  Continue
   False Abort
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamTitleBarWidget::HandleProjectComparison(void)
{
   bool q_Continue = true;

   if (C_CamProHandler::h_GetInstance()->CheckChanges())
   {
      C_OgeWiCustomMessage::E_Outputs e_Output;
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Unsaved changes"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you want to save the project changes?"));
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("Save"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Don't Save"));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle save configuration action
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle save configuration as action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::SaveAsConfig(void)
{
   QString c_PreviousDir = C_CamProHandler::h_GetInstance()->GetCurrentProjDir();
   QString c_FileName;

   if (c_PreviousDir.isEmpty())
   {
      c_PreviousDir = C_Uti::h_GetExePath();
   }
   c_FileName = C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("Save CAN Monitor Configuration"),
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle load configuration action

   \param[in]  orc_FilePath   path of project file to load

   \return
   see C_CamProHandler::LoadFromFile for return values
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_CamTitleBarWidget::LoadConfig(const QString & orc_FilePath)
{
   int32_t s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);
   s32_Return = C_CamProHandler::h_GetInstance()->LoadFromFile(orc_FilePath.toStdString().c_str());
   QApplication::restoreOverrideCursor();

   if (s32_Return != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      QString c_Details;

      c_Message.SetHeading(C_GtGetText::h_GetText("Project load"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Failed to load project: ") + orc_FilePath);
      c_Details = C_GtGetText::h_GetText("For more information see ");
      c_Details += C_Uti::h_GetLink(C_GtGetText::h_GetText("log file"), mc_STYLE_GUIDE_COLOR_LINK,
                                    C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
      c_Details += ".";
      c_Message.SetDetails(c_Details);

      //Update log file
      C_OscLoggingHandler::h_Flush();

      //Show message
      c_Message.Execute();
   }

   // update recent projects menu
   this->UpdateRecentProjectsAndWindowTitle();

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten resize event slot

   Here: remove or add button text depending on size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeWiOnlyBackground::resizeEvent(opc_Event);
   this->m_SetButtonsText(this->width() < 1100);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show about screen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_ShowAbout(void)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   new C_NagAboutDialog(*c_New, "openSYDE CAN Monitor", ":/images/CAN_Monitor_logo.png", 20);

   //Resize
   c_New->SetSize(QSize(650, 350));

   c_New->exec();

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Trigger help
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_TriggerHelp(void)
{
   stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(this->metaObject()->className());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle new configuration action
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle save to file action with valid file path

   \param[in]  orc_File    Selected file path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_DoSaveToFileAction(const QString & orc_File)
{
   int32_t s32_Return;

   QApplication::setOverrideCursor(Qt::WaitCursor);
   s32_Return = C_CamProHandler::h_GetInstance()->SaveToFile(orc_File.toStdString().c_str());
   QApplication::restoreOverrideCursor();

   if (s32_Return != C_NO_ERR)
   {
      const QString c_Log = C_Uti::h_GetLink(
         C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str(), mc_STYLE_GUIDE_COLOR_LINK,
         static_cast<QString>("file:\\\\\\") + C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Project save"));
      c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText("For more details see log file %1")).arg(
                                  c_Log));

      //Update log file
      C_OscLoggingHandler::h_Flush();

      //Show message
      c_Message.Execute();
   }

   // update recent projects menu
   this->UpdateRecentProjectsAndWindowTitle();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of open project button.

   Do unsaved changes check, open file dialog and trigger load.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnOpenProjectClicked(void)
{
   if (HandleProjectComparison() == true)
   {
      QString c_PreviousDir = C_CamProHandler::h_GetInstance()->GetCurrentProjDir();

      if (c_PreviousDir.isEmpty())
      {
         c_PreviousDir = C_Uti::h_GetExePath();
      }

      const QString c_FullFilePath =
         C_OgeWiUtil::h_GetOpenFileName(this, C_GtGetText::h_GetText("Select openSYDE CAN Monitor Project File"),
                                        c_PreviousDir, C_CamTitleBarWidget::mhc_FILTER, "*.syde_cam");

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of action triggered in recent-projects-menu.

   \param[in]  opc_Action  selected action in menu
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remap database paths to new location.

   Following convention is used:
   prior path was        relative                               absolute
                         |                                        |
                        relative to new location possible?        |
                         |                |                       |
                        yes               no                      |
                         |                |                       |
   make new path        relative          absolute             absolute

   \param[in]  orc_NewFileName   new save as path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_RemapOnSaveAs(const QString & orc_NewFileName) const
{
   // iterate over all databases
   std::vector<C_CamProDatabaseData> c_Databases = C_CamProHandler::h_GetInstance()->GetDatabases();
   for (uint32_t u32_Pos = 0; u32_Pos < c_Databases.size(); u32_Pos++)
   {
      C_CamProDatabaseData & rc_Database = c_Databases[u32_Pos];
      const QString c_PrevName = rc_Database.c_Name;
      const QFileInfo c_Info(rc_Database.c_Name);
      QString c_NewName = C_CamUti::h_GetAbsPathFromProj(rc_Database.c_Name);

      // if path was relative before try to make it relative to new project path
      if (c_Info.isRelative() == true)
      {
         const QDir c_Dir = static_cast<QFileInfo>(orc_NewFileName).dir();
         c_NewName = c_Dir.relativeFilePath(c_NewName);
      }

      // set new path
      C_CamProHandler::h_GetInstance()->SetDatabaseName(u32_Pos, c_NewName);
      C_CamProHandler::h_GetInstance()->ReplaceDatabaseName(c_PrevName, c_NewName);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set buttons text or remove them if only icon should be visible (in small width mode)

   \param[in]  oq_IconOnly    Flag to indicate icon only vs icon with text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_SetButtonsText(const bool oq_IconOnly) const
{
   this->mpc_Ui->pc_PushButtonNew->SetIconOnly(oq_IconOnly);
   this->mpc_Ui->pc_PushButtonSaveAs->SetIconOnly(oq_IconOnly);
   this->mpc_Ui->pc_PushButtonSave->SetIconOnly(oq_IconOnly);

   if (oq_IconOnly == true)
   {
      const uint32_t u32_SMALL_MAXIMUM_SIZE = 34;
      this->mpc_Ui->pc_PushButtonNew->setText(C_GtGetText::h_GetText(""));
      this->mpc_Ui->pc_ToolButtonLoad->setText(C_GtGetText::h_GetText(""));
      this->mpc_Ui->pc_PushButtonSaveAs->setText(C_GtGetText::h_GetText(""));
      this->mpc_Ui->pc_PushButtonSave->setText(C_GtGetText::h_GetText(""));

      this->mpc_Ui->pc_PushButtonNew->setMaximumWidth(u32_SMALL_MAXIMUM_SIZE);
      this->mpc_Ui->pc_ToolButtonLoad->setMaximumWidth((2 * u32_SMALL_MAXIMUM_SIZE) + 6);
      this->mpc_Ui->pc_PushButtonSaveAs->setMaximumWidth(u32_SMALL_MAXIMUM_SIZE);
      this->mpc_Ui->pc_PushButtonSave->setMaximumWidth(u32_SMALL_MAXIMUM_SIZE);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonNew->setText(C_GtGetText::h_GetText("New Project"));
      this->mpc_Ui->pc_ToolButtonLoad->setText(C_GtGetText::h_GetText("Open Project"));
      this->mpc_Ui->pc_PushButtonSaveAs->setText(C_GtGetText::h_GetText("Save Project As"));
      this->mpc_Ui->pc_PushButtonSave->setText(C_GtGetText::h_GetText("Save Project"));

      // we cannot change Qt constant but it is still better than using the hard coded magic number 16777215
      this->mpc_Ui->pc_PushButtonNew->setMaximumWidth(QWIDGETSIZE_MAX);    //lint !e893 !e9130 !e9136
      this->mpc_Ui->pc_ToolButtonLoad->setMaximumWidth(QWIDGETSIZE_MAX);   //lint !e893 !e9130 !e9136
      this->mpc_Ui->pc_PushButtonSaveAs->setMaximumWidth(QWIDGETSIZE_MAX); //lint !e893 !e9130 !e9136
      this->mpc_Ui->pc_PushButtonSave->setMaximumWidth(QWIDGETSIZE_MAX);   //lint !e893 !e9130 !e9136
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change color of left side of tool button on menu show.

   We need a style sheet property here, because only the menu button can have an own ":open" style;
   the rest of the button ignores this flag and uses pressed color.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnAboutToShowMenu(void) const
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ToolButtonLoad, "MenuOpen", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change color of left side of tool button on menu hide.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamTitleBarWidget::m_OnAboutToHideMenu(void) const
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ToolButtonLoad, "MenuOpen", false);
}
