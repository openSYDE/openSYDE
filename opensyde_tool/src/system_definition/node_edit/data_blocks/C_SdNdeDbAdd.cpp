//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Dialog for system node data block properties (implementation)

   Dialog for system node data block properties.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.04.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDbAdd.h"
#include "ui_C_SdNdeDbAdd.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent     Reference to parent
   \param[in]     ou32_NodeIndex Node index

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDbAdd::~C_SdNdeDbAdd(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbAdd::InitStaticNames(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));

   this->mpc_Ui->pc_LabelHeading->setText(C_GtGetText::h_GetText("Select Type"));
   this->mpc_Ui->pc_RadioButtonProgramming_TSP->setText(C_GtGetText::h_GetText(
                                                           "Programmable application(s) (extracted from openSYDE Target Support Package)"));
   this->mpc_Ui->pc_RadioButtonProgramming_Manual->setText(C_GtGetText::h_GetText(
                                                              "Programmable application (configure manually)"));
   if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
   {
      if (pc_Node->pc_DeviceDefinition->q_ProgrammingSupport == true)
      {
         this->mpc_Ui->pc_RadioButtonProgramming_TSP->setEnabled(true);
         this->mpc_Ui->pc_RadioButtonProgramming_Manual->setEnabled(true);
         //Always select something
         this->mpc_Ui->pc_RadioButtonProgramming_TSP->setChecked(true);
         //Tool tips
         this->mpc_Ui->pc_RadioButtonProgramming_TSP->SetToolTipInformation(
            C_GtGetText::h_GetText("Programmable application(s) (extracted from openSYDE Target Support Package)"),
            C_GtGetText::h_GetText(
               "A programmable application developed by using the openSYDE feature \"Generate Code\"."
               "\nConfiguration is extracted from openSYDE Target Support Package."));
         this->mpc_Ui->pc_RadioButtonProgramming_Manual->SetToolTipInformation(
            C_GtGetText::h_GetText("Programmable application (configure manually)"),
            C_GtGetText::h_GetText(
               "A programmable application developed by using the openSYDE feature \"Generate Code\"."
               "\nConfiguration is made manually by the user."));
      }
      else
      {
         this->mpc_Ui->pc_RadioButtonProgramming_TSP->setEnabled(false);
         this->mpc_Ui->pc_RadioButtonProgramming_Manual->setEnabled(false);
         //Always select something
         this->mpc_Ui->pc_RadioButtonBinary->setChecked(true);
         //Tool tips
         this->mpc_Ui->pc_RadioButtonProgramming_TSP->SetToolTipInformation(
            C_GtGetText::h_GetText("Programmable application(s) (extracted from openSYDE Target Support Package)"),
            C_GtGetText::h_GetText(
               "A programmable application developed by using the openSYDE feature \"Generate Code\"."
               "\nConfiguration is extracted from openSYDE Target Support Package."));
         this->mpc_Ui->pc_RadioButtonProgramming_Manual->SetToolTipInformation(
            C_GtGetText::h_GetText("Programmable application (configure manually)"),
            C_GtGetText::h_GetText(
               "A programmable application developed by using the openSYDE feature \"Generate Code\"."
               "\nConfiguration is made manually by the user."));
      }
      if (pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == true)
      {
         this->mpc_Ui->pc_RadioButtonBinary->setText(C_GtGetText::h_GetText("File container"));
         //Check if feature available
         if (C_PuiSdHandler::h_GetInstance()->GetProgrammableApplications(this->mu32_NodeIndex).size() ==
             pc_Node->c_Applications.size())
         {
            this->mpc_Ui->pc_RadioButtonBinary->setEnabled(true);
            //Tool tips
            this->mpc_Ui->pc_RadioButtonBinary->SetToolTipInformation(
               C_GtGetText::h_GetText("File container"),
               C_GtGetText::h_GetText(
                  "A file container for any number of files you want to transfer to your file based device"));
         }
         else
         {
            this->mpc_Ui->pc_RadioButtonBinary->setEnabled(false);
            //Tool tips
            this->mpc_Ui->pc_RadioButtonBinary->SetToolTipInformation(
               C_GtGetText::h_GetText("File container"),
               C_GtGetText::h_GetText(
                  "A file container for any number of files you want to transfer to your file based device\n"
                  "Deactivated because there can only be one file container"));
         }
      }
      else
      {
         this->mpc_Ui->pc_RadioButtonBinary->setText(C_GtGetText::h_GetText("Binary"));
         //Tool tips
         this->mpc_Ui->pc_RadioButtonBinary->SetToolTipInformation(
            C_GtGetText::h_GetText("Binary"),
            C_GtGetText::h_GetText("Any kind of application or data created by another tool chain."));
      }
   }

   //Tool tips
   //this->mpc_Ui->pc_LabelHeading->SetToolTipInformation(C_GtGetText::h_GetText("Choose Type"),
   //                                                     C_GtGetText::h_GetText(
   //                                                        "Select the type of Data Block you want this device
   // to contain"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get from TSP flag

   Warning: should only be used if not binary/file based

   \return
   From TSP flag

   \created     05.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdNdeDbAdd::GetFromTSP(void) const
{
   bool q_Retval = false;

   if (this->mpc_Ui->pc_RadioButtonProgramming_TSP->isChecked() == true)
   {
      q_Retval = true;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get chosen application type

   \return
   Chosen application type

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeApplication::E_Type C_SdNdeDbAdd::GetApplicationType(void) const
{
   C_OSCNodeApplication::E_Type e_Retval;
   if ((this->mpc_Ui->pc_RadioButtonProgramming_TSP->isChecked() == true) ||
       (this->mpc_Ui->pc_RadioButtonProgramming_Manual->isChecked() == true))
   {
      e_Retval = C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION;
   }
   else
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      if ((pc_Node != NULL) && (pc_Node->pc_DeviceDefinition != NULL))
      {
         if (pc_Node->pc_DeviceDefinition->q_FlashloaderOpenSydeIsFileBased == true)
         {
            e_Retval = C_OSCNodeApplication::eFILE_CONTAINER;
         }
         else
         {
            e_Retval = C_OSCNodeApplication::eBINARY;
         }
      }
      else
      {
         //Should not happen
         e_Retval = C_OSCNodeApplication::eBINARY;
      }
   }
   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbAdd::m_OkClicked(void)
{
   if (this->mpc_Ui->pc_RadioButtonProgramming_TSP->isChecked() == true)
   {
      if (C_PuiSdHandler::h_GetInstance()->GetProgrammableApplications(this->mu32_NodeIndex).size() > 0UL)
      {
         C_OgeWiCustomMessage::E_Outputs e_Output;
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         c_Message.SetHeading(C_GtGetText::h_GetText("Delete existing data blocks"));
         c_Message.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
         c_Message.SetOKButtonText(C_GtGetText::h_GetText("Delete and Continue"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Before creating new Data Block(s) from openSYDE Target Support Package, "
                                     "all existing Data Blocks of type programming application will be deleted. "
                                     "\n\nDo you really want to continue?"));
         c_Message.SetCustomMinHeight(280);
         c_Message.SetCustomMinWidth(700);
         e_Output = c_Message.Execute();
         if (e_Output == C_OgeWiCustomMessage::eYES)
         {
            this->mrc_ParentDialog.accept();
         }
      }
      else
      {
         this->mrc_ParentDialog.accept();
      }
   }
   else
   {
      this->mrc_ParentDialog.accept();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     19.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbAdd::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}
