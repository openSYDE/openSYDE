//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Name char length settings popup dialog

   Name char length settings popup dialog

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_GtGetText.hpp"
#include "C_NagCharLengthSettingPopupDialog.hpp"
#include "ui_C_NagCharLengthSettingPopupDialog.h"
#include "C_OscUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagCharLengthSettingPopupDialog::C_NagCharLengthSettingPopupDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_NagCharLengthSettingPopupDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_CurrentSpinBoxValue(C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit())
{
   this->mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_SpinBox->setValue(C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimit());
   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_NagCharLengthSettingPopupDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this,
           &C_NagCharLengthSettingPopupDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_SpinBox, static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged), this,
           &C_NagCharLengthSettingPopupDialog::m_ValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagCharLengthSettingPopupDialog::~C_NagCharLengthSettingPopupDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagCharLengthSettingPopupDialog::InitStaticNames(void) const
{
   const QString c_SpinBoxToolTip =
      static_cast<QString>("Mininum: %1\nMaximum : %2").arg(this->mpc_Ui->pc_SpinBox->minimum()).arg(
         this->mpc_Ui->pc_SpinBox->maximum());
   const QString c_ResultMaxCharOfDiagOrNvm =
      static_cast<QString>("Resulting max. length of generated variable name (DIAG/NVM) = %1").arg(
         this->mpc_Ui->pc_SpinBox->value() + 5);
   const QString c_ResultMaxCharOfCommSignal =
      static_cast<QString>("Resulting max. length of generated variable name (COMM Signal) = %1").arg(
         (2 * this->mpc_Ui->pc_SpinBox->value()) + 5);

   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("PROJECT"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Global Settings"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("System Definition"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_SpinBox->SetToolTipInformation("Value", c_SpinBoxToolTip);

   this->mpc_Ui->pc_SetMaxLengthLabel->setText("Max length of openSYDE element names");
   this->mpc_Ui->pc_SetMaxLengthLabel->SetToolTipInformation("",
                                                             "This limit is used for openSYDE element names like Nodes, Buses, Datapools, "
                                                             "Variables, Parameters, COMM Signals, ...");
   this->mpc_Ui->pc_ResultLengthOfDIAGorNVM->setText(c_ResultMaxCharOfDiagOrNvm);
   this->mpc_Ui->pc_ResultLengthOfCOMMSignal->setText(c_ResultMaxCharOfCommSignal);
   this->mpc_Ui->pc_ResultLengthOfDIAGorNVM->SetToolTipInformation(
      "As a result, this limit affects the length of the variable names in generated code",
      "Example C Coder:\nMax. length of generated variable name (DIAG/NVM): <prefix=4>+_+<diag/nvm_var_name_length>;\n"
      "Max. length of generated variable name (COMM): <prefix=3>+_+<COMM_message_name_length>+_+<COMM_signal_name_length>;");
   this->mpc_Ui->pc_ResultLengthOfCOMMSignal->SetToolTipInformation(
      "As a result, this limit affects the length of the variable names in generated code",
      "Example C Coder:\nMax. length of generated variable name (DIAG/NVM): <prefix=4>+_+<diag/nvm_var_name_length>;\n"
      "Max. length of generated variable name (COMM): <prefix=3>+_+<COMM_message_name_length>+_+<COMM_signal_name_length>;");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Applying max char settings value

   \return
   bool q_IsMaxCharLimitChanged
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagCharLengthSettingPopupDialog::ApplyMaxCharLimitSettings()
{
   bool q_IsMaxCharLimitChanged = false;

   std::list<C_OscSystemNameMaxCharLimitChangeReportItem> orc_ChangedItems;
   C_PuiSdHandler::h_GetInstance()->GetNameMaxCharLimitAffectedItems(
      this->mpc_Ui->pc_SpinBox->value(), orc_ChangedItems);
   if (orc_ChangedItems.size() > 0)
   {
      q_IsMaxCharLimitChanged = this->m_AskUserForModifyName(orc_ChangedItems);
   }
   else
   {
      C_PuiSdHandler::h_GetInstance()->SetNameMaxCharLimit(this->mpc_Ui->pc_SpinBox->value());
      if (this->mu32_CurrentSpinBoxValue != static_cast<uint32_t>(this->mpc_Ui->pc_SpinBox->value()))
      {
         q_IsMaxCharLimitChanged = true;
      }
   }
   return q_IsMaxCharLimitChanged;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagCharLengthSettingPopupDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
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
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagCharLengthSettingPopupDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagCharLengthSettingPopupDialog::m_CancelClicked()
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for spin box value change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagCharLengthSettingPopupDialog::m_ValueChanged()
{
   const QString c_ResultMaxCharOfDiagOrNvm =
      static_cast<QString>("Resulting max. length of generated variable name (DIAG/NVM) = %1").arg(
         this->mpc_Ui->pc_SpinBox->value() + 5);
   const QString c_ResultMaxCharOfCommSignal =
      static_cast<QString>("Resulting max. length of generated variable name (COMM Signal) = %1").arg(
         (2 * this->mpc_Ui->pc_SpinBox->value()) + 5);

   this->mpc_Ui->pc_ResultLengthOfDIAGorNVM->setText(c_ResultMaxCharOfDiagOrNvm);
   this->mpc_Ui->pc_ResultLengthOfCOMMSignal->setText(c_ResultMaxCharOfCommSignal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Ask user for confirmation on Modifying name based on name char length settings

   \return
   bool q_IsMaxCharLimitChanged
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagCharLengthSettingPopupDialog::m_AskUserForModifyName(
   std::list<C_OscSystemNameMaxCharLimitChangeReportItem> orc_ChangedItems)
{
   bool q_IsMaxCharLimitChanged = false;
   QString c_Details;
   int32_t s32_NodesCount = 0;
   int32_t s32_DatapoolsCount = 0;
   int32_t s32_DatapoolListsCount = 0;
   int32_t s32_DatapoolListElementsCount = 0;
   int32_t s32_BusesCount = 0;
   int32_t s32_MessagesCount = 0;
   int32_t s32_HalChannelsCount = 0;
   int32_t s32_OthersCount = 0;

   QString c_NodeDetails;
   QString c_DatapoolDetails;
   QString c_DatapoolListDetails;
   QString c_DatapoolListElementDetails;
   QString c_BusDetails;
   QString c_MessageDetails;
   QString c_HalChannelDetails;
   QString c_OtherDetails;

   for (const C_OscSystemNameMaxCharLimitChangeReportItem & rc_Item : orc_ChangedItems)
   {
      const QString c_DetailEntry = static_cast<QString>("Original: %1; New: %2\n")
                                    .arg(rc_Item.c_OriginalName.c_str())
                                    .arg(rc_Item.c_NewName.c_str());

      if (rc_Item.c_Type == "node-name")
      {
         s32_NodesCount++;
         c_NodeDetails.append(c_DetailEntry);
      }
      else if (rc_Item.c_Type == "node-datapool-name")
      {
         s32_DatapoolsCount++;
         c_DatapoolDetails.append(c_DetailEntry);
      }
      else if (rc_Item.c_Type == "node-datapool-list-name")
      {
         s32_DatapoolListsCount++;
         c_DatapoolListDetails.append(c_DetailEntry);
      }
      else if (rc_Item.c_Type == "node-datapool-list-element-name")
      {
         s32_DatapoolListElementsCount++;
         c_DatapoolListElementDetails.append(c_DetailEntry);
      }
      else if (rc_Item.c_Type == "bus-name")
      {
         s32_BusesCount++;
         c_BusDetails.append(c_DetailEntry);
      }
      else if ((rc_Item.c_Type == "tx-message-name") || (rc_Item.c_Type == "rx-message-name"))
      {
         s32_MessagesCount++;
         c_MessageDetails.append(c_DetailEntry);
      }
      else if (rc_Item.c_Type == "hal-channel-name")
      {
         s32_HalChannelsCount++;
         c_HalChannelDetails.append(c_DetailEntry);
      }
      else
      {
         s32_OthersCount++;
         c_OtherDetails.append(c_DetailEntry);
      }
   }

   c_Details.append(static_cast<QString>("Impact on names overall: %1\n\n").arg(orc_ChangedItems.size()));
   if (s32_NodesCount > 0)
   {
      c_Details.append(static_cast<QString>("Node Names (%1):\n").arg(s32_NodesCount)).append(c_NodeDetails).append(
         "\n\n");
   }
   if (s32_DatapoolsCount > 0)
   {
      c_Details.append(static_cast<QString>("Datapool Names (%1):\n").arg(s32_DatapoolsCount)).append(
         c_DatapoolDetails).
      append("\n\n");
   }
   if (s32_DatapoolListsCount > 0)
   {
      c_Details.append(static_cast<QString>("Datapool List Names (%1):\n").arg(s32_DatapoolListsCount)).append(
         c_DatapoolListDetails).append("\n\n");
   }
   if (s32_DatapoolListElementsCount > 0)
   {
      c_Details.append(static_cast<QString>("Datapool List Element Names (%1):\n").arg(s32_DatapoolListElementsCount))
      .
      append(c_DatapoolListElementDetails).append("\n\n");
   }
   if (s32_BusesCount > 0)
   {
      c_Details.append(static_cast<QString>("Bus Names (%1):\n").arg(s32_BusesCount)).append(c_BusDetails).append(
         "\n\n");
   }
   if (s32_MessagesCount > 0)
   {
      c_Details.append(static_cast<QString>("Message Names (%1):\n").arg(s32_MessagesCount)).append(c_MessageDetails).
      append("\n\n");
   }
   if (s32_HalChannelsCount > 0)
   {
      c_Details.append(static_cast<QString>("Hal Channel Names (%1):\n").arg(s32_HalChannelsCount)).append(
         c_HalChannelDetails).append("\n\n");
   }
   if (s32_OthersCount > 0)
   {
      c_Details.append(static_cast<QString>("Other Names (%1):\n").arg(s32_OthersCount)).append(c_OtherDetails);
   }

   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eQUESTION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Modify openSYDE element names length"));
   c_MessageBox.SetDescription(
      "Modifying name length based on your char length settings would have impact on some openSYDE element names. "
      "Check details for complete list."
      "\n\nAre you sure you want to apply the changes?");
   c_MessageBox.SetDetails(c_Details);
   c_MessageBox.SetOkButtonText("Apply");
   c_MessageBox.SetNoButtonText("Cancel");
   c_MessageBox.SetCustomMinHeight(280, 550);
   c_MessageBox.SetCustomMinWidth(800);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      q_IsMaxCharLimitChanged = true;
      C_PuiSdHandler::h_GetInstance()->SetNameMaxCharLimit(this->mpc_Ui->pc_SpinBox->value());
      C_PuiSdHandler::h_GetInstance()->ApplyNameMaxCharLimit(this->mpc_Ui->pc_SpinBox->value());
   }
   return q_IsMaxCharLimitChanged;
}
