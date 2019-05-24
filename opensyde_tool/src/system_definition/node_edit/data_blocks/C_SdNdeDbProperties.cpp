//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new data block properties (implementation)

   Widget for new data block properties

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileDialog>

#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_ImpUtil.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDbProperties.h"
#include "C_OgeWiCustomMessage.h"
#include "ui_C_SdNdeDbProperties.h"
#include "C_SdNdeDbSelectDataPools.h"
#include "C_Uti.h"
#include "C_ImpUtil.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.
   Warning: uninitialized if os32_ApplicationIndex invalid

   \param[in]     ou32_NodeIndex        Node index
   \param[in]     os32_ApplicationIndex Application index
   \param[in,out] orc_Parent            Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbProperties::C_SdNdeDbProperties(const stw_types::uint32 ou32_NodeIndex, const sint32 os32_ApplicationIndex,
                                         stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbProperties),
   mu32_NodeIndex(ou32_NodeIndex),
   ms32_ApplicationIndex(os32_ApplicationIndex),
   me_Type(C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   //Configure
   this->mpc_Ui->pc_PushButtonClearProject->SetSvg("://images/system_views/IconClearAllEnabled.svg");
   this->mpc_Ui->pc_PushButtonAddDataPool->SetSvg("://images/IconAddEnabled.svg");
   this->mpc_Ui->pc_SpinBoxProcessID->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxProcessID->SetMaximumCustom(255);
   this->mpc_Ui->pc_ScrollAreaWidgetContents->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaDataPools->DeactivateScrollbarResize();

   //Remove debug labels
   this->mpc_Ui->pc_GroupBoxDataPoolsEmpty->setTitle("");
   this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setTitle("");

   InitStaticNames();
   if (os32_ApplicationIndex >= 0)
   {
      m_LoadData(ou32_NodeIndex, static_cast<uint32>(os32_ApplicationIndex));
   }

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonProject, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OnClickProject);
   connect(this->mpc_Ui->pc_PushButtonCodeGenerate, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_OnClickGenerate);
   connect(this->mpc_Ui->pc_PushButtonCodeGenerator, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_OnClickGenerator);
   connect(this->mpc_Ui->pc_PushButtonOutputFile, &QPushButton::clicked, this, &C_SdNdeDbProperties::m_OnClickOutput);
   connect(this->mpc_Ui->pc_PushButtonClearProject, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_OnClickClearProject);
   connect(this->mpc_Ui->pc_LineEditName, &C_OgeLePropertiesName::textChanged, this,
           &C_SdNdeDbProperties::m_OnNameEdited);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxProcessID, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNdeDbProperties::m_OnProcessIDChanged);
   connect(this->mpc_Ui->pc_PushButtonAddDataPool, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_HandleAddDataPools);
   connect(this->mpc_Ui->pc_PushButtonRevertToDefault, &QPushButton::clicked, this,
           &C_SdNdeDbProperties::m_HandleRevertCodeGenerator);

   // connect for updating paths that are relative to project path
   connect(this->mpc_Ui->pc_LineEditProject, &C_OgeLeFilePath::editingFinished,
           this, &C_SdNdeDbProperties::m_UpdatePathsRelativeToProject);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbProperties::~C_SdNdeDbProperties(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get window size for binary case

   \return
   Window size for binary case
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdNdeDbProperties::h_GetBinaryWindowSize(void)
{
   return QSize(900, 524);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get window size for default case

   \return
   Window size for default case
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SdNdeDbProperties::h_GetDefaultWindowSize(void)
{
   return QSize(900, 877);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Data Block"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelType->setText(C_GtGetText::h_GetText("Type"));
   this->mpc_Ui->pc_LabelHeadingProject->setText(C_GtGetText::h_GetText("Project"));
   this->mpc_Ui->pc_LabelHeadingProjectTools->setText(C_GtGetText::h_GetText("Project Tools"));
   this->mpc_Ui->pc_LabelProject->setText(C_GtGetText::h_GetText("Project Path"));
   this->mpc_Ui->pc_LabelProcessID->setText(C_GtGetText::h_GetText("Process ID"));
   this->mpc_Ui->pc_LabelOutputFile->setText(C_GtGetText::h_GetText("Output File"));
   this->mpc_Ui->pc_LabelIDE->setText(C_GtGetText::h_GetText("IDE Call"));
   this->mpc_Ui->pc_LabelCodeGenerator->setText(C_GtGetText::h_GetText("Code Generator"));
   this->mpc_Ui->pc_LabelCodeGenerate->setText(C_GtGetText::h_GetText("Gen. Code Directory"));
   this->mpc_Ui->pc_LabelHeadingDatapools->setText(C_GtGetText::h_GetText("Owned Datapools"));
   this->mpc_Ui->pc_LabelDataPoolsEmpty->setText("No assigned Datapools, \nadd any via the '+' button");
   this->mpc_Ui->pc_CommentText->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
   this->mpc_Ui->pc_PushButtonRevertToDefault->setText(C_GtGetText::h_GetText("Revert To Default"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   //Tool tips

   this->mpc_Ui->pc_LabelName->SetToolTipInformation(
      C_GtGetText::h_GetText("Name"),
      C_GtGetText::h_GetText("Symbolic Data Block name. Unique within node\n"
                             "\nFollowing C naming conventions are required:"
                             "\n - must not be empty"
                             "\n - only alphanumeric characters + \"_\""
                             "\n - should not be longer than 31 characters"));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this Data Block."));
   this->mpc_Ui->pc_LabelType->SetToolTipInformation(C_GtGetText::h_GetText("Type"),
                                                     C_GtGetText::h_GetText("Type of Data Block."));

   this->mpc_Ui->pc_LabelHeadingDatapools->SetToolTipInformation(
      C_GtGetText::h_GetText("Owned Datapools"),
      C_GtGetText::h_GetText("List of all Datapools which are mapped to this programmable application"
                             "\n(Relevant for code generation)."));

   this->mpc_Ui->pc_PushButtonAddDataPool->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                                 C_GtGetText::h_GetText(
                                                                    "Add Datapool(s) to list."));

   this->mpc_Ui->pc_PushButtonClearProject->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                                  C_GtGetText::h_GetText("Clear project definitions."));

   this->mpc_Ui->pc_LabelProcessID->SetToolTipInformation(
      C_GtGetText::h_GetText("Process ID"),
      C_GtGetText::h_GetText("Process ID of an application: Used to identify each application when more than one are "
                             "running on the same server.\nThis is required by remote Datapool configurations."));

   this->mpc_Ui->pc_LabelProject->SetToolTipInformation(
      C_GtGetText::h_GetText("Project Path"),
      C_GtGetText::h_GetText("Root directory of the target project. Absolute or relative to *.syde."));
   this->mpc_Ui->pc_LabelOutputFile->SetToolTipInformation(
      C_GtGetText::h_GetText("Output File"),
      C_GtGetText::h_GetText("Location of output file. Absolute or relative to project path."));
   this->mpc_Ui->pc_LabelCodeGenerate->SetToolTipInformation(
      C_GtGetText::h_GetText("Generated Code Directory"),
      C_GtGetText::h_GetText("Location of the directory for openSYDE generated code. "
                             "Absolute or relative to project path."));

   this->mpc_Ui->pc_LabelCodeGenerator->SetToolTipInformation(
      C_GtGetText::h_GetText("Code Generator"),
      C_GtGetText::h_GetText("Location of code generator executable (*.exe / *.bat). "
                             "Absolute or relative to openSYDE.exe."));

   this->mpc_Ui->pc_LabelIDE->SetToolTipInformation(
      C_GtGetText::h_GetText("IDE Call"),
      C_GtGetText::h_GetText("Command line IDE call. Absolute or relative to openSYDE.exe."));

   this->mpc_Ui->pc_PushButtonRevertToDefault->SetToolTipInformation(
      C_GtGetText::h_GetText(""),
      C_GtGetText::h_GetText("Set path to default openSYDE code generator (syde_coder_c)."));
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init from application

   Warning: should only be used in case of invalid application index

   \param[in] orc_Application Application to use for initialization
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::LoadFromData(const C_OSCNodeApplication & orc_Application)
{
   this->mc_SelectedDataPools.clear();
   m_LoadFromData(orc_Application);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply new data block properties

   \param[in,out] orc_Application Application to apply to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::ApplyNewData(C_OSCNodeApplication & orc_Application) const
{
   //Section 1
   orc_Application.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();
   orc_Application.c_Comment = this->mpc_Ui->pc_CommentText->toPlainText().toStdString().c_str();
   //Section 2
   orc_Application.c_ProjectPath = this->mpc_Ui->pc_LineEditProject->GetPath().toStdString().c_str();
   orc_Application.u8_ProcessId = static_cast<uint8>(this->mpc_Ui->pc_SpinBoxProcessID->value());
   orc_Application.c_ResultPath = this->mpc_Ui->pc_LineEditOutputFile->GetPath().toStdString().c_str();
   orc_Application.c_IDECall = this->mpc_Ui->pc_LineEditIDE->text().toStdString().c_str();
   orc_Application.c_CodeGeneratorPath =
      this->mpc_Ui->pc_LineEditCodeGenerator->GetPath().toStdString().c_str();
   orc_Application.c_GeneratePath = this->mpc_Ui->pc_LineEditCodeGenerate->GetPath().toStdString().c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data pools of application

   \param[in] ou32_ApplicationIndex Index of application to apply data pools to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::HandleDataPools(const uint32 ou32_ApplicationIndex) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32 u32_ItDataPool = 0UL; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         bool q_Found = false;
         const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         for (uint32 u32_ItSelect = 0UL; u32_ItSelect < this->mc_SelectedDataPools.size(); ++u32_ItSelect)
         {
            if (this->mc_SelectedDataPools[u32_ItSelect] == u32_ItDataPool)
            {
               q_Found = true;
            }
         }
         //Hint: these operations should not change the vector so no reload of nodes necessary
         if (q_Found == true)
         {
            //Always assign this one
            tgl_assert(C_PuiSdHandler::h_GetInstance()->AssignDataPoolApplication(this->mu32_NodeIndex, u32_ItDataPool,
                                                                                  static_cast<sint32>(
                                                                                     ou32_ApplicationIndex)) ==
                       C_NO_ERR);
         }
         else
         {
            if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
                (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationIndex))
            {
               //Unassign this one
               tgl_assert(C_PuiSdHandler::h_GetInstance()->AssignDataPoolApplication(this->mu32_NodeIndex,
                                                                                     u32_ItDataPool,
                                                                                     -1) == C_NO_ERR);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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

   Only allow OK if application name and process ID are valid, if process ID error: list existing ones!
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OkClicked(void)
{
   const QString c_ErrorName = this->m_CheckName();
   const QString c_ErrorID = this->m_CheckID();

   const bool q_NameIsValid = (c_ErrorName == "");
   const bool q_IDIsValid = (c_ErrorID == "");

   if ((q_NameIsValid == true) && (q_IDIsValid == true))
   {
      this->mrc_ParentDialog.accept();
   }
   else
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      const QString c_Heading = C_GtGetText::h_GetText("Data Block Properties");
      QString c_Details;
      QString c_MessageText;
      if ((q_NameIsValid == false) && (q_IDIsValid == false))
      {
         c_MessageText =
            C_GtGetText::h_GetText("Name and Process ID are invalid! Choose valid name and ID.");
         c_Details = "- " + c_ErrorName + "- " + c_ErrorID;
      }
      else if (q_NameIsValid == false)
      {
         c_MessageText =
            C_GtGetText::h_GetText("Name is invalid! Choose a valid name.");
         c_Details = c_ErrorName;
      }
      else if (q_IDIsValid == false)
      {
         c_MessageText =
            C_GtGetText::h_GetText("Process ID is already used! Choose a unique Process ID.");
         c_Details = c_ErrorID;
      }
      else
      {
         // Nothing to do
      }

      c_Message.SetHeading(c_Heading);
      c_Message.SetDescription(c_MessageText);
      c_Message.SetDetails(c_Details);
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check data block name

   \return
   error text of name check (empty if no error occured)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_CheckName() const
{
   QString c_Return = "";

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Check Name
      bool q_DuplicateNameError = false;
      bool q_InvalidNameError =
         !C_OSCUtils::h_CheckValidCName(this->mpc_Ui->pc_LineEditName->text().toStdString().c_str());

      for (uint32 u32_ItApplication = 0UL; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
      {
         //Avoid conflict with itself
         if ((this->ms32_ApplicationIndex >= 0) &&
             (static_cast<uint32>(this->ms32_ApplicationIndex) == u32_ItApplication))
         {
            //Skip itself
         }
         else
         {
            const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApplication];
            //Check name
            if (this->mpc_Ui->pc_LineEditName->text().compare(rc_Application.c_Name.c_str()) == 0)
            {
               q_DuplicateNameError = true;
            }
         }
      }

      if (q_DuplicateNameError == true)
      {
         c_Return += C_GtGetText::h_GetText("Selected Data Block name is already in use.\n");
      }
      if (q_InvalidNameError == true)
      {
         c_Return +=
            C_GtGetText::h_GetText("Selected Data Block name is empty or contains invalid characters.\n");
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check process ID

   \return
   error text of process ID check (empty if no error occured)
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbProperties::m_CheckID(void) const
{
   QString c_Return = "";

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Check ID
      bool q_ProcessIDError = false;
      std::vector<uint32> c_UsedProcessIds;
      for (uint32 u32_ItApplication = 0UL; u32_ItApplication < pc_Node->c_Applications.size(); ++u32_ItApplication)
      {
         //Avoid conflict with itself
         if ((this->ms32_ApplicationIndex >= 0) &&
             (static_cast<uint32>(this->ms32_ApplicationIndex) == u32_ItApplication))
         {
            //Skip itself
         }
         else
         {
            const C_OSCNodeApplication & rc_Application = pc_Node->c_Applications[u32_ItApplication];
            //Only check programmable applications
            if (rc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
            {
               //Only check process IDs if current application is programmable
               if (this->me_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
               {
                  //Check process ID
                  c_UsedProcessIds.push_back(rc_Application.u8_ProcessId);
                  if (static_cast<uint8>(this->mpc_Ui->pc_SpinBoxProcessID->value()) == rc_Application.u8_ProcessId)
                  {
                     q_ProcessIDError = true;
                  }
               }
            }
         }
      }
      if (q_ProcessIDError == true)
      {
         if (c_UsedProcessIds.size() > 0UL)
         {
            QString c_Ids;
            for (uint32 u32_ItId = 0; u32_ItId < (c_UsedProcessIds.size() - 1UL); ++u32_ItId)
            {
               c_Ids += QString("%1,").arg(c_UsedProcessIds[u32_ItId]);
            }
            c_Ids +=
               QString::number(c_UsedProcessIds[static_cast<std::vector<uint32>::size_type>(c_UsedProcessIds.size() -
                                                                                            1UL)]);
            c_Return += QString(C_GtGetText::h_GetText("Already used Process IDs: %1")).arg(c_Ids);
         }
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load from specified data block

   \param[in] ou32_NodeIndex        Node index
   \param[in] ou32_ApplicationIndex Application index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_LoadData(const stw_types::uint32 ou32_NodeIndex,
                                     const stw_types::uint32 ou32_ApplicationIndex)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   const C_OSCNodeApplication * const pc_Appl = C_PuiSdHandler::h_GetInstance()->GetApplication(ou32_NodeIndex,
                                                                                                ou32_ApplicationIndex);

   if ((pc_Appl != NULL) && (pc_Node != NULL))
   {
      //Add existing selected datapools
      this->mc_SelectedDataPools.clear();
      for (uint32 u32_ItDataPool = 0UL; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == ou32_ApplicationIndex))
         {
            this->mc_SelectedDataPools.push_back(u32_ItDataPool);
         }
      }
      m_LoadFromData(*pc_Appl);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load from specified data block

   \param[in] orc_Application Application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_LoadFromData(const C_OSCNodeApplication & orc_Application)
{
   //Section 1
   this->mpc_Ui->pc_LineEditName->setText(orc_Application.c_Name.c_str());
   this->mpc_Ui->pc_CommentText->setText(orc_Application.c_Comment.c_str());
   this->me_Type = orc_Application.e_Type;
   if (orc_Application.e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
   {
      this->mpc_Ui->pc_GroupBoxProject->setVisible(true);
      this->m_SetVisibilityOfProgrammableLines(true);
      this->mpc_Ui->pc_GroupBoxDatapools->setVisible(true);
   }
   else if (orc_Application.e_Type == C_OSCNodeApplication::eBINARY)
   {
      // first hide lines, then set group box visible
      this->m_SetVisibilityOfProgrammableLines(false);
      this->mpc_Ui->pc_GroupBoxProject->setVisible(true);
      this->mpc_Ui->pc_GroupBoxDatapools->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxProject->setVisible(false);
      this->mpc_Ui->pc_GroupBoxDatapools->setVisible(false);
   }
   switch (orc_Application.e_Type)
   {
   case C_OSCNodeApplication::eBINARY:
      this->mpc_Ui->pc_LabelTypeValue->setText(C_GtGetText::h_GetText("Binary"));
      break;
   case C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION:
      this->mpc_Ui->pc_LabelTypeValue->setText(C_GtGetText::h_GetText("Programmable application"));
      break;
   }
   //Section 2 (order is relevant: first set project path and then output file and code generate directory)
   this->mpc_Ui->pc_LineEditProject->SetPath(orc_Application.c_ProjectPath.c_str(),
                                             C_PuiProject::h_GetInstance()->GetFolderPath());

   const QString c_ProjectPath =
      C_ImpUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   this->mpc_Ui->pc_SpinBoxProcessID->setValue(orc_Application.u8_ProcessId);
   this->mpc_Ui->pc_LineEditOutputFile->SetPath(orc_Application.c_ResultPath.c_str(), c_ProjectPath);
   this->mpc_Ui->pc_LineEditIDE->setText(orc_Application.c_IDECall.c_str());
   this->mpc_Ui->pc_LineEditCodeGenerator->SetPath(orc_Application.c_CodeGeneratorPath.c_str(), C_Uti::h_GetExePath());
   this->mpc_Ui->pc_LineEditCodeGenerate->SetPath(orc_Application.c_GeneratePath.c_str(), c_ProjectPath);
   //Section 3
   m_InitDataPoolsSection();

   // initial check
   this->m_CheckName();
   this->m_CheckID();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set visibility of properties that are different for programmable and binary applications

   long description of function within several lines

   \param[in]     orq_Visible    true: show  false: hide

   \return
   possible return value(s) and description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_SetVisibilityOfProgrammableLines(const bool & orq_Visible) const
{
   // process ID
   this->mpc_Ui->pc_LabelProcessID->setVisible(orq_Visible);
   this->mpc_Ui->pc_SpinBoxProcessID->setVisible(orq_Visible);

   // Generated Code Folder
   this->mpc_Ui->pc_LabelCodeGenerate->setVisible(orq_Visible);
   this->mpc_Ui->pc_LineEditCodeGenerate->setVisible(orq_Visible);
   this->mpc_Ui->pc_PushButtonCodeGenerate->setVisible(orq_Visible);

   // Code Generator
   this->mpc_Ui->pc_LabelCodeGenerator->setVisible(orq_Visible);
   this->mpc_Ui->pc_LineEditCodeGenerator->setVisible(orq_Visible);
   this->mpc_Ui->pc_PushButtonCodeGenerator->setVisible(orq_Visible);
   this->mpc_Ui->pc_PushButtonRevertToDefault->setVisible(orq_Visible);

   // Remove vertical spacers
   if ((orq_Visible == false))
   {
      if (this->mpc_Ui->pc_SpacerProject != NULL)
      {
         this->mpc_Ui->pc_GridLayout->removeItem(this->mpc_Ui->pc_SpacerProject);
         delete this->mpc_Ui->pc_SpacerProject;
      }
      if (this->mpc_Ui->pc_SpacerTools != NULL)
      {
         this->mpc_Ui->pc_GridLayout->removeItem(this->mpc_Ui->pc_SpacerTools);
         delete this->mpc_Ui->pc_SpacerTools;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Project Path" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickProject(void)
{
   QString c_FolderName; // for default folder

   const QDir c_Folder(C_ImpUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath()));

   if ((c_Folder.exists() == true) && (this->mpc_Ui->pc_LineEditProject->GetPath().isEmpty() == false))
   {
      c_FolderName = c_Folder.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Target Project Location"),
                                        c_FolderName, QFileDialog::ShowDirsOnly);

   if (c_Path != "")
   {
      // check if relative path is possible and appreciated
      c_Path = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_Path, C_PuiProject::h_GetInstance()->GetFolderPath());
      this->mpc_Ui->pc_LineEditProject->SetPath(c_Path, C_PuiProject::h_GetInstance()->GetFolderPath());

      // update tooltips of line edits with paths relative to project
      this->m_UpdatePathsRelativeToProject();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Output File" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickOutput(void)
{
   QString c_FolderName; // for default folder
   //Project path
   const QString c_ProjectPath =
      C_ImpUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   //Output file may be relative to project
   QString c_FilePath = this->mpc_Ui->pc_LineEditOutputFile->GetPath();
   const QDir c_ProjectDir(c_ProjectPath);
   const QFileInfo c_File(c_ProjectDir.absoluteFilePath(c_FilePath));
   const QString c_FilterName = QString(C_GtGetText::h_GetText("HEX file (*.hex);;Others (*.*)"));

   if (c_File.exists() == true)
   {
      c_FolderName = c_File.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   // do not use QFileDialog::getOpenFileName because it does not support default suffix
   QFileDialog c_Dialog(this,  C_GtGetText::h_GetText("Select Output File"), c_FolderName, c_FilterName);
   c_Dialog.setDefaultSuffix("hex");

   if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
   {
      c_FilePath = c_Dialog.selectedFiles().at(0); // multi-selection is not possible
      // check if relative path is possible and appreciated
      c_FilePath = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_FilePath, c_ProjectPath);
      this->mpc_Ui->pc_LineEditOutputFile->SetPath(c_FilePath, c_ProjectPath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Code Generator" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickGenerator(void)
{
   QString c_FolderName; // for default folder

   const QFileInfo c_File(this->mpc_Ui->pc_LineEditCodeGenerator->GetPath());

   if (c_File.exists() == true)
   {
      c_FolderName = c_File.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   const QString c_FilterName = QString(C_GtGetText::h_GetText("Executable (*.exe *.bat);;Others (*.*)"));
   QString c_FilePath = QFileDialog::getOpenFileName(this, C_GtGetText::h_GetText("Select Code Generator"),
                                                     c_FolderName, c_FilterName, NULL);

   if (c_FilePath != "")
   {
      // check if relative path is possible and appreciated
      c_FilePath = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_FilePath, C_Uti::h_GetExePath());
      this->mpc_Ui->pc_LineEditCodeGenerator->SetPath(c_FilePath, C_Uti::h_GetExePath());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on browse button for "Code Generator Output File" section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickGenerate(void)
{
   QString c_FolderName; // for default folder

   const QDir c_Folder(this->mpc_Ui->pc_LineEditCodeGenerate->GetPath());

   if ((c_Folder.exists() == true) && (this->mpc_Ui->pc_LineEditCodeGenerate->GetPath().isEmpty() == false))
   {
      c_FolderName = c_Folder.path();
   }
   else
   {
      c_FolderName = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   QString c_Path =
      QFileDialog::getExistingDirectory(this, C_GtGetText::h_GetText("Select Directory for Generated Code"),
                                        c_FolderName, QFileDialog::ShowDirsOnly);

   if (c_Path != "")
   {
      // check if relative path is possible and appreciated
      c_Path =
         C_ImpUtil::h_AskUserToSaveRelativePath(this, c_Path, this->mpc_Ui->pc_LineEditProject->GetPath());
      this->mpc_Ui->pc_LineEditCodeGenerate->SetPath(c_Path, this->mpc_Ui->pc_LineEditProject->GetPath());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle click on button "Clear project"
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnClickClearProject(void) const
{
   this->mpc_Ui->pc_LineEditProject->SetPath("", "");
   this->mpc_Ui->pc_SpinBoxProcessID->setValue(0);
   this->mpc_Ui->pc_LineEditOutputFile->SetPath("", "");
   this->mpc_Ui->pc_LineEditIDE->setText("");
   this->mpc_Ui->pc_LineEditCodeGenerator->SetPath("", "");
   this->mpc_Ui->pc_LineEditCodeGenerate->SetPath("", "");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for text changed signal of line edit name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnNameEdited(void) const
{
   const QString c_ErrorText = this->m_CheckName();
   const bool q_Error = (c_ErrorText == "");

   //set valid text property flag
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid", q_Error);

   if (q_Error)
   {
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Data Block Name");
      this->mpc_Ui->pc_LineEditName->SetToolTipInformation(c_Heading, c_ErrorText, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for value changed signal of process ID spin box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_OnProcessIDChanged(void) const
{
   const QString c_ErrorText = this->m_CheckID();
   const bool q_Error = (c_ErrorText == "");

   //set valid text property flag
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxProcessID, "Valid", q_Error);

   if (q_Error)
   {
      this->mpc_Ui->pc_SpinBoxProcessID->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxProcessID->SetToolTipAdditionalInfo(c_ErrorText, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up all data pool widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_CleanUpDataPoolWidgets(void)
{
   for (std::vector<C_SdNdeDbDataPoolEntry *>::const_iterator c_It = this->mc_DataPoolWidgets.begin();
        c_It != this->mc_DataPoolWidgets.end(); ++c_It)
   {
      C_SdNdeDbDataPoolEntry * const pc_Entry = *c_It;
      if (pc_Entry != NULL)
      {
         this->mpc_Ui->pc_VerticalLayoutDataPools->removeWidget(pc_Entry);
         pc_Entry->hide();
      }
   }
   this->mc_DataPoolWidgets.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update owned datapools count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_UpdateOwnedDpsCount(void) const
{
   //update owned dp count
   this->mpc_Ui->pc_LabelHeadingDatapools->setText(QString(C_GtGetText::h_GetText("Owned Datapools (%1)")).arg(
                                                      this->mc_SelectedDataPools.size()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update tool tips for paths that are relative to project path

   Slot for project path editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_UpdatePathsRelativeToProject() const
{
   const QString c_ProjectPath =
      C_ImpUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditProject->GetPath());
   QString c_Temp;

   // output file path
   c_Temp = this->mpc_Ui->pc_LineEditOutputFile->GetPath();
   this->mpc_Ui->pc_LineEditOutputFile->SetPath(c_Temp, c_ProjectPath);

   // generated code directory
   c_Temp = this->mpc_Ui->pc_LineEditCodeGenerate->GetPath();
   this->mpc_Ui->pc_LineEditCodeGenerate->SetPath(c_Temp, c_ProjectPath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize the data pools section (based on member variable)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_InitDataPoolsSection(void)
{
   m_CleanUpDataPoolWidgets();
   sint32 s32_Count = 0;
   for (std::vector<uint32>::const_iterator c_It = this->mc_SelectedDataPools.begin();
        c_It != this->mc_SelectedDataPools.end(); ++c_It)
   {
      const uint32 u32_Val = *c_It;
      C_SdNdeDbDataPoolEntry * const pc_Entry = new C_SdNdeDbDataPoolEntry(this->mu32_NodeIndex, u32_Val, this);

      connect(pc_Entry, &C_SdNdeDbDataPoolEntry::SigDeleteDataPool, this, &C_SdNdeDbProperties::m_HandleDeleteDataPool);

      this->mpc_Ui->pc_VerticalLayoutDataPools->insertWidget(s32_Count, pc_Entry);
      ++s32_Count;
      this->mc_DataPoolWidgets.push_back(pc_Entry);
   }
   if (this->mc_SelectedDataPools.size() > 0UL)
   {
      this->mpc_Ui->pc_GroupBoxDataPoolsEmpty->setVisible(false);
      this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxDataPoolsEmpty->setVisible(true);
      this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setVisible(false);
   }

   //update owned dp count
   this->m_UpdateOwnedDpsCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle add data pools operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_HandleAddDataPools(void)
{
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   const C_SdNdeDbSelectDataPools * const pc_Dialog = new C_SdNdeDbSelectDataPools(this->mu32_NodeIndex,
                                                                                   this->ms32_ApplicationIndex,
                                                                                   this->mc_SelectedDataPools, *c_New);

   //Resize
   c_New->SetSize(QSize(800, 800));

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_SelectedDataPools = pc_Dialog->GetSelectedDataPools();
      for (uint32 u32_It = 0UL; u32_It < c_SelectedDataPools.size(); ++u32_It)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_CurItem = c_SelectedDataPools[u32_It];
         this->mc_SelectedDataPools.push_back(rc_CurItem.u32_DataPoolIndex);
      }
      m_InitDataPoolsSection();
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle revert to default code generator action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_HandleRevertCodeGenerator(void) const
{
   this->mpc_Ui->pc_LineEditCodeGenerator->SetPath(C_ImpUtil::h_GetSydeCoderCPath(), C_Uti::h_GetExePath());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete datapool entry

   \param[in,out] opc_Source Source widget
   \param[in]     ou32_Index Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbProperties::m_HandleDeleteDataPool(C_SdNdeDbDataPoolEntry * const opc_Source, const uint32 ou32_Index)
{
   if (opc_Source != NULL)
   {
      for (std::vector<uint32>::iterator c_It = this->mc_SelectedDataPools.begin();
           c_It != this->mc_SelectedDataPools.end(); ++c_It)
      {
         if (*c_It == ou32_Index)
         {
            this->mc_SelectedDataPools.erase(c_It);
            break;
         }
      }
      for (std::vector<C_SdNdeDbDataPoolEntry *>::iterator c_It = this->mc_DataPoolWidgets.begin();
           c_It != this->mc_DataPoolWidgets.end(); ++c_It)
      {
         if (*c_It == opc_Source)
         {
            this->mc_DataPoolWidgets.erase(c_It);
            break;
         }
      }
      this->mpc_Ui->pc_VerticalLayoutDataPools->removeWidget(opc_Source);
      opc_Source->hide();
      if (this->mc_SelectedDataPools.size() == 0UL)
      {
         this->mpc_Ui->pc_GroupBoxDataPoolsEmpty->setVisible(true);
         this->mpc_Ui->pc_GroupBoxDataPoolsNotEmpty->setVisible(false);
      }
   }

   //update owned dp count
   this->m_UpdateOwnedDpsCount();
}
