//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver timeout configuration

   One message receiver timeout configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"

#include "C_SdBueMessageRxTimeoutConfig.hpp"
#include "ui_C_SdBueMessageRxTimeoutConfig.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_AUTO = 0;
const int32_t C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_CUSTOM = 1;
const int32_t C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_DISABLED = 2;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent                      Reference to parent
   \param[in]      oe_ReceiveTimeoutMode           Receive timeout mode
   \param[in]      oq_TxMethodOnEvent              Flag if Tx method is on event
   \param[in]      oq_DisablePossible              Flag if the timeout configuration can be disabled
   \param[in]      ou32_ReceiveTimeoutValue        Receive timeout value
   \param[in]      ou32_LastKnownCycleTimeValue    Last known cycle time value
   \param[in]      ou32_AutoReceiveTimeoutValue    Calculated default timeout value
   \param[in]      orc_NameForTitle                Dialog title name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxTimeoutConfig::C_SdBueMessageRxTimeoutConfig(C_OgePopUpDialog & orc_Parent,
                                                             const C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_ReceiveTimeoutMode, const bool oq_TxMethodOnEvent, const bool oq_DisablePossible, const uint32_t ou32_ReceiveTimeoutValue, const uint32_t ou32_LastKnownCycleTimeValue, const uint32_t ou32_AutoReceiveTimeoutValue,
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

   this->InitStaticNames();

   if (oq_TxMethodOnEvent == true)
   {
      // No restriction of a defined timeout time due to not known cycle time
      this->mpc_Ui->pc_SpinBoxTimeout->SetMinimumCustom(1);

      //maximum permitted RX timeout is limited by:
      //* 3x maximum cycle time + 10  (3 * 50000 + 10)
      //* uint16 variable used in openSYDE server -> 65535
      this->mpc_Ui->pc_SpinBoxTimeout->SetMaximumCustom(65535);
   }
   else
   {
      // In case of a not on event message, the timeout time can not be disabled
      if (oq_DisablePossible == false)
      {
         //lint -e{1938}  static const is guaranteed preinitialized before main
         this->mpc_Ui->pc_ComboBoxTimeoutActive->SetItemState(mhs32_INDEX_DISABLED, false);
      }

      this->mpc_Ui->pc_SpinBoxTimeout->SetMinimumCustom(ou32_LastKnownCycleTimeValue);

      // The maximum cycle time is further limited by the effect the cycle time has on the permitted ECeS TX gap.
      // The TX-gap is a uint8 on the ECU sude and is calculated as "(timeout/cycletime) = 1".
      // In practice is makes no sense to send a message in high intervals but to check only after a long time.
      // So we do limit the timeout to (cycle-time * 100);
      // Even the 100 is way over what makes sense in a real application.
      // To keep complexity low we also apply this to non-ECeS.
      // In case of CANopen 0 could be possible, but does not make any sens as maximum value.
      int32_t s32_MaxCycleTime = static_cast<int32_t>(ou32_LastKnownCycleTimeValue * 100);
      if ((s32_MaxCycleTime > 65535) ||
          (s32_MaxCycleTime == 0))
      {
         //limit to 16 bit
         s32_MaxCycleTime = 65535;
      }
      this->mpc_Ui->pc_SpinBoxTimeout->SetMaximumCustom(s32_MaxCycleTime);
   }

   //Title
   this->mpc_ParentDialog->SetTitle(orc_NameForTitle);
   this->mpc_ParentDialog->SetSubTitle(C_GtGetText::h_GetText("Receive Timeout"));

   if ((oe_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED) &&
       (oq_DisablePossible == true))
   {
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_DISABLED);
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(ou32_AutoReceiveTimeoutValue);
   }
   else if (oe_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM)
   {
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_CUSTOM);
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(ou32_ReceiveTimeoutValue);
   }
   else
   {
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_AUTO);
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(ou32_AutoReceiveTimeoutValue);
   }

   this->m_HandleInactiveStates();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this,
           &C_SdBueMessageRxTimeoutConfig::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SdBueMessageRxTimeoutConfig::m_CancelClicked);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTimeoutActive,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueMessageRxTimeoutConfig::m_OnReceiveTimeoutModeChanged);
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
   this->mpc_Ui->pc_ComboBoxTimeoutActive->addItem(C_GtGetText::h_GetText("Disabled"));
   this->mpc_Ui->pc_SpinBoxTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));

   //Tool tips
   this->mpc_Ui->pc_LabelTimeoutModeDescription->SetToolTipInformation(
      C_GtGetText::h_GetText("Timeout Mode"),
      C_GtGetText::h_GetText("Available modes for receive timeout: "
                             "\nAuto: Auto calculation of timeout value (3x Cycle Time + 10ms)"
                             "\nCustom: Manually set timeout value"
                             "\nDisabled: Timeout monitoring deactivated (only with Tx Method: On Event)"));

   this->mpc_Ui->pc_LabelTimeoutValueDescription->SetToolTipInformation(
      C_GtGetText::h_GetText("Timeout Value"),
      C_GtGetText::h_GetText("This property is relevant for source code file generation. "
                             "\nWithin this interval a valid message should be received. "
                             "\nOtherwise the COMM stack (implemented on device) will report an "
                             "error."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns receive timeout mode

   \return
   Auto receive timeout mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdNodeCanMessage::E_RxTimeoutMode C_SdBueMessageRxTimeoutConfig::GetReceiveTimeoutMode(void) const
{
   C_PuiSdNodeCanMessage::E_RxTimeoutMode e_Value;

   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_AUTO)
   {
      e_Value = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO;
   }
   else if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_CUSTOM)
   {
      e_Value = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_CUSTOM;
   }
   else
   {
      e_Value = C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_DISABLED;
   }

   return e_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns receive timeout value

   \return
   Receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SdBueMessageRxTimeoutConfig::GetReceiveTimeoutValue(void) const
{
   uint32_t u32_Return;

   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() != C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_DISABLED)
   {
      u32_Return = static_cast<uint32_t>(this->mpc_Ui->pc_SpinBoxTimeout->value());
   }
   else
   {
      // Special case disabled: The zero is used as identification for disabled receive timeout on server side
      u32_Return = 0U;
   }

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
   // Concrete timeout value only available in custom mode
   this->mpc_Ui->pc_SpinBoxTimeout->setEnabled((this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() ==
                                                C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_CUSTOM));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle combo box change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxTimeoutConfig::m_OnReceiveTimeoutModeChanged(void) const
{
   this->m_HandleInactiveStates();

   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_AUTO)
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
   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxTimeoutConfig::mhs32_INDEX_AUTO)
   {
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(static_cast<int32_t>(this->mu32_AutoReceiveTimeoutValue));
   }
}
