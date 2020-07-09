//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for trace settings popup dialog

   Widget for trace settings popup dialog

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamMetSettingsPopup.h"
#include "ui_C_CamMetSettingsPopup.h"

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

   \param[in,out]  orc_Parent       Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetSettingsPopup::C_CamMetSettingsPopup(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CamMetSettingsPopup),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // initialize static names
   this->m_InitStaticNames();

   // initialize GUI
   this->m_InitStaticGUIElements();

   this->mpc_Ui->pc_SpinBoxTraceBuffer->SetMinimumCustom(500);
   this->mpc_Ui->pc_SpinBoxTraceBuffer->SetMaximumCustom(50000);

   // connects
   connect(this->mpc_Ui->pc_PushButtonSave, &C_OgePubDialog::clicked, this, &C_CamMetSettingsPopup::m_OnOk);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this, &C_CamMetSettingsPopup::m_OnCancel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetSettingsPopup::~C_CamMetSettingsPopup(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the initial values for the settings

   \param[in]  oq_DisplayTimestampAbsoluteTimeOfDay   New absolute timestamp setting
                                                      true: Timestamp with time of day
                                                      false: Timestamp beginning at start of measurement
   \param[in]  ou32_TraceBufferSize                   Trace buffer size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetSettingsPopup::SetValues(const bool oq_DisplayTimestampAbsoluteTimeOfDay,
                                      const uint32 ou32_TraceBufferSize) const
{
   this->mpc_Ui->pc_SpinBoxTraceBuffer->setValue(ou32_TraceBufferSize);

   if (oq_DisplayTimestampAbsoluteTimeOfDay == false)
   {
      this->mpc_Ui->pc_ComboBoxAbsoluteTimestampMode->setCurrentIndex(0);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxAbsoluteTimestampMode->setCurrentIndex(1);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns display style for absolute timestamp

   \retval   true: Timestamp with time of day
   \retval   false: Timestamp beginning at start of measurement
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMetSettingsPopup::GetDisplayTimestampAbsoluteTimeOfDay(void) const
{
   bool q_DisplayTimestampAbsoluteTimeOfDay;

   switch (this->mpc_Ui->pc_ComboBoxAbsoluteTimestampMode->currentIndex())
   {
   case 1:
      q_DisplayTimestampAbsoluteTimeOfDay = true;
      break;
   case 0: // Equals the default case
   default:
      q_DisplayTimestampAbsoluteTimeOfDay = false;
      break;
   }

   return q_DisplayTimestampAbsoluteTimeOfDay;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the configured size of the trace buffer

   \return
   Configured buffer size
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::uint32 C_CamMetSettingsPopup::GetTraceBufferSize(void) const
{
   return static_cast<uint32>(this->mpc_Ui->pc_SpinBoxTraceBuffer->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetSettingsPopup::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         this->m_OnOk();
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
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetSettingsPopup::m_InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Trace"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Settings"));

   this->mpc_Ui->pc_LabelSettings->setText(C_GtGetText::h_GetText("General"));

   this->mpc_Ui->pc_LabAbsoluteTimestampMode->setText(C_GtGetText::h_GetText("Absolute Time Mode"));
   this->mpc_Ui->pc_LabAbsoluteTimestampMode->SetToolTipInformation(C_GtGetText::h_GetText("Absolute Time Mode"),
                                                                    C_GtGetText::h_GetText(
                                                                       "Configuration for the time mode absolute:\n"
                                                                       "   - Timestamp beginning at start of measurement\n"
                                                                       "   - Timestamp with time of day"));
   this->mpc_Ui->pc_LabTraceBuffer->setText(C_GtGetText::h_GetText("Trace Buffer Size"));
   this->mpc_Ui->pc_LabTraceBuffer->SetToolTipInformation(C_GtGetText::h_GetText("Trace Buffer Size"),
                                                          C_GtGetText::h_GetText(
                                                             "Configuration of the maximum number of shown messages in the trace.\n"
                                                             "Will be set when starting the communication or when the trace will be cleared.\n"
                                                             "Default is 1000. A large number can have a negative effect on the performance."));

   this->mpc_Ui->pc_ComboBoxAbsoluteTimestampMode->clear();
   this->mpc_Ui->pc_ComboBoxAbsoluteTimestampMode->addItem(C_GtGetText::h_GetText("Since Measurement Start"));
   this->mpc_Ui->pc_ComboBoxAbsoluteTimestampMode->addItem(C_GtGetText::h_GetText("Time of Day"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize colors, fonts, ...
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetSettingsPopup::m_InitStaticGUIElements(void) const
{
   // label "Settings"
   this->mpc_Ui->pc_LabelSettings->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelSettings->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelSettings->SetFontPixel(14);

   // label "Absolute timestamp mode"
   this->mpc_Ui->pc_LabAbsoluteTimestampMode->SetForegroundColor(8);
   this->mpc_Ui->pc_LabAbsoluteTimestampMode->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabAbsoluteTimestampMode->SetFontPixel(12);

   // label "Absolute timestamp mode"
   this->mpc_Ui->pc_LabTraceBuffer->SetForegroundColor(8);
   this->mpc_Ui->pc_LabTraceBuffer->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabTraceBuffer->SetFontPixel(12);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetSettingsPopup::m_OnCancel(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetSettingsPopup::m_OnOk(void)
{
   this->mrc_ParentDialog.accept();
}
