//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog shows settings for creating a Service Project

   The user needs to specify a path and the permissions for the views which shall be accessible
   in the created service project. Password is optional. If no password is given the generated syde_sp file is a
   zip archive (can be opened via 7zip). If a password is given the generated syde_sp file will be encrypted.

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PopCreateServiceProjDialogWidget.hpp"
#include "ui_C_PopCreateServiceProjDialogWidget.h"
#include "C_OgeWiUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiProject.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "TglFile.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"

#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"
#include "C_UsFiler.hpp"
#include "C_UsHandler.hpp"

#include "C_PuiSvHandler.hpp"
#include "C_PuiSvData.hpp"
#include "C_PuiUtil.hpp"

#include "C_OgeTreeViewCheckable.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]  orc_Parent    Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopCreateServiceProjDialogWidget::C_PopCreateServiceProjDialogWidget(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_PopCreateServiceProjDialogWidget),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   // init names and line edit behaviour
   InitStaticNames();
   this->m_InitPwdLineEdit();

   this->mpc_Ui->pc_LineEditPath->SetDragAndDropActiveForFile("syde_sp");

   // set default path if nothing in user settings
   QString c_DefaultPath;
   if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
   {
      c_DefaultPath = "NewProject.syde_sp";
      this->SetSpPath(c_DefaultPath);
   }
   else if (C_UsHandler::h_GetInstance()->GetLastKnownServiceProjectPath() == "")
   {
      c_DefaultPath = C_PuiProject::h_GetInstance()->GetFolderPath() +
                      C_PuiProject::h_GetInstance()->GetName() +
                      ".syde_sp";
      this->SetSpPath(c_DefaultPath);
   }
   else
   {
      // use last known path from user settings
      this->SetSpPath(C_UsHandler::h_GetInstance()->GetLastKnownServiceProjectPath());
   }

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_PopCreateServiceProjDialogWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_PopCreateServiceProjDialogWidget::m_OnCancel);
   connect(this->mpc_Ui->pc_PubSpPath, &QPushButton::clicked, this,
           &C_PopCreateServiceProjDialogWidget::m_SpPathClicked);
   connect(this->mpc_Ui->pc_PubTogglePwd, &QPushButton::pressed, this,
           &C_PopCreateServiceProjDialogWidget::m_ShowPassword);
   connect(this->mpc_Ui->pc_PubTogglePwd, &QPushButton::released, this,
           &C_PopCreateServiceProjDialogWidget::m_HidePassword);
   connect(this->mpc_Ui->pc_TreeView, &C_OgeTreeViewToolTipBaseCheckable::SigSelectionChanged, this,
           &C_PopCreateServiceProjDialogWidget::m_UpdateSelection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PopCreateServiceProjDialogWidget::~C_PopCreateServiceProjDialogWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::InitStaticNames(void) const
{
   //set main title
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Service Project"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Create"));

   //labels and buttons
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabPermissions->setText(C_GtGetText::h_GetText("Permissions"));
   this->mpc_Ui->pc_LabPassword->setText(C_GtGetText::h_GetText("Password"));
   this->mpc_Ui->pc_LabPath->setText(C_GtGetText::h_GetText("Path"));
   this->mpc_Ui->pc_PubSpPath->setText("");
   this->mpc_Ui->pc_PubTogglePwd->setText("");
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Create"));

   // label tooltips
   this->mpc_Ui->pc_LabPassword->SetToolTipInformation(C_GtGetText::h_GetText("Password (optional)"),
                                                       C_GtGetText::h_GetText(
                                                          "Service project will be encrypted if password is given."));
   this->mpc_Ui->pc_LabPath->SetToolTipInformation(C_GtGetText::h_GetText("Service Project path"),
                                                   C_GtGetText::h_GetText(
                                                      "Full path to save as Service Project (*.syde_sp)."));
   this->mpc_Ui->pc_LabPermissions->SetToolTipInformation(C_GtGetText::h_GetText("Permissions"),
                                                          C_GtGetText::h_GetText(
                                                             "Specify which views shall be accessible within Service Project."));

   // button tooltips
   this->mpc_Ui->pc_PubTogglePwd->SetToolTipInformation(C_GtGetText::h_GetText("Show/Hide Password"),
                                                        C_GtGetText::h_GetText(
                                                           "Shows/hides password when pressed down."));
   this->mpc_Ui->pc_PubSpPath->SetToolTipInformation(C_GtGetText::h_GetText("Browse"),
                                                     C_GtGetText::h_GetText("Browse for export path."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initalizes the model for the view within the dialog
 *
 *  \param[in]    orc_ViewIndices   View indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::PrepareDialog(const std::vector<uint32_t> & orc_ViewIndices)
{
   this->mpc_Ui->pc_TreeView->Init(&this->mc_Model, orc_ViewIndices);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the checked items for service mode permissions

   \param[out]       orc_ViewIndices     View indices
   \param[out]      orc_ChildIndicesPerView   Vector of vectors of childs per view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::GetCheckedItems(std::vector<uint32_t> & orc_ViewIndices,
                                                         std::vector<std::vector<uint32_t> > & orc_ChildIndicesPerView)
const
{
   this->mpc_Ui->pc_TreeView->GetCheckedItems(&this->mc_Model, orc_ViewIndices, orc_ChildIndicesPerView);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns content of password line edit

   \return
   password in plain text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PopCreateServiceProjDialogWidget::GetPassword() const
{
   return this->mpc_Ui->pc_LineEditPassword->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getter of full service project file path

   \return

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_PopCreateServiceProjDialogWidget::GetSpPath(void) const
{
   return this->mpc_Ui->pc_LineEditPath->GetPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief Setter for full service project path

   \param[in]       orc_SpPath     Path to service project

*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::SetSpPath(const QString & orc_SpPath) const
{
   this->mpc_Ui->pc_LineEditPath->SetPath(orc_SpPath, C_PuiProject::h_GetInstance()->GetFolderPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls function to save project as service project and prepares parameters.
            Calls function to save permissions to user settings

   \retval   C_NO_ERR   Service project save with no problems
   \retval   C_RANGE    The view configuration is invalid
   \retval   C_RD_WR    problems accessing file system (could not erase pre-existing file before saving;
   \retval               no write access to file)
   \retval   C_RANGE    Path is empty
   \retval   C_NOACT    Could not create project directory
   \retval   C_COM      Bus sorting failed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PopCreateServiceProjDialogWidget::SaveCurrentProjectForServiceMode(void)
{
   int32_t s32_Retval;

   std::vector<std::array<bool, 3> > c_ViewConfigs = this->m_CollectViewConfig();
   const QString c_Path = C_PuiUtil::h_GetAbsolutePathFromProject(this->GetSpPath());
   const QString c_Password = this->GetPassword();

   s32_Retval = C_PuiProject::h_GetInstance()->SaveCurrentProjectForServiceMode(c_Path, c_Password, c_ViewConfigs);

   if (s32_Retval == C_NO_ERR)
   {
      this->m_SavePermissionsToUserSettings(c_ViewConfigs);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validates the service project export path

   \retval   C_NOACT   Path is empty
   \retval   C_RANGE   File suffix *.syde_sp missing
   \retval   C_RDWR   Invalid path (e.g. contains invalid characters)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PopCreateServiceProjDialogWidget::m_CheckPath(void)
{
   int32_t s32_Return = C_NO_ERR;
   const QFileInfo c_FileInfo(this->GetSpPath());

   if (this->GetSpPath() == "")
   {
      s32_Return = C_NOACT;
   }
   else if (c_FileInfo.completeSuffix().compare("syde_sp") != 0)
   {
      s32_Return = C_RANGE;
   }
   else if (C_OscUtils::h_CheckValidFilePath(this->GetSpPath().toStdString().c_str()) == false)
   {
      s32_Return = C_RD_WR;
   }
   else
   {
      //nothing to do here
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_OkClicked(void)
{
   const int32_t s32_Result = this->m_CheckPath();
   bool q_Continue = true;

   if (s32_Result != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_ErrorPath(this, C_OgeWiCustomMessage::eERROR);
      c_ErrorPath.SetHeading(C_GtGetText::h_GetText("Create Service Project"));

      switch (s32_Result)
      {
      case C_NOACT:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "You need to enter an output path."));
         break;
      case C_RANGE:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid Service Project format. File format (*.syde_sp) is required."));
         break;
      case C_RD_WR:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid path (e.g. containing invalid characters)."));
         break;
      default:
         break;
      }
      c_ErrorPath.SetOkButtonText(C_GtGetText::h_GetText("OK"));
      c_ErrorPath.SetCustomMinHeight(180, 180);
      c_ErrorPath.exec();

      q_Continue = false;
   }

   if (q_Continue == true)
   {
      // check for password
      if (this->GetPassword() == "")
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eWARNING);
         c_Message.SetHeading(C_GtGetText::h_GetText("Create service project"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "No password specified. The project won't be encrypted. Do you want to continue?"));
         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
         c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
         c_Message.SetCustomMinHeight(180, 180);
         if (c_Message.Execute() != C_OgeWiCustomMessage::eOK)
         {
            // canceled by user
            q_Continue = false;
         }
      }
      // q_Continue stays true if there is a password
   }

   if (q_Continue == true)
   {
      // check if specified file already exists
      if (C_OgeWiUtil::h_AskOverwriteFile(this, this->GetSpPath()) == C_NO_ERR)
      {
         this->mrc_ParentDialog.accept();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_OnCancel(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for click on choose path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_SpPathClicked()
{
   const QString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditPath->GetPath());
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("syde_sp file (*.syde_sp);;Others (*.*)"));

   // Do not use QFileDialog::getOpenFileName because it does not support default suffix
   QFileDialog c_Dialog(this,  C_GtGetText::h_GetText("Select Output File"), c_Tmp, c_FilterName);

   c_Dialog.setDefaultSuffix("syde_sp");

   if (c_Dialog.exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      QString c_FilePath;
      c_FilePath = c_Dialog.selectedFiles().at(0); // no multi select

      if (c_FilePath != "")
      {
         this->SetSpPath(c_FilePath.toStdString().c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initializes password line edit incl icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_InitPwdLineEdit(void)
{
   this->mpc_Ui->pc_LineEditPassword->setPlaceholderText("optional");
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditPassword, "NoRightBorder", true);
   this->mpc_Ui->pc_PubTogglePwd->setIcon(QIcon("://images/main_page_and_navi_bar/Icon_password_show.svg"));
   this->mpc_Ui->pc_PubTogglePwd->setMenu(NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password click & hold
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_ShowPassword(void)
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Normal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password release
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_HidePassword(void)
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   this->mpc_Ui->pc_LineEditPassword->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enables/disables create button based on selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_UpdateSelection(const int32_t os32_SelectionCount) const
{
   if (os32_SelectionCount > 0)
   {
      this->mpc_Ui->pc_PushButtonOk->setDisabled(false);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonOk->setDisabled(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Collecting configurations of each view based on "Setup", "Update", "Dashboard" got checked or not

   \retval   c_ViewConfig   Vector of arrays, which hold 3 booleans each. Each array represents a view
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<std::array<bool, 3> > C_PopCreateServiceProjDialogWidget::m_CollectViewConfig(void)
{
   std::vector<std::array<bool, 3> > c_ViewConfig;

   std::vector<uint32_t> c_ViewIndices;
   std::vector<std::vector<uint32_t> > c_ChildIndicesPerView;

   this->GetCheckedItems(c_ViewIndices, c_ChildIndicesPerView);

   for (uint32_t u32_ItView = 0; u32_ItView < c_ChildIndicesPerView.size(); ++u32_ItView)
   {
      std::array<bool, 3> c_Temp;
      bool q_Temp;
      for (uint32_t u32_ItChild = 0; u32_ItChild < c_ChildIndicesPerView[u32_ItView].size(); ++u32_ItChild)
      {
         if (c_ChildIndicesPerView[u32_ItView][u32_ItChild] == 1)
         {
            q_Temp = true;
         }
         else
         {
            q_Temp = false;
         }
         c_Temp[u32_ItChild] = q_Temp;
      }
      c_ViewConfig.push_back(c_Temp);
   }

   return c_ViewConfig;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Saves the current permissions for this project in user_settings.ini

   \param[in]       c_ViewConfigs     Vector of arrays, which hold 3 booleans each. Each array represents a view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PopCreateServiceProjDialogWidget::m_SavePermissionsToUserSettings(std::vector<std::array<bool,
                                                                                                3> > & orc_ViewConfigs)
{
   // get views
   const uint32_t u32_ViewCnt = C_PuiSvHandler::h_GetInstance()->GetViewCount();

   if (orc_ViewConfigs.size() == u32_ViewCnt)
   {
      for (uint32_t u32_ItView = 0; u32_ItView < u32_ViewCnt; ++u32_ItView)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(u32_ItView);
         if (pc_View != NULL)
         {
            const QString c_Name = pc_View->GetName().c_str();
            C_UsHandler::h_GetInstance()->SetViewPermission(c_Name, orc_ViewConfigs[u32_ItView]);
         }
      }
   }
}
