//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new project import/creation (implementation)

   Widget for new project import/creation

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDir>
#include <QFileDialog>

#include "C_Uti.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_ImpUtil.h"
#include "C_PuiUtil.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_OSCZipFile.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeDbAddNewProject.h"
#include "ui_C_SdNdeDbAddNewProject.h"
#include "C_OSCTargetSupportPackageFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SdNdeDbAddNewProject::mhc_StartTD = "<td style=\"padding: 0 9px 0 0;\">";
const QString C_SdNdeDbAddNewProject::mhc_ContinueTD = "<td style=\"padding: 0 9px 0 9px;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_NodeIndex        Node index
   \param[in,out] orc_Parent            Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbAddNewProject::C_SdNdeDbAddNewProject(const uint32 ou32_NodeIndex,
                                               stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbAddNewProject),
   ms32_TSPReadResult(-1),
   mu32_NodeIndex(ou32_NodeIndex),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();
   m_Init(ou32_NodeIndex);
   this->mpc_Ui->pc_TextEditTSPDescription->setReadOnly(true);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // Remove "..." string
   this->mpc_Ui->pc_PushButtonCreateIn->setText("");
   this->mpc_Ui->pc_PushButtonTSP->setText("");

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDbAddNewProject::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdNdeDbAddNewProject::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonTSP, &QPushButton::clicked, this,
           &C_SdNdeDbAddNewProject::m_TSPButtonClicked);
   connect(this->mpc_Ui->pc_PushButtonCreateIn, &QPushButton::clicked, this,
           &C_SdNdeDbAddNewProject::m_CreateInButtonClicked);
   connect(this->mpc_Ui->pc_LineEditTSP, &C_OgeLeFilePath::editingFinished,
           this, &C_SdNdeDbAddNewProject::m_OnLoadTSP);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbAddNewProject::~C_SdNdeDbAddNewProject(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Add Data Blocks"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Extract from openSYDE Target Support Package"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_LabelTSP->setText(C_GtGetText::h_GetText("openSYDE Target Support Package"));
   this->mpc_Ui->pc_LabelCreateIn->setText(C_GtGetText::h_GetText("Create In Directory"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Extract"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   //tooltips
   this->mpc_Ui->pc_LabelTSP->SetToolTipInformation(
      C_GtGetText::h_GetText("openSYDE Target Support Package"),
      C_GtGetText::h_GetText("openSYDE Target Support Package provided by target deployment"));

   this->mpc_Ui->pc_LabelCreateIn->SetToolTipInformation(
      C_GtGetText::h_GetText("Create In Directory"),
      C_GtGetText::h_GetText("Location where the openSYDE Target Support Package should be extracted at."));

   this->mpc_Ui->pc_PushButtonTSP->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for openSYDE Target Support Package."));

   this->mpc_Ui->pc_PushButtonCreateIn->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for location where openSYDE Target Support Package should be extracted at."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current TSP path

   \return
   Current TSP path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbAddNewProject::GetTSPPath(void) const
{
   return C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditTSP->GetPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full TSP path.

   For appropriate QLineEdit the minimized path is shown.
   Also loads the TSP if possible

   \param[in] orc_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::SetTSPPath(const QString & orc_New)
{
   this->mpc_Ui->pc_LineEditTSP->SetPath(orc_New, C_PuiProject::h_GetInstance()->GetFolderPath());
   this->m_OnLoadTSP();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of applications in current TSP

   \return
   Number of applications in current TSP
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNdeDbAddNewProject::GetTSPApplicationCount(void) const
{
   return this->mc_Package.c_Applications.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add selected project to application

   \param[in]     ou32_TSPIndex   Application index in TSP
   \param[in,out] orc_Application Application to apply new properties to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::AddSelectedProject(const uint32 ou32_TSPIndex,
                                                C_OSCNodeApplication & orc_Application) const
{
   if (ou32_TSPIndex < this->mc_Package.c_Applications.size())
   {
      QString c_CodeGeneratorPath;
      const C_OSCTSPApplication & rc_SelectedApp = this->mc_Package.c_Applications[ou32_TSPIndex];
      orc_Application.c_Name = rc_SelectedApp.c_Name;
      orc_Application.c_Comment = rc_SelectedApp.c_Comment;
      orc_Application.c_GeneratePath = rc_SelectedApp.c_GeneratePath;
      orc_Application.c_ResultPath = rc_SelectedApp.c_ResultPath;
      orc_Application.u8_ProcessId = rc_SelectedApp.u8_ProcessId;
      // do not concatenate project path with syde-file path because we support relative paths here
      orc_Application.c_ProjectPath =
         C_Uti::h_ConcatPathIfNecessary(this->mpc_Ui->pc_LineEditCreateIn->GetPath(),
                                        rc_SelectedApp.c_ProjectFolder.c_str()).toStdString().c_str();
      orc_Application.c_IDECall = rc_SelectedApp.c_IdeCall;
      orc_Application.u16_GenCodeVersion = rc_SelectedApp.u16_GenCodeVersion;

      //Handle default code generator flag
      if (rc_SelectedApp.q_IsStandardSydeCoderC == true)
      {
         c_CodeGeneratorPath = C_ImpUtil::h_GetSydeCoderCPath();
      }
      else
      {
         c_CodeGeneratorPath = rc_SelectedApp.c_CodeGeneratorPath.c_str();
      }
      orc_Application.c_CodeGeneratorPath = c_CodeGeneratorPath.toStdString().c_str();

      // Handle is-programmable flag
      if (rc_SelectedApp.q_IsProgrammable == true)
      {
         orc_Application.e_Type = C_OSCNodeApplication::E_Type::ePROGRAMMABLE_APPLICATION;
      }
      else
      {
         orc_Application.e_Type = C_OSCNodeApplication::E_Type::eBINARY;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle code gen config
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::HandleCodeGenerationConfig(void) const
{
   C_OSCNodeOpenSYDEServerSettings c_Settings;

   c_Settings.s16_DPDDataBlockIndex = static_cast<uint16>(this->mc_Package.u8_ApplicationIndex);
   c_Settings.u16_MaxMessageBufferTx = this->mc_Package.u16_MaxMessageBufferTx;
   c_Settings.u16_MaxRoutingMessageBufferRx = this->mc_Package.u16_MaxRoutingMessageBufferRx;
   c_Settings.u8_MaxParallelTransmissions = this->mc_Package.u8_MaxParallelTransmissions;

   C_PuiSdHandler::h_GetInstance()->SetNodeOpenSYDEServerSettings(this->mu32_NodeIndex, c_Settings);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         m_OkClicked();
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
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_OkClicked(void)
{
   bool q_Continue = false;

   if (this->ms32_TSPReadResult != C_NO_ERR)
   {
      QString c_Details;
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
      switch (this->ms32_TSPReadResult)
      {
      case C_RANGE:
         c_Details = C_GtGetText::h_GetText("Specified openSYDE Target Support Package does not exist.");
         break;
      case C_NOACT:
         c_Details =
            C_GtGetText::h_GetText("Specified file is present but its structure is invalid (e.g. invalid XML file).");
         break;
      case C_CONFIG:
         c_Details = C_GtGetText::h_GetText("In specified file is a XML node or attribute missing.");
         break;
      default:
         c_Details = C_GtGetText::h_GetText("Unknown error occurred.");
         break;
      }
      c_Message.SetDescription(QString(C_GtGetText::h_GetText(
                                          "Could not load specified openSYDE Target Support Package.")));
      c_Message.SetDetails(c_Details);
      c_Message.Execute();
   }
   else
   {
      QDir c_CreateInFolder(
         C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->GetPath()));
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

      if (this->mpc_Ui->pc_LineEditTSP->GetPath() == "")
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
         c_Message.SetDescription(QString(C_GtGetText::h_GetText(
                                             "No openSYDE Target Support Package specified")));
         c_Message.Execute();
      }
      else if ((pc_Node != NULL) && (this->mc_Package.c_DeviceName != pc_Node->c_DeviceType))
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
         c_Message.SetDescription(
            QString(C_GtGetText::h_GetText("The openSYDE Target Support Package device \"%1\" does not match the "
                                           "device \"%2\" of this node")).
            arg(this->mc_Package.c_DeviceName.c_str()).
            arg(pc_Node->c_DeviceType.c_str()));
         c_Message.Execute();
      }
      else if (this->mpc_Ui->pc_LineEditCreateIn->GetPath().isEmpty() == true)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
         c_Message.SetDescription(QString(C_GtGetText::h_GetText("Define a directory for the project here.")));
         c_Message.Execute();
      }
      else if ((c_CreateInFolder.exists() == true) &&
               (c_CreateInFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() != 0))
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
         c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
         c_Message.SetNOButtonText(C_GtGetText::h_GetText("Continue without Clearing"));
         c_Message.SetOKButtonText(C_GtGetText::h_GetText("Clear and Continue"));
         c_Message.SetDescription(
            QString(C_GtGetText::h_GetText("Directory \"%1\" is not empty. \n\nShould this directory be cleared? "
                                           "Attention: Your data will be lost!")).arg(c_CreateInFolder.absolutePath()));
         c_Message.SetCustomMinHeight(280);
         c_Message.SetCustomMinWidth(800);

         switch (c_Message.Execute())
         {
         case C_OgeWiCustomMessage::eYES:
            QApplication::setOverrideCursor(Qt::WaitCursor);
            if (c_CreateInFolder.removeRecursively() == true)
            {
               q_Continue = true;
               QApplication::restoreOverrideCursor();
            }
            else
            {
               C_OgeWiCustomMessage c_Message2(this, C_OgeWiCustomMessage::eERROR);
               c_Message2.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
               c_Message2.SetDescription(
                  QString(C_GtGetText::h_GetText("Could not clear directory \"%1\".")).
                  arg(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->GetPath())));
               QApplication::restoreOverrideCursor();
               c_Message2.Execute();
            }
            break;
         case C_OgeWiCustomMessage::eNO:
            q_Continue = true;
            break;
         default:
            //Abort
            break;
         }
      }
      else
      {
         q_Continue = true;
      }
   }
   if (q_Continue == true)
   {
      const QFileInfo c_TSPFileInfo(this->GetTSPPath()); // file path -> use absoluteDir() to get directory of file
      stw_scl::C_SCLString c_ErrorText;
      const QString c_Path =
         QDir::cleanPath(c_TSPFileInfo.absoluteDir().absoluteFilePath(this->mc_Package.c_TemplatePath.c_str()));
      QApplication::setOverrideCursor(Qt::WaitCursor);
      if (C_OSCZipFile::h_UnpackZipFile(c_Path.toStdString().c_str(),
                                        C_PuiUtil::h_GetAbsolutePathFromProject(
                                           this->mpc_Ui->pc_LineEditCreateIn->GetPath()).toStdString().c_str(),
                                        &c_ErrorText) == C_NO_ERR)
      {
         this->mrc_ParentDialog.accept();
         QApplication::restoreOverrideCursor();
      }
      else
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add Data Blocks"));
         c_Message.SetDescription(QString(C_GtGetText::h_GetText("Could not extract openSYDE Target Support Package "
                                                                 "from file \"%1\" to directory \"%2\".")).arg(c_Path).
                                  arg(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->
                                                                              GetPath())));
         c_Message.SetCustomMinWidth(800);
         c_Message.SetDetails(c_ErrorText.c_str());
         QApplication::restoreOverrideCursor();
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle TSP path button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_TSPButtonClicked(void)
{
   QString c_FolderName; // for default folder
   QString c_FilePath = "";
   const QString c_Suffix = "syde_tsp";
   const QFileInfo c_File(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditTSP->GetPath()));
   const QString c_FilterName = QString(C_GtGetText::h_GetText("openSYDE Target Support Package file")) +
                                " (*." + c_Suffix + ")";

   if (c_File.exists() == true)
   {
      c_FolderName = c_File.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   // do not use QFileDialog::getOpenFileName because it does not support default suffix
   QFileDialog c_Dialog(this, C_GtGetText::h_GetText("Select openSYDE Target Support Package File"),
                        c_FolderName, c_FilterName);
   c_Dialog.setDefaultSuffix(c_Suffix);

   if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
   {
      c_FilePath = c_Dialog.selectedFiles().at(0); // multi-selection is not possible
   }

   if (c_FilePath != "")
   {
      this->SetTSPPath(c_FilePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle create in path button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_CreateInButtonClicked(void)
{
   QString c_FolderName; // for default folder

   const QDir c_Folder(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->GetPath()));

   if (c_Folder.exists() == true)
   {
      c_FolderName = c_Folder.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   QString c_Path = QFileDialog::getExistingDirectory(this,
                                                      C_GtGetText::h_GetText("Select Directory for Target Project"),
                                                      c_FolderName, QFileDialog::ShowDirsOnly);

   if (c_Path != "")
   {
      // check if relative path is possible and appreciated
      c_Path = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_Path, C_PuiProject::h_GetInstance()->GetFolderPath());

      if (c_Path != "")
      {
         this->mpc_Ui->pc_LineEditCreateIn->SetPath(c_Path, C_PuiProject::h_GetInstance()->GetFolderPath());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle loading TSP
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_OnLoadTSP(void)
{
   this->ms32_TSPReadResult = C_OSCTargetSupportPackageFiler::h_Load(
      this->mc_Package,
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditTSP->GetPath()).toStdString().c_str());

   if (this->ms32_TSPReadResult == C_NO_ERR)
   {
      QString c_Text = "<html><body>";
      m_AddTopSection(c_Text);
      m_AddTemplateSection(c_Text);
      c_Text += "</body></html>";
      this->mpc_Ui->pc_TextEditTSPDescription->setHtml(c_Text);
   }
   else
   {
      this->mpc_Ui->pc_TextEditTSPDescription->setPlainText(C_GtGetText::h_GetText(
                                                               "<openSYDE Target Support Package description>"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle top section of TSP description

   \param[in,out] orc_Content Text to append to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_AddTopSection(QString & orc_Content) const
{
   orc_Content += "<h4>" + QString(C_GtGetText::h_GetText("openSYDE Target Support Package")) + "</h4>";
   orc_Content += "<table>";
   orc_Content += "<tr>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("Description:");
   orc_Content += "</td>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_ContinueTD;
   orc_Content += this->mc_Package.c_Comment.c_str();
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "<tr>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_StartTD;
   orc_Content += C_GtGetText::h_GetText("Number of applications:");
   orc_Content += "</td>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_ContinueTD;
   orc_Content += QString::number(this->mc_Package.c_Applications.size());
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "</table>";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle template section of TSP description

   \param[in,out] orc_Content Text to append to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_AddTemplateSection(QString & orc_Content) const
{
   for (uint32 u32_ItTemplate = 0UL; u32_ItTemplate < this->mc_Package.c_Applications.size(); ++u32_ItTemplate)
   {
      const C_OSCTSPApplication & rc_Template = this->mc_Package.c_Applications[u32_ItTemplate];
      orc_Content += "<h4>" + QString(C_GtGetText::h_GetText("Application %1")).arg(u32_ItTemplate + 1UL) + "</h4>";
      orc_Content += "<table>";
      orc_Content += "<tr>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Name:");
      orc_Content += "</td>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_ContinueTD;
      orc_Content += rc_Template.c_Name.c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_StartTD;
      orc_Content += C_GtGetText::h_GetText("Comment:");
      orc_Content += "</td>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_ContinueTD;
      orc_Content += rc_Template.c_Comment.c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "</table>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init based on specified application

   \param[in] ou32_NodeIndex        Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_Init(const uint32 ou32_NodeIndex) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Use default
      const QString c_NodePath = C_OSCUtils::h_NiceifyStringForFileName(pc_Node->c_Properties.c_Name).c_str();
      this->mpc_Ui->pc_LineEditCreateIn->SetPath(c_NodePath, C_PuiProject::h_GetInstance()->GetFolderPath());
   }
}
