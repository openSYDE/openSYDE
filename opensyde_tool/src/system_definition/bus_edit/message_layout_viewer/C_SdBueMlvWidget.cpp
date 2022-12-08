//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing message layout

   Widget for showing message layout

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>

#include "stwtypes.hpp"

#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeCbxText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdBueMlvWidget.hpp"
#include "ui_C_SdBueMlvWidget.h"

#include "C_SdBueMlvGraphicsView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvWidget::C_SdBueMlvWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMlvWidget),
   me_Protocol(C_OscCanProtocol::eLAYER2)
{
   mpc_Ui->setupUi(this);

   // create scene for graphics view
   this->mpc_Scene = new C_SdBueMlvGraphicsScene();

   this->mpc_Ui->pc_GraphicsView->setScene(this->mpc_Scene);

   this->mpc_Scene->setSceneRect(0.0, 0.0, static_cast<float64_t>(this->mpc_Ui->pc_GraphicsView->width()),
                                 static_cast<float64_t>(this->mpc_Ui->pc_GraphicsView->height()));

   this->mpc_Ui->pc_GraphicsView->horizontalScrollBar()->setValue(0);
   this->mpc_Ui->pc_GraphicsView->verticalScrollBar()->setValue(0);

   // Set initial disabled due to missing content
   this->mpc_Ui->pc_ComboBoxMultiplexer->setEnabled(false);

   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigChangeCursor, this, &C_SdBueMlvWidget::m_ChangeCursor);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigMessageUpdated, this, &C_SdBueMlvWidget::m_OnMessageUpdated);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigSignalActivated, this, &C_SdBueMlvWidget::m_OnSignalActivated);
   // MLV actions
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigAddSignal, this, &C_SdBueMlvWidget::SigAddSignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigAddSignalMultiplexed, this,
           &C_SdBueMlvWidget::SigAddSignalMultiplexed);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigCopySignal, this, &C_SdBueMlvWidget::SigCopySignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigCutSignal, this, &C_SdBueMlvWidget::SigCutSignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigPasteSignal, this, &C_SdBueMlvWidget::SigPasteSignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigDeleteSignal, this, &C_SdBueMlvWidget::SigDeleteSignal);
   //Connect for MLV tool tip
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigShowToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SdBueMlvGraphicsView::ShowToolTip);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigHideToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SdBueMlvGraphicsView::HideToolTip);
   connect(this->mpc_Ui->pc_GraphicsView, &C_SdBueMlvGraphicsView::SigShowToolTip, this->mpc_Scene,
           &C_SdBueMlvGraphicsScene::DisplayToolTip);

   this->InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvWidget::~C_SdBueMlvWidget(void)
{
   delete this->mpc_Ui;
   delete this->mpc_Scene;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelMultiplexer->setText(C_GtGetText::h_GetText("Multiplexer Signal"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out] opc_Value Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::SetMessageSyncManager(stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetMessageSyncManager(opc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new com protocol

   \param[in] ore_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value)
{
   this->me_Protocol = ore_Value;

   // Multiplexer is only relevant and used by layer 2 protocol
   this->mpc_Ui->pc_GroupBoxMultiplexer->setVisible(this->me_Protocol == C_OscCanProtocol::eLAYER2);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetComProtocol(ore_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select message

   \param[in] orc_MessageId Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::SelectMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   // Fill the message multiplexer signal combo box
   this->m_UpdateMultiplexerValues(orc_MessageId, false, 0);

   this->m_SelectMessage(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select signal

   \param[in] orc_MessageId     Message identification indices
   \param[in] ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::SelectSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                    const uint32_t ou32_SignalIndex)
{
   const C_OscCanSignal * const pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(orc_MessageId,
                                                                                          ou32_SignalIndex);
   uint16_t u16_MultiplexerValue = 0;
   bool q_SetMultiplexerValue = false;

   if ((pc_Signal != NULL) &&
       (pc_Signal->e_MultiplexerType == C_OscCanSignal::eMUX_MULTIPLEXED_SIGNAL))
   {
      // Change the multiplexer value only if it is a multiplexed signal. All other types are always visible for
      // the concrete CAN message
      q_SetMultiplexerValue = true;
      u16_MultiplexerValue = pc_Signal->u16_MultiplexValue;
   }

   // Fill the message multiplexer signal combo box
   this->m_UpdateMultiplexerValues(orc_MessageId, q_SetMultiplexerValue, u16_MultiplexerValue);

   this->m_SelectMessage(orc_MessageId);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetSignal(ou32_SignalIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::RefreshColors(void)
{
   this->mpc_Scene->RefreshColors();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided resize event

   Adapting of the scene rectangle

   \param[in,out] opc_Event  Pointer to resize event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->setSceneRect(0.0, 0.0,
                                    static_cast<float64_t>(this->mpc_Ui->pc_GraphicsView->width()),
                                    static_cast<float64_t>(this->mpc_Ui->pc_GraphicsView->height()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select message

   \param[in] orc_MessageId Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::m_SelectMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   if (this->mpc_Scene != NULL)
   {
      bool q_MultiplexedMsg = false;
      uint16_t u16_MultiplexerValue = 0;

      if ((this->me_Protocol == C_OscCanProtocol::eLAYER2) &&
          (this->mpc_Ui->pc_ComboBoxMultiplexer->isEnabled() == true))
      {
         q_MultiplexedMsg = true;
         u16_MultiplexerValue = this->m_GetSelectedMultiplexerValues();
      }

      this->mpc_Scene->SetMessage(orc_MessageId, q_MultiplexedMsg, u16_MultiplexerValue);
   }

   // Save for UpdateMultiplexerValues
   this->mc_PreviousMessageId = orc_MessageId;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::m_ChangeCursor(const Qt::CursorShape oe_Cursor)
{
   this->setCursor(oe_Cursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal position in message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::m_OnMessageUpdated(void)
{
   Q_EMIT (this->SigMessageUpdated());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  On change of multiplexer value changed in combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::m_OnMultiplexerValueChanged(void)
{
   if ((this->mpc_Scene != NULL) &&
       (this->me_Protocol == C_OscCanProtocol::eLAYER2) &&
       (this->mpc_Ui->pc_ComboBoxMultiplexer->isEnabled() == true))
   {
      this->mpc_Scene->SetMultiplexValue(this->m_GetSelectedMultiplexerValues());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of active signal via selector widget

   \param[in] ou32_SignalIndex Active signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::m_OnSignalActivated(const uint32_t ou32_SignalIndex)
{
   Q_EMIT (this->SigSignalActivated(ou32_SignalIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the combo box with all possible multiplexer values

   Set start value in priority order:
   1. If oq_SetSpecificMultiplexerValue is true set ou16_MultiplexerValue if it still exists
   2. If orc_MessageId is the same message as before set the old selected value if it still exists
   3. The first value

   \param[in] orc_MessageId                     Message identification indices
   \param[in] oq_SetSpecificMultiplexerValue    Flag if a specific multiplexer value shall be selected if available
   \param[in] ou16_MultiplexerValue             Multiplexer value to set if oq_SetSpecificMultiplexerValue is true
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvWidget::m_UpdateMultiplexerValues(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                 const bool oq_SetSpecificMultiplexerValue,
                                                 const uint16_t ou16_MultiplexerValue)
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxMultiplexer, static_cast<void (QComboBox::*)(
                                                                   int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueMlvWidget::m_OnMultiplexerValueChanged);

   // Only relevant and used by layer 2 protocol
   if (this->me_Protocol == C_OscCanProtocol::eLAYER2)
   {
      bool q_TryToSetPreviousValue = false;
      uint16_t u16_PreviousValue = 0;
      const C_OscCanMessage * const pc_Message =
         stw::opensyde_gui_logic::C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

      if ((orc_MessageId == this->mc_PreviousMessageId) &&
          (this->mpc_Ui->pc_ComboBoxMultiplexer->isEnabled() == true))
      {
         // Special case: Same message which could have the same multiplexer values
         u16_PreviousValue = this->m_GetSelectedMultiplexerValues();
         q_TryToSetPreviousValue = true;
      }

      this->mpc_Ui->pc_ComboBoxMultiplexer->clear();
      this->mc_MultiplexerValues.clear();

      tgl_assert(pc_Message != NULL);
      if (pc_Message != NULL)
      {
         uint32_t u32_MultiplexerIndex;

         // Check the message
         if (pc_Message->IsMultiplexed(&u32_MultiplexerIndex) == false)
         {
            // No selection possible
            this->mpc_Ui->pc_ComboBoxMultiplexer->addItem(C_GtGetText::h_GetText("No Multiplexer"));
            this->mpc_Ui->pc_ComboBoxMultiplexer->setEnabled(false);
         }
         else
         {
            std::set<uint16_t> c_MultiplexerValues;
            const C_OscNodeDataPoolListElement * const pc_MultiplexerElement =
               stw::opensyde_gui_logic::C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(
                  orc_MessageId,
                  u32_MultiplexerIndex);
            tgl_assert(pc_MultiplexerElement != NULL);

            pc_Message->GetMultiplexerValues(c_MultiplexerValues);

            if ((pc_MultiplexerElement != NULL) &&
                (c_MultiplexerValues.size() > 0))
            {
               // Selection possible
               std::set<uint16_t>::const_iterator c_ItValue;
               const QString c_SignalName = static_cast<QString>(pc_MultiplexerElement->c_Name.c_str()) +
                                            static_cast<QString>(" = ");
               QString c_SpecificEntry = "";

               // Add all multiplexer values to the combo box
               for (c_ItValue = c_MultiplexerValues.begin(); c_ItValue != c_MultiplexerValues.end(); ++c_ItValue)
               {
                  QString c_ComboBoxEntry = c_SignalName + QString::number(*c_ItValue);
                  this->mpc_Ui->pc_ComboBoxMultiplexer->addItem(c_ComboBoxEntry);
                  this->mc_MultiplexerValues.insert(std::pair<QString, uint16_t>(c_ComboBoxEntry, *c_ItValue));
               }

               this->mpc_Ui->pc_ComboBoxMultiplexer->setEnabled(true);

               if ((oq_SetSpecificMultiplexerValue == true) &&
                   (c_MultiplexerValues.find(ou16_MultiplexerValue) != c_MultiplexerValues.end()))
               {
                  // Set the multiplexer value of the specific multiplexer value
                  c_SpecificEntry = this->m_GetComboBoxEntryByMultiplexerValue(ou16_MultiplexerValue);
               }
               else if ((q_TryToSetPreviousValue == true) &&
                        (c_MultiplexerValues.find(u16_PreviousValue) != c_MultiplexerValues.end()))
               {
                  // Set the multiplexer value of the last call
                  c_SpecificEntry = this->m_GetComboBoxEntryByMultiplexerValue(u16_PreviousValue);
               }
               else
               {
                  // Nothing to do
               }

               if (c_SpecificEntry != "")
               {
                  // Set a specific entry
                  this->mpc_Ui->pc_ComboBoxMultiplexer->setCurrentText(c_SpecificEntry);
               }
            }
            else
            {
               // No selection due to defined multiplexer signal, but no multiplexed signals
               this->mpc_Ui->pc_ComboBoxMultiplexer->addItem(C_GtGetText::h_GetText("No multiplexed signals defined"));
               this->mpc_Ui->pc_ComboBoxMultiplexer->setEnabled(false);
            }
         }
      }
   }

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxMultiplexer, static_cast<void (QComboBox::*)(
                                                                int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMlvWidget::m_OnMultiplexerValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the selected multiplexer values

   \return
   Selected multiplexer value
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SdBueMlvWidget::m_GetSelectedMultiplexerValues(void) const
{
   uint16_t u16_Value = 0U;

   const std::map<QString, uint16_t>::const_iterator c_ItValue =
      this->mc_MultiplexerValues.find(this->mpc_Ui->pc_ComboBoxMultiplexer->currentText());

   if (c_ItValue != this->mc_MultiplexerValues.end())
   {
      u16_Value = c_ItValue->second;
   }
   else
   {
      tgl_assert(false);
   }

   return u16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the string of the combo box associated to the given multiplexer signal value

   \param[in]       ou16_Value     Multiplexer signal value

   \retval   Empty string       No combo box entry found for ou16_Value
   \retval   Not empty string   Combo box entry string for ou16_Value
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdBueMlvWidget::m_GetComboBoxEntryByMultiplexerValue(const uint16_t ou16_Value) const
{
   QString c_Entry = "";

   std::map<QString, uint16_t>::const_iterator c_ItValue;

   for (c_ItValue = this->mc_MultiplexerValues.begin(); c_ItValue != this->mc_MultiplexerValues.end(); ++c_ItValue)
   {
      if (c_ItValue->second == ou16_Value)
      {
         c_Entry = c_ItValue->first;
         break;
      }
   }

   return c_Entry;
}
