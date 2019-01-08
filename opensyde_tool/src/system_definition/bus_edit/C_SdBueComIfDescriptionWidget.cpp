//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing and editing COM interface description

   Widget for editing bus properties

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QSpacerItem>

#include "C_SdBueComIfDescriptionWidget.h"
#include "ui_C_SdBueComIfDescriptionWidget.h"

#include "stwerrors.h"
#include "C_UsHandler.h"
#include "C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand.h"
#include "C_Uti.h"
#include "C_PuiSdHandler.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     24.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueComIfDescriptionWidget::C_SdBueComIfDescriptionWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueComIfDescriptionWidget),
   mu32_BusIndex(0),
   mu32_NodeIndex(0),
   mu32_InterfaceIndex(0),
   mq_ModeSingleNode(false),
   mq_IndexValid(false),
   mc_UndoManager(NULL)
{
   mpc_Ui->setupUi(this);

   // init static names
   this->InitStaticNames();

   //Set undo manager
   this->mpc_Ui->pc_MessageSelectorWidget->SetUndoManager(&this->mc_UndoManager);

   //Set message sync manager
   this->mpc_Ui->pc_MessageSelectorWidget->SetMessageSyncManager(&this->mc_MessageSyncManager);
   this->mpc_Ui->pc_MsgSigEditWidget->SetMessageSyncManager(&this->mc_MessageSyncManager);
   this->mpc_Ui->pc_MsgSigTableWidget->SetMessageSyncManager(&this->mc_MessageSyncManager);

   //Initial set protocol
   this->m_ProtocolChanged();

   //Last step
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 105);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 195);

   // connect functionality for undo/redo
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigConnectNodeToProt,
           this, &C_SdBueComIfDescriptionWidget::m_ConnectNodeToProt);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigConnectNodeToProtAndAddDataPool,
           this, &C_SdBueComIfDescriptionWidget::m_ConnectNodeToProtAndAddDataPool);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigDisconnectNodeFromProt,
           this, &C_SdBueComIfDescriptionWidget::m_DisconnectNodeFromProt);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigAddDataPool,
           this, &C_SdBueComIfDescriptionWidget::m_AddDataPool);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigReload,
           this, &C_SdBueComIfDescriptionWidget::Reload);

   connect(this->mpc_Ui->pc_ProtocolTabWidget, &stw_opensyde_gui_elements::C_OgeTawSelector::currentChanged,
           this, &C_SdBueComIfDescriptionWidget::m_ProtocolChanged);

   //Selection signals
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigMessageSelected,
           this, &C_SdBueComIfDescriptionWidget::m_SelectMessageProperties);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigSignalSelected,
           this, &C_SdBueComIfDescriptionWidget::m_SelectSignalProperties);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigMessagesSelected, this,
           &C_SdBueComIfDescriptionWidget::m_OnMessagesSelected);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_MsgSigTableWidget, &C_SdBueMessageSignalTableWidget::SigMessageSelected, this,
           static_cast<void (C_SdBueComIfDescriptionWidget::*)(const C_OSCCanMessageIdentificationIndices &) const>(
              &C_SdBueComIfDescriptionWidget::SelectMessage));
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_MsgSigTableWidget, &C_SdBueMessageSignalTableWidget::SigSignalSelected, this,
           static_cast<void (C_SdBueComIfDescriptionWidget::*)(
                          const C_OSCCanMessageIdentificationIndices &,
                          const uint32 &) const>(&C_SdBueComIfDescriptionWidget::SelectSignal));
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigSignalSelected, this,
           static_cast<void (C_SdBueComIfDescriptionWidget::*)(const C_OSCCanMessageIdentificationIndices &,
                                                               const uint32 &) const>(&C_SdBueComIfDescriptionWidget::
                                                                                      SelectSignal));

   //Change
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigMessageIdChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnMessageIdChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigMessageNameChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnMessageNameChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigMessageRxChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnMessageRxChanged);

   //Signal
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigSignalCountOfMessageChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnSignalCountOfMessageChanged);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigMessageCountChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnMessageCountChanged);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigSignalNameChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnSignalNameChange);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigSelectName,
           this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SelectName);
   //Error
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigRecheckError, this,
           &C_SdBueComIfDescriptionWidget::m_RecheckError);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigRecheckErrorGlobal, this,
           &C_SdBueComIfDescriptionWidget::m_RecheckErrorGlobal);

   // MLV actions
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigAddSignal, this,
           &C_SdBueComIfDescriptionWidget::AddSignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigCopySignal, this,
           &C_SdBueComIfDescriptionWidget::CopySignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigCutSignal, this,
           &C_SdBueComIfDescriptionWidget::CutSignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigPasteSignal, this,
           &C_SdBueComIfDescriptionWidget::PasteSignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigDeleteSignal, this,
           &C_SdBueComIfDescriptionWidget::DeleteSignal);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     24.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueComIfDescriptionWidget::~C_SdBueComIfDescriptionWidget(void)
{
   if (this->mq_IndexValid == true)
   {
      bool q_Continue = false;
      if (this->mq_ModeSingleNode == false)
      {
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);
         if ((pc_Bus != NULL) && (pc_Bus->e_Type == C_OSCSystemBus::eCAN))
         {
            q_Continue = true;
         }
      }
      else
      {
         //For node case this is not easy to detect if user ever clicked on comm interface description tab...
         q_Continue = true;
      }
      if (q_Continue == true)
      {
         //Store splitter position
         const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

         if (c_Sizes.size() > 0)
         {
            const sintn sn_Size = c_Sizes.at(0);
            //Avoid saving invalid values
            if (sn_Size > 0)
            {
               C_UsHandler::h_GetInstance()->SetSdBusEditTreeSplitterX(sn_Size);
            }
         }
      }
   }
   m_SaveMessageUserSettings();
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     06.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::InitStaticNames(void) const
{
   // tooltip Layer 2
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(0, "OSI Layer 2",
                                                             C_GtGetText::h_GetText(
                                                                "Edit Messages and Signals of protocol type OSI Layer 2."));

   // tooltip ECeS
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(1, "ECeS",
                                                             C_GtGetText::h_GetText(
                                                                "Edit Messages and Signals of protocol type ECeS (ESX CAN efficient safety protocol)."));

   // tooltip ECoS
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(2, "ECoS",
                                                             C_GtGetText::h_GetText(
                                                                "Edit Messages and Signals of protocol type ECoS (ESX CANopen safety protocol)."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the node 'mode' of the widget with all necessary indexes

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index

   \created     31.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetNodeDataPool(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex,
                                                    const stw_types::uint32 ou32_ListIndex)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   QVBoxLayout * const pc_Layout = dynamic_cast<QVBoxLayout *>(this->mpc_Ui->pc_BueSelectorWidget->layout());

   this->m_SaveMessageUserSettings();

   this->mu32_NodeIndex = ou32_NodeIndex;
   //For each interface two lists
   this->mu32_InterfaceIndex = ou32_ListIndex / 2;

   if ((pc_Layout != NULL) && (this->mq_ModeSingleNode == false))
   {
      QSpacerItem * const pc_Spacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
      pc_Layout->insertSpacerItem(0, pc_Spacer);
      pc_Layout->setStretch(0, 0);
      //lint -e{429}  no memory leak because of the parent of pc_Spacer and the Qt memory management
   }

   this->mq_ModeSingleNode = true;

   //Set flag
   this->mq_IndexValid = true;

   //Map data pool to com protocol
   if (pc_Node != NULL)
   {
      for (uint32 u32_ItProtocol = 0; u32_ItProtocol < pc_Node->c_ComProtocols.size(); ++u32_ItProtocol)
      {
         const C_OSCCanProtocol & rc_Protocol = pc_Node->c_ComProtocols[u32_ItProtocol];
         if (rc_Protocol.u32_DataPoolIndex == ou32_DataPoolIndex)
         {
            //Even if this widget isn't visible, its index is still used
            m_SetProtocol(rc_Protocol.e_Type);
         }
      }
   }

   m_ReloadMessages();

   this->mc_UndoManager.clear();

   // adapt the ui
   this->mpc_Ui->pc_NodeSelectorWidget->setVisible(false);
   this->mpc_Ui->pc_ProtocolTabWidget->setVisible(false);

   //For each interface two lists
   this->mpc_Ui->pc_MsgSigEditWidget->SetNodeDataPool(ou32_NodeIndex, ou32_ListIndex / 2);

   //handle initial focus
   this->mpc_Ui->pc_MessageSelectorWidget->SetInitialFocus();

   this->m_RestoreMessageUserSettings();

   this->updateGeometry();
   //lint -e{429}  no memory leak because of the parent of pc_Spacer by insertSpacerItem and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the bus 'mode' of the widget

   \param[in] ou32_BusIndex      Bus index

   \created     31.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetBusId(const uint32 ou32_BusIndex)
{
   this->m_SaveMessageUserSettings();

   this->mu32_BusIndex = ou32_BusIndex;
   this->mq_ModeSingleNode = false;

   //Set flag
   this->mq_IndexValid = true;

   //Undo
   this->mc_UndoManager.clear();

   m_ReloadMessages();

   this->mpc_Ui->pc_NodeSelectorWidget->SetBusId(ou32_BusIndex);
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(this->GetActProtocol());
   this->mpc_Ui->pc_MsgSigEditWidget->SetBusId(ou32_BusIndex);

   this->m_RestoreMessageUserSettings();

   //handle initial focus
   this->mpc_Ui->pc_MessageSelectorWidget->SetInitialFocus();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle complete reload

   \created     04.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::Reload(void)
{
   const C_OSCCanProtocol::E_Type e_Protocol = this->GetActProtocol();

   //Message sync manager
   m_ReloadMessages();

   //Update widgets
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(e_Protocol);
   this->mpc_Ui->pc_MessageSelectorWidget->SetProtocolType(e_Protocol);
   this->mpc_Ui->pc_MsgSigEditWidget->SetComProtocol(e_Protocol);

   //Show messages
   m_OnMessagesSelected();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle connection change

   \created     12.06.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::PartialReload(void)
{
   //Update node check marks
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(this->GetActProtocol());
   m_ReloadMessages();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle selection change with all aspects

   \param[in] orc_MessageId Message identification indices

   \created     08.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SelectMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   m_SelectMessageProperties(orc_MessageId);
   //Tree
   this->mpc_Ui->pc_MessageSelectorWidget->SelectMessage(orc_MessageId);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle selection change with all aspects

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_MessageIndex  Message index

   \created     10.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SelectMessage(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                  const uint32 ou32_ListIndex, const uint32 ou32_MessageIndex) const
{
   C_OSCCanMessageIdentificationIndices c_MessageId;

   // fills all informations except the message index
   m_PrepareMessageId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, c_MessageId);

   c_MessageId.u32_MessageIndex = ou32_MessageIndex;
   this->SelectMessage(c_MessageId);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle selection change with all aspects

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \created     08.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SelectSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                 const uint32 & oru32_SignalIndex) const
{
   m_SelectSignalProperties(orc_MessageId, oru32_SignalIndex);
   //Tree
   this->mpc_Ui->pc_MessageSelectorWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle selection change with all aspects

   \param[in] ou32_NodeIndex     Node index
   \param[in] ou32_DataPoolIndex Data pool index
   \param[in] ou32_ListIndex     List index
   \param[in] ou32_ElementIndex  Element index in the datapool

   \created     10.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SelectSignal(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                 const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex) const
{
   C_OSCCanMessageIdentificationIndices c_MessageId;

   // fills all informations except the message index
   const std::vector<C_OSCCanMessage> * pc_Messages = m_PrepareMessageId(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                         ou32_ListIndex, c_MessageId);

   if (pc_Messages != NULL)
   {
      uint32 u32_Counter;
      uint32 u32_SignalIndex = 0U;

      // search the signal in all messages
      for (u32_Counter = 0U; u32_Counter < pc_Messages->size(); ++u32_Counter)
      {
         const C_OSCCanMessage & rc_Message = (*pc_Messages)[u32_Counter];

         for (u32_SignalIndex = 0U; u32_SignalIndex < rc_Message.c_Signals.size(); ++u32_SignalIndex)
         {
            if (ou32_ElementIndex == rc_Message.c_Signals[u32_SignalIndex].u32_ComDataElementIndex)
            {
               c_MessageId.u32_MessageIndex = u32_Counter;
               this->SelectSignal(c_MessageId, u32_SignalIndex);
               break;
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new signal

   \param[in] orc_MessageId      Message identification indices
   \param[in] ou16_StartBit      Start bit for new signal

   \created     06.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::AddSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                              const uint16 ou16_StartBit) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->AddSignal(orc_MessageId, ou16_StartBit);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy specific signal

   \param[in] orc_MessageId      Message identification indices
   \param[in] ou32_SignalIndex   Signal index

   \created     06.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::CopySignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                               const uint32 ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->CopySignal(orc_MessageId, ou32_SignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Cut specific signal

   \param[in] orc_MessageId      Message identification indices
   \param[in] ou32_SignalIndex   Signal index

   \created     06.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::CutSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                              const uint32 ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->CutSignal(orc_MessageId, ou32_SignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste copied signal

   \param[in] orc_MessageId      Message identification indices
   \param[in] ou16_StartBit      Start bit for new signal

   \created     06.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::PasteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                const uint16 ou16_StartBit) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->PasteSignal(orc_MessageId, ou16_StartBit);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete specific signal

   \param[in] orc_MessageId      Message identification indices
   \param[in] ou32_SignalIndex   Signal index

   \created     06.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::DeleteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                 const uint32 ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->DeleteSignal(orc_MessageId, ou32_SignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press release event slot

   \param[in,out] opc_KeyEvent Key event identification and information

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   //Debug commands
   if (((C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ShiftModifier) == true) &&
        (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)) &&
       (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::AltModifier) == true))
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_O:
         //Undo redo command view
         this->mc_UndoManager.ToggleCommandDisplay();
         break;
      default:
         break;
      }
   }
   //Debug commands end
   //alt
   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::AltModifier) == true)
   {
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ShiftModifier) == true)
      {
         switch (opc_KeyEvent->key())
         {
         case Qt::Key_Backspace:
            // redo
            this->mc_UndoManager.DoRedo();
            break;
         default:
            break;
         }
      }
      else
      {
         switch (opc_KeyEvent->key())
         {
         case Qt::Key_Backspace:
            // undo
            this->mc_UndoManager.DoUndo();
            break;
         default:
            break;
         }
      }
   }
   //ctrl
   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_Z:
         // undo
         this->mc_UndoManager.DoUndo();
         break;
      case Qt::Key_Y:
         // redo
         this->mc_UndoManager.DoRedo();
         break;
      default:
         break;
      }
   }

   QWidget::keyPressEvent(opc_KeyEvent);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out] opc_Event Event identification and information

   \created     22.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::showEvent(QShowEvent * const opc_Event)
{
   //Redundant call in bus edit but necessary for node edit
   TriggerLoadOfSplitterUserSettings();
   m_RestoreMessageUserSettings();
   QWidget::showEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten hide event slot

   Here: Save info

   \param[in,out] opc_Event Event identification and information

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::hideEvent(QHideEvent * const opc_Event)
{
   m_SaveMessageUserSettings();
   QWidget::hideEvent(opc_Event);
}

//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ConnectNodeToProt(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex)
{
   this->mc_UndoManager.DoConnectNodeToProt(ou32_NodeIndex, ou32_InterfaceIndex,
                                            this->GetActProtocol(), this);
   m_OnConnectionChange();
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ConnectNodeToProtAndAddDataPool(const uint32 ou32_NodeIndex,
                                                                      const uint32 ou32_InterfaceIndex)
{
   this->mc_UndoManager.DoConnectNodeToProtAndAddDataPool(ou32_NodeIndex, ou32_InterfaceIndex,
                                                          this->GetActProtocol(),
                                                          this);
   m_OnConnectionChange();
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_DisconnectNodeFromProt(const uint32 ou32_NodeIndex,
                                                             const uint32 ou32_InterfaceIndex)
{
   this->mc_UndoManager.DoDisconnectNodeFromProt(ou32_NodeIndex, ou32_InterfaceIndex,
                                                 this->GetActProtocol(), this);
   m_OnConnectionChange();
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_AddDataPool(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex)
{
   this->mc_UndoManager.DoAddDataPool(ou32_NodeIndex, ou32_InterfaceIndex,
                                      this->GetActProtocol(),
                                      this);
   m_OnConnectionChange();
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle protocol change

   \created     04.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ProtocolChanged(void)
{
   //Undo
   this->mc_UndoManager.clear();

   Reload();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Realod messages after connection change

   \created     24.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ReloadMessages(void)
{
   //Message sync manager
   if (this->mq_ModeSingleNode == false)
   {
      this->mc_MessageSyncManager.Init(this->mu32_BusIndex, this->GetActProtocol());
      this->mpc_Ui->pc_MessageSelectorWidget->SetBusId(this->mu32_BusIndex);
   }
   else
   {
      this->mc_MessageSyncManager.Init(this->mu32_NodeIndex, this->mu32_InterfaceIndex, this->GetActProtocol());
      this->mpc_Ui->pc_MessageSelectorWidget->SetNodeDataPool(this->mu32_NodeIndex, this->mu32_InterfaceIndex);
   }

   //Message tree
   this->mpc_Ui->pc_MessageSelectorWidget->InitFromData();

   //Message table
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();
}

//-----------------------------------------------------------------------------
C_OSCCanProtocol::E_Type C_SdBueComIfDescriptionWidget::GetActProtocol(void) const
{
   C_OSCCanProtocol::E_Type e_Protocol;

   switch (this->mpc_Ui->pc_ProtocolTabWidget->currentIndex())
   {
   case 1:
      e_Protocol = C_OSCCanProtocol::eECES;
      break;
   case 2:
      e_Protocol = C_OSCCanProtocol::eCAN_OPEN_SAFETY;
      break;
   case 0: // default case: layer 2
   default:
      e_Protocol = C_OSCCanProtocol::eLAYER2;
      break;
   }

   return e_Protocol;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger load of splitter user settings

   \created     07.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::TriggerLoadOfSplitterUserSettings(void) const
{
   const sint32 s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetSdBusEditTreeSplitterX();

   this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set current protocol

   Warning: no update mechanism is triggered

   \param[in] oe_Protocol Current protocol

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SetProtocol(const C_OSCCanProtocol::E_Type oe_Protocol) const
{
   switch (oe_Protocol)
   {
   case C_OSCCanProtocol::eECES:
      this->mpc_Ui->pc_ProtocolTabWidget->setCurrentIndex(1);
      break;
   case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
      this->mpc_Ui->pc_ProtocolTabWidget->setCurrentIndex(2);
      break;
   case C_OSCCanProtocol::eLAYER2:
      this->mpc_Ui->pc_ProtocolTabWidget->setCurrentIndex(0);
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select message

   \param[in] orc_MessageId Message identification indices

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SelectMessageProperties(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MsgSigEditWidget->SelectMessage(orc_MessageId);
   //Visibility
   this->mpc_Ui->pc_MsgSigEditWidget->setVisible(true);
   this->mpc_Ui->pc_MsgSigTableWidget->setVisible(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select signal

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SelectSignalProperties(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                             const uint32 & oru32_SignalIndex) const
{
   this->mpc_Ui->pc_MsgSigEditWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
   //Visibility
   this->mpc_Ui->pc_MsgSigEditWidget->setVisible(true);
   this->mpc_Ui->pc_MsgSigTableWidget->setVisible(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Any message id is invalid

   \created     25.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageIdChange(void) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnMessageIdChange();
   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();
}

//-----------------------------------------------------------------------------
/*!
   \brief   The specified message data was changed

   \created     25.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageNameChange(void) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnMessageNameChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   The signal name has changed

   \param[in] orc_MessageId Message identification indices

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnSignalNameChange(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnSignalNameChange(orc_MessageId);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle connection change

   \created     26.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnConnectionChange(void) const
{
   this->mpc_Ui->pc_MsgSigEditWidget->OnConnectionChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On change of signal count in message

   \param[in] orc_MessageId Message identification indices

   \created     26.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnSignalCountOfMessageChanged(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   this->mpc_Ui->pc_MsgSigEditWidget->OnSignalCountOfMessageChanged(orc_MessageId);
   //Update messages (All properties changes)
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();

   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   //Signal change
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger global error check (icons only)

   \created     28.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_RecheckErrorGlobal(void) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->RecheckErrorGlobal();
   Q_EMIT this->SigErrorChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger recheck of error values for tree

   \param[in] orc_MessageId Message identification indices

   \created     28.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_RecheckError(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->RecheckError(orc_MessageId);
}

//-----------------------------------------------------------------------------
/*!
   \brief   If messages selected

   \created     03.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessagesSelected(void) const
{
   this->mpc_Ui->pc_MsgSigEditWidget->setVisible(false);
   this->mpc_Ui->pc_MsgSigTableWidget->setVisible(true);
   //Update messages (All properties changes may be missing)
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On change of message count

   \created     05.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageCountChanged(void)
{
   //Update messages
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();

   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   //Signal change
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Change of RX configuration of message

   \created     09.05.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageRxChanged(void) const
{
   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register any change

   \created     09.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnChange(void)
{
   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save current state in user settings

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SaveMessageUserSettings(void) const
{
   if (this->mq_IndexValid == true)
   {
      const stw_opensyde_core::C_OSCCanProtocol::E_Type e_SelectedProtocol = GetActProtocol();
      bool q_MessageSelected;
      QString c_SelectedMessageName;
      bool q_SignalSelected;
      QString c_SelectedSignalName;

      //Get
      this->mpc_Ui->pc_MsgSigEditWidget->GetLastSelection(q_MessageSelected, c_SelectedMessageName,
                                                          q_SignalSelected, c_SelectedSignalName);
      if (this->mq_ModeSingleNode == false)
      {
         const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);
         if (pc_Bus != NULL)
         {
            //Set
            C_UsHandler::h_GetInstance()->SetProjSdBusSelectedMessage(
               pc_Bus->c_Name.c_str(), e_SelectedProtocol, q_MessageSelected, c_SelectedMessageName,
               q_SignalSelected, c_SelectedSignalName);
         }
      }
      else
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPool(
            this->mu32_NodeIndex, e_SelectedProtocol);
         const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
            this->mu32_NodeIndex, e_SelectedProtocol, this->mu32_InterfaceIndex, true);
         if (((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL))
         {
            //Set
            C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolListSelectedMessage(
               pc_Node->c_Properties.c_Name.c_str(), pc_DataPool->c_Name.c_str(),
               pc_List->c_Name.c_str(), e_SelectedProtocol, q_MessageSelected, c_SelectedMessageName, q_SignalSelected,
               c_SelectedSignalName);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Restore state from user settings

   Warning: This does reopen a const copy of the user settings

   \created     02.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_RestoreMessageUserSettings(void)
{
   C_OSCCanProtocol::E_Type e_SelectedProtocol = GetActProtocol();
   bool q_MessageSelected = false;
   QString c_SelectedMessageName;
   bool q_SignalSelected = false;
   QString c_SelectedSignalName;

   if (this->mq_ModeSingleNode == false)
   {
      const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_BusIndex);
      if (pc_Bus != NULL)
      {
         //Restore
         const C_UsCommunication c_UserSettingsBus = C_UsHandler::h_GetInstance()->GetProjSdBus(pc_Bus->c_Name.c_str());
         //Get
         c_UserSettingsBus.GetLastSelectedMessage(e_SelectedProtocol, q_MessageSelected, c_SelectedMessageName,
                                                  q_SignalSelected, c_SelectedSignalName);
         //Reinit necessary (in some cases)
         this->mc_MessageSyncManager.Init(this->mu32_BusIndex, e_SelectedProtocol);
      }
   }
   else
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPool(
         this->mu32_NodeIndex, e_SelectedProtocol);
      const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
         this->mu32_NodeIndex, e_SelectedProtocol, this->mu32_InterfaceIndex, true);
      if (((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL))
      {
         //Do not allow change of protocol as this may be different from the expected value in the default case
         C_OSCCanProtocol::E_Type e_Tmp;
         //Restore
         const C_UsNode c_UserSettingsNode = C_UsHandler::h_GetInstance()->GetProjSdNode(
            pc_Node->c_Properties.c_Name.c_str());
         const C_UsNodeDatapool c_UserSettingsDataPool = c_UserSettingsNode.GetDatapool(pc_DataPool->c_Name.c_str());
         const C_UsCommunication c_UserSettingsList = c_UserSettingsDataPool.GetCommList(pc_List->c_Name.c_str());
         //Get
         c_UserSettingsList.GetLastSelectedMessage(e_Tmp, q_MessageSelected, c_SelectedMessageName,
                                                   q_SignalSelected, c_SelectedSignalName);
      }
   }
   //Set
   m_SetProtocol(e_SelectedProtocol);
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(e_SelectedProtocol);
   this->mpc_Ui->pc_MessageSelectorWidget->SetProtocolType(e_SelectedProtocol);
   this->mpc_Ui->pc_MsgSigEditWidget->SetComProtocol(e_SelectedProtocol);
   if ((q_MessageSelected == true) || (q_SignalSelected == true))
   {
      C_OSCCanMessageIdentificationIndices c_MessageId;
      if (this->mc_MessageSyncManager.GetMessageIdForMessageName(c_SelectedMessageName, c_MessageId) == C_NO_ERR)
      {
         if (q_SignalSelected == true)
         {
            const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            if (pc_Message != NULL)
            {
               uint32 u32_FoundIndex = 0UL;
               bool q_Found = false;
               for (uint32 u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OSCNodeDataPoolListElement * const pc_OSCData =
                     C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(c_MessageId, u32_ItSignal);
                  if (pc_OSCData != NULL)
                  {
                     if (pc_OSCData->c_Name == c_SelectedSignalName.toStdString().c_str())
                     {
                        u32_FoundIndex = u32_ItSignal;
                        q_Found = true;
                        break;
                     }
                  }
               }
               if (q_Found == true)
               {
                  this->SelectSignal(c_MessageId, u32_FoundIndex);
               }
            }
         }
         else
         {
            this->SelectMessage(c_MessageId);
         }
      }
   }
}

//-----------------------------------------------------------------------------
const std::vector<C_OSCCanMessage> * C_SdBueComIfDescriptionWidget::m_PrepareMessageId(const uint32 ou32_NodeIndex,
                                                                                       const uint32 ou32_DataPoolIndex,
                                                                                       const uint32 ou32_ListIndex,
                                                                                       stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   const std::vector<C_OSCCanMessage> * pc_Messages = NULL;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCCanMessageContainer * pc_MessageContainer = NULL;
      uint32 u32_Counter;

      orc_MessageId.u32_InterfaceIndex = ou32_ListIndex / 2;

      // search the com protocol
      for (u32_Counter = 0U; u32_Counter < pc_Node->c_ComProtocols.size(); ++u32_Counter)
      {
         if (ou32_DataPoolIndex == pc_Node->c_ComProtocols[u32_Counter].u32_DataPoolIndex)
         {
            orc_MessageId.e_ComProtocol = pc_Node->c_ComProtocols[u32_Counter].e_Type;
            // get the message container, we need it later
            if (orc_MessageId.u32_InterfaceIndex < pc_Node->c_ComProtocols[u32_Counter].c_ComMessages.size())
            {
               pc_MessageContainer =
                  &pc_Node->c_ComProtocols[u32_Counter].c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            }
            break;
         }
      }

      if (pc_MessageContainer != NULL)
      {
         C_OSCNodeDataPoolList c_OscList;
         C_PuiSdNodeDataPoolList c_UiList;

         C_PuiSdHandler::h_GetInstance()->GetDataPoolList(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, c_OscList,
                                                          c_UiList);

         orc_MessageId.u32_NodeIndex = ou32_NodeIndex;

         orc_MessageId.q_MessageIsTx = C_OSCCanProtocol::h_ListIsComTx(c_OscList);

         // search the matching signal
         if (orc_MessageId.q_MessageIsTx == true)
         {
            pc_Messages = &pc_MessageContainer->c_TxMessages;
         }
         else
         {
            pc_Messages = &pc_MessageContainer->c_RxMessages;
         }
      }
   }

   return pc_Messages;
}
