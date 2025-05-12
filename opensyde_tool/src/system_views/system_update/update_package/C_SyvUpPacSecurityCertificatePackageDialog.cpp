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
   this->mpc_ScrollLayout = new QVBoxLayout(this->mpc_Ui->pc_ScrollAreaWidget);
   this->mpc_ScrollLayout->setContentsMargins(0, 0, 0, 0);
   this->mpc_ListWidget = new C_SyvUpPacPemFilesListWidget(this->mpc_Ui->pc_ScrollAreaWidget);
   this->mpc_ScrollLayout->addWidget(this->mpc_ListWidget);

   InitStaticNames();
   this->m_InitPemFilesSection();
   this->m_InitPasswordLineEdit();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mpc_Ui->pc_PushButtonAdd->SetCustomIcons("://images/IconAddEnabled.svg", "",
                                                  "", "://images/IconAddDisabled.svg");
   this->mpc_Ui->pc_PushButtonClearall->SetCustomIcons("://images/system_views/IconClearAllEnabled.svg", "",
                                                       "", "://images/system_views/IconClearAllDisabled.svg");

   this->m_OnAddPemFileStateChanged(static_cast<int32_t>(this->mpc_Ui->pc_ChxBoxAddPEMFiles->isChecked()));

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonAdd, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_HandleAddPemFile);
   connect(this->mpc_Ui->pc_PushButtonClearall, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_HandleClearAllPemFiles);
   connect(this->mpc_ListWidget, &C_SyvUpPacPemFilesListWidget::SigUpdateFileCounter, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_UpdateLabelTitleWithFileCounter);
   connect(this->mpc_Ui->pc_PubPrivateKeyPath, &QPushButton::clicked, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_PrivateKeyPathClicked);
   connect(this->mpc_Ui->pc_PubTogglePassword, &QPushButton::pressed, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_ShowPassword);
   connect(this->mpc_Ui->pc_PubTogglePassword, &QPushButton::released, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_HidePassword);
   connect(this->mpc_Ui->pc_ChxBoxAddPEMFiles, &QCheckBox::stateChanged, this,
           &C_SyvUpPacSecurityCertificatePackageDialog::m_OnAddPemFileStateChanged);
   this->setAcceptDrops(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacSecurityCertificatePackageDialog::~C_SyvUpPacSecurityCertificatePackageDialog(void)
{
   delete this->mpc_ScrollLayout;
   delete this->mpc_ListWidget;
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::InitStaticNames(void) const
{
   //Secure Authentication
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Security Certificate Package"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Create"));
   this->mpc_Ui->pc_LabelSecureAuthentication->setText(C_GtGetText::h_GetText("Secure Authentication"));
   this->mpc_Ui->pc_ChxBoxAddPEMFiles->setText(C_GtGetText::h_GetText("Add new authentication PEM files"));
   this->mpc_Ui->pc_LabelNewAuthenticationPEMFile->setText(
      static_cast<QString>(C_GtGetText::h_GetText("New authentication PEM files (%1)")).arg(QString::number(this
                                                                                                            ->
                                                                                                            mpc_ListWidget
                                                                                                            ->
                                                                                                            count())));

   //Secure Update
   this->mpc_Ui->pc_LabelSecureUpdate->setText(C_GtGetText::h_GetText("Secure Update"));
   this->mpc_Ui->pc_ChxBoxApplyUpdateConfiguration->setText(C_GtGetText::h_GetText(
                                                               "Apply new secure update configuration"));
   this->mpc_Ui->pc_LabelNewConfiguration->setText(C_GtGetText::h_GetText("New Configuration"));
   this->mpc_Ui->pc_LabelPublicPEMFile->setText(C_GtGetText::h_GetText("Public PEM File"));
   this->mpc_Ui->pc_LabelPassword->setText(C_GtGetText::h_GetText("Encryption Password"));

   //Tooltips
   this->mpc_Ui->pc_PushButtonClearall->SetToolTipInformation(C_GtGetText::h_GetText("Clear all"),
                                                              C_GtGetText::h_GetText(
                                                                 "clear all .pem files."));
   this->mpc_Ui->pc_PushButtonAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add"),
                                                         C_GtGetText::h_GetText(
                                                            "Add new .pem file."));
   this->mpc_Ui->pc_LabelPublicPEMFile->SetToolTipInformation(C_GtGetText::h_GetText("Signature Private Key"),
                                                              C_GtGetText::h_GetText(
                                                                 "Full path to private key file (*.pem)."));
   this->mpc_Ui->pc_LabelPassword->SetToolTipInformation(C_GtGetText::h_GetText("Encryption Password (optional)"),
                                                         C_GtGetText::h_GetText(
                                                            "Flashware will be encrypted if password is given."));
   this->mpc_Ui->pc_PubPrivateKeyPath->SetToolTipInformation(C_GtGetText::h_GetText("Browse"),
                                                             C_GtGetText::h_GetText(
                                                                "Browse for path to private key(pem) file."));
   this->mpc_Ui->pc_PubTogglePassword->SetToolTipInformation(C_GtGetText::h_GetText("Show/Hide Password"),
                                                             C_GtGetText::h_GetText(
                                                                "Shows/hides password when pressed down."));

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Create"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
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
/*! \brief  Overridden dragEnterEvent slot
 *
 *  Passes opc_Event to mpc_ListWidget DragEnterEvent

   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   this->mpc_ListWidget->DragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dragMoveEvent slot
 *
 *  Passes opc_Event to mpc_ListWidget DragMoveEvent

   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   this->mpc_ListWidget->DragMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dropEvent slot
 *
 *  Passes opc_Event to mpc_ListWidget DropEvent

   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::dropEvent(QDropEvent * const opc_Event)
{
   this->mpc_ListWidget->DropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_OkClicked(void)
{
   this->mpc_ListWidget->SetLastKnownPemFilePaths();
   this->m_OnValidateSecureUpdateConfig();
   this->mrc_ParentDialog.accept();
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
/*! \brief  Init PEM files Section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_InitPemFilesSection()
{
   if (this->mpc_ListWidget->count() <= 0)
   {
      this->mpc_Ui->pc_GbxNoPEMFiles->setVisible(true);
      this->mpc_Ui->pc_ScrollAreaPEMFiles->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GbxNoPEMFiles->setVisible(false);
      this->mpc_Ui->pc_ScrollAreaPEMFiles->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handling Add PEM file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_HandleAddPemFile()
{
   this->mpc_ListWidget->AddFileAction(this->mpc_ListWidget->count());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete all PEM files from the list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_HandleClearAllPemFiles()
{
   this->mpc_ListWidget->DeleteAllItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update Title with Counter value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_UpdateLabelTitleWithFileCounter()
{
   if (this->mpc_ListWidget != NULL)
   {
      this->mpc_Ui->pc_LabelNewAuthenticationPEMFile->setText(
         static_cast<QString>(C_GtGetText::h_GetText("New authentication PEM files (%1)")).arg(QString::number(
                                                                                                  this
                                                                                                  ->
                                                                                                  mpc_ListWidget
                                                                                                  ->
                                                                                                  count())));
   }

   this->m_InitPemFilesSection();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initializes password line edit incl icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_InitPasswordLineEdit()
{
   this->mpc_Ui->pc_LineEditPassword->setPlaceholderText("optional");
   this->mpc_Ui->pc_LineEditPassword->setEchoMode(QLineEdit::Password);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditPassword, "NoRightBorder", true);
   this->mpc_Ui->pc_PubTogglePassword->setIcon(QIcon("://images/main_page_and_navi_bar/Icon_password_show.svg"));
   this->mpc_Ui->pc_PubTogglePassword->setMenu(NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for click on choose path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_PrivateKeyPathClicked()
{
   const QString c_Tmp =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditPublicPEMFile->GetPath());
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
/*! \brief  Sets the path to the private key
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_SetPrivateKeyPath(const QString & orc_Path) const
{
   this->mpc_Ui->pc_LineEditPublicPEMFile->SetPath(orc_Path);
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
/*! \brief  Slot on Add new PEM file checkbox state changed

   \param[in]  os32_State   State
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_OnAddPemFileStateChanged(const int32_t os32_State)
{
   if (static_cast<Qt::CheckState>(os32_State) == Qt::Checked)
   {
      this->mpc_Ui->pc_PushButtonAdd->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonAdd->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Validating Secure Update Configuration after create button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacSecurityCertificatePackageDialog::m_OnValidateSecureUpdateConfig()
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
/*! \brief  Validates the private key path

   \retval   C_NOACT   Path is empty
   \retval   C_RANGE   File suffix *.pem missing
   \retval   C_RDWR    Invalid path (e.g. contains invalid characters)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacSecurityCertificatePackageDialog::m_CheckPath()
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
/*! \brief  Getter of full private key file path

   \return QString .... private file path

*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvUpPacSecurityCertificatePackageDialog::GetPrivateKeyPath() const
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
   QStringList c_PemFiles = this->mpc_ListWidget->GetPemFilePaths();

   for (const QString & rc_FilePath : c_PemFiles)
   {
      const stw::scl::C_SclString c_File(rc_FilePath.toStdString());
      c_PemFilePaths.push_back(c_File);
   }

   return c_PemFilePaths;
}
