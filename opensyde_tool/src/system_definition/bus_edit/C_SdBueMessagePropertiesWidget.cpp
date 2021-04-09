//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing message properties

   Widget for editing message properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include <QSpinBox>
#include <QComboBox>

#include "stwerrors.h"
#include "C_OgeLeComboBox.h"
#include "C_SdBueMessagePropertiesWidget.h"
#include "C_GtGetText.h"
#include "C_OSCUtils.h"
#include "C_PuiSdHandler.h"
#include "TGLUtils.h"
#include "ui_C_SdBueMessagePropertiesWidget.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_SdUtil.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sint32 ms32_TX_TYPE_INDEX_CYCLIC = 0;
const sint32 ms32_TX_TYPE_INDEX_ON_CHANGE = 1;
const sint32 ms32_TX_TYPE_INDEX_SPONTANEOUS = 2;

const uint8 mu8_DIRECTION_INDEX_TRANSMIT = 0;
const uint8 mu8_DIRECTION_INDEX_RECEIVE = 1;

const uint32 mu32_DEFAULT_CYCLE_TIME = 100U;

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
C_SdBueMessagePropertiesWidget::C_SdBueMessagePropertiesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMessagePropertiesWidget),
   mpc_MessageSyncManager(NULL),
   me_ComProtocol(C_OSCCanProtocol::eLAYER2),
   mq_IdIsValid(false),
   mu32_BusIndex(0),
   mu32_NodeIndex(0),
   mu32_InterfaceIndex(0),
   mq_ModeSingleNode(false),
   mq_InternalRxChange(false)
{
   // init UI
   mpc_Ui->setupUi(this);

   InitStaticNames();

   //Ui restriction
   this->mpc_Ui->pc_LineEditName->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);
   this->mpc_Ui->pc_LabelCriticalMessage->setWordWrap(true);

   //Remove debug string
   this->mpc_Ui->pc_GroupBoxCriticalMessage->setTitle("");
   this->mpc_Ui->pc_GroupBoxCriticalMessage->setVisible(false);

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxDlc->SetMaximumCustom(8);
   this->mpc_Ui->pc_SpinBoxLater->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxLater->SetMaximumCustom(50000);
   this->mpc_Ui->pc_SpinBoxEarly->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxEarly->SetMaximumCustom(50000);
   this->mpc_Ui->pc_SpinBoxCycleTime->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxCycleTime->SetMaximumCustom(50000);

   //Initial setup
   m_OnTxMethodChange(this->mpc_Ui->pc_ComboBoxTxMethod->currentIndex());
   m_OnExtendedChange(this->mpc_Ui->pc_CheckBoxExtendedType->isChecked());

   //We do want the SpinBoxes to emit "valueChanged" on button clicks immediately but
   // for changes through keyboard only after enter or when the field is left.
   //This prevents problems premature triggering of automatic calculations or checkn when the user enters a value
   // manually and is not finished yet.
   this->mpc_Ui->pc_SpinBoxCycleTime->setKeyboardTracking(false);
   this->mpc_Ui->pc_SpinBoxLater->setKeyboardTracking(false);
   this->mpc_Ui->pc_SpinBoxEarly->setKeyboardTracking(false);
   this->mpc_Ui->pc_SpinBoxId->setKeyboardTracking(false);
   //Do not deactivate keyboard tracking for DLC SpinEdit; it only has digit anyway

   // connects
   // These elements have multiple connects to different slots
   connect(this->mpc_Ui->pc_CheckBoxExtendedType, &C_OgeChxProperties::toggled, this,
           &C_SdBueMessagePropertiesWidget::m_OnExtendedChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTxMethod, static_cast<void (QComboBox::*)(
                                                             sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxMethodChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxLater, static_cast<void (QSpinBox::*)(sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_SyncCycleToLater);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCycleTime, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_SyncLaterToCycle);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because the ownership of these objects was never transferred to this class
C_SdBueMessagePropertiesWidget::~C_SdBueMessagePropertiesWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::InitStaticNames(void) const
{
   QLineEdit * pc_LineEdit;

   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_CheckBoxExtendedType->setText(C_GtGetText::h_GetText("Extended Type"));
   this->mpc_Ui->pc_LabelID->setText(C_GtGetText::h_GetText("CAN ID"));
   this->mpc_Ui->pc_LabelTxMethod->setText(C_GtGetText::h_GetText("Tx Method"));
   this->mpc_Ui->pc_LabelDlc->setText(C_GtGetText::h_GetText("DLC"));
   this->mpc_Ui->pc_LabelCycleTime->setText(C_GtGetText::h_GetText("Cycle Time"));
   this->mpc_Ui->pc_LabelEarly->setText(C_GtGetText::h_GetText("Not earlier than"));
   this->mpc_Ui->pc_LabelLater->setText(C_GtGetText::h_GetText("But not later than"));
   this->mpc_Ui->pc_LabelTransmitter->setText(C_GtGetText::h_GetText("Transmitter"));
   this->mpc_Ui->pc_LabelReceiver->setText(C_GtGetText::h_GetText("Receiver(s)"));
   this->mpc_Ui->pc_LabelDirection->setText(C_GtGetText::h_GetText("Direction"));
   this->mpc_Ui->pc_LabelCanSettings->setText(C_GtGetText::h_GetText("CAN Settings"));
   this->mpc_Ui->pc_LabelCriticalMessage->setText(C_GtGetText::h_GetText(
                                                     "No receivers allowed until the message is unique."));
   this->mpc_Ui->pc_LabelTransmitterNode->setText(C_GtGetText::h_GetText("Node"));
   this->mpc_Ui->pc_LabelTransmitterDatapool->setText(C_GtGetText::h_GetText("Datapool"));
   this->mpc_Ui->pc_LabelTransmitterDatapoolOnly->setText(C_GtGetText::h_GetText("Datapool"));

   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
   pc_LineEdit = new C_OgeLeComboBox(this->mpc_Ui->pc_ComboBoxTransmitterNode);
   pc_LineEdit->setPlaceholderText(C_GtGetText::h_GetText("Choose a transmitter"));
   pc_LineEdit->setReadOnly(true);
   // force line edit to be not selectable (QLineEdit has no text interaction flags like QTextEdit or QLabel)
   connect(pc_LineEdit, &QLineEdit::selectionChanged, pc_LineEdit, &QLineEdit::deselect);
   pc_LineEdit->setAccessibleDescription("");
   this->mpc_Ui->pc_ComboBoxTransmitterNode->setLineEdit(pc_LineEdit);
   this->mpc_Ui->pc_ComboBoxTransmitterNode->setContextMenuPolicy(Qt::NoContextMenu);

   //Combo boxes
   this->mpc_Ui->pc_ComboBoxDirection->addItem(C_GtGetText::h_GetText("Transmit"));
   this->mpc_Ui->pc_ComboBoxDirection->addItem(C_GtGetText::h_GetText("Receive"));

   this->mpc_Ui->pc_ComboBoxTxMethod->addItem(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_CYCLIC));
   this->mpc_Ui->pc_ComboBoxTxMethod->addItem(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_ON_CHANGE));
   this->mpc_Ui->pc_ComboBoxTxMethod->addItem(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_ON_EVENT));

   //Other
   this->mpc_Ui->pc_SpinBoxCycleTime->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpinBoxEarly->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpinBoxLater->setSuffix(C_GtGetText::h_GetText(" ms"));
   this->mpc_Ui->pc_SpinBoxId->setPrefix(C_GtGetText::h_GetText("0x"));

   //Tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                                                     C_GtGetText::h_GetText(
                                                        "Symbolic message name. Unique within a bus."
                                                        "\nFollowing C naming conventions are required:"
                                                        "\n - must not be empty"
                                                        "\n - only alphanumeric characters and \"_\""
                                                        "\n - should not be longer than 31 characters"));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this message."));

   this->mpc_Ui->pc_CheckBoxExtendedType->SetToolTipInformation(C_GtGetText::h_GetText("Extended Type"),
                                                                C_GtGetText::h_GetText("CAN ID Type. "
                                                                                       "\nStandard Type: 11Bit "
                                                                                       "\nExtended Type: 29Bit"));
   this->mpc_Ui->pc_LabelID->SetToolTipInformation(C_GtGetText::h_GetText("CAN ID"),
                                                   C_GtGetText::h_GetText("CAN Identifier (Hex).\n"
                                                                          "Unique within a bus."));
   this->mpc_Ui->pc_LabelDlc->SetToolTipInformation(C_GtGetText::h_GetText("DLC"),
                                                    C_GtGetText::h_GetText("Data Length Code. Number of data bytes."));
   this->mpc_Ui->pc_LabelCycleTime->SetToolTipInformation(C_GtGetText::h_GetText("Cycle Time"),
                                                          C_GtGetText::h_GetText(
                                                             "Cyclic method property. Message is transmitted every time"
                                                             " after this time is expired."));
   this->mpc_Ui->pc_LabelTxMethod->SetToolTipInformation(
      C_GtGetText::h_GetText("Tx Method"),
      C_GtGetText::h_GetText("Message transmission method. "
                             "\nCyclic: Message is transmitted cyclically."
                             "\nOn Change: Message is transmitted if any signal value is changed."
                             "\nOn Event: Message transmission is handled by application (spontaneous)."));
   this->mpc_Ui->pc_LabelEarly->SetToolTipInformation(
      C_GtGetText::h_GetText("Not earlier than"),
      C_GtGetText::h_GetText("On change method property. "
                             "\nIf a signal changes during this time, the message will not be transmitted."
                             "\nThis allows the bus load to be controlled."));
   this->mpc_Ui->pc_LabelLater->SetToolTipInformation(
      C_GtGetText::h_GetText("But no later than"),
      C_GtGetText::h_GetText("On change method property. "
                             "\nIf the signal does not change, the message will still be sent after this time."));
   this->mpc_Ui->pc_LabelDirection->SetToolTipInformation(
      C_GtGetText::h_GetText("Direction"),
      C_GtGetText::h_GetText("Message is either transmitted or received."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out] opc_Value Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_MessageSyncManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Message id setter

   Sets the private message id of widget

   \param[in] oq_IsValid    Flag if ID is valid
   \param[in] orc_MessageId Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetMessageId(const bool oq_IsValid,
                                                  const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   this->mq_IdIsValid = oq_IsValid;
   this->mc_MessageId = orc_MessageId;

   if (this->mq_IdIsValid)
   {
      //load bus data
      this->m_LoadFromData();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load bus information

   Load bus information from core data using bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_LoadFromData(void)
{
   if (this->mq_IdIsValid)
   {
      const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);

      //Disconnects for RegisterChange
      DisconnectAllChanges();

      tgl_assert(pc_Message != NULL);
      if (pc_Message != NULL)
      {
         uint32 u32_UsedCylceTime;
         //Name
         this->mpc_Ui->pc_LineEditName->setText(pc_Message->c_Name.c_str());

         //Comment
         this->mpc_Ui->pc_TextEditComment->setText(pc_Message->c_Comment.c_str());

         //Extended
         this->mpc_Ui->pc_CheckBoxExtendedType->setChecked(pc_Message->q_IsExtended);

         //Id
         this->mpc_Ui->pc_SpinBoxId->setValue(pc_Message->u32_CanId);
         this->mpc_Ui->pc_SpinBoxId->textFromValue(this->mpc_Ui->pc_SpinBoxId->value());

         //Dlc
         this->mpc_Ui->pc_SpinBoxDlc->setValue(pc_Message->u16_Dlc);

         //Tx method
         this->mpc_Ui->pc_ComboBoxTxMethod->setCurrentIndex(mh_TxMethodToIndex(pc_Message->e_TxMethod));

         if (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_EVENT)
         {
            this->mpc_Ui->pc_WidgetReceiver->SetTxMethodOnEvent(true);
            // No configurable cycle time
            u32_UsedCylceTime = 0U;
         }
         else
         {
            this->mpc_Ui->pc_WidgetReceiver->SetTxMethodOnEvent(false);

            u32_UsedCylceTime = pc_Message->u32_CycleTimeMs;
         }
         this->mpc_Ui->pc_ComboBoxTxMethod->SetItemState(ms32_TX_TYPE_INDEX_ON_CHANGE, !pc_Message->IsMultiplexed());

         //Cycle time
         this->mpc_Ui->pc_SpinBoxCycleTime->setValue(u32_UsedCylceTime);
         this->mpc_Ui->pc_SpinBoxLater->setValue(u32_UsedCylceTime);

         if (pc_Message->e_TxMethod != C_OSCCanMessage::eTX_METHOD_ON_EVENT)
         {
            this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(static_cast<sintn>(u32_UsedCylceTime));
         }
         else
         {
            // In case of on Event the receive timeout need the default cycle time for correct calculations
            this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(mu32_DEFAULT_CYCLE_TIME);
         }

         //Delay time
         this->mpc_Ui->pc_SpinBoxEarly->setValue(pc_Message->u16_DelayTimeMs);

         if (this->mq_ModeSingleNode == true)
         {
            const C_PuiSdNodeCanMessage * const pc_UiMessage = C_PuiSdHandler::h_GetInstance()->GetUiCanMessage(
               this->mc_MessageId);
            tgl_assert(pc_UiMessage != NULL);
            if (pc_UiMessage != NULL)
            {
               if (this->mc_MessageId.q_MessageIsTx == true)
               {
                  this->mpc_Ui->pc_ComboBoxDirection->setCurrentIndex(mu8_DIRECTION_INDEX_TRANSMIT);
               }
               else
               {
                  this->mpc_Ui->pc_ComboBoxDirection->setCurrentIndex(mu8_DIRECTION_INDEX_RECEIVE);
               }
               // Let the function not disconnect for its own, it is already disconnect. Avoiding multiple reconnecting.
               this->m_NodeModeDirectionChanged(false);
            }
         }
         else
         {
            if (this->mpc_MessageSyncManager != NULL)
            {
               const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds =
                  this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);

               //Reload connected nodes
               this->m_ReloadNodes();

               //transmitter / receiver
               //Select transmitter
               this->m_UpdateTxSelection(c_MatchingMessageIds);

               m_ConnectNodeSpecificFields();
               m_HandleCriticalMessagesAndRx(true);
               m_DisconnectNodeSpecificFields();
            }
         }
         //Initial error check
         m_CheckMessageName();
         m_CheckMessageId();
         m_CheckEarlyTime();
      }

      //connects for RegisterChange
      ConnectAllChanges();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle extended id checkbox change

   \param[in] orq_Extended Extended ID active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnExtendedChange(const bool & orq_Extended) const
{
   if (this->me_ComProtocol == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
   {
      this->mpc_Ui->pc_SpinBoxId->SetMinimumCustom(mu32_PROTOCOL_ECOS_MESSAGE_ID_MIN);
      this->mpc_Ui->pc_SpinBoxId->SetMaximumCustom(mu32_PROTOCOL_ECOS_MESSAGE_ID_MAX);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxId->SetMinimumCustom(0);
      if (orq_Extended == true)
      {
         this->mpc_Ui->pc_SpinBoxId->SetMaximumCustom(0x1FFFFFFF);
      }
      else
      {
         this->mpc_Ui->pc_SpinBoxId->SetMaximumCustom(0x7FF);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle checkbox tx method change

   \param[in] ors32_State Check box state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnTxMethodChange(const sint32 & ors32_State) const
{
   switch (ors32_State)
   {
   case ms32_TX_TYPE_INDEX_CYCLIC:
      this->mpc_Ui->pc_LabelCycleTime->setVisible(true);
      this->mpc_Ui->pc_SpinBoxCycleTime->setVisible(true);
      this->mpc_Ui->pc_LabelEarly->setVisible(false);
      this->mpc_Ui->pc_SpinBoxEarly->setVisible(false);
      this->mpc_Ui->pc_LabelLater->setVisible(false);
      this->mpc_Ui->pc_SpinBoxLater->setVisible(false);
      //Handle timeout section
      this->mpc_Ui->pc_WidgetReceiver->SetTxMethodOnEvent(false);
      break;
   case ms32_TX_TYPE_INDEX_ON_CHANGE:
      this->mpc_Ui->pc_LabelCycleTime->setVisible(false);
      this->mpc_Ui->pc_SpinBoxCycleTime->setVisible(false);
      this->mpc_Ui->pc_LabelEarly->setVisible(true);
      this->mpc_Ui->pc_SpinBoxEarly->setVisible(true);
      this->mpc_Ui->pc_LabelLater->setVisible(true);
      this->mpc_Ui->pc_SpinBoxLater->setVisible(true);
      //Handle timeout section
      this->mpc_Ui->pc_WidgetReceiver->SetTxMethodOnEvent(false);
      break;
   default:
      this->mpc_Ui->pc_LabelCycleTime->setVisible(false);
      this->mpc_Ui->pc_SpinBoxCycleTime->setVisible(false);
      this->mpc_Ui->pc_LabelEarly->setVisible(false);
      this->mpc_Ui->pc_SpinBoxEarly->setVisible(false);
      this->mpc_Ui->pc_LabelLater->setVisible(false);
      this->mpc_Ui->pc_SpinBoxLater->setVisible(false);
      //Handle timeout section
      this->mpc_Ui->pc_WidgetReceiver->SetTxMethodOnEvent(true);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform tx method type to index

   \param[in] ore_TxMethod Tx method type

   \return
   Combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueMessagePropertiesWidget::mh_TxMethodToIndex(const C_OSCCanMessage::E_TxMethodType & ore_TxMethod)
{
   sint32 s32_Retval;

   switch (ore_TxMethod)
   {
   case C_OSCCanMessage::eTX_METHOD_CYCLIC:
      s32_Retval = ms32_TX_TYPE_INDEX_CYCLIC;
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_CHANGE:
      s32_Retval = ms32_TX_TYPE_INDEX_ON_CHANGE;
      break;
   case C_OSCCanMessage::eTX_METHOD_ON_EVENT:
      s32_Retval = ms32_TX_TYPE_INDEX_SPONTANEOUS;
      break;
   default:
      s32_Retval = -1;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform index to tx method type

   \param[in] ors32_Index Index

   \return
   Tx method type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanMessage::E_TxMethodType C_SdBueMessagePropertiesWidget::mh_IndexToTxMethod(const sint32 & ors32_Index)
{
   C_OSCCanMessage::E_TxMethodType e_Retval;
   if (ors32_Index == ms32_TX_TYPE_INDEX_CYCLIC)
   {
      e_Retval = C_OSCCanMessage::eTX_METHOD_CYCLIC;
   }
   else if (ors32_Index == ms32_TX_TYPE_INDEX_ON_CHANGE)
   {
      e_Retval = C_OSCCanMessage::eTX_METHOD_ON_CHANGE;
   }
   else
   {
      e_Retval = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle name change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnNameChanged(void)
{
   this->m_TrimmMessageName();
   m_OnPropertiesChanged();
   m_CheckMessageName();
   Q_EMIT (this->SigMessageNameChanged());
   Q_EMIT (this->SigRecheckError());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle id change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnIdChanged(void)
{
   m_OnPropertiesChanged();
   m_CheckMessageId();
   Q_EMIT this->SigMessageNameChanged();
   Q_EMIT this->SigRecheckError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dlc change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnDlcChanged(void)
{
   if (this->mq_IdIsValid)
   {
      m_OnPropertiesChanged();
      Q_EMIT this->SigMessageDlcChanged(this->mc_MessageId);
      Q_EMIT this->SigRecheckError();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle cycle time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnCycleTimeChanged(void)
{
   m_OnPropertiesChanged();

   if (this->mpc_Ui->pc_ComboBoxTxMethod->currentIndex() != ms32_TX_TYPE_INDEX_SPONTANEOUS)
   {
      // Only in case of Tx Method on Event the SpinBox value is not usable for sure
      this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(
         static_cast<uint32>(this->mpc_Ui->pc_SpinBoxCycleTime->value()));
   }
   else
   {
      this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(mu32_DEFAULT_CYCLE_TIME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle tx method change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnTxMethodChanged()
{
   m_OnPropertiesChanged();
   Q_EMIT this->SigRecheckError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle early time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnEarlyOrLaterTimeChanged(void)
{
   m_OnPropertiesChanged();
   m_CheckEarlyTime();
   Q_EMIT this->SigRecheckError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle any property change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnPropertiesChanged(void)
{
   if (this->mq_IdIsValid)
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         const C_OSCCanMessage * const pc_CanMessage =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);

         m_RegisterChange();

         if (pc_CanMessage != NULL)
         {
            C_OSCCanMessageIdentificationIndices c_NewMessageId;
            //save data
            //copy current message
            C_OSCCanMessage c_MessageData = *pc_CanMessage;
            C_OSCCanMessage::E_TxMethodType e_CurrentTxMethod;

            //name
            c_MessageData.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();

            //comment
            c_MessageData.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();

            //Extended
            c_MessageData.q_IsExtended = this->mpc_Ui->pc_CheckBoxExtendedType->isChecked();

            //Id
            c_MessageData.u32_CanId = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxId->value());

            //Dlc
            c_MessageData.u16_Dlc = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxDlc->value());

            //Tx method
            e_CurrentTxMethod = mh_IndexToTxMethod(this->mpc_Ui->pc_ComboBoxTxMethod->currentIndex());
            if (e_CurrentTxMethod != c_MessageData.e_TxMethod)
            {
               if (c_MessageData.e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_EVENT)
               {
                  // In case of change from on event with no valid cycle times, set cycle time values to default
                  this->mpc_Ui->pc_SpinBoxCycleTime->setValue(mu32_DEFAULT_CYCLE_TIME);
                  this->mpc_Ui->pc_SpinBoxLater->setValue(mu32_DEFAULT_CYCLE_TIME);
                  this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(
                     static_cast<sintn>(mu32_DEFAULT_CYCLE_TIME));
               }
               else if (e_CurrentTxMethod == C_OSCCanMessage::eTX_METHOD_ON_EVENT)
               {
                  // In case of change to on event with no valid cycle times, set cycle time values to 0
                  this->mpc_Ui->pc_SpinBoxCycleTime->setValue(0);
                  this->mpc_Ui->pc_SpinBoxLater->setValue(0);
                  this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(
                     static_cast<sintn>(mu32_DEFAULT_CYCLE_TIME));
               }
               else
               {
                  // Nothing to do
               }
            }
            c_MessageData.e_TxMethod = e_CurrentTxMethod;

            //Cycle time
            if (c_MessageData.e_TxMethod == C_OSCCanMessage::eTX_METHOD_CYCLIC)
            {
               c_MessageData.u32_CycleTimeMs = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxCycleTime->value());
            }
            else if (c_MessageData.e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE)
            {
               c_MessageData.u32_CycleTimeMs = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxLater->value());
            }
            else
            {
               // No cycle time in case of an on event message
               c_MessageData.u32_CycleTimeMs = 0U;
            }

            //Delay time
            c_MessageData.u16_DelayTimeMs = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxEarly->value());

            //save new message data
            tgl_assert(this->mpc_MessageSyncManager->SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
                          this->mc_MessageId,
                          c_MessageData) ==
                       C_NO_ERR);
            if (this->mpc_MessageSyncManager->RecheckCriticalMessage(this->mc_MessageId, c_NewMessageId) == true)
            {
               this->mc_MessageId = c_NewMessageId;
               Q_EMIT (this->SigMessageIdChanged(this->mc_MessageId));
            }

            m_HandleCriticalMessagesAndRx(false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register message direction change

   Only for node mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnDirectionChanged(void)
{
   if (this->mq_IdIsValid)
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         const C_OSCCanMessageContainer * pc_MessageContainer;
         m_RegisterChange();
         const C_OSCCanMessageIdentificationIndices c_PrevMessageId = this->mc_MessageId;

         if (this->mpc_Ui->pc_ComboBoxDirection->currentIndex() == mu8_DIRECTION_INDEX_TRANSMIT)
         {
            this->mc_MessageId.q_MessageIsTx = true;
         }
         else
         {
            this->mc_MessageId.q_MessageIsTx = false;
         }

         //save new message data
         tgl_assert(this->mpc_MessageSyncManager->SetCanMessageDirection(c_PrevMessageId,
                                                                         this->mc_MessageId.q_MessageIsTx) == C_NO_ERR);

         //Update message index
         pc_MessageContainer = C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
            this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
            this->mc_MessageId.u32_DatapoolIndex);

         if (pc_MessageContainer != NULL)
         {
            const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(
               this->mc_MessageId.q_MessageIsTx);
            tgl_assert(rc_Messages.size() > 0);
            if (rc_Messages.size() > 0)
            {
               this->mc_MessageId.u32_MessageIndex = static_cast<uint32>(rc_Messages.size()) - 1UL;
            }
         }

         Q_EMIT (this->SigMessageIdChanged(this->mc_MessageId));

         if (this->mc_NodeDatapoolIndexes.size() > 1)
         {
            if (this->mc_MessageId.q_MessageIsTx == false)
            {
               // Activate initial all other Datapools for Rx too
               uint32 u32_Counter;

               for (u32_Counter = 0U; u32_Counter < this->mc_NodeDatapoolIndexes.size(); ++u32_Counter)
               {
                  const uint32 u32_OhterDatapoolIndex = this->mc_NodeDatapoolIndexes[u32_Counter];
                  if (this->mc_MessageId.u32_DatapoolIndex != u32_OhterDatapoolIndex)
                  {
                     const uint32 u32_DefaultReceiveTimeoutValue = this->m_GetDefaultTimeout();
                     tgl_assert(this->mpc_MessageSyncManager->AddCanMessageRx(this->mc_MessageId,
                                                                              this->mu32_NodeIndex,
                                                                              this->mu32_InterfaceIndex,
                                                                              this->mc_NodeDatapoolIndexes[u32_Counter]) ==
                                C_NO_ERR);

                     //Also add default receive timeout value (important for core values)
                     tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeout(this->mc_MessageId,
                                                                                          this->mu32_NodeIndex,
                                                                                          this->mu32_InterfaceIndex,
                                                                                          this->mc_NodeDatapoolIndexes[
                                                                                             u32_Counter],
                                                                                          u32_DefaultReceiveTimeoutValue) ==
                                C_NO_ERR);
                  }
               }

               Q_EMIT this->SigMessageRxChanged();
            }
            else
            {
               const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds =
                  this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);
               uint32 u32_Counter;

               for (u32_Counter = 0U; u32_Counter < c_MatchingMessageIds.size(); ++u32_Counter)
               {
                  const C_OSCCanMessageIdentificationIndices & rc_MsgId = c_MatchingMessageIds[u32_Counter];

                  if (rc_MsgId.q_MessageIsTx == false)
                  {
                     // Remove the other Rx messages. When Tx is selected, no Rx is allowed
                     tgl_assert(this->mpc_MessageSyncManager->DeleteCanMessageRx(this->mc_MessageId,
                                                                                 rc_MsgId.u32_NodeIndex,
                                                                                 rc_MsgId.u32_InterfaceIndex,
                                                                                 rc_MsgId.u32_DatapoolIndex) ==
                                C_NO_ERR);
                  }
               }
            }
         }

         // Refresh ui
         this->m_NodeModeDirectionChanged();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle transmitter change

   Special case: The combo box node index was changed. The Datapool combo box must be reloaded
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnTxChangedReloadDatapools(void)
{
   const sintn sn_CurrentNodeIndex = this->mpc_Ui->pc_ComboBoxTransmitterNode->currentIndex();
   bool q_Transmitter = false;

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxTransmitterDatapool,
              static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnTxChanged);

   this->mpc_Ui->pc_ComboBoxTransmitterDatapool->clear();

   // Check if a node index was selected
   if (sn_CurrentNodeIndex >= 0)
   {
      uint32 u32_MappedDatapoolNamesCounter;
      const std::vector<QString> & rc_MappingDatapoolNames = this->mc_DatapoolNamesTxSelection[sn_CurrentNodeIndex];

      // Add all to this node index associated Datapool names to the combo box
      for (u32_MappedDatapoolNamesCounter = 0U; u32_MappedDatapoolNamesCounter < rc_MappingDatapoolNames.size();
           ++u32_MappedDatapoolNamesCounter)
      {
         this->mpc_Ui->pc_ComboBoxTransmitterDatapool->addItem(rc_MappingDatapoolNames[u32_MappedDatapoolNamesCounter]);
      }
      this->mpc_Ui->pc_ComboBoxTransmitterDatapool->setCurrentIndex(0);

      q_Transmitter = true;
   }

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTransmitterDatapool,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxChanged);

   this->m_OnTxChanged();
   this->m_BusModeAdaptTransmitterComboBoxesValid(q_Transmitter);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle transmitter change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnTxChanged(void)
{
   if (this->mq_IdIsValid)
   {
      if ((this->mc_BusInterfaceIndexes.size() == this->mc_BusNodeIndexes.size()) &&
          (this->mc_BusInterfaceIndexes.size() == this->mc_BusDatapoolIndexes.size()))
      {
         if (this->mpc_MessageSyncManager != NULL)
         {
            const sint32 s32_CurrentIndex = this->m_GetVectorIndexOfComboBoxSelection();

            m_RegisterChange();

            if (s32_CurrentIndex >= 0)
            {
               const uint32 u32_CurrentIndex = static_cast<uint32>(s32_CurrentIndex);
               if (u32_CurrentIndex < this->mc_BusNodeIndexes.size())
               {
                  if (this->mpc_MessageSyncManager->ChangeCanMessageTx(this->mc_MessageId,
                                                                       this->mc_BusNodeIndexes[u32_CurrentIndex],
                                                                       this->mc_BusInterfaceIndexes[u32_CurrentIndex],
                                                                       this->mc_BusDatapoolIndexes[u32_CurrentIndex]) ==
                      C_NO_ERR)
                  {
                     //Update message id if necessary
                     if ((this->mc_MessageId.q_MessageIsTx == true) ||
                         (((this->mc_MessageId.u32_NodeIndex == this->mc_BusNodeIndexes[u32_CurrentIndex]) &&
                           (this->mc_MessageId.u32_InterfaceIndex == this->mc_BusInterfaceIndexes[u32_CurrentIndex]) &&
                           (this->mc_MessageId.u32_DatapoolIndex == this->mc_BusDatapoolIndexes[u32_CurrentIndex])) &&
                          (this->mc_MessageId.e_ComProtocol == this->me_ComProtocol)))
                     {
                        const C_OSCCanMessageContainer * const pc_MessageContainer =
                           C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                              this->mc_BusNodeIndexes[u32_CurrentIndex], this->me_ComProtocol,
                              this->mc_BusInterfaceIndexes[u32_CurrentIndex],
                              this->mc_BusDatapoolIndexes[u32_CurrentIndex]);

                        if (pc_MessageContainer != NULL)
                        {
                           //Update message id
                           this->mc_MessageId.u32_NodeIndex = this->mc_BusNodeIndexes[u32_CurrentIndex];
                           this->mc_MessageId.e_ComProtocol = this->me_ComProtocol;
                           this->mc_MessageId.u32_InterfaceIndex = this->mc_BusInterfaceIndexes[u32_CurrentIndex];
                           this->mc_MessageId.u32_DatapoolIndex = this->mc_BusDatapoolIndexes[u32_CurrentIndex];
                           this->mc_MessageId.q_MessageIsTx = true;
                           this->mc_MessageId.u32_MessageIndex =
                              static_cast<uint32>(pc_MessageContainer->c_TxMessages.size()) - 1UL;

                           Q_EMIT (this->SigMessageIdChanged(this->mc_MessageId));
                        }
                     }

                     this->mpc_Ui->pc_WidgetReceiver->CheckSpecificNode(this->mc_BusNodeIndexes[u32_CurrentIndex],
                                                                        this->mc_BusInterfaceIndexes[u32_CurrentIndex],
                                                                        this->mc_BusDatapoolIndexes[u32_CurrentIndex],
                                                                        false);

                     // update the Rx widget
                     this->m_UpdateRxAfterTxSelection(
                        this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId));
                     Q_EMIT this->SigRecheckError();
                     C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxTransmitterNode, "Valid", true);
                  }
               }
            }
         }
      }
      else
      {
         tgl_assert(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle receiver changed

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface index (ID)
   \param[in] ou32_DatapoolIndex  Datapool index (ID)
   \param[in] oq_Checked          Flag if checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnRxChanged(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                   const uint32 ou32_DatapoolIndex, const bool oq_Checked)
{
   if (this->mq_IdIsValid)
   {
      if (this->mq_InternalRxChange == false)
      {
         m_RegisterChange();
         if (this->mpc_MessageSyncManager != NULL)
         {
            if (oq_Checked == true)
            {
               const uint32 u32_DefaultReceiveTimeoutValue = this->m_GetDefaultTimeout();
               tgl_assert(this->mpc_MessageSyncManager->AddCanMessageRx(this->mc_MessageId, ou32_NodeIndex,
                                                                        ou32_InterfaceIndex,
                                                                        ou32_DatapoolIndex) == C_NO_ERR);
               //Also add default receive timeout value (important for core values)
               tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeout(this->mc_MessageId, ou32_NodeIndex,
                                                                                    ou32_InterfaceIndex,
                                                                                    ou32_DatapoolIndex,
                                                                                    u32_DefaultReceiveTimeoutValue) ==
                          C_NO_ERR);
            }
            else
            {
               bool q_Found = true;
               bool q_MessageIdChanged = false;
               const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingIds =
                  this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);
               //Update message id if necessary
               if ((this->mc_MessageId.q_MessageIsTx == false) &&
                   (this->mc_MessageId.u32_NodeIndex == ou32_NodeIndex) &&
                   (this->mc_MessageId.u32_InterfaceIndex == ou32_InterfaceIndex) &&
                   (this->mc_MessageId.u32_DatapoolIndex == ou32_DatapoolIndex))
               {
                  //Check if there is a valid message id left
                  q_Found = false;
                  for (uint32 u32_ItMatchingMessage = 0; u32_ItMatchingMessage < c_MatchingIds.size();
                       ++u32_ItMatchingMessage)
                  {
                     const C_OSCCanMessageIdentificationIndices & rc_CurrentMatchingMessageId =
                        c_MatchingIds[u32_ItMatchingMessage];
                     if ((this->mc_MessageId == rc_CurrentMatchingMessageId) == false)
                     {
                        //Everything fine
                        this->mc_MessageId = rc_CurrentMatchingMessageId;
                        q_Found = true;
                        q_MessageIdChanged = true;
                        break;
                     }
                  }
               }
               if (q_Found == true)
               {
                  this->mpc_MessageSyncManager->DeleteCanMessageRx(this->mc_MessageId, ou32_NodeIndex,
                                                                   ou32_InterfaceIndex, ou32_DatapoolIndex);
                  //Signal message id change after sync manager was updated
                  if (q_MessageIdChanged == true)
                  {
                     Q_EMIT this->SigMessageIdChanged(this->mc_MessageId);
                  }
               }
               else
               {
                  //Revert action as there would be no receiver and transmitter left
                  std::vector<stw_types::uint32> c_TmpNodeIndexes;
                  std::vector<stw_types::uint32> c_TmpInterfaceIndexes;
                  std::vector<stw_types::uint32> c_TmpDatapoolIndexes;
                  C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR, C_GtGetText::h_GetText(
                                                       "A message cannot exist without a receiver and transmitter."));
                  c_MessageBox.SetCustomMinHeight(180, 180);
                  c_TmpNodeIndexes.push_back(ou32_NodeIndex);
                  c_TmpInterfaceIndexes.push_back(ou32_InterfaceIndex);
                  c_TmpDatapoolIndexes.push_back(ou32_DatapoolIndex);

                  //Inform user
                  c_MessageBox.Execute();

                  //Revert change
                  this->mq_InternalRxChange = true;
                  this->mpc_Ui->pc_WidgetReceiver->CheckNodes(c_TmpNodeIndexes, c_TmpInterfaceIndexes,
                                                              c_TmpDatapoolIndexes);
               }
            }
         }

         Q_EMIT this->SigMessageRxChanged();
      }
      else
      {
         this->mq_InternalRxChange = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle auto receive timeout flag changed (bus mode)

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface index (ID)
   \param[in] ou32_DatapoolIndex  Datapool index (ID)
   \param[in] oe_TimeoutMode      Receive timeout mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnRxTimeoutFlagChanged(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_InterfaceIndex,
                                                              const uint32 ou32_DatapoolIndex,
                                                              const C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_TimeoutMode)
{
   if (this->mq_IdIsValid)
   {
      m_RegisterChange();
      tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeoutAutoFlag(this->mc_MessageId, ou32_NodeIndex,
                                                                                   ou32_InterfaceIndex,
                                                                                   ou32_DatapoolIndex,
                                                                                   oe_TimeoutMode) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle receive timeout value changed (bus mode)

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface index (ID)
   \param[in] ou32_DatapoolIndex  Datapool index (ID)
   \param[in] ou32_TimeoutValue   Receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnRxTimeoutValueChanged(const uint32 ou32_NodeIndex,
                                                               const uint32 ou32_InterfaceIndex,
                                                               const uint32 ou32_DatapoolIndex,
                                                               const uint32 ou32_TimeoutValue)
{
   if (this->mq_IdIsValid)
   {
      m_RegisterChange();
      tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeout(this->mc_MessageId, ou32_NodeIndex,
                                                                           ou32_InterfaceIndex, ou32_DatapoolIndex,
                                                                           ou32_TimeoutValue) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect update signals for fields which are affected by protocol changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_ConnectProtocolSpecificFields(void) const
{
   connect(this->mpc_Ui->pc_CheckBoxExtendedType, &C_OgeChxProperties::toggled, this,
           &C_SdBueMessagePropertiesWidget::m_OnPropertiesChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxId, static_cast<void (QSpinBox::*)(sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxDlc, static_cast<void (QSpinBox::*)(sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnDlcChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTxMethod, static_cast<void (QComboBox::*)(
                                                             sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxMethodChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect update signals for fields which are affected by protocol changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_DisconnectProtocolSpecificFields(void) const
{
   disconnect(this->mpc_Ui->pc_CheckBoxExtendedType, &C_OgeChxProperties::toggled, this,
              &C_SdBueMessagePropertiesWidget::m_OnPropertiesChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxId, static_cast<void (QSpinBox::*)(sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnIdChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxDlc, static_cast<void (QSpinBox::*)(sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnDlcChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxTxMethod, static_cast<void (QComboBox::*)(
                                                                sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnTxMethodChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect update signals for fields which are affected by node changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_ConnectNodeSpecificFields(void) const
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTransmitterNode, static_cast<void (QComboBox::*)(
                                                                    sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxChangedReloadDatapools);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTransmitterDatapool,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxChanged);
   connect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeToggled, this,
           &C_SdBueMessagePropertiesWidget::m_OnRxChanged);
   connect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeReceiveTimeoutMode, this,
           &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutFlagChanged);
   connect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeReceiveTimeout, this,
           &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect update signals for fields which are affected by node changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_DisconnectNodeSpecificFields(void) const
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxTransmitterNode,
              static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged),
              this, &C_SdBueMessagePropertiesWidget::m_OnTxChangedReloadDatapools);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxTransmitterDatapool,
              static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnTxChanged);
   disconnect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeToggled, this,
              &C_SdBueMessagePropertiesWidget::m_OnRxChanged);
   disconnect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeReceiveTimeoutMode, this,
              &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutFlagChanged);
   disconnect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeReceiveTimeout, this,
              &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set widgets visible/invisible depending on 'mode' (node vs bus).

   \param[in]       oq_BusMode  true: bus mode, false: node mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_SetModeSpecificWidgetsVisible(const bool oq_BusMode) const
{
   // bus edit widgets
   this->mpc_Ui->pc_LabelTransmitterDatapool->setVisible(oq_BusMode);
   this->mpc_Ui->pc_ComboBoxTransmitterDatapool->setVisible(oq_BusMode);
   this->mpc_Ui->pc_LabelTransmitterNode->setVisible(oq_BusMode);
   this->mpc_Ui->pc_ComboBoxTransmitterNode->setVisible(oq_BusMode);

   // node edit widgets
   this->mpc_Ui->pc_LabelDirection->setVisible(!oq_BusMode);
   this->mpc_Ui->pc_ComboBoxDirection->setVisible(!oq_BusMode);
   this->mpc_Ui->pc_LabelTransmitterDatapoolOnly->setVisible(!oq_BusMode);
   this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly->setVisible(!oq_BusMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set transmit widgets visible and receive widgets invisible or vice versa.

   \param[in]       oq_Visible   true: show transmit and hide receive
                                 false: hide transmit and show receive
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_SetNodeModeTransmitVisible(const bool oq_Visible) const
{
   this->mpc_Ui->pc_LabelTransmitter->setVisible(oq_Visible);
   this->mpc_Ui->pc_SeparatorTransmitter->setVisible(oq_Visible);
   this->mpc_Ui->pc_LabelTransmitterDatapoolOnly->setVisible(oq_Visible);
   this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly->setVisible(oq_Visible);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node mode receive widgets visible/invisible.

   \param[in]       oq_Visible   true: show, false: hide
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_SetNodeModeReceiveVisible(const bool oq_Visible) const
{
   this->mpc_Ui->pc_LabelReceiver->setVisible(oq_Visible);
   this->mpc_Ui->pc_SeparatorReceiver->setVisible(oq_Visible);
   this->mpc_Ui->pc_WidgetReceiver->setVisible(oq_Visible);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change (without signal dis-/reconnection)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_ReloadNodes(void)
{
   if (this->mq_ModeSingleNode == false)
   {
      C_PuiSdNodeCanMessageSyncManager::h_GetConnectedAndActiveInterfaces(this->mu32_BusIndex,
                                                                          this->me_ComProtocol,
                                                                          this->mc_BusNodeIndexes,
                                                                          this->mc_BusInterfaceIndexes,
                                                                          this->mc_BusDatapoolIndexes);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_UpdateTxSelection(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds)
{
   bool q_Transmitter = false;

   std::vector<QString> c_NodeNames;
   std::vector<QString> c_DatapoolNames;

   // fill combo box
   this->mpc_Ui->pc_ComboBoxTransmitterNode->clear();
   this->mpc_Ui->pc_ComboBoxTransmitterDatapool->clear();
   this->mc_MappingTxSelection.clear();
   this->mc_DatapoolNamesTxSelection.clear();

   if (C_SdUtil::h_GetNames(this->mc_BusNodeIndexes, this->mc_BusInterfaceIndexes, c_NodeNames,
                            false, &this->mc_BusDatapoolIndexes, &c_DatapoolNames) == C_NO_ERR)
   {
      std::vector<uint32> c_MappingDatapools;
      std::vector<QString> c_MappingDatapoolNames;
      for (uint32 u32_ItName = 0; u32_ItName < c_NodeNames.size(); ++u32_ItName)
      {
         const uint32 u32_ItNextName = u32_ItName + 1U;
         const QString c_CurrentName = c_NodeNames[u32_ItName];

         c_MappingDatapools.push_back(u32_ItName);
         c_MappingDatapoolNames.push_back(c_DatapoolNames[u32_ItName]);

         if ((u32_ItNextName >= c_NodeNames.size()) ||
             (c_NodeNames[u32_ItNextName] != c_CurrentName))
         {
            // No further entries or the next entry has a different node name ->
            // Finished. Add all collected indexes for each associated Datapool
            this->mpc_Ui->pc_ComboBoxTransmitterNode->addItem(c_CurrentName);
            this->mc_MappingTxSelection.push_back(c_MappingDatapools);
            // Save the Datapool names as well due to necessary reloading of Datapool combo box when
            // changing node index combo box
            this->mc_DatapoolNamesTxSelection.push_back(c_MappingDatapoolNames);

            // Next entry is an other node name, clear the collected indexes
            c_MappingDatapools.clear();
            c_MappingDatapoolNames.clear();
         }
      }
   }

   // get the actual selection
   tgl_assert((this->mc_BusNodeIndexes.size() == this->mc_BusInterfaceIndexes.size()) &&
              (this->mc_BusNodeIndexes.size() == this->mc_BusDatapoolIndexes.size()) &&
              (this->mc_BusInterfaceIndexes.size() == c_NodeNames.size()) &&
              (this->mc_BusInterfaceIndexes.size() == c_DatapoolNames.size()));

   if ((this->mc_BusNodeIndexes.size() == this->mc_BusInterfaceIndexes.size()) &&
       (this->mc_BusInterfaceIndexes.size() == c_NodeNames.size()) &&
       (this->mc_BusInterfaceIndexes.size() == c_DatapoolNames.size()))
   {
      for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MatchingMessageIds.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId = orc_MatchingMessageIds[u32_ItMessage];
         if (rc_CurrentMessageId.q_MessageIsTx == true)
         {
            for (uint32 u32_ItNode = 0U; u32_ItNode < this->mc_BusNodeIndexes.size(); ++u32_ItNode)
            {
               if ((this->mc_BusNodeIndexes[u32_ItNode] == rc_CurrentMessageId.u32_NodeIndex) &&
                   (this->mc_BusInterfaceIndexes[u32_ItNode] == rc_CurrentMessageId.u32_InterfaceIndex) &&
                   (this->mc_BusDatapoolIndexes[u32_ItNode] == rc_CurrentMessageId.u32_DatapoolIndex))
               {
                  uint32 u32_ComboBoxNodeIndex = 0U;
                  uint32 u32_ComboBoxDatapoolIndex = 0U;
                  uint32 u32_CounterDatapoolIndex;
                  const std::vector<QString> * pc_MappedDatapoolNames;

                  // Get the indexes of the combo boxes
                  m_GetComboBoxIndexesByVectorIndex(u32_ItNode, u32_ComboBoxNodeIndex, u32_ComboBoxDatapoolIndex);

                  pc_MappedDatapoolNames = &this->mc_DatapoolNamesTxSelection[u32_ComboBoxNodeIndex];
                  this->mpc_Ui->pc_ComboBoxTransmitterNode->setCurrentIndex(static_cast<sintn>(u32_ComboBoxNodeIndex));

                  // Add all associated Datapools
                  for (u32_CounterDatapoolIndex = 0U;
                       u32_CounterDatapoolIndex < pc_MappedDatapoolNames->size();
                       ++u32_CounterDatapoolIndex)
                  {
                     this->mpc_Ui->pc_ComboBoxTransmitterDatapool->addItem(
                        (*pc_MappedDatapoolNames)[u32_CounterDatapoolIndex]);
                  }
                  // Select the exact matching one
                  this->mpc_Ui->pc_ComboBoxTransmitterDatapool->setCurrentIndex(u32_ComboBoxDatapoolIndex);

                  q_Transmitter = true;
                  break;
               }
            }

            if (q_Transmitter == true)
            {
               break;
            }
         }
      }
   }

   this->m_BusModeAdaptTransmitterComboBoxesValid(q_Transmitter);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_UpdateRxAfterTxSelection(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds, const bool oq_SkipDisconnect)
{
   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   std::vector<uint32> c_DatapoolIndexes;
   std::vector<QString> c_NodeNames;
   std::vector<QString> c_DatapoolNames;
   bool q_TxSelected = false;

   if (oq_SkipDisconnect == false)
   {
      this->m_DisconnectNodeSpecificFields();
   }

   // get all names
   if (C_SdUtil::h_GetNames(this->mc_BusNodeIndexes, this->mc_BusInterfaceIndexes, c_NodeNames, false,
                            &this->mc_BusDatapoolIndexes, &c_DatapoolNames) == C_NO_ERR)
   {
      std::vector<C_PuiSdNodeCanMessage::E_RxTimeoutMode> c_ReceiveTimeoutModes;
      std::vector<uint32> c_ReceiveTimeoutValues;
      const sint32 s32_CurrentNodeIndex = this->mpc_Ui->pc_ComboBoxTransmitterNode->currentIndex();

      //Fill those flags
      if ((this->mc_BusNodeIndexes.size() == this->mc_BusInterfaceIndexes.size()) &&
          (this->mc_BusNodeIndexes.size() == this->mc_BusDatapoolIndexes.size()))
      {
         c_ReceiveTimeoutValues.reserve(this->mc_BusNodeIndexes.size());
         c_ReceiveTimeoutModes.reserve(this->mc_BusNodeIndexes.size());
         for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_BusNodeIndexes.size(); ++u32_ItNode)
         {
            bool q_Found = false;
            for (uint32 u32_ItMessageId = 0; u32_ItMessageId < orc_MatchingMessageIds.size(); ++u32_ItMessageId)
            {
               const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId =
                  orc_MatchingMessageIds[u32_ItMessageId];

               if ((rc_CurrentMessageId.u32_NodeIndex == this->mc_BusNodeIndexes[u32_ItNode]) &&
                   (rc_CurrentMessageId.u32_InterfaceIndex == this->mc_BusInterfaceIndexes[u32_ItNode]) &&
                   (rc_CurrentMessageId.u32_DatapoolIndex == this->mc_BusDatapoolIndexes[u32_ItNode]))
               {
                  const C_OSCCanMessage * const pc_OSCData = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                     rc_CurrentMessageId);
                  const C_PuiSdNodeCanMessage * const pc_UiData = C_PuiSdHandler::h_GetInstance()->GetUiCanMessage(
                     rc_CurrentMessageId);

                  //No assertion because this may happen (not critical anyway)
                  if ((pc_OSCData != NULL) && (pc_UiData != NULL))
                  {
                     //Timeout value
                     c_ReceiveTimeoutValues.push_back(pc_OSCData->u32_TimeoutMs);
                     //Timeout flag
                     c_ReceiveTimeoutModes.push_back(pc_UiData->e_ReceiveTimeoutMode);
                     q_Found = true;
                  }
                  break;
               }
            }
            if (q_Found == false)
            {
               //Add default
               c_ReceiveTimeoutModes.push_back(C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO);
               c_ReceiveTimeoutValues.push_back(this->m_GetDefaultTimeout());
            }
         }
      }
      else
      {
         tgl_assert(false);
      }

      // is one node selected as Tx?
      if (s32_CurrentNodeIndex >= 0)
      {
         std::vector<QString>::iterator c_ItName;
         std::vector<std::vector<QString>::iterator> c_NamesToDelete;
         std::vector<uint32> c_RxNodeIndexes;
         std::vector<uint32> c_RxInterfaceIndexes;
         std::vector<uint32> c_RxDatapoolIndexes;
         std::vector<QString> c_RxDatapoolNames;
         std::vector<C_PuiSdNodeCanMessage::E_RxTimeoutMode> c_RxReceiveTimeoutModes;
         std::vector<uint32> c_RxReceiveTimeoutValues;
         std::vector<uint32> * pc_MappedDatapools;
         uint32 u32_EntryCounter = 0U;
         uint32 u32_RemoveCounter;

         // remove this node from the vector
         tgl_assert(this->mc_BusNodeIndexes.size() == this->mc_BusInterfaceIndexes.size());
         tgl_assert(this->mc_BusNodeIndexes.size() == this->mc_BusDatapoolIndexes.size());
         tgl_assert(c_NodeNames.size() == this->mc_BusInterfaceIndexes.size());
         tgl_assert(this->mc_BusDatapoolIndexes.size() == this->mc_BusInterfaceIndexes.size());
         tgl_assert(c_DatapoolNames.size() == this->mc_BusInterfaceIndexes.size());

         tgl_assert(s32_CurrentNodeIndex < static_cast<sint32>(this->mc_MappingTxSelection.size()));
         pc_MappedDatapools = &this->mc_MappingTxSelection[s32_CurrentNodeIndex];

         for (c_ItName = c_NodeNames.begin(); c_ItName != c_NodeNames.end(); ++c_ItName)
         {
            bool q_Delete = false;
            uint32 u32_MappedDatapoolsCounter;

            // Delete all entries with the same node index and interface index and all associated Datapools
            // Tx and Rx is not allowed on different Datapools c_RxDatapoolIndexes
            // Search for matching indexes
            for (u32_MappedDatapoolsCounter = 0U; u32_MappedDatapoolsCounter < pc_MappedDatapools->size();
                 ++u32_MappedDatapoolsCounter)
            {
               if (u32_EntryCounter == (*pc_MappedDatapools)[u32_MappedDatapoolsCounter])
               {
                  q_Delete = true;
                  break;
               }
            }

            if (q_Delete == true)
            {
               // Mark the matching indexes for removing
               c_NamesToDelete.push_back(c_ItName);
            }
            else
            {
               // synchronize the other necessary vectors
               c_RxNodeIndexes.push_back(this->mc_BusNodeIndexes[u32_EntryCounter]);
               c_RxInterfaceIndexes.push_back(this->mc_BusInterfaceIndexes[u32_EntryCounter]);
               c_RxDatapoolIndexes.push_back(this->mc_BusDatapoolIndexes[u32_EntryCounter]);
               c_RxDatapoolNames.push_back(c_DatapoolNames[u32_EntryCounter]);
               c_RxReceiveTimeoutModes.push_back(c_ReceiveTimeoutModes[u32_EntryCounter]);
               c_RxReceiveTimeoutValues.push_back(c_ReceiveTimeoutValues[u32_EntryCounter]);
            }
            ++u32_EntryCounter;
         }

         for (u32_RemoveCounter = 0U; u32_RemoveCounter < c_NamesToDelete.size(); ++u32_RemoveCounter)
         {
            // Remove the marked entries
            c_NodeNames.erase(c_NamesToDelete[u32_RemoveCounter]);
         }

         this->mpc_Ui->pc_WidgetReceiver->AddNodes(c_NodeNames, c_RxDatapoolNames, c_RxNodeIndexes,
                                                   c_RxInterfaceIndexes,
                                                   c_RxDatapoolIndexes,
                                                   c_RxReceiveTimeoutModes, c_RxReceiveTimeoutValues);

         q_TxSelected = true;
      }
      else
      {
         // no restrictions by Tx
         this->mpc_Ui->pc_WidgetReceiver->AddNodes(c_NodeNames, c_DatapoolNames, this->mc_BusNodeIndexes,
                                                   this->mc_BusInterfaceIndexes,
                                                   this->mc_BusDatapoolIndexes,
                                                   c_ReceiveTimeoutModes, c_ReceiveTimeoutValues);
      }
   }

   // prepare the vectors
   if (orc_MatchingMessageIds.size() > 1)
   {
      uint32 u32_Size;
      if (q_TxSelected == true)
      {
         //Typical size (1 tx, X rx)
         u32_Size = static_cast<uint32>(orc_MatchingMessageIds.size()) - 1UL;
      }
      else
      {
         u32_Size = orc_MatchingMessageIds.size();
      }

      c_NodeIndexes.reserve(u32_Size);
      c_InterfaceIndexes.reserve(u32_Size);
      c_DatapoolIndexes.reserve(u32_Size);
   }

   // update the selection
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MatchingMessageIds.size(); ++u32_ItMessage)
   {
      const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId = orc_MatchingMessageIds[u32_ItMessage];
      if (rc_CurrentMessageId.q_MessageIsTx == false)
      {
         c_NodeIndexes.push_back(rc_CurrentMessageId.u32_NodeIndex);
         c_InterfaceIndexes.push_back(rc_CurrentMessageId.u32_InterfaceIndex);
         c_DatapoolIndexes.push_back(rc_CurrentMessageId.u32_DatapoolIndex);
      }
   }
   this->mpc_Ui->pc_WidgetReceiver->CheckNodes(c_NodeIndexes, c_InterfaceIndexes, c_DatapoolIndexes);
   if (oq_SkipDisconnect == false)
   {
      m_ConnectNodeSpecificFields();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle critical message state

   \param[in]  oq_UpdateRx       Flag if Rx list must be updated
   \param[in]  oq_HandleSignals  Flag if signals must be handled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_HandleCriticalMessagesAndRx(const bool oq_UpdateRx, const bool oq_HandleSignals)
{
   if (this->mq_IdIsValid)
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         if (this->mpc_MessageSyncManager->CheckCriticalMessageId(this->mc_MessageId) == true)
         {
            this->mpc_Ui->pc_GroupBoxCriticalMessage->setVisible(true);
            this->mpc_Ui->pc_WidgetReceiver->setVisible(false);
         }
         else
         {
            const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds =
               this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);
            if (oq_HandleSignals == true)
            {
               m_DisconnectNodeSpecificFields();
            }
            this->mpc_Ui->pc_GroupBoxCriticalMessage->setVisible(false);

            if (oq_UpdateRx == true)
            {
               if (this->mq_ModeSingleNode == false)
               {
                  //Select receiver
                  this->m_UpdateRxAfterTxSelection(c_MatchingMessageIds, true);
               }

               if ((this->mq_ModeSingleNode == false) ||
                   (this->mpc_Ui->pc_ComboBoxDirection->currentIndex() == mu8_DIRECTION_INDEX_RECEIVE))
               {
                  this->mpc_Ui->pc_WidgetReceiver->setVisible(true);
               }
            }

            if (oq_HandleSignals == true)
            {
               m_ConnectNodeSpecificFields();
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the vectors mc_NodeIndexes, mc_InterfaceIndexes and mc_DatapoolIndexes

   Dependent of the current selection in both combo boxes

   \return
   >= 0: Index value
   <  0: No valid index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueMessagePropertiesWidget::m_GetVectorIndexOfComboBoxSelection(void) const
{
   sint32 s32_Return = -1;

   if ((this->mpc_Ui->pc_ComboBoxTransmitterNode->currentIndex() >= 0) &&
       (this->mpc_Ui->pc_ComboBoxTransmitterNode->currentIndex() <
        static_cast<sintn>(this->mc_MappingTxSelection.size())))
   {
      const std::vector<uint32> & rc_MappingDatapools =
         this->mc_MappingTxSelection[this->mpc_Ui->pc_ComboBoxTransmitterNode->currentIndex()];

      if ((this->mpc_Ui->pc_ComboBoxTransmitterDatapool->currentIndex() >= 0) &&
          (this->mpc_Ui->pc_ComboBoxTransmitterDatapool->currentIndex() <
           static_cast<sintn>(rc_MappingDatapools.size())))
      {
         s32_Return = rc_MappingDatapools[this->mpc_Ui->pc_ComboBoxTransmitterDatapool->currentIndex()];
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the combo boxes for node and Datapool for a specific vector index

   Throws assertion when not found

   \param[in]       ou32_VectorIndex            Index of the vectors mc_NodeIndexes, mc_InterfaceIndexes and
                                                mc_DatapoolIndexes
   \param[out]      oru32_ComboBoxNodeIndex     Detected index of node combo box
   \param[out]      oru32_ComboBoxDatapoolIndex Detected index of Datapool combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_GetComboBoxIndexesByVectorIndex(const uint32 ou32_VectorIndex,
                                                                       uint32 & oru32_ComboBoxNodeIndex,
                                                                       uint32 & oru32_ComboBoxDatapoolIndex) const
{
   uint32 u32_CounterNodes;
   uint32 u32_CounterDatapools;
   bool q_Found = false;

   // Search a matching mapping
   for (u32_CounterNodes = 0U; u32_CounterNodes < this->mc_MappingTxSelection.size(); ++u32_CounterNodes)
   {
      const std::vector<stw_types::uint32> & rc_DatapoolIndexes = this->mc_MappingTxSelection[u32_CounterNodes];

      for (u32_CounterDatapools = 0U; u32_CounterDatapools < rc_DatapoolIndexes.size(); ++u32_CounterDatapools)
      {
         if (rc_DatapoolIndexes[u32_CounterDatapools] == ou32_VectorIndex)
         {
            oru32_ComboBoxNodeIndex = u32_CounterNodes;
            oru32_ComboBoxDatapoolIndex = u32_CounterDatapools;
            q_Found = true;
            break;
         }
      }

      if (q_Found == true)
      {
         break;
      }
   }

   tgl_assert(q_Found);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync cycle spin box to later spin box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_SyncCycleToLater(void) const
{
   this->mpc_Ui->pc_SpinBoxCycleTime->setValue(this->mpc_Ui->pc_SpinBoxLater->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sync later spin box to cycle spin box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_SyncLaterToCycle(void) const
{
   this->mpc_Ui->pc_SpinBoxLater->setValue(this->mpc_Ui->pc_SpinBoxCycleTime->value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the tool tip and color of combo boxes for node and Datapool transmitter configuration

   \param[in]       oq_Valid     Flag if transmitter configuration is valid or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_BusModeAdaptTransmitterComboBoxesValid(const bool oq_Valid) const
{
   if (oq_Valid == false)
   {
      const QString c_Content = C_GtGetText::h_GetText("No transmitter selected.");
      //Set invalid
      this->mpc_Ui->pc_ComboBoxTransmitterNode->setCurrentIndex(-1);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxTransmitterNode, "Valid", false);
      this->mpc_Ui->pc_ComboBoxTransmitterNode->SetToolTipInformation("", c_Content, C_NagToolTip::eERROR);

      this->mpc_Ui->pc_ComboBoxTransmitterDatapool->setVisible(false);
      this->mpc_Ui->pc_LabelTransmitterDatapool->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxTransmitterNode->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxTransmitterNode, "Valid", true);

      this->mpc_Ui->pc_ComboBoxTransmitterDatapool->setVisible(
         this->mpc_Ui->pc_ComboBoxTransmitterDatapool->count() > 1);
      this->mpc_Ui->pc_LabelTransmitterDatapool->setVisible(
         this->mpc_Ui->pc_ComboBoxTransmitterDatapool->count() > 1);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adaption and updated of the ui due to changed direction

   \param[in]  oq_DontDisconnect    Do a disconnect and connect for all change signals
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_NodeModeDirectionChanged(const bool oq_Disconnect)
{
   if (this->mq_IdIsValid)
   {
      if ((this->mq_ModeSingleNode == true) &&
          (this->mpc_MessageSyncManager != NULL))
      {
         std::vector<uint32> c_NodeIndexes;
         std::vector<uint32> c_InterfaceIndexes;
         std::vector<QString> c_NodeNames;
         std::vector<QString> c_DatapoolNames;

         // Working in single node mode always with same node and interface index
         c_NodeIndexes.resize(this->mc_NodeDatapoolIndexes.size(), this->mu32_NodeIndex);
         c_InterfaceIndexes.resize(this->mc_NodeDatapoolIndexes.size(), this->mu32_InterfaceIndex);

         if (oq_Disconnect == true)
         {
            this->DisconnectAllChanges();
         }

         // get all names
         if (C_SdUtil::h_GetNames(c_NodeIndexes, c_InterfaceIndexes, c_NodeNames, false,
                                  &this->mc_NodeDatapoolIndexes, &c_DatapoolNames) == C_NO_ERR)
         {
            tgl_assert(c_NodeNames.size() == c_NodeIndexes.size());
            tgl_assert(c_DatapoolNames.size() == c_NodeIndexes.size());

            if (this->mpc_Ui->pc_ComboBoxDirection->currentIndex() == mu8_DIRECTION_INDEX_RECEIVE)
            {
               // Prepare receive widget for single node mode
               std::vector<C_PuiSdNodeCanMessage::E_RxTimeoutMode> c_ReceiveTimeoutModes;
               std::vector<uint32> c_ReceiveTimeoutValues;
               const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds =
                  this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);
               std::vector<uint32> c_CheckedDatapools;
               uint32 u32_CounterCheckedDatapools;

               // Hide transmit and show receive Datapool configuration
               this->m_SetNodeModeTransmitVisible(false);
               this->m_SetNodeModeReceiveVisible(true);

               c_ReceiveTimeoutValues.reserve(c_NodeIndexes.size());
               c_ReceiveTimeoutModes.reserve(c_NodeIndexes.size());

               for (uint32 u32_ItDatapools = 0U; u32_ItDatapools < this->mc_NodeDatapoolIndexes.size();
                    ++u32_ItDatapools)
               {
                  bool q_Found = false;
                  for (uint32 u32_ItMessageId = 0; u32_ItMessageId < c_MatchingMessageIds.size(); ++u32_ItMessageId)
                  {
                     const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId =
                        c_MatchingMessageIds[u32_ItMessageId];

                     if ((rc_CurrentMessageId.u32_NodeIndex == c_NodeIndexes[u32_ItDatapools]) &&
                         (rc_CurrentMessageId.u32_InterfaceIndex == c_InterfaceIndexes[u32_ItDatapools]) &&
                         (rc_CurrentMessageId.u32_DatapoolIndex == this->mc_NodeDatapoolIndexes[u32_ItDatapools]))
                     {
                        const C_OSCCanMessage * const pc_OSCData = C_PuiSdHandler::h_GetInstance()->GetCanMessage(
                           rc_CurrentMessageId);
                        const C_PuiSdNodeCanMessage * const pc_UiData =
                           C_PuiSdHandler::h_GetInstance()->GetUiCanMessage(
                              rc_CurrentMessageId);

                        //No assertion because this may happen (not critical anyway)
                        if ((pc_OSCData != NULL) && (pc_UiData != NULL))
                        {
                           //Timeout value
                           c_ReceiveTimeoutValues.push_back(pc_OSCData->u32_TimeoutMs);
                           //Timeout flag
                           c_ReceiveTimeoutModes.push_back(pc_UiData->e_ReceiveTimeoutMode);
                           q_Found = true;
                        }

                        c_CheckedDatapools.push_back(rc_CurrentMessageId.u32_DatapoolIndex);
                        break;
                     }
                  }

                  if (q_Found == false)
                  {
                     //Add default
                     c_ReceiveTimeoutModes.push_back(C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO);
                     c_ReceiveTimeoutValues.push_back(this->m_GetDefaultTimeout());
                  }
               }

               this->mpc_Ui->pc_WidgetReceiver->AddNodes(c_NodeNames, c_DatapoolNames, c_NodeIndexes,
                                                         c_InterfaceIndexes,
                                                         this->mc_NodeDatapoolIndexes,
                                                         c_ReceiveTimeoutModes, c_ReceiveTimeoutValues);

               this->mpc_Ui->pc_WidgetReceiver->SetModeSingleNode(true);

               for (u32_CounterCheckedDatapools = 0U; u32_CounterCheckedDatapools < c_CheckedDatapools.size();
                    ++u32_CounterCheckedDatapools)
               {
                  this->mpc_Ui->pc_WidgetReceiver->CheckSpecificNode(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                                                                     c_CheckedDatapools[u32_CounterCheckedDatapools],
                                                                     true);
               }
            }
            else
            {
               // Hide receive and show transmit Datapool configuration
               this->m_SetNodeModeTransmitVisible(true);
               this->m_SetNodeModeReceiveVisible(false);

               // Clear deprecated config
               this->mpc_Ui->pc_WidgetReceiver->Clear();
               this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly->clear();

               // Show the Datapool selection only if at least two Datapools are available
               if (c_DatapoolNames.size() > 1)
               {
                  uint32 u32_Counter;

                  for (u32_Counter = 0U; u32_Counter < c_DatapoolNames.size(); ++u32_Counter)
                  {
                     this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly->addItem(c_DatapoolNames[u32_Counter]);

                     if (this->mc_MessageId.u32_DatapoolIndex == this->mc_NodeDatapoolIndexes[u32_Counter])
                     {
                        // Select the matching Datapool
                        this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly->setCurrentIndex(u32_Counter);
                     }
                  }
               }
               else
               {
                  // Hide transmit Datapool configuration
                  this->m_SetNodeModeTransmitVisible(false);
               }
            }
         }

         if (oq_Disconnect == true)
         {
            this->ConnectAllChanges();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot when the Datapool of a Tx message was changed in case of node mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_NodeModeTransmitDatapoolChanged(void)
{
   if (this->mq_IdIsValid)
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         const sintn sn_CurrentIndex = this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly->currentIndex();
         if ((sn_CurrentIndex >= 0) &&
             (sn_CurrentIndex < static_cast<sintn>(this->mc_NodeDatapoolIndexes.size())))
         {
            const uint32 u32_SelectedDatapoolIndex = this->mc_NodeDatapoolIndexes[sn_CurrentIndex];

            if (this->mpc_MessageSyncManager->ChangeCanMessageTx(this->mc_MessageId,
                                                                 this->mu32_NodeIndex,
                                                                 this->mu32_InterfaceIndex,
                                                                 u32_SelectedDatapoolIndex) == C_NO_ERR)
            {
               const C_OSCCanMessageContainer * const pc_MessageContainer =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                     this->mu32_NodeIndex, this->me_ComProtocol,
                     this->mu32_InterfaceIndex, u32_SelectedDatapoolIndex);

               if (pc_MessageContainer != NULL)
               {
                  //Update message id
                  this->mc_MessageId.u32_DatapoolIndex = u32_SelectedDatapoolIndex;
                  this->mc_MessageId.q_MessageIsTx = true;
                  this->mc_MessageId.u32_MessageIndex = static_cast<uint32>(pc_MessageContainer->c_TxMessages.size()) -
                                                        1UL;

                  Q_EMIT this->SigMessageIdChanged(this->mc_MessageId);
               }

               Q_EMIT this->SigRecheckError();
            }
         }
         else
         {
            tgl_assert(false);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the default timeout value

   \return
   Default timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdBueMessagePropertiesWidget::m_GetDefaultTimeout(void) const
{
   uint32 u32_Return;

   if (this->mpc_Ui->pc_ComboBoxTxMethod->currentIndex() != ms32_TX_TYPE_INDEX_SPONTANEOUS)
   {
      // Only in case of Tx Method on Event the SpinBox value is not usable for sure
      u32_Return = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxCycleTime->value());
   }
   else
   {
      u32_Return = mu32_DEFAULT_CYCLE_TIME;
   }

   u32_Return = (u32_Return * 3UL) + 10UL;

   return u32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node 'mode' of the widget with all necessary indexes

   \param[in] ou32_NodeIndex      Node index
   \param[in] ou32_InterfaceIndex Interface index
   \param[in] orc_DatapoolIndexes All Datapool indexes associated to the same protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetNodeId(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_InterfaceIndex,
                                               const std::vector<stw_types::uint32> & orc_DatapoolIndexes)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_InterfaceIndex = ou32_InterfaceIndex;
   this->mc_NodeDatapoolIndexes = orc_DatapoolIndexes;
   this->mq_ModeSingleNode = true;

   // show/hide mode specific widgets
   this->m_SetModeSpecificWidgetsVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the bus 'mode' of the widget

   \param[in] ou32_BusIndex Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetBusId(const uint32 ou32_BusIndex)
{
   this->mu32_BusIndex = ou32_BusIndex;
   this->mq_ModeSingleNode = false;

   // show/hide mode specific widgets
   this->m_SetModeSpecificWidgetsVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::OnConnectionChange(void)
{
   if (this->mq_IdIsValid)
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         C_OSCCanMessageIdentificationIndices c_NewMessageId;
         if (this->mpc_MessageSyncManager->RecheckCriticalMessage(this->mc_MessageId, c_NewMessageId) == true)
         {
            this->mc_MessageId = c_NewMessageId;
            Q_EMIT this->SigMessageIdChanged(this->mc_MessageId);
         }
         {
            const std::vector<C_OSCCanMessageIdentificationIndices> c_MatchingMessageIds =
               this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);

            m_DisconnectNodeSpecificFields();

            //Reload connected nodes
            this->m_ReloadNodes();

            //transmitter / receiver
            //Select transmitter
            this->m_UpdateTxSelection(c_MatchingMessageIds);

            //Select receiver
            m_HandleCriticalMessagesAndRx(true, false);

            m_ConnectNodeSpecificFields();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new com protocol

   \param[in] ore_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetComProtocol(const C_OSCCanProtocol::E_Type & ore_Value)
{
   DisconnectAllChanges();
   this->me_ComProtocol = ore_Value;
   //Protocol specific changes
   if (ore_Value == C_OSCCanProtocol::eECES)
   {
      //Can id
      this->mpc_Ui->pc_CheckBoxExtendedType->setEnabled(true);
      this->mpc_Ui->pc_CheckBoxExtendedType->setChecked(false);

      //Dlc
      this->mpc_Ui->pc_SpinBoxDlc->setEnabled(false);
      this->mpc_Ui->pc_SpinBoxDlc->setValue(8);

      //Tx method
      this->mpc_Ui->pc_ComboBoxTxMethod->setEnabled(false);
      this->mpc_Ui->pc_ComboBoxTxMethod->setCurrentIndex(ms32_TX_TYPE_INDEX_CYCLIC);
   }
   else if (ore_Value == C_OSCCanProtocol::eCAN_OPEN_SAFETY)
   {
      //Can id
      this->mpc_Ui->pc_CheckBoxExtendedType->setEnabled(false);
      this->mpc_Ui->pc_CheckBoxExtendedType->setChecked(false);

      //Dlc
      this->mpc_Ui->pc_SpinBoxDlc->setEnabled(true);

      //Tx method
      this->mpc_Ui->pc_ComboBoxTxMethod->setEnabled(false);
      this->mpc_Ui->pc_ComboBoxTxMethod->setCurrentIndex(ms32_TX_TYPE_INDEX_CYCLIC);
   }
   else
   {
      //Layer 2

      //Can id
      this->mpc_Ui->pc_CheckBoxExtendedType->setEnabled(true);
      this->mpc_Ui->pc_CheckBoxExtendedType->setChecked(false);

      //Dlc
      this->mpc_Ui->pc_SpinBoxDlc->setEnabled(true);

      //Tx method
      this->mpc_Ui->pc_ComboBoxTxMethod->setEnabled(true);
   }
   //Min max handling (initially extended type is always set to false)
   m_OnExtendedChange(false);
   //Restrict delay time to 16 bit unsigned
   this->mpc_Ui->pc_SpinBoxEarly->SetMinimumCustom(0);
   this->mpc_Ui->pc_SpinBoxEarly->SetMaximumCustom(50000);
   //Restrict cycle time to 50000 (CANdb++ limit)
   this->mpc_Ui->pc_SpinBoxCycleTime->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxCycleTime->SetMaximumCustom(50000);
   this->mpc_Ui->pc_SpinBoxLater->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxLater->SetMaximumCustom(50000);
   ConnectAllChanges();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects the node name in the text edit for fast editing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SelectName(void) const
{
   this->mpc_Ui->pc_LineEditName->setFocus();
   this->mpc_Ui->pc_LineEditName->selectAll();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In case of a disconnected node update of the unique message ids

   \param[in]     ou32_NodeIndex      Node index
   \param[in]     ou32_InterfaceIndex Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::OnNodeDisconnected(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex)
{
   if (this->mq_IdIsValid)
   {
      if ((this->mpc_MessageSyncManager != NULL) &&
          (this->mc_MessageId.u32_NodeIndex == ou32_NodeIndex) &&
          (this->mc_MessageId.u32_InterfaceIndex == ou32_InterfaceIndex))
      {
         // This message id is affected of the change -> Check if it has matching message ids with an other
         // node/interface combination
         this->mpc_MessageSyncManager->ReplaceMessageIdWithMatchingId(this->mc_MessageId);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect update signals for fields which signal changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::ConnectAllChanges(void) const
{
   m_ConnectProtocolSpecificFields();
   m_ConnectNodeSpecificFields();
   connect(this->mpc_Ui->pc_LineEditName, &QLineEdit::editingFinished, this,
           &C_SdBueMessagePropertiesWidget::m_OnNameChanged);
   connect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
           &C_SdBueMessagePropertiesWidget::m_OnPropertiesChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxCycleTime, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnCycleTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxEarly, static_cast<void (QSpinBox::*)(
                                                         sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnEarlyOrLaterTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxLater, static_cast<void (QSpinBox::*)(
                                                         sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnEarlyOrLaterTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxDirection, static_cast<void (QComboBox::*)(
                                                              sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnDirectionChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly,
           static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged),
           this, &C_SdBueMessagePropertiesWidget::m_NodeModeTransmitDatapoolChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect update signals for fields which signal changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::DisconnectAllChanges(void) const
{
   m_DisconnectProtocolSpecificFields();
   m_DisconnectNodeSpecificFields();
   disconnect(this->mpc_Ui->pc_LineEditName, &QLineEdit::editingFinished, this,
              &C_SdBueMessagePropertiesWidget::m_OnNameChanged);
   disconnect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
              &C_SdBueMessagePropertiesWidget::m_OnPropertiesChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxCycleTime, static_cast<void (QSpinBox::*)(
                                                                sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnCycleTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxEarly, static_cast<void (QSpinBox::*)(
                                                            sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnEarlyOrLaterTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxLater, static_cast<void (QSpinBox::*)(
                                                            sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnEarlyOrLaterTimeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxDirection, static_cast<void (QComboBox::*)(
                                                                 sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnDirectionChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxTransmitterDatapoolOnly,
              static_cast<void (QComboBox::*)(sintn)>(&C_OgeCbxText::currentIndexChanged),
              this, &C_SdBueMessagePropertiesWidget::m_NodeModeTransmitDatapoolChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current message id

   \return
   Current matching message ids
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> C_SdBueMessagePropertiesWidget::
GetMatchingMessageIds(void) const
{
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> c_Retval;
   if (this->mq_IdIsValid)
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         c_Retval = this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus name
   - check input
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_CheckMessageName(void) const
{
   if (this->mq_IdIsValid)
   {
      const QString c_Name = this->mpc_Ui->pc_LineEditName->text();

      //check
      bool q_NameIsValid = false;
      bool q_InvalidName = false;
      bool q_DuplicateName = false;

      if (this->mpc_MessageSyncManager != NULL)
      {
         this->mpc_MessageSyncManager->CheckMessageNameBus(c_Name.toStdString().c_str(), q_NameIsValid,
                                                           &this->mc_MessageId, &q_InvalidName, &q_DuplicateName);
      }

      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid", q_NameIsValid);
      if (q_NameIsValid == true)
      {
         this->mpc_Ui->pc_LineEditName->SetToolTipInformation("", "", stw_opensyde_gui::C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Heading = C_GtGetText::h_GetText("Message Name");
         QString c_Content;
         if (q_InvalidName == true)
         {
            c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters\n");
         }
         if (q_DuplicateName == true)
         {
            c_Content += C_GtGetText::h_GetText("- is already in use\n");
         }
         this->mpc_Ui->pc_LineEditName->SetToolTipInformation(c_Heading, c_Content,
                                                              stw_opensyde_gui::C_NagToolTip::eERROR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus id
   - check input
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_CheckMessageId(void) const
{
   if (this->mq_IdIsValid)
   {
      const uint32 u32_Id = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxId->value());

      //check
      bool q_NameIsValid = false;
      bool q_EcosRangeError = false;
      bool q_EcosEvenError = false;
      bool q_DuplicateDetected = false;

      if (this->mpc_MessageSyncManager != NULL)
      {
         this->mpc_MessageSyncManager->CheckMessageIdBus(u32_Id, q_NameIsValid, &this->mc_MessageId, &q_EcosRangeError,
                                                         &q_EcosEvenError, &q_DuplicateDetected);
      }

      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxId, "Valid", q_NameIsValid);
      if (q_NameIsValid == true)
      {
         this->mpc_Ui->pc_SpinBoxId->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
      }
      else
      {
         QString c_Content;
         if (q_EcosRangeError == true)
         {
            c_Content += C_GtGetText::h_GetText("Message ID out of range for ECOS\n");
         }
         if (q_EcosEvenError == true)
         {
            c_Content += C_GtGetText::h_GetText("Message ID may not be even for ECOS\n");
         }
         if (q_DuplicateDetected == true)
         {
            c_Content += C_GtGetText::h_GetText("Message ID already in use\n");
         }
         this->mpc_Ui->pc_SpinBoxId->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check delay time
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_CheckEarlyTime(void) const
{
   if (this->mq_IdIsValid)
   {
      bool q_Valid = true;

      const C_OSCCanMessageContainer * const pc_MessageContainer =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(this->mc_MessageId.u32_NodeIndex,
                                                                         this->mc_MessageId.e_ComProtocol,
                                                                         this->mc_MessageId.u32_InterfaceIndex,
                                                                         this->mc_MessageId.u32_DatapoolIndex);

      tgl_assert(pc_MessageContainer != NULL);
      if (pc_MessageContainer != NULL)
      {
         bool q_Invalid;
         pc_MessageContainer->CheckMessageLocalError(NULL, this->mc_MessageId.u32_MessageIndex,
                                                     this->mc_MessageId.q_MessageIsTx, NULL, NULL, &q_Invalid, NULL,
                                                     NULL,
                                                     NULL,
                                                     C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(this->
                                                                                                            mc_MessageId
                                                                                                            .
                                                                                                            e_ComProtocol));
         q_Valid = !q_Invalid;
      }

      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxEarly, "Valid", q_Valid);
      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxLater, "Valid", q_Valid);
      if (q_Valid == true)
      {
         this->mpc_Ui->pc_SpinBoxEarly->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
         this->mpc_Ui->pc_SpinBoxLater->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Heading1 = C_GtGetText::h_GetText("Not earlier than");
         const QString c_Heading2 = C_GtGetText::h_GetText("But not later than");
         const QString c_Content1 =
            static_cast<QString>(C_GtGetText::h_GetText("Value may not be greater than the value for \"%1\"")).arg(
               c_Heading2);
         const QString c_Content2 =
            static_cast<QString>(C_GtGetText::h_GetText("Value may not be smaller than the value for \"%1\"")).arg(
               c_Heading1);
         this->mpc_Ui->pc_SpinBoxEarly->SetToolTipAdditionalInfo(c_Content1, C_NagToolTip::eERROR);
         this->mpc_Ui->pc_SpinBoxLater->SetToolTipAdditionalInfo(c_Content2, C_NagToolTip::eERROR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trimm bus name

   Remove whitespaces at the beginning and end of the string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_TrimmMessageName(void) const
{
   this->mpc_Ui->pc_LineEditName->setText(this->mpc_Ui->pc_LineEditName->text().trimmed());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register Change

   Function where ui elements register a change. Change will be sent via a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_RegisterChange(void)
{
   //signal
   Q_EMIT this->SigChanged();
}
