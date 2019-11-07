//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver timeout configuration

   One message receiver timeout configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GtGetText.h"

#include "C_SdBueMessageRxTimeoutConfig.h"
#include "ui_C_SdBueMessageRxTimeoutConfig.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sint32 C_SdBueMessageRxTimeoutConfig::mhs32_IndexAuto = 0;
const stw_types::sint32 C_SdBueMessageRxTimeoutConfig::mhs32_IndexCustom = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent                Reference to parent
   \param[in] oq_UseAutoReceiveTimeoutFlag  Flag whether to use auto receive timeout or custom
   \param[in] ou32_ReceiveTimeoutValue      Receive timeout value
   \param[in] u32_LastKnownCycleTimeValue   Last known cycle time value
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxTimeoutConfig::C_SdBueMessageRxTimeoutConfig(C_OgePopUpDialog & orc_Parent,
                                                             const bool oq_UseAutoReceiveTimeoutFlag,
                                                             const uint32 ou32_ReceiveTimeoutValue,
                                                             const uint32 ou32_LastKnownCycleTimeValue,
                                                             const uint32 ou32_AutoReceiveTimeoutValue,
                                                             const QString & orc_NameForTitle) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageRxTimeoutConfig),
   mpc_ParentDialog(&orc_Parent),
   mu32_AutoReceiveTimeoutValue(ou32_AutoReceiveTimeoutValue)
{
   this->mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);
   //Register close
   this->mpc_ParentDialog->SetNotifyAndBlockClose(true);

   this->mpc_Ui->pc_SpinBoxTimeout->SetMinimumCustom(ou32_LastKnownCycleTimeValue);
   this->mpc_Ui->pc_SpinBoxTimeout->SetMaximumCustom(150010); // 3x maximum cycle time + 10

   this->InitStaticNames();

   //Title
   this->mpc_ParentDialog->SetTitle(orc_NameForTitle);
   this->mpc_ParentDialog->SetSubTitle(C_GtGetText::h_GetText("Receive Timeout"));

   if (oq_UseAutoReceiveTimeoutFlag == true)
   {
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxTimeoutConfig::mhs32_IndexAuto);
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(ou32_AutoReceiveTimeoutValue);
   }
   else
   {
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxTimeoutConfig::mhs32_IndexCustom);
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(ou32_ReceiveTimeoutValue);
   }

   this->m_HandleInactiveStates();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_SdBueMessageRxTimeoutConfig::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SdBueMessageRxTimeoutConfig::m_CancelClicked);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTimeoutActive,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueMessageRxTimeoutConfig::m_OnUseReceiveTimeoutChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxTimeoutConfig::~C_SdBueMessageRxTimeoutConfig()
{
   delete this->mpc_Ui;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::InitStaticNames(void) const
{
   //Texts
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Timeout Configuration"));
   this->mpc_Ui->pc_LabelTimeoutModeDescription->setText(C_GtGetText::h_GetText("Timeout Mode"));
   this->mpc_Ui->pc_LabelTimeoutValueDescription->setText(C_GtGetText::h_GetText("Timeout Value"));

   this->mpc_Ui->pc_ComboBoxTimeoutActive->addItem(C_GtGetText::h_GetText("Auto"));
   this->mpc_Ui->pc_ComboBoxTimeoutActive->addItem(C_GtGetText::h_GetText("Custom"));
   this->mpc_Ui->pc_SpinBoxTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));

   //Tool tips
   this->mpc_Ui->pc_LabelTimeoutModeDescription->SetToolTipInformation(
      C_GtGetText::h_GetText("Timeout Mode"),
      C_GtGetText::h_GetText("Available modes for receive timeout: "
                             "\nAuto: Auto calculation of timeout value (3x Cycle Time + 10ms)"
                             "\nCustom: Manually set timeout value"));

   this->mpc_Ui->pc_LabelTimeoutValueDescription->SetToolTipInformation(
      C_GtGetText::h_GetText("Timeout Value"),
      C_GtGetText::h_GetText("This property is relevant for code generation. "
                             "\nWithin this interval a valid message should be received. "
                             "\nOtherwise the Process Data Exchange Stack (implemented on device) will report an "
                             "error."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns auto receive timeout flag

   \return
   Auto receive timeout flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageRxTimeoutConfig::GetUseAutoReceiveTimeoutFlag(void) const
{
   bool q_Value;

   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_IndexAuto)
   {
      q_Value = true;
   }
   else
   {
      q_Value = false;
   }

   return q_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns receive timeout value

   \return
   Receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_SdBueMessageRxTimeoutConfig::GetReceiveTimeoutValue(void) const
{
   return static_cast<uint32>(this->mpc_Ui->pc_SpinBoxTimeout->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
void C_SdBueMessageRxTimeoutConfig::m_OkClicked(void)
{
   this->mpc_ParentDialog->accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::m_CancelClicked(void)
{
   this->mpc_ParentDialog->reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the inactive states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::m_HandleInactiveStates(void) const
{
   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_IndexAuto)
   {
      // Concrete timeout value only available in custom mode
      this->mpc_Ui->pc_SpinBoxTimeout->setEnabled(false);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxTimeout->setEnabled(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle combo box change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::m_OnUseReceiveTimeoutChanged(void) const
{
   this->m_HandleInactiveStates();

   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_IndexAuto)
   {
      this->m_UpdateAutoReceiveTimeoutValue();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update automatic receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::m_UpdateAutoReceiveTimeoutValue(void) const
{
   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_IndexAuto)
   {
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(static_cast<sintn>(this->mu32_AutoReceiveTimeoutValue));
   }
}
