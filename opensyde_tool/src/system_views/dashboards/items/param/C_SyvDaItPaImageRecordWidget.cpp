//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of recording an NVM image to a file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>

#include <QFileInfo>
#include <QFileDialog>
#include <QDateTime>

#include "stwerrors.h"

#include "C_SyvDaItPaImageRecordWidget.h"
#include "ui_C_SyvDaItPaImageRecordWidget.h"

#include "constants.h"
#include "C_Uti.h"
#include "C_OSCUtils.h"
#include "C_OgeWiUtil.h"
#include "C_UsHandler.h"
#include "C_GtGetText.h"
#include "C_SyvDaItUtil.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCLoggingHandler.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaItPaImageRecordWidget::mhc_FILE_EXTENSION = ".syde_psi";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent       Parent
   \param[in,out]  orc_ComDriver    Com driver
   \param[in]      orc_ListItemIds  List item ids
   \param[in]      orc_ViewName     View name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaImageRecordWidget::C_SyvDaItPaImageRecordWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                           C_SyvComDriverDiag & orc_ComDriver,
                                                           const std::vector<C_OSCNodeDataPoolListElementId > & orc_ListItemIds,
                                                           const QString & orc_ViewName) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaItPaImageRecordWidget),
   mpc_ParentDialog(&orc_Parent),
   mrc_ComDriver(orc_ComDriver),
   mc_ListItemIds(orc_ListItemIds),
   me_Step(eSELECTFILE),
   mu32_CurrentNode(0U),
   mq_RequestPending(false),
   mc_FilePath(""),
   mc_ViewName(orc_ViewName)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);
   this->mpc_Ui->pc_LabelRead->setEnabled(false);
   this->mpc_Ui->pc_LabelConfirm->setEnabled(false);
   this->mpc_Ui->pc_LabelValidateFile->setEnabled(false);
   this->mpc_Ui->pc_LabelFinished->setEnabled(false);
   this->mpc_Ui->pc_WiConfirm->setEnabled(true);
   this->mpc_Ui->pc_CbConfirm->setEnabled(false);
   this->mpc_Ui->pc_CbConfirm->setVisible(false);
   this->mpc_Ui->pc_BushButtonOk->setVisible(false);
   this->mpc_Ui->pc_WiConfirm->setVisible(false);

   //Register close
   this->mpc_ParentDialog->SetNotifyAndBlockClose(true);

   // init gui
   this->mpc_Ui->pc_BopperleSelectFile->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);

   this->InitText();

   this->mpc_Ui->pc_TextBrowserConfirm->setHtml(C_GtGetText::h_GetText("Empty, not read yet."));

   m_LoadUserSettings();

   m_PrepareVariablesForParametrization();

   //Remove debug text
   this->mpc_Ui->pc_GroupBoxUserComment->setTitle("");
   this->mpc_Ui->pc_PushButtonBrowse->setText("");

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDaItPaImageRecordWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvDaItPaImageRecordWidget::m_OnCancel);
   connect(this->mpc_ParentDialog, &C_OgePopUpDialog::SigCloseIgnored,
           this, &C_SyvDaItPaImageRecordWidget::m_OnCancel);
   connect(this->mpc_Ui->pc_PushButtonBrowse, &QPushButton::clicked, this, &C_SyvDaItPaImageRecordWidget::m_OnBrowse);
   connect(this->mpc_Ui->pc_PbConfirm, &QPushButton::clicked, this, &C_SyvDaItPaImageRecordWidget::m_ConfirmClicked);

   connect(this->mpc_Ui->pc_CbConfirm, &C_OgeChxProperties::stateChanged,
           this, &C_SyvDaItPaImageRecordWidget::m_ConfirmCheckBoxChanged);
   connect(this->mpc_Ui->pc_LineEditPath, &C_OgeLeFilePath::editingFinished,
           this, &C_SyvDaItPaImageRecordWidget::m_FilePathChanged);

   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvDaItPaImageRecordWidget::m_Timer);
   this->mc_Timer.setInterval(15);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaImageRecordWidget::~C_SyvDaItPaImageRecordWidget()
{
   //Clean up
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_ParentDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::InitText(void)
{
   // set title
   this->mpc_ParentDialog->SetTitle(QString(C_GtGetText::h_GetText("Parametrization")));
   this->mpc_ParentDialog->SetSubTitle(QString(C_GtGetText::h_GetText("Record Parameter Set from System")));

   this->mpc_Ui->pc_LabelFinished->setText(QString(C_GtGetText::h_GetText("Finished")));
   this->mpc_Ui->pc_LabelSelectFile->setText(QString(C_GtGetText::h_GetText("File")));
   this->mpc_Ui->pc_LabelRead->setText(QString(C_GtGetText::h_GetText("Read")));
   this->mpc_Ui->pc_LabelConfirm->setText(QString(C_GtGetText::h_GetText("Confirm")));
   this->mpc_Ui->pc_LabelValidateFile->setText(QString(C_GtGetText::h_GetText("Validate File(s)")));

   this->mpc_Ui->pc_LabelHeadingSelectFile->setText(C_GtGetText::h_GetText("File"));
   this->mpc_Ui->pc_LabelStepDescription->setText(
      C_GtGetText::h_GetText("Choose a path to save the file to and click "
                             "\"Read Parameters\" to start the parameter set record process."));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_CommentText->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
   this->mpc_Ui->pc_LabelHeadingConfirm->setText(C_GtGetText::h_GetText("Read Parameter Lists"));
   this->mpc_Ui->pc_LineEditPath->setPlaceholderText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LabelMultipleNodes->setText(
      C_GtGetText::h_GetText(
         "Attention: Recording for multiple nodes will result in one file per node."
         " Name format: \"<SelectedPathAndFileName>_<NodeName>.syde_psi\""));

   this->mpc_Ui->pc_PbConfirm->setText(QString(C_GtGetText::h_GetText("Read Parameters")));
   this->mpc_Ui->pc_CbConfirm->setText(QString(C_GtGetText::h_GetText(
                                                  "Confirmed, all required parameter lists are included")));

   this->mpc_Ui->pc_PushButtonBrowse->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse"),
      C_GtGetText::h_GetText("Browse for location where to save recorded parameter set image file."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle save user settings operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::SaveUserSettings(void) const
{
   //Save
   QString c_Name;
   QString c_Path;

   m_SplitNameAndPath(this->mc_FilePath, c_Name, c_Path);

   C_UsHandler::h_GetInstance()->SetProjSvParamRecord(this->mc_ViewName, c_Path, c_Name);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get file info

   \param[in]  orc_Comment    User comment for file

   \return
   File info
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCParamSetInterpretedFileInfoData C_SyvDaItPaImageRecordWidget::h_GetFileInfoData(const QString & orc_Comment)
{
   C_OSCParamSetInterpretedFileInfoData c_Retval;
   QString c_Name = qgetenv("USER");

   if (c_Name.isEmpty())
   {
      c_Name = qgetenv("USERNAME");
   }
   c_Retval.c_Creator = c_Name.toStdString().c_str();
   c_Retval.c_DateTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss").toStdString().c_str();
   c_Retval.c_ToolName = "openSYDE";
   //lint -e{40} Defined by project file
   c_Retval.c_ToolVersion = C_Uti::h_GetApplicationVersion().toStdString().c_str();
   c_Retval.c_ProjectName = C_PuiProject::h_GetInstance()->GetName().toStdString().c_str();
   c_Retval.c_ProjectVersion = C_PuiProject::h_GetInstance()->c_Version;
   c_Retval.c_UserComment = orc_Comment.toStdString().c_str();
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: handle escape key

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::keyPressEvent(QKeyEvent * const opc_Event)
{
   if (opc_Event->key() == static_cast<sintn>(Qt::Key_Escape))
   {
      m_OnCancel();
   }
   else
   {
      QWidget::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle browse event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_OnBrowse(void)
{
   QString c_Folder = C_Uti::h_GetAbsolutePathFromExe(this->mpc_Ui->pc_LineEditPath->GetPath());

   const QString c_Path =
      C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("Select File for Parameter Set Image"),
                                     c_Folder, C_GtGetText::h_GetText(
                                        "openSYDE parameter set image (*") + mhc_FILE_EXTENSION + ")",
                                     "", QFileDialog::DontConfirmOverwrite); // overwrite is handled later

   if (c_Path.compare("") != 0)
   {
      this->mpc_Ui->pc_LineEditPath->SetPath(c_Path);
      this->mc_FilePath = C_Uti::h_GetAbsolutePathFromExe(c_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Split the full path of a file into path and name.

   \param[in]   orc_FullPath  full path to file
   \param[out]  orc_Name      file name
   \param[out]  orc_Path      path without file name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_SplitNameAndPath(const QString & orc_FullPath, QString & orc_Name,
                                                      QString & orc_Path) const
{
   const QUrl c_FullPathUrl = QUrl::fromLocalFile(orc_FullPath);

   if (c_FullPathUrl.isValid() == true)
   {
      orc_Name = c_FullPathUrl.fileName();
      orc_Path = orc_FullPath;
      orc_Path.remove(orc_Name);
   }
}
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_FilePathChanged(void)
{
   this->mc_FilePath = this->mpc_Ui->pc_LineEditPath->GetPath();

   if (this->mc_FilePath.isEmpty() == false)
   {
      this->mc_FilePath = C_Uti::h_GetAbsolutePathFromExe(this->mc_FilePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current set path and project name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_ReadClicked(void)
{
   if (this->mc_FilePath.compare("") != 0)
   {
      std::vector<QString> c_ConflictedFiles;
      const QFileInfo c_BaseInfo(this->mc_FilePath);
      //Check conflicted files with automated file creation
      if (this->mc_AllNodeIndexes.size() > 1UL)
      {
         for (uint32 u32_ItNodes = 0UL; u32_ItNodes < this->mc_AllNodeIndexes.size(); ++u32_ItNodes)
         {
            const QString c_FullPath = this->m_GetPathForNode(this->mc_AllNodeIndexes[u32_ItNodes],
                                                              this->mc_FilePath);
            const QFileInfo c_NodeFileInfo(c_FullPath);
            if (c_NodeFileInfo.exists() == true)
            {
               c_ConflictedFiles.push_back(c_FullPath);
            }
         }
      }
      if (c_BaseInfo.dir().exists() == true)
      {
         if (("." + c_BaseInfo.completeSuffix()) == mhc_FILE_EXTENSION)
         {
            if (C_OSCUtils::h_CheckValidFilePath(c_BaseInfo.absoluteFilePath().toStdString().c_str()) == true)
            {
               // Check if file exists already
               QFile c_File;
               bool q_Continue = true;

               c_File.setFileName(this->mc_FilePath);

               if (((c_File.exists() == true) && (this->mc_AllNodeIndexes.size() == 1UL)) ||
                   (c_ConflictedFiles.size() > 0UL))
               {
                  QString c_Details = C_GtGetText::h_GetText("The following file(s) already exist: \n");
                  C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
                  C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

                  c_MessageBox.SetHeading(C_GtGetText::h_GetText("Parameter Set Image File save"));
                  c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                                 "Do you really want to overwrite the existing file(s)?"));
                  //Append conflicted files as details
                  if (c_ConflictedFiles.size() > 0UL)
                  {
                     for (uint32 u32_ItFile = 0UL; u32_ItFile < c_ConflictedFiles.size(); ++u32_ItFile)
                     {
                        c_Details += c_ConflictedFiles[u32_ItFile];
                        c_Details += "\n";
                     }
                  }
                  else
                  {
                     c_Details += this->mc_FilePath;
                  }
                  c_MessageBox.SetDetails(c_Details);
                  c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Overwrite"));
                  c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Back"));
                  c_MessageBox.SetCustomMinHeight(180, 300);
                  c_MessageBox.SetCustomMinWidth(800);
                  e_ReturnMessageBox = c_MessageBox.Execute();

                  switch (e_ReturnMessageBox)
                  {
                  case C_OgeWiCustomMessage::eYES:
                     if (this->mc_AllNodeIndexes.size() == 1UL)
                     {
                        // Delete old file
                        q_Continue = c_File.remove();
                     }
                     else
                     {
                        //Delete automatically generated files
                        for (uint32 u32_ItFile = 0UL; u32_ItFile < c_ConflictedFiles.size(); ++u32_ItFile)
                        {
                           QFile c_CurFile;

                           c_CurFile.setFileName(c_ConflictedFiles[u32_ItFile]);

                           // Delete old file
                           q_Continue = c_CurFile.remove();

                           if (q_Continue == false)
                           {
                              c_Details = C_GtGetText::h_GetText("The following file(s) can not be overwritten: \n");
                              c_Details += c_ConflictedFiles[u32_ItFile];
                              break;
                           }
                        }
                     }
                     if (q_Continue == false)
                     {
                        // Error on deleting file
                        C_OgeWiCustomMessage c_MessageBoxErrorRemove(this, C_OgeWiCustomMessage::E_Type::eERROR);
                        c_MessageBoxErrorRemove.SetHeading(C_GtGetText::h_GetText("Parameter Set Image File save"));
                        c_MessageBoxErrorRemove.SetDescription(C_GtGetText::h_GetText("File cannot be overwritten!"));
                        c_MessageBoxErrorRemove.SetDetails(c_Details);
                        c_MessageBoxErrorRemove.SetCustomMinHeight(180, 300);
                        c_MessageBoxErrorRemove.Execute();
                     }
                     break;
                  case C_OgeWiCustomMessage::eCANCEL:
                     // do not continue and do nothing
                     q_Continue = false;
                     break;
                  case C_OgeWiCustomMessage::eNO:
                     // do not continue and do nothing
                     q_Continue = false;
                     break;
                  default:
                     break;
                  }
               }

               if (q_Continue == true)
               {
                  // Adapt GUI
                  this->mpc_Ui->pc_ProgressSelectFile->SetProgress(100);
                  this->mpc_Ui->pc_GroupBoxUserComment->setVisible(false);
                  this->mpc_Ui->pc_WiConfirm->setVisible(true);
                  this->mpc_Ui->pc_LineEditPath->setVisible(false);
                  this->mpc_Ui->pc_PushButtonBrowse->setVisible(false);
                  this->mpc_Ui->pc_LabelRead->setEnabled(true);
                  this->mpc_Ui->pc_BopperleRead->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
                  this->mpc_Ui->pc_LabelHeadingSelectFile->setText(C_GtGetText::h_GetText("Read Parameter Values"));
                  this->mpc_Ui->pc_LabelStepDescription->setText(C_GtGetText::h_GetText(
                                                                    "Reading parameter values from NVM..."));
                  this->mpc_Ui->pc_PbConfirm->setEnabled(false);

                  this->m_StartReadElementsOfNode();
               }
            }
            else
            {
               C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_MessageBox.SetHeading(C_GtGetText::h_GetText("Parameter Set Image File"));
               c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("File path contains invalid characters.")));
               c_MessageBox.SetCustomMinHeight(180, 180);
               c_MessageBox.Execute();
            }
         }
         else
         {
            C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Parameter Set Image File"));
            c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText(
                                                   "The specified file has the wrong extension, use: \"%1\".")).arg(
                                           C_SyvDaItPaImageRecordWidget::mhc_FILE_EXTENSION));
            c_MessageBox.SetDetails(QString("Invalid extension: \"%1\"").arg("." + c_BaseInfo.completeSuffix()));
            c_MessageBox.SetCustomMinHeight(180, 250);
            c_MessageBox.Execute();
         }
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Parameter Set Image File"));
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("The specified directory does not exist."));
         c_MessageBox.SetDetails(QString("Invalid path: \"%1\"").arg(this->mc_FilePath));
         c_MessageBox.SetCustomMinHeight(180, 250);
         c_MessageBox.Execute();
      }
   }
   else
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Parameter Set Image File"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("The file path is empty. Please enter valid file path."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Starts the read of all elements

   Implemented steps: 1
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_StartReadElementsOfNode(void)
{
   //Each node
   // Prepare all data dealer (Step 1)
   for (uint32 u32_ItNode = 0U; u32_ItNode < this->mc_AllNodeIndexes.size(); ++u32_ItNode)
   {
      tgl_assert(this->mrc_ComDriver.NvmSafeClearInternalContent(this->mc_AllNodeIndexes[u32_ItNode]) == C_NO_ERR);
   }

   //Create file
   this->me_Step = eCREATEPARAMETERSETFILE;

   this->mc_Timer.start();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepare all variables for parametrization
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_PrepareVariablesForParametrization(void)
{
   uint32 u32_Counter;

   // Prepare the writing for each node
   this->mc_AllNodeIndexes.clear();
   this->mc_RelevantListsForEachNode.clear();

   this->mu32_CurrentNode = 0U;

   for (u32_Counter = 0U; u32_Counter < this->mc_ListItemIds.size(); ++u32_Counter)
   {
      // Check if a changed element for this node was registered already
      uint32 u32_NodeCounter;
      bool q_NodeFound = false;

      for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_AllNodeIndexes.size(); ++u32_NodeCounter)
      {
         if (this->mc_AllNodeIndexes[u32_NodeCounter] == this->mc_ListItemIds[u32_Counter].u32_NodeIndex)
         {
            q_NodeFound = true;
            break;
         }
      }

      if (q_NodeFound == false)
      {
         // Both indexes must be synchronous
         // Add the node index
         this->mc_AllNodeIndexes.push_back(this->mc_ListItemIds[u32_Counter].u32_NodeIndex);
         u32_NodeCounter = static_cast<uint32>(this->mc_AllNodeIndexes.size()) - 1;
         //  Adapt size of vector for each node
         this->mc_RelevantListsForEachNode.resize(this->mc_RelevantListsForEachNode.size() + 1);
      }

      // Save all lists
      this->mc_RelevantListsForEachNode[u32_NodeCounter].push_back(
         static_cast<C_OSCNodeDataPoolListId>(this->mc_ListItemIds[u32_Counter]));
   }

   //Handle user info
   this->mpc_Ui->pc_LabelMultipleNodes->setVisible(this->mc_AllNodeIndexes.size() > 1UL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read parameters for all parameter lists

   Implemented steps: 2

   \param[in]  orc_Comment    User comment for file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_ReadElementsOfNode(const QString & orc_Comment)
{
   if (this->mu32_CurrentNode < this->mc_AllNodeIndexes.size())
   {
      sint32 s32_Return;

      if (this->mq_RequestPending == false)
      {
         // Step 2
         s32_Return = this->mrc_ComDriver.PollNvmSafeReadParameterValues(
            this->mc_AllNodeIndexes[this->mu32_CurrentNode],
            this->mc_RelevantListsForEachNode[this->mu32_CurrentNode]);

         if (s32_Return == C_NO_ERR)
         {
            this->mq_RequestPending = true;
         }
         else if (s32_Return == C_RANGE)
         {
            // Error
            this->mc_Timer.stop();
            this->m_ReportError("NvmSafeReadParameterValues", "Wrong node index.", s32_Return);
         }
         else
         {
            // On C_BUSY is nothing to do
         }
      }
      else
      {
         sint32 s32_ServiceResult;
         s32_Return = this->mrc_ComDriver.GetPollResults(s32_ServiceResult);

         if (s32_Return == C_NO_ERR)
         {
            // Thread finished
            if (s32_ServiceResult == C_NO_ERR)
            {
               // Prepare the next node
               this->mq_RequestPending = false;
               ++this->mu32_CurrentNode;

               if (this->mu32_CurrentNode >= this->mc_AllNodeIndexes.size())
               {
                  // No further nodes
                  this->mu32_CurrentNode = 0U;
                  this->mc_Timer.stop();

                  // Start step 3
                  s32_Return = this->m_CreateParameterSetFile(orc_Comment);
                  if (s32_Return == C_NO_ERR)
                  {
                     // Start step 4
                     s32_Return = this->m_ReadBackElementsOfNodeFromFile();

                     if (s32_Return == C_NO_ERR)
                     {
                        this->mpc_Ui->pc_WiConfirm->setEnabled(true);
                        this->mpc_Ui->pc_ProgressRead->SetProgress(100);
                        this->mpc_Ui->pc_LabelConfirm->setEnabled(true);
                        this->mpc_Ui->pc_BopperleConfirm->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21,
                                                                               mc_STYLE_GUIDE_COLOR_13);
                        this->mpc_Ui->pc_LabelHeadingSelectFile->setText(C_GtGetText::h_GetText("Confirm"));
                        this->mpc_Ui->pc_LabelStepDescription->setText(
                           C_GtGetText::h_GetText("Parameter lists are read from system. Check if all required "
                                                  "parameter lists are listed below. If yes, check "
                                                  "\"All required parameter lists are included\" "
                                                  "and click \"Validate File(s)\"."));
                        this->mpc_Ui->pc_PbConfirm->setText(QString(C_GtGetText::h_GetText("Validate File(s)")));
                        this->mpc_Ui->pc_CbConfirm->setVisible(true);
                        this->mpc_Ui->pc_CbConfirm->setEnabled(true);

                        this->me_Step = eCONFIRM;
                     }
                  }
               }
               else
               {
                  this->mpc_Ui->pc_ProgressRead->SetProgress((this->mu32_CurrentNode * 100U) /
                                                             this->mc_AllNodeIndexes.size());
               }
            }
            else
            {
               // Error of service
               this->mc_Timer.stop();
               m_ReportErrorNvmSafeReadParameterValues(s32_ServiceResult);
            }
         }
      }
   }
   else
   {
      // Error of service
      this->mc_Timer.stop();
      this->m_ReportError("NvmSafeReadParameterValues", "No lists found.", C_RANGE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create parameter set file

   Implemented steps: 3

   \param[in]  orc_Comment    User comment for file

   \return
   C_NO_ERR   data saved
   C_OVERFLOW Wrong sequence of function calls
   C_CONFIG   Internal data invalid
   C_BUSY     File already exists
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDaItPaImageRecordWidget::m_CreateParameterSetFile(const QString & orc_Comment)
{
   const C_OSCParamSetInterpretedFileInfoData c_FileInfo = C_SyvDaItPaImageRecordWidget::h_GetFileInfoData(orc_Comment);
   sint32 s32_Return = C_NO_ERR;

   for (uint32 u32_ItNode = 0U; (u32_ItNode < this->mc_AllNodeIndexes.size()) && (s32_Return == C_NO_ERR); ++u32_ItNode)
   {
      s32_Return = this->mrc_ComDriver.NvmSafeCreateCleanFileWithoutCRC(
         this->mc_AllNodeIndexes[u32_ItNode],
         this->m_GetPathForNode(this->mc_AllNodeIndexes[u32_ItNode], this->mc_FilePath),
         c_FileInfo);
   }

   if (s32_Return != C_NO_ERR)
   {
      QString c_ErrorText;

      switch (s32_Return)
      {
      case C_OVERFLOW:
         c_ErrorText = "Wrong sequence of function calls.";
         break;
      case C_CONFIG:
         c_ErrorText = "Internal data invalid.";
         break;
      case C_BUSY:
         c_ErrorText = "File already exists.";
         break;
      case C_RD_WR:
         c_ErrorText = "Could not write to file (e.g. missing write permissions or missing directory).";
         break;
      default:
         c_ErrorText = QString("Creating file failed with unknown reason (%1).").arg(s32_Return);
         break;
      }

      this->m_ReportError("NvmSafeCreateCleanFileWithoutCRC", c_ErrorText, s32_Return);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read parameters back from file

   Implemented steps: 4, 5

   \return
   C_NO_ERR   data read
   C_OVERFLOW Wrong sequence of function calls
   C_RANGE    Path does not match the path of the preceding function calls
              At least one node index of the relevant node indexes is out of range
   C_RD_WR    specified file does not exist
              specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG   Mismatch of data with current node
               or no valid pointer to the original instance of "C_OSCNode" is set in "C_OSCDataDealer"
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvDaItPaImageRecordWidget::m_ReadBackElementsOfNodeFromFile(void)
{
   sint32 s32_Return = C_NO_ERR;
   uint32 u32_Counter;

   this->me_Step = eREADBACKFROMFILE;

   // Step 4
   for (u32_Counter = 0U; (u32_Counter < this->mc_AllNodeIndexes.size()) && (s32_Return == C_NO_ERR); ++u32_Counter)
   {
      s32_Return =
         this->mrc_ComDriver.NvmSafeReadFileWithoutCRC(this->mc_AllNodeIndexes[u32_Counter],
                                                       this->m_GetPathForNode(this->mc_AllNodeIndexes[u32_Counter],
                                                                              this->mc_FilePath));
   }

   if (s32_Return == C_NO_ERR)
   {
      std::vector<std::vector<C_OSCNodeDataPoolListId> > c_DataPoolListsForEachNode;
      c_DataPoolListsForEachNode.resize(this->mc_AllNodeIndexes.size());

      for (u32_Counter = 0U; u32_Counter < this->mc_AllNodeIndexes.size(); ++u32_Counter)
      {
         // Step 5
         s32_Return = this->mrc_ComDriver.NvmSafeCheckParameterFileContents(
            this->mc_AllNodeIndexes[u32_Counter],
            this->m_GetPathForNode(this->mc_AllNodeIndexes[u32_Counter], this->mc_FilePath),
            c_DataPoolListsForEachNode[u32_Counter]);

         if (s32_Return != C_NO_ERR)
         {
            QString c_ErrorText;

            switch (s32_Return)
            {
            case C_OVERFLOW:
               c_ErrorText = "Wrong sequence of function calls.";
               break;
            case C_CONFIG:
               c_ErrorText = "Mismatch of data with current node "
                             "or no valid pointer to the original instance of"
                             " \"C_OSCNode\" is set in \"C_OSCDataDealer\".";
               break;
            case C_RANGE:
               c_ErrorText = "Path does not match the path of the preceding function calls or node index out of range.";
               break;
            case C_RD_WR:
               c_ErrorText = "Specified file does not exist "
                             " specified file is present but structure is invalid (e.g. invalid XML file)";
               break;
            default:
               c_ErrorText = QString("Read back of file failed with unknown reason (%1).").arg(s32_Return);
               break;
            }

            this->m_ReportError("NvmSafeCheckParameterFileContents", c_ErrorText, s32_Return);
            break;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         this->m_PrepareConfirmStep(c_DataPoolListsForEachNode);
      }
   }
   else
   {
      this->m_ReportError("NvmSafeReadFileWithoutCRC", "Read back of file failed.", s32_Return);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get text to display for a step

   \param[in]  orc_DataPoolListsForEachNode  Read infos
   \param[in]  oq_IsConfirm                  Flag if confirm step

   \return
   Text to display
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaImageRecordWidget::m_GetTextForStep(
   const std::vector<std::vector<C_OSCNodeDataPoolListId> > & orc_DataPoolListsForEachNode,
   const bool oq_IsConfirm) const
{
   std::list<QString> c_NodeText;
   std::list<QString>::const_iterator c_ItNodeText;
   QString c_Text;
   uint32 u32_NodeCounter;

   for (u32_NodeCounter = 0U; u32_NodeCounter < orc_DataPoolListsForEachNode.size(); ++u32_NodeCounter)
   {
      if (orc_DataPoolListsForEachNode[u32_NodeCounter].size() > 0)
      {
         const uint32 u32_CurNodeIndex = orc_DataPoolListsForEachNode[u32_NodeCounter][0].u32_NodeIndex;
         const C_OSCNode * const pc_OSCNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_CurNodeIndex);

         c_Text = "";

         if (pc_OSCNode != NULL)
         {
            uint32 u32_ListCounter;
            uint32 u32_CurDataPoolIndex = 0xFFFFFFFFU;
            const QString c_Path = this->m_GetPathForNode(u32_CurNodeIndex, this->mc_FilePath);
            const QString c_PathHtml = C_Uti::h_GetLink(c_Path, mc_STYLE_GUIDE_COLOR_LINK, "file:///" + c_Path);

            // Node name
            if (oq_IsConfirm == false)
            {
               c_Text += "<div>" + QString(C_GtGetText::h_GetText("Path: %1")).arg(c_PathHtml) + "</div>";
            }
            if (oq_IsConfirm == true)
            {
               c_Text += "<div><u>";
            }
            else
            {
               c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(1UL));
            }
            c_Text += QString(C_GtGetText::h_GetText("Node")) + " - " +
                      QString(pc_OSCNode->c_Properties.c_Name.c_str());
            if (oq_IsConfirm == true)
            {
               c_Text += "</u>";
            }
            c_Text += "</div>";

            for (u32_ListCounter = 0U;
                 u32_ListCounter < orc_DataPoolListsForEachNode[u32_NodeCounter].size();
                 ++u32_ListCounter)
            {
               const C_OSCNodeDataPoolList * const pc_OSCList =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
                     u32_CurNodeIndex,
                     orc_DataPoolListsForEachNode[u32_NodeCounter][u32_ListCounter].u32_DataPoolIndex,
                     orc_DataPoolListsForEachNode[u32_NodeCounter][u32_ListCounter].u32_ListIndex);

               if (u32_CurDataPoolIndex !=
                   orc_DataPoolListsForEachNode[u32_NodeCounter][u32_ListCounter].u32_DataPoolIndex)
               {
                  // A new datapool
                  const C_OSCNodeDataPool * pc_OSCDataPool;

                  u32_CurDataPoolIndex =
                     orc_DataPoolListsForEachNode[u32_NodeCounter][u32_ListCounter].u32_DataPoolIndex;

                  pc_OSCDataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(u32_CurNodeIndex,
                                                                                   u32_CurDataPoolIndex);

                  if (pc_OSCDataPool != NULL)
                  {
                     if (oq_IsConfirm == true)
                     {
                        c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(1UL));
                     }
                     else
                     {
                        c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(2UL));
                     }
                     c_Text += QString(C_GtGetText::h_GetText("DataPool")) +
                               " - " + QString(pc_OSCDataPool->c_Name.c_str()) + "</div>";
                  }
               }

               if (pc_OSCList != NULL)
               {
                  // Listname
                  if (oq_IsConfirm == true)
                  {
                     c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(2UL));
                  }
                  else
                  {
                     c_Text += QString("<div %1>").arg(C_SyvDaItUtil::mh_GetHtmlIndentStyle(3UL));
                  }
                  c_Text += QString(C_GtGetText::h_GetText("List")) +
                            " - " + QString(pc_OSCList->c_Name.c_str()) + "</div>";
               }
            }
         }
         c_Text += "<br />";
         c_NodeText.push_back(c_Text);
      }
   }

   // Sort for node names. The node name is always at the start of each string and must be unique.
   c_NodeText.sort();
   // Font and style configuration. Stylesheets does not work with html. Color is STYLE_GUIDE_COLOR_6
   c_Text = "<html><body><span style=\" font-family:Segoe UI; font-size:13px; font-weight: normal; color:#565668;\">";

   for (c_ItNodeText = c_NodeText.begin(); c_ItNodeText != c_NodeText.end(); ++c_ItNodeText)
   {
      c_Text += *c_ItNodeText;
   }

   c_Text += "</span></body></html>";
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Displays the read values

   Implemented steps: 6

   \param[in]  orc_DataPoolListsForEachNode  Result of step 5
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_PrepareConfirmStep(
   const std::vector<std::vector<C_OSCNodeDataPoolListId> > & orc_DataPoolListsForEachNode)
{
   const QString c_TextConfirm = this->m_GetTextForStep(orc_DataPoolListsForEachNode, true);

   this->mc_FinishedText = this->m_GetTextForStep(orc_DataPoolListsForEachNode, false);
   this->mpc_Ui->pc_TextBrowserConfirm->setHtml(c_TextConfirm);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Writes the CRC of relevant lists

   Implemented steps: 8
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_WriteCrcOfNodeToFile(void)
{
   sint32 s32_Return = C_NO_ERR;

   for (uint32 u32_ItNode = 0U; (u32_ItNode < this->mc_AllNodeIndexes.size()) && (s32_Return == C_NO_ERR); ++u32_ItNode)
   {
      s32_Return = this->mrc_ComDriver.NvmSafeUpdateCRCForFile(
         this->mc_AllNodeIndexes[u32_ItNode],
         this->m_GetPathForNode(this->mc_AllNodeIndexes[u32_ItNode], this->mc_FilePath));
   }

   if (s32_Return == C_NO_ERR)
   {
      this->mpc_Ui->pc_LabelFinished->setEnabled(true);
      this->mpc_Ui->pc_BushButtonOk->setVisible(true);
      this->mpc_Ui->pc_BushButtonCancel->setVisible(false);
      this->mpc_Ui->pc_ProgressValidateFile->SetProgress(100);
      this->mpc_Ui->pc_BopperleFinished->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
      this->mpc_Ui->pc_LabelHeadingSelectFile->setText(C_GtGetText::h_GetText("Finished"));
      this->mpc_Ui->pc_LabelStepDescription->setText(C_GtGetText::h_GetText(
                                                        "Parameter set file(s) recording finished successfully."));
      this->me_Step = eFINISHED;
   }
   else
   {
      QString c_ErrorText;

      switch (s32_Return)
      {
      case C_OVERFLOW:
         c_ErrorText = "Wrong sequence of function calls.";
         break;
      case C_RD_WR:
         c_ErrorText = "Specified file does not exist or "
                       "specified file is present but its structure is invalid (e.g. invalid XML file).";
         break;
      case C_RANGE:
         c_ErrorText = "Path does not match the path of the preceding function calls.";
         break;
      default:
         c_ErrorText = QString("Validating file failed with unknown reason (%1).").arg(s32_Return);
         break;
      }

      this->m_ReportError("NvmSafeUpdateCRCForFile", c_ErrorText, s32_Return);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_OkClicked(void)
{
   this->mpc_ParentDialog->accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_OnCancel(void)
{
   if ((this->me_Step == eCREATEPARAMETERSETFILE) ||
       (this->me_Step == eREADBACKFROMFILE) ||
       (this->me_Step == eCONFIRM) ||
       (this->me_Step == eWRITECRC))
   {
      // ask the user to continue
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Parameter Set Record interrupt"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to interrupt the process? \n"
                                                      "The unfinished file(s) will be deleted."));
      c_Message.SetNOButtonText("Interrupt");
      c_Message.SetOKButtonText("Don't Interrupt");
      c_Message.SetCustomMinHeight(180, 180);
      C_OgeWiCustomMessage::E_Outputs e_Output;
      e_Output = c_Message.Execute();
      if (e_Output == C_OgeWiCustomMessage::eNO)
      {
         this->mpc_ParentDialog->reject();
         // Delete the not finished and not valid file
         if (this->mc_AllNodeIndexes.size() == 1UL)
         {
            QFile c_File;

            c_File.setFileName(this->mc_FilePath);
            c_File.remove();
         }
         else
         {
            for (uint32 u32_ItNodes = 0UL; u32_ItNodes < this->mc_AllNodeIndexes.size(); ++u32_ItNodes)
            {
               const QString c_FullPath = this->m_GetPathForNode(this->mc_AllNodeIndexes[u32_ItNodes],
                                                                 this->mc_FilePath);
               QFile c_File;

               c_File.setFileName(c_FullPath);
               c_File.remove();
            }
         }
      }
   }
   else
   {
      this->mpc_ParentDialog->reject();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change of confirm status of correctness of the NVM lists

   Implemented steps: 7
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_ConfirmCheckBoxChanged(void) const
{
   this->mpc_Ui->pc_PbConfirm->setEnabled(this->mpc_Ui->pc_CbConfirm->isChecked());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   User confirms the correctness of the NVM lists

   Implemented steps: 7
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_ConfirmClicked(void)
{
   if (this->me_Step == eSELECTFILE)
   {
      m_ReadClicked();
   }
   if (this->me_Step == eCONFIRM)
   {
      if (this->mpc_Ui->pc_CbConfirm->isChecked() == true)
      {
         this->mpc_Ui->pc_ProgressConfirm->SetProgress(100);
         this->mpc_Ui->pc_PbConfirm->setVisible(false);
         this->mpc_Ui->pc_CbConfirm->setVisible(false);
         this->mpc_Ui->pc_LabelValidateFile->setEnabled(true);
         this->mpc_Ui->pc_TextBrowserConfirm->setHtml(this->mc_FinishedText);
         this->mpc_Ui->pc_BopperleValidateFile->SetMainBopperleColor(mc_STYLE_GUIDE_COLOR_21, mc_STYLE_GUIDE_COLOR_13);
         this->mpc_Ui->pc_LabelHeadingSelectFile->setText(C_GtGetText::h_GetText("Validate File(s)"));
         this->mpc_Ui->pc_LabelStepDescription->setText(C_GtGetText::h_GetText("Validating parameter set file(s)..."));
         this->me_Step = eWRITECRC;

         // Step 8
         this->m_WriteCrcOfNodeToFile();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore user settings value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_LoadUserSettings(void)
{
   const C_UsSystemView c_View = C_UsHandler::h_GetInstance()->GetProjSvSetupView(this->mc_ViewName);
   QString c_Name;
   QString c_Path;

   if (c_View.c_ParamRecordFileName.compare("") == 0)
   {
      c_Name = "ParameterSetImage" + mhc_FILE_EXTENSION;
   }
   else
   {
      c_Name = c_View.c_ParamRecordFileName;
   }

   if (c_Name.startsWith("/") == true)
   {
      // remove first "/"
      c_Name = c_Name.remove(0, 1);
   }

   c_Path = C_Uti::h_CheckAndReplaceWithExePathIfNecessary(c_View.c_ParamRecordPath);

   if (c_Path.endsWith("/") == false)
   {
      c_Path = c_Path + "/";
   }

   this->mpc_Ui->pc_LineEditPath->SetPath(c_Path + c_Name);
   this->mc_FilePath = C_Uti::h_GetAbsolutePathFromExe(c_Path + c_Name);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_Timer(void)
{
   if (this->me_Step == eCREATEPARAMETERSETFILE)
   {
      this->m_ReadElementsOfNode(this->mpc_Ui->pc_CommentText->toPlainText());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_ReportError(const QString & orc_FunctionName, const QString & orc_ErrorText,
                                                 const stw_types::sint32 os32_ErrorCode)
{
   const QString c_Text = "Function " + orc_FunctionName +
                          " ended with error code \"" + C_Uti::h_StwError(os32_ErrorCode) + "\"";

   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);

   osc_write_log_info("Write NVM parameters", c_Text.toStdString().c_str());

   c_Message.SetDescription(QString(C_GtGetText::h_GetText("Function %1 ended with error.")).arg(orc_FunctionName));
   c_Message.SetDetails(QString(C_GtGetText::h_GetText("Error code:\n %1 \nError text: \n %2"))
                        .arg(os32_ErrorCode).arg(orc_ErrorText));
   c_Message.SetCustomMinHeight(180, 350);
   c_Message.Execute();

   // Close dialog on error
   this->m_OnCancel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error for function "NvmSafeReadParameterValues"

   \param[in]  os32_ErrorCode    Function result
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImageRecordWidget::m_ReportErrorNvmSafeReadParameterValues(const sint32 os32_ErrorCode)
{
   const QString c_Log = C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str();
   uint8 u8_NRC;
   QString c_Details = "";
   QString c_Description;
   C_OgeWiCustomMessage c_Message(this);
   sint32 s32_Return = this->mrc_ComDriver.GetPollResultNRC(u8_NRC);

   tgl_assert(s32_Return == C_NO_ERR);

   switch (os32_ErrorCode)
   {
   case C_NO_ERR:
      c_Description += "Data prepared for file";
      break;
   case C_OVERFLOW:
      c_Description += "Wrong sequence of function calls";
      break;
   case C_RANGE:
      c_Description += "Datapool list IDs invalid";
      break;
   case C_CHECKSUM:
      c_Description += "CRC over the values of a parameter list read from the ECU does not match those values";
      break;
   case C_CONFIG:
      c_Description += "No valid diagnostic protocol is set";
      c_Description += "or no valid pointer to the original instance of \"C_OSCNode\" is set in \"C_OSCDataDealer\"";
      break;
   case C_NOACT:
      c_Description += "Server communication protocol service could not be requested";
      break;
   case C_TIMEOUT:
      c_Description += "Server communication protocol service has timed out";
      break;
   case C_WARN:
      c_Description += "Server communication protocol service error response was received";

      // Using <br/> as new line symbol instead of \n due to problems with the h_GetLink syntax
      switch (u8_NRC)
      {
      case 0x13:
         c_Details = C_GtGetText::h_GetText("Incorrect length of request<br/>");
         break;
      case 0x31:
         c_Details = C_GtGetText::h_GetText("Address or length format invalid (> 4 bytes)<br/>"
                                            "Requested memory range specified by address and size invalid<br/>");
         break;
      case 0x72:
         c_Details = C_GtGetText::h_GetText("Server reading NVM failed<br/>");
         break;
      case 0x33:
         c_Details = C_GtGetText::h_GetText("Required security level was not unlocked<br/>");
         break;
      case 0x14:
         c_Details =
            C_GtGetText::h_GetText("The total length of the response message exceeds the available buffer size<br/>");
         break;
      case 0x7F:
         c_Details =
            C_GtGetText::h_GetText("Server is not in the correct diagnostic session<br/>");
         break;
      default:
         c_Details = QString(C_GtGetText::h_GetText("Unknown NRC: 0x%1<br/>")).arg(QString::number(u8_NRC, 16));
         break;
      }
      break;
   case C_COM:
      c_Description += "Communication driver reported error";
      break;
   default:
      //Should not happen
      c_Description += "Unknown";
      break;
   }

   C_OSCLoggingHandler::h_Flush();
   c_Details += QString(C_GtGetText::h_GetText("See log file for details: ")) +
                C_Uti::h_GetLink(c_Log, mc_STYLESHEET_GUIDE_COLOR_LINK, c_Log);

   c_Message.SetType(C_OgeWiCustomMessage::eERROR);
   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(230, 300);
   c_Message.Execute();
   // Close dialog on error (WITHOUT user confirmation)
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt the base path for this node as necessary

   \param[in]  ou32_NodeIndex    Node index (which node is this file for)
   \param[in]  orc_Path          Base path to use as base and fallback in special cases

   \return
   The adapted path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaImageRecordWidget::m_GetPathForNode(const uint32 ou32_NodeIndex, const QString & orc_Path) const
{
   QString c_Retval = orc_Path;

   //Only use special handling for recording more than one node
   if (this->mc_AllNodeIndexes.size() > 1UL)
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         const QFileInfo c_FileInfo(orc_Path);
         const QDir c_Dir(c_FileInfo.absoluteDir());
         const QString c_NodeFileNameBase = C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(
            pc_Node->c_Properties.c_Name).c_str();
         const QString c_NodeFileName = c_FileInfo.completeBaseName() + "_" + c_NodeFileNameBase + "." +
                                        c_FileInfo.suffix();
         c_Retval = c_Dir.absoluteFilePath(c_NodeFileName);
      }
   }
   return c_Retval;
}
