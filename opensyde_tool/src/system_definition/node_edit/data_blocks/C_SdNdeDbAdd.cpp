//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system node data block properties (implementation)

   Dialog for system node data block properties.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDbAdd.h"
#include "ui_C_SdNdeDbAdd.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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

   \param[in,out] orc_Parent     Reference to parent
   \param[in]     ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbAdd::C_SdNdeDbAdd(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 ou32_NodeIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbAdd),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Data Blocks"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Add new Data Block"));

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNdeDbAdd::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this, &C_SdNdeDbAdd::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbAdd::~C_SdNdeDbAdd(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAdd::InitStaticNames(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Select Type"));

   //Radio buttons text
   this->mpc_Ui->pc_RadioButtonBinary->setText(C_GtGetText::h_GetText("Binary"));
   this->mpc_Ui->pc_RadioButtonProgrammingAppl->setText(C_GtGetText::h_GetText("Programmable application"));
   this->mpc_Ui->pc_RadioButtonTSP->setText(C_GtGetText::h_GetText(
                                               "Add from openSYDE Target Support Package (*.syde_tsp)"));

   //Radio buttons tooltips
   this->mpc_Ui->pc_RadioButtonBinary->SetToolTipInformation(
      C_GtGetText::h_GetText("Binary"),
      C_GtGetText::h_GetText(
         "Any kind of application or data created by another tool chain."));
   this->mpc_Ui->pc_RadioButtonTSP->SetToolTipInformation(
      C_GtGetText::h_GetText("Add from openSYDE Target Support Package (*.syde_tsp)"),
      C_GtGetText::h_GetText("Add Data Block(s) extracted from openSYDE Target Support Package (*.syde_tsp)"));
   this->mpc_Ui->pc_RadioButtonProgrammingAppl->SetToolTipInformation(
      C_GtGetText::h_GetText("Programmable Application"),
      C_GtGetText::h_GetText(
         "A programmable application developed by using the openSYDE feature \"Generate Code\"."));

   //Binary and TSP is default in every use case
   this->mpc_Ui->pc_RadioButtonBinary->setChecked(true);
   this->mpc_Ui->pc_RadioButtonTSP->setChecked(true);

   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      if (pc_Node->pc_DeviceDefinition->q_ProgrammingSupport == true)
      {
         this->mpc_Ui->pc_RadioButtonProgrammingAppl->setEnabled(true);
      }
      else
      {
         this->mpc_Ui->pc_RadioButtonProgrammingAppl->setEnabled(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get from TSP flag

   Warning: should only be used if not binary/file based

   \return
   From TSP flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDbAdd::GetFromTSP(void) const
{
   bool q_Retval = false;

   if (this->mpc_Ui->pc_RadioButtonTSP->isChecked() == true)
   {
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get chosen application type

   \return
   Chosen application type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeApplication::E_Type C_SdNdeDbAdd::GetApplicationType(void) const
{
   C_OSCNodeApplication::E_Type e_Retval;
   if ((this->mpc_Ui->pc_RadioButtonTSP->isChecked() == true) ||
       (this->mpc_Ui->pc_RadioButtonProgrammingAppl->isChecked() == true))
   {
      e_Retval = C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION;
   }
   else
   {
      e_Retval = C_OSCNodeApplication::eBINARY;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAdd::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAdd::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbAdd::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
