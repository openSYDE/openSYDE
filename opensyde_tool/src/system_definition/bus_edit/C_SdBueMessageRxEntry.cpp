//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver edit entry (implementation)

   One message receiver edit entry

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdUtil.h"
#include "C_GtGetText.h"
#include "C_SdBueMessageRxEntry.h"
#include "ui_C_SdBueMessageRxEntry.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sint32 C_SdBueMessageRxEntry::mhs32_IndexAuto = 0;
const stw_types::sint32 C_SdBueMessageRxEntry::mhs32_IndexCustom = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxEntry::C_SdBueMessageRxEntry(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageRxEntry),
   mu32_NodeIndex(0),
   mu32_InterfaceIndex(0),
   mu32_LastKnownCycleTimeValue(0),
   mq_AlwaysHide(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_SpinBoxTimeout->SetMinimumCustom(10);
   this->mpc_Ui->pc_SpinBoxTimeout->SetMaximumCustom(150010);

   InitStaticNames();
   m_HandleInactiveStates();

   //Icon
   this->mc_NodeActive = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconNode.svg");
   this->mc_NodeInactive = C_SdUtil::h_InitStaticIcon(":/images/system_definition/IconNodeInactive.svg");
   this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_NodeInactive);

   connect(this->mpc_Ui->pc_CheckBoxActive, &stw_opensyde_gui_elements::C_OgeChxDefaultSmall::toggled, this,
           &C_SdBueMessageRxEntry::m_OnCheckBoxToggled);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTimeoutActive,
           static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueMessageRxEntry::m_OnUseReceiveTimeoutChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxTimeout, static_cast<void (QSpinBox::*)(sintn)>(&QSpinBox::valueChanged), this,
           &C_SdBueMessageRxEntry::m_OnReceiveTimeoutChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageRxEntry::~C_SdBueMessageRxEntry(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelTimeoutModeDescription->setText(C_GtGetText::h_GetText("Timeout Mode"));
   this->mpc_Ui->pc_LabelTimeoutValueDescription->setText(C_GtGetText::h_GetText("Timeout Value"));
   this->mpc_Ui->pc_ComboBoxTimeoutActive->addItem(C_GtGetText::h_GetText("Auto"));
   this->mpc_Ui->pc_ComboBoxTimeoutActive->addItem(C_GtGetText::h_GetText("Custom"));
   this->mpc_Ui->pc_SpinBoxTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));

   //Tool tips
   this->mpc_Ui->pc_LabelTimeoutModeDescription->SetToolTipInformation(C_GtGetText::h_GetText("Timeout Mode"),
                                                                       C_GtGetText::h_GetText(
                                                                          "Available modes for receive timeout: "
                                                                          "\nAuto: Auto calculation of timeout value. "
                                                                          "(3*Cyclic Time) + 10ms"
                                                                          "\nCustom: User specific timeout value"));

   this->mpc_Ui->pc_LabelTimeoutValueDescription->SetToolTipInformation(C_GtGetText::h_GetText("Timeout Value"),
                                                                        C_GtGetText::h_GetText(
                                                                           "This property is relevant for code generation. "
                                                                           "\nWithin this interval a valid message "
                                                                           "should be received. "
                                                                           "\nOtherwise the Process Data Exchange Stack "
                                                                           "(implemented on device) will report an error."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init

   \param[in] orc_EntryName                 Entry name
   \param[in] orc_NodeIndex                 Node index (ID)
   \param[in] orc_InterfaceIndex            Interface Index (ID)
   \param[in] orc_UseAutoReceiveTimeoutFlag Flag whether to use auto receive timeout or custom
   \param[in] orc_ReceiveTimeoutValue       Receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::Init(const QString & orc_EntryName, const uint32 ou32_NodeIndex,
                                 const uint32 ou32_InterfaceIndex, const bool oq_UseAutoReceiveTimeoutFlag,
                                 const uint32 ou32_ReceiveTimeoutValue)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_InterfaceIndex = ou32_InterfaceIndex;
   this->mpc_Ui->pc_CheckBoxActive->setText(orc_EntryName);
   if (oq_UseAutoReceiveTimeoutFlag == true)
   {
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxEntry::mhs32_IndexAuto);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxTimeoutActive->setCurrentIndex(C_SdBueMessageRxEntry::mhs32_IndexCustom);
   }
   this->mpc_Ui->pc_SpinBoxTimeout->setValue(ou32_ReceiveTimeoutValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update last known cycle time value

   \param[in] ou32_Value Last known cycle time value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetLastKnownCycleTimeValue(const uint32 ou32_Value)
{
   this->mu32_LastKnownCycleTimeValue = ou32_Value;
   this->mpc_Ui->pc_SpinBoxTimeout->SetMinimumCustom(this->mu32_LastKnownCycleTimeValue);
   m_UpdateAutoReceiveTimeoutValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set flag to always hide timeout section

   \param[in] oq_Hide Flag to always hide timeout section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetAlwaysHideTimeout(const bool oq_Hide)
{
   this->mq_AlwaysHide = oq_Hide;
   m_HandleInactiveStates();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if match

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface Index (ID)

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMessageRxEntry::DoesMatch(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex) const
{
   bool q_Retval;

   if ((this->mu32_NodeIndex == ou32_NodeIndex) && (this->mu32_InterfaceIndex == ou32_InterfaceIndex))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set checked state

   \param[in] oq_Checked new state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::SetChecked(const bool oq_Checked) const
{
   this->mpc_Ui->pc_CheckBoxActive->setChecked(oq_Checked);
   m_HandleInactiveStates();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle check box toggle
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_OnCheckBoxToggled(void)
{
   if (this->mpc_Ui->pc_CheckBoxActive->isChecked() == true)
   {
      this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_NodeActive);
   }
   else
   {
      this->mpc_Ui->pc_CheckBoxActive->setIcon(this->mc_NodeInactive);
   }
   m_HandleInactiveStates();
   Q_EMIT this->SigNodeToggled(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                               this->mpc_Ui->pc_CheckBoxActive->isChecked());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle combo box change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_OnUseReceiveTimeoutChanged(void)
{
   if (this->mpc_Ui->pc_CheckBoxActive->isChecked())
   {
      bool q_Value;

      if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxEntry::mhs32_IndexAuto)
      {
         q_Value = true;
         m_UpdateAutoReceiveTimeoutValue();
      }
      else
      {
         q_Value = false;
      }
      m_HandleInactiveStates();
      Q_EMIT this->SigNodeUseAutoReceiveTimeout(this->mu32_NodeIndex, this->mu32_InterfaceIndex, q_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle spin box change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_OnReceiveTimeoutChanged(void)
{
   if (this->mpc_Ui->pc_CheckBoxActive->isChecked())
   {
      Q_EMIT this->SigNodeReceiveTimeout(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                                         static_cast<uint32>(this->mpc_Ui->pc_SpinBoxTimeout->value()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle the inactive states
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_HandleInactiveStates(void) const
{
   if ((this->mpc_Ui->pc_CheckBoxActive->isChecked()) && (this->mq_AlwaysHide == false))
   {
      this->mpc_Ui->pc_GroupBoxMode->setVisible(true);
      this->mpc_Ui->pc_GroupBoxValue->setVisible(true);

      this->mpc_Ui->pc_ComboBoxTimeoutActive->setEnabled(true);

      if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxEntry::mhs32_IndexAuto)
      {
         //Partial
         this->mpc_Ui->pc_SpinBoxTimeout->setEnabled(false);
      }
      else
      {
         //Enable all (does not affect read only)
         this->mpc_Ui->pc_SpinBoxTimeout->setEnabled(true);
      }
   }
   else
   {
      //Disable all
      this->mpc_Ui->pc_GroupBoxMode->setVisible(false);
      this->mpc_Ui->pc_GroupBoxValue->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update automatic receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageRxEntry::m_UpdateAutoReceiveTimeoutValue(void) const
{
   if (this->mpc_Ui->pc_ComboBoxTimeoutActive->currentIndex() == C_SdBueMessageRxEntry::mhs32_IndexAuto)
   {
      this->mpc_Ui->pc_SpinBoxTimeout->setValue(static_cast<sintn>((this->mu32_LastKnownCycleTimeValue * 3UL) + 10UL));
   }
}
