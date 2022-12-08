//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with all status information labels
 (implementation)

   Widget with all status information labels

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamMetStatusBarWidget.hpp"
#include "ui_C_CamMetStatusBarWidget.h"

#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetStatusBarWidget::C_CamMetStatusBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_CamMetStatusBarWidget),
   mq_BusLoadWarning(false),
   mq_TxError(false),
   mu32_TxErrors(0U),
   mu32_TransmittedMessages(0U),
   mu32_FilteredMessages(0U),
   mu32_ActiveFilters(0U)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetStatusBarWidget::~C_CamMetStatusBarWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::InitStaticNames(void)
{
   this->SetBusLoad(0U, 0);
   this->SetTxErrors(0U);
   this->m_UpdateFilterLabel();

   this->mpc_Ui->pc_ActiveFiltersLabel->SetToolTipInformation(
      C_GtGetText::h_GetText("Active Receive Filters"),
      C_GtGetText::h_GetText("Number of active receive filters and "
                             "number of CAN messages which are filtered since measurement start."));
   this->mpc_Ui->pc_BusLoadLabel->SetToolTipInformation(
      C_GtGetText::h_GetText("Bus Load"),
      C_GtGetText::h_GetText("Approximate bus load."));
   this->mpc_Ui->pc_TxInfoLabel->SetToolTipInformation(
      C_GtGetText::h_GetText("Tx Errors"),
      C_GtGetText::h_GetText("Number of failed tries to transmit CAN messages."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the label with CAN bus load

   \param[in] ou8_BusLoad     CAN bus load in percentage
   \param[in] os32_CanBitrate CAN bitrate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::SetBusLoad(const uint8_t ou8_BusLoad, const int32_t os32_CanBitrate)
{
   const QString c_Bitrate =
      (os32_CanBitrate > 0) ? static_cast<QString>(" (@%1 kBit/s)").arg(QString::number(os32_CanBitrate)) : "";

   this->mpc_Ui->pc_BusLoadLabel->setText(static_cast<QString>(C_GtGetText::h_GetText("Bus Load: %1%%2")).
                                          arg(QString::number(ou8_BusLoad)).arg(c_Bitrate));

   // Prevent applying the stylesheet each call
   if ((this->mq_BusLoadWarning == false) &&
       (ou8_BusLoad >= 100U))
   {
      stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_BusLoadLabel, "Error",
                                                                      true);
      this->mq_BusLoadWarning = true;
   }
   else if ((this->mq_BusLoadWarning == true) &&
            (ou8_BusLoad < 100U))
   {
      stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_BusLoadLabel, "Error",
                                                                      false);
      this->mq_BusLoadWarning = false;
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the label with Tx errors

   \param[in] ou32_TxErrors Count of Tx errors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::SetTxErrors(const uint32_t ou32_TxErrors)
{
   this->mu32_TxErrors = ou32_TxErrors;
   this->m_UpdateTxLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the number of filtered CAN messages

   \param[in] ou32_NumTxMessages Number of transmitted messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::SetTransmittedMessages(const uint32_t ou32_NumTxMessages)
{
   this->mu32_TransmittedMessages = ou32_NumTxMessages;
   this->m_UpdateTxLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the number of filtered CAN messages

   \param[in] ou32_FilteredMessages Number of filtered messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::SetFilteredMessages(const uint32_t ou32_FilteredMessages)
{
   this->mu32_FilteredMessages = ou32_FilteredMessages;
   this->m_UpdateFilterLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the label with active filters

   \param[in] ou32_ActiveFilters Number of active CAN filters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::SetActiveFilters(const uint32_t ou32_ActiveFilters)
{
   this->mu32_ActiveFilters = ou32_ActiveFilters;
   this->m_UpdateFilterLabel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the filter label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::m_UpdateFilterLabel(void) const
{
   if (this->mu32_ActiveFilters > 0U)
   {
      this->mpc_Ui->pc_ActiveFiltersLabel->setText(static_cast<QString>(C_GtGetText::h_GetText("Active Filters: %1")).
                                                   arg(QString::number(this->mu32_ActiveFilters)));
   }
   else
   {
      this->mpc_Ui->pc_ActiveFiltersLabel->setText(C_GtGetText::h_GetText("No Active Filters"));
   }

   if ((this->mu32_FilteredMessages > 0U) ||
       (this->mu32_ActiveFilters > 0U))
   {
      this->mpc_Ui->pc_ActiveFiltersLabel->setText(this->mpc_Ui->pc_ActiveFiltersLabel->text() +
                                                   static_cast<QString>(C_GtGetText::h_GetText(
                                                                           " (Filtered Messages: %1)")).
                                                   arg(QString::number(this->mu32_FilteredMessages)));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the transmission information label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetStatusBarWidget::m_UpdateTxLabel(void)
{
   QString c_Text = C_GtGetText::h_GetText("Tx Messages: ");

   c_Text += QString::number(this->mu32_TransmittedMessages);

   if (this->mu32_TxErrors > 0U)
   {
      c_Text += C_GtGetText::h_GetText(" (Tx Errors: ");
      c_Text += QString::number(this->mu32_TxErrors);
      c_Text += ")";

      // Prevent applying the stylesheet each call
      if (this->mq_TxError == false)
      {
         stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TxInfoLabel, "Error",
                                                                         true);
         this->mq_TxError = true;
      }
   }
   else
   {
      // Prevent applying the stylesheet each call
      if (this->mq_TxError == true)
      {
         stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TxInfoLabel, "Error",
                                                                         false);
         this->mq_TxError = false;
      }
   }

   // Set label text
   this->mpc_Ui->pc_TxInfoLabel->setText(c_Text);
}
