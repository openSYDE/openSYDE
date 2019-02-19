//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing signal properties

   Widget for editing signal properties

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

#include <limits>
#include <QSpinBox>

#include "TGLUtils.h"
#include "C_SdUtil.h"
#include "constants.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_PuiSdHandler.h"
#include "C_SdTooltipUtil.h"
#include "C_OgeSpxInt64Table.h"
#include "C_SdNdeDataPoolContentUtil.h"
#include "C_SdBueSignalPropertiesWidget.h"
#include "ui_C_SdBueSignalPropertiesWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const sint32 ms32_TYPE_INDEX_UNSIGNED = 0;
const sint32 ms32_TYPE_INDEX_SIGNED = 1;
const sint32 ms32_TYPE_INDEX_FLOATING_32 = 2;
const sint32 ms32_TYPE_INDEX_FLOATING_64 = 3;

const sint32 ms32_BYTE_ORDER_INDEX_INTEL = 0;
const sint32 ms32_BYTE_ORDER_INDEX_MOTOROLA = 1;

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
C_SdBueSignalPropertiesWidget::C_SdBueSignalPropertiesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueSignalPropertiesWidget),
   mpc_MessageSyncManager(NULL),
   mq_DataChangesAllowed(false),
   mq_PositionUpdate(true)
{
   // init UI
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   //Ui restriction
   this->mpc_Ui->pc_SpinBoxLength->SetMinimumCustom(1);
   this->mpc_Ui->pc_SpinBoxLength->SetMaximumCustom(64);
   this->mpc_Ui->pc_SpinBoxStartBit->SetMaximumCustom(63);

   //Offset
   //lint -e{10,530,747,1015,1013,1960}  c++11 feature
   this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMinimumCustom(std::numeric_limits<float64>::lowest());
   this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMaximumCustom(std::numeric_limits<float64>::max());
   //Factor needs to be above zero
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_DoubleSpinBoxFactor->SetMinimumCustom(C_OgeSpxFactor::mhf64_FACTOR_MIN);
   this->mpc_Ui->pc_DoubleSpinBoxFactor->SetMaximumCustom(std::numeric_limits<float64>::max());

   //AutoMinMax
   m_HandleValueType(this->mpc_Ui->pc_ComboBoxType->currentIndex());
   m_UpdateAutoMinMax();

   //Ui restriction
   this->mpc_Ui->pc_LineEditName->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);

   // connects
   connect(this->mpc_Ui->pc_CheckBoxAutoMinMax, &C_OgeChxProperties::toggled, this,
           &C_SdBueSignalPropertiesWidget::m_HandleAutoMinMaxCheckBox);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                         sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleValueType);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxLength, static_cast<void (QSpinBox::*)(
                                                          sintn)>(&QSpinBox::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleValueLengthChange);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     24.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueSignalPropertiesWidget::~C_SdBueSignalPropertiesWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the mpc_MessageSyncManager because of handling it on an other position
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::InitStaticNames(void) const
{
   QString c_PhysicalValueInfo;
   QString c_InfoText;

   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelValue->setText(C_GtGetText::h_GetText("Value"));
   this->mpc_Ui->pc_LabelMin->setText(C_GtGetText::h_GetText("Min"));
   this->mpc_Ui->pc_LabelMax->setText(C_GtGetText::h_GetText("Max"));
   this->mpc_Ui->pc_LabelFactor->setText(C_GtGetText::h_GetText("Factor"));
   this->mpc_Ui->pc_LabelOffset->setText(C_GtGetText::h_GetText("Offset"));
   this->mpc_Ui->pc_LabelInitValue->setText(C_GtGetText::h_GetText("Init Value"));
   this->mpc_Ui->pc_LabelUnit->setText(C_GtGetText::h_GetText("Unit"));
   this->mpc_Ui->pc_LabelValueType->setText(C_GtGetText::h_GetText("Value Type"));
   this->mpc_Ui->pc_LabelLayout->setText(C_GtGetText::h_GetText("Layout"));
   this->mpc_Ui->pc_LabelType->setText(C_GtGetText::h_GetText("Type"));
   this->mpc_Ui->pc_LabelLength->setText(C_GtGetText::h_GetText("Length"));
   this->mpc_Ui->pc_LabelByteOrder->setText(C_GtGetText::h_GetText("Byte Order"));
   this->mpc_Ui->pc_LabelMessage->setText(C_GtGetText::h_GetText("Message"));
   this->mpc_Ui->pc_LabelStartBit->setText(C_GtGetText::h_GetText("Start Bit"));
   this->mpc_Ui->pc_CheckBoxAutoMinMax->setText(C_GtGetText::h_GetText("Auto min/max"));

   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));

   //Combo boxes
   this->mpc_Ui->pc_ComboBoxType->addItem(C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                         eUINT8));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                         eSINT8));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                         eFLOAT32));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                         eFLOAT64));

   this->mpc_Ui->pc_ComboBoxByteOrder->addItem(C_SdUtil::h_ConvertByteOrderToName(C_OSCCanSignal::eBYTE_ORDER_INTEL));
   this->mpc_Ui->pc_ComboBoxByteOrder->addItem(C_SdUtil::h_ConvertByteOrderToName(C_OSCCanSignal::eBYTE_ORDER_MOTOROLA));

   //Other
   this->mpc_Ui->pc_SpinBoxLength->setSuffix(C_GtGetText::h_GetText("bit"));

   //Tool tips
   //Tool tip content partly adapted from Vector CANdb++ Documentation
   c_PhysicalValueInfo = C_GtGetText::h_GetText("The raw value of a signal is the value as it is transmitted in the network."
                                                "\nThe physical value of a signal is the value of the physical quantity (e.g. speed, "
                                                "\nrpm, temperature, etc.) that represents the signal."
                                                "\nThe following conversion formula is used to transform the raw value "
                                                "\nto a physical value or in the reverse direction:"
                                                "\n\n[Physical value] = ([Raw value] * [Factor]) + [Offset]");

   //name
   c_InfoText =  C_GtGetText::h_GetText("Symbolic signal name. Unique within a message."
                                        "\nC naming conventions must be followed:"
                                        "\n - must not be empty"
                                        "\n - only alphanumeric characters + \"_\""
                                        "\n - should not be longer than 31 characters");
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"), c_InfoText);

   //comment
   c_InfoText =  C_GtGetText::h_GetText("Comment for this signal.");
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"), c_InfoText);

   //auto min max
   c_InfoText =  C_GtGetText::h_GetText(
      "Minimum and maximum are defined automatically depending on signal length.\nValues are set as physical values.\n\n");
   this->mpc_Ui->pc_CheckBoxAutoMinMax->SetToolTipInformation(C_GtGetText::h_GetText("Auto Min/Max"),
                                                              c_InfoText.append(c_PhysicalValueInfo));

   //min
   c_InfoText =  C_GtGetText::h_GetText("Minimum is set as physical value.\n\n");
   this->mpc_Ui->pc_LabelMin->SetToolTipInformation(C_GtGetText::h_GetText("Min"), c_InfoText.append(
                                                       c_PhysicalValueInfo));

   //max
   c_InfoText =  C_GtGetText::h_GetText("Maximum is set as physical value.\n\n");
   this->mpc_Ui->pc_LabelMax->SetToolTipInformation(C_GtGetText::h_GetText("Max"), c_InfoText.append(
                                                       c_PhysicalValueInfo));

   //init
   c_InfoText =  C_GtGetText::h_GetText("Initial value is set as physical value.\n\n");
   this->mpc_Ui->pc_LabelInitValue->SetToolTipInformation(C_GtGetText::h_GetText("Init Value"), c_InfoText.append(
                                                             c_PhysicalValueInfo));

   //unit
   c_InfoText =  C_GtGetText::h_GetText("Unit of the signals physical quantity\n\n");
   this->mpc_Ui->pc_LabelUnit->SetToolTipInformation(C_GtGetText::h_GetText("Unit"), c_InfoText.append(
                                                        c_PhysicalValueInfo));

   //factor + offset
   this->mpc_Ui->pc_LabelFactor->SetToolTipInformation(C_GtGetText::h_GetText("Factor"), c_PhysicalValueInfo);
   this->mpc_Ui->pc_LabelOffset->SetToolTipInformation(C_GtGetText::h_GetText("Offset"), c_PhysicalValueInfo);

   //byte order
   c_InfoText =  C_GtGetText::h_GetText("Signals can be defined with a byte order for either Motorola or Intel processors. "
                                        "\nThe individual signals within a message may also have different byte orders.");
   this->mpc_Ui->pc_LabelByteOrder->SetToolTipInformation(C_GtGetText::h_GetText("Byte Order"), c_InfoText);

   //value type
   c_InfoText =  C_GtGetText::h_GetText("Value type"
                                        "\nSigned: Signed Integer"
                                        "\nUnsigned: Unsigned Integer"
                                        "\nIEEE Float: 32 Bit IEEE Float"
                                        "\nIEEE Double: 64 Bit IEEE Double");
   this->mpc_Ui->pc_LabelValueType->SetToolTipInformation(C_GtGetText::h_GetText("Type"), c_InfoText);

   //Length
   c_InfoText =  C_GtGetText::h_GetText("Signal length in bits.");
   this->mpc_Ui->pc_LabelLength->SetToolTipInformation(C_GtGetText::h_GetText("Length"), c_InfoText);

   //Start Bit
   c_InfoText =  C_GtGetText::h_GetText("Start bit of the signal inside the message payload.");
   this->mpc_Ui->pc_LabelStartBit->SetToolTipInformation(C_GtGetText::h_GetText("Start Bit"), c_InfoText);

   //Message
   c_InfoText =  C_GtGetText::h_GetText("Name of message which contains this signal.");
   this->mpc_Ui->pc_LabelMessage->SetToolTipInformation(C_GtGetText::h_GetText("Message"), c_InfoText);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set message sync manager

   \param[in,out] opc_Value Message sync manager

   \created     25.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_MessageSyncManager = opc_Value;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal ID setter

   Sets the private signal id of widget

   \param[in] orc_MessageId    Message identification indices
   \param[in] ou32_SignalIndex New signal id

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SetSignalId(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                const uint32 ou32_SignalIndex)
{
   this->mc_MessageId = orc_MessageId;
   this->mu32_SignalIndex = ou32_SignalIndex;

   //load bus data
   this->m_LoadFromData();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On change of signal position in message

   \param[in] orq_SignalErrorChange Optional flag to suppress error signal

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::ReloadPosition(const bool & orq_SignalErrorChange)
{
   const C_OSCCanSignal * const pc_OSCSignal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(this->mc_MessageId,
                                                                                             this->mu32_SignalIndex);

   if (pc_OSCSignal != NULL)
   {
      m_DisconnectPositionUpdate();

      //Start bit
      this->mpc_Ui->pc_SpinBoxStartBit->setValue(pc_OSCSignal->u16_ComBitStart);
      //Length
      this->mpc_Ui->pc_SpinBoxLength->setValue(pc_OSCSignal->u16_ComBitLength);

      m_ConnectPositionUpdate();
      //Force another save to ensure min max adaptations (after reconnect!)
      m_RegisterChange();
      m_CheckMessagePosition(orq_SignalErrorChange);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current message id

   \return
   Current message id

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_opensyde_core::C_OSCCanMessageIdentificationIndices C_SdBueSignalPropertiesWidget::GetMessageId(void) const
{
   return mc_MessageId;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current signal index

   \return
   Current signal index

   \created     27.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SdBueSignalPropertiesWidget::GetSignalIndex(void) const
{
   return this->mu32_SignalIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set protocol specific settings

   \param[in] ore_Type Current protcol

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SetProtocol(const C_OSCCanProtocol::E_Type & ore_Type)
{
   if (ore_Type == C_OSCCanProtocol::eECES)
   {
      m_DisconnectPositionUpdate();
      this->mpc_Ui->pc_SpinBoxLength->SetMinimumCustom(1);
      this->mpc_Ui->pc_SpinBoxLength->SetMaximumCustom(64 - 16);
      this->mpc_Ui->pc_SpinBoxStartBit->SetMinimumCustom(0);
      this->mpc_Ui->pc_SpinBoxStartBit->SetMaximumCustom((64 - 16) - 1);
      m_ConnectPositionUpdate();
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxLength->SetMinimumCustom(1);
      this->mpc_Ui->pc_SpinBoxLength->SetMaximumCustom(64);
      this->mpc_Ui->pc_SpinBoxStartBit->SetMinimumCustom(0);
      this->mpc_Ui->pc_SpinBoxStartBit->SetMaximumCustom(64 - 1);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Selects the node name in the text edit for fast editing

   \created     02.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SelectName(void) const
{
   this->mpc_Ui->pc_LineEditName->setFocus();
   this->mpc_Ui->pc_LineEditName->selectAll();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load bus information

   Load bus information from core data using bus index

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_LoadFromData(void)
{
   const C_OSCCanMessage * const pc_OSCMessage = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
      C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(this->mc_MessageId, this->mu32_SignalIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UISignalCommon =
      C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(this->mc_MessageId, this->mu32_SignalIndex);

   this->mq_DataChangesAllowed = false;
   //disconnects for UnRegisterChange
   disconnect(this->mpc_Ui->pc_LineEditName, &QLineEdit::textChanged, this,
              &C_SdBueSignalPropertiesWidget::m_OnTextChanged);
   disconnect(this->mpc_Ui->pc_LineEditName, &QLineEdit::editingFinished, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterNameChange);
   disconnect(this->mpc_Ui->pc_LineEditUnit, &QLineEdit::textChanged, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterChange);
   disconnect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_DoubleSpinBoxFactor, static_cast<void (QDoubleSpinBox::*)(
                                                                   float64)>(&QDoubleSpinBox::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_RegisterScalingChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_DoubleSpinBoxOffset, static_cast<void (QDoubleSpinBox::*)(
                                                                   float64)>(&QDoubleSpinBox::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_RegisterScalingChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxByteOrder, static_cast<void (QComboBox::*)(
                                                                 sintn)>(&QComboBox::currentIndexChanged), this,
              &C_SdBueSignalPropertiesWidget::m_RegisterPositionChange);
   disconnect(this->mpc_Ui->pc_CheckBoxAutoMinMax, &C_OgeChxProperties::toggled, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterChange);
   disconnect(this->mpc_Ui->pc_WidgetMin, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange);
   disconnect(this->mpc_Ui->pc_WidgetMax, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange);
   disconnect(this->mpc_Ui->pc_WidgetInit, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
              &C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                            sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleValueType);

   //May happen while deleting multiple signals
   if (((pc_OSCSignalCommon != NULL) && (pc_UISignalCommon != NULL)) && (pc_OSCMessage != NULL))
   {
      tgl_assert(this->mu32_SignalIndex < pc_OSCMessage->c_Signals.size());
      if (this->mu32_SignalIndex < pc_OSCMessage->c_Signals.size())
      {
         const C_OSCCanSignal & rc_OSCSignal = pc_OSCMessage->c_Signals[this->mu32_SignalIndex];

         //name
         this->mpc_Ui->pc_LineEditName->setText(pc_OSCSignalCommon->c_Name.c_str());

         //comment
         this->mpc_Ui->pc_TextEditComment->setText(pc_OSCSignalCommon->c_Comment.c_str());

         //Auto min max
         this->mpc_Ui->pc_CheckBoxAutoMinMax->setChecked(pc_UISignalCommon->q_AutoMinMaxActive);

         m_ReInitMinMaxAndInit();

         //Factor
         this->mpc_Ui->pc_DoubleSpinBoxFactor->setValue(pc_OSCSignalCommon->f64_Factor);

         //Offset
         this->mpc_Ui->pc_DoubleSpinBoxOffset->setValue(pc_OSCSignalCommon->f64_Offset);

         //Unit
         this->mpc_Ui->pc_LineEditUnit->setText(pc_OSCSignalCommon->c_Unit.c_str());

         //Byte order
         switch (rc_OSCSignal.e_ComByteOrder)
         {
         case C_OSCCanSignal::eBYTE_ORDER_INTEL:
            this->mpc_Ui->pc_ComboBoxByteOrder->setCurrentIndex(ms32_BYTE_ORDER_INDEX_INTEL);
            break;
         case C_OSCCanSignal::eBYTE_ORDER_MOTOROLA:
            this->mpc_Ui->pc_ComboBoxByteOrder->setCurrentIndex(ms32_BYTE_ORDER_INDEX_MOTOROLA);
            break;
         default:
            this->mpc_Ui->pc_ComboBoxByteOrder->setCurrentIndex(-1);
            break;
         }

         //Type
         switch (pc_OSCSignalCommon->GetType())
         {
         case C_OSCNodeDataPoolContent::eSINT8:
         case C_OSCNodeDataPoolContent::eSINT16:
         case C_OSCNodeDataPoolContent::eSINT32:
         case C_OSCNodeDataPoolContent::eSINT64:
            this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(ms32_TYPE_INDEX_SIGNED);
            break;
         case C_OSCNodeDataPoolContent::eUINT8:
         case C_OSCNodeDataPoolContent::eUINT16:
         case C_OSCNodeDataPoolContent::eUINT32:
         case C_OSCNodeDataPoolContent::eUINT64:
            this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(ms32_TYPE_INDEX_UNSIGNED);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(ms32_TYPE_INDEX_FLOATING_32);
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(ms32_TYPE_INDEX_FLOATING_64);
            break;
         default:
            this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(-1);
            break;
         }

         ReloadPosition(false);

         //Message
         this->mpc_Ui->pc_LabelMessageValue->setText(pc_OSCMessage->c_Name.c_str());

         //Initial check
         m_CheckSignalName(false);
         m_CheckMinMaxAndInitValue(false);
      }
   }

   this->mq_DataChangesAllowed = true;
   //connects for RegisterChange
   connect(this->mpc_Ui->pc_LineEditName, &QLineEdit::textChanged, this,
           &C_SdBueSignalPropertiesWidget::m_OnTextChanged);
   connect(this->mpc_Ui->pc_LineEditName, &QLineEdit::editingFinished, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterNameChange);
   connect(this->mpc_Ui->pc_LineEditUnit, &QLineEdit::textChanged, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterChange);
   connect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_DoubleSpinBoxFactor, static_cast<void (QDoubleSpinBox::*)(
                                                                float64)>(&QDoubleSpinBox::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_RegisterScalingChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_DoubleSpinBoxOffset, static_cast<void (QDoubleSpinBox::*)(
                                                                float64)>(&QDoubleSpinBox::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_RegisterScalingChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxByteOrder, static_cast<void (QComboBox::*)(
                                                              sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueSignalPropertiesWidget::m_RegisterPositionChange);
   connect(this->mpc_Ui->pc_CheckBoxAutoMinMax, &C_OgeChxProperties::toggled, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterChange);
   connect(this->mpc_Ui->pc_WidgetMin, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange);
   connect(this->mpc_Ui->pc_WidgetMax, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange);
   connect(this->mpc_Ui->pc_WidgetInit, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
           &C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                         sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleValueType);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle toggle of auto min max check box

   \param[in] orq_AutoMinMaxActive Flag for auto min max active

   \created     31.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleAutoMinMaxCheckBox(const bool & orq_AutoMinMaxActive)
{
   if (orq_AutoMinMaxActive == true)
   {
      //Deactivate all fields
      this->mpc_Ui->pc_LabelMin->setEnabled(false);
      this->mpc_Ui->pc_LabelMax->setEnabled(false);
      this->mpc_Ui->pc_WidgetMin->setEnabled(false);
      this->mpc_Ui->pc_WidgetMax->setEnabled(false);

      if (this->mq_DataChangesAllowed == true)
      {
         //Assign new values
         m_SaveToData();
      }
   }
   else
   {
      //Activate all fields
      this->mpc_Ui->pc_LabelMin->setEnabled(true);
      this->mpc_Ui->pc_LabelMax->setEnabled(true);
      this->mpc_Ui->pc_WidgetMin->setEnabled(true);
      this->mpc_Ui->pc_WidgetMax->setEnabled(true);
   }
   //Renew min & max
   this->m_ReInitMinMaxAndInit();
   //Trigger error check
   m_CheckMinMaxAndInitValue();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle value type change

   \param[in] ors32_Index Current index

   \created     31.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleValueType(const sint32 & ors32_Index)
{
   if (ors32_Index == ms32_TYPE_INDEX_UNSIGNED)
   {
      //No change
   }
   else if (ors32_Index == ms32_TYPE_INDEX_SIGNED)
   {
      //No change
   }
   else if (ors32_Index == ms32_TYPE_INDEX_FLOATING_32)
   {
      this->mpc_Ui->pc_SpinBoxLength->setValue(32);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxLength->setValue(64);
   }
   if ((ors32_Index == ms32_TYPE_INDEX_UNSIGNED) || (ors32_Index == ms32_TYPE_INDEX_SIGNED))
   {
      this->mpc_Ui->pc_SpinBoxLength->setEnabled(true);
   }
   else
   {
      this->mpc_Ui->pc_SpinBoxLength->setEnabled(false);
   }
   m_UpdateAutoMinMax();

   // TODO
   /*
   if (this->mq_PositionUpdate == true)
   {
      Q_EMIT this->SigTypeChanged(this->mc_MessageId);
   }
   */
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle change of bit length

   \param[in] ors32_Value New spin box value

   \created     31.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleValueLengthChange(const sint32 & ors32_Value)
{
   Q_UNUSED(ors32_Value)
   m_UpdateAutoMinMax();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger update of auto min max

   \created     31.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_UpdateAutoMinMax(void)
{
   m_HandleAutoMinMaxCheckBox(this->mpc_Ui->pc_CheckBoxAutoMinMax->isChecked());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt value to signal length

   \param[in]     ou16_BitLength Current bit length
   \param[in,out] orc_Content    Content to restrict

   \created     04.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::mh_AdaptValueToSignalLength(const uint16 ou16_BitLength,
                                                                C_OSCNodeDataPoolContent & orc_Content)
{
   C_OSCNodeDataPoolContent c_Min;
   C_OSCNodeDataPoolContent c_Max;

   c_Min.SetArray(false);
   c_Min.SetType(orc_Content.GetType());
   tgl_assert(mh_InitMin(c_Min, ou16_BitLength) == C_NO_ERR);

   c_Max.SetArray(false);
   c_Max.SetType(orc_Content.GetType());
   tgl_assert(mh_InitMax(c_Max, ou16_BitLength) == C_NO_ERR);

   if (orc_Content < c_Min)
   {
      orc_Content = c_Min;
   }
   if (orc_Content > c_Max)
   {
      orc_Content = c_Max;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load generic content

   \param[in,out] opc_Widget     Spin box
   \param[in]     orc_Content    Value
   \param[in]     of64_Factor    Scaling factor
   \param[in]     of64_Offset    Scaling offset
   \param[in]     ou16_BitLength Signal bit length
   \param[in]     opc_Min        Optional minimum
   \param[in]     opc_Max        Optional maximum

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdBueSignalPropertiesWidget::m_LoadGeneric(C_OgeWiSpinBoxGroup * const opc_Widget,
                                                    const C_OSCNodeDataPoolContent & orc_Content,
                                                    const float64 of64_Factor, const float64 of64_Offset,
                                                    const uint16 ou16_BitLength,
                                                    const C_OSCNodeDataPoolContent * const opc_Min,
                                                    const C_OSCNodeDataPoolContent * const opc_Max) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (opc_Min != NULL)
   {
      if ((orc_Content.GetType() != opc_Min->GetType()) || (opc_Min->GetArray() == true))
      {
         s32_Retval = C_RANGE;
      }
   }
   if (opc_Max != NULL)
   {
      if ((orc_Content.GetType() != opc_Max->GetType()) || (opc_Max->GetArray() == true))
      {
         s32_Retval = C_RANGE;
      }
   }
   if (orc_Content.GetArray() == true)
   {
      s32_Retval = C_RANGE;
   }
   if ((s32_Retval == C_NO_ERR) && (opc_Widget != NULL))
   {
      C_OSCNodeDataPoolContent c_Min;
      C_OSCNodeDataPoolContent c_Max;
      if (opc_Min != NULL)
      {
         c_Min = *opc_Min;
      }
      else
      {
         c_Min.SetArray(false);
         c_Min.SetType(orc_Content.GetType());
         if (mh_InitMin(c_Min, ou16_BitLength) != C_NO_ERR)
         {
            C_SdNdeDataPoolContentUtil::h_InitMin(c_Min);
         }
      }
      if (opc_Max != NULL)
      {
         c_Max = *opc_Max;
      }
      else
      {
         c_Max.SetArray(false);
         c_Max.SetType(orc_Content.GetType());
         if (mh_InitMax(c_Max, ou16_BitLength) != C_NO_ERR)
         {
            C_SdNdeDataPoolContentUtil::h_InitMax(c_Max);
         }
      }
      opc_Widget->Init(c_Min, c_Max, of64_Factor, of64_Offset);
      opc_Widget->SetValue(C_SdNdeDataPoolContentUtil::h_ConvertScaledContentToGeneric(orc_Content, of64_Factor,
                                                                                       of64_Offset, 0UL));
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set content to minimum allowed value

   \param[in,out] orc_Content    Content to set
   \param[in]     ou16_BitLength Signal bit length

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     24.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdBueSignalPropertiesWidget::mh_InitMin(C_OSCNodeDataPoolContent & orc_Content, const uint16 ou16_BitLength)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;

   //Check type is realistic for number of bits
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      if (ou16_BitLength > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      if (ou16_BitLength > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      if (ou16_BitLength > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      if (ou16_BitLength > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      if (ou16_BitLength > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      if (ou16_BitLength > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      if (ou16_BitLength > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      if (ou16_BitLength > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      if (ou16_BitLength > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      if (ou16_BitLength > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Unsigned
      if (q_IsUnsigned == true)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(0);
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(0);
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(0);
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(0ULL);
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
      }
      //Signed
      if (q_IsSigned == true)
      {
         uint64 u64_Max = 0;

         //We need exactly one more than half of the unsigned maximum
         if (ou16_BitLength > 0U)
         {
            u64_Max += 1ULL << (static_cast<uint64>(ou16_BitLength) - 1ULL);
         }

         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(-static_cast<sint8>(u64_Max));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(-static_cast<sint16>(u64_Max));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(-static_cast<sint32>(u64_Max));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(-static_cast<sint64>(u64_Max));
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
      }
      //Float
      if (q_IsFloat == true)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(-std::numeric_limits<float32>::max());
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(-std::numeric_limits<float64>::max());
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set content to maximum allowed value

   \param[in,out] orc_Content    Content to set
   \param[in]     ou16_BitLength Signal bit length

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid

   \created     24.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdBueSignalPropertiesWidget::mh_InitMax(C_OSCNodeDataPoolContent & orc_Content, const uint16 ou16_BitLength)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_IsUnsigned = false;
   bool q_IsSigned = false;
   bool q_IsFloat = false;

   //Check type is realistic for number of bits
   switch (orc_Content.GetType())
   {
   case C_OSCNodeDataPoolContent::eUINT8:
      if (ou16_BitLength > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT16:
      if (ou16_BitLength > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT32:
      if (ou16_BitLength > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eUINT64:
      if (ou16_BitLength > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsUnsigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT8:
      if (ou16_BitLength > 8)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT16:
      if (ou16_BitLength > 16)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT32:
      if (ou16_BitLength > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eSINT64:
      if (ou16_BitLength > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsSigned = true;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT32:
      if (ou16_BitLength > 32)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   case C_OSCNodeDataPoolContent::eFLOAT64:
      if (ou16_BitLength > 64)
      {
         s32_Retval = C_RANGE;
      }
      q_IsFloat = true;
      break;
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Unsigned
      if (q_IsUnsigned == true)
      {
         uint64 u64_Max = 0;

         for (uint16 u16_ItBit = 0; u16_ItBit < ou16_BitLength; ++u16_ItBit)
         {
            u64_Max += 1ULL << static_cast<uint64>(u16_ItBit);
         }

         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eUINT8:
            orc_Content.SetValueU8(static_cast<uint8>(u64_Max));
            break;
         case C_OSCNodeDataPoolContent::eUINT16:
            orc_Content.SetValueU16(static_cast<uint16>(u64_Max));
            break;
         case C_OSCNodeDataPoolContent::eUINT32:
            orc_Content.SetValueU32(static_cast<uint32>(u64_Max));
            break;
         case C_OSCNodeDataPoolContent::eUINT64:
            orc_Content.SetValueU64(u64_Max);
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
      }
      //Signed
      if (q_IsSigned == true)
      {
         uint64 u64_Max = 0;

         //We need exactly one more than half of the unsigned maximum
         if (ou16_BitLength > 0U)
         {
            u64_Max += 1ULL << (static_cast<uint64>(ou16_BitLength) - 1ULL);
         }

         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eSINT8:
            orc_Content.SetValueS8(static_cast<sint8>(u64_Max - 1U));
            break;
         case C_OSCNodeDataPoolContent::eSINT16:
            orc_Content.SetValueS16(static_cast<sint16>(u64_Max - 1U));
            break;
         case C_OSCNodeDataPoolContent::eSINT32:
            orc_Content.SetValueS32(static_cast<sint32>(u64_Max - 1UL));
            break;
         case C_OSCNodeDataPoolContent::eSINT64:
            orc_Content.SetValueS64(static_cast<sint64>(u64_Max - 1ULL));
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
      }
      //Float
      if (q_IsFloat == true)
      {
         switch (orc_Content.GetType())
         {
         case C_OSCNodeDataPoolContent::eFLOAT32:
            orc_Content.SetValueF32(std::numeric_limits<float32>::max());
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            orc_Content.SetValueF64(std::numeric_limits<float64>::max());
            break;
         default:
            s32_Retval = C_CONFIG;
            break;
         }
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save generic content

   \param[in]     opc_Widget  Spin box
   \param[in,out] orc_Content Value
   \param[in]     of64_Factor Scaling factor
   \param[in]     of64_Offset Scaling offset

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SdBueSignalPropertiesWidget::m_SaveGeneric(const C_OgeWiSpinBoxGroup * const opc_Widget,
                                                    C_OSCNodeDataPoolContent & orc_Content, const float64 of64_Factor,
                                                    const float64 of64_Offset) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (opc_Widget != NULL)
   {
      s32_Retval = C_SdNdeDataPoolContentUtil::h_SetDataVariableFromGenericWithScaling(
         opc_Widget->GetValue(), orc_Content, of64_Factor, of64_Offset, 0UL);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Current type

   \return
   Get current type

   \created     24.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_SdBueSignalPropertiesWidget::m_GetCurrentType(void) const
{
   C_OSCNodeDataPoolContent::E_Type e_Retval;
   const sintn sn_Length = this->mpc_Ui->pc_SpinBoxLength->value();
   const sint32 s32_CurrentIndex = this->mpc_Ui->pc_ComboBoxType->currentIndex();

   if (s32_CurrentIndex == ms32_TYPE_INDEX_UNSIGNED)
   {
      if (sn_Length <= 8)
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT8;
      }
      else if ((sn_Length <= 16) && (sn_Length > 8))
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT16;
      }
      else if ((sn_Length <= 32) && (sn_Length > 16))
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT32;
      }
      else
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT64;
      }
   }
   else if (s32_CurrentIndex == ms32_TYPE_INDEX_SIGNED)
   {
      if (sn_Length <= 8)
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT8;
      }
      else if ((sn_Length <= 16) && (sn_Length > 8))
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT16;
      }
      else if ((sn_Length <= 32) && (sn_Length > 16))
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT32;
      }
      else
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT64;
      }
   }
   else if (s32_CurrentIndex == ms32_TYPE_INDEX_FLOATING_32)
   {
      e_Retval = C_OSCNodeDataPoolContent::eFLOAT32;
   }
   else
   {
      e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
   }
   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reinitialize min max and init based on stored values

   \created     24.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_ReInitMinMaxAndInit(const float64 * const opf64_Factor,
                                                          const float64 * const opf64_Offset) const
{
   const C_OSCCanMessage * const pc_OSCMessage = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
      C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(this->mc_MessageId, this->mu32_SignalIndex);

   if ((pc_OSCSignalCommon != NULL) && (pc_OSCMessage != NULL))
   {
      tgl_assert(this->mu32_SignalIndex < pc_OSCMessage->c_Signals.size());
      if (this->mu32_SignalIndex < pc_OSCMessage->c_Signals.size())
      {
         const C_OSCCanSignal & rc_OSCSignal = pc_OSCMessage->c_Signals[this->mu32_SignalIndex];
         float64 f64_Factor;
         float64 f64_Offset;
         if (opf64_Factor != NULL)
         {
            f64_Factor = *opf64_Factor;
         }
         else
         {
            f64_Factor = pc_OSCSignalCommon->f64_Factor;
         }
         if (opf64_Offset != NULL)
         {
            f64_Offset = *opf64_Offset;
         }
         else
         {
            f64_Offset = pc_OSCSignalCommon->f64_Offset;
         }
         //Min
         tgl_assert(m_LoadGeneric(this->mpc_Ui->pc_WidgetMin, pc_OSCSignalCommon->c_MinValue, f64_Factor, f64_Offset,
                                  rc_OSCSignal.u16_ComBitLength, NULL, &pc_OSCSignalCommon->c_MaxValue) == C_NO_ERR);

         //Max
         tgl_assert(m_LoadGeneric(this->mpc_Ui->pc_WidgetMax, pc_OSCSignalCommon->c_MaxValue, f64_Factor, f64_Offset,
                                  rc_OSCSignal.u16_ComBitLength, &pc_OSCSignalCommon->c_MinValue, NULL) == C_NO_ERR);
         //Init
         if (pc_OSCSignalCommon->c_DataSetValues.size() > 0)
         {
            tgl_assert(m_LoadGeneric(this->mpc_Ui->pc_WidgetInit, pc_OSCSignalCommon->c_DataSetValues[0],
                                     f64_Factor, f64_Offset, rc_OSCSignal.u16_ComBitLength,
                                     &pc_OSCSignalCommon->c_MinValue,
                                     &pc_OSCSignalCommon->c_MaxValue) == C_NO_ERR);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check signal name

   \param[in] orq_SignalErrorChange Optional flag to suppress error signal

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckSignalName(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_Combined;
      bool q_NameInvalid = false;
      bool q_NameConflict = false;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, NULL, NULL, &q_NameConflict, &q_NameInvalid,
                                           NULL, NULL, NULL,
                                           C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(this->mc_MessageId.
                                                                                                  e_ComProtocol));

      q_Combined = (q_NameInvalid == false) && (q_NameConflict == false);
      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditName, "Valid", q_Combined);
      if (q_Combined == true)
      {
         this->mpc_Ui->pc_LineEditName->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Heading = C_GtGetText::h_GetText("Signal Name");
         QString c_Content;
         if (q_NameInvalid == true)
         {
            c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters\n");
         }
         if (q_NameConflict == true)
         {
            c_Content += C_GtGetText::h_GetText("- is already in use\n");
         }
         this->mpc_Ui->pc_LineEditName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
      }
      if (orq_SignalErrorChange == true)
      {
         Q_EMIT this->SigRecheckError(this->mc_MessageId);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check position values

   \param[in] orq_SignalErrorChange Optional flag to suppress error signal

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckMessagePosition(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_PositionValid;
      bool q_LayoutConflict = false;
      bool q_BorderConflict = false;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, &q_LayoutConflict, &q_BorderConflict, NULL,
                                           NULL, NULL, NULL, NULL, C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                              this->mc_MessageId.e_ComProtocol));
      q_PositionValid = (q_LayoutConflict == false) && (q_BorderConflict == false);
      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxStartBit, "Valid", q_PositionValid);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxLength, "Valid", q_PositionValid);
      if (q_PositionValid == true)
      {
         this->mpc_Ui->pc_SpinBoxStartBit->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
         this->mpc_Ui->pc_SpinBoxLength->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Content = C_GtGetText::h_GetText(
            "Either signal out of range of message DLC or intersecting signals detected.");
         this->mpc_Ui->pc_SpinBoxStartBit->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
         this->mpc_Ui->pc_SpinBoxLength->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
      }
      if (orq_SignalErrorChange == true)
      {
         Q_EMIT this->SigRecheckError(this->mc_MessageId);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check min, max or init value

   \param[in] orq_SignalErrorChange Optional flag to suppress error signal

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckMinMaxAndInitValue(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_InitValid;
      bool q_MinOverMax = false;
      bool q_ValueBelowMin = false;
      bool q_ValueOverMax = false;
      bool q_MinMaxValid;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, NULL, NULL, NULL, NULL,
                                           &q_MinOverMax, &q_ValueBelowMin, &q_ValueOverMax,
                                           C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                              this->mc_MessageId.e_ComProtocol));
      q_InitValid = (q_ValueBelowMin == false) && (q_ValueOverMax == false);
      q_MinMaxValid = (q_MinOverMax == false);
      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetMin, "Valid", q_MinMaxValid);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetMax, "Valid", q_MinMaxValid);
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_WidgetInit, "Valid", q_InitValid);
      if (q_MinMaxValid == true)
      {
         this->mpc_Ui->pc_WidgetMin->SetToolTipInformation("", "", stw_opensyde_gui::C_NagToolTip::eDEFAULT);
         this->mpc_Ui->pc_WidgetMax->SetToolTipInformation("", "", stw_opensyde_gui::C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Content = C_GtGetText::h_GetText("Minimum over maximum value");
         this->mpc_Ui->pc_WidgetMin->SetToolTipInformation("", c_Content, stw_opensyde_gui::C_NagToolTip::eERROR);
         this->mpc_Ui->pc_WidgetMax->SetToolTipInformation("", c_Content, stw_opensyde_gui::C_NagToolTip::eERROR);
      }
      if (q_InitValid == true)
      {
         this->mpc_Ui->pc_WidgetInit->SetToolTipInformation("", "", stw_opensyde_gui::C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Content = C_GtGetText::h_GetText("Init value out of range of maximum and minimum value");
         this->mpc_Ui->pc_WidgetInit->SetToolTipInformation("", c_Content, stw_opensyde_gui::C_NagToolTip::eDEFAULT);
      }
      if (orq_SignalErrorChange == true)
      {
         Q_EMIT this->SigRecheckError(this->mc_MessageId);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trimm bus name

   Remove whitespaces at the beginning and end of the string

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_TrimmMessageName(void) const
{
   this->mpc_Ui->pc_LineEditName->setText(this->mpc_Ui->pc_LineEditName->text().trimmed());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register Change

   Function where ui elements register a change. Change will be sent via a signal

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_RegisterChange(void)
{
   m_SaveToData();
   //signal
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_OnTextChanged(void)
{
   this->m_SaveToData();
   this->m_CheckSignalName(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register name change

   Function where ui elements register a change. Change will be sent via a signal

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_RegisterNameChange(void)
{
   this->m_TrimmMessageName();
   m_RegisterChange();
   this->m_CheckSignalName();
   Q_EMIT this->SigNameChanged(this->mc_MessageId, this->mu32_SignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register scaling change

   \created     24.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_RegisterScalingChange(void)
{
   const float64 f64_Factor = this->mpc_Ui->pc_DoubleSpinBoxFactor->value();
   const float64 f64_Offset = this->mpc_Ui->pc_DoubleSpinBoxOffset->value();

   //Manually insert updated values
   m_ReInitMinMaxAndInit(&f64_Factor, &f64_Offset);
   //Save after reinit so there is no change of unscaled values
   m_RegisterChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register position change

   Function where ui elements register a change. Change will be sent via a signal

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_RegisterPositionChange(void)
{
   m_RegisterChange();
   m_CheckMessagePosition();
   //Trigger reload of potentially changed min&max values
   m_ReInitMinMaxAndInit();
   Q_EMIT this->SigPositionChanged(this->mc_MessageId, this->mu32_SignalIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register min, max or init value change

   Function where ui elements register a change. Change will be sent via a signal

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_RegisterMinMaxAndInitChange(void)
{
   m_RegisterChange();
   m_CheckMinMaxAndInitValue();
   m_ReInitMinMaxAndInit();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save data

   \created     29.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_SaveToData(void)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      const C_PuiSdNodeCanSignal * const pc_UiSignal = C_PuiSdHandler::h_GetInstance()->GetUiCanSignal(
         this->mc_MessageId, this->mu32_SignalIndex);
      if (pc_UiSignal != NULL)
      {
         C_OSCCanSignal c_OSCSignal;
         C_OSCCanMessageIdentificationIndices c_NewMessageId;
         C_OSCNodeDataPoolListElement c_OSCSignalCommon;
         C_PuiSdNodeDataPoolListElement c_UISignalCommon;

         //name
         c_OSCSignalCommon.c_Name = this->mpc_Ui->pc_LineEditName->text().toStdString().c_str();

         //comment
         c_OSCSignalCommon.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();

         //Auto min max
         c_UISignalCommon.q_AutoMinMaxActive = this->mpc_Ui->pc_CheckBoxAutoMinMax->isChecked();

         //Factor
         c_OSCSignalCommon.f64_Factor = this->mpc_Ui->pc_DoubleSpinBoxFactor->value();

         //Offset
         c_OSCSignalCommon.f64_Offset = this->mpc_Ui->pc_DoubleSpinBoxOffset->value();

         //Length
         c_OSCSignal.u16_ComBitLength = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxLength->value());

         //Min
         c_OSCSignalCommon.c_MinValue.SetArray(false);
         c_OSCSignalCommon.c_MinValue.SetType(this->m_GetCurrentType());
         if (c_UISignalCommon.q_AutoMinMaxActive == true)
         {
            mh_InitMin(c_OSCSignalCommon.c_MinValue, c_OSCSignal.u16_ComBitLength);
         }
         else
         {
            m_SaveGeneric(this->mpc_Ui->pc_WidgetMin, c_OSCSignalCommon.c_MinValue, c_OSCSignalCommon.f64_Factor,
                          c_OSCSignalCommon.f64_Offset);
         }

         //Max
         c_OSCSignalCommon.c_MaxValue.SetArray(false);
         c_OSCSignalCommon.c_MaxValue.SetType(this->m_GetCurrentType());
         if (c_UISignalCommon.q_AutoMinMaxActive == true)
         {
            mh_InitMax(c_OSCSignalCommon.c_MaxValue, c_OSCSignal.u16_ComBitLength);
         }
         else
         {
            m_SaveGeneric(this->mpc_Ui->pc_WidgetMax, c_OSCSignalCommon.c_MaxValue, c_OSCSignalCommon.f64_Factor,
                          c_OSCSignalCommon.f64_Offset);
         }

         //Init
         c_OSCSignalCommon.c_DataSetValues.resize(1);
         c_OSCSignalCommon.c_DataSetValues[0].SetArray(false);
         c_OSCSignalCommon.c_DataSetValues[0].SetType(this->m_GetCurrentType());
         m_SaveGeneric(this->mpc_Ui->pc_WidgetInit, c_OSCSignalCommon.c_DataSetValues[0], c_OSCSignalCommon.f64_Factor,
                       c_OSCSignalCommon.f64_Offset);

         //Also update min & max & init value for new range
         mh_AdaptValueToSignalLength(c_OSCSignal.u16_ComBitLength, c_OSCSignalCommon.c_MinValue);
         mh_AdaptValueToSignalLength(c_OSCSignal.u16_ComBitLength, c_OSCSignalCommon.c_MaxValue);
         mh_AdaptValueToSignalLength(c_OSCSignal.u16_ComBitLength, c_OSCSignalCommon.c_DataSetValues[0]);

         //Value (Necessary for type consistency)
         c_OSCSignalCommon.c_Value = c_OSCSignalCommon.c_MinValue;
         c_OSCSignalCommon.c_NvmValue = c_OSCSignalCommon.c_MinValue;

         //Unit
         c_OSCSignalCommon.c_Unit = this->mpc_Ui->pc_LineEditUnit->text().toStdString().c_str();

         //Byte order
         switch (this->mpc_Ui->pc_ComboBoxByteOrder->currentIndex())
         {
         case ms32_BYTE_ORDER_INDEX_INTEL:
            c_OSCSignal.e_ComByteOrder = C_OSCCanSignal::eBYTE_ORDER_INTEL;
            break;
         case ms32_BYTE_ORDER_INDEX_MOTOROLA:
            c_OSCSignal.e_ComByteOrder = C_OSCCanSignal::eBYTE_ORDER_MOTOROLA;
            break;
         default:
            break;
         }

         //Start bit
         c_OSCSignal.u16_ComBitStart = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxStartBit->value());

         //Add critical message check
         if (this->mpc_MessageSyncManager->RecheckCriticalMessage(this->mc_MessageId, c_NewMessageId) == true)
         {
            tgl_assert(false);
            //Not possible here but should also not happen
            //this->mc_MessageId = c_NewMessageId;
         }

         this->mpc_MessageSyncManager->SetCanSignal(this->mc_MessageId, this->mu32_SignalIndex, c_OSCSignal,
                                                    c_OSCSignalCommon, c_UISignalCommon, *pc_UiSignal);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Reconnect signals for position change

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_ConnectPositionUpdate(void)
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxLength, static_cast<void (QSpinBox::*)(
                                                          sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_RegisterPositionChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxStartBit, static_cast<void (QSpinBox::*)(
                                                            sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_RegisterPositionChange);

   this->mq_PositionUpdate = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Disconnect signals for position change

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_DisconnectPositionUpdate(void)
{
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxLength, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_RegisterPositionChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxStartBit, static_cast<void (QSpinBox::*)(
                                                               sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_RegisterPositionChange);

   this->mq_PositionUpdate = false;
}
