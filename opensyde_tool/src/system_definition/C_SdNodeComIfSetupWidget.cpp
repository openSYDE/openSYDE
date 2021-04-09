//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node Com Interface Setup

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNodeComIfSetupWidget.h"
#include "ui_C_SdNodeComIfSetupWidget.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSdUtil.h"
#include "C_SdUtil.h"
#include "C_Uti.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_types;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent             Reference to parent
   \param[in]     oru32_NodeIndex        Node index
   \param[in]     oru32_BusIndex         Bus index
   \param[in]     ors32_SpecialInterface Special interface (Used in some cases to not disable any connected interface)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNodeComIfSetupWidget::C_SdNodeComIfSetupWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                   const uint32 & oru32_NodeIndex, const uint32 & oru32_BusIndex,
                                                   const sint32 & ors32_SpecialInterface) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNodeComIfSetupWidget),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_BusIndex(oru32_BusIndex),
   ms32_SpecialInterface(ors32_SpecialInterface),
   mq_InteractionPossible(false)
{
   // init UI
   this->mpc_Ui->setupUi(this);

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxNodeId->SetMaximumCustom(126);

   this->mrc_ParentDialog.SetWidget(this);

   InitStaticNames();
   m_InitFromData();

   this->mpc_Ui->pc_SpinBoxNodeId->SetMaximumCustom(C_SdUtil::h_GetNodeIdMaximum(this->mu32_NodeIndex));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNodeComIfSetupWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SdNodeComIfSetupWidget::m_CancelClicked);
   //lint -e{929}  Qt interface
   connect(this->mpc_Ui->pc_SpinBoxNodeId, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged), this,
           &C_SdNodeComIfSetupWidget::m_OnNodeIdChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNodeComIfSetupWidget::~C_SdNodeComIfSetupWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeComIfSetupWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Communication Interface Setup"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelComInterfaceHeading->setText(C_GtGetText::h_GetText("1. Select COMM Interface"));
   this->mpc_Ui->pc_LabelNodeIdHeading->setText(C_GtGetText::h_GetText("2. Define Node ID"));
   this->mpc_Ui->pc_LabelNodeId->setText(C_GtGetText::h_GetText("Node ID"));
   this->mpc_Ui->pc_LabelProtocolsNode->setText(C_GtGetText::h_GetText("Active protocols"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdNodeComIfSetupWidget::GetSelectedInterface(void) const
{
   uint32 u32_Retval = 0;
   const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);

   if (pc_Bus != NULL)
   {
      u32_Retval = C_SdUtil::h_GetActiveNodeInterface(*this->mpc_Ui->pc_ComboBoxComIntf, this->mu32_NodeIndex,
                                                      pc_Bus->e_Type);
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node id

   \return
   Selected node id
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SdNodeComIfSetupWidget::GetNodeId(void) const
{
   return static_cast<uint8>(this->mpc_Ui->pc_SpinBoxNodeId->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interaction possible flag

   \return
   True  Interaction possible
   False Invalid state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeComIfSetupWidget::GetInteractionPossible(void) const
{
   return this->mq_InteractionPossible;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the COM datapool configuration

   \param[out]    orq_ComProtocolL2          Flag if Layer 2 COM datapool exist
   \param[out]    orq_ComProtocolECeS        Flag if ECeS COM datapool exist
   \param[out]    orq_ComProtocolECoS        Flag if ECoS COM datapool exist
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeComIfSetupWidget::GetComDataPoolConfiguration(bool & orq_ComProtocolL2, bool & orq_ComProtocolECeS,
                                                           bool & orq_ComProtocolECoS) const
{
   orq_ComProtocolL2 = this->mpc_Ui->pc_CheckBoxLayer2->isChecked();
   orq_ComProtocolECeS = this->mpc_Ui->pc_CheckBoxECeS->isChecked();
   orq_ComProtocolECoS = this->mpc_Ui->pc_CheckBoxECoS->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeComIfSetupWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_SdNodeComIfSetupWidget::m_OkClicked(void)
{
   if (m_CheckDatapoolNumber() == true)
   {
      this->mrc_ParentDialog.accept();
   }
   else
   {
      C_SdUtil::h_CheckDatapoolNumber(mu32_NodeIndex, true, this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeComIfSetupWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeComIfSetupWidget::m_InitFromData(void)
{
   const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if ((pc_Bus != NULL) && (pc_Node != NULL))
   {
      uint32 u32_Proposal;
      bool q_DatapoolL2;
      bool q_DatapoolECeS;
      bool q_DatapoolECoS;

      this->mc_UsedNodeIds = C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(this->mu32_BusIndex,
                                                                                      this->mu32_NodeIndex,
                                                                                      this->ms32_SpecialInterface);

      this->mq_InteractionPossible = C_SdUtil::h_InitNodeInterfaceComboBox(*pc_Node,
                                                                           pc_Bus->e_Type,
                                                                           this->mpc_Ui->pc_ComboBoxComIntf,
                                                                           this->ms32_SpecialInterface,
                                                                           q_DatapoolL2,
                                                                           q_DatapoolECeS,
                                                                           q_DatapoolECoS);

      this->mpc_Ui->pc_CheckBoxLayer2->setChecked(q_DatapoolL2);
      this->mpc_Ui->pc_CheckBoxECeS->setChecked(q_DatapoolECeS);
      this->mpc_Ui->pc_CheckBoxECoS->setChecked(q_DatapoolECoS);

      // Only relevant for CAN
      this->mpc_Ui->pc_GroupBoxProtocols->setVisible(pc_Bus->e_Type == C_OSCSystemBus::eCAN);

      //Used bus ids
      C_SdUtil::h_InitUsedNodeIdsLabel(this->mc_UsedNodeIds, pc_Bus->c_Name.c_str(), this->mpc_Ui->pc_LabelUsedNodeIds);
      //Get next valid bus id
      u32_Proposal = C_SdUtil::h_GetNextFreeNodeId(pc_Node->c_Properties.c_ComInterfaces, this->mc_UsedNodeIds,
                                                   this->ms32_SpecialInterface);

      this->mpc_Ui->pc_SpinBoxNodeId->setValue(u32_Proposal);
      //Initial check value
      m_OnNodeIdChange(u32_Proposal);
      //Node name
      this->mrc_ParentDialog.SetTitle(pc_Node->c_Properties.c_Name.c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check new node id value

   \param[in] ors32_Value New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNodeComIfSetupWidget::m_OnNodeIdChange(const sint32 & ors32_Value)
{
   bool q_Valid = true;

   for (uint32 u32_ItBusId = 0; u32_ItBusId < this->mc_UsedNodeIds.size(); ++u32_ItBusId)
   {
      if (static_cast<uint32>(ors32_Value) == this->mc_UsedNodeIds[u32_ItBusId])
      {
         q_Valid = false;
         break;
      }
   }

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxNodeId, "Valid", q_Valid);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check the total number of datapools

   The total number of datapools is bounded above by C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE.

   \return
      true:  total number is ok
      false: total number exceeds maximum
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNodeComIfSetupWidget::m_CheckDatapoolNumber(void) const
{
   bool q_Return = false;
   uint32 u32_Counter;
   const C_OSCNode * const pc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(mu32_NodeIndex);

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
      if ((this->mpc_Ui->pc_CheckBoxLayer2->isChecked() == true) && (q_L2ProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      if ((this->mpc_Ui->pc_CheckBoxECeS->isChecked() == true) && (q_ECeSProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      if ((this->mpc_Ui->pc_CheckBoxECoS->isChecked() == true) && (q_ECoSProtocolFound == false))
      {
         u32_DatapoolsToAdd++;
      }

      // check if the resulting total number of datapools is too high
      if ((pc_OscNode->c_DataPools.size() + u32_DatapoolsToAdd) <= C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE)
      {
         // all OK
         q_Return = true;
      }
   }

   return q_Return;
}
