//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating Security Certificate Package

   Creating Security Certificate Package

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.hpp"
#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_SyvUpPacSecurityCertificatePackageDialog.hpp"
#include "ui_C_SyvUpPacSecurityCertificatePackageDialog.h"
#include "C_OgeWiUtil.hpp"
#include "C_PuiUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_UsHandler.hpp"
#include "C_OscUtils.hpp"
#include "C_OscSecurityPem.hpp"
#include "C_OscSecurityPemSecUpdate.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SclString.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecurityCertificatePackageDialog::C_SyvUpPacSecurityCertificatePackageDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvUpPacSecurityCertificatePackageDialog),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   m_InitStaticNames();
   this->m_UpdatePemFileWidget();
   this->m_ReloadLastSavedData();
   this->m_ReloadLastSavedPassword();
   this->m_UpdateLabelTitleWithFileCounter();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);
   this->mpc_Ui->pc_PEMFilesWidget->setAcceptDrops(true);

   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "",
                                                  "", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PushButtonClearall->SetCustomIcons("://images/system_views/IconClearAllEnabled.svg", "",
                                                       "", "://images/system_views/IconClearAllDisabled.svg");

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_HandleAddPemFile);
   connect(this->mpc_Ui->pc_PushButtonClearall, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_HandleClearAllPemFiles);
   connect(this->mpc_Ui->pc_PEMFilesWidget, &C_SyvUpPacPemFilesWidget::SigUpdateFileCounter, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_UpdateLabelTitleWithFileCounter);
   connect(this->mpc_Ui->pc_PubPublicKeyPath, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_PublicKeyPathClicked);
   connect(this->mpc_Ui->pc_PubTogglePassword, &QPushButton::pressed, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_ShowPassword);
   connect(this->mpc_Ui->pc_PubTogglePassword, &QPushButton::released, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_HidePassword);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecurityCertificatePackageDialog::~C_SyvUpPacSecurityCertificatePackageDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Getter of full public key file path

   \return QString .... public file path

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacSecurityCertificatePackageDialog::GetPublicKeyPath() const
{
   return this->mpc_Ui->pc_LineEditPublicPEMFile->GetPath();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns content of password

   \return
   password in plain text
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacSecurityCertificatePackageDialog::GetPassword() const
{
   return this->mpc_Ui->pc_LineEditPassword->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get pem files

   \return
   std::vector<stw::scl::C_SclString>   returns vector of Pem Files
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw::scl::C_SclString> C_SyvUpPacSecurityCertificatePackageDialog::GetPemFiles()
{
   std::vector<stw::scl::C_SclString> c_PemFilePaths;
   QStringList c_PemFiles = this->mpc_Ui->pc_PEMFilesWidget->GetPemFilePaths();

   for (const QString & rc_FilePath : c_PemFiles)
   {
      const stw::scl::C_SclString c_File(rc_FilePath.toStdString());
      c_PemFilePaths.push_back(c_File);
   }

   return c_PemFilePaths;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get option add pem files

   \return
   Flags

   \retval   True    Add pem files
   \retval   False   Dont add pem files
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacSecurityCertificatePackageDialog::GetOptionAddPemFiles() const
{
   return this->mpc_Ui->pc_ChxBoxAddPEMFiles->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get option add secure authentification

   \return
   Flags

   \retval   True    Add secure authentification
   \retval   False   Dont add secure authentification
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacSecurityCertificatePackageDialog::GetOptionAddSecureAuthentification() const
{
   return this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->mrc_ParentDialog.accept();
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
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_InitStaticNames(void) const
{
   //Secure Authentication
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Security Certificate Package"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Create"));
   this->mpc_Ui->pc_LabelSecureAuthentication->setText(C_GtGetText::h_GetText("Secure Authentication"));
   this->mpc_Ui->pc_ChxBoxAddPEMFiles->setText(C_GtGetText::h_GetText("Add new authentication PEM files"));
   this->mpc_Ui->pc_ChxBoxAddPEMFiles->SetToolTipInformation(C_GtGetText::h_GetText("Add new authentication PEM files"),
                                                             C_GtGetText::h_GetText(
                                                                "Secure Authentication: if \"Add new authentication PEM "
                                                                "files\" is enabled, new authentication PEM files are "
                                                                "added to the Security Certificate Package."));
   this->mpc_Ui->pc_LabelInstallationWorkflow->setText(C_GtGetText::h_GetText("Installation Workflow"));
   this->mpc_Ui->pc_LabelWorkflowInfo->setText(
      C_GtGetText::h_GetText(
         "When the Security Certificate Package is installed on X.App Node, following steps are done:\n"
         "- delete existing configuration for the security features \"Secure Authentication\" (private PEM files) and \"Secure Update\" (public PEM files).\n"
         "- Secure Authentication: if \"Add new authentication PEM files\" is enabled, new authentication PEM files are added.\n"
         "- Secure Update: if \"Apply new Secure Update configuration\" is enabled, new Secure Update config is added.")
      );
   if (this->mpc_Ui->pc_PEMFilesWidget != NULL)
   {
      this->mpc_Ui->pc_LabelNewAuthenticationPEMFile->setText(
         static_cast<QString>(C_GtGetText::h_GetText("New authentication PEM files (%1)")).arg(QString::number(this
                                                                                                               ->
                                                                                                               mpc_Ui->
                                                                                                               pc_PEMFilesWidget
                                                                                                               ->
                                                                                                               GetFilesCount())));
   }

   //Secure Update
   this->mpc_Ui->pc_LabelSecureUpdate->setText(C_GtGetText::h_GetText("Secure Update"));
   this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->setText(C_GtGetText::h_GetText(
                                                               "Apply new Secure Update configuration"));
   this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                             "Apply new Secure Update configuration"),
                                                                          C_GtGetText::h_GetText(
                                                                             "Secure Update: if "
                                                                             "\"Apply new Secure Update configuration\" "
                                                                             "is enabled, new Secure Update config is "
                                                                             "added to the Security Certificate Package."));
   this->mpc_Ui->pc_LabelNewConfiguration->setText(C_GtGetText::h_GetText("New Configuration"));
   this->mpc_Ui->pc_LabelPublicPEMFile->setText(C_GtGetText::h_GetText("Public PEM File"));
   this->mpc_Ui->pc_LabelPassword->setText(C_GtGetText::h_GetText("Encryption Password"));

   //Tooltips
   this->mpc_Ui->pc_PushButtonClearall->SetToolTipInformation(C_GtGetText::h_GetText("Clear"),
                                                              C_GtGetText::h_GetText(
                                                                 "Remove all PEM files."));
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                                         C_GtGetText::h_GetText(
                                                            "Add new PEM file."));
   this->mpc_Ui->pc_LabelPublicPEMFile->SetToolTipInformation(C_GtGetText::h_GetText("Signature Public Key"),
                                                              C_GtGetText::h_GetText(
                                                                 "Full path to public key file (*.pem)."));
   this->mpc_Ui->pc_LabelPassword->SetToolTipInformation(C_GtGetText::h_GetText("Encryption Password (optional)"),
                                                         C_GtGetText::h_GetText(
                                                            "Flashware will be encrypted if password is given."));
   this->mpc_Ui->pc_PubPublicKeyPath->SetToolTipInformation(C_GtGetText::h_GetText("Browse"),
                                                            C_GtGetText::h_GetText(
                                                               "Browse for path to public key file (PEM file)."));
   this->mpc_Ui->pc_PubTogglePassword->SetToolTipInformation(C_GtGetText::h_GetText("Show/Hide Password"),
                                                             C_GtGetText::h_GetText(
                                                                "Shows/hides password when pressed down."));

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Create"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_OkClicked(void)
{
   if (this->mpc_Ui->pc_PEMFilesWidget != NULL)
   {
      this->mpc_Ui->pc_PEMFilesWidget->SaveUserSettings();
   }
   C_UsHandler::h_GetInstance()->SetLastKnownPublicPemFilePath(this->mpc_Ui->pc_LineEditPublicPEMFile->text());
   C_UsHandler::h_GetInstance()->SetLastKnownPemFilePassword(this->mpc_Ui->pc_LineEditPassword->text());
   C_UsHandler::h_GetInstance()->SetLastKnownAddPemFileState(this->mpc_Ui->pc_ChxBoxAddPEMFiles->isChecked());
   C_UsHandler::h_GetInstance()->SetLastKnownSecureUpdateConfigState(
      this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->isChecked());
   if (this->m_OnValidateSecureAuthenticationConfig() == C_NO_ERR)
   {
      if (this->m_OnValidateSecureUpdateConfig() == C_NO_ERR)
      {
         this->mrc_ParentDialog.accept();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reloads last create package data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_ReloadLastSavedData()
{
   this->mpc_Ui->pc_ChxBoxAddPEMFiles->setChecked(C_UsHandler::h_GetInstance()->GetLastKnownAddPemFileState());
   this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->setChecked(
      C_UsHandler::h_GetInstance()->GetLastKnownSecureUpdateConfigState());
   this->mpc_Ui->pc_LineEditPublicPEMFile->SetPath(C_UsHandler::h_GetInstance()->GetLastKnownPublicPemFilePath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updating pem file widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_UpdatePemFileWidget()
{
   if (this->mpc_Ui->pc_PEMFilesWidget != NULL)
   {
      this->mpc_Ui->pc_PEMFilesWidget->UpdatePemFileWidget();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handling Add PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_HandleAddPemFile()
{
   if (this->mpc_Ui->pc_PEMFilesWidget != NULL)
   {
      this->mpc_Ui->pc_PEMFilesWidget->HandleAddPemFile();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete all PEM files from the list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_HandleClearAllPemFiles()
{
   if (this->mpc_Ui->pc_PEMFilesWidget != NULL)
   {
      this->mpc_Ui->pc_PEMFilesWidget->HandleClearAllPemFiles();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update Title with Counter value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_UpdateLabelTitleWithFileCounter()
{
   if (this->mpc_Ui->pc_PEMFilesWidget != NULL)
   {
      const int32_t s32_FileCount = mpc_Ui->pc_PEMFilesWidget->GetFilesCount();
      this->mpc_Ui->pc_LabelNewAuthenticationPEMFile->setText(
         static_cast<QString>(C_GtGetText::h_GetText("New authentication PEM files (%1)")).arg(QString::number(
                                                                                                  s32_FileCount)));

      if (s32_FileCount == 0)
      {
         this->mpc_Ui->pc_PushButtonClearall->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_PushButtonClearall->setEnabled(true);
      }
   }

   this->m_UpdatePemFileWidget();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reloads last saved password
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_ReloadLastSavedPassword()
{
   const QString c_Password = C_UsHandler::h_GetInstance()->GetLastKnownPemFilePassword();

   if (c_Password.isEmpty())
   {
      this->mpc_Ui->pc_LineEditPassword->setPlaceholderText("optional");
   }
   else
   {
      this->mpc_Ui->pc_LineEditPassword->setText(c_Password);
   }

   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditPassword, "NoRightBorder", true);
   this->mpc_Ui->pc_PubTogglePassword->setIcon(QIcon("://images/main_page_and_navi_bar/Icon_password_show.svg"));
   this->mpc_Ui->pc_PubTogglePassword->setMenu(NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for click on choose path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_PublicKeyPathClicked()
{
   QString c_Folder;
   const QString c_FilePath =
      C_UsHandler::h_GetInstance()->GetLastKnownPublicPemFilePath();

   if (!c_FilePath.isEmpty())
   {
      const QFileInfo c_FileInfo(c_FilePath);
      c_Folder = c_FileInfo.absolutePath();
   }
   else
   {
      c_Folder = C_Uti::h_GetExePath();
   }

   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("PEM file (*.pem);;Others (*.*)"));

   // Do not use QFileDialog::getOpenFileName because it does not support default suffix
   QFileDialog c_Dialog(this,  C_GtGetText::h_GetText("Select Output File"), c_Folder, c_FilterName);

   c_Dialog.setDefaultSuffix("pem");

   if (c_Dialog.exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      this->mpc_Ui->pc_LineEditPublicPEMFile->SetPath(c_Dialog.selectedFiles().at(0));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password click & hold
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_ShowPassword()
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Normal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for button show password release
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_HidePassword()
{
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   this->mpc_Ui->pc_LineEditPassword->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Validating Secure Update Configuration after create button click

   \return
   Status of secure update config validation

   \retval   C_NOACT     Path is empty
   \retval   C_RANGE     File suffix *.pem missing
   \retval   C_RD_WR     Invalid path (e.g. contains invalid characters)
   \retval   C_CONFIG    Incorrect key usage or no private keys included
   \retval   C_NO_ERR    no error/warning
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacSecurityCertificatePackageDialog::m_OnValidateSecureUpdateConfig()
{
   const int32_t s32_Result = this->m_CheckUpdatePath();

   if (s32_Result != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_ErrorPath(this, C_OgeWiCustomMessage::eERROR);
      c_ErrorPath.SetHeading(C_GtGetText::h_GetText("Create Security Certificate Package"));

      switch (s32_Result)
      {
      case C_NOACT:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "You need to enter a public PEM file to apply new Secure Update configuration."));
         break;
      case C_RANGE:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid file format of public PEM file for Secure Update. The format (*.pem) is required."));
         break;
      case C_RD_WR:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid path of public PEM file for Secure Update "
                                       "(e.g. containing invalid characters or file not found)."));
         break;
      case C_CONFIG:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Public PEM file for Secure Update has invalid key configuration."));
         c_ErrorPath.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                                C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."),  mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                 C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()));
         C_OscLoggingHandler::h_Flush(); // update log file
         break;
      default:
         // should never occur...
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Public PEM file for Secure Update not valid for unknown reason."));
         break;
      }
      c_ErrorPath.SetOkButtonText(C_GtGetText::h_GetText("OK"));
      c_ErrorPath.exec();
   }

   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Validating Secure authentication configuration after create button click

   \return
   Status of secure authentication config validation

   \retval   C_NOACT     Path is empty
   \retval   C_RANGE     File suffix *.pem missing
   \retval   C_RD_WR     Invalid path (e.g. contains invalid characters)
   \retval   C_CONFIG    Incorrect key usage or no private keys included
   \retval   C_NO_ERR    no error/warning
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacSecurityCertificatePackageDialog::m_OnValidateSecureAuthenticationConfig()
{
   QString c_PathDetails;
   const int32_t s32_Result = this->m_CheckAuthPemFiles(c_PathDetails);

   c_PathDetails = C_GtGetText::h_GetText("The following file lead to this error: \n") + c_PathDetails;

   if (s32_Result != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_ErrorPath(this, C_OgeWiCustomMessage::eERROR);
      c_ErrorPath.SetHeading(C_GtGetText::h_GetText("Create Security Certificate Package"));

      switch (s32_Result)
      {
      case C_NOACT:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "No new authentication PEM files selected."));
         break;
      case C_RANGE:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid file format of at least one file for Secure Authentication. "
                                       "The format (*.pem) is required."));
         c_ErrorPath.SetDetails(c_PathDetails);
         break;
      case C_RD_WR:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "Invalid path of of at least one file for Secure Authentication "
                                       "(e.g. containing invalid characters or file not found)."));
         c_ErrorPath.SetDetails(c_PathDetails);
         break;
      case C_CONFIG:
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "At least one file for Secure Authentication has invalid key configuration."));
         c_ErrorPath.SetDetails(c_PathDetails.replace("\n", "<br>") + "<br>" +
                                static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                                C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."),  mc_STYLESHEET_GUIDE_COLOR_LINK,
                                                 C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str()));
         C_OscLoggingHandler::h_Flush(); // update log file
         break;
      default:
         // should never occur...
         c_ErrorPath.SetDescription(C_GtGetText::h_GetText(
                                       "At least one file for Secure Authentication not valid for unknown reason."));
         c_ErrorPath.SetDetails(c_PathDetails);
         break;
      }
      c_ErrorPath.SetOkButtonText(C_GtGetText::h_GetText("OK"));
      c_ErrorPath.exec();
   }

   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validates the public PEM file for secure update

   \retval   C_NOACT     Path is empty
   \retval   C_RANGE     File suffix *.pem missing
   \retval   C_RD_WR     Invalid path (e.g. contains invalid characters)
   \retval   C_CONFIG    Incorrect key usage or no private keys included
   \retval   C_NO_ERR    no error/warning
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacSecurityCertificatePackageDialog::m_CheckUpdatePath()
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->isChecked())
   {
      const QFileInfo c_FileInfo(this->GetPublicKeyPath());

      // first some "easy checks"
      if (this->GetPublicKeyPath() == "")
      {
         s32_Return = C_NOACT;
      }
      else if (c_FileInfo.completeSuffix().toLower().compare("pem") != 0)
      {
         s32_Return = C_RANGE;
      }
      else if (C_OscUtils::h_CheckValidFilePath(this->GetPublicKeyPath().toStdString().c_str()) == false)
      {
         s32_Return = C_RD_WR;
      }
      else if (c_FileInfo.exists() == false)
      {
         s32_Return = C_RD_WR;
      }
      else
      {
         //nothing to do here
      }

      // now check the selected file a bit more detailed
      if ((s32_Return == C_NO_ERR) && (c_FileInfo.exists() == true))
      {
         C_OscSecurityPemSecUpdate c_Pem;
         std::string c_ErrorMessage;

         // C_OscSecurityPemSecUpdate::LoadFromFile checks for correct key usage
         const int32_t s32_Result = c_Pem.LoadFromFile(this->GetPublicKeyPath().toStdString(), c_ErrorMessage);

         if (s32_Result != C_NO_ERR)
         {
            osc_write_log_error("Load PEM file",
                                c_ErrorMessage + " (Path: " + this->GetPublicKeyPath().toStdString() + ")");
            s32_Return = C_CONFIG;
         }
         else
         {
            const C_OscSecurityPemKeyInfo & rc_KeyInfo = c_Pem.GetKeyInfo();

            // check that public keys are available
            if (rc_KeyInfo.GetX509CertificateData().empty())
            {
               osc_write_log_error("PEM file keys",
                                   "No public keys found in PEM file. (Path: " +
                                   this->GetPublicKeyPath().toStdString() + ")");
               s32_Return = C_CONFIG;
            }

            // check that no private key is contained
            if (rc_KeyInfo.GetPrivateKey().empty() == false)
            {
               osc_write_log_error("PEM file keys",
                                   "File contains a private key, but should be a public PEM file. (Path: " +
                                   this->GetPublicKeyPath().toStdString() + ")");
               s32_Return = C_CONFIG;
            }
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Validates the secure authentication PEM files

   \param[out]  orc_ErrorPath    PEM file path that led to error path (only first fail gets reported)

   \retval   C_NOACT     PEM files list is empty after enabling "AddNewPemFiles" checkbox
   \retval   C_RANGE     File suffix *.pem missing
   \retval   C_RD_WR     Invalid path (e.g. contains invalid characters)
   \retval   C_CONFIG    Incorrect key usage or no private keys included
   \retval   C_NO_ERR    no error/warning
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacSecurityCertificatePackageDialog::m_CheckAuthPemFiles(QString & orc_ErrorPath) const
{
   int32_t s32_Return = C_NO_ERR;

   if (this->mpc_Ui->pc_ChxBoxAddPEMFiles->isChecked())
   {
      const QStringList & rc_Files = this->mpc_Ui->pc_PEMFilesWidget->GetPemFilePaths();
      if (rc_Files.isEmpty())
      {
         s32_Return = C_NOACT;
      }

      // check all files (stop on first failure, we do not need to parse all if one is invalid)
      for (QStringList::const_iterator c_It = rc_Files.begin(); (c_It < rc_Files.end()) && (s32_Return == C_NO_ERR);
           ++c_It)
      {
         const QString & rc_CurFile = *c_It;
         const QFileInfo c_FileInfo(rc_CurFile);

         // first some "easy checks"
         if (c_FileInfo.completeSuffix().toLower().compare("pem") != 0)
         {
            s32_Return = C_RANGE;
         }
         else if (C_OscUtils::h_CheckValidFilePath(rc_CurFile.toStdString().c_str()) == false)
         {
            s32_Return = C_RD_WR;
         }
         else if (c_FileInfo.exists() == false)
         {
            s32_Return = C_RD_WR;
         }
         else
         {
            //nothing to do here
         }

         // now check the file a bit more detailed
         if ((s32_Return == C_NO_ERR) && (c_FileInfo.exists() == true))
         {
            C_OscSecurityPem c_Pem;
            std::string c_ErrorMessage;

            const int32_t s32_Result = c_Pem.LoadFromFile(rc_CurFile.toStdString(), c_ErrorMessage);

            if (s32_Result != C_NO_ERR)
            {
               osc_write_log_error("Load PEM file",
                                   c_ErrorMessage + " (Path: " + rc_CurFile.toStdString() + ")");
               s32_Return = C_CONFIG;
            }
            else
            {
               const C_OscSecurityPemKeyInfo & rc_KeyInfo = c_Pem.GetKeyInfo();

               // check that any keys are available
               if (rc_KeyInfo.AreKeysAvailable(c_ErrorMessage, false) == false)
               {
                  osc_write_log_error("PEM file keys",
                                      c_ErrorMessage + " (Path: " + rc_CurFile.toStdString() + ")");
                  s32_Return = C_CONFIG;
               }

               // check that at least one private key is contained
               if (rc_KeyInfo.GetPrivateKey().empty() == true)
               {
                  osc_write_log_error("PEM file keys",
                                      "No private key available. (Path: " + rc_CurFile.toStdString() + ")");
                  s32_Return = C_CONFIG;
               }

               // check for correct key usage
               const C_OscSecurityPemKeyInfo::C_CertificateKeyUsageInformation & rc_Usage =
                  rc_KeyInfo.GetKeyUsageInformation();
               if (((rc_Usage.q_KeyUsageDefined == true) && (rc_Usage.q_KeyUsageDigitalSignature == false)) ||
                   ((rc_Usage.q_ExtendedKeyUsageDefined == true) && (rc_Usage.q_ExtendedKeyUsageClientAuth == false)))
               {
                  // error: key usage is defined, but not the right one
                  osc_write_log_error("PEM file keys",
                                      "Certificate: Key usage flags are defined in PEM file, but not correct. "
                                      "Expected flags \"digitalSignature\" and \"id-kp-clientAuth\" to be set. (Path: " +
                                      rc_CurFile.toStdString() + ")");
                  s32_Return = C_CONFIG;
               }
               else if ((rc_Usage.q_KeyUsageDefined == false) || (rc_Usage.q_KeyUsageDigitalSignature == false) ||
                        (rc_Usage.q_ExtendedKeyUsageDefined == false) ||
                        (rc_Usage.q_ExtendedKeyUsageClientAuth == false))
               {
                  // no error: we need to stay compatible and allow to load "old" PEM files that had no key usage
                  osc_write_log_warning("PEM file keys",
                                        "Certificate: Key usage flags not as expected in PEM file used for secure "
                                        "authentication configuration. Expected flags \"digitalSignature\" and "
                                        "\"id-kp-clientAuth\" to be set. (Path: " +
                                        rc_CurFile.toStdString() + ")");
               }
               else
               {
                  // key usage correct or not defined
               }
            }
         }

         if (s32_Return != C_NO_ERR)
         {
            orc_ErrorPath = rc_CurFile;
         }
      }
   }

   return s32_Return;
}
