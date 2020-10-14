//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for bus messages (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QAction>

#include "C_GtGetText.h"
#include "C_SdBueMessageSelectorWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "ui_C_SdBueMessageSelectorWidget.h"

#include "C_OgeWiUtil.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

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
C_SdBueMessageSelectorWidget::C_SdBueMessageSelectorWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessageSelectorWidget),
   mpc_ContextMenu(NULL),
   mq_MessagesActive(true),
   mpc_AddMessageAction(NULL),
   mpc_AddSignalAction(NULL),
   mpc_AddSignalActionWithKey(NULL),
   mpc_CopyAction(NULL),
   mpc_CutAction(NULL),
   mpc_PasteAction(NULL),
   mpc_DeleteAction(NULL)
{
   mpc_Ui->setupUi(this);

   m_SetupContextMenu();

   InitStaticNames();

   //Add button
   this->mpc_Ui->pc_PbAddMessage->setIconSize(QSize(13, 13));
   this->mpc_Ui->pc_PbAddMessage->SetCustomIcon("://images/IconAddEnabled.svg", "://images/IconAddDisabled.svg");

   //Deactivate debug string
   this->mpc_Ui->pc_GroupBoxNoMessages->setTitle("");

   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FramSperatorUp,
                                                                  "HasColor7Background",
                                                                  true);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FramSperatorBottom,
                                                                  "HasColor7Background",
                                                                  true);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_MessagesPbWidget, "Selected", true);

   connect(this->mpc_Ui->pc_PbTreeWidgetRoot, &stw_opensyde_gui_elements::C_OgePubTreeWidgetRoot::clicked,
           this, &C_SdBueMessageSelectorWidget::m_MessagesButtonClicked);
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigErrorChanged,
           this, &C_SdBueMessageSelectorWidget::SigErrorChanged);
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigSelectionChanged,
           this, &C_SdBueMessageSelectorWidget::m_MessagesSelectionChanged);
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigReload,
           this, &C_SdBueMessageSelectorWidget::SigCommDataPoolAdded);
   //Selection signals
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigMessageSelected,
           this, &C_SdBueMessageSelectorWidget::m_SelectMessage);
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigSignalSelected,
           this, &C_SdBueMessageSelectorWidget::m_SelectSignal);
   //Signals
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigSignalCountOfMessageChanged, this,
           &C_SdBueMessageSelectorWidget::m_OnSignalCountOfMessageChanged);
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigMessageCountChanged, this,
           &C_SdBueMessageSelectorWidget::m_OnMessageCountChanged);
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigSelectName, this,
           &C_SdBueMessageSelectorWidget::SigSelectName);
   connect(this->mpc_Ui->pc_PbAddMessage, &QPushButton::clicked, this,
           &C_SdBueMessageSelectorWidget::m_AddMessageButtonClicked);
   //Handle zero message case
   connect(this->mpc_Ui->pc_MessageTreeWidget, &C_SdBueMessageSelectorTreeWidget::SigZeroMessages,
           this->mpc_Ui->pc_PbTreeWidgetRoot, &C_OgePubTreeWidgetRoot::click);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMessageSelectorWidget::~C_SdBueMessageSelectorWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_ContextMenu and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active node id

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  orc_DatapoolIndexes  All Datapool indexes associated to the same protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SetNodeId(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                             const std::vector<uint32> & orc_DatapoolIndexes) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SetNodeId(ou32_NodeIndex, ou32_InterfaceIndex, orc_DatapoolIndexes);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active bus id

   \param[in]  ou32_BusIndex  Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SetBusId(const uint32 ou32_BusIndex) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SetBusId(ou32_BusIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current com protocol

   \param[in]  ore_Value   Com protocol value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SetProtocolType(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SetProtocolType(ore_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set undo manager

   \param[in,out]  opc_Value  Undo manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SetUndoManager(stw_opensyde_gui_logic::C_SdBueUnoManager * const opc_Value) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SetUndoManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SetMessageSyncManager(opc_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init from data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::InitFromData(void) const
{
   this->mpc_Ui->pc_MessageTreeWidget->InitFromData();

   //Text
   this->mpc_Ui->pc_PbTreeWidgetRoot->setText(QString(C_GtGetText::h_GetText("Messages (%1)")).arg(this->mpc_Ui->
                                                                                                   pc_MessageTreeWidget
                                                                                                   ->topLevelItemCount()));

   //Handle visibility
   if (this->mpc_Ui->pc_MessageTreeWidget->topLevelItemCount() > 0)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_MessageTreeWidget->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_MessageTreeWidget->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PbAddMessage->setText(C_GtGetText::h_GetText("Add new Message"));

   this->mpc_Ui->pc_PbTreeWidgetRoot->SetToolTipInformation(C_GtGetText::h_GetText("Messages"),
                                                            C_GtGetText::h_GetText(
                                                               "Show overview of all messages / signals"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Any message id is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::OnMessageIdChange(void) const
{
   this->mpc_Ui->pc_MessageTreeWidget->OnMessageIdChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The specified message data was changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::OnMessageNameChange(void) const
{
   this->mpc_Ui->pc_MessageTreeWidget->OnMessageNameChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal name has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::OnSignalNameChange(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   this->mpc_Ui->pc_MessageTreeWidget->OnSignalNameChange(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal start bit has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::OnSignalStartBitChange(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
const
{
   //Not necessary right now but kept if just sorting one tree sub element is supported
   Q_UNUSED(orc_MessageId)
   //Handles resorting
   this->mpc_Ui->pc_MessageTreeWidget->OnMessageNameChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In case of a disconnected node update of the unique message ids

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::OnNodeDisconnected(const uint32 ou32_NodeIndex,
                                                      const uint32 ou32_InterfaceIndex) const
{
   this->mpc_Ui->pc_MessageTreeWidget->OnNodeDisconnected(ou32_NodeIndex, ou32_InterfaceIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger global error check (icons only)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::RecheckErrorGlobal(void) const
{
   this->mpc_Ui->pc_MessageTreeWidget->RecheckErrorGlobal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger recheck of error values for tree

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::RecheckError(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   this->mpc_Ui->pc_MessageTreeWidget->RecheckError(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set initial focus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SetInitialFocus(void) const
{
   this->mpc_Ui->pc_PbTreeWidgetRoot->setFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle select messages action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SelectMessagesWithoutSignal(void)
{
   this->mpc_Ui->pc_MessageTreeWidget->collapseAll();
   this->mpc_Ui->pc_MessageTreeWidget->DeselectAllItems();
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_MessagesPbWidget, "Selected", true);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FramSperatorUp,
                                                                  "HasColor7Background",
                                                                  true);
   stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FramSperatorBottom,
                                                                  "HasColor7Background",
                                                                  true);
   this->mq_MessagesActive = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SelectMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SelectMessage(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::SelectSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                const uint32 & oru32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageTreeWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new signal

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  ou16_StartBit  Start bit for new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::AddSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                             const uint16 ou16_StartBit) const
{
   this->mpc_Ui->pc_MessageTreeWidget->AddSignalWithStartBit(orc_MessageId, ou16_StartBit, false, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  ou16_StartBit        Start bit for new signal
   \param[in]  ou16_MultiplexValue  Concrete multiplexed value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::AddSignalMultiplexed(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                        const uint16 ou16_StartBit,
                                                        const uint16 ou16_MultiplexValue) const
{
   this->mpc_Ui->pc_MessageTreeWidget->AddSignalWithStartBit(orc_MessageId, ou16_StartBit, true, ou16_MultiplexValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::CopySignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                              const uint32 ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageTreeWidget->CopySignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::CutSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                             const uint32 ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageTreeWidget->CutSignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste copied signal

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  ou16_StartBit  Start bit for new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::PasteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                               const uint16 ou16_StartBit) const
{
   this->mpc_Ui->pc_MessageTreeWidget->PasteSignal(orc_MessageId, ou16_StartBit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::DeleteSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                const uint32 ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageTreeWidget->DeleteSignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press release event slot

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOriginal = true;

   //shift
   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ShiftModifier) == true)
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_Delete:
         // cut
         this->mpc_Ui->pc_MessageTreeWidget->Cut();
         q_CallOriginal = false;
         break;
      case Qt::Key_Insert:
         // paste
         this->mpc_Ui->pc_MessageTreeWidget->Paste();
         break;
      default:
         break;
      }
   }
   //ctrl
   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_X:
         // cut
         this->mpc_Ui->pc_MessageTreeWidget->Cut();
         q_CallOriginal = false;
         break;
      case Qt::Key_Insert:
      case Qt::Key_C:
         // copy
         this->mpc_Ui->pc_MessageTreeWidget->Copy();
         q_CallOriginal = false;
         break;
      case Qt::Key_V:
         // paste
         this->mpc_Ui->pc_MessageTreeWidget->Paste();
         break;
      case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
      case Qt::Key_Plus:
         this->mpc_Ui->pc_MessageTreeWidget->Add();
         q_CallOriginal = false;
         break;
      default:
         break;
      }
   }
   else
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_F2:
         this->mpc_Ui->pc_MessageTreeWidget->EditName();
         break;
      case Qt::Key_Delete:
         this->mpc_Ui->pc_MessageTreeWidget->Delete();
         break;
      default:
         break;
      }
   }

   if (q_CallOriginal == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_AddMessageButtonClicked(void) const
{
   if (this->mpc_Ui->pc_MessageTreeWidget->CheckIfAnyNodeConnected() == true)
   {
      this->mpc_Ui->pc_MessageTreeWidget->AddMessage();
   }
   else
   {
      C_OgeWiCustomMessage c_MessageBox(this->parentWidget());
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Message add"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("Cannot add new message. Select an active node."));
      c_MessageBox.SetCustomMinHeight(180, 180);
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_MessagesButtonClicked(void)
{
   this->SelectMessagesWithoutSignal();
   Q_EMIT this->SigMessagesSelected();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_MessagesSelectionChanged(void)
{
   if (this->mq_MessagesActive == true)
   {
      stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_MessagesPbWidget, "Selected",
                                                                     false);
      stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FramSperatorUp,
                                                                     "HasColor7Background",
                                                                     false);
      stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FramSperatorBottom,
                                                                     "HasColor7Background",
                                                                     false);
      this->mq_MessagesActive = false;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_SelectMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT this->SigMessageSelected(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_SelectSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                  const uint32 & oru32_SignalIndex)
{
   Q_EMIT this->SigSignalSelected(orc_MessageId, oru32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_SetupContextMenu(void)
{
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   this->mpc_AddMessageAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add new Message"),
                                                                 this->mpc_Ui->pc_MessageTreeWidget,
                                                                 &C_SdBueMessageSelectorTreeWidget::AddMessage,
                                                                 static_cast<sintn>(Qt::CTRL) +
                                                                 static_cast<sintn>(Qt::Key_Plus));

   this->mpc_AddSignalAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add new Signal"),
                                                                this->mpc_Ui->pc_MessageTreeWidget,
                                                                &C_SdBueMessageSelectorTreeWidget::AddSignal);

   this->mpc_AddSignalActionWithKey = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Add new Signal"),
                                                                       this->mpc_Ui->pc_MessageTreeWidget,
                                                                       &C_SdBueMessageSelectorTreeWidget::AddSignal,
                                                                       static_cast<sintn>(Qt::CTRL) +
                                                                       static_cast<sintn>(Qt::Key_Plus));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ExpandAllAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Expand all"),
                                                                this->mpc_Ui->pc_MessageTreeWidget,
                                                                &C_SdBueMessageSelectorTreeWidget::ExpandAll);

   this->mpc_CollapseAllAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Collapse all"),
                                                                  this->mpc_Ui->pc_MessageTreeWidget,
                                                                  &C_SdBueMessageSelectorTreeWidget::CollapseAll);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_CutAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Cut"),
                                                          this->mpc_Ui->pc_MessageTreeWidget,
                                                          &C_SdBueMessageSelectorTreeWidget::Cut,
                                                          static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));
   this->mpc_CopyAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                                           this->mpc_Ui->pc_MessageTreeWidget,
                                                           &C_SdBueMessageSelectorTreeWidget::Copy,
                                                           static_cast<sintn>(Qt::CTRL) +
                                                           static_cast<sintn>(Qt::Key_C));
   this->mpc_PasteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Paste"),
                                                            this->mpc_Ui->pc_MessageTreeWidget,
                                                            &C_SdBueMessageSelectorTreeWidget::Paste,
                                                            static_cast<sintn>(Qt::CTRL) +
                                                            static_cast<sintn>(Qt::Key_V));

   this->mpc_ContextMenu->addSeparator();

   this->mpc_DeleteAction = this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Delete"),
                                                             this->mpc_Ui->pc_MessageTreeWidget,
                                                             &C_SdBueMessageSelectorTreeWidget::Delete,
                                                             static_cast<sintn>(Qt::Key_Delete));

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_SdBueMessageSelectorWidget::customContextMenuRequested, this,
           &C_SdBueMessageSelectorWidget::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   //Check if valid pos
   if (orc_Pos.y() < this->mpc_Ui->pc_VerticalSpacer->geometry().y())
   {
      if (this->mpc_Ui->pc_MessageTreeWidget->CheckIfAnyNodeConnected() == true)
      {
         QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
         sint32 s32_Level = this->mpc_Ui->pc_MessageTreeWidget->GetLevelOfPos(c_PosGlobal);
         bool q_ShowContextMenu = true;

         if (s32_Level == 1) // Message level
         {
            this->mpc_AddMessageAction->setVisible(true);
            this->mpc_AddSignalAction->setVisible(true);
            this->mpc_AddSignalActionWithKey->setVisible(false);

            this->mpc_ExpandAllAction->setVisible(true);
            this->mpc_CollapseAllAction->setVisible(true);

            this->mpc_CopyAction->setVisible(true);
            this->mpc_CutAction->setVisible(true);
            this->mpc_PasteAction->setVisible(true);
            this->mpc_DeleteAction->setVisible(true);
         }
         else if (s32_Level == 2) // Signal level
         {
            this->mpc_AddMessageAction->setVisible(false);
            this->mpc_AddSignalAction->setVisible(false);
            this->mpc_AddSignalActionWithKey->setVisible(true);

            this->mpc_ExpandAllAction->setVisible(true);
            this->mpc_CollapseAllAction->setVisible(true);

            this->mpc_CopyAction->setVisible(true);
            this->mpc_CutAction->setVisible(true);
            this->mpc_PasteAction->setVisible(true);
            this->mpc_DeleteAction->setVisible(true);
         }
         else if (this->mpc_Ui->pc_PbTreeWidgetRoot->rect().contains(this->mpc_Ui->pc_PbTreeWidgetRoot->mapFromGlobal(
                                                                        this->mapToGlobal(orc_Pos))) == true)
         {
            // no level and under
            this->mpc_AddMessageAction->setVisible(true);
            this->mpc_AddSignalAction->setVisible(false);
            this->mpc_AddSignalActionWithKey->setVisible(false);

            this->mpc_ExpandAllAction->setVisible(true);
            this->mpc_CollapseAllAction->setVisible(false);

            this->mpc_CopyAction->setVisible(false);
            this->mpc_CutAction->setVisible(false);
            this->mpc_PasteAction->setVisible(true);
            this->mpc_DeleteAction->setVisible(false);

            this->m_MessagesButtonClicked();
         }
         else
         {
            // no context menu
            q_ShowContextMenu = false;
         }

         if (q_ShowContextMenu == true)
         {
            this->mpc_ContextMenu->popup(c_PosGlobal);
         }
      }
      else
      {
         C_OgeWiCustomMessage c_MessageBox(this);
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("No actions available"));
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("Select an active node and retry."));
         c_MessageBox.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal count in message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_OnSignalCountOfMessageChanged(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT (this->SigSignalCountOfMessageChanged(orc_MessageId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of message count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessageSelectorWidget::m_OnMessageCountChanged(void)
{
   Q_EMIT (this->SigMessageCountChanged());

   //Text
   this->mpc_Ui->pc_PbTreeWidgetRoot->setText(QString(C_GtGetText::h_GetText("Messages (%1)")).arg(this->mpc_Ui->
                                                                                                   pc_MessageTreeWidget
                                                                                                   ->topLevelItemCount()));

   //Handle visibility
   if (this->mpc_Ui->pc_MessageTreeWidget->topLevelItemCount() > 0)
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(false);
      this->mpc_Ui->pc_MessageTreeWidget->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxNoMessages->setVisible(true);
      this->mpc_Ui->pc_MessageTreeWidget->setVisible(false);
   }
}
