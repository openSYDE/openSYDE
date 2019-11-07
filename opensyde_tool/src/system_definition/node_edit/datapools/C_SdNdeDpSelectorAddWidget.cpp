//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for adding a new Datapool and configuring if it is a stand alone or shared Datapool

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeDpSelectorAddWidget.h"
#include "ui_C_SdNdeDpSelectorAddWidget.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
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

   \param[in,out] orc_Parent           Reference to parent
   \param[in]     ou32_NodeIndex       Node index
   \param[in,out] opc_OSCDataPool      Reference to the actual core datapool object
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorAddWidget::C_SdNdeDpSelectorAddWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 ou32_NodeIndex,
   C_OSCNodeDataPool & orc_OSCDataPool) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDpSelectorAddWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mrc_OSCDataPool(orc_OSCDataPool)
{
   this->mpc_Ui->setupUi(this);

   // Init widget
   InitStaticNames();
   this->m_InitFromData();
   this->m_OnStandAloneChange();
   this->m_OnSharedDataPoolChanged();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDpSelectorAddWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_SdNdeDpSelectorAddWidget::m_CancelClicked);

   connect(this->mpc_Ui->pc_RadioButtonStandAlone, &stw_opensyde_gui_elements::C_OgeRabProperties::toggled, this,
           &C_SdNdeDpSelectorAddWidget::m_OnStandAloneChange);
   connect(this->mpc_Ui->pc_RadioButtonShared, &stw_opensyde_gui_elements::C_OgeRabProperties::toggled, this,
           &C_SdNdeDpSelectorAddWidget::m_OnSharedChanged);

   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_ComboBoxSharedDatapool,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdNdeDpSelectorAddWidget::m_OnSharedDataPoolChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpSelectorAddWidget::~C_SdNdeDpSelectorAddWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Datapools"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Add new Datapool"));
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Select Type"));

   this->mpc_Ui->pc_RadioButtonStandAlone->setText(C_GtGetText::h_GetText("Stand-alone Datapool"));
   this->mpc_Ui->pc_RadioButtonShared->setText(C_GtGetText::h_GetText("Shared Datapool"));

   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelSharedDatapool->setText(C_GtGetText::h_GetText("Share with"));
   this->mpc_Ui->pc_LabelSharedDatapoolInfo->setText(C_GtGetText::h_GetText("Already shared with:"));

   //tooltips
   this->mpc_Ui->pc_RadioButtonStandAlone->SetToolTipInformation(C_GtGetText::h_GetText("Stand-alone Datapool"),
                                                                 C_GtGetText::h_GetText(
                                                                    "Default type. A Datapool without relationship to other Datapools."));

   this->mpc_Ui->pc_RadioButtonShared->SetToolTipInformation(C_GtGetText::h_GetText("Shared Datapool"),
                                                             C_GtGetText::h_GetText(
                                                                "A Datapool with relationship to other Datapools.\n"
                                                                "Datapool configuration and properties are synchronized"
                                                                "within shared Datapools."));

   this->mpc_Ui->pc_LabelSharedDatapool->SetToolTipInformation(C_GtGetText::h_GetText("Share with"),
                                                               C_GtGetText::h_GetText("Select share partner Datapool."));

   this->mpc_Ui->pc_LabelSharedDatapoolInfo->SetToolTipInformation(C_GtGetText::h_GetText("Already shared with"),
                                                                   C_GtGetText::h_GetText(
                                                                      "List of Datapools which are already shared with "
                                                                      "selected shared partner Datapool."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the result of the dialog if a shared datapool is selected

   \param[out]    orc_SharedDatapoolId      Datapool ID of selected shared datapool partner to the new datapool
                                            Will be filled if at least one shareable datapool is available

   \return
   true     A shared datapool was selected
   false    No shared datapool was selected. Stand alone datapool.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpSelectorAddWidget::GetSelectedSharedDatapool(C_OSCNodeDataPoolId & orc_SharedDatapoolId) const
{
   const bool q_Return = this->mpc_Ui->pc_RadioButtonShared->isChecked();

   if (this->mpc_Ui->pc_ComboBoxSharedDatapool->count() > 0)
   {
      const std::map<QString, stw_opensyde_core::C_OSCNodeDataPoolId>::const_iterator c_ItDatapool =
         this->mc_AvailableDatapools.find(this->mpc_Ui->pc_ComboBoxSharedDatapool->currentText());

      tgl_assert(c_ItDatapool != this->mc_AvailableDatapools.end());
      if (c_ItDatapool != this->mc_AvailableDatapools.end())
      {
         orc_SharedDatapoolId = c_ItDatapool->second;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief   Init dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::m_InitFromData(void)
{
   // Fill the combo box with all datapools of the same type beginning with datapools
   // of the same node
   const C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   uint32 u32_DatapoolCounter;
   uint32 u32_NodeCounter;
   C_OSCNodeDataPoolId c_DatapoolId;

   // Add datapools of current node
   if (pc_Node != NULL)
   {
      c_DatapoolId.u32_NodeIndex = this->mu32_NodeIndex;

      for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < pc_Node->c_DataPools.size(); ++u32_DatapoolCounter)
      {
         const C_OSCNodeDataPool & rc_Datapool = pc_Node->c_DataPools[u32_DatapoolCounter];

         if (rc_Datapool.e_Type == this->mrc_OSCDataPool.e_Type)
         {
            const QString c_DatapoolName = rc_Datapool.c_Name.c_str();
            this->mpc_Ui->pc_ComboBoxSharedDatapool->addItem(c_DatapoolName);

            // Save the unique identification for the datapool
            c_DatapoolId.u32_DataPoolIndex = u32_DatapoolCounter;
            this->mc_AvailableDatapools[c_DatapoolName] = c_DatapoolId;
         }
      }
   }

   // Add datapools of all other nodes
   for (u32_NodeCounter = 0U; u32_NodeCounter < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_NodeCounter)
   {
      if (u32_NodeCounter != this->mu32_NodeIndex)
      {
         pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeCounter);

         if (pc_Node != NULL)
         {
            const QString c_NodeName = QString(pc_Node->c_Properties.c_Name.c_str()) + "::";

            c_DatapoolId.u32_NodeIndex = u32_NodeCounter;

            for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < pc_Node->c_DataPools.size(); ++u32_DatapoolCounter)
            {
               const C_OSCNodeDataPool & rc_Datapool = pc_Node->c_DataPools[u32_DatapoolCounter];

               if (rc_Datapool.e_Type == this->mrc_OSCDataPool.e_Type)
               {
                  const QString c_Text = c_NodeName + QString(rc_Datapool.c_Name.c_str());
                  this->mpc_Ui->pc_ComboBoxSharedDatapool->addItem(c_Text);

                  // Save the unique identification for the datapool
                  c_DatapoolId.u32_DataPoolIndex = u32_DatapoolCounter;
                  this->mc_AvailableDatapools[c_Text] = c_DatapoolId;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for change to radio button Stand-alone
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::m_OnStandAloneChange(void) const
{
   this->mpc_Ui->pc_ComboBoxSharedDatapool->setEnabled(false);
   this->mpc_Ui->pc_LabelSharedDatapool->setEnabled(false);
   this->mpc_Ui->pc_LabelSharedDatapoolInfo->setEnabled(false);
   this->mpc_Ui->pc_ListWidgetSharedDatapoolInfo->setEnabled(false);

   if (this->mpc_Ui->pc_ComboBoxSharedDatapool->count() == 0)
   {
      this->mpc_Ui->pc_RadioButtonShared->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for change to radio button Share
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::m_OnSharedChanged(void) const
{
   this->mpc_Ui->pc_ComboBoxSharedDatapool->setEnabled(true);
   this->mpc_Ui->pc_ListWidgetSharedDatapoolInfo->setEnabled(true);
   this->mpc_Ui->pc_LabelSharedDatapool->setEnabled(true);
   this->mpc_Ui->pc_LabelSharedDatapoolInfo->setEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for change of the combo box with the selected shared datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpSelectorAddWidget::m_OnSharedDataPoolChanged(void) const
{
   const C_PuiSdSharedDatapools & rc_SharedDatapools = C_PuiSdHandler::h_GetInstance()->GetSharedDatapoolsConst();
   C_OSCNodeDataPoolId c_SelectedDatapoolId;
   uint32 u32_SharedGroup = 0U;

   // Remove previous results
   this->mpc_Ui->pc_ListWidgetSharedDatapoolInfo->clear();

   // Search for already shared datapools with the selected datapool and update the list
   this->GetSelectedSharedDatapool(c_SelectedDatapoolId);

   // c_SelectedDatapoolId is only valid if the combo box has at lest one entry
   if ((this->mpc_Ui->pc_ComboBoxSharedDatapool->count() > 0) &&
       (rc_SharedDatapools.IsSharedDatapool(c_SelectedDatapoolId, &u32_SharedGroup) == true))
   {
      // Datapool is already shared
      std::vector<QString> c_SharedDatapoolGroup;
      uint32 u32_DatapoolCounter;

      tgl_assert(C_SdNdeDpUtil::GetSharedDatapoolGroup(u32_SharedGroup, c_SelectedDatapoolId,
                                                             this->mu32_NodeIndex,
                                                             c_SharedDatapoolGroup) == C_NO_ERR);

      for (u32_DatapoolCounter = 0U; u32_DatapoolCounter < c_SharedDatapoolGroup.size(); ++u32_DatapoolCounter)
      {
         this->mpc_Ui->pc_ListWidgetSharedDatapoolInfo->addItem(c_SharedDatapoolGroup[u32_DatapoolCounter]);
      }
   }
}
