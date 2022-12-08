//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing messages and / or signals

   Widget for editing messages and / or signals

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_UsHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdBueMessageSignalEditWidget.hpp"
#include "ui_C_SdBueMessageSignalEditWidget.h"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageSignalEditWidget::C_SdBueMessageSignalEditWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageSignalEditWidget),
   me_ProtocolType(C_OscCanProtocol::eLAYER2)
{
   // Init UI
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_MsgPropertiesWidget->setVisible(false);
   this->mpc_Ui->pc_SignalLabel->setVisible(false);
   this->mpc_Ui->pc_CoDisabledInfoWidget->setVisible(false);

   this->mpc_Ui->pc_CoDisabledInfoIconLabel->SetSvg("://images/Info_Icon_MessageBox.svg");
   this->mpc_Ui->pc_CoDisabledInfoNoteLabel->SetForegroundColor(3);
   this->mpc_Ui->pc_CoDisabledInfoNoteLabel->SetFontPixel(14, false, true);
   this->mpc_Ui->pc_CoDisabledInfoLabel->SetForegroundColor(7);
   this->mpc_Ui->pc_CoDisabledInfoLabel->SetFontPixel(14, false, false);

   this->InitStaticNames();

   //Init splitter
   this->mpc_Ui->pc_Splitter->SetMargins(9, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 1);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1000);

   connect(this->mpc_Ui->pc_MsgPropertiesWidget, &C_SdBueMessagePropertiesWidget::SigMessageIdChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnMessageIdChange);
   connect(this->mpc_Ui->pc_MsgPropertiesWidget, &C_SdBueMessagePropertiesWidget::SigMessageNameChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnMessageNameChange);
   connect(this->mpc_Ui->pc_MsgPropertiesWidget, &C_SdBueMessagePropertiesWidget::SigMessageDlcChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnMessageDlcChange);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigMessageUpdated, this,
           &C_SdBueMessageSignalEditWidget::m_OnSignalUpdatedViaSelector);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigUpdateMlv, this,
           &C_SdBueMessageSignalEditWidget::m_OnSignalUpdatedViaProperties);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigNameChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnSignalNameChanged);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigStartBitChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnSignalStartBitChanged);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigSignalPositionChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnSignalPositionChanged);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigSignalActivated, this,
           &C_SdBueMessageSignalEditWidget::m_OnSignalActivated);
   //Error
   connect(this->mpc_Ui->pc_MsgPropertiesWidget, &C_SdBueMessagePropertiesWidget::SigRecheckError, this,
           &C_SdBueMessageSignalEditWidget::m_RecheckErrorGlobal);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigRecheckError, this,
           &C_SdBueMessageSignalEditWidget::m_RecheckErrorGlobal);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigRecheckError, this,
           &C_SdBueMessageSignalEditWidget::m_RecheckError);
   //Change
   connect(this->mpc_Ui->pc_MsgPropertiesWidget, &C_SdBueMessagePropertiesWidget::SigChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnChange);
   connect(this->mpc_Ui->pc_SigPropertiesWidget, &C_SdBueSignalPropertiesWidget::SigChanged, this,
           &C_SdBueMessageSignalEditWidget::m_OnChange);
   connect(this->mpc_Ui->pc_MsgPropertiesWidget, &C_SdBueMessagePropertiesWidget::SigMessageRxChanged, this,
           &C_SdBueMessageSignalEditWidget::SigMessageRxChanged);

   // MLV actions
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigAddSignal, this,
           &C_SdBueMessageSignalEditWidget::SigAddSignal);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigAddSignalMultiplexed, this,
           &C_SdBueMessageSignalEditWidget::SigAddSignalMultiplexed);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigCopySignal, this,
           &C_SdBueMessageSignalEditWidget::SigCopySignal);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigCutSignal, this,
           &C_SdBueMessageSignalEditWidget::SigCutSignal);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigPasteSignal, this,
           &C_SdBueMessageSignalEditWidget::SigPasteSignal);
   connect(this->mpc_Ui->pc_MsgLayoutViewerWidget, &C_SdBueMlvWidget::SigDeleteSignal, this,
           &C_SdBueMessageSignalEditWidget::SigDeleteSignal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageSignalEditWidget::~C_SdBueMessageSignalEditWidget(void)
{
   //Store splitter position
   const QList<int32_t> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      const int32_t s32_Size = c_Sizes.at(0);
      //Avoid saving invalid values
      if (s32_Size > 0)
      {
         C_UsHandler::h_GetInstance()->SetSdBusEditLayoutSplitterHorizontal(s32_Size);
      }
   }
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SetMessageSyncManager(
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const
{
   this->mpc_Ui->pc_MsgPropertiesWidget->SetMessageSyncManager(opc_Value);
   this->mpc_Ui->pc_SigPropertiesWidget->SetMessageSyncManager(opc_Value);
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SetMessageSyncManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new com protocol

   \param[in]  ore_Value   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SetComProtocol(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Value)
{
   this->me_ProtocolType = ore_Value;
   this->mpc_Ui->pc_MsgPropertiesWidget->SetComProtocol(ore_Value);
   this->mpc_Ui->pc_SigPropertiesWidget->SetComProtocol(ore_Value);
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SetComProtocol(ore_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SelectMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectMessage(orc_MessageId);
   this->mpc_Ui->pc_MsgPropertiesWidget->setVisible(true);
   this->mpc_Ui->pc_SigPropertiesWidget->setVisible(false);
   this->mpc_Ui->pc_MessageLabel->setVisible(true);
   this->mpc_Ui->pc_SignalLabel->setVisible(false);
   this->mpc_Ui->pc_MsgPropertiesWidget->SetMessageId(true, orc_MessageId);
   //also initialize signal properties to have valid indices for everything besides signal index itself
   this->mpc_Ui->pc_SigPropertiesWidget->SetSignalId(orc_MessageId, 0U);

   this->m_HandleDisabledPdoInfo(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SelectSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                  const uint32_t & oru32_SignalIndex) const
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
   this->mpc_Ui->pc_MsgPropertiesWidget->setVisible(false);
   this->mpc_Ui->pc_MsgPropertiesWidget->SetMessageId(false, C_OscCanMessageIdentificationIndices());
   this->mpc_Ui->pc_SigPropertiesWidget->setVisible(true);
   this->mpc_Ui->pc_MessageLabel->setVisible(false);
   this->mpc_Ui->pc_SignalLabel->setVisible(true);
   this->mpc_Ui->pc_SigPropertiesWidget->SetSignalId(orc_MessageId, oru32_SignalIndex);

   this->m_HandleDisabledPdoInfo(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Partial select signal

   For avoiding problems with event handling of some spin boxes, the signal properties widget should not be
   reloaded when not necessary.
   Only refreshing the MLV and the PDO disabled info

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::PartialSelectSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint32_t & oru32_SignalIndex) const
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
   this->m_HandleDisabledPdoInfo(orc_MessageId);
   // In case of a changed DLC, check the message position again
   this->mpc_Ui->pc_SigPropertiesWidget->RecheckMessagePosition();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Hide
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::Hide(void)
{
   this->setVisible(false);
   this->mpc_Ui->pc_MsgPropertiesWidget->SetMessageId(false, C_OscCanMessageIdentificationIndices());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node 'mode' of the widget with all necessary indexes

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  orc_DatapoolIndexes  All Datapool indexes associated to the same protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SetNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                               const std::vector<uint32_t> & orc_DatapoolIndexes) const
{
   this->mpc_Ui->pc_MsgPropertiesWidget->SetNodeId(ou32_NodeIndex, ou32_InterfaceIndex, orc_DatapoolIndexes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the bus 'mode' of the widget

   \param[in]  ou32_BusIndex  Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SetBusId(const uint32_t ou32_BusIndex) const
{
   this->mpc_Ui->pc_MsgPropertiesWidget->SetBusId(ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::OnConnectionChange(void) const
{
   //Only relevant if visible
   if (this->mpc_Ui->pc_MsgPropertiesWidget->isVisible() == true)
   {
      this->mpc_Ui->pc_MsgPropertiesWidget->OnConnectionChange();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In case of a disconnected node update of the unique message ids

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::OnNodeDisconnected(const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_InterfaceIndex)
{
   //Only relevant if visible
   if (this->mpc_Ui->pc_MsgPropertiesWidget->isVisible() == true)
   {
      this->mpc_Ui->pc_MsgPropertiesWidget->OnNodeDisconnected(ou32_NodeIndex, ou32_InterfaceIndex);
   }
   else if (this->mpc_Ui->pc_SigPropertiesWidget->isVisible() == true)
   {
      this->mpc_Ui->pc_SigPropertiesWidget->OnNodeDisconnected(ou32_NodeIndex, ou32_InterfaceIndex);
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal count in message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::OnSignalCountOfMessageChanged(
   const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   //Check if relevant
   bool q_Found = false;
   const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> c_MatchingMessageIds =
      this->mpc_Ui->pc_MsgPropertiesWidget->GetMatchingMessageIds();

   for (uint32_t u32_ItMessage = 0; u32_ItMessage < c_MatchingMessageIds.size(); ++u32_ItMessage)
   {
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & rc_MessageId =
         c_MatchingMessageIds[u32_ItMessage];
      if (rc_MessageId == orc_MessageId)
      {
         q_Found = true;
      }
   }
   if (q_Found == true)
   {
      this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectMessage(orc_MessageId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_SignalLabel->setText(C_GtGetText::h_GetText("Signal Properties"));
   this->mpc_Ui->pc_MessageLabel->setText(C_GtGetText::h_GetText("Message Properties"));
   this->mpc_Ui->pc_CoDisabledInfoNoteLabel->setText(C_GtGetText::h_GetText("Note:"));
   this->mpc_Ui->pc_CoDisabledInfoLabel->setText(
      C_GtGetText::h_GetText("This PDO is disabled. Unique COB-ID check is inactive on disabled PDOs."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selecting the name of the actual element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::SelectName(void) const
{
   if (this->isVisible() == true)
   {
      if (this->mpc_Ui->pc_MsgPropertiesWidget->isVisible() == true)
      {
         this->mpc_Ui->pc_MsgPropertiesWidget->SelectName();
      }
      else if (this->mpc_Ui->pc_SigPropertiesWidget->isVisible() == true)
      {
         this->mpc_Ui->pc_SigPropertiesWidget->SelectName();
      }
      else
      {
         // nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload the current selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::RefreshSelection(void)
{
   bool q_MessageSelected;
   bool q_SignalSelected;
   C_OscCanMessageIdentificationIndices c_MessageId;
   uint32_t u32_SignalIndex;

   this->GetLastSelection(q_MessageSelected, NULL,
                          q_SignalSelected, NULL,
                          &c_MessageId, &u32_SignalIndex);

   if (q_SignalSelected == true)
   {
      this->PartialSelectSignal(c_MessageId, u32_SignalIndex);
   }
   else if (q_MessageSelected == true)
   {
      this->SelectMessage(c_MessageId);
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last selection info

   \param[out]  orq_MessageSelected    Set flag if there is a selected message
   \param[out]  opc_MessageName        Optional: Selected message name if any
   \param[out]  orq_SignalSelected     Flag if signal selected
   \param[out]  opc_SignalName         Optional: Selected signal name if any
   \param[out]  opc_MessageId          Optional: Selected message id if any
   \param[out]  opu32_SignalIndex      Optional: Selected signal index if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::GetLastSelection(bool & orq_MessageSelected, QString * const opc_MessageName,
                                                      bool & orq_SignalSelected, QString * const opc_SignalName,
                                                      C_OscCanMessageIdentificationIndices * const opc_MessageId,
                                                      uint32_t * const opu32_SignalIndex) const
{
   // do not use visibility of signal/message properties widget here, because they might be already invisible
   orq_MessageSelected = false;
   orq_SignalSelected = false;

   // check if message is selected (matching IDs vector is empty if signal is selected)
   const std::vector<C_OscCanMessageIdentificationIndices> c_MatchingIds =
      this->mpc_Ui->pc_MsgPropertiesWidget->GetMatchingMessageIds();
   if (c_MatchingIds.size() > 0UL)
   {
      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MatchingIds[0]);
      if (pc_Message != NULL)
      {
         orq_MessageSelected = true;

         if (opc_MessageName != NULL)
         {
            *opc_MessageName = pc_Message->c_Name.c_str();
         }
         if (opc_MessageId != NULL)
         {
            *opc_MessageId = c_MatchingIds[0];
         }
      }
   }

   // if no message is selected, check for signal selection
   if (orq_MessageSelected == false)
   {
      const C_OscCanMessageIdentificationIndices c_Id = this->mpc_Ui->pc_SigPropertiesWidget->GetMessageId();
      const uint32_t u32_SignalIndex = this->mpc_Ui->pc_SigPropertiesWidget->GetSignalIndex();
      const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_Id);
      const C_OscNodeDataPoolListElement * const pc_Signal =
         C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(c_Id, u32_SignalIndex);
      if ((pc_Message != NULL) && (pc_Signal != NULL))
      {
         orq_MessageSelected = false;
         orq_SignalSelected = true;

         if (opc_MessageName != NULL)
         {
            *opc_MessageName = pc_Message->c_Name.c_str();
         }
         if (opc_SignalName != NULL)
         {
            *opc_SignalName = pc_Signal->c_Name.c_str();
         }

         if (opc_MessageId != NULL)
         {
            *opc_MessageId = c_Id;
         }
         if (opu32_SignalIndex != NULL)
         {
            *opu32_SignalIndex = u32_SignalIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::RefreshColors(void) const
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->RefreshColors();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect update signals for fields which signal changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::ConnectAllChanges(void) const
{
   this->mpc_Ui->pc_MsgPropertiesWidget->ConnectAllChanges();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect update signals for fields which signal changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::DisconnectAllChanges(void) const
{
   this->mpc_Ui->pc_MsgPropertiesWidget->DisconnectAllChanges();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::showEvent(QShowEvent * const opc_Event)
{
   const int32_t s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetSdBusEditLayoutSplitterHorizontal();

   this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Any message id is invalid

   \param[in]  orc_MessageId  Message id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnMessageIdChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectMessage(orc_MessageId);
   Q_EMIT (this->SigMessageIdChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The specified message data was changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnMessageNameChange(void)
{
   Q_EMIT (this->SigMessageNameChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The specified message data was changed

   \param[in]  orc_MessageId  Message id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnMessageDlcChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectMessage(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal position in message

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Index of signal of message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnSignalUpdatedViaProperties(
   const C_OscCanMessageIdentificationIndices & orc_MessageId, const uint32_t ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MsgLayoutViewerWidget->SelectSignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal position in message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnSignalUpdatedViaSelector(void)
{
   C_OscCanMessageIdentificationIndices c_MessageId;

   //If current selected
   this->mpc_Ui->pc_SigPropertiesWidget->ReloadPosition();
   //If not current selected
   if (this->m_GetMessageId(c_MessageId) == C_NO_ERR)
   {
      Q_EMIT (this->SigRecheckError(c_MessageId));
   }
   Q_EMIT (this->SigChanged());
   Q_EMIT (this->SigSignalStartBitChanged(c_MessageId));
   Q_EMIT (this->SigSignalPositionChanged(c_MessageId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of active signal via selector widget

   \param[in]  ou32_SignalIndex  Active signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnSignalActivated(const uint32_t ou32_SignalIndex)
{
   C_OscCanMessageIdentificationIndices c_MessageId;

   if (m_GetMessageId(c_MessageId) == C_NO_ERR)
   {
      Q_EMIT this->SigSignalSelected(c_MessageId, ou32_SignalIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal name

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnSignalNameChanged(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT this->SigSignalNameChanged(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal start bit

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnSignalStartBitChanged(
   const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT (this->SigSignalStartBitChanged(orc_MessageId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal position

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnSignalPositionChanged(
   const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT (this->SigSignalPositionChanged(orc_MessageId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger global error check (icons only)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_RecheckErrorGlobal(void)
{
   Q_EMIT this->SigRecheckErrorGlobal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger recheck of error values for tree

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_RecheckError(const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT this->SigRecheckError(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current active message id

   \param[out]  orc_MessageId    Message identification indices

   \retval   C_NO_ERR   Message ID found
   \retval   C_RANGE    Message ID not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueMessageSignalEditWidget::m_GetMessageId(C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   int32_t s32_Retval = C_NO_ERR;

   if (this->mpc_Ui->pc_MsgPropertiesWidget->isVisible())
   {
      const std::vector<C_OscCanMessageIdentificationIndices> c_MatchingMessageIds =
         this->mpc_Ui->pc_MsgPropertiesWidget->GetMatchingMessageIds();
      if (c_MatchingMessageIds.size() > 0)
      {
         orc_MessageId = c_MatchingMessageIds[0];
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      orc_MessageId = this->mpc_Ui->pc_SigPropertiesWidget->GetMessageId();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register any change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_OnChange(void)
{
   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Shows or hides the disabled CANopen PDO info depending of the message and protocol type

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSignalEditWidget::m_HandleDisabledPdoInfo(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   bool q_ShowDisabledPdoInfo = false;

   if (this->me_ProtocolType == C_OscCanProtocol::eCAN_OPEN)
   {
      // Special case for CANopen: Check if PDOs are disabled
      const C_OscCanMessage * const pc_CanMessage =
         C_PuiSdHandler::h_GetInstance()->GetCanMessage(orc_MessageId);

      tgl_assert(pc_CanMessage != NULL);
      if (pc_CanMessage != NULL)
      {
         q_ShowDisabledPdoInfo = !pc_CanMessage->q_CanOpenManagerMessageActive;
      }
   }

   this->mpc_Ui->pc_CoDisabledInfoWidget->setVisible(q_ShowDisabledPdoInfo);
}
