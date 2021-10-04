//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for main start view with use case buttons

   This widget is designed in a ui file.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

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
#include "C_PuiUtil.h"
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
#include "C_GiSyColorSelectWidget.h"
#include "C_PopCreateServiceProjDialogWidget.h"
#include "C_PopPasswordDialogWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
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
   connect(this->mpc_Ui->pc_BtnCreateServiceProj, &C_OgePubProjAction::clicked, this,
           &C_NagMainWidget::m_OnCreateServiceProj);
   connect(this->mpc_Ui->pc_BtnEdit, &C_OgePubIconOnly::clicked, this, &C_NagMainWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_LabelVersion, &C_OgeLabDoubleClick::SigDoubleClicked, this, &C_NagMainWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_LineEditVersion, &C_OgeLeDark::editingFinished, this, &C_NagMainWidget::m_OnEditFinished);
   connect(this->mpc_Ui->pc_BtnClear, &C_OgePubIconOnly::clicked, this, &C_NagMainWidget::m_OnClear);
   connect(this->mpc_Ui->pc_TableView, &C_PopFileTableView::clicked, this, &C_NagMainWidget::m_OnIndexClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagMainWidget::~C_NagMainWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the background element

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle edit leave

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::InitText(void) const
{
   QString c_String;

   //fake padding with Spaces.
   c_String = "    ";
   c_String.append(C_GtGetText::h_GetText("SYSTEM DEFINITION"));
   this->mpc_Ui->pc_BtnSysDef->setText(c_String);
   this->mpc_Ui->pc_BtnSysView->setText(C_GtGetText::h_GetText("SYSTEM COMMISSIONING"));
   this->mpc_Ui->pc_BtnAbout->setText(C_GtGetText::h_GetText("About"));
   this->mpc_Ui->pc_BtnNewProj->setText(C_GtGetText::h_GetText("New Project"));
   this->mpc_Ui->pc_BtnOpenProj->setText(C_GtGetText::h_GetText("Open Project"));
   this->mpc_Ui->pc_BtnSaveProjAs->setText(C_GtGetText::h_GetText("Save Project As"));
   this->mpc_Ui->pc_BtnCreateServiceProj->setText(C_GtGetText::h_GetText("Create Service Project"));
   this->mpc_Ui->pc_LabelCurProjTitle->setText(C_GtGetText::h_GetText("Current Project"));
   this->mpc_Ui->pc_LabelNoRecentProj->setText(C_GtGetText::h_GetText(
                                                  "No recent projects found, use \"Open Project\"."));
   this->mpc_Ui->pc_GroupBoxNoRecentProj->setTitle("");

   // tool tips
   this->mpc_Ui->pc_BtnNewProj->SetToolTipInformation(C_GtGetText::h_GetText("New Project"),
                                                      C_GtGetText::h_GetText("Create new empty project."));
   this->mpc_Ui->pc_BtnOpenProj->SetToolTipInformation(C_GtGetText::h_GetText("Open Project"),
                                                       C_GtGetText::h_GetText("Browse to open existing project."));
   this->mpc_Ui->pc_BtnSaveProjAs->SetToolTipInformation(C_GtGetText::h_GetText("Save Project As"),
                                                         C_GtGetText::h_GetText(
                                                            "Select name and location to save the project."));
   this->mpc_Ui->pc_BtnCreateServiceProj->SetToolTipInformation(C_GtGetText::h_GetText("Create Service Project"),
                                                                C_GtGetText::h_GetText(
                                                                   "Set a password and select the views that shall be accessible in your Service Project"));
   this->mpc_Ui->pc_BtnAbout->SetToolTipInformation(C_GtGetText::h_GetText("About"),
                                                    C_GtGetText::h_GetText("Show information about openSYDE tool."));
   this->mpc_Ui->pc_BtnSysDef->SetToolTipInformation(C_GtGetText::h_GetText("SYSTEM DEFINITION"),
                                                     C_GtGetText::h_GetText("Define your Network "
                                                                            "Topology, Nodes properties, Datapools, "
                                                                            "Interface description, "
                                                                            "generate files and more."));
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Perform initial project load.

   Instantiate C_PuiProject, perform initial project load, handle errors and update GUI.

   Do not try to load other recent projects, just open an empty project.
   (If the last recent project is invalid, all others might be invalid too, e.g. because of invalid
   devices.ini. In this case the user would be forced to close n message boxes if there are n recent projects
   in user settings - very annoying).
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::LoadInitialProject(void)
{
   uint16 u16_Version = 0U;
   QString c_LoadedProject;
   sint32 s32_Error = C_NO_ERR;

   if (C_PuiProject::h_GetInstance()->PrepareLoadInitialProject() == C_CHECKSUM)
   {
      QString c_Password;
      // The initial project is a service project with password. Get a password
      s32_Error = this->m_GetPassword(c_Password);

      if (s32_Error == C_NO_ERR)
      {
         C_PuiProject::h_GetInstance()->SetPassword(c_Password);
      }
   }

   if (s32_Error == C_NO_ERR)
   {
      // Initial load (command line or first recent project)
      s32_Error = C_PuiProject::h_GetInstance()->LoadInitialProject(&u16_Version, c_LoadedProject);
   }
   else if (s32_Error == C_WARN)
   {
      this->m_CancelPasswordDialog(u16_Version);
   }
   else
   {
      //No further steps necessary
   }

   C_PopErrorHandling::h_ProjectLoadErr(s32_Error, c_LoadedProject, this, u16_Version);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open a project.

   \param[in]  orc_FilePath   path to project file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::LoadProject(const QString & orc_FilePath)
{
   if (orc_FilePath.compare("") != 0)
   {
      uint16 u16_Version = 0U;
      stw_types::sint32 s32_Result;

      Q_EMIT (this->SigBeforeOtherProjectLoad());
      C_PuiProject::h_GetInstance()->SetPath(orc_FilePath);
      s32_Result = this->m_LoadConcreteProject(&u16_Version);
      C_PopErrorHandling::h_ProjectLoadErr(s32_Result, C_PuiProject::h_GetInstance()->GetPath(), this, u16_Version);

      if (s32_Result == C_NO_ERR)
      {
         C_UsHandler::h_GetInstance()->AddToRecentProjects(C_PuiProject::h_GetInstance()->GetPath());
         // Save new recent project configuration
         C_UsHandler::h_GetInstance()->Save();
         Q_EMIT (this->SigOtherProjectLoaded(true));
      }
      else
      {
         this->m_CancelPasswordDialog(u16_Version);
      }
   }

   this->UpdateRecentProjects();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating recent projects.
*/
//----------------------------------------------------------------------------------------------------------------------
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
      // if h_Load fails it still could be a syde_sp-file
      else
      {
         const QFileInfo c_FileInfo(*c_ItList);
         if (c_FileInfo.completeSuffix().compare("syde_sp") == 0)
         {
            // create a dummy project and append to c_Projects. Otherwise checks in UpdateData(l.360) will fail and
            // we want to display at least "???" for those properties we cannot get the information from .syde-file
            // date properties are handled in model class directly
            c_Tmp.c_Author = "N/A";
            c_Tmp.c_Editor = "N/A";
            c_Tmp.c_OpenSYDEVersion = "N/A";
            c_Tmp.c_Template = "N/A";
            c_Tmp.c_Version = "N/A";

            c_Files.push_back(*c_ItList);
            c_Projects.push_back(c_Tmp);
         }
      }
   }
   if (c_Files.size() > 0)
   {
      this->mpc_Ui->pc_LabRecentProjects->setText(
         static_cast<QString>(C_GtGetText::h_GetText("Recent Projects (%1)")).arg(c_Files.size()));
      this->mpc_Ui->pc_GroupBoxNoRecentProj->setVisible(false);
      this->mpc_Ui->pc_TableView->setVisible(true);
      this->mpc_Ui->pc_TableView->UpdateData(c_Files, c_Projects);
   }
   else
   {
      this->mpc_Ui->pc_LabRecentProjects->setText(C_GtGetText::h_GetText("Recent Projects"));
      this->mpc_Ui->pc_GroupBoxNoRecentProj->setVisible(true);
      this->mpc_Ui->pc_TableView->setVisible(false);
   }

   // update current project section
   m_UpdateCurrProjInfo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for save project as button click.
*/
//----------------------------------------------------------------------------------------------------------------------
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
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for create service project button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::OnCreateServiceProj(void)
{
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   C_PopCreateServiceProjDialogWidget * const pc_Dialog = new C_PopCreateServiceProjDialogWidget(*c_New);

   // get views for tree visualization of the project permissions
   std::vector<uint32> c_ViewIndices;
   c_ViewIndices.reserve(C_PuiSvHandler::h_GetInstance()->GetViewCount());
   for (uint32 u32_ItView = 0; u32_ItView < C_PuiSvHandler::h_GetInstance()->GetViewCount(); ++u32_ItView)
   {
      c_ViewIndices.push_back(u32_ItView);
   }

   // initializes tree model/view
   pc_Dialog->PrepareDialog(c_ViewIndices);

   // if user confirms everything in C_PopCreateServiceProjDialogWidget::m_OkClicked
   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      // save inputs as user settings
      const QString c_SpPath = C_PuiUtil::h_GetAbsolutePathFromProject(pc_Dialog->GetSpPath());
      C_UsHandler::h_GetInstance()->SetLastKnownServiceProjectPath(c_SpPath);

      // create service project
      const sint32 s32_Result = pc_Dialog->SaveCurrentProjectForServiceMode();
      C_PopErrorHandling::h_ServiceProjectSaveErr(s32_Result, pc_Dialog);
      if (s32_Result == C_NO_ERR)
      {
         C_OgeWiCustomMessage c_FinishMessage(this, C_OgeWiCustomMessage::E_Type::eINFORMATION);
         const QString c_Details =
            static_cast<QString>("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
            arg(C_GtGetText::h_GetText("File saved at ")).
            arg(static_cast<QString>(c_SpPath.toStdString().c_str())).
            arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
            arg(static_cast<QString>(c_SpPath.toStdString().c_str()));
         c_FinishMessage.SetHeading(C_GtGetText::h_GetText("Create Service Project"));
         c_FinishMessage.SetDescription(C_GtGetText::h_GetText(
                                           "Service Project created successfully."));
         c_FinishMessage.SetOKButtonText(C_GtGetText::h_GetText("OK"));
         c_FinishMessage.SetDetails(c_Details);
         c_FinishMessage.SetCustomMinHeight(180, 250);
         c_FinishMessage.Execute();
      }
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e593  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open color picker
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::OpenColorPicker(void)
{
   QPointer<C_OgePopUpDialog> const c_Popup = new C_OgePopUpDialog(this, this);
   C_GiSyColorSelectWidget * const pc_ColorWidget = new C_GiSyColorSelectWidget(*c_Popup, mc_STYLE_GUIDE_COLOR_7);

   //Resize
   c_Popup->SetSize(QSize(412, 620));

   // open color picker dialog
   if (c_Popup->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      pc_ColorWidget->ChooseSelectedColor();
   }

   if (c_Popup != NULL)
   {
      c_Popup->HideOverlay();
   }
} //lint !e429  //no memory leak because of the parent of pc_ColorWidget and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle service mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::HandleServiceMode(void) const
{
   const bool q_ServiceMode = C_PuiSvHandler::h_GetInstance()->GetServiceModeActive();

   this->mpc_Ui->pc_BtnSysDef->setEnabled(!q_ServiceMode);
   this->mpc_Ui->pc_BtnEdit->setEnabled(!q_ServiceMode);
   this->mpc_Ui->pc_BtnCreateServiceProj->setEnabled(!q_ServiceMode);
   this->mpc_Ui->pc_BtnSaveProjAs->setEnabled(!q_ServiceMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get first valid view for service mode

   \param[out]  oru32_ViewIndex  View index
   \param[out]  ors32_SubMode    Sub mode
   \param[out]  orq_Found        Found
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::h_GetFirstValidViewForServiceMode(uint32 & oru32_ViewIndex, sint32 & ors32_SubMode,
                                                        bool & orq_Found)
{
   orq_Found = false;

   for (uint32 u32_ItView = 0UL; (u32_ItView < C_PuiSvHandler::h_GetInstance()->GetViewCount()) && (!orq_Found);
        ++u32_ItView)
   {
      const C_PuiSvData * const pc_ViewData = C_PuiSvHandler::h_GetInstance()->GetView(u32_ItView);
      if (pc_ViewData != NULL)
      {
         if (pc_ViewData->GetServiceModeActive())
         {
            oru32_ViewIndex = u32_ItView;
            if (pc_ViewData->GetServiceModeSetupActive())
            {
               ors32_SubMode = ms32_SUBMODE_SYSVIEW_SETUP;
               orq_Found = true;
            }
            else if (pc_ViewData->GetServiceModeUpdateActive())
            {
               ors32_SubMode = ms32_SUBMODE_SYSVIEW_UPDATE;
               orq_Found = true;
            }
            else if (pc_ViewData->GetServiceModeDashboardActive())
            {
               ors32_SubMode = ms32_SUBMODE_SYSVIEW_DASHBOARD;
               orq_Found = true;
            }
            else
            {
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes paths of disabled icons.
*/
//----------------------------------------------------------------------------------------------------------------------
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
   this->mpc_Ui->pc_BtnCreateServiceProj->SetCustomIcon("://images/main_page_and_navi_bar/IconServiceProjEnabled.svg",
                                                        "://images/main_page_and_navi_bar/IconServiceProjDisabled.svg");

   // edit button
   this->mpc_Ui->pc_BtnEdit->SetCustomIcons("://images/main_page_and_navi_bar/IconEdit.svg", "", "",
                                            "://images/main_page_and_navi_bar/IconEditDisabled.svg");
   this->mpc_Ui->pc_BtnEdit->setIconSize(mc_ICON_SIZE_24);

   // clear button
   this->mpc_Ui->pc_BtnClear->setIcon(QIcon("://images/main_page_and_navi_bar/Icon_clear_table.svg"));
   this->mpc_Ui->pc_BtnClear->setIconSize(mc_ICON_SIZE_24);

   // settings (no icon -> less padding on right side)
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_BtnAbout, "NoIcon", true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize GUI elements.
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating current project informations.

   Update project name, project version and tooltip content.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_UpdateCurrProjInfo(void)
{
   const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_SEMIBOLD_24);
   const QFontMetrics c_FontMetrics(c_Font);
   const QString c_Name = C_PuiProject::h_GetInstance()->GetName();

   if (c_Name.compare("") != 0)
   {
      //Current project
      const QString c_TooltipContent =
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Author: %1 \nCreated: %2 \nLast Modified: %3 (by %4) \nUsed openSYDE version: %5")).
         arg(C_PuiProject::h_GetInstance()->c_Author.c_str()).
         arg(C_PuiProject::h_GetTimeFormatted(C_PuiProject::h_GetInstance()->c_CreationTime).c_str()).
         arg(C_PuiProject::h_GetTimeFormatted(C_PuiProject::h_GetInstance()->c_ModificationTime).c_str()).
         arg(C_PuiProject::h_GetInstance()->c_Editor.c_str()).
         arg(C_Uti::h_ConvertVersionToSTWStyle(C_PuiProject::h_GetInstance()->c_OpenSYDEVersion.c_str()));

      this->mpc_Ui->pc_LabelCurProjName->setText(c_FontMetrics.elidedText(c_Name, Qt::ElideRight,
                                                                          this->mpc_Ui->pc_LabelCurProjName->
                                                                          maximumWidth()));
      this->mpc_Ui->pc_LabelCurProjName->SetToolTipInformation(c_Name, c_TooltipContent);
      Q_EMIT (this->SigNewApplicationName(static_cast<QString>("openSYDE - %1 (%2)").
                                          arg(c_Name).arg(C_PuiProject::h_GetInstance()->GetPath())));
   }
   else
   {
      //New project
      this->mpc_Ui->pc_LabelCurProjName->setText(C_GtGetText::h_GetText("New project"));
      this->mpc_Ui->pc_LabelCurProjName->SetToolTipInformation(C_GtGetText::h_GetText("New project"),
                                                               C_GtGetText::h_GetText("This project is not yet saved."));
      Q_EMIT (this->SigNewApplicationName(static_cast<QString>("openSYDE - ") +
                                          static_cast<QString>(C_GtGetText::h_GetText("New project"))));
   }
   //Always use version from current project
   this->mpc_Ui->pc_LabelVersion->setText(c_FontMetrics.elidedText(C_PuiProject::h_GetInstance()->c_Version.c_str(),
                                                                   Qt::ElideLeft, 120));
   this->mpc_Ui->pc_LabelVersion->SetToolTipInformation(C_GtGetText::h_GetText("Project Version"),
                                                        C_PuiProject::h_GetInstance()->c_Version.c_str());
   this->mpc_Ui->pc_LineEditVersion->setText(C_PuiProject::h_GetInstance()->c_Version.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for button system definition click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_SysDefClicked()
{
   Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY, 0,
                               C_GtGetText::h_GetText("NETWORK TOPOLOGY")));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for button system view click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_SysViewClicked()
{
   uint32 u32_ViewIndex = 0UL;
   sint32 s32_Mode = ms32_SUBMODE_SYSVIEW_SETUP;
   bool q_Found = false;

   //Check system view service mode availability
   if (C_PuiSvHandler::h_GetInstance()->GetServiceModeActive())
   {
      C_NagMainWidget::h_GetFirstValidViewForServiceMode(u32_ViewIndex, s32_Mode, q_Found);
   }
   else
   {
      q_Found = true;
   }
   if (q_Found)
   {
      QString c_SubMode;
      QString c_SubSubMode;

      C_SyvUtil::h_GetViewDisplayName(u32_ViewIndex, s32_Mode, c_SubMode, c_SubSubMode);

      Q_EMIT (this->SigChangeMode(ms32_MODE_SYSVIEW, s32_Mode, u32_ViewIndex, c_SubMode,
                                  c_SubSubMode));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for button settings click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_AboutClicked()
{
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   new C_NagAboutDialog(*c_New, "openSYDE", ":/images/LogoOpensyde_XXL.png", 27,
                        C_GtGetText::h_GetText("QCustomPlot by Emanuel Eichhammer"));

   //Resize
   c_New->SetSize(QSize(650, 350));

   c_New->exec();

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for new project button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_OnNewProj(void)
{
   if (C_PopUtil::h_AskUserToContinue(this) == true)
   {
      Q_EMIT (this->SigBeforeOtherProjectLoad());
      C_PuiProject::h_GetInstance()->LoadEmpty();

      Q_EMIT (this->SigOtherProjectLoaded(true));
      Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_TOPOLOGY));

      UpdateRecentProjects();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for open project button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_OnOpenProj(void)
{
   if (C_PopUtil::h_AskUserToContinue(this) == true)
   {
      QString c_Folder = "";
      const QString c_Suffix = "*.syde;*.syde_sp";
      const QString c_Filter = static_cast<QString>(C_GtGetText::h_GetText("openSYDE project")) + " (" + c_Suffix + ")";

      C_UsHandler::h_GetInstance()->GetMostRecentFolder(c_Folder);

      const QString c_File = C_OgeWiUtil::h_GetOpenFileName(this, C_GtGetText::h_GetText("Open openSYDE Project"),
                                                            c_Folder, c_Filter, c_Suffix);
      if (c_File.isEmpty() == false)
      {
         LoadProject(c_File);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for create service project button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_OnCreateServiceProj()
{
   this->OnCreateServiceProj();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On edit action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_OnEdit(void) const
{
   if (C_PuiSvHandler::h_GetInstance()->GetServiceModeActive() == false)
   {
      this->mpc_Ui->pc_LabelVersion->setVisible(false);
      this->mpc_Ui->pc_LineEditVersion->setText(C_PuiProject::h_GetInstance()->c_Version.c_str());
      this->mpc_Ui->pc_LineEditVersion->setVisible(true);
      this->mpc_Ui->pc_LineEditVersion->selectAll();
      this->mpc_Ui->pc_LineEditVersion->setFocus();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On edit action finished
*/
//----------------------------------------------------------------------------------------------------------------------
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
   const QFontMetrics c_FontMetrics(c_Font);
   this->mpc_Ui->pc_LabelVersion->setText(c_FontMetrics.elidedText(C_PuiProject::h_GetInstance()->c_Version.c_str(),
                                                                   Qt::ElideLeft, 120));

   // update current project section (mainly for recent version in tooltip)
   this->m_UpdateCurrProjInfo();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for clear button click
*/
//----------------------------------------------------------------------------------------------------------------------
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
   c_MessageBox.SetCustomMinHeight(180, 180);
   //   c_MessageBox.SetCustomMinWidth(700);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      //clear table
      C_UsHandler::h_GetInstance()->ClearRecentProjects();
      this->UpdateRecentProjects();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for table click

   \param[in]  orc_ModelIndex    Model index
*/
//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Loads the concrete project

   The functions checks if the project needs a password for opening. If necessary the dialog for the password
   will be opened.

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
   C_WARN      User canceled the password dialog
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_NagMainWidget::m_LoadConcreteProject(uint16 * const opu16_FileVersion)
{
   sint32 s32_Result = C_NO_ERR;

   // Check if a password is necessary
   if (C_PuiProject::h_GetInstance()->IsPasswordNecessary() == true)
   {
      QString c_Password;
      // Password loop until the right one has been entered
      do
      {
         // Get the password
         s32_Result = this->m_GetPassword(c_Password);

         if (s32_Result == C_NO_ERR)
         {
            // Load project with given password
            C_PuiProject::h_GetInstance()->SetPassword(c_Password);
            s32_Result = C_PuiProject::h_GetInstance()->Load(opu16_FileVersion);

            if (s32_Result == C_CHECKSUM)
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Open Service Project"));
               c_Message.SetDescription(C_GtGetText::h_GetText("You entered the wrong password. Please try again."));
               c_Message.SetOKButtonText(C_GtGetText::h_GetText("OK"));
               c_Message.SetCustomMinHeight(180, 180);
               c_Message.exec();
            }
         }
      }
      while (s32_Result == C_CHECKSUM);
   }
   else
   {
      // No password, just load the project
      s32_Result = C_PuiProject::h_GetInstance()->Load(opu16_FileVersion);
   }

   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Shows the password dialog and returns the password entered by the user

   \param[out]  orc_Password  Password entered by user

   \return
   C_NO_ERR    User entered the password
   C_WARN      User canceled the dialog
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_NagMainWidget::m_GetPassword(QString & orc_Password)
{
   sint32 s32_Return = C_WARN;

   orc_Password = "";

   // show password dialog
   C_PopPasswordDialogWidget * const pc_Dialog = new C_PopPasswordDialogWidget(this);

   if (pc_Dialog->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      orc_Password = pc_Dialog->GetPassword();
      s32_Return = C_NO_ERR;
   }

   return s32_Return;
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Action that executed after cancel clicked on password dialog

   \param[in]  ou16_ProjectFileVersion    Project file version
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagMainWidget::m_CancelPasswordDialog(uint16 ou16_ProjectFileVersion)
{
   sint32 s32_Result;

   // we also want those projects in the "recent projects" list, which fail to load
   C_UsHandler::h_GetInstance()->AddToRecentProjects(C_PuiProject::h_GetInstance()->GetPath());
   C_UsHandler::h_GetInstance()->Save();

   // load last available recent project (where "available" here means: *.syde exists)
   const QStringList c_RecentProjects = C_UsHandler::h_GetInstance()->GetRecentProjects();

   Q_EMIT (this->SigBeforeOtherProjectLoad());
   C_PuiProject::h_GetInstance()->SetPath("");

   for (sint32 s32_ItRecentProject = 0; s32_ItRecentProject < c_RecentProjects.count(); ++s32_ItRecentProject)
   {
      QFileInfo c_File;
      c_File.setFile(c_RecentProjects[s32_ItRecentProject]);
      // do not load same project again
      if ((c_File.exists() == true) &&
          (c_RecentProjects[s32_ItRecentProject] != C_PuiProject::h_GetInstance()->GetPath()) &&
          (c_File.suffix() == "syde"))
      {
         C_PuiProject::h_GetInstance()->SetPath(c_RecentProjects[s32_ItRecentProject]);
         break;
      }
   }

   s32_Result = this->m_LoadConcreteProject(&ou16_ProjectFileVersion);

   // very unlikely, but the previous project could be corrupt -> check it and load empty project else
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
   Q_EMIT (this->SigOtherProjectLoaded(false));
}
