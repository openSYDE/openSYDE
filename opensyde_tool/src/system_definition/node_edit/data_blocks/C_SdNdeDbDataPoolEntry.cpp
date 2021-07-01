//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for data pool entry with delete functionality (implementation)

   Widget for data pool entry with delete functionality

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiSdUtil.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"
#include "C_SdNdeDbDataPoolEntry.h"
#include "ui_C_SdNdeDbDataPoolEntry.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_NodeIndex     Node index
   \param[in]     ou32_DataPoolIndex Data pool index
   \param[in,out] opc_Parent         Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbDataPoolEntry::C_SdNdeDbDataPoolEntry(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_DataPoolIndex, QWidget * const opc_Parent) :
   stw_opensyde_gui_elements::C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbDataPoolEntry),
   mu32_DataPoolIndex(ou32_DataPoolIndex)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_PushButtonDelete->SetSvg("://images/main_page_and_navi_bar/Icon_delete.svg");

   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                            C_GtGetText::h_GetText("Remove Datapool from this list."));

   m_Init(ou32_NodeIndex, ou32_DataPoolIndex);

   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this, &C_SdNdeDbDataPoolEntry::m_OnDeleteClick);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbDataPoolEntry::~C_SdNdeDbDataPoolEntry()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle delete button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbDataPoolEntry::m_OnDeleteClick(void)
{
   Q_EMIT this->SigDeleteDataPool(this, this->mu32_DataPoolIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init based on specified data pool

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbDataPoolEntry::m_Init(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex)
{
   const C_OSCNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(ou32_NodeIndex,
                                                                                                 ou32_DataPoolIndex);
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Datapool != NULL))
   {
      QString c_Text;

      c_Text = pc_Datapool->c_Name.c_str();
      c_Text += " (";
      c_Text += C_PuiSdUtil::h_ConvertDataPoolTypeToString(pc_Datapool->e_Type);
      if (pc_Datapool->e_Type == C_OSCNodeDataPool::eCOM)
      {
         c_Text += ", ";
         c_Text += C_GtGetText::h_GetText("Protocol: ");
         c_Text += C_PuiSdUtil::h_ConvertProtocolTypeToString(C_PuiSdUtil::h_GetRelatedCANProtocolType(
                                                                 ou32_NodeIndex, ou32_DataPoolIndex));
      }
      c_Text += ")";
      this->mpc_Ui->pc_LabelName->setText(c_Text);
      this->SetToolTipInformation(c_Text, pc_Datapool->c_Comment.c_str());

      if ((pc_Node->pc_DeviceDefinition != NULL) && (pc_Node->pc_DeviceDefinition->q_ProgrammingSupport == false))
      {
         this->mpc_Ui->pc_PushButtonDelete->setVisible(false);
      }
   }
}
