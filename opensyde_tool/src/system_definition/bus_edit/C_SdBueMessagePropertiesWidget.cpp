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
   this->mpc_Ui->pc_SpinBoxReceiveTimeout->SetMaximumCustom(150010);

   //Remove debug strings
   this->mpc_Ui->pc_GroupBoxBusEdit->setTitle("");
   this->mpc_Ui->pc_GroupBoxNodeEdit->setTitle("");
   this->mpc_Ui->pc_GroupBoxCriticalMessage->setTitle("");
   this->mpc_Ui->pc_GroupBoxTimeout->setTitle("");

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxDlc->SetMaximumCustom(8);
   this->mpc_Ui->pc_SpinBoxLater->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxLater->SetMaximumCustom(50000);
   this->mpc_Ui->pc_SpinBoxEarly->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxEarly->SetMaximumCustom(50000);
   this->mpc_Ui->pc_SpinBoxCycleTime->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxCycleTime->SetMaximumCustom(50000);

   //Initial setup
   m_OnExtendedChange(this->mpc_Ui->pc_CheckBoxExtendedType->isChecked());
   m_OnTxMethodChange(this->mpc_Ui->pc_ComboBoxTxMethod->currentIndex());

   // connects
   connect(this->mpc_Ui->pc_LineEditName, &QLineEdit::textChanged, this,
           &C_SdBueMessagePropertiesWidget::m_CheckMessageName);
   connect(this->mpc_Ui->pc_CheckBoxExtendedType, &C_OgeChxProperties::toggled, this,
           &C_SdBueMessagePropertiesWidget::m_OnExtendedChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTxMethod, static_cast<void (QComboBox::*)(
                                                             sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxMethodChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxDlc, static_cast<void (QSpinBox::*)(sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnDlcChange);

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxLater, static_cast<void (QSpinBox::*)(
                                                         sintn)>(&C_OgeSpxNumber::valueChanged), this,
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
C_SdBueMessagePropertiesWidget::~C_SdBueMessagePropertiesWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because the ownership of these objects was never transfered to this class
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
   this->mpc_Ui->pc_LabelTxMethod->setText(C_GtGetText::h_GetText("TX Method"));
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
   this->mpc_Ui->pc_LabelReceiveTimeout->setText(C_GtGetText::h_GetText("Receive timeout"));
   this->mpc_Ui->pc_LabelTimoutValueDescription->setText(C_GtGetText::h_GetText("Timeout Value"));
   this->mpc_Ui->pc_CheckBoxAutoTimeout->setText(C_GtGetText::h_GetText("Auto (3*Cycle Time+10ms)"));

   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));
   pc_LineEdit = new C_OgeLeComboBox(this->mpc_Ui->pc_ComboBoxTransmitter);
   pc_LineEdit->setPlaceholderText(C_GtGetText::h_GetText("Choose a transmitter"));
   pc_LineEdit->setReadOnly(true);
   // force line edit to be not selectable (QLineEdit has no text interaction flags like QTextEdit or QLabel)
   connect(pc_LineEdit, &QLineEdit::selectionChanged, pc_LineEdit, &QLineEdit::deselect);
   pc_LineEdit->setAccessibleDescription("");
   this->mpc_Ui->pc_ComboBoxTransmitter->setLineEdit(pc_LineEdit);
   this->mpc_Ui->pc_ComboBoxTransmitter->setContextMenuPolicy(Qt::NoContextMenu);

   //Combo boxes
   this->mpc_Ui->pc_ComboBoxDirection->addItem(C_GtGetText::h_GetText("Transmit"));
   this->mpc_Ui->pc_ComboBoxDirection->addItem(C_GtGetText::h_GetText("Receive"));

   this->mpc_Ui->pc_ComboBoxTxMethod->addItem(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_CYCLIC));
   this->mpc_Ui->pc_ComboBoxTxMethod->addItem(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_ON_CHANGE));
   this->mpc_Ui->pc_ComboBoxTxMethod->addItem(C_SdUtil::h_ConvertTxMethodToName(C_OSCCanMessage::eTX_METHOD_ON_EVENT));

   //Other
   this->mpc_Ui->pc_SpinBoxReceiveTimeout->setSuffix(C_GtGetText::h_GetText(" ms"));
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
                                                        "\n - only alphanumeric characters + \"_\""
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
   this->mpc_Ui->pc_LabelTxMethod->SetToolTipInformation(C_GtGetText::h_GetText("TX Method"),
                                                         C_GtGetText::h_GetText("Message transmission method. "
                                                                                "\nCyclic: Message is transmitted "
                                                                                "cyclically."
                                                                                "\nOn change: Message is transmitted if"
                                                                                " any signal value is changed."
                                                                                "\nOn event: Message transmission is "
                                                                                "handled by application (spontaneous)."));
   this->mpc_Ui->pc_LabelEarly->SetToolTipInformation(C_GtGetText::h_GetText("Not earlier than"),
                                                      C_GtGetText::h_GetText("On change method property. "
                                                                             "\nIf a signal changes during this time, the message will not be transmitted."
                                                                             "\nThis allows the bus load to be controlled."));
   this->mpc_Ui->pc_LabelLater->SetToolTipInformation(C_GtGetText::h_GetText("But no later than"),
                                                      C_GtGetText::h_GetText("On change method property. "
                                                                             "\nIf the signal does not change, the message "
                                                                             "will still be sent after this time."));
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

   \param[in] orc_MessageId Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetMessageId(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   this->mc_MessageId = orc_MessageId;

   //load bus data
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load bus information

   Load bus information from core data using bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_LoadFromData(void)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);

   //Disconnects for RegisterChange
   m_DisconnectAllChanges();

   tgl_assert(pc_Message != NULL);
   if (pc_Message != NULL)
   {
      //Name
      this->mpc_Ui->pc_LineEditName->setText(pc_Message->c_Name.c_str());

      //Comment
      this->mpc_Ui->pc_TextEditComment->setText(pc_Message->c_Comment.c_str());

      //Extended
      this->mpc_Ui->pc_CheckBoxExtendedType->setChecked(pc_Message->q_IsExtended);

      //Id
      this->mpc_Ui->pc_SpinBoxId->setValue(pc_Message->u32_CanId);

      //Dlc
      this->mpc_Ui->pc_SpinBoxDlc->setValue(pc_Message->u16_Dlc);

      //Tx method
      this->mpc_Ui->pc_ComboBoxTxMethod->setCurrentIndex(h_TxMethodToIndex(pc_Message->e_TxMethod));
      if (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_EVENT)
      {
         this->mpc_Ui->pc_WidgetReceiver->SetAlwaysHideTimeout(true);
      }
      else
      {
         this->mpc_Ui->pc_WidgetReceiver->SetAlwaysHideTimeout(false);
      }

      //Cycle time
      this->mpc_Ui->pc_SpinBoxCycleTime->setValue(pc_Message->u32_CycleTimeMs);
      this->mpc_Ui->pc_SpinBoxLater->setValue(pc_Message->u32_CycleTimeMs);
      this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(static_cast<sintn>(pc_Message->u32_CycleTimeMs));

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
               //Handle timeout
               this->mpc_Ui->pc_GroupBoxTimeout->setVisible(false);
            }
            else
            {
               this->mpc_Ui->pc_ComboBoxDirection->setCurrentIndex(mu8_DIRECTION_INDEX_RECEIVE);
               //Handle timeout
               this->mpc_Ui->pc_GroupBoxTimeout->setVisible(true);
            }
            //Always fill
            this->mpc_Ui->pc_CheckBoxAutoTimeout->setChecked(pc_UiMessage->q_UseAutoReceiveTimeout);
            if (pc_UiMessage->q_UseAutoReceiveTimeout == true)
            {
               //No problem with read only
               this->mpc_Ui->pc_SpinBoxReceiveTimeout->setEnabled(false);
               this->mpc_Ui->pc_SpinBoxReceiveTimeout->setValue(static_cast<sintn>((static_cast<uint32>(this->mpc_Ui->
                                                                                                        pc_SpinBoxCycleTime
                                                                                                        ->value())
                                                                                    *
                                                                                    3UL) + 10UL));
            }
            else
            {
               //No problem with read only
               this->mpc_Ui->pc_SpinBoxReceiveTimeout->setEnabled(true);
               this->mpc_Ui->pc_SpinBoxReceiveTimeout->setValue(pc_Message->u32_TimeoutMs);
            }
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
            m_HandleCriticalMessagesAndRx();
            m_DisconnectNodeSpecificFields();
         }
      }
      //Initial error check
      m_CheckMessageName();
      m_CheckMessageId();
      m_CheckEarlyTime();
   }

   //connects for RegisterChange
   m_ConnectAllChanges();
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
      this->mpc_Ui->pc_WidgetReceiver->SetAlwaysHideTimeout(false);
      if (this->mq_ModeSingleNode == true)
      {
         if (this->mpc_Ui->pc_ComboBoxDirection->currentIndex() == mu8_DIRECTION_INDEX_RECEIVE)
         {
            this->mpc_Ui->pc_GroupBoxTimeout->setVisible(true);
         }
         else
         {
            this->mpc_Ui->pc_GroupBoxTimeout->setVisible(false);
         }
      }
      break;
   case ms32_TX_TYPE_INDEX_ON_CHANGE:
      this->mpc_Ui->pc_LabelCycleTime->setVisible(false);
      this->mpc_Ui->pc_SpinBoxCycleTime->setVisible(false);
      this->mpc_Ui->pc_LabelEarly->setVisible(true);
      this->mpc_Ui->pc_SpinBoxEarly->setVisible(true);
      this->mpc_Ui->pc_LabelLater->setVisible(true);
      this->mpc_Ui->pc_SpinBoxLater->setVisible(true);
      //Handle timeout section
      this->mpc_Ui->pc_WidgetReceiver->SetAlwaysHideTimeout(false);
      if (this->mq_ModeSingleNode == true)
      {
         if (this->mpc_Ui->pc_ComboBoxDirection->currentIndex() == mu8_DIRECTION_INDEX_RECEIVE)
         {
            this->mpc_Ui->pc_GroupBoxTimeout->setVisible(true);
         }
         else
         {
            this->mpc_Ui->pc_GroupBoxTimeout->setVisible(false);
         }
      }
      break;
   default:
      this->mpc_Ui->pc_LabelCycleTime->setVisible(false);
      this->mpc_Ui->pc_SpinBoxCycleTime->setVisible(false);
      this->mpc_Ui->pc_LabelEarly->setVisible(false);
      this->mpc_Ui->pc_SpinBoxEarly->setVisible(false);
      this->mpc_Ui->pc_LabelLater->setVisible(false);
      this->mpc_Ui->pc_SpinBoxLater->setVisible(false);
      //Handle timeout section
      this->mpc_Ui->pc_WidgetReceiver->SetAlwaysHideTimeout(true);
      if (this->mq_ModeSingleNode == true)
      {
         this->mpc_Ui->pc_GroupBoxTimeout->setVisible(false);
      }
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle dlc change

   \param[in] orsn_NewValue New dlc value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnDlcChange(const sintn & orsn_NewValue) const
{
   if (orsn_NewValue == 1)
   {
      this->mpc_Ui->pc_SpinBoxDlc->setSuffix(C_GtGetText::h_GetText(" Byte"));
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxDlc->setSuffix(C_GtGetText::h_GetText(" Bytes"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform tx method type to index

   \param[in] ore_TxMethod Tx method type

   \return
   Combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueMessagePropertiesWidget::h_TxMethodToIndex(const C_OSCCanMessage::E_TxMethodType & ore_TxMethod)
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
C_OSCCanMessage::E_TxMethodType C_SdBueMessagePropertiesWidget::h_IndexToTxMethod(const sint32 & ors32_Index)
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
   this->m_CheckMessageName();
   m_OnPropertiesChanged();
   m_CheckMessageName();
   Q_EMIT this->SigMessageNameChanged();
   Q_EMIT this->SigRecheckError();
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
   m_OnPropertiesChanged();
   Q_EMIT this->SigMessageDlcChanged(this->mc_MessageId);
   Q_EMIT this->SigRecheckError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle cycle time change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnCycleTimeChanged(void)
{
   m_OnPropertiesChanged();
   this->mpc_Ui->pc_WidgetReceiver->SetLastKnownCycleTimeValue(static_cast<uint32>(this->mpc_Ui->pc_SpinBoxCycleTime->
                                                                                   value()));
   if (this->mq_ModeSingleNode == true)
   {
      this->mpc_Ui->pc_SpinBoxReceiveTimeout->SetMinimumCustom(this->mpc_Ui->pc_SpinBoxCycleTime->value());
      if (this->mpc_Ui->pc_CheckBoxAutoTimeout->isChecked() == true)
      {
         this->mpc_Ui->pc_SpinBoxReceiveTimeout->setValue((this->mpc_Ui->pc_SpinBoxCycleTime->value() * 3) + 10);
      }
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
   if (this->mpc_MessageSyncManager != NULL)
   {
      const C_OSCCanMessage * const pc_CanMessage = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);

      m_RegisterChange();

      if (pc_CanMessage != NULL)
      {
         C_OSCCanMessageIdentificationIndices c_NewMessageId;
         //save data
         //copy current message
         C_OSCCanMessage c_MessageData = *pc_CanMessage;

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
         c_MessageData.e_TxMethod = h_IndexToTxMethod(this->mpc_Ui->pc_ComboBoxTxMethod->currentIndex());

         //Cycle time
         if (c_MessageData.e_TxMethod == C_OSCCanMessage::eTX_METHOD_CYCLIC)
         {
            c_MessageData.u32_CycleTimeMs =
               static_cast<uint32>(this->mpc_Ui->pc_SpinBoxCycleTime->value());
         }
         else
         {
            c_MessageData.u32_CycleTimeMs = static_cast<uint32>(this->mpc_Ui->pc_SpinBoxLater->value());
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
            Q_EMIT this->SigMessageIdChanged(this->mc_MessageId);
         }
         m_HandleCriticalMessagesAndRx();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register message direction change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnDirectionChanged(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      const C_OSCCanMessageContainer * pc_MessageContainer;
      m_RegisterChange();
      const C_OSCCanMessageIdentificationIndices c_PrevMessageId = this->mc_MessageId;

      if (this->mpc_Ui->pc_ComboBoxDirection->currentIndex() == mu8_DIRECTION_INDEX_TRANSMIT)
      {
         this->mc_MessageId.q_MessageIsTx = true;
         //Handle timeout
         this->mpc_Ui->pc_GroupBoxTimeout->setVisible(false);
      }
      else
      {
         this->mc_MessageId.q_MessageIsTx = false;
         //Handle timeout
         this->mpc_Ui->pc_GroupBoxTimeout->setVisible(true);
      }

      //save new message data
      tgl_assert(this->mpc_MessageSyncManager->SetCanMessageDirection(c_PrevMessageId,
                                                                      this->mc_MessageId.q_MessageIsTx) == C_NO_ERR);

      //Update message index
      pc_MessageContainer = C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
         this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex);
      if (pc_MessageContainer != NULL)
      {
         const std::vector<C_OSCCanMessage> & rc_Messages = pc_MessageContainer->GetMessagesConst(
            this->mc_MessageId.q_MessageIsTx);
         tgl_assert(rc_Messages.size() > 0);
         if (rc_Messages.size() > 0)
         {
            this->mc_MessageId.u32_MessageIndex = rc_Messages.size() - 1UL;
         }
      }

      Q_EMIT this->SigMessageIdChanged(this->mc_MessageId);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle transmitter change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnTxChanged(void)
{
   if (this->mc_InterfaceIndexes.size() == this->mc_NodeIndexes.size())
   {
      if (this->mpc_MessageSyncManager != NULL)
      {
         const sint32 s32_CurrentIndex = this->mpc_Ui->pc_ComboBoxTransmitter->currentIndex();
         m_RegisterChange();
         if (s32_CurrentIndex >= 0)
         {
            const uint32 u32_CurrentIndex = static_cast<uint32>(s32_CurrentIndex);
            if (u32_CurrentIndex < this->mc_NodeIndexes.size())
            {
               if (this->mpc_MessageSyncManager->ChangeCanMessageTx(this->mc_MessageId,
                                                                    this->mc_NodeIndexes[u32_CurrentIndex],
                                                                    this->mc_InterfaceIndexes[u32_CurrentIndex]) ==
                   C_NO_ERR)
               {
                  //Update message id if necessary
                  if ((this->mc_MessageId.q_MessageIsTx == true) ||
                      (((this->mc_MessageId.u32_NodeIndex == this->mc_NodeIndexes[u32_CurrentIndex]) &&
                        (this->mc_MessageId.u32_InterfaceIndex == this->mc_InterfaceIndexes[u32_CurrentIndex])) &&
                       (this->mc_MessageId.e_ComProtocol == this->me_ComProtocol)))
                  {
                     const C_OSCCanMessageContainer * const pc_MessageContainer =
                        C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                           this->mc_NodeIndexes[u32_CurrentIndex], this->me_ComProtocol,
                           this->mc_InterfaceIndexes[u32_CurrentIndex]);
                     if (pc_MessageContainer != NULL)
                     {
                        //Update message id
                        this->mc_MessageId.u32_NodeIndex = this->mc_NodeIndexes[u32_CurrentIndex];
                        this->mc_MessageId.e_ComProtocol = this->me_ComProtocol;
                        this->mc_MessageId.u32_InterfaceIndex = this->mc_InterfaceIndexes[u32_CurrentIndex];
                        this->mc_MessageId.q_MessageIsTx = true;
                        this->mc_MessageId.u32_MessageIndex = pc_MessageContainer->c_TxMessages.size() - 1UL;

                        Q_EMIT this->SigMessageIdChanged(this->mc_MessageId);
                     }
                  }

                  this->mpc_Ui->pc_WidgetReceiver->CheckSpecificNode(this->mc_NodeIndexes[u32_CurrentIndex],
                                                                     this->mc_InterfaceIndexes[u32_CurrentIndex],
                                                                     false);

                  // update the Rx widget
                  this->m_UpdateRxAfterTxSelection(this->mpc_MessageSyncManager->GetMatchingMessageVector(this->
                                                                                                          mc_MessageId));
                  Q_EMIT this->SigRecheckError();
                  C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxTransmitter, "Valid", true);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle receiver changed

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface index (ID)
   \param[in] oq_Checked          Flag if checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnRxChanged(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                   const bool oq_Checked)
{
   if (this->mq_InternalRxChange == false)
   {
      m_RegisterChange();
      if (this->mpc_MessageSyncManager != NULL)
      {
         if (oq_Checked == true)
         {
            const uint32 u32_DefaultReceiveTimeoutValue =
               (static_cast<uint32>(this->mpc_Ui->pc_SpinBoxCycleTime->value()) * 3UL) + 10UL;
            tgl_assert(this->mpc_MessageSyncManager->AddCanMessageRx(this->mc_MessageId, ou32_NodeIndex,
                                                                     ou32_InterfaceIndex) == C_NO_ERR);
            //Also add default receive timeout value (important for core values)
            tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeout(this->mc_MessageId, ou32_NodeIndex,
                                                                                 ou32_InterfaceIndex,
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
            if (((this->mc_MessageId.q_MessageIsTx == false) && (this->mc_MessageId.u32_NodeIndex == ou32_NodeIndex)) &&
                (this->mc_MessageId.u32_InterfaceIndex == ou32_InterfaceIndex))
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
                  }
               }
            }
            if (q_Found == true)
            {
               this->mpc_MessageSyncManager->DeleteCanMessageRx(this->mc_MessageId, ou32_NodeIndex,
                                                                ou32_InterfaceIndex);
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
               C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR, C_GtGetText::h_GetText(
                                                    "A message can not exist without a receiver and transmitter."));

               c_TmpNodeIndexes.push_back(ou32_NodeIndex);
               c_TmpInterfaceIndexes.push_back(ou32_InterfaceIndex);

               //Inform user
               c_MessageBox.Execute();

               //Revert change
               this->mq_InternalRxChange = true;
               this->mpc_Ui->pc_WidgetReceiver->CheckNodes(c_TmpNodeIndexes, c_TmpInterfaceIndexes);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle auto receive timeout flag changed (bus mode)

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface index (ID)
   \param[in] oq_UseAuto          Flag if automatic receive timeout
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnRxTimeoutFlagChanged(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_InterfaceIndex, const bool oq_UseAuto)
{
   m_RegisterChange();
   tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeoutAutoFlag(this->mc_MessageId, ou32_NodeIndex,
                                                                                ou32_InterfaceIndex,
                                                                                oq_UseAuto) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle receive timeout value changed (bus mode)

   \param[in] ou32_NodeIndex      Node index (ID)
   \param[in] ou32_InterfaceIndex Interface index (ID)
   \param[in] ou32_TimeoutValue   Receive timeout value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnRxTimeoutValueChanged(const uint32 ou32_NodeIndex,
                                                               const uint32 ou32_InterfaceIndex,
                                                               const uint32 ou32_TimeoutValue)
{
   m_RegisterChange();
   tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeout(this->mc_MessageId, ou32_NodeIndex,
                                                                        ou32_InterfaceIndex,
                                                                        ou32_TimeoutValue) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle auto receive timeout flag changed (node mode)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnSingleTimeoutFlagChange(void)
{
   const bool oq_AutoTimeout = this->mpc_Ui->pc_CheckBoxAutoTimeout->isChecked();

   m_RegisterChange();
   tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeoutAutoFlag(this->mc_MessageId,
                                                                                this->mc_MessageId.u32_NodeIndex,
                                                                                this->mc_MessageId.u32_InterfaceIndex,
                                                                                oq_AutoTimeout) == C_NO_ERR);
   if (oq_AutoTimeout == true)
   {
      //No problem with read only
      this->mpc_Ui->pc_SpinBoxReceiveTimeout->setEnabled(false);
      this->mpc_Ui->pc_SpinBoxReceiveTimeout->setValue(static_cast<sintn>((static_cast<uint32>(this->mpc_Ui->
                                                                                               pc_SpinBoxCycleTime->
                                                                                               value())
                                                                           *
                                                                           3UL) + 10UL));
   }
   else
   {
      //No problem with read only
      this->mpc_Ui->pc_SpinBoxReceiveTimeout->setEnabled(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle receive timeout value changed (node mode)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_OnSingleTimeoutValueChange(void)
{
   m_RegisterChange();
   tgl_assert(this->mpc_MessageSyncManager->SetCanMessageReceiveTimeout(this->mc_MessageId,
                                                                        this->mc_MessageId.u32_NodeIndex,
                                                                        this->mc_MessageId.u32_InterfaceIndex,
                                                                        this->mpc_Ui->pc_SpinBoxReceiveTimeout->value()) ==
              C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect update signals for fields which are affected by protocol changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_ConnectProtocolSpecificFields(void)
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
void C_SdBueMessagePropertiesWidget::m_DisconnectProtocolSpecificFields(void)
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
void C_SdBueMessagePropertiesWidget::m_ConnectNodeSpecificFields(void)
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTransmitter, static_cast<void (QComboBox::*)(
                                                                sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnTxChanged);
   connect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeToggled, this,
           &C_SdBueMessagePropertiesWidget::m_OnRxChanged);
   connect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeUseReceiveTimeout, this,
           &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutFlagChanged);
   connect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeReceiveTimeout, this,
           &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect update signals for fields which are affected by node changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_DisconnectNodeSpecificFields(void)
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxTransmitter, static_cast<void (QComboBox::*)(
                                                                   sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnTxChanged);
   disconnect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeToggled, this,
              &C_SdBueMessagePropertiesWidget::m_OnRxChanged);
   disconnect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeUseReceiveTimeout, this,
              &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutFlagChanged);
   disconnect(this->mpc_Ui->pc_WidgetReceiver, &C_SdBueMessageRxList::SigNodeReceiveTimeout, this,
              &C_SdBueMessagePropertiesWidget::m_OnRxTimeoutValueChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reconnect update signals for fields which signal changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_ConnectAllChanges(void)
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
   connect(this->mpc_Ui->pc_CheckBoxAutoTimeout, &C_OgeChxProperties::toggled, this,
           &C_SdBueMessagePropertiesWidget::m_OnSingleTimeoutFlagChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxReceiveTimeout, static_cast<void (QSpinBox::*)(
                                                                  sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueMessagePropertiesWidget::m_OnSingleTimeoutValueChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect update signals for fields which signal changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_DisconnectAllChanges(void)
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
   disconnect(this->mpc_Ui->pc_CheckBoxAutoTimeout, &C_OgeChxProperties::toggled, this,
              &C_SdBueMessagePropertiesWidget::m_OnSingleTimeoutFlagChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxReceiveTimeout, static_cast<void (QSpinBox::*)(
                                                                     sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueMessagePropertiesWidget::m_OnSingleTimeoutValueChange);
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
                                                                          this->mc_NodeIndexes,
                                                                          this->mc_InterfaceIndexes);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_UpdateTxSelection(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds) const
{
   bool q_Transmitter = false;

   std::vector<QString> c_NodeNames;

   // fill combo box
   this->mpc_Ui->pc_ComboBoxTransmitter->clear();
   if (C_SdUtil::h_GetNames(this->mc_NodeIndexes, this->mc_InterfaceIndexes, c_NodeNames) == C_NO_ERR)
   {
      for (uint32 u32_ItName = 0; u32_ItName < c_NodeNames.size(); ++u32_ItName)
      {
         this->mpc_Ui->pc_ComboBoxTransmitter->addItem(c_NodeNames[u32_ItName]);
      }
   }

   // get the actual selection
   tgl_assert((this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size()) &&
              (this->mc_InterfaceIndexes.size() == c_NodeNames.size()));
   if ((this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size()) &&
       (this->mc_InterfaceIndexes.size() == c_NodeNames.size()))
   {
      for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MatchingMessageIds.size(); ++u32_ItMessage)
      {
         const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId = orc_MatchingMessageIds[u32_ItMessage];
         if (rc_CurrentMessageId.q_MessageIsTx == true)
         {
            for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_NodeIndexes.size(); ++u32_ItNode)
            {
               if ((this->mc_NodeIndexes[u32_ItNode] == rc_CurrentMessageId.u32_NodeIndex) &&
                   (this->mc_InterfaceIndexes[u32_ItNode] == rc_CurrentMessageId.u32_InterfaceIndex))
               {
                  this->mpc_Ui->pc_ComboBoxTransmitter->setCurrentIndex(static_cast<sintn>(u32_ItNode));
                  q_Transmitter = true;
               }
            }
         }
      }
   }
   if (q_Transmitter == false)
   {
      const QString c_Content = C_GtGetText::h_GetText("No transmitter selected.");
      //Set invalid
      this->mpc_Ui->pc_ComboBoxTransmitter->setCurrentIndex(-1);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxTransmitter, "Valid", false);
      this->mpc_Ui->pc_ComboBoxTransmitter->SetToolTipInformation("", c_Content, C_NagToolTip::eERROR);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxTransmitter->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxTransmitter, "Valid", true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_UpdateRxAfterTxSelection(
   const std::vector<C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds, const bool oq_SkipDisconnect)
{
   std::vector<uint32> c_NodeIndexes;
   std::vector<uint32> c_InterfaceIndexes;
   std::vector<QString> c_NodeNames;
   bool q_TxSelected = false;

   if (oq_SkipDisconnect == false)
   {
      m_DisconnectNodeSpecificFields();
   }

   // get all names
   if (C_SdUtil::h_GetNames(this->mc_NodeIndexes, this->mc_InterfaceIndexes, c_NodeNames) == C_NO_ERR)
   {
      std::vector<bool> c_UseAutoReceiveTimeoutFlags;
      std::vector<uint32> c_ReceiveTimeoutValues;
      //Fill those flags
      tgl_assert(this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size());
      if (this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size())
      {
         c_ReceiveTimeoutValues.reserve(this->mc_NodeIndexes.size());
         c_UseAutoReceiveTimeoutFlags.reserve(this->mc_NodeIndexes.size());
         for (uint32 u32_ItNode = 0; u32_ItNode < this->mc_NodeIndexes.size(); ++u32_ItNode)
         {
            bool q_Found = false;
            for (uint32 u32_ItMessageId = 0; u32_ItMessageId < orc_MatchingMessageIds.size(); ++u32_ItMessageId)
            {
               const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId =
                  orc_MatchingMessageIds[u32_ItMessageId];
               if ((rc_CurrentMessageId.u32_NodeIndex == this->mc_NodeIndexes[u32_ItNode]) &&
                   (rc_CurrentMessageId.u32_InterfaceIndex == this->mc_InterfaceIndexes[u32_ItNode]))
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
                     c_UseAutoReceiveTimeoutFlags.push_back(pc_UiData->q_UseAutoReceiveTimeout);
                     q_Found = true;
                  }
                  break;
               }
            }
            if (q_Found == false)
            {
               //Add default
               c_UseAutoReceiveTimeoutFlags.push_back(true);
               c_ReceiveTimeoutValues.push_back(20);
            }
         }
      }
      // is one node selected as Tx?
      if (this->mpc_Ui->pc_ComboBoxTransmitter->currentIndex() >= 0)
      {
         const uint32 u32_CurrentIndex = static_cast<uint32>(this->mpc_Ui->pc_ComboBoxTransmitter->currentIndex());
         std::vector<QString>::iterator c_ItName;
         std::vector<QString>::iterator c_ItNameToDelete = c_NodeNames.end();
         std::vector<uint32> c_RxNodeIndexes;
         std::vector<uint32> c_RxInterfaceIndexes;
         std::vector<bool> c_RxUseAutoReceiveTimeoutFlags;
         std::vector<uint32> c_RxReceiveTimeoutValues;
         uint32 u32_Counter = 0U;

         // remove this node from the vector
         tgl_assert(this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size());
         tgl_assert(c_NodeNames.size() == this->mc_InterfaceIndexes.size());
         tgl_assert(
            static_cast<uint32>(this->mpc_Ui->pc_ComboBoxTransmitter->count()) == this->mc_InterfaceIndexes.size());
         for (c_ItName = c_NodeNames.begin(); c_ItName != c_NodeNames.end(); ++c_ItName)
         {
            if (u32_Counter == u32_CurrentIndex)
            {
               c_ItNameToDelete = c_ItName;
            }
            else
            {
               // synchronize the other necessary vectors
               c_RxNodeIndexes.push_back(this->mc_NodeIndexes[u32_Counter]);
               c_RxInterfaceIndexes.push_back(this->mc_InterfaceIndexes[u32_Counter]);
               c_RxUseAutoReceiveTimeoutFlags.push_back(c_UseAutoReceiveTimeoutFlags[u32_Counter]);
               c_RxReceiveTimeoutValues.push_back(c_ReceiveTimeoutValues[u32_Counter]);
            }
            ++u32_Counter;
         }

         if (c_ItNameToDelete != c_NodeNames.end())
         {
            c_NodeNames.erase(c_ItNameToDelete);
         }

         this->mpc_Ui->pc_WidgetReceiver->AddNodes(c_NodeNames, c_RxNodeIndexes, c_RxInterfaceIndexes,
                                                   c_RxUseAutoReceiveTimeoutFlags, c_RxReceiveTimeoutValues);

         q_TxSelected = true;
      }
      else
      {
         // no restrictions by Tx
         this->mpc_Ui->pc_WidgetReceiver->AddNodes(c_NodeNames, this->mc_NodeIndexes, this->mc_InterfaceIndexes,
                                                   c_UseAutoReceiveTimeoutFlags, c_ReceiveTimeoutValues);
      }
   }

   // prepare the vectors
   if (orc_MatchingMessageIds.size() > 1)
   {
      uint32 u32_Size;
      if (q_TxSelected == true)
      {
         //Typical size (1 tx, X rx)
         u32_Size = orc_MatchingMessageIds.size() - 1UL;
      }
      else
      {
         u32_Size = orc_MatchingMessageIds.size();
      }

      c_NodeIndexes.reserve(u32_Size);
      c_InterfaceIndexes.reserve(u32_Size);
   }

   // update the selection
   for (uint32 u32_ItMessage = 0; u32_ItMessage < orc_MatchingMessageIds.size(); ++u32_ItMessage)
   {
      const C_OSCCanMessageIdentificationIndices & rc_CurrentMessageId = orc_MatchingMessageIds[u32_ItMessage];
      if (rc_CurrentMessageId.q_MessageIsTx == false)
      {
         c_NodeIndexes.push_back(rc_CurrentMessageId.u32_NodeIndex);
         c_InterfaceIndexes.push_back(rc_CurrentMessageId.u32_InterfaceIndex);
      }
   }
   this->mpc_Ui->pc_WidgetReceiver->CheckNodes(c_NodeIndexes, c_InterfaceIndexes);
   if (oq_SkipDisconnect == false)
   {
      m_ConnectNodeSpecificFields();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle critical message state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_HandleCriticalMessagesAndRx(const bool oq_HandleSignals)
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
         //Select receiver
         this->m_UpdateRxAfterTxSelection(c_MatchingMessageIds, true);
         this->mpc_Ui->pc_WidgetReceiver->setVisible(true);
         if (oq_HandleSignals == true)
         {
            m_ConnectNodeSpecificFields();
         }
      }
   }
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
/*! \brief   Sets the node 'mode' of the widget with all necessary indexes

   \param[in] ou32_NodeIndex      Node index
   \param[in] ou32_InterfaceIndex Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::SetNodeDataPool(const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_InterfaceIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_InterfaceIndex = ou32_InterfaceIndex;
   this->mq_ModeSingleNode = true;
   this->mpc_Ui->pc_GroupBoxBusEdit->setVisible(false);
   this->mpc_Ui->pc_GroupBoxNodeEdit->setVisible(true);
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
   this->mpc_Ui->pc_GroupBoxBusEdit->setVisible(true);
   this->mpc_Ui->pc_GroupBoxNodeEdit->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::OnConnectionChange(void)
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
         m_HandleCriticalMessagesAndRx(false);

         m_ConnectNodeSpecificFields();
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
   m_DisconnectAllChanges();
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
   m_ConnectAllChanges();
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
/*! \brief   Get current message id

   \return
   Current matching message ids
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> C_SdBueMessagePropertiesWidget::
GetMatchingMessageIds(void) const
{
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> c_Retval;
   if (this->mpc_MessageSyncManager != NULL)
   {
      c_Retval = this->mpc_MessageSyncManager->GetMatchingMessageVector(this->mc_MessageId);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check bus id
   - check input
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_CheckMessageId(void) const
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check delay time
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMessagePropertiesWidget::m_CheckEarlyTime(void) const
{
   bool q_Valid = true;

   const C_OSCCanMessageContainer * const pc_MessageContainer =
      C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(this->mc_MessageId.u32_NodeIndex,
                                                                      this->mc_MessageId.e_ComProtocol,
                                                                      this->mc_MessageId.u32_InterfaceIndex);

   tgl_assert(pc_MessageContainer != NULL);
   if (pc_MessageContainer != NULL)
   {
      bool q_Invalid;
      pc_MessageContainer->CheckMessageLocalError(NULL, this->mc_MessageId.u32_MessageIndex,
                                                  this->mc_MessageId.q_MessageIsTx, NULL, NULL, &q_Invalid, NULL, NULL,
                                                  NULL,
                                                  C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(this->
                                                                                                         mc_MessageId.
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
         QString(C_GtGetText::h_GetText("Value may not be greater than the value for \"%1\"")).arg(
            c_Heading2);
      const QString c_Content2 =
         QString(C_GtGetText::h_GetText("Value may not be smaller than the value for \"%1\"")).arg(
            c_Heading1);
      this->mpc_Ui->pc_SpinBoxEarly->SetToolTipAdditionalInfo(c_Content1, C_NagToolTip::eERROR);
      this->mpc_Ui->pc_SpinBoxLater->SetToolTipAdditionalInfo(c_Content2, C_NagToolTip::eERROR);
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
