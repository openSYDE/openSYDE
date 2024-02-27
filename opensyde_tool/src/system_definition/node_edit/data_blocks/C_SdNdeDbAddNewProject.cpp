//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new project import/creation (implementation)

   Widget for new project import/creation

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QDir>
#include <QFileDialog>

#include "C_Uti.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_ImpUtil.hpp"
#include "C_PuiUtil.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_OscZipFile.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeDbAddNewProject.hpp"
#include "ui_C_SdNdeDbAddNewProject.h"
#include "C_OscTargetSupportPackageFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SdNdeDbAddNewProject::mhc_START_TD = "<td style=\"padding: 0 9px 0 0;\">";
const QString C_SdNdeDbAddNewProject::mhc_CONTINUE_TD = "<td style=\"padding: 0 9px 0 9px;\">";
const QString C_SdNdeDbAddNewProject::mhc_SUFFIX = "syde_tsp";

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
C_SdNdeDbAddNewProject::C_SdNdeDbAddNewProject(const uint32_t ou32_NodeIndex,
                                               stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbAddNewProject),
   ms32_TspReadResult(-1),
   mu32_NodeIndex(ou32_NodeIndex),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_LineEditTSP->SetDragAndDropActiveForFile(mhc_SUFFIX);
   this->mpc_Ui->pc_LineEditCreateIn->SetDragAndDropActiveForFolder(true);

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
           &C_SdNdeDbAddNewProject::m_TspButtonClicked);
   connect(this->mpc_Ui->pc_PushButtonCreateIn, &QPushButton::clicked, this,
           &C_SdNdeDbAddNewProject::m_CreateInButtonClicked);
   connect(this->mpc_Ui->pc_LineEditTSP, &C_OgeLeFilePath::editingFinished,
           this, &C_SdNdeDbAddNewProject::m_OnLoadTsp);
   connect(this->mpc_Ui->pc_LineEditCreateIn, &C_OgeLeFilePath::SigPathDropped,
           this, &C_SdNdeDbAddNewProject::m_OnDroppedCreatinPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbAddNewProject::~C_SdNdeDbAddNewProject(void) noexcept
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Import openSYDE Target Support Package"));
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
      C_GtGetText::h_GetText(
         "Location where the openSYDE Target Support Package content (e.g.: Target project, Flashware, ...) should be extracted at."));

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
QString C_SdNdeDbAddNewProject::GetTspPath(void) const
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
void C_SdNdeDbAddNewProject::SetTspPath(const QString & orc_New)
{
   this->mpc_Ui->pc_LineEditTSP->SetPath(orc_New, C_PuiProject::h_GetInstance()->GetFolderPath());
   this->m_OnLoadTsp();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of applications in current TSP

   \return
   Number of applications in current TSP
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDbAddNewProject::GetTspApplicationCount(void) const
{
   return this->mc_Package.c_Applications.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add selected project to application

   \param[in]     ou32_TspIndex     Application index in TSP
   \param[in,out] orc_Application   Application to apply new properties to
   \param[out]    orc_Warnings      Warnings that occured
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::AddSelectedProject(const uint32_t ou32_TspIndex, C_OscNodeApplication & orc_Application,
                                                QString & orc_Warnings) const
{
   if (ou32_TspIndex < this->mc_Package.c_Applications.size())
   {
      QString c_CodeGeneratorPath;
      const C_OscTspApplication & rc_SelectedApp = this->mc_Package.c_Applications[ou32_TspIndex];
      const QString c_ProjectPath =
         C_Uti::h_ConcatPathIfNecessary(this->mpc_Ui->pc_LineEditCreateIn->GetPath(),
                                        rc_SelectedApp.c_ProjectFolder.c_str());
      orc_Application.c_Name = rc_SelectedApp.c_Name;
      orc_Application.c_Comment = rc_SelectedApp.c_Comment;
      orc_Application.c_GeneratePath = rc_SelectedApp.c_GeneratePath;
      orc_Application.c_ResultPaths = rc_SelectedApp.c_ResultPaths;
      orc_Application.u8_ProcessId = rc_SelectedApp.u8_ProcessId;
      // do not concatenate project path with syde-file path because we support relative paths here
      orc_Application.c_ProjectPath = c_ProjectPath.toStdString().c_str();
      orc_Application.c_IdeCall = rc_SelectedApp.c_IdeCall;
      orc_Application.u16_GenCodeVersion = rc_SelectedApp.u16_GenCodeVersion;

      // for psi generation the generation path can not be edited and therefore should be empty;
      // its functionality is taken over by the project path.
      if (rc_SelectedApp.q_GeneratesPsiFiles == true)
      {
         orc_Application.c_GeneratePath = "";
         orc_Application.c_ProjectPath =
            C_Uti::h_ConcatPathIfNecessary(c_ProjectPath, rc_SelectedApp.c_GeneratePath.c_str()).toStdString().c_str();
      }

      //do not allow to save higher value as highest known code structure version
      if (orc_Application.u16_GenCodeVersion > mu16_HIGHEST_KNOWN_CODE_STRUCTURE_VERSION)
      {
         orc_Application.u16_GenCodeVersion = mu16_HIGHEST_KNOWN_CODE_STRUCTURE_VERSION;
         orc_Warnings.append(static_cast<QString>(C_GtGetText::h_GetText(
                                                     "Code structure version of application %1 is unknown and "
                                                     "therefore set to most recent version %2.\n")).
                             arg(orc_Application.c_Name.c_str()).arg(mu16_HIGHEST_KNOWN_CODE_STRUCTURE_VERSION));
      }

      //Handle default file generator flag
      if (rc_SelectedApp.q_IsStandardSydeCoderCe == true)
      {
         c_CodeGeneratorPath = C_ImpUtil::h_GetSydeCoderCePath();
      }
      else
      {
         c_CodeGeneratorPath = rc_SelectedApp.c_CodeGeneratorPath.c_str();
      }
      orc_Application.c_CodeGeneratorPath = c_CodeGeneratorPath.toStdString().c_str();

      // Handle file generation flags
      if (rc_SelectedApp.q_IsProgrammable == true)
      {
         orc_Application.e_Type = C_OscNodeApplication::ePROGRAMMABLE_APPLICATION;
      }
      else
      {
         if (rc_SelectedApp.q_GeneratesPsiFiles == true)
         {
            orc_Application.e_Type = C_OscNodeApplication::ePARAMETER_SET_HALC;
         }
         else
         {
            orc_Application.e_Type = C_OscNodeApplication::eBINARY;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle file generation configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::HandleCodeGenerationConfig(void) const
{
   C_OscNodeOpenSydeServerSettings c_DdpSettings;
   const C_OscNodeCodeExportSettings c_GeneralSettings = this->mc_Package.c_CodeExportSettings;

   c_DdpSettings.s16_DpdDataBlockIndex = static_cast<uint16_t>(this->mc_Package.u8_ApplicationIndex);
   c_DdpSettings.u16_MaxMessageBufferTx = this->mc_Package.u16_MaxMessageBufferTx;
   c_DdpSettings.u16_MaxRoutingMessageBufferRx = this->mc_Package.u16_MaxRoutingMessageBufferRx;
   c_DdpSettings.u8_MaxParallelTransmissions = this->mc_Package.u8_MaxParallelTransmissions;

   C_PuiSdHandler::h_GetInstance()->SetNodeOpenSydeServerSettings(this->mu32_NodeIndex, c_DdpSettings);
   C_PuiSdHandler::h_GetInstance()->SetNodeCodeExportSettings(this->mu32_NodeIndex, c_GeneralSettings);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  returns path to halc def as it is in TSP

   \retval   QString   path to syde_halc_def file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbAddNewProject::GetHalcDefinitionFileName()
{
   QString c_HalcDefPath = mc_Package.c_HalcDefPath.c_str();

   return c_HalcDefPath.remove(0, 2);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  returns path to halc def file at location user chose in Import Dialog

   \retval   QString   path to syde_halc_def file
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbAddNewProject::GetProcessedHalcDefinitionPath(void)
{
   const QString c_Path = this->mpc_Ui->pc_LineEditCreateIn->GetPath();
   QString c_HalcDefPath = mc_Package.c_HalcDefPath.c_str();

   c_HalcDefPath.remove(0, 1);
   c_HalcDefPath = c_Path + c_HalcDefPath;

   return c_HalcDefPath;
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
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
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
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   QDir c_CreateInFolder(
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->GetPath()));

   if (pc_Node != NULL)
   {
      bool q_Continue = false;
      bool q_ValidTsp = false;
      if (this->ms32_TspReadResult != C_NO_ERR)
      {
         QString c_Details;
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Import TSP"));
         switch (this->ms32_TspReadResult)
         {
         case C_RANGE:
            c_Details = C_GtGetText::h_GetText("Specified openSYDE Target Support Package does not exist.");
            break;
         case C_NOACT:
            c_Details =
               C_GtGetText::h_GetText(
                  "Specified file is present but its structure is invalid (e.g. invalid XML file).");
            break;
         case C_CONFIG:
            c_Details = C_GtGetText::h_GetText("In specified file is a XML node or attribute missing.");
            break;
         default:
            c_Details = C_GtGetText::h_GetText("Unknown error occurred.");
            break;
         }
         c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                          "Could not load specified openSYDE Target Support Package.")));
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(180, 250);
         c_Message.Execute();
      }
      else
      {
         // no error on parsing TSP (see m_OnLoadTSP())
         q_ValidTsp = true;

         if (this->mc_Package.c_DeviceName != pc_Node->c_DeviceType)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Import TSP"));
            c_Message.SetDescription(
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "The openSYDE Target Support Package device \"%1\" does not match the "
                                       "device type \"%2\" of this node")).
               arg(this->mc_Package.c_DeviceName.c_str()).
               arg(pc_Node->c_DeviceType.c_str()));
            c_Message.SetCustomMinHeight(230, 180);
            c_Message.Execute();
         }
         // No output dir
         else if (this->mpc_Ui->pc_LineEditCreateIn->GetPath().isEmpty() == true)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
            c_Message.SetHeading(C_GtGetText::h_GetText("Import TSP"));
            c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                             "Define a directory for the project here.")));
            c_Message.SetCustomMinHeight(180, 180);
            c_Message.Execute();
         }
         // TSP device matches current node and dialog is executed properly -> user may continue
         else
         {
            q_Continue = true;
         }
      }

      if ((q_Continue == true) && (q_ValidTsp == true))
      {
         bool q_IsWarningSet = true;

         C_OgeWiCustomMessage c_Warning(this, C_OgeWiCustomMessage::eWARNING);
         c_Warning.SetHeading(C_GtGetText::h_GetText("Import TSP"));
         c_Warning.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
         c_Warning.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));

         // Currently there are datablocks for this node and the TSP contains datablocks AND additionally
         // there is a current halc config and TSP contains a halc config -> both gets deleted
         if (((pc_Node->c_Applications.empty() == false) && (mc_Package.c_Applications.size() > 0)) &&
             ((pc_Node->c_HalcConfig.IsClear() == false) && (mc_Package.c_HalcDefPath != "")))
         {
            c_Warning.SetDescription(C_GtGetText::h_GetText(
                                        "All existing Data Blocks will be deleted and the hardware configuration will be cleared. Do you really want to continue?"));
         }
         // Currently there are datablocks for this node and the TSP contains datablocks. (No check for halc -> see case
         // above)
         else if ((pc_Node->c_Applications.empty() == false) && (mc_Package.c_Applications.size() > 0))
         {
            c_Warning.SetDescription(C_GtGetText::h_GetText(
                                        "All existing Data Blocks will be deleted. Do you really want to continue?"));
         }
         // Currently there is a halc def and TSP contains halc def.
         else if ((pc_Node->c_HalcConfig.IsClear() == false) && (mc_Package.c_HalcDefPath != ""))
         {
            c_Warning.SetDescription(C_GtGetText::h_GetText(
                                        "The existing hardware configuration will be cleared. Do you really want to continue?"));
         }
         // Cases this else gets reached (no warning needed):
         // No current datablocks or halc
         // There is a current halc, but none in TSP
         // There are current datablocks, but none in TSP
         else
         {
            q_IsWarningSet = false;
         }

         if (q_IsWarningSet == true)
         {
            // only if warning is set the message box is executed. User cancels message box -> shall not continue
            if (c_Warning.Execute() != C_OgeWiCustomMessage::eOK)
            {
               q_Continue = false;
            }
         }
         // when no warning is set user may continue
      }

      if (q_Continue == true)
      {
         if ((c_CreateInFolder.exists() == true) &&
             (c_CreateInFolder.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() != 0))
         {
            q_Continue = false;

            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
            c_Message.SetHeading(C_GtGetText::h_GetText("Import TSP"));
            c_Message.SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));
            c_Message.SetNoButtonText(C_GtGetText::h_GetText("Continue without Clearing"));
            c_Message.SetOkButtonText(C_GtGetText::h_GetText("Clear and Continue"));
            c_Message.SetDescription(
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "Directory \"%1\" is not empty. \n\nShould this directory be cleared? "
                                       "Attention: Your data will be lost!")).arg(c_CreateInFolder.
                                                                                  absolutePath()));
            c_Message.SetCustomMinHeight(230, 230);
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
                  c_Message2.SetHeading(C_GtGetText::h_GetText("Import TSP"));
                  c_Message2.SetDescription(
                     static_cast<QString>(C_GtGetText::h_GetText("Could not clear directory \"%1\".")).
                     arg(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->GetPath())));
                  QApplication::restoreOverrideCursor();
                  c_Message2.SetCustomMinHeight(180, 180);
                  c_Message2.Execute();
               }
               break;
            case C_OgeWiCustomMessage::eNO:
               q_Continue = true;
               break;
            case C_OgeWiCustomMessage::eCANCEL:
            default:
               //Abort
               break;
            }
         }
         // if folder does not exist yet, q_Continue remains true

         if (q_Continue == true)
         {
            const QFileInfo c_TspFileInfo(this->GetTspPath()); // file path -> use absoluteDir() to get directory of
                                                               // file
            stw::scl::C_SclString c_ErrorText;
            const QString c_Path =
               QDir::cleanPath(c_TspFileInfo.absoluteDir().absoluteFilePath(this->mc_Package.c_TemplatePath.c_str()));
            QApplication::setOverrideCursor(Qt::WaitCursor);
            if (C_OscZipFile::h_UnpackZipFile(c_Path.toStdString().c_str(),
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
               c_Message.SetHeading(C_GtGetText::h_GetText("Import TSP"));
               c_Message.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                                "Could not extract openSYDE Target Support Package "
                                                                "from file \"%1\" to directory \"%2\".")).arg(
                                           c_Path).
                                        arg(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditCreateIn->
                                                                                    GetPath())));
               c_Message.SetCustomMinWidth(800);
               c_Message.SetDetails(c_ErrorText.c_str());
               c_Message.SetCustomMinHeight(230, 300);
               QApplication::restoreOverrideCursor();
               c_Message.Execute();
            }
         }
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
void C_SdNdeDbAddNewProject::m_TspButtonClicked(void)
{
   QString c_FolderName; // for default folder
   QString c_FilePath = "";
   const QFileInfo c_File(C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditTSP->GetPath()));
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("openSYDE Target Support Package file")) +
                                " (*." + mhc_SUFFIX + ")";

   if (c_File.exists() == true)
   {
      c_FolderName = c_File.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   c_FilePath =
      C_OgeWiUtil::h_GetOpenFileName(this, C_GtGetText::h_GetText("Select openSYDE Target Support Package File"),
                                     c_FolderName, c_FilterName, mhc_SUFFIX);
   if (c_FilePath != "")
   {
      this->SetTspPath(c_FilePath);
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

   const QString c_Path = QFileDialog::getExistingDirectory(
      this,
      C_GtGetText::h_GetText("Select Directory for Target Project"),
      c_FolderName, QFileDialog::ShowDirsOnly);

   if (c_Path != "")
   {
      this->m_SetCreateInPath(c_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle a dropped folder path in create in line edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_OnDroppedCreatinPath(void)
{
   this->m_SetCreateInPath(this->mpc_Ui->pc_LineEditCreateIn->GetPath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full create in path.

   \param[in] orc_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_SetCreateInPath(const QString & orc_New)
{
   // check if relative path is possible and appreciated
   const QString c_Path = C_ImpUtil::h_AskUserToSaveRelativePath(this, orc_New,
                                                                 C_PuiProject::h_GetInstance()->GetFolderPath());

   if (c_Path != "")
   {
      this->mpc_Ui->pc_LineEditCreateIn->SetPath(c_Path, C_PuiProject::h_GetInstance()->GetFolderPath());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle loading TSP
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_OnLoadTsp(void)
{
   this->ms32_TspReadResult = C_OscTargetSupportPackageFiler::h_Load(
      this->mc_Package,
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditTSP->GetPath()).toStdString().c_str());

   if (this->ms32_TspReadResult == C_NO_ERR)
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
   orc_Content += "<h3>" + static_cast<QString>(C_GtGetText::h_GetText("openSYDE Target Support Package")) + "</h3>";
   orc_Content += "<table>";
   orc_Content += "<tr>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_START_TD;
   orc_Content += C_GtGetText::h_GetText("Description:");
   orc_Content += "</td>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_CONTINUE_TD;
   orc_Content += this->mc_Package.c_Comment.c_str();
   orc_Content += "</td>";
   orc_Content += "</tr>";
   orc_Content += "<tr>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_START_TD;
   orc_Content += C_GtGetText::h_GetText("Number of Data Blocks:");
   orc_Content += "</td>";
   orc_Content += C_SdNdeDbAddNewProject::mhc_CONTINUE_TD;
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
   for (uint32_t u32_ItTemplate = 0UL; u32_ItTemplate < this->mc_Package.c_Applications.size(); ++u32_ItTemplate)
   {
      const C_OscTspApplication & rc_Template = this->mc_Package.c_Applications[u32_ItTemplate];
      orc_Content += "<h4>" + static_cast<QString>(C_GtGetText::h_GetText("Data Block %1")).arg(u32_ItTemplate + 1) +
                     "</h4>";
      orc_Content += "<table>";
      orc_Content += "<tr>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_START_TD;
      orc_Content += C_GtGetText::h_GetText("Name:");
      orc_Content += "</td>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_CONTINUE_TD;
      orc_Content += rc_Template.c_Name.c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_START_TD;
      orc_Content += C_GtGetText::h_GetText("Comment:");
      orc_Content += "</td>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_CONTINUE_TD;
      orc_Content += rc_Template.c_Comment.c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "</table>";
   }

   // if we have a HALC Config in TSP we want to display the path to the halc_def-file and a comment
   if ((mc_Package.c_HalcDefPath != "") && (mc_Package.c_HalcComment != ""))
   {
      orc_Content += "<h4>" + static_cast<QString>(C_GtGetText::h_GetText("Hardware Configuration")) +
                     "</h4>";
      orc_Content += "<table>";
      orc_Content += "<tr>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_START_TD;
      orc_Content += C_GtGetText::h_GetText("HALC definition file:");
      orc_Content += "</td>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_CONTINUE_TD;
      orc_Content += this->mc_Package.c_HalcDefPath.c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "<tr>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_START_TD;
      orc_Content += C_GtGetText::h_GetText("Comment: ");
      orc_Content += "</td>";
      orc_Content += C_SdNdeDbAddNewProject::mhc_CONTINUE_TD;
      orc_Content += this->mc_Package.c_HalcComment.c_str();
      orc_Content += "</td>";
      orc_Content += "</tr>";
      orc_Content += "</table>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init path and title based on specified application

   \param[in] ou32_NodeIndex        Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAddNewProject::m_Init(const uint32_t ou32_NodeIndex) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Use default
      const QString c_NodePath = C_OscUtils::h_NiceifyStringForFileName(pc_Node->c_Properties.c_Name).c_str();
      this->mpc_Ui->pc_LineEditCreateIn->SetPath(c_NodePath, C_PuiProject::h_GetInstance()->GetFolderPath());

      //Set Title
      const stw::scl::C_SclString c_NodeName = pc_Node->c_Properties.c_Name;
      const QString c_QnodeName = static_cast<QString>(c_NodeName.c_str());
      const QString c_Title = c_QnodeName;
      this->mrc_ParentDialog.SetTitle(c_Title);
   }
}
