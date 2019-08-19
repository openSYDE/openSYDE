//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node to Node connection setup pop up dialog

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNodeToNodeConnectionSetupWidget.h"
#include "ui_C_SdNodeToNodeConnectionSetupWidget.h"
#include "constants.h"
#include "C_Uti.h"
#include "C_GtGetText.h"
#include "stwerrors.h"
#include "C_PuiSdHandler.h"
#include "TGLUtils.h"
#include "C_SdUtil.h"
#include "C_OgeWiUtil.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_types;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent       Reference to parent
   \param[in]     oru32_Node1Index Node 1 index
   \param[in]     oru32_Node2Index Node 2 index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNodeToNodeConnectionSetupWidget::C_SdNodeToNodeConnectionSetupWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 & oru32_Node1Index,
   const uint32 & oru32_Node2Index) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNodeToNodeConnectionSetupWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_Node1Index(oru32_Node1Index),
   mu32_Node2Index(oru32_Node2Index),
   mq_InteractionPossible(false),
   mc_BUS_TYPE_CAN(C_GtGetText::h_GetText("CAN")),
   mc_BUS_TYPE_ETHERNET(C_GtGetText::h_GetText("Ethernet"))
{
   // init UI
   this->mpc_Ui->setupUi(this);

   this->mrc_ParentDialog.SetWidget(this);

   InitStaticNames();
   m_InitFromData();

   //Remove debug string
   this->mpc_Ui->pc_GroupBoxInterfaces1->setTitle("");
   this->mpc_Ui->pc_GroupBoxInterfaces2->setTitle("");

   this->mpc_Ui->pc_SpinBoxNode1->SetMaximumCustom(C_SdUtil::h_GetNodeIdMaximum(this->mu32_Node1Index));
   this->mpc_Ui->pc_SpinBoxNode2->SetMaximumCustom(C_SdUtil::h_GetNodeIdMaximum(this->mu32_Node2Index));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SdNodeToNodeConnectionSetupWidget::m_CancelClicked);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_SpinBoxNode1, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNodeToNodeConnectionSetupWidget::m_CheckNodeIds);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_SpinBoxNode2, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNodeToNodeConnectionSetupWidget::m_CheckNodeIds);
   connect(this->mpc_Ui->pc_RadioButtonCreateNew, &stw_opensyde_gui_elements::C_OgeRabProperties::toggled, this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OnNewOrExistingChange);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_ComboBoxBusType, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_SdNodeToNodeConnectionSetupWidget::m_OnBusTypeChange);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_ComboBoxExistingBus,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OnExistingBusChange);
   connect(this->mpc_Ui->pc_LineEditBusName, &stw_opensyde_gui_elements::C_OgeLeProperties::textChanged, this,
           &C_SdNodeToNodeConnectionSetupWidget::m_OnBusNameChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNodeToNodeConnectionSetupWidget::~C_SdNodeToNodeConnectionSetupWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Node to Node"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Link Setup"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelNode1NodeIdDescription->setText(C_GtGetText::h_GetText("Node ID"));
   this->mpc_Ui->pc_LabelNode2NodeIdDescription->setText(C_GtGetText::h_GetText("Node ID"));
   this->mpc_Ui->pc_LabelInterfaceHeading->setText(C_GtGetText::h_GetText("2. Select COMM Interface"));
   this->mpc_Ui->pc_LabelNodeIdHeading->setText(C_GtGetText::h_GetText("3. Define Node ID"));
   this->mpc_Ui->pc_LabelBusHeading->setText(C_GtGetText::h_GetText("1. Select Bus"));
   this->mpc_Ui->pc_LabelBusType->setText(C_GtGetText::h_GetText("Type"));
   this->mpc_Ui->pc_LabelBusName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_RadioButtonCreateNew->setText(C_GtGetText::h_GetText("Create New Bus"));
   this->mpc_Ui->pc_RadioButtonSelectExisting->setText(C_GtGetText::h_GetText("Select Existing Bus"));
   this->mpc_Ui->pc_LabelProtocolsNode1->setText(C_GtGetText::h_GetText("Active Protocols"));
   this->mpc_Ui->pc_LabelProtocolsNode2->setText(C_GtGetText::h_GetText("Active Protocols"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNodeToNodeConnectionSetupWidget::GetSelectedInterface1(void) const
{
   return C_SdUtil::h_GetActiveNodeInterface(*this->mpc_Ui->pc_ComboBoxComIntf1, this->mu32_Node1Index,
                                             this->GetBusType());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node id

   \return
   Selected node id
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SdNodeToNodeConnectionSetupWidget::GetNodeId1(void) const
{
   return static_cast<uint8>(this->mpc_Ui->pc_SpinBoxNode1->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNodeToNodeConnectionSetupWidget::GetSelectedInterface2(void) const
{
   return C_SdUtil::h_GetActiveNodeInterface(*this->mpc_Ui->pc_ComboBoxComIntf2, this->mu32_Node2Index,
                                             this->GetBusType());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node id

   \return
   Selected node id
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SdNodeToNodeConnectionSetupWidget::GetNodeId2(void) const
{
   return static_cast<uint8>(this->mpc_Ui->pc_SpinBoxNode2->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the COM datapool configuration of the first node

   \param[out]    orq_ComProtocolL2          Flag if Layer 2 COM datapool exist
   \param[out]    orq_ComProtocolECeS        Flag if ECeS COM datapool exist
   \param[out]    orq_ComProtocolECoS        Flag if ECoS COM datapool exist
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::GetComDataPoolConfigurationNode1(bool & orq_ComProtocolL2,
                                                                           bool & orq_ComProtocolECeS,
                                                                           bool & orq_ComProtocolECoS) const
{
   orq_ComProtocolL2 = this->mpc_Ui->pc_CheckBoxLayer2Node1->isChecked();
   orq_ComProtocolECeS = this->mpc_Ui->pc_CheckBoxECeSNode1->isChecked();
   orq_ComProtocolECoS = this->mpc_Ui->pc_CheckBoxECoSNode1->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the COM datapool configuration of the second node

   \param[out]    orq_ComProtocolL2          Flag if Layer 2 COM datapool exist
   \param[out]    orq_ComProtocolECeS        Flag if ECeS COM datapool exist
   \param[out]    orq_ComProtocolECoS        Flag if ECoS COM datapool exist
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::GetComDataPoolConfigurationNode2(bool & orq_ComProtocolL2,
                                                                           bool & orq_ComProtocolECeS,
                                                                           bool & orq_ComProtocolECoS) const
{
   orq_ComProtocolL2 = this->mpc_Ui->pc_CheckBoxLayer2Node2->isChecked();
   orq_ComProtocolECeS = this->mpc_Ui->pc_CheckBoxECeSNode2->isChecked();
   orq_ComProtocolECoS = this->mpc_Ui->pc_CheckBoxECoSNode2->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current active bus type

   \return
   Current active bus type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSystemBus::E_Type C_SdNodeToNodeConnectionSetupWidget::GetBusType(void) const
{
   C_OSCSystemBus::E_Type e_Retval;

   if (this->mpc_Ui->pc_RadioButtonCreateNew->isChecked() == true)
   {
      if (this->mpc_Ui->pc_ComboBoxBusType->currentText() == this->mc_BUS_TYPE_ETHERNET)
      {
         e_Retval = C_OSCSystemBus::eETHERNET;
      }
      else
      {
         e_Retval = C_OSCSystemBus::eCAN;
      }
   }
   else
   {
      const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->GetBusIndex());
      tgl_assert(pc_Bus != NULL);
      if (pc_Bus != NULL)
      {
         e_Retval = pc_Bus->e_Type;
      }
      else
      {
         e_Retval = C_OSCSystemBus::eCAN;
      }
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current selected bus index

   Warning: Only use if user has selected "use existing bus" option

   \return
   Current selected bus index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNodeToNodeConnectionSetupWidget::GetBusIndex(void) const
{
   uint32 u32_Retval = static_cast<uint32>(this->mpc_Ui->pc_ComboBoxExistingBus->currentIndex());

   if (u32_Retval < this->mc_BusIndices.size())
   {
      u32_Retval = this->mc_BusIndices[u32_Retval];
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current bus name

   Warning: Only use if user has selected "use existing bus" option

   \return
   Current bus name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNodeToNodeConnectionSetupWidget::GetBusName(void) const
{
   return this->mpc_Ui->pc_LineEditBusName->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if creation of new bus requested

   \return
   True  Create new
   False Use existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::CheckIfCreateNew(void) const
{
   return this->mpc_Ui->pc_RadioButtonCreateNew->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interaction possible flag

   \return
   True  Interaction possible
   False Invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::GetInteractionPossible(void) const
{
   return this->mq_InteractionPossible;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OkClicked(void)
{
   if (m_CheckDatapoolNumberNodes() == true)
   {
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_InitFromData(void)
{
   const C_OSCNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node1Index);
   const C_OSCNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node2Index);

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      bool q_NewBusPossible;
      bool q_ExistingBusExists;
      bool q_NewBusRestricted;
      bool q_ExistingBusRestricted;
      stw_opensyde_core::C_OSCSystemBus::E_Type e_NewBusRestrictedType;
      stw_opensyde_core::C_OSCSystemBus::E_Type e_ExistingBusRestrictedType;

      this->m_CheckTypeRestrictions(q_NewBusPossible,  q_NewBusRestricted, e_NewBusRestrictedType, q_ExistingBusExists,
                                    q_ExistingBusRestricted,
                                    e_ExistingBusRestrictedType);

      this->mq_InteractionPossible = q_NewBusPossible || q_ExistingBusExists;

      // If no interaction is possible, this dialog will not be shown
      if (this->mq_InteractionPossible == true)
      {
         // Adapt enable states if necessary
         if (q_NewBusPossible == true)
         {
            C_OSCSystemBus c_OSCBus;

            if (q_ExistingBusExists == false)
            {
               this->mpc_Ui->pc_RadioButtonSelectExisting->setEnabled(false);
            }

            //Init name
            this->mpc_Ui->pc_LineEditBusName->setText(C_Uti::h_GetUniqueName(C_PuiSdHandler::h_GetInstance()->
                                                                             GetExistingBusNames(),
                                                                             c_OSCBus.c_Name).c_str());
         }
         else
         {
            // Only a connect to an already existing bus is possible. No new bus can be created, the target node
            // has no further free interfaces
            this->mpc_Ui->pc_RadioButtonCreateNew->setEnabled(false);
            this->mpc_Ui->pc_ComboBoxExistingBus->setEnabled(true);
            this->mpc_Ui->pc_LabelBusType->setEnabled(false);
            this->mpc_Ui->pc_ComboBoxBusType->setEnabled(false);
            this->mpc_Ui->pc_LabelBusName->setEnabled(false);
            this->mpc_Ui->pc_LineEditBusName->setEnabled(false);
            this->mpc_Ui->pc_RadioButtonSelectExisting->setChecked(true);
         }

         // Combo box for bus type
         this->mpc_Ui->pc_ComboBoxBusType->clear();
         if (q_NewBusPossible == true)
         {
            if (q_NewBusRestricted == true)
            {
               if (e_NewBusRestrictedType == C_OSCSystemBus::eCAN)
               {
                  this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BUS_TYPE_CAN);
               }
               else
               {
                  this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BUS_TYPE_ETHERNET);
               }
            }
            else
            {
               this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BUS_TYPE_CAN);
               this->mpc_Ui->pc_ComboBoxBusType->addItem(this->mc_BUS_TYPE_ETHERNET);
            }
         }

         //Dynamic labels
         this->mpc_Ui->pc_LabelNode1Interfaces->setText(pc_Node1->c_Properties.c_Name.c_str());
         this->mpc_Ui->pc_LabelNode2Interfaces->setText(pc_Node2->c_Properties.c_Name.c_str());
         this->mpc_Ui->pc_LabelNode1NodeId->setText(pc_Node1->c_Properties.c_Name.c_str());
         this->mpc_Ui->pc_LabelNode2NodeId->setText(pc_Node2->c_Properties.c_Name.c_str());

         //Existing buses
         this->mpc_Ui->pc_ComboBoxExistingBus->clear();
         if (q_ExistingBusExists == true)
         {
            const uint32 u32_BusCount = C_PuiSdHandler::h_GetInstance()->GetOSCBusesSize();

            this->mc_BusIndices.clear();
            for (uint32 u32_ItBus = 0; u32_ItBus < u32_BusCount; ++u32_ItBus)
            {
               uint32 u32_InterfaceCounter;
               const C_OSCSystemBus * pc_Bus = NULL;

               // Check if this bus is already connected to the target
               for (u32_InterfaceCounter = 0U; u32_InterfaceCounter < pc_Node2->c_Properties.c_ComInterfaces.size();
                    ++u32_InterfaceCounter)
               {
                  if ((pc_Node2->c_Properties.c_ComInterfaces[u32_InterfaceCounter].q_IsBusConnected == true) &&
                      (pc_Node2->c_Properties.c_ComInterfaces[u32_InterfaceCounter].u32_BusIndex == u32_ItBus))
                  {
                     // Bus is connected to at least one interface of the node
                     pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_ItBus);
                     break;
                  }
               }

               if (pc_Bus != NULL)
               {
                  QString c_Text = pc_Bus->c_Name.c_str();

                  if (pc_Bus->e_Type == C_OSCSystemBus::eCAN)
                  {
                     c_Text += " (" + QString::number(pc_Bus->u64_BitRate / 1000ULL);
                     c_Text += " kbit/s)";
                  }

                  if ((q_ExistingBusRestricted == false) ||
                      (pc_Bus->e_Type == e_ExistingBusRestrictedType))
                  {
                     // Add only if no restrictions for an existing bus exists or the bus matches
                     this->mpc_Ui->pc_ComboBoxExistingBus->addItem(c_Text);
                     this->mc_BusIndices.push_back(u32_ItBus);
                  }
               }
            }
         }

         m_HandleTypeChange();
         m_HandleBusChange();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of current bus type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_HandleTypeChange(void) const
{
   const C_OSCNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node1Index);
   const C_OSCNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node2Index);

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      bool q_DatapoolL2;
      bool q_DatapoolECeS;
      bool q_DatapoolECoS;

      const C_OSCSystemBus::E_Type e_Type = this->GetBusType();

      // Node 1
      C_SdUtil::h_InitNodeInterfaceComboBox(*pc_Node1,
                                            e_Type,
                                            this->mpc_Ui->pc_ComboBoxComIntf1, -1,
                                            q_DatapoolL2,
                                            q_DatapoolECeS,
                                            q_DatapoolECoS);

      this->mpc_Ui->pc_CheckBoxLayer2Node1->setChecked(q_DatapoolL2);
      this->mpc_Ui->pc_CheckBoxECeSNode1->setChecked(q_DatapoolECeS);
      this->mpc_Ui->pc_CheckBoxECoSNode1->setChecked(q_DatapoolECoS);

      // Node 2
      if (this->mpc_Ui->pc_RadioButtonCreateNew->isChecked() == true)
      {
         // Target node has at least one free interface and "create new bus" is selected
         C_SdUtil::h_InitNodeInterfaceComboBox(
            *pc_Node2, e_Type, this->mpc_Ui->pc_ComboBoxComIntf2, -1,
            q_DatapoolL2, q_DatapoolECeS, q_DatapoolECoS);

         this->mpc_Ui->pc_ComboBoxComIntf2->setEnabled(true);
      }
      else
      {
         const uint32 u32_CurBusIndex = this->GetBusIndex();
         uint32 u32_Intfcounter;

         // Select the used interface for the current bus which is not changeable for the target node in this case
         for (u32_Intfcounter = 0U; u32_Intfcounter < pc_Node2->c_Properties.c_ComInterfaces.size(); ++u32_Intfcounter)
         {
            const C_OSCNodeComInterfaceSettings & rc_Interface =
               pc_Node2->c_Properties.c_ComInterfaces[u32_Intfcounter];

            if ((rc_Interface.q_IsBusConnected == true) &&
                (rc_Interface.u32_BusIndex == u32_CurBusIndex))
            {
               // Target node can have no free interfaces and "select existing bus" is checked
               C_SdUtil::h_InitNodeInterfaceComboBox(
                  *pc_Node2, e_Type, this->mpc_Ui->pc_ComboBoxComIntf2,
                  static_cast<sint32>(rc_Interface.u8_InterfaceNumber),
                  q_DatapoolL2, q_DatapoolECeS, q_DatapoolECoS);

               this->mpc_Ui->pc_ComboBoxComIntf2->setEnabled(false);
            }
         }
      }

      this->mpc_Ui->pc_CheckBoxLayer2Node2->setChecked(q_DatapoolL2);
      this->mpc_Ui->pc_CheckBoxECeSNode2->setChecked(q_DatapoolECeS);
      this->mpc_Ui->pc_CheckBoxECoSNode2->setChecked(q_DatapoolECoS);

      // Only relevant for CAN
      this->mpc_Ui->pc_GroupBoxProtocolsNode1->setVisible(e_Type == C_OSCSystemBus::eCAN);
      this->mpc_Ui->pc_GroupBoxProtocolsNode2->setVisible(e_Type == C_OSCSystemBus::eCAN);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of selected bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_HandleBusChange(void)
{
   const C_OSCNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node1Index);
   const C_OSCNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node2Index);

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      uint32 u32_Proposal;
      const uint32 u32_BusIndex = this->GetBusIndex();

      std::vector<stw_types::uint32> c_TmpUsedNodeIds;

      if (this->mpc_Ui->pc_RadioButtonCreateNew->isChecked() == true)
      {
         this->mc_UsedNodeIds.clear();
         //Used bus ids
         C_SdUtil::h_InitUsedNodeIdsLabel(this->mc_UsedNodeIds, "", this->mpc_Ui->pc_LabelUsedNodeIds);

         this->mpc_Ui->pc_SpinBoxNode2->setEnabled(true);
      }
      else
      {
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(u32_BusIndex);
         if (pc_Bus != NULL)
         {
            //NodeIds
            this->mc_UsedNodeIds = C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(u32_BusIndex, 0, -1);
            //Used bus ids
            C_SdUtil::h_InitUsedNodeIdsLabel(this->mc_UsedNodeIds,
                                             pc_Bus->c_Name.c_str(), this->mpc_Ui->pc_LabelUsedNodeIds);
         }

         this->mpc_Ui->pc_SpinBoxNode2->setEnabled(false);
      }
      //Get next valid bus id 1
      u32_Proposal = C_SdUtil::h_GetNextFreeNodeId(pc_Node1->c_Properties.c_ComInterfaces, this->mc_UsedNodeIds, -1);

      this->mpc_Ui->pc_SpinBoxNode1->setValue(u32_Proposal);

      //Get next valid bus id 2
      if (this->mpc_Ui->pc_RadioButtonCreateNew->isChecked() == true)
      {
         // On a new bus, the ID for the target node can be changed
         c_TmpUsedNodeIds = this->mc_UsedNodeIds;
         //Sorted push
         c_TmpUsedNodeIds.push_back(u32_Proposal);
         C_SdUtil::h_SortIndicesAscending(c_TmpUsedNodeIds);
         u32_Proposal = C_SdUtil::h_GetNextFreeNodeId(pc_Node2->c_Properties.c_ComInterfaces, c_TmpUsedNodeIds, -1);
      }
      else
      {
         uint32 u32_IntfCounter;

         // Search the current used node id for the interface
         for (u32_IntfCounter = 0U; u32_IntfCounter < pc_Node2->c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
         {
            const C_OSCNodeComInterfaceSettings & rc_Interface =
               pc_Node2->c_Properties.c_ComInterfaces[u32_IntfCounter];

            if ((rc_Interface.q_IsBusConnected == true) &&
                (rc_Interface.u32_BusIndex == u32_BusIndex))
            {
               // On an existing bus, the ID can not be changed
               u32_Proposal = static_cast<uint32>(rc_Interface.u8_NodeID);
               break;
            }
         }
      }

      this->mpc_Ui->pc_SpinBoxNode2->setValue(u32_Proposal);
      //Initial check value
      m_CheckNodeIds();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check new node id value

   \param[in] ors32_Value New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnNodeId1Change(const sint32 & ors32_Value)
{
   bool q_Valid = true;

   for (uint32 u32_ItBusId = 0; u32_ItBusId < this->mc_UsedNodeIds.size(); ++u32_ItBusId)
   {
      if (static_cast<uint32>(ors32_Value) == this->mc_UsedNodeIds[u32_ItBusId])
      {
         q_Valid = false;
      }
   }
   //Check spin box match
   if (ors32_Value == this->mpc_Ui->pc_SpinBoxNode2->value())
   {
      q_Valid = false;
   }

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxNode1, "Valid", q_Valid);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check new node id value

   \param[in] ors32_Value New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnNodeId2Change(const sint32 & ors32_Value)
{
   bool q_Valid = true;

   // Can not be invalid when disabled
   if (this->mpc_Ui->pc_SpinBoxNode2->isEnabled() == true)
   {
      for (uint32 u32_ItBusId = 0; u32_ItBusId < this->mc_UsedNodeIds.size(); ++u32_ItBusId)
      {
         if (static_cast<uint32>(ors32_Value) == this->mc_UsedNodeIds[u32_ItBusId])
         {
            q_Valid = false;
         }
      }
      //Check spin box match
      if (ors32_Value == this->mpc_Ui->pc_SpinBoxNode1->value())
      {
         q_Valid = false;
      }
   }

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxNode2, "Valid", q_Valid);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Recheck all node IDs
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_CheckNodeIds(void)
{
   m_OnNodeId1Change(this->mpc_Ui->pc_SpinBoxNode1->value());
   m_OnNodeId2Change(this->mpc_Ui->pc_SpinBoxNode2->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check new bus name value

   \param[in] orc_Value Bus name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnBusNameChange(const QString & orc_Value) const
{
   const bool q_Valid = C_PuiSdHandler::h_GetInstance()->CheckBusNameAvailable(orc_Value.toStdString().c_str());

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditBusName, "Valid", q_Valid);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of new or existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnNewOrExistingChange(void)
{
   m_HandleTypeChange();
   m_HandleBusChange();
   if (this->CheckIfCreateNew() == true)
   {
      this->mpc_Ui->pc_ComboBoxExistingBus->setEnabled(false);
      this->mpc_Ui->pc_LabelBusType->setEnabled(true);
      this->mpc_Ui->pc_ComboBoxBusType->setEnabled(true);
      this->mpc_Ui->pc_LabelBusName->setEnabled(true);
      this->mpc_Ui->pc_LineEditBusName->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxExistingBus->setEnabled(true);
      this->mpc_Ui->pc_LabelBusType->setEnabled(false);
      this->mpc_Ui->pc_ComboBoxBusType->setEnabled(false);
      this->mpc_Ui->pc_LabelBusName->setEnabled(false);
      this->mpc_Ui->pc_LineEditBusName->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of new bus type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnBusTypeChange(void) const
{
   m_HandleTypeChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of existing bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_OnExistingBusChange(void)
{
   m_HandleTypeChange();
   m_HandleBusChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if any bus type restrictions necessary

   \param[out] orq_NewBusPossible           Flag if a new bus is possible
   \param[out] orq_NewBusRestricted         Flag if a new bus type is restricted
   \param[out] ore_NewBusType               New bus type to restrict to (Only valid if orq_NewBusRestricted is true)
   \param[out] orq_ExistingBusExists        Flag if at least one existing bus exists
   \param[out] orq_ExistingBusRestricted    Flag if an existing bus type is restricted
   \param[out] ore_ExistingBusType          Existing bus type to restrict to
                                            (Only valid if orq_ExistingBusRestricted is true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeToNodeConnectionSetupWidget::m_CheckTypeRestrictions(bool & orq_NewBusPossible,
                                                                  bool & orq_NewBusRestricted,
                                                                  C_OSCSystemBus::E_Type & ore_NewBusType,
                                                                  bool & orq_ExistingBusExists,
                                                                  bool & orq_ExistingBusRestricted,
                                                                  C_OSCSystemBus::E_Type & ore_ExistingBusType) const
{
   const C_OSCNode * const pc_Node1 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node1Index);
   const C_OSCNode * const pc_Node2 = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Node2Index);

   orq_NewBusPossible = false;
   orq_NewBusRestricted = false;
   orq_ExistingBusExists = false;
   orq_ExistingBusRestricted = false;

   if ((pc_Node1 != NULL) && (pc_Node2 != NULL))
   {
      // Flags for the source node only
      const bool q_SourceCanWorks = C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node1->c_Properties.c_ComInterfaces,
                                                                            C_OSCSystemBus::eCAN);
      const bool q_SourceEthWorks = C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node1->c_Properties.c_ComInterfaces,
                                                                            C_OSCSystemBus::eETHERNET);
      // Flags for creating a new bus
      const bool q_NewCanWorks = q_SourceCanWorks &&
                                 C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node2->c_Properties.c_ComInterfaces,
                                                                         C_OSCSystemBus::eCAN);
      const bool q_NewEthernetWorks = q_SourceEthWorks &&
                                      C_SdUtil::h_CheckNodeInterfaceAvailable(pc_Node2->c_Properties.c_ComInterfaces,
                                                                              C_OSCSystemBus::eETHERNET);

      // Flags for using an existing bus
      const bool q_ExistingCanWorks = q_SourceCanWorks &&
                                      C_SdUtil::h_CheckNodeInterfaceConnected(pc_Node2->c_Properties.c_ComInterfaces,
                                                                              C_OSCSystemBus::eCAN);
      const bool q_ExistingEthernetWorks = q_SourceEthWorks &&
                                           C_SdUtil::h_CheckNodeInterfaceConnected(
         pc_Node2->c_Properties.c_ComInterfaces, C_OSCSystemBus::eETHERNET);

      // Adapt output parameter for new bus
      if ((q_NewCanWorks == true) && (q_NewEthernetWorks == true))
      {
         orq_NewBusPossible = true;
         orq_NewBusRestricted = false;
      }
      else if (q_NewCanWorks == true)
      {
         orq_NewBusPossible = true;
         orq_NewBusRestricted = true;
         ore_NewBusType = C_OSCSystemBus::eCAN;
      }
      else if (q_NewEthernetWorks == true)
      {
         orq_NewBusPossible = true;
         orq_NewBusRestricted = true;
         ore_NewBusType = C_OSCSystemBus::eETHERNET;
      }
      else
      {
         // Creating a new bus is not possible
      }

      // Adapt output parameter for existing bus
      if ((q_ExistingCanWorks == true) && (q_ExistingEthernetWorks == true))
      {
         orq_ExistingBusExists = true;
         orq_ExistingBusRestricted = false;
      }
      else if (q_ExistingCanWorks == true)
      {
         orq_ExistingBusExists = true;
         orq_ExistingBusRestricted = true;
         ore_ExistingBusType = C_OSCSystemBus::eCAN;
      }
      else if (q_ExistingEthernetWorks == true)
      {
         orq_ExistingBusExists = true;
         orq_ExistingBusRestricted = true;
         ore_ExistingBusType = C_OSCSystemBus::eETHERNET;
      }
      else
      {
         // Using an existing bus is not possible
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check the total number of datapools for both nodes

   The total number of datapools is bounded above by mu32_NODE_DATA_POOL_MAX.

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::m_CheckDatapoolNumberNodes(void) const
{
   bool q_Return;

   q_Return = m_CheckDatapoolNumberNode1() && m_CheckDatapoolNumberNode2();

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check the total number of datapools for node 1

   The total number of datapools is bounded above by mu32_NODE_DATA_POOL_MAX.

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::m_CheckDatapoolNumberNode1(void) const
{
   bool q_Return = false;
   uint32 u32_Counter;
   const C_OSCNode * const pc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(mu32_Node1Index);

   if (pc_OscNode != NULL)
   {
      bool q_L2ProtocolFound = false;
      bool q_ECeSProtocolFound = false;
      bool q_ECoSProtocolFound = false;

      // ECoS
      for (u32_Counter = 0U; u32_Counter < pc_OscNode->c_ComProtocols.size(); ++u32_Counter)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_OscNode->c_ComProtocols[u32_Counter];
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eLAYER2)
         {
            // Protocol found
            q_L2ProtocolFound = true;
            break;
         }
      }

      // ECeS
      for (u32_Counter = 0U; u32_Counter < pc_OscNode->c_ComProtocols.size(); ++u32_Counter)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_OscNode->c_ComProtocols[u32_Counter];
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eECES)
         {
            // Protocol found
            q_ECeSProtocolFound = true;
            break;
         }
      }

      // ECoS
      for (u32_Counter = 0U; u32_Counter < pc_OscNode->c_ComProtocols.size(); ++u32_Counter)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_OscNode->c_ComProtocols[u32_Counter];
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
         {
            // Protocol found
            q_ECoSProtocolFound = true;
            break;
         }
      }

      // is enough space available?
      uint32 u32_DatapoolsToAdd = 0;
      if ((this->mpc_Ui->pc_CheckBoxLayer2Node1->isChecked() == true) && (q_L2ProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      if ((this->mpc_Ui->pc_CheckBoxECeSNode1->isChecked() == true) && (q_ECeSProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      if ((this->mpc_Ui->pc_CheckBoxECoSNode1->isChecked() == true) && (q_ECoSProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      // check if the resulting total number of datapools is too high
      if ((pc_OscNode->c_DataPools.size() + u32_DatapoolsToAdd) > mu32_NODE_DATA_POOL_MAX)
      {
         C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool add"));
         c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("Node %1 cannot create %2 more COMM Datapool(s) "
                                                                    "without exceeding the limit of %3 Datapools.")).
                                     arg(pc_OscNode->c_Properties.c_Name.c_str()).
                                     arg(u32_DatapoolsToAdd).
                                     arg(mu32_NODE_DATA_POOL_MAX));
         c_MessageBox.Execute();
      }
      else
      {
         // all OK
         q_Return = true;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check the total number of datapools for node 2

   The total number of datapools is bounded above by mu32_NODE_DATA_POOL_MAX.

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeToNodeConnectionSetupWidget::m_CheckDatapoolNumberNode2(void) const
{
   bool q_Return = false;
   uint32 u32_Counter;
   const C_OSCNode * const pc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(mu32_Node2Index);

   if (pc_OscNode != NULL)
   {
      bool q_L2ProtocolFound = false;
      bool q_ECeSProtocolFound = false;
      bool q_ECoSProtocolFound = false;

      // ECoS
      for (u32_Counter = 0U; u32_Counter < pc_OscNode->c_ComProtocols.size(); ++u32_Counter)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_OscNode->c_ComProtocols[u32_Counter];
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eLAYER2)
         {
            // Protocol found
            q_L2ProtocolFound = true;
            break;
         }
      }

      // ECeS
      for (u32_Counter = 0U; u32_Counter < pc_OscNode->c_ComProtocols.size(); ++u32_Counter)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_OscNode->c_ComProtocols[u32_Counter];
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eECES)
         {
            // Protocol found
            q_ECeSProtocolFound = true;
            break;
         }
      }

      // ECoS
      for (u32_Counter = 0U; u32_Counter < pc_OscNode->c_ComProtocols.size(); ++u32_Counter)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_OscNode->c_ComProtocols[u32_Counter];
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
         {
            // Protocol found
            q_ECoSProtocolFound = true;
            break;
         }
      }

      // is enough space available?
      uint32 u32_DatapoolsToAdd = 0;
      if ((this->mpc_Ui->pc_CheckBoxLayer2Node2->isChecked() == true) && (q_L2ProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      if ((this->mpc_Ui->pc_CheckBoxECeSNode2->isChecked() == true) && (q_ECeSProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      if ((this->mpc_Ui->pc_CheckBoxECoSNode2->isChecked() == true) && (q_ECoSProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      // check if the resulting total number of datapools is too high
      if ((pc_OscNode->c_DataPools.size() + u32_DatapoolsToAdd) > mu32_NODE_DATA_POOL_MAX)
      {
         C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Datapool add"));
         c_MessageBox.SetDescription(QString(C_GtGetText::h_GetText("Node %1 cannot create %2 more COMM Datapool(s) "
                                                                    "without exceeding the limit of %3 Datapools.")).
                                     arg(pc_OscNode->c_Properties.c_Name.c_str()).
                                     arg(u32_DatapoolsToAdd).
                                     arg(mu32_NODE_DATA_POOL_MAX));
         c_MessageBox.Execute();
      }
      else
      {
         // all OK
         q_Return = true;
      }
   }

   return q_Return;
}
