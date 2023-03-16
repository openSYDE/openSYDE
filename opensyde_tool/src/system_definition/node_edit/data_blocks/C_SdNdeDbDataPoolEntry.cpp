//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for data pool entry with delete functionality (implementation)

   Widget for data pool entry with delete functionality

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDbDataPoolEntry.hpp"
#include "ui_C_SdNdeDbDataPoolEntry.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in,out]  opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbDataPoolEntry::C_SdNdeDbDataPoolEntry(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                               QWidget * const opc_Parent) :
   stw::opensyde_gui_elements::C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbDataPoolEntry),
   mu32_DataPoolIndex(ou32_DataPoolIndex)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_PushButtonDelete->SetSvg("://images/main_page_and_navi_bar/Icon_delete.svg");

   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Remove"),
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

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbDataPoolEntry::m_Init(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex)
{
   const C_OscNodeDataPool * const pc_Datapool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(ou32_NodeIndex,
                                                                                                 ou32_DataPoolIndex);
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   if ((pc_Node != NULL) && (pc_Datapool != NULL))
   {
      QString c_Text;

      c_Text = pc_Datapool->c_Name.c_str();
      c_Text += " (";
      c_Text += C_PuiSdUtil::h_ConvertDataPoolTypeToString(pc_Datapool->e_Type);
      if (pc_Datapool->e_Type == C_OscNodeDataPool::eCOM)
      {
         c_Text += ", ";
         c_Text += C_GtGetText::h_GetText("Protocol: ");
         c_Text += C_PuiSdUtil::h_ConvertProtocolTypeToString(C_PuiSdUtil::h_GetRelatedCanProtocolType(
                                                                 ou32_NodeIndex, ou32_DataPoolIndex));
      }
      c_Text += ")";
      this->mpc_Ui->pc_LabelName->setText(c_Text);
      this->SetToolTipInformation(c_Text, pc_Datapool->c_Comment.c_str());

      if ((pc_Node->pc_DeviceDefinition != NULL) &&
          (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()) &&
          (pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_ProgrammingSupport == false))
      {
         this->mpc_Ui->pc_PushButtonDelete->setVisible(false);
      }
   }
}
