//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for creating secure archive file (.syde_sup)

   Dialog for creating secure archive file (.syde_sup)

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.hpp"

#include "C_SyvUpPacSecureArchiveDialog.hpp"
#include "ui_C_SyvUpPacSecureArchiveDialog.h"

#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_UsHandler.hpp"
#include "C_OscUtils.hpp"
#include "C_PuiUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecureArchiveDialog::C_SyvUpPacSecureArchiveDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent)
   :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpPacSecureArchiveDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);
   this->InitStaticNames();

   // register the widget for display
   this->mrc_ParentDialog.SetWidget(this);

   this->m_InitPwdLineEdit();

   this->mpc_Ui->pc_LineEditPath->SetDragAndDropActiveForFile("pem");

   // get path from user settings
   const QString c_FilePath = C_UsHandler::h_GetInstance()->GetLastKnownPrivateKeyPath();
   if (c_FilePath != "")
   {
      this->m_SetPrivateKeyPath(c_FilePath);
   }

   // connects
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked,
           this, &C_SyvUpPacSecureArchiveDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked,
           this, &C_SyvUpPacSecureArchiveDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_PubTogglePwd, &QPushButton::pressed, this,
           &C_SyvUpPacSecureArchiveDialog::m_ShowPassword);
   connect(this->mpc_Ui->pc_PubTogglePwd, &QPushButton::released, this,
           &C_SyvUpPacSecureArchiveDialog::m_HidePassword);
   connect(this->mpc_Ui->pc_PubPrivateKeyPath, &QPushButton::clicked, this,
           &C_SyvUpPacSecureArchiveDialog::m_PrivateKeyPathClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecureArchiveDialog::~C_SyvUpPacSecureArchiveDialog()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::InitStaticNames() const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Service Update Package "));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Secure Archive File (*.syde_sup)"));

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Create"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabPrivateKey->setText(C_GtGetText::h_GetText("Private Signature Key"));
   this->mpc_Ui->pc_LabPassword->setText(C_GtGetText::h_GetText("Password"));

   // Tooltips
   this->mpc_Ui->pc_LabPassword->SetToolTipInformation(C_GtGetText::h_GetText("Password (optional)"),
                                                       C_GtGetText::h_GetText(
                                                          "Private key will be encrypted if password is given."));
   this->mpc_Ui->pc_LabPrivateKey->SetToolTipInformation(C_GtGetText::h_GetText("Private key path"),
                                                         C_GtGetText::h_GetText(
                                                            "Full path to private key file (*.pem)."));
   this->mpc_Ui->pc_PubTogglePwd->SetToolTipInformation(C_GtGetText::h_GetText("Show/Hide Password"),
                                                        C_GtGetText::h_GetText(
                                                           "Shows/hides password when pressed down."));
   this->mpc_Ui->pc_PubPrivateKeyPath->SetToolTipInformation(C_GtGetText::h_GetText("Browse"),
                                                             C_GtGetText::h_GetText(
                                                                "Browse for path to private key(pem) file."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validates the private key path

   \retval   C_NOACT   Path is empty
   \retval   C_RANGE   File suffix *.pem missing
   \retval   C_RDWR    Invalid path (e.g. contains invalid characters)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacSecureArchiveDialog::m_CheckPath(void)
{
   int32_t s32_Return = C_NO_ERR;
   const QFileInfo c_FileInfo(this->GetPrivateKeyPath());

   if (this->GetPrivateKeyPath() == "")
   {
      s32_Return = C_NOACT;
   }
   else if (c_FileInfo.completeSuffix().compare("pem") != 0)
   {
      s32_Return = C_RANGE;
   }
   else if (C_OscUtils::h_CheckValidFilePath(this->GetPrivateKeyPath().toStdString().c_str()) == false)
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
/*! \brief   Slot of OK button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::m_OkClicked(void)
{
   const int32_t s32_Result = this->m_CheckPath();
   bool q_Continue = true;

   if (s32_Result != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_ErrorPath(this, C_OgeWiCustomMessage::eERROR);
      c_ErrorPath.SetHeading(C_GtGetText::h_GetText("Create Service Update Package"));

      switch (s32_Result)
      {
      case C_NOACT:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "You need to enter a signature file name."));
         break;
      case C_RANGE:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid file format. The format (*.pem) is required."));
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
         c_Message.SetHeading(C_GtGetText::h_GetText("Create service update package"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "No password specified. The service update package won't be encrypted. Do you want to continue?"));
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
      C_UsHandler::h_GetInstance()->SetLastKnownPrivateKeyPath(this->GetPrivateKeyPath());
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::m_CancelClicked(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initializes password line edit incl icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::m_InitPwdLineEdit(void)
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
void C_SyvUpPacSecureArchiveDialog::m_ShowPassword(void)
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Normal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password release
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::m_HidePassword(void)
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   this->mpc_Ui->pc_LineEditPassword->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the path to the private key
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::m_SetPrivateKeyPath(const QString & orc_Path) const
{
   this->mpc_Ui->pc_LineEditPath->SetPath(orc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for click on choose path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecureArchiveDialog::m_PrivateKeyPathClicked()
{
   const QString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditPath->GetPath());
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("pem file (*.pem);;Others (*.*)"));

   // Do not use QFileDialog::getOpenFileName because it does not support default suffix
   QFileDialog c_Dialog(this,  C_GtGetText::h_GetText("Select Output File"), c_Tmp, c_FilterName);

   c_Dialog.setDefaultSuffix("pem");

   if (c_Dialog.exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      QString c_FilePath;
      c_FilePath = c_Dialog.selectedFiles().at(0); // no multi select

      if (c_FilePath != "")
      {
         this->m_SetPrivateKeyPath(c_FilePath.toStdString().c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns content of password

   \return
   password in plain text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacSecureArchiveDialog::GetPassword(void) const
{
   return this->mpc_Ui->pc_LineEditPassword->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getter of full private key file path

   \return

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacSecureArchiveDialog::GetPrivateKeyPath(void) const
{
   return this->mpc_Ui->pc_LineEditPath->GetPath();
}
