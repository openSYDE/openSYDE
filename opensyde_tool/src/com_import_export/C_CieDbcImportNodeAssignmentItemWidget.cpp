//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Item for list of DBC nodes, its messages and the selection combobox for corresponding topology node

   Item for list of DBC nodes, its messages and the selection combobox for corresponding topology node

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CieDbcImportNodeAssignmentItemWidget.h"
#include "ui_C_CieDbcImportNodeAssignmentItemWidget.h"

#include "TGLUtils.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
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

   \param[in]        orc_DbcNode       Node data structure read from DBC file
   \param[in]        orc_OsyNodeNames  Names of nodes resp. node and interface connected to specific bus
   \param[in,out]    opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDbcImportNodeAssignmentItemWidget::C_CieDbcImportNodeAssignmentItemWidget(
   const C_CieConverter::C_CIENode & orc_DbcNode, const std::vector<QString> & orc_OsyNodeNames,
   const std::vector<uint32> & orc_NodeIndexes, const std::vector<uint32> & orc_InterfaceIndexes,
   QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_CieDbcImportNodeAssignmentItemWidget),
   mrc_DbcNode(orc_DbcNode),
   mc_NodeIndexes(orc_NodeIndexes),
   mc_InterfaceIndexes(orc_InterfaceIndexes),
   ms32_PreviousCbxIndex(0)
{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_LabImportNode->setText(orc_DbcNode.c_Properties.c_Name.c_str());
   this->mpc_Ui->pc_LabImportNode->SetToolTipInformation(
      C_GtGetText::h_GetText(orc_DbcNode.c_Properties.c_Name.c_str()),
      C_GtGetText::h_GetText(orc_DbcNode.c_Properties.c_Comment.c_str()));

   this->mpc_Ui->pc_LabInfo->setText(static_cast<QString>("%1 Tx / %2 Rx").
                                     arg(orc_DbcNode.c_TxMessages.size()).arg(orc_DbcNode.c_RxMessages.size()));

   // fill combobox with all connected topology nodes
   this->mpc_Ui->pc_CbxOsyNodes->addItem(C_GtGetText::h_GetText("<ignore>"));
   for (std::vector<QString>::const_iterator c_It = orc_OsyNodeNames.begin(); c_It != orc_OsyNodeNames.end(); ++c_It)
   {
      this->mpc_Ui->pc_CbxOsyNodes->addItem(*c_It);
   }

   //lint -e{929} Cast required to avoid ambiguous signal of Qt interface
   connect(this->mpc_Ui->pc_CbxOsyNodes, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_CieDbcImportNodeAssignmentItemWidget::m_OnComboboxIndexChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDbcImportNodeAssignmentItemWidget::~C_CieDbcImportNodeAssignmentItemWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all tuples of DBC node, system definition node index and interface index.

   indexes are -1 if "ignore" was selected

   \return
   class package of DBC nodes an its assigned node indexes and interface indexes
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieDbcOsyNodeAssignment C_CieDbcImportNodeAssignmentItemWidget::GetNodeAssignment(void) const
{
   C_CieDbcOsyNodeAssignment c_Return;
   const sint32 s32_CurrentIndex = this->mpc_Ui->pc_CbxOsyNodes->currentIndex();

   c_Return.s32_AssignedOsyNodeIndex = -1;
   c_Return.s32_AssignedOsyInterfaceIndex = -1;
   c_Return.c_CieNode = this->mrc_DbcNode;

   if (s32_CurrentIndex > 0) // index 0 is "ignore"
   {
      tgl_assert(this->mc_InterfaceIndexes.size() == this->mc_NodeIndexes.size());

      // subtract one of current index because of ignore item in combobox
      const uint32 u32_DataIndex = static_cast<uint32>(s32_CurrentIndex - 1);

      if (u32_DataIndex < this->mc_NodeIndexes.size())
      {
         c_Return.s32_AssignedOsyNodeIndex = static_cast<sint32>(this->mc_NodeIndexes[u32_DataIndex]);
         c_Return.s32_AssignedOsyInterfaceIndex = static_cast<sint32>(this->mc_InterfaceIndexes[u32_DataIndex]);
      }
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief     Return if the combobox selection is not "ignored".

   \retval   true    A Node is selected
   \retval   false   No node resp. ignored is selected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CieDbcImportNodeAssignmentItemWidget::IsAssigned(void) const
{
   // index 0 is "ignored"
   return(this->mpc_Ui->pc_CbxOsyNodes->currentIndex() > 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Enable or disable specified combobox item.

   \param[in]       ou32_Index    Combobox index of item that should be enabled/disabled
   \param[in]       oq_Enable     flag of enable or disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentItemWidget::UpdateComboboxEntries(const uint32 ou32_Index, const bool oq_Enable) const
{
   this->mpc_Ui->pc_CbxOsyNodes->SetItemState(ou32_Index, oq_Enable);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of combo box index changed signal.

   Emits a signal to inform the other combo boxes that a specific index now must be disabled/enabled.

   \param[in]       os32_Index   New combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieDbcImportNodeAssignmentItemWidget::m_OnComboboxIndexChanged(const sint32 os32_Index)
{
   tgl_assert(os32_Index >= 0);

   // trigger enabling of previous index
   Q_EMIT (this->SigUpdateOtherComboboxes(this->ms32_PreviousCbxIndex, true, this));

   // all cases except "ignore"
   if (os32_Index > 0)
   {
      // trigger disabling of new index
      Q_EMIT (this->SigUpdateOtherComboboxes(os32_Index, false, this));
   }

   // remember new previous index
   this->ms32_PreviousCbxIndex = os32_Index;
}
