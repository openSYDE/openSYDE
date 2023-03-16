//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an application in the application list (implementation)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_Uti.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiUtil.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDbWidget.hpp"
#include "C_OgePopUpDialog.hpp"
#include "ui_C_SdNdeDbWidget.h"
#include "C_PuiProject.hpp"
#include "constants.hpp"
#include "C_SdUtil.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SdNdeDbProperties.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_ImpUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::scl;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_ApplicationIndex  Application index
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbWidget::C_SdNdeDbWidget(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                                 QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbWidget),
   mu32_ApplicationIndex(ou32_ApplicationIndex),
   mu32_NodeIndex(ou32_NodeIndex)
{
   const QIcon c_IconError = QIcon("://images/Error_iconV2.svg");

   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // Configure
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "C_SdNdeDbWidget", true);
   this->mpc_Ui->pc_LabName->SetForegroundColor(4);
   this->mpc_Ui->pc_LabName->SetFontPixel(13);
   this->mpc_Ui->pc_LabFileGenActive->SetForegroundColor(4);
   this->mpc_Ui->pc_LabFileGenActive->SetFontPixel(13);
   this->mpc_Ui->pc_LabOwned->SetForegroundColor(6);
   this->mpc_Ui->pc_LabOwned->SetFontPixel(13);
   this->mpc_Ui->pc_LabOwnedCount->SetForegroundColor(6);
   this->mpc_Ui->pc_LabOwnedCount->SetFontPixel(13, false, true);
   this->mpc_Ui->pc_LabOutput->SetForegroundColor(6);
   this->mpc_Ui->pc_LabOutput->SetFontPixel(13);
   this->mpc_Ui->pc_LabOutputCount->SetForegroundColor(4);
   this->mpc_Ui->pc_LabOutputCount->SetFontPixel(13, false, true);
   this->mpc_Ui->pc_WiOutput->SetBackgroundColor(-1);
   this->mpc_Ui->pc_WiOwned->SetBackgroundColor(-1);
   this->mpc_Ui->pc_TedComment->setReadOnly(true);
   this->mpc_Ui->pc_PubComment->setCheckable(true);
   this->mpc_Ui->pc_PubComment->setChecked(true);
   this->m_OnCommentToggled(true);
   this->mpc_Ui->pc_PubComment->setVisible(false); // disable comment button because dynamic resize does not work

   //Remove debug string
   this->mpc_Ui->pc_PubEdit->setText("");
   this->mpc_Ui->pc_PubDelete->setText("");
   this->mpc_Ui->pc_PubOpenIde->setText("");
   this->mpc_Ui->pc_LabErrorIcon->setText("");
   this->mpc_Ui->pc_LabFileGenIcon->setText("");
   this->mpc_Ui->pc_LabOwnedIcon->setText("");
   this->mpc_Ui->pc_LabOutputIcon->setText("");

   //Icons
   const QSize c_ICON_SIZE(16, 16);
   this->mpc_Ui->pc_LabErrorIcon->setPixmap(c_IconError.pixmap(mc_ICON_SIZE_24));
   this->mpc_Ui->pc_PubEdit->setIconSize(c_ICON_SIZE);
   this->mpc_Ui->pc_PubEdit->SetCustomIcons("://images/IconEditSmallActive.svg", "://images/IconEditSmallActive.svg",
                                            "://images/IconEditSmallActive.svg", "://images/IconEditSmallActive.svg");
   this->mpc_Ui->pc_PubDelete->setIconSize(c_ICON_SIZE);
   this->mpc_Ui->pc_PubDelete->SetCustomIcons("://images/system_views/IconTabClose.svg",
                                              "://images/system_views/IconTabCloseHover.svg",
                                              "://images/IconCloseClicked.svg",
                                              "://images/IconCloseDisabled.svg");
   this->mpc_Ui->pc_PubOpenIde->setIconSize(c_ICON_SIZE);
   this->mpc_Ui->pc_PubOpenIde->SetCustomIcons("://images/system_definition/IconOpenExternalTool.svg",
                                               "://images/system_definition/IconOpenExternalTool.svg",
                                               "://images/system_definition/IconOpenExternalTool.svg",
                                               "://images/system_definition/IconOpenExternalTool.svg");
   // comment button: checked = comment visible = arrow up
   this->mpc_Ui->pc_PubComment->SetSvg("://images/system_definition/IconCommentDataBlockDown.svg", "", "",
                                       "://images/system_definition/IconCommentDataBlockUp.svg");
   this->mpc_Ui->pc_LabFileGenIcon->SetSvg("://images/system_definition/IconGenerateCodeSmall.svg");
   this->mpc_Ui->pc_LabOwnedIcon->SetSvg("://images/system_definition/IconDataPoolOwned.svg");
   this->mpc_Ui->pc_LabOutputIcon->SetSvg("://images/system_definition/IconOutputFile.svg");

   // Load data
   m_LoadData();

   //Visual connections
   connect(this->mpc_Ui->pc_PubEdit, &QPushButton::clicked, this, &C_SdNdeDbWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_PubDelete, &QPushButton::clicked, this, &C_SdNdeDbWidget::m_OnDelete);
   connect(this->mpc_Ui->pc_PubOpenIde, &QPushButton::clicked, this, &C_SdNdeDbWidget::m_OnOpenIdeClicked);
   connect(this->mpc_Ui->pc_PubComment, &QPushButton::toggled, this, &C_SdNdeDbWidget::m_OnCommentToggled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbWidget::~C_SdNdeDbWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabOwned->setText(C_GtGetText::h_GetText("Owned\nDatapools"));
   this->mpc_Ui->pc_LabOutput->setText(C_GtGetText::h_GetText("Output\nFiles"));

   //Tool tips
   this->mpc_Ui->pc_PubOpenIde->SetToolTipInformation(C_GtGetText::h_GetText("Open IDE"),
                                                      C_GtGetText::h_GetText(
                                                         "Execute IDE Call specified in project properties."));
   this->mpc_Ui->pc_PubEdit->SetToolTipInformation(C_GtGetText::h_GetText("Edit"),
                                                   C_GtGetText::h_GetText("Edit Data Block properties."));
   this->mpc_Ui->pc_PubDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                     C_GtGetText::h_GetText("Delete Data Block."));
   this->mpc_Ui->pc_PubComment->SetToolTipInformation(
      C_GtGetText::h_GetText("Comment"),
      C_GtGetText::h_GetText("Show or hide comment for this Data Block."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data pool section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::UpdateApplication(void)
{
   this->m_LoadData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the application index

   \param[in]  ou32_ApplicationIndex   New index of the application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::UpdateApplicationIndex(const uint32_t ou32_ApplicationIndex)
{
   this->mu32_ApplicationIndex = ou32_ApplicationIndex;
   this->m_LoadData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node index

   \return
   Current node index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDbWidget::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get application index

   \return
   Current application index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDbWidget::GetApplicationIndex(void) const
{
   return this->mu32_ApplicationIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check process ID value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::CheckProcessIdError(void) const
{
   QString c_Info;
   bool q_Valid = false;
   const std::vector<uint32_t> c_UsedIds = C_SdUtil::h_GetUsedProcessIdsForApplicationUniqueAndSortedAscending(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      tgl_assert(pc_Node->CheckApplicationProcessIdValid(this->mu32_ApplicationIndex, q_Valid) == C_NO_ERR);
      c_Info =
         C_SdUtil::h_InitUsedIdsString(c_UsedIds, pc_Node->c_Properties.c_Name.c_str(), C_GtGetText::h_GetText("node"));
   }
   this->mpc_Ui->pc_LabErrorIcon->setVisible(!q_Valid);
   if (q_Valid == false)
   {
      const QString c_Heading = C_GtGetText::h_GetText("Data block: Process ID invalid");
      this->mpc_Ui->pc_LabErrorIcon->SetToolTipInformation(c_Heading, c_Info, C_NagToolTip::eERROR);
   }
   else
   {
      this->mpc_Ui->pc_LabErrorIcon->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                           C_GtGetText::h_GetText(""),
                                                           C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden mouse double click event slot

   Here: Enter edit mode

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QWidget::mouseDoubleClickEvent(opc_Event);
   this->m_OnEdit();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load new data into UI
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::m_LoadData(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   const C_OscNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if ((pc_Application != NULL) && (pc_Node != NULL))
   {
      this->mpc_Ui->pc_LabName->setText(static_cast<QString>("#%1 - %2").
                                        arg(this->mu32_ApplicationIndex + 1).
                                        arg(pc_Application->c_Name.c_str()));
      if (pc_Application->c_Comment == "")
      {
         this->mpc_Ui->pc_TedComment->setText(C_GtGetText::h_GetText("<No comment>"));
      }
      else
      {
         this->mpc_Ui->pc_TedComment->setText(pc_Application->c_Comment.c_str());
      }

      if (pc_Application->e_Type == C_OscNodeApplication::eBINARY)
      {
         this->mpc_Ui->pc_LabFileGenActive->setText(C_GtGetText::h_GetText("File Generation: Disabled"));
         this->mpc_Ui->pc_LabFileGenActive->setDisabled(true);
         this->mpc_Ui->pc_LabOwned->setDisabled(true);
         this->mpc_Ui->pc_LabFileGenActive->SetForegroundColor(9);
         this->mpc_Ui->pc_LabOwned->SetForegroundColor(9);
         this->mpc_Ui->pc_LabOwnedCount->SetForegroundColor(9);
         this->mpc_Ui->pc_LabFileGenIcon->SetSvg("://images/system_definition/IconGenerateCodeSmallDisabled.svg");
         this->mpc_Ui->pc_LabOwnedIcon->SetSvg("://images/system_definition/IconDataPoolOwnedDisabled.svg");
      }
      else
      {
         if (pc_Application->e_Type == C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
         {
            this->mpc_Ui->pc_LabFileGenActive->setText(C_GtGetText::h_GetText("File Generation: Source Code"));
         }
         else
         {
            this->mpc_Ui->pc_LabFileGenActive->setText(C_GtGetText::h_GetText("File Generation: Parameter Set Image"));
         }
         this->mpc_Ui->pc_LabFileGenActive->setDisabled(false);
         this->mpc_Ui->pc_LabOwned->setDisabled(false);
         this->mpc_Ui->pc_LabFileGenActive->SetForegroundColor(6);
         this->mpc_Ui->pc_LabOwned->SetForegroundColor(6);
         this->mpc_Ui->pc_LabOwnedCount->SetForegroundColor(4);
         this->mpc_Ui->pc_LabFileGenIcon->SetSvg("://images/system_definition/IconGenerateCodeSmall.svg");
         this->mpc_Ui->pc_LabOwnedIcon->SetSvg("://images/system_definition/IconDataPoolOwned.svg");
      }

      // File count
      this->mpc_Ui->pc_LabOutputCount->setText(QString::number(pc_Application->c_ResultPaths.size()));
      this->mpc_Ui->pc_WiOutput->SetToolTipInformation(
         static_cast<QString>(C_GtGetText::h_GetText("Output Files (%1)")).arg(pc_Application->c_ResultPaths.size()),
         this->m_GetAllOutputFiles());

      // Datapool count
      const uint32_t u32_DpCount = this->m_CountAllAssociatedDataPools();
      this->mpc_Ui->pc_LabOwnedCount->setText(static_cast<QString>("%1").arg(u32_DpCount));
      if (u32_DpCount > 0)
      {
         this->mpc_Ui->pc_WiOwned->SetToolTipInformation(
            static_cast<QString>(C_GtGetText::h_GetText("Owned Datapools (%1)")).arg(u32_DpCount),
            this->m_GetAllAssociatedDataPoolNames());
      }

      //Initial error check
      CheckProcessIdError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On edit action: show properties dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnEdit(void)
{
   const C_OscNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if (pc_Application != NULL)
   {
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
      C_SdNdeDbProperties * const pc_Dialog = new C_SdNdeDbProperties(this->mu32_NodeIndex,
                                                                      this->mu32_ApplicationIndex, *c_New);

      //Resize
      if (pc_Application->e_Type != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         c_New->SetSize(C_SdNdeDbProperties::h_GetBinaryWindowSize());
      }
      else
      {
         c_New->SetSize(C_SdNdeDbProperties::h_GetDefaultWindowSize());
      }

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         C_OscNodeApplication c_Copy = *pc_Application;

         // get new data
         pc_Dialog->ApplyNewData(c_Copy);

         // update application and Datapools
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetApplication(this->mu32_NodeIndex, this->mu32_ApplicationIndex,
                                                                    c_Copy) == C_NO_ERR);
         pc_Dialog->HandleDataPools(this->mu32_ApplicationIndex);

         // Inform about change (this also triggers reload off all Data Block tiles including this one)
         Q_EMIT (this->SigOwnedDataPoolsChanged());
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On delete action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnDelete(void)
{
   bool q_DeletePoosible = false;

   if (m_GetType() != C_OscNodeApplication::ePROGRAMMABLE_APPLICATION)
   {
      q_DeletePoosible = true;
   }
   else
   {
      //More complex handling required for programmable applications
      //Check if the action does have critical impact
      if (m_CountAllAssociatedDataPools() > 0)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         //Detected change from programmable application to other type so ask user to confirm
         c_Message.SetHeading(C_GtGetText::h_GetText("Data Block delete"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Do you really want to delete this Data Block?\n"
                                     "All owned Datapools will become unassigned."));
         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
         c_Message.SetNoButtonText(C_GtGetText::h_GetText("Keep"));
         c_Message.SetCustomMinHeight(180, 180);
         if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
         {
            //Delete each associated data pool
            m_UnassignAllAssociatedDataPools();
            //Trigger application delete
            Q_EMIT this->SigDelete(this->mu32_NodeIndex, this->mu32_ApplicationIndex);
         }
         else
         {
            //No action requested
         }
      }
      else
      {
         q_DeletePoosible = true;
      }
   }
   if (q_DeletePoosible == true)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      //Detected change from programmable application to other type so ask user to confirm
      c_Message.SetHeading(C_GtGetText::h_GetText("Data Block delete "));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to delete this Data Block?"));
      c_Message.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
      c_Message.SetNoButtonText(C_GtGetText::h_GetText("Keep"));
      c_Message.SetCustomMinHeight(180, 180);
      if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
      {
         //Trigger application delete
         Q_EMIT this->SigDelete(this->mu32_NodeIndex, this->mu32_ApplicationIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger open IDE
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnOpenIdeClicked(void)
{
   const C_OscNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if (pc_Application != NULL)
   {
      const QString c_IdeCall =
         static_cast<QString>(C_PuiUtil::h_ResolvePlaceholderVariables(
                                 pc_Application->c_IdeCall.c_str(),
                                 C_PuiUtil::h_GetResolvedAbsPathFromProject(pc_Application->c_ProjectPath.c_str())));

      if (c_IdeCall == "")
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR,
                                           C_GtGetText::h_GetText("No IDE provided. Edit Data Block properties and "
                                                                  "insert an IDE call."));
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Open IDE"));
         c_MessageBox.SetCustomMinHeight(180, 180);
         c_MessageBox.Execute();
      }
      else if (C_ImpUtil::h_OpenIde(c_IdeCall) != C_NO_ERR)
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR,
                                           C_GtGetText::h_GetText("Could not start IDE. Possible reasons: "
                                                                  "Insufficient permissions or missing executable."));
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Open IDE"));
         c_MessageBox.SetDetails(static_cast<QString>(C_GtGetText::h_GetText(
                                                         "The following call returned an error: \n%1")).
                                 arg(c_IdeCall));
         c_MessageBox.SetCustomMinHeight(200, 270);
         c_MessageBox.Execute();
      }
      else
      {
         // Nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On comment toggled

   \param[in]  oq_Checked  Checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnCommentToggled(const bool oq_Checked)
{
   this->mpc_Ui->pc_TedComment->setVisible(oq_Checked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unassign all data pools associated to this application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbWidget::m_UnassignAllAssociatedDataPools(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) == this->mu32_ApplicationIndex))
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->UnassignDataPoolApplication(this->mu32_NodeIndex,
                                                                                    u32_ItDataPool) == C_NO_ERR);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current application type

   \return
   Current application type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeApplication::E_Type C_SdNdeDbWidget::m_GetType(void) const
{
   C_OscNodeApplication::E_Type e_Retval = C_OscNodeApplication::eBINARY;
   const C_OscNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if (pc_Application != NULL)
   {
      e_Retval = pc_Application->e_Type;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Count all data pools which are associated with this application

   \return
   Number of data pools which are associated with this application (0 in error case)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdNdeDbWidget::m_CountAllAssociatedDataPools(void) const
{
   uint32_t u32_Retval = 0;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) == this->mu32_ApplicationIndex))
         {
            ++u32_Retval;
         }
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all associated Datapool names for tooltip

   \return
   all associated Datapool names concatenated
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbWidget::m_GetAllAssociatedDataPoolNames(void) const
{
   QString c_Retval = "";

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OscNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32_t>(rc_DataPool.s32_RelatedDataBlockIndex) == this->mu32_ApplicationIndex))
         {
            c_Retval += rc_DataPool.c_Name.c_str();
            c_Retval += " (";
            c_Retval += C_PuiSdUtil::h_ConvertDataPoolTypeToString(rc_DataPool.e_Type);
            if (rc_DataPool.e_Type == C_OscNodeDataPool::eCOM)
            {
               c_Retval += ", ";
               c_Retval += C_GtGetText::h_GetText("Protocol: ");
               c_Retval += C_PuiSdUtil::h_ConvertProtocolTypeToString(C_PuiSdUtil::h_GetRelatedCanProtocolType(
                                                                         this->mu32_NodeIndex, u32_ItDataPool));
            }
            c_Retval += ")\n";
         }
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all output files for tooltip

   \return
   all output files concatenated
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDbWidget::m_GetAllOutputFiles(void) const
{
   QString c_Retval;

   const C_OscNodeApplication * const pc_Application =
      C_PuiSdHandler::h_GetInstance()->GetApplication(this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if (pc_Application != NULL)
   {
      for (uint32_t u32_ItOutputFiles = 0; u32_ItOutputFiles < pc_Application->c_ResultPaths.size();
           u32_ItOutputFiles++)
      {
         c_Retval += static_cast<QString>(C_GtGetText::h_GetText("Output File"));
         if (pc_Application->c_ResultPaths.size() > 1)
         {
            c_Retval += " ";
            c_Retval += QString::number(u32_ItOutputFiles + 1);
         }
         c_Retval += ": ";
         c_Retval += pc_Application->c_ResultPaths[u32_ItOutputFiles].c_str();
         c_Retval += "\n";
      }
   }

   return c_Retval;
}
