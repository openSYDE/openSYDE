//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing messages or signals in a table

   Widget for showing messages or signals in a table

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include "C_SdBueMessageSignalTableWidget.h"
#include "ui_C_SdBueMessageSignalTableWidget.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageSignalTableWidget::C_SdBueMessageSignalTableWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageSignalTableWidget),
   mq_Messages(true)
{
   mpc_Ui->setupUi(this);

   this->InitStaticNames();

   //Initial setup
   this->mpc_Ui->pc_RadioButtonMessages->setChecked(true);
   this->mpc_Ui->pc_RadioButtonSignals->setChecked(false);
   this->mpc_Ui->pc_TableViewMessages->setVisible(true);
   this->mpc_Ui->pc_TableViewSignals->setVisible(false);
   this->mpc_Ui->pc_LabelNoMessages->setVisible(false);
   this->mpc_Ui->pc_LabelNoSignals->setVisible(false);

   connect(this->mpc_Ui->pc_RadioButtonMessages, &stw_opensyde_gui_elements::C_OgeRabCheckBox::toggled, this,
           &C_SdBueMessageSignalTableWidget::m_OnMessagesToggle);
   connect(this->mpc_Ui->pc_RadioButtonSignals, &stw_opensyde_gui_elements::C_OgeRabCheckBox::toggled, this,
           &C_SdBueMessageSignalTableWidget::m_OnSignalsToggle);
   //Selection
   connect(this->mpc_Ui->pc_TableViewMessages, &C_SdBueMessageTableView::SigMessageSelected, this,
           &C_SdBueMessageSignalTableWidget::m_OnMessageSelected);
   connect(this->mpc_Ui->pc_TableViewSignals, &C_SdBueSignalTableView::SigSignalSelected, this,
           &C_SdBueMessageSignalTableWidget::m_OnSignalSelected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageSignalTableWidget::~C_SdBueMessageSignalTableWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const
{
   this->mpc_Ui->pc_TableViewMessages->SetMessageSyncManager(opc_Value);
   this->mpc_Ui->pc_TableViewSignals->SetMessageSyncManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal data changes to table
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::UpdateData(void)
{
   this->mpc_Ui->pc_TableViewMessages->UpdateData();
   this->mpc_Ui->pc_TableViewSignals->UpdateData();

   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_RadioButtonMessages->setText(C_GtGetText::h_GetText("Message View"));
   this->mpc_Ui->pc_RadioButtonSignals->setText(C_GtGetText::h_GetText("Signal View"));

   this->mpc_Ui->pc_LabelNoMessages->setText(C_GtGetText::h_GetText("No messages declared"));
   this->mpc_Ui->pc_LabelNoSignals->setText(C_GtGetText::h_GetText("No signals declared"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings

   \param[in]  orc_MessageValues    Message values
   \param[in]  orc_SignalValues     Signal values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::LoadUserSettings(const std::vector<stw_types::sint32> & orc_MessageValues,
                                                       const std::vector<stw_types::sint32> & orc_SignalValues) const
{
   this->mpc_Ui->pc_TableViewSignals->LoadUserSettings(orc_SignalValues);
   this->mpc_Ui->pc_TableViewMessages->LoadUserSettings(orc_MessageValues);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings

   \param[in,out]  orc_MessageValues   Message values
   \param[in,out]  orc_SignalValues    Signal values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::SaveUserSettings(std::vector<stw_types::sint32> & orc_MessageValues,
                                                       std::vector<stw_types::sint32> & orc_SignalValues) const
{
   this->mpc_Ui->pc_TableViewSignals->SaveUserSettings(orc_SignalValues);
   this->mpc_Ui->pc_TableViewMessages->SaveUserSettings(orc_MessageValues);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle messages radio button toggle

   \param[in]  orq_Checked    Radio button state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::m_OnMessagesToggle(const bool & orq_Checked)
{
   this->mq_Messages = orq_Checked;

   this->m_UpdateUi();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle signals radio button toggle

   \param[in]  orq_Checked    Radio button state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::m_OnSignalsToggle(const bool & orq_Checked)
{
   m_OnMessagesToggle(orq_Checked == false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle message selection

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::m_OnMessageSelected(
   const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT this->SigMessageSelected(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle signal selection

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::m_OnSignalSelected(
   const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
   const stw_types::uint32 & oru32_SignalIndex)
{
   Q_EMIT this->SigSignalSelected(orc_MessageId, oru32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalTableWidget::m_UpdateUi(void)
{
   if (this->mq_Messages == true)
   {
      if (this->mpc_Ui->pc_TableViewMessages->GetCountRows() > 0)
      {
         this->mpc_Ui->pc_LabelNoMessages->setVisible(false);
         this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
         this->mpc_Ui->pc_TableViewMessages->setVisible(true);
      }
      else
      {
         this->mpc_Ui->pc_TableViewMessages->setVisible(false);
         this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
         this->mpc_Ui->pc_LabelNoMessages->setVisible(true);
      }

      this->mpc_Ui->pc_TableViewSignals->setVisible(false);
      this->mpc_Ui->pc_LabelNoSignals->setVisible(false);
   }
   else
   {
      if (this->mpc_Ui->pc_TableViewSignals->GetCountRows() > 0)
      {
         this->mpc_Ui->pc_LabelNoSignals->setVisible(false);
         this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
         this->mpc_Ui->pc_TableViewSignals->setVisible(true);
      }
      else
      {
         this->mpc_Ui->pc_TableViewSignals->setVisible(false);
         this->mpc_Ui->pc_VerticalSpacerBottom->changeSize(0, 40, QSizePolicy::Fixed, QSizePolicy::Expanding);
         this->mpc_Ui->pc_LabelNoSignals->setVisible(true);
      }

      this->mpc_Ui->pc_TableViewMessages->setVisible(false);
      this->mpc_Ui->pc_LabelNoMessages->setVisible(false);
   }

   this->updateGeometry();
}
