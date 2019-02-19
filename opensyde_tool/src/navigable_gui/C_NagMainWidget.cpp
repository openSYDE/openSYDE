//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for main start view with use case buttons

   This widget is designed in a ui file.

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include <QFileDialog>

#include "stwerrors.h"
#include "constants.h"
#include "C_SyvUtil.h"
#include "C_NagMainWidget.h"
#include "C_HeHandler.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "ui_C_NagMainWidget.h"

#include "C_PuiSvHandler.h"
#include "C_PuiSvData.h"
#include "C_PopUtil.h"
#include "C_UsHandler.h"
#include "C_PuiProject.h"
#include "C_PopErrorHandling.h"
#include "C_OgePopUpDialog.h"
#include "C_PopSaveAsDialogWidget.h"
#include "C_NagAboutDialog.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OSCProjectFiler.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

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

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_NagMainWidget::C_NagMainWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagMainWidget)
{
   this->mpc_Ui->setupUi(this);

   // Initialize buttons in disabled case
   this->m_InitIcons();
   this->m_InitGUIElements();

   //Initialize all visible strings on the widget
   this->InitText();

   // connect the buttons
   connect(this->mpc_Ui->pc_BtnSysDef, &QPushButton::clicked, this, &C_NagMainWidget::m_SysDefClicked);
   connect(this->mpc_Ui->pc_BtnSysView, &QPushButton::clicked, this, &C_NagMainWidget::m_SysViewClicked);
   connect(this->mpc_Ui->pc_BtnAbout, &QPushButton::clicked, this, &C_NagMainWidget::m_AboutClicked);
   connect(this->mpc_Ui->pc_BtnNewProj, &C_OgePubProjAction::clicked, this, &C_NagMainWidget::m_OnNewProj);
   connect(this->mpc_Ui->pc_BtnOpenProj, &C_OgePubProjAction::clicked, this, &C_NagMainWidget::m_OnOpenProj);
   connect(this->mpc_Ui->pc_BtnSaveProjAs, &C_OgePubProjAction::clicked, this, &C_NagMainWidget::OnSaveProjAs);
   connect(this->mpc_Ui->pc_BtnEdit, &C_OgePubIconOnly::clicked, this, &C_NagMainWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_LabelVersion, &C_OgeLabDoubleClick::SigDoubleClicked, this, &C_NagMainWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_LineEditVersion, &C_OgeLeDark::editingFinished, this, &C_NagMainWidget::m_OnEditFinished);
   connect(this->mpc_Ui->pc_BtnClear, &C_OgePubIconOnly::clicked, this, &C_NagMainWidget::m_OnClear);
   connect(this->mpc_Ui->pc_TableView, &C_PopFileTableView::clicked, this, &C_NagMainWidget::m_OnIndexClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_NagMainWidget::~C_NagMainWidget()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   //draw background
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this, &c_Painter);

   // draw 1px wide line between toolbar and main widget
   c_Painter.setPen(mc_STYLE_GUIDE_COLOR_6);
   c_Painter.drawLine(this->rect().topLeft(), this->rect().topRight());

   QWidget::paintEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle edit leave

   \param[in,out] opc_Event   Event identification and information

   \created     22.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_Escape)) &&
       (this->mpc_Ui->pc_LineEditVersion->isVisible() == true))
   {
      // reset version
      this->mpc_Ui->pc_LineEditVersion->setText(C_PuiProject::h_GetInstance()->c_Version.c_str());
      // finish editing
      this->m_OnEditFinished();
      //Don't call base and accept to properly signal a handled escape
      opc_Event->accept();
   }
   else
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     07.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::InitText(void) const
{
   this->mpc_Ui->pc_BtnSysView->setText(C_GtGetText::h_GetText("SYSTEM COMMISSIONING"));
   this->mpc_Ui->pc_BtnSysDef->setText(C_GtGetText::h_GetText("SYSTEM DEFINITION"));
   this->mpc_Ui->pc_BtnAbout->setText(C_GtGetText::h_GetText("About"));
   this->mpc_Ui->pc_BtnNewProj->setText(C_GtGetText::h_GetText("New Project"));
   this->mpc_Ui->pc_BtnOpenProj->setText(C_GtGetText::h_GetText("Open Project"));
   this->mpc_Ui->pc_BtnSaveProjAs->setText(C_GtGetText::h_GetText("Save Project as"));
   this->mpc_Ui->pc_LabelCurProjTitle->setText(C_GtGetText::h_GetText("Current Project"));
   this->mpc_Ui->pc_LabRecentProjects->setText(C_GtGetText::h_GetText("Recent Projects"));
   this->mpc_Ui->pc_LabelNoRecentProj->setText(C_GtGetText::h_GetText(
                                                  "No recent projects found, use \"Open Project\"."));
   this->mpc_Ui->pc_GroupBoxNoRecentProj->setTitle("");

   // tool tips
   this->mpc_Ui->pc_BtnNewProj->SetToolTipInformation(C_GtGetText::h_GetText("New Project"),
                                                      C_GtGetText::h_GetText("Create new empty project."));
   this->mpc_Ui->pc_BtnOpenProj->SetToolTipInformation(C_GtGetText::h_GetText("Open Project"),
                                                       C_GtGetText::h_GetText("Browse to open existing project."));
   this->mpc_Ui->pc_BtnSaveProjAs->SetToolTipInformation(C_GtGetText::h_GetText("Save Project as"),
                                                         C_GtGetText::h_GetText(
                                                            "Select name and location to save the project"));
   this->mpc_Ui->pc_BtnAbout->SetToolTipInformation(C_GtGetText::h_GetText("About"),
                                                    C_GtGetText::h_GetText("Show information about openSYDE tool."));
   this->mpc_Ui->pc_BtnSysDef->SetToolTipInformation(C_GtGetText::h_GetText("SYSTEM DEFINITION"),
                                                     C_GtGetText::h_GetText("Define your Network "
                                                                            "Topology, Nodes properties, Datapools, "
                                                                            "Interface description, "
                                                                            "generate code and more."));
   this->mpc_Ui->pc_BtnSysView->SetToolTipInformation(C_GtGetText::h_GetText("SYSTEM COMMISSIONING"),
                                                      C_GtGetText::h_GetText("Manage your system views, "
                                                                             "update your system, analyze your system "
                                                                             "behavior in Dashboards and more."));
   this->mpc_Ui->pc_BtnClear->SetToolTipInformation(C_GtGetText::h_GetText("Clear Recent Projects"),
                                                    C_GtGetText::h_GetText("Delete recent projects from user settings."));
   this->mpc_Ui->pc_BtnEdit->SetToolTipInformation(C_GtGetText::h_GetText("Edit Project Version"),
                                                   C_GtGetText::h_GetText(
                                                      "Edit version of your project. \nThis version "
                                                      "number gets saved on next project save."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes paths of disabled icons.

   \created     25.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_InitIcons(void) const
{
   // modes
   this->mpc_Ui->pc_BtnSysDef->SetDisabledIcon("://images/IconSystemDefinitionGray.svg");
   this->mpc_Ui->pc_BtnSysView->SetDisabledIcon("://images/IconSystemCommissioningGray.svg");

   // project operations
   this->mpc_Ui->pc_BtnNewProj->SetCustomIcon("://images/main_page_and_navi_bar/Icon_new_project.svg",
                                              "://images/main_page_and_navi_bar/Icon_new_project_disabled.svg");
   this->mpc_Ui->pc_BtnOpenProj->SetCustomIcon("://images/main_page_and_navi_bar/Icon_open_project.svg",
                                               "://images/main_page_and_navi_bar/Icon_open_project_disabled.svg");
   this->mpc_Ui->pc_BtnSaveProjAs->SetCustomIcon("://images/main_page_and_navi_bar/Icon_save_as.svg",
                                                 "://images/main_page_and_navi_bar/Icon_save_as_disabled.svg");

   // edit button
   this->mpc_Ui->pc_BtnEdit->setIcon(QIcon("://images/main_page_and_navi_bar/IconEdit.svg"));
   this->mpc_Ui->pc_BtnEdit->setIconSize(mc_ICON_SIZE_24);

   // clear button
   this->mpc_Ui->pc_BtnClear->setIcon(QIcon("://images/main_page_and_navi_bar/Icon_clear_table.svg"));
   this->mpc_Ui->pc_BtnClear->setIconSize(mc_ICON_SIZE_24);

   // settings (no icon -> less padding on right side)
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_BtnAbout, "NoIcon", true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize GUI elements.

   \created     27.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_InitGUIElements(void) const
{
   // stw logo
   QPixmap c_ImgLogo;

   c_ImgLogo.load(":images/STW_Logo.png");
   c_ImgLogo = c_ImgLogo.scaled((c_ImgLogo.width() / 12), (c_ImgLogo.height() / 12),
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
   this->mpc_Ui->pc_LabelLogo->setPixmap(c_ImgLogo);

   // labels
   this->mpc_Ui->pc_LabelCurProjTitle->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelCurProjTitle->SetForegroundColor(12);
   this->mpc_Ui->pc_LabelCurProjTitle->SetFontPixel(18, true);

   this->mpc_Ui->pc_LabRecentProjects->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabRecentProjects->SetForegroundColor(12);
   this->mpc_Ui->pc_LabRecentProjects->SetFontPixel(18, true);

   this->mpc_Ui->pc_LabelCurProjName->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelCurProjName->SetForegroundColor(12);
   this->mpc_Ui->pc_LabelCurProjName->SetFontPixel(24, false);

   this->mpc_Ui->pc_LabelVersion->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelVersion->SetForegroundColor(12);
   this->mpc_Ui->pc_LabelVersion->SetFontPixel(24, false);

   this->mpc_Ui->pc_LabelNoRecentProj->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelNoRecentProj->SetForegroundColor(10);
   this->mpc_Ui->pc_LabelNoRecentProj->SetFontPixel(12, false);

   // frames
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameCurrProj, "HasColor9Background", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameVersion, "HasColor9Background", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameRecentProjects, "HasColor9Background", true);

   // hide line edit version on start
   this->mpc_Ui->pc_LineEditVersion->setVisible(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for updating current project informations.

   Update project name, project version and tooltip content.

   \created     30.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_UpdateCurrProjInfo(void)
{
   const QString c_Name = C_PuiProject::h_GetInstance()->GetName();

   if (c_Name.compare("") != 0)
   {
      QFont c_Font = mc_STYLE_GUIDE_FONT_SEMIBOLD_24;
      c_Font.setPixelSize(c_Font.pointSize());
      QFontMetrics c_FontMetrics(c_Font);

      QString c_TooltipContent = QString(
         C_GtGetText::h_GetText("Author: %1 \nCreated: %2 \nLast Modified: %3 \nUsed openSYDE version: %4")).
                                 arg(C_PuiProject::h_GetInstance()->c_Author.c_str()).
                                 arg(C_PuiProject::h_GetTimeFormatted(
                                        C_PuiProject::h_GetInstance()->c_CreationTime).c_str()).
                                 arg(C_PuiProject::h_GetTimeFormatted(
                                        C_PuiProject::h_GetInstance()->c_ModificationTime).c_str()).
                                 arg(C_Uti::h_ConvertVersionToSTWStyle(
                                        C_PuiProject::h_GetInstance()->c_OpenSYDEVersion.c_str()));
      this->mpc_Ui->pc_LabelVersion->setText(c_FontMetrics.elidedText(
                                                C_PuiProject::h_GetInstance()->c_Version.c_str(), Qt::ElideLeft,
                                                120));
      this->mpc_Ui->pc_LabelVersion->SetToolTipInformation(C_GtGetText::h_GetText("Project Version"),
                                                           C_PuiProject::h_GetInstance()->c_Version.c_str());
      this->mpc_Ui->pc_LineEditVersion->setText(C_PuiProject::h_GetInstance()->c_Version.c_str());
      this->mpc_Ui->pc_LabelCurProjName->setText(c_FontMetrics.elidedText(c_Name, Qt::ElideRight,
                                                                          this->mpc_Ui->pc_LabelCurProjName->
                                                                          maximumWidth()));
      this->mpc_Ui->pc_LabelCurProjName->SetToolTipInformation(c_Name, c_TooltipContent);
      Q_EMIT (this->SigNewApplicationName(QString("openSYDE - %1 (%2)").
                                          arg(c_Name).arg(C_PuiProject::h_GetInstance()->GetPath())));
   }
   else
   {
      this->mpc_Ui->pc_LabelCurProjName->setText(C_GtGetText::h_GetText("New project"));
      this->mpc_Ui->pc_LabelVersion->setText("0.01r0b0");
      this->mpc_Ui->pc_LabelCurProjName->SetToolTipInformation("New project", "This project is not yet saved.");
      Q_EMIT (this->SigNewApplicationName(QString("openSYDE - ") + QString(C_GtGetText::h_GetText("New project"))));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for updating recent projects.

   \created     31.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::UpdateRecentProjects(void)
{
   std::vector<QString> c_Files;
   std::vector<C_OSCProject> c_Projects;
   const QStringList c_List = C_UsHandler::h_GetInstance()->GetRecentProjects();
   for (QStringList::const_iterator c_ItList = c_List.begin(); c_ItList != c_List.end(); ++c_ItList)
   {
      C_OSCProject c_Tmp;
      if (C_OSCProjectFiler::h_Load(c_Tmp, c_ItList->toStdString().c_str()) == C_NO_ERR)
      {
         c_Files.push_back(*c_ItList);
         c_Projects.push_back(c_Tmp);
      }
   }
   if (c_Files.size() > 0)
   {
      this->mpc_Ui->pc_GroupBoxNoRecentProj->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(true);
      this->mpc_Ui->pc_TableView->UpdateData(c_Files, c_Projects);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoRecentProj->setVisible(true);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }

   // update current project section
   m_UpdateCurrProjInfo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for button system definition click

   \created     07.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_SysDefClicked()
{
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY, 0,
                              C_GtGetText::h_GetText("Network Topology"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for button system view click

   \created     07.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_SysViewClicked()
{
   QString c_SubMode;
   QString c_SubSubMode;

   C_SyvUtil::h_GetViewDisplayName(0U, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);

   Q_EMIT this->SigChangeMode(ms32_MODE_SYSVIEW, ms32_SUBMODE_SYSVIEW_SETUP, 0, c_SubMode, c_SubSubMode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for button settings click

   \created     07.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_AboutClicked()
{
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   new C_NagAboutDialog(*c_New, "openSYDE", ":/images/LogoOpensyde_XXL.png", 27);

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
   \brief   Slot function for new project button click.

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_OnNewProj(void)
{
   if (C_PopUtil::h_AskUserToContinue(this) == true)
   {
      Q_EMIT this->SigBeforeOtherProjectLoad();
      C_PuiProject::h_GetInstance()->LoadEmpty();

      Q_EMIT this->SigOtherProjectLoaded(true);
      Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY);

      UpdateRecentProjects();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for open project button click.

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_OnOpenProj(void)
{
   if (C_PopUtil::h_AskUserToContinue(this) == true)
   {
      QString c_Folder;
      const QString c_Suffix = "syde";
      const QString c_Filter = QString(C_GtGetText::h_GetText("openSYDE project")) + " (*." + c_Suffix + ")";

      C_UsHandler::h_GetInstance()->GetMostRecentFolder(c_Folder);

      // do not use QFileDialog::getOpenFileName because it does not support default suffix
      // that we want to be added automatically (if typing "project" instead of "project.syde": load project.syde)
      QFileDialog c_Dialog(this, C_GtGetText::h_GetText("Open openSYDE Project"), c_Folder, c_Filter);
      c_Dialog.setDefaultSuffix(c_Suffix);
      if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
      {
         const QString c_File = c_Dialog.selectedFiles().at(0); // multi-selection is not possible
         LoadProject(c_File);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Open a project.

   \param[in]     orc_FilePath      path to project file

   \created     01.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::LoadProject(const QString & orc_FilePath)
{
   if (orc_FilePath.compare("") != 0)
   {
      stw_types::sint32 s32_Result;

      Q_EMIT this->SigBeforeOtherProjectLoad();
      C_PuiProject::h_GetInstance()->SetPath(orc_FilePath);
      s32_Result = C_PuiProject::h_GetInstance()->Load();
      C_PopErrorHandling::mh_ProjectLoadErr(s32_Result, C_PuiProject::h_GetInstance()->GetPath(), this);

      if (s32_Result == C_NO_ERR)
      {
         C_UsHandler::h_GetInstance()->AddToRecentProjects(C_PuiProject::h_GetInstance()->GetPath());
         // Save new recent project configuration
         C_UsHandler::h_GetInstance()->Save();
         Q_EMIT this->SigOtherProjectLoaded(true);
      }
      else
      {
         // Make sure to remove project from recent projects
         C_UsHandler::h_GetInstance()->RemoveOfRecentProjects(orc_FilePath);

         // load last available recent project (where "available" here means: *.syde exists)
         const QStringList c_RecentProjects = C_UsHandler::h_GetInstance()->GetRecentProjects();

         Q_EMIT this->SigBeforeOtherProjectLoad();
         C_PuiProject::h_GetInstance()->SetPath("");

         for (sint32 s32_ItRecentProject = 0; s32_ItRecentProject < c_RecentProjects.count(); ++s32_ItRecentProject)
         {
            QFileInfo c_File;
            c_File.setFile(c_RecentProjects[s32_ItRecentProject]);
            // do not load same project again
            if ((c_File.exists() == true) && (c_RecentProjects[s32_ItRecentProject] != orc_FilePath))
            {
               C_PuiProject::h_GetInstance()->SetPath(c_RecentProjects[s32_ItRecentProject]);
               break;
            }
         }

         s32_Result = C_PuiProject::h_GetInstance()->Load();

         // very unlikely, but the previous project could be corrupt -> check it and load empty project else
         C_PopErrorHandling::mh_ProjectLoadErr(s32_Result, C_PuiProject::h_GetInstance()->GetPath(), this);
         if (s32_Result == C_NO_ERR)
         {
            C_UsHandler::h_GetInstance()->AddToRecentProjects(C_PuiProject::h_GetInstance()->GetPath());
            // Save recent project configuration
            C_UsHandler::h_GetInstance()->Save();
         }
         else
         {
            C_PuiProject::h_GetInstance()->LoadEmpty();
         }
         Q_EMIT this->SigOtherProjectLoaded(false);
      }
   }

   this->UpdateRecentProjects();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for save project as button click.

   \created     26.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::OnSaveProjAs(void)
{
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   C_PopSaveAsDialogWidget * const pc_Dialog = new C_PopSaveAsDialogWidget(*c_New);
   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      UpdateRecentProjects();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   On edit action

   \created     30.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_OnEdit(void) const
{
   this->mpc_Ui->pc_LabelVersion->setVisible(false);
   this->mpc_Ui->pc_LineEditVersion->setText(C_PuiProject::h_GetInstance()->c_Version.c_str());
   this->mpc_Ui->pc_LineEditVersion->setVisible(true);
   this->mpc_Ui->pc_LineEditVersion->selectAll();
   this->mpc_Ui->pc_LineEditVersion->setFocus();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On edit action finished

   \created     30.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_OnEditFinished(void)
{
   const QString c_NewVersion = this->mpc_Ui->pc_LineEditVersion->text();

   this->mpc_Ui->pc_LabelVersion->setVisible(true);
   this->mpc_Ui->pc_LineEditVersion->setVisible(false);

   // memorize new version
   C_PuiProject::h_GetInstance()->c_Version = c_NewVersion.toStdString().c_str();

   // display elided new version
   QFont c_Font = mc_STYLE_GUIDE_FONT_SEMIBOLD_24;
   c_Font.setPixelSize(c_Font.pointSize());
   QFontMetrics c_FontMetrics(c_Font);
   this->mpc_Ui->pc_LabelVersion->setText(c_FontMetrics.elidedText(C_PuiProject::h_GetInstance()->c_Version.c_str(),
                                                                   Qt::ElideLeft, 120));

   // update current project section (mainly for recent version in tooltip)
   this->m_UpdateCurrProjInfo();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for clear button click

   \created     31.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_NagMainWidget::m_OnClear(void)
{
   // ask user to confirm
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Clear recent projects"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                  "Do you really want to clear the table and hence "
                                  "forget all recent project user settings?"));
   c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Clear"));
   c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
   c_MessageBox.SetCustomMinWidth(700);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      //clear table
      C_UsHandler::h_GetInstance()->ClearRecentProjects();
      this->UpdateRecentProjects();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for table click

   \created     31.07.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------

void C_NagMainWidget::m_OnIndexClicked(const QModelIndex & orc_ModelIndex)
{
   if (C_PopUtil::h_AskUserToContinue(this) == true)
   {
      QString c_FilePath;

      if (this->mpc_Ui->pc_TableView->ConvertRowToFile(orc_ModelIndex.row(), c_FilePath) == C_NO_ERR)
      {
         LoadProject(c_FilePath);
         this->mpc_Ui->pc_TableView->scrollToTop();
      }
   }
}
