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

   this->mrc_ParentDialog.SetWidget(this);

   InitStaticNames();
   m_InitFromData();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNodeComIfSetupWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SdNodeComIfSetupWidget::m_CancelClicked);
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
   this->mpc_Ui->pc_LabelComInterfaceHeading->setText(C_GtGetText::h_GetText("Select COMM Interface"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected node interface

   \return
   Selected node interface (Not necessarily the index)
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_SdNodeComIfSetupWidget::GetSelectedInterface(void) const
{
   uint32 u32_Retval = 0;
   const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);

   if (pc_Bus != NULL)
   {
      u32_Retval = C_SdUtil::h_GetActiveNodeInterface(*this->mpc_Ui->pc_ComboBoxComIntf, this->mu32_NodeIndex,
                                                      pc_Bus->e_Type);
   }
   return static_cast<uint8>(u32_Retval);
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
   this->mrc_ParentDialog.accept();
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
      this->mq_InteractionPossible = C_SdUtil::h_InitNodeInterfaceComboBox(*pc_Node,
                                                                           pc_Bus->e_Type,
                                                                           this->mpc_Ui->pc_ComboBoxComIntf,
                                                                           this->ms32_SpecialInterface);

      //Node name
      this->mrc_ParentDialog.SetTitle(C_PuiSdUtil::h_GetNodeBaseNameOrName(this->mu32_NodeIndex));
   }
}
