//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing signal properties

   Widget for editing signal properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <limits>
#include <QFlags>
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
#include "C_OgeWiCustomMessage.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_OSCNodeDataPoolContentUtil.h"
#include "C_SdBueSignalPropertiesWidget.h"
#include "ui_C_SdBueSignalPropertiesWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sint32 ms32_BYTE_ORDER_INDEX_INTEL = 0;
const sint32 ms32_BYTE_ORDER_INDEX_MOTOROLA = 1;

const sint32 ms32_MUX_DEFAULT = 0;
const sint32 ms32_MUX_MULTIPLEXER_SIGNAL = 1;
const sint32 ms32_MUX_MULTIPLEXED_SIGNAL = 2;

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
C_SdBueSignalPropertiesWidget::C_SdBueSignalPropertiesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueSignalPropertiesWidget),
   mpc_MessageSyncManager(NULL),
   mu32_SignalIndex(0U),
   me_DataType(eTY_UNSIGNED)
{
   // init UI
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_ComboBoxType->view()->setTextElideMode(Qt::ElideRight);

   InitStaticNames();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the mpc_MessageSyncManager because of handling it on an other position
C_SdBueSignalPropertiesWidget::~C_SdBueSignalPropertiesWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
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
   this->mpc_Ui->pc_LabelStartBit->setText(C_GtGetText::h_GetText("Start Bit"));
   this->mpc_Ui->pc_LabelMuxType->setText(C_GtGetText::h_GetText("Multiplexer Type"));
   this->mpc_Ui->pc_LabelMuxValue->setText(C_GtGetText::h_GetText("Multiplexer Value"));
   this->mpc_Ui->pc_CheckBoxAutoMinMax->setText(C_GtGetText::h_GetText("Auto min/max"));

   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));

   //Combo boxes
   //Ensure there are four items
   this->mpc_Ui->pc_ComboBoxType->addItem("1");
   this->mpc_Ui->pc_ComboBoxType->addItem("2");
   this->mpc_Ui->pc_ComboBoxType->addItem("3");
   this->mpc_Ui->pc_ComboBoxType->addItem("4");
   //Ensure the indices match the text
   this->mpc_Ui->pc_ComboBoxType->setItemText(static_cast<sintn>(eTY_UNSIGNED),
                                              C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                             eUINT8));
   this->mpc_Ui->pc_ComboBoxType->setItemText(static_cast<sintn>(eTY_SIGNED),
                                              C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                             eSINT8));
   this->mpc_Ui->pc_ComboBoxType->setItemText(static_cast<sintn>(eTY_FLOAT32),
                                              C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                             eFLOAT32));
   this->mpc_Ui->pc_ComboBoxType->setItemText(static_cast<sintn>(eTY_FLOAT64),
                                              C_SdTooltipUtil::h_ConvertTypeToNameSimplified(C_OSCNodeDataPoolContent::
                                                                                             eFLOAT64));

   //Ensure there are two items
   this->mpc_Ui->pc_ComboBoxByteOrder->addItem("1");
   this->mpc_Ui->pc_ComboBoxByteOrder->addItem("2");
   //Ensure the indices match the text
   this->mpc_Ui->pc_ComboBoxByteOrder->setItemText(ms32_BYTE_ORDER_INDEX_INTEL,
                                                   C_SdUtil::h_ConvertByteOrderToName(C_OSCCanSignal::eBYTE_ORDER_INTEL));
   this->mpc_Ui->pc_ComboBoxByteOrder->setItemText(ms32_BYTE_ORDER_INDEX_MOTOROLA,
                                                   C_SdUtil::h_ConvertByteOrderToName(C_OSCCanSignal::
                                                                                      eBYTE_ORDER_MOTOROLA));

   //Ensure there are three items
   this->mpc_Ui->pc_ComboBoxMuxType->addItem("1");
   this->mpc_Ui->pc_ComboBoxMuxType->addItem("2");
   this->mpc_Ui->pc_ComboBoxMuxType->addItem("3");
   //Ensure the indices match the text
   this->mpc_Ui->pc_ComboBoxMuxType->setItemText(ms32_MUX_DEFAULT, C_GtGetText::h_GetText("none"));
   this->mpc_Ui->pc_ComboBoxMuxType->setItemText(ms32_MUX_MULTIPLEXER_SIGNAL,
                                                 C_GtGetText::h_GetText("Multiplexer Signal"));
   this->mpc_Ui->pc_ComboBoxMuxType->setItemText(ms32_MUX_MULTIPLEXED_SIGNAL,
                                                 C_GtGetText::h_GetText("Multiplexed Signal"));

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
                                        "\n - only alphanumeric characters and \"_\""
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

   //Multiplexer
   c_InfoText =  C_GtGetText::h_GetText("Available types:\n"
                                        "none (default): Standard signal, present in all messages of this type\n"
                                        "Multiplexer Signal: Multiplexer signal, present in all messages of this type.\n"
                                        "Value signifies which multiplexed signals will be present in the current instance of this message.\n"
                                        "Multiplexed Signal: Multiplexed signal, only present in messages with the multiplexer signal value matching this signal's multiplexer value.\n");
   this->mpc_Ui->pc_LabelMuxType->SetToolTipInformation(C_GtGetText::h_GetText("Multiplexer Type"), c_InfoText);
   c_InfoText =  C_GtGetText::h_GetText(
      "With which multiplexer signal value should this multiplexed signal be transmitted.");
   this->mpc_Ui->pc_LabelMuxValue->SetToolTipInformation(C_GtGetText::h_GetText("Multiplexer Value"), c_InfoText);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set message sync manager

   \param[in,out]  opc_Value  Message sync manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SetMessageSyncManager(
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   this->mpc_MessageSyncManager = opc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal ID setter

   Sets the private signal id of widget

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  New signal id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SetSignalId(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                                const uint32 ou32_SignalIndex)
{
   this->mc_MessageId = orc_MessageId;
   this->mu32_SignalIndex = ou32_SignalIndex;

   //load bus data
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal position in message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::ReloadPosition(void)
{
   this->m_HandleAnyChange(eCHA_MLV);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current message id

   \return
   Current message id
*/
//----------------------------------------------------------------------------------------------------------------------
stw_opensyde_core::C_OSCCanMessageIdentificationIndices C_SdBueSignalPropertiesWidget::GetMessageId(void) const
{
   return mc_MessageId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current signal index

   \return
   Current signal index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SdBueSignalPropertiesWidget::GetSignalIndex(void) const
{
   return this->mu32_SignalIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects the node name in the text edit for fast editing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::SelectName(void) const
{
   this->mpc_Ui->pc_LineEditName->setFocus();
   this->mpc_Ui->pc_LineEditName->selectAll();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   In case of a disconnected node update of the unique message ids

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::OnNodeDisconnected(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex)
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load bus information

   Load bus information from core data using bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_LoadFromData(void)
{
   const C_OSCCanMessage * const pc_OSCMessage = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_PuiSdNodeCanSignal * const pc_UiSignal = C_PuiSdHandler::h_GetInstance()->GetUiCanSignal(this->mc_MessageId,
                                                                                                    this->mu32_SignalIndex);
   const C_OSCNodeDataPoolListElement * const pc_OSCSignalCommon =
      C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolListElement(this->mc_MessageId, this->mu32_SignalIndex);
   const C_PuiSdNodeDataPoolListElement * const pc_UISignalCommon =
      C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolListElement(this->mc_MessageId, this->mu32_SignalIndex);

   //May happen while deleting multiple signals
   if ((((pc_OSCSignalCommon != NULL) && (pc_UISignalCommon != NULL)) && (pc_OSCMessage != NULL)) &&
       (pc_UiSignal != NULL))
   {
      tgl_assert(this->mu32_SignalIndex < pc_OSCMessage->c_Signals.size());
      if (this->mu32_SignalIndex < pc_OSCMessage->c_Signals.size())
      {
         const C_OSCCanSignal & rc_OSCSignal = pc_OSCMessage->c_Signals[this->mu32_SignalIndex];

         //Copy
         this->mc_DataOSCSignal = rc_OSCSignal;
         this->mc_DataOSCSignalCommon = *pc_OSCSignalCommon;
         this->mc_DataUiSignal = *pc_UiSignal;
         this->mc_DataUiSignalCommon = *pc_UISignalCommon;

         //Helper
         switch (this->mc_DataOSCSignalCommon.GetType())
         {
         case C_OSCNodeDataPoolContent::eSINT8:
         case C_OSCNodeDataPoolContent::eSINT16:
         case C_OSCNodeDataPoolContent::eSINT32:
         case C_OSCNodeDataPoolContent::eSINT64:
            this->me_DataType = eTY_SIGNED;
            break;
         case C_OSCNodeDataPoolContent::eUINT8:
         case C_OSCNodeDataPoolContent::eUINT16:
         case C_OSCNodeDataPoolContent::eUINT32:
         case C_OSCNodeDataPoolContent::eUINT64:
            this->me_DataType = eTY_UNSIGNED;
            break;
         case C_OSCNodeDataPoolContent::eFLOAT32:
            this->me_DataType = eTY_FLOAT32;
            break;
         case C_OSCNodeDataPoolContent::eFLOAT64:
            this->me_DataType = eTY_FLOAT64;
            break;
         default:
            break;
         }

         //One time changes
         //Multiplexer (one time)
         this->m_InitComboBox(*pc_OSCMessage, this->mu32_SignalIndex);
         //Set range before value
         m_HandleMuxValueRange(*pc_OSCMessage, this->mu32_SignalIndex);

         //Update all fields
         m_UpdateUIForChange(eCHA_NAME);
         m_UpdateUIForChange(eCHA_COMMENT);
         m_UpdateUIForChange(eCHA_AUTO_MIN_MAX);
         m_UpdateUIForChange(eCHA_MIN);
         m_UpdateUIForChange(eCHA_MAX);
         m_UpdateUIForChange(eCHA_FACTOR);
         m_UpdateUIForChange(eCHA_OFFSET);
         m_UpdateUIForChange(eCHA_INIT);
         m_UpdateUIForChange(eCHA_UNIT);
         m_UpdateUIForChange(eCHA_BYTE_ORDER);
         m_UpdateUIForChange(eCHA_VALUE_TYPE);
         m_UpdateUIForChange(eCHA_LENGTH);
         m_UpdateUIForChange(eCHA_START_BIT);
         m_UpdateUIForChange(eCHA_MUX_TYPE);
         m_UpdateUIForChange(eCHA_MUX_VALUE);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle toggle of auto min max check box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleAutoMinMaxCheckBoxChange(void)
{
   this->m_HandleAnyChange(eCHA_AUTO_MIN_MAX);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle min value change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMinChange(void)
{
   this->m_HandleAnyChange(eCHA_MIN);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle max value change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMaxChange(void)
{
   this->m_HandleAnyChange(eCHA_MAX);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle factor change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleFactorChange(void)
{
   this->m_HandleAnyChange(eCHA_FACTOR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle offset change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleOffsetChange(void)
{
   this->m_HandleAnyChange(eCHA_OFFSET);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle init value change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleInitChange(void)
{
   this->m_HandleAnyChange(eCHA_INIT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle unit change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleUnitChange(void)
{
   this->m_HandleAnyChange(eCHA_UNIT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle byte order change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleByteOrderChange(void)
{
   this->m_HandleAnyChange(eCHA_BYTE_ORDER);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle value type change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleValueTypeChange(void)
{
   this->m_HandleAnyChange(eCHA_VALUE_TYPE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle change of bit length
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleValueLengthChange(void)
{
   this->m_HandleAnyChange(eCHA_LENGTH);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle start bit change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleStartBitChange(void)
{
   this->m_HandleAnyChange(eCHA_START_BIT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle multiplexer type change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMuxTypeChange(void)
{
   this->m_HandleAnyChange(eCHA_MUX_TYPE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle multiplexer value change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMuxValueChange(void)
{
   this->m_HandleAnyChange(eCHA_MUX_VALUE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt value to signal length

   \param[in]      ou16_BitLength   Current bit length
   \param[in,out]  orc_Content      Content to restrict
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::mh_AdaptValueToSignalLength(const uint16 ou16_BitLength,
                                                                C_OSCNodeDataPoolContent & orc_Content)
{
   C_OSCNodeDataPoolContent c_Min;
   C_OSCNodeDataPoolContent c_Max;

   c_Min.SetArray(false);
   c_Min.SetType(orc_Content.GetType());
   C_SdNdeDpContentUtil::h_InitMinForSignal(c_Min, ou16_BitLength);

   c_Max.SetArray(false);
   c_Max.SetType(orc_Content.GetType());
   C_SdNdeDpContentUtil::h_InitMaxForSignal(c_Max, ou16_BitLength);

   if (orc_Content < c_Min)
   {
      orc_Content = c_Min;
   }
   if (orc_Content > c_Max)
   {
      orc_Content = c_Max;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load generic content

   \param[in,out]  opc_Widget       Spin box
   \param[in]      orc_Content      Value
   \param[in]      of64_Factor      Scaling factor
   \param[in]      of64_Offset      Scaling offset
   \param[in]      ou16_BitLength   Signal bit length
   \param[in]      opc_Min          Optional minimum
   \param[in]      opc_Max          Optional maximum

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
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
         C_SdNdeDpContentUtil::h_InitMinForSignal(c_Min, ou16_BitLength);
      }
      if (opc_Max != NULL)
      {
         c_Max = *opc_Max;
      }
      else
      {
         c_Max.SetArray(false);
         c_Max.SetType(orc_Content.GetType());
         C_SdNdeDpContentUtil::h_InitMaxForSignal(c_Max, ou16_BitLength);
      }
      opc_Widget->Init(c_Min, c_Max, of64_Factor, of64_Offset);
      opc_Widget->SetValue(C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(orc_Content, of64_Factor,
                                                                                 of64_Offset, 0UL));
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save generic content

   \param[in]      opc_Widget    Spin box
   \param[in,out]  orc_Content   Value
   \param[in]      of64_Factor   Scaling factor
   \param[in]      of64_Offset   Scaling offset

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdBueSignalPropertiesWidget::m_SaveGeneric(const C_OgeWiSpinBoxGroup * const opc_Widget,
                                                    C_OSCNodeDataPoolContent & orc_Content, const float64 of64_Factor,
                                                    const float64 of64_Offset) const
{
   sint32 s32_Retval = C_NO_ERR;

   if (opc_Widget != NULL)
   {
      s32_Retval = C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(
         opc_Widget->GetValue(), orc_Content, of64_Factor, of64_Offset, 0UL);
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Current type

   \return
   Get current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeDataPoolContent::E_Type C_SdBueSignalPropertiesWidget::m_GetCurrentType(void) const
{
   C_OSCNodeDataPoolContent::E_Type e_Retval = C_OSCNodeDataPoolContent::eUINT8;
   const uint16 u16_BitLength = this->mc_DataOSCSignal.u16_ComBitLength;

   switch (this->me_DataType)
   {
   case eTY_UNSIGNED:
      if (u16_BitLength <= 8U)
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT8;
      }
      else if ((u16_BitLength <= 16U) && (u16_BitLength > 8U))
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT16;
      }
      else if ((u16_BitLength <= 32U) && (u16_BitLength > 16U))
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT32;
      }
      else
      {
         e_Retval = C_OSCNodeDataPoolContent::eUINT64;
      }
      break;
   case eTY_SIGNED:
      if (u16_BitLength <= 8U)
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT8;
      }
      else if ((u16_BitLength <= 16U) && (u16_BitLength > 8U))
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT16;
      }
      else if ((u16_BitLength <= 32U) && (u16_BitLength > 16U))
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT32;
      }
      else
      {
         e_Retval = C_OSCNodeDataPoolContent::eSINT64;
      }
      break;
   case eTY_FLOAT32:
      e_Retval = C_OSCNodeDataPoolContent::eFLOAT32;
      break;
   case eTY_FLOAT64:
      e_Retval = C_OSCNodeDataPoolContent::eFLOAT64;
      break;
   default:
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check signal name

   \param[in]  orq_SignalErrorChange   Optional flag to suppress error signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckSignalName(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.u32_DatapoolIndex, this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_Combined;
      bool q_NameInvalid = false;
      bool q_NameConflict = false;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, NULL, NULL, &q_NameConflict, &q_NameInvalid,
                                           NULL, NULL, NULL, NULL, NULL,
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
         Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check MUX type

   \param[in]  orq_SignalErrorChange   Optional flag to suppress error signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckMUXType(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.u32_DatapoolIndex, this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_MuxTypeInvalid;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, &q_MuxTypeInvalid, NULL,
                                           C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                              this->mc_MessageId.e_ComProtocol));
      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_ComboBoxMuxType, "Valid", !q_MuxTypeInvalid);
      if (q_MuxTypeInvalid == false)
      {
         this->mpc_Ui->pc_ComboBoxMuxType->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Heading = C_GtGetText::h_GetText(
            "Multiplexer type");
         const QString c_Content = C_GtGetText::h_GetText(
            "There is currently no multiplexer defined for this multiplexed signal");
         this->mpc_Ui->pc_ComboBoxMuxType->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
      }
      if (orq_SignalErrorChange == true)
      {
         Q_EMIT this->SigRecheckError(this->mc_MessageId);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check MUX value

   \param[in]  orq_SignalErrorChange   Optional flag to suppress error signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckMUXValue(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.u32_DatapoolIndex, this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_MuxValueInvalid;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, &q_MuxValueInvalid,  C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                              this->mc_MessageId.e_ComProtocol));
      //set invalid text property
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_SpinBoxMuxValue, "Valid", !q_MuxValueInvalid);
      if (q_MuxValueInvalid == false)
      {
         this->mpc_Ui->pc_SpinBoxMuxValue->SetToolTipAdditionalInfo("", C_NagToolTip::eDEFAULT);
      }
      else
      {
         const QString c_Content = C_GtGetText::h_GetText(
            "This value is out of range of the Multiplexer signal");
         this->mpc_Ui->pc_SpinBoxMuxValue->SetToolTipAdditionalInfo(c_Content, C_NagToolTip::eERROR);
      }
      if (orq_SignalErrorChange == true)
      {
         Q_EMIT this->SigRecheckError(this->mc_MessageId);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check position values

   \param[in]  orq_SignalErrorChange   Optional flag to suppress error signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_CheckMessagePosition(const bool & orq_SignalErrorChange)
{
   const C_OSCCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
   const C_OSCNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOSCCanDataPoolList(
      this->mc_MessageId.u32_NodeIndex, this->mc_MessageId.e_ComProtocol, this->mc_MessageId.u32_InterfaceIndex,
      this->mc_MessageId.u32_DatapoolIndex, this->mc_MessageId.q_MessageIsTx);

   //check
   if ((pc_Message != NULL) && (pc_List != NULL))
   {
      bool q_PositionValid;
      bool q_LayoutConflict = false;
      bool q_BorderConflict = false;

      pc_Message->CheckErrorSignalDetailed(pc_List, this->mu32_SignalIndex, &q_LayoutConflict, &q_BorderConflict, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL, C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle name change and do every check without sending any error changed signals yet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleNameChangeWithoutSignal(void)
{
   //Avoid signals
   this->m_HandleAnyChange(eCHA_NAME, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle name change with all necessary steps and update triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleNameChangeWithSignal(void)
{
   this->m_HandleAnyChange(eCHA_NAME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle comment change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleCommentChange(void)
{
   this->m_HandleAnyChange(eCHA_COMMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data

   \param[in]  oe_Change   Detected UI change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_SaveToData(const E_Change oe_Change)
{
   if (this->mpc_MessageSyncManager != NULL)
   {
      C_OSCCanMessageIdentificationIndices c_NewMessageId;
      //Multiplexer
      if ((oe_Change == eCHA_MUX_TYPE) &&
          (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL))
      {
         //Adapt parent
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
         if ((pc_Message != NULL) && (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE))
         {
            C_OSCCanMessage c_Copy = *pc_Message;
            c_Copy.u32_CycleTimeMs = 0U;
            c_Copy.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
            this->mpc_MessageSyncManager->SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
               this->mc_MessageId, c_Copy);
         }
      }

      this->mpc_MessageSyncManager->SetCanSignal(this->mc_MessageId, this->mu32_SignalIndex, this->mc_DataOSCSignal,
                                                 this->mc_DataOSCSignalCommon, this->mc_DataUiSignalCommon,
                                                 this->mc_DataUiSignal);

      //Add critical message check (after change of data)
      if (this->mpc_MessageSyncManager->RecheckCriticalMessage(this->mc_MessageId, c_NewMessageId) == true)
      {
         tgl_assert(false);
         //Not possible here but should also not happen
         //this->mc_MessageId = c_NewMessageId;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current mutliplexer type

   \return
   Current mutliplexer type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCCanSignal::E_MultiplexerType C_SdBueSignalPropertiesWidget::m_GetMuxType(void) const
{
   C_OSCCanSignal::E_MultiplexerType e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxMuxType->currentIndex())
   {
   case ms32_MUX_DEFAULT:
      e_Retval = C_OSCCanSignal::eMUX_DEFAULT;
      break;
   case ms32_MUX_MULTIPLEXER_SIGNAL:
      e_Retval = C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL;
      break;
   case ms32_MUX_MULTIPLEXED_SIGNAL:
      e_Retval = C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL;
      break;
   default:
      e_Retval = C_OSCCanSignal::eMUX_DEFAULT;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize the combo box for multiplexer type

   \param[in]  orc_Message       Current message
   \param[in]  ou32_SignalIndex  Current signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_InitComboBox(const C_OSCCanMessage & orc_Message,
                                                   const uint32 ou32_SignalIndex) const
{
   bool q_OtherMultiplexer = false;

   //Handle multiplexer
   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
   {
      //Only check other signals
      if (ou32_SignalIndex != u32_ItSig)
      {
         const C_OSCCanSignal & rc_Signal = orc_Message.c_Signals[u32_ItSig];
         if (rc_Signal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
         {
            q_OtherMultiplexer = true;
         }
      }
   }
   this->mpc_Ui->pc_ComboBoxMuxType->SetItemState(ms32_MUX_MULTIPLEXER_SIGNAL, !q_OtherMultiplexer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle automated range for multiplexer value

   \param[in]  orc_Message       Current message
   \param[in]  ou32_SignalIndex  Current signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMuxValueRange(const C_OSCCanMessage & orc_Message,
                                                          const uint32 ou32_SignalIndex) const
{
   bool q_Max = false;
   sintn sn_Max = 0;

   this->mpc_Ui->pc_SpinBoxMuxValue->SetMinimumCustom(0);
   //Handle multiplexer
   for (uint32 u32_ItSig = 0UL; u32_ItSig < orc_Message.c_Signals.size(); ++u32_ItSig)
   {
      //Only check other signals
      if (ou32_SignalIndex != u32_ItSig)
      {
         const C_OSCCanSignal & rc_Signal = orc_Message.c_Signals[u32_ItSig];
         if (rc_Signal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
         {
            const uint64 u64_Max = (1ULL << rc_Signal.u16_ComBitLength) - 1ULL;
            sn_Max = static_cast<sintn>(u64_Max);
            q_Max = true;
         }
      }
   }
   //Automated maximum
   this->mpc_Ui->pc_SpinBoxMuxValue->SetMaximumCustom(
      static_cast<sintn>(std::numeric_limits<uint16>::max()), q_Max, sn_Max);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle all changes on UI

   \param[in]  oe_Change                              Detected UI change
   \param[in]  oq_AllowSignalsToInformOtherWidgets    Optional flag to allow or block signals to inform other widgets
                                                      of changes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleAnyChange(const C_SdBueSignalPropertiesWidget::E_Change oe_Change,
                                                      const bool oq_AllowSignalsToInformOtherWidgets)
{
   std::list<E_Change> c_Changes;
   std::list<E_Change> c_ErrorChanges;
   QString c_UserNotificationText;
   QString c_UserNotificationAdditionalInformation;
   //Step 1: get new value from UI
   this->m_ApplyNewValueFromUI(oe_Change);
   //Step 2: change all values depending on the current value change
   this->m_AdaptOtherValues(oe_Change, c_Changes, c_ErrorChanges, c_UserNotificationText,
                            c_UserNotificationAdditionalInformation);
   //Step 3: adapt other signals
   this->m_UpdateOtherSignalsForChange(oe_Change);
   //Step 4: Write new value to data storage
   this->m_SaveToData(oe_Change);
   //Step 5: Update UI errors (usually only relevant for itself)
   for (std::list<E_Change>::const_iterator c_ItChange = c_ErrorChanges.begin(); c_ItChange != c_ErrorChanges.end();
        ++c_ItChange)
   {
      this->m_UpdateErrorForChange(*c_ItChange);
   }
   //Step 6: Update UI based on all changes
   for (std::list<E_Change>::const_iterator c_ItChange = c_Changes.begin(); c_ItChange != c_Changes.end();
        ++c_ItChange)
   {
      this->m_UpdateUIForChange(*c_ItChange);
   }
   if (oq_AllowSignalsToInformOtherWidgets)
   {
      //Step 7: Send signals for current change
      this->m_SendSignalForChange(oe_Change);
   }
   //User notification last
   if ((c_UserNotificationText.isEmpty() == false) || (c_UserNotificationAdditionalInformation.isEmpty() == false))
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Multiplexer type"));
      c_Message.SetDescription(c_UserNotificationText);
      c_Message.SetDetails(c_UserNotificationAdditionalInformation);
      c_Message.SetCustomMinHeight(230, 400); //unknown issue: message box grabs the place it need.
                                              //but better than without custom adaptation
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply value from UI

   \param[in]  oe_Change   Detected UI change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_ApplyNewValueFromUI(const C_SdBueSignalPropertiesWidget::E_Change oe_Change)
{
   const C_OSCCanSignal * pc_Signal;

   switch (oe_Change)
   {
   case eCHA_NAME:
      this->mc_DataOSCSignalCommon.c_Name = this->mpc_Ui->pc_LineEditName->text().trimmed().toStdString().c_str();
      break;
   case eCHA_COMMENT:
      this->mc_DataOSCSignalCommon.c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText().toStdString().c_str();
      break;
   case eCHA_AUTO_MIN_MAX:
      this->mc_DataUiSignalCommon.q_AutoMinMaxActive = this->mpc_Ui->pc_CheckBoxAutoMinMax->isChecked();
      break;
   case eCHA_FACTOR:
      this->mc_DataOSCSignalCommon.f64_Factor = this->mpc_Ui->pc_DoubleSpinBoxFactor->value();
      break;
   case eCHA_OFFSET:
      this->mc_DataOSCSignalCommon.f64_Offset = this->mpc_Ui->pc_DoubleSpinBoxOffset->value();
      break;
   case eCHA_VALUE_TYPE:
      this->me_DataType = static_cast<E_Type>(this->mpc_Ui->pc_ComboBoxType->currentIndex());
      break;
   case eCHA_LENGTH:
      this->mc_DataOSCSignal.u16_ComBitLength = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxLength->value());
      break;
   case eCHA_MIN:
      this->mc_DataOSCSignalCommon.c_MinValue.SetArray(false);
      this->mc_DataOSCSignalCommon.c_MinValue.SetType(this->m_GetCurrentType());
      //Get change from UI should only happen if user can interact
      tgl_assert(this->mc_DataUiSignalCommon.q_AutoMinMaxActive == false);
      m_SaveGeneric(this->mpc_Ui->pc_WidgetMin, this->mc_DataOSCSignalCommon.c_MinValue,
                    this->mc_DataOSCSignalCommon.f64_Factor,
                    this->mc_DataOSCSignalCommon.f64_Offset);
      break;
   case eCHA_MAX:
      this->mc_DataOSCSignalCommon.c_MaxValue.SetArray(false);
      this->mc_DataOSCSignalCommon.c_MaxValue.SetType(this->m_GetCurrentType());
      //Get change from UI should only happen if user can interact
      tgl_assert(this->mc_DataUiSignalCommon.q_AutoMinMaxActive == false);
      m_SaveGeneric(this->mpc_Ui->pc_WidgetMax, this->mc_DataOSCSignalCommon.c_MaxValue,
                    this->mc_DataOSCSignalCommon.f64_Factor,
                    this->mc_DataOSCSignalCommon.f64_Offset);
      break;
   case eCHA_INIT:
      this->mc_DataOSCSignalCommon.c_DataSetValues.resize(1);
      this->mc_DataOSCSignalCommon.c_DataSetValues[0].SetArray(false);
      this->mc_DataOSCSignalCommon.c_DataSetValues[0].SetType(this->m_GetCurrentType());
      m_SaveGeneric(this->mpc_Ui->pc_WidgetInit, this->mc_DataOSCSignalCommon.c_DataSetValues[0],
                    this->mc_DataOSCSignalCommon.f64_Factor,
                    this->mc_DataOSCSignalCommon.f64_Offset);
      break;
   case eCHA_UNIT:
      this->mc_DataOSCSignalCommon.c_Unit = this->mpc_Ui->pc_LineEditUnit->text().toStdString().c_str();
      break;
   case eCHA_BYTE_ORDER:
      switch (this->mpc_Ui->pc_ComboBoxByteOrder->currentIndex())
      {
      case ms32_BYTE_ORDER_INDEX_INTEL:
         this->mc_DataOSCSignal.e_ComByteOrder = C_OSCCanSignal::eBYTE_ORDER_INTEL;
         break;
      case ms32_BYTE_ORDER_INDEX_MOTOROLA:
         this->mc_DataOSCSignal.e_ComByteOrder = C_OSCCanSignal::eBYTE_ORDER_MOTOROLA;
         break;
      default:
         break;
      }
      break;
   case eCHA_MUX_TYPE:
      this->mc_DataOSCSignal.e_MultiplexerType = this->m_GetMuxType();
      break;
   case eCHA_MUX_VALUE:
      this->mc_DataOSCSignal.u16_MultiplexValue = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxMuxValue->value());
      break;
   case eCHA_START_BIT:
      this->mc_DataOSCSignal.u16_ComBitStart = static_cast<uint16>(this->mpc_Ui->pc_SpinBoxStartBit->value());
      break;
   case eCHA_MLV:
      //Data changed outside of this widgets responsibility
      //Update value from data!
      pc_Signal = C_PuiSdHandler::h_GetInstance()->GetCanSignal(this->mc_MessageId, this->mu32_SignalIndex);
      if (pc_Signal != NULL)
      {
         this->mc_DataOSCSignal.u16_ComBitStart = pc_Signal->u16_ComBitStart;
         this->mc_DataOSCSignal.u16_ComBitLength = pc_Signal->u16_ComBitLength;
      }
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle automated adaptations for other fields based on the current change

   \param[in]   oe_Change                                   Detected UI change
   \param[out]  orc_Changes                                 Requested UI changes for other fields
   \param[out]  orc_ErrorChanges                            Requested error check (usually only relevant for itself)
   \param[out]  orc_UserNotificationText                    User notification text if any
   \param[out]  orc_UserNotificationAdditionalInformation   User notification additional text if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_AdaptOtherValues(const C_SdBueSignalPropertiesWidget::E_Change oe_Change,
                                                       std::list<E_Change> & orc_Changes,
                                                       std::list<E_Change> & orc_ErrorChanges,
                                                       QString & orc_UserNotificationText,
                                                       QString & orc_UserNotificationAdditionalInformation)
{
   C_OSCNodeDataPoolContent::E_Type e_NewType;
   switch (oe_Change)
   {
   case eCHA_NAME:
      //Update itself for error
      orc_ErrorChanges.push_back(eCHA_NAME);
      //Affects MLV
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_MUX_VALUE:
      //Update itself for error
      orc_ErrorChanges.push_back(eCHA_MUX_VALUE);
      //Error update might be necessary
      orc_Changes.push_back(eCHA_START_BIT);
      orc_Changes.push_back(eCHA_LENGTH);
      //Affects MLV
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_BYTE_ORDER:
      //Affects MLV
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_INIT:
   case eCHA_UNIT:
   case eCHA_COMMENT:
      //No other elements affected
      break;
   case eCHA_START_BIT:
      //Update itself for error
      orc_ErrorChanges.push_back(eCHA_START_BIT);
      //Error
      orc_Changes.push_back(eCHA_LENGTH);
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_MIN:
   case eCHA_MAX:
   case eCHA_AUTO_MIN_MAX:
      //Dataset resize just to be on the safe
      this->mc_DataOSCSignalCommon.c_DataSetValues.resize(1);
      //Min
      if (oe_Change != eCHA_MIN)
      {
         this->m_HandleMinValueRange();
         orc_Changes.push_back(eCHA_MIN);
      }
      //Max
      if (oe_Change != eCHA_MAX)
      {
         this->m_HandleMaxValueRange();
         orc_Changes.push_back(eCHA_MAX);
      }
      //Init
      this->m_HandleInitValueRange();
      orc_Changes.push_back(eCHA_INIT);
      break;
   case eCHA_LENGTH:
      //Dataset resize just to be on the safe
      this->mc_DataOSCSignalCommon.c_DataSetValues.resize(1);
      //Determine new data type
      e_NewType = this->m_GetCurrentType();
      //Handle the actual new type
      //Adapt types
      this->m_InitializeDataWithPotentialNewType(e_NewType);
      //Min
      this->m_HandleMinValueRange();
      orc_Changes.push_back(eCHA_MIN);
      //Max
      this->m_HandleMaxValueRange();
      orc_Changes.push_back(eCHA_MAX);
      //Init
      this->m_HandleInitValueRange();
      orc_Changes.push_back(eCHA_INIT);
      //Update itself for error
      orc_ErrorChanges.push_back(eCHA_LENGTH);
      //Error check change
      orc_Changes.push_back(eCHA_START_BIT);
      //MLV update trigger
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_MLV:
      //Dataset resize just to be on the safe
      this->mc_DataOSCSignalCommon.c_DataSetValues.resize(1);
      //Determine new data type
      e_NewType = this->m_GetCurrentType();
      //Handle the actual new type
      //Adapt types
      this->m_InitializeDataWithPotentialNewType(e_NewType);
      //Min
      this->m_HandleMinValueRange();
      orc_Changes.push_back(eCHA_MIN);
      //Max
      this->m_HandleMaxValueRange();
      orc_Changes.push_back(eCHA_MAX);
      //Init
      this->m_HandleInitValueRange();
      orc_Changes.push_back(eCHA_INIT);
      //Error check change
      orc_Changes.push_back(eCHA_START_BIT);
      orc_Changes.push_back(eCHA_LENGTH);
      break;
   case eCHA_VALUE_TYPE:
      //Dataset resize just to be on the safe
      this->mc_DataOSCSignalCommon.c_DataSetValues.resize(1);
      //Determine new data type
      e_NewType = this->m_GetCurrentType();
      //This might trigger a length change
      //Length field might change in all cases (disabled/enabled)
      orc_Changes.push_back(eCHA_LENGTH);
      //Handle length adaptation
      if (e_NewType == C_OSCNodeDataPoolContent::eFLOAT32)
      {
         this->mc_DataOSCSignal.u16_ComBitLength = 32U;
      }
      else if (e_NewType == C_OSCNodeDataPoolContent::eFLOAT64)
      {
         this->mc_DataOSCSignal.u16_ComBitLength = 64U;
      }
      else
      {
         //No change necessary
      }
      //Handle the actual new type
      //Adapt types
      this->m_InitializeDataWithPotentialNewType(e_NewType);
      //Min
      this->m_HandleMinValueRange();
      orc_Changes.push_back(eCHA_MIN);
      //Max
      this->m_HandleMaxValueRange();
      orc_Changes.push_back(eCHA_MAX);
      //Init
      this->m_HandleInitValueRange();
      orc_Changes.push_back(eCHA_INIT);
      //MLV update trigger
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_MUX_TYPE:
      //Dataset resize just to be on the safe
      this->mc_DataOSCSignalCommon.c_DataSetValues.resize(1);
      //Special type handling for multiplexer signals
      //Each of MUX type might affect those fields visually even if their value stays unchanged (disabled/enabled)
      orc_Changes.push_back(eCHA_AUTO_MIN_MAX);
      orc_Changes.push_back(eCHA_FACTOR);
      orc_Changes.push_back(eCHA_OFFSET);
      orc_Changes.push_back(eCHA_UNIT);
      orc_Changes.push_back(eCHA_LENGTH);
      orc_Changes.push_back(eCHA_VALUE_TYPE);
      orc_Changes.push_back(eCHA_MUX_VALUE);
      //Error check change
      orc_Changes.push_back(eCHA_START_BIT);
      orc_Changes.push_back(eCHA_MUX_TYPE);
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         bool q_MessageAdapt;
         const C_OSCCanMessage * const pc_Message =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
         float64 f64_CurrentInitVal;

         // The init value will be set to 0.0 later than the other values
         this->mc_DataOSCSignalCommon.c_DataSetValues[0].GetAnyValueAsFloat64(f64_CurrentInitVal, 0);

         //Only relevant if the range gets restricted
         orc_Changes.push_back(eCHA_MIN);
         orc_Changes.push_back(eCHA_MAX);
         //Restricted values
         //lint -e{9137} If due to rounding a inaccuracy exists, it is good that the comparison fails. It will be reset.
         if (((pc_Message != NULL) && (pc_Message->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE)) ||
             (this->mc_DataUiSignalCommon.q_AutoMinMaxActive == false) ||
             (this->mc_DataOSCSignalCommon.f64_Factor != 1.0) ||
             (this->mc_DataOSCSignalCommon.f64_Offset != 0.0) ||
             (this->mc_DataOSCSignalCommon.c_Unit != "") ||
             (this->mc_DataOSCSignal.u16_ComBitLength > 16U) ||
             (this->me_DataType != eTY_UNSIGNED) ||
             (f64_CurrentInitVal != 0.0))
         {
            q_MessageAdapt = true;
         }
         else
         {
            q_MessageAdapt = false;
         }
         if (q_MessageAdapt == true)
         {
            orc_UserNotificationText = C_GtGetText::h_GetText(
               "The message has been adapted to some restrictions concerning multiplexer messages and signals.");
            orc_UserNotificationAdditionalInformation = C_GtGetText::h_GetText("Multiplexer message restrictions:\n"
                                                                               "- Tx method may not be \"On Change\"\n"
                                                                               "\n"
                                                                               "Multiplexer signal restrictions:\n"
                                                                               "- Auto min/max: active\n"
                                                                               "- Factor: 1\n"
                                                                               "- Offset: 0\n"
                                                                               "- Init value: 0\n"
                                                                               "- Unit: disabled\n"
                                                                               "- Length: Maximum 16 bit\n"
                                                                               "- Type: unsigned");
         }
         this->mc_DataUiSignalCommon.q_AutoMinMaxActive = true;
         this->mc_DataOSCSignalCommon.f64_Factor = 1.0;
         this->mc_DataOSCSignalCommon.f64_Offset = 0.0;
         this->mc_DataOSCSignalCommon.c_Unit = "";
         if (this->mc_DataOSCSignal.u16_ComBitLength > 16U)
         {
            this->mc_DataOSCSignal.u16_ComBitLength = 16U;
         }
         //Type
         this->me_DataType = eTY_UNSIGNED;
      }
      //Determine new data type
      e_NewType = this->m_GetCurrentType();
      //Handle the actual new type
      //Adapt types
      this->m_InitializeDataWithPotentialNewType(e_NewType);
      //Min
      this->m_HandleMinValueRange();
      orc_Changes.push_back(eCHA_MIN);
      //Max
      this->m_HandleMaxValueRange();
      orc_Changes.push_back(eCHA_MAX);
      //Init
      this->m_HandleInitValueRange();
      orc_Changes.push_back(eCHA_INIT);
      //Multiplexer signal init value
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         //Adapt init value for multiplexer
         // Must be set after adapting minimum and maximum
         C_OSCNodeDataPoolContentUtil::h_SetValueInContent(0.0, this->mc_DataOSCSignalCommon.c_DataSetValues[0]);
      }
      //MLV update trigger
      orc_Changes.push_back(eCHA_MLV);
      break;
   case eCHA_FACTOR:
   case eCHA_OFFSET:
      //Only visibility changes
      orc_Changes.push_back(eCHA_MIN);
      orc_Changes.push_back(eCHA_MAX);
      orc_Changes.push_back(eCHA_INIT);
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize data structure with potential new type

   \param[in]  oe_Type  Expected Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_InitializeDataWithPotentialNewType(const C_OSCNodeDataPoolContent::E_Type oe_Type)
{
   this->mc_DataOSCSignalCommon.c_MinValue.SetArray(false);
   this->mc_DataOSCSignalCommon.c_MinValue.SetType(oe_Type);

   this->mc_DataOSCSignalCommon.c_MaxValue.SetArray(false);
   this->mc_DataOSCSignalCommon.c_MaxValue.SetType(oe_Type);

   this->mc_DataOSCSignalCommon.c_DataSetValues[0].SetArray(false);
   this->mc_DataOSCSignalCommon.c_DataSetValues[0].SetType(oe_Type);

   //Value (Necessary for type consistency)
   this->mc_DataOSCSignalCommon.c_Value = this->mc_DataOSCSignalCommon.c_MinValue;
   this->mc_DataOSCSignalCommon.c_NvmValue = this->mc_DataOSCSignalCommon.c_MinValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt min value to range and auto min max flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMinValueRange()
{
   if (this->mc_DataUiSignalCommon.q_AutoMinMaxActive == true)
   {
      C_SdNdeDpContentUtil::h_InitMinForSignal(this->mc_DataOSCSignalCommon.c_MinValue,
                                               this->mc_DataOSCSignal.u16_ComBitLength);
   }
   else
   {
      mh_AdaptValueToSignalLength(this->mc_DataOSCSignal.u16_ComBitLength,
                                  this->mc_DataOSCSignalCommon.c_MinValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt max value to range and auto min max flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleMaxValueRange()
{
   if (this->mc_DataUiSignalCommon.q_AutoMinMaxActive == true)
   {
      C_SdNdeDpContentUtil::h_InitMaxForSignal(this->mc_DataOSCSignalCommon.c_MaxValue,
                                               this->mc_DataOSCSignal.u16_ComBitLength);
   }
   else
   {
      mh_AdaptValueToSignalLength(this->mc_DataOSCSignal.u16_ComBitLength,
                                  this->mc_DataOSCSignalCommon.c_MaxValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt init value to allowed range
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_HandleInitValueRange(void)
{
   mh_AdaptValueToSignalLength(this->mc_DataOSCSignal.u16_ComBitLength,
                               this->mc_DataOSCSignalCommon.c_DataSetValues[0]);
   if (this->mc_DataOSCSignalCommon.c_DataSetValues[0] < this->mc_DataOSCSignalCommon.c_MinValue)
   {
      this->mc_DataOSCSignalCommon.c_DataSetValues[0] = this->mc_DataOSCSignalCommon.c_MinValue;
   }
   if (this->mc_DataOSCSignalCommon.c_DataSetValues[0] > this->mc_DataOSCSignalCommon.c_MaxValue)
   {
      this->mc_DataOSCSignalCommon.c_DataSetValues[0] = this->mc_DataOSCSignalCommon.c_MaxValue;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle any changes in other signals

   \param[in]  oe_Change   Detected UI change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_UpdateOtherSignalsForChange(
   const C_SdBueSignalPropertiesWidget::E_Change oe_Change) const
{
   switch (oe_Change)
   {
   case eCHA_MLV:
   case eCHA_LENGTH:
      if ((this->mpc_MessageSyncManager != NULL) &&
          (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL))
      {
         //Adapt parent
         const C_OSCCanMessage * const pc_OSCMessage =
            C_PuiSdHandler::h_GetInstance()->GetCanMessage(this->mc_MessageId);
         if ((pc_OSCMessage != NULL) && (pc_OSCMessage->e_TxMethod == C_OSCCanMessage::eTX_METHOD_ON_CHANGE))
         {
            C_OSCCanMessage c_Copy = *pc_OSCMessage;
            // On event has no cycle time
            c_Copy.u32_CycleTimeMs = 0U;
            c_Copy.e_TxMethod = C_OSCCanMessage::eTX_METHOD_ON_EVENT;
            this->mpc_MessageSyncManager->SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
               this->mc_MessageId, c_Copy);
         }
      }
      break;
   case eCHA_INIT:
   case eCHA_NAME:
   case eCHA_MUX_TYPE:
   case eCHA_VALUE_TYPE:
   case eCHA_START_BIT:
   case eCHA_UNIT:
   case eCHA_COMMENT:
   case eCHA_MUX_VALUE:
   case eCHA_BYTE_ORDER:
   case eCHA_MIN:
   case eCHA_MAX:
   case eCHA_AUTO_MIN_MAX:
   case eCHA_FACTOR:
   case eCHA_OFFSET:
      //No change necessary
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle UI updates based on the specified change

  Includes error check for initial call

   \param[in]  oe_Change   Detected UI change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_UpdateUIForChange(const E_Change oe_Change)
{
   //Don't trigger any new changes as all data is pulled from the internal data
   m_DisconnectAll();
   switch (oe_Change)
   {
   case eCHA_NAME:
      //Restrictions
      this->mpc_Ui->pc_LineEditName->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);
      //Value
      this->mpc_Ui->pc_LineEditName->setText(this->mc_DataOSCSignalCommon.c_Name.c_str());
      break;
   case eCHA_MUX_TYPE:
      //Restrictions
      if (this->mc_MessageId.e_ComProtocol == C_OSCCanProtocol::eLAYER2)
      {
         this->mpc_Ui->pc_ComboBoxMuxType->setVisible(true);
         this->mpc_Ui->pc_LabelMuxType->setVisible(true);
      }
      else
      {
         this->mpc_Ui->pc_ComboBoxMuxType->setVisible(false);
         this->mpc_Ui->pc_LabelMuxType->setVisible(false);
      }
      //Value
      switch (this->mc_DataOSCSignal.e_MultiplexerType)
      {
      case C_OSCCanSignal::eMUX_DEFAULT:
         this->mpc_Ui->pc_ComboBoxMuxType->setCurrentIndex(ms32_MUX_DEFAULT);
         break;
      case C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL:
         this->mpc_Ui->pc_ComboBoxMuxType->setCurrentIndex(ms32_MUX_MULTIPLEXER_SIGNAL);
         break;
      case C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL:
         this->mpc_Ui->pc_ComboBoxMuxType->setCurrentIndex(ms32_MUX_MULTIPLEXED_SIGNAL);
         break;
      default:
         break;
      }
      break;
   case eCHA_VALUE_TYPE:
      //Restrictions
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_LabelType->setEnabled(false);
         this->mpc_Ui->pc_ComboBoxType->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelType->setEnabled(true);
         this->mpc_Ui->pc_ComboBoxType->setEnabled(true);
      }
      //Value
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(static_cast<sintn>(this->me_DataType));
      break;
   case eCHA_START_BIT:
      //Restrictions
      if (this->mc_MessageId.e_ComProtocol == C_OSCCanProtocol::eECES)
      {
         this->mpc_Ui->pc_SpinBoxStartBit->SetMaximumCustom((64 - 16) - 1);
      }
      else
      {
         this->mpc_Ui->pc_SpinBoxStartBit->SetMaximumCustom(64 - 1);
      }
      //Value
      this->mpc_Ui->pc_SpinBoxStartBit->setValue(this->mc_DataOSCSignal.u16_ComBitStart);
      break;
   case eCHA_LENGTH:
      //Restrictions
      if ((this->mc_DataOSCSignalCommon.GetType() == C_OSCNodeDataPoolContent::eFLOAT32) ||
          (this->mc_DataOSCSignalCommon.GetType() == C_OSCNodeDataPoolContent::eFLOAT64))
      {
         this->mpc_Ui->pc_LabelLength->setEnabled(false);
         this->mpc_Ui->pc_SpinBoxLength->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelLength->setEnabled(true);
         this->mpc_Ui->pc_SpinBoxLength->setEnabled(true);
      }
      this->mpc_Ui->pc_SpinBoxLength->SetMinimumCustom(1);
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_SpinBoxLength->SetMaximumCustom(16);
      }
      else
      {
         if (this->mc_MessageId.e_ComProtocol == C_OSCCanProtocol::eECES)
         {
            this->mpc_Ui->pc_SpinBoxLength->SetMaximumCustom(64 - 16);
         }
         else
         {
            this->mpc_Ui->pc_SpinBoxLength->SetMaximumCustom(64);
         }
      }
      //Value
      this->mpc_Ui->pc_SpinBoxLength->setValue(this->mc_DataOSCSignal.u16_ComBitLength);
      break;
   case eCHA_INIT:
      //Restrictions
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_LabelInitValue->setEnabled(false);
         this->mpc_Ui->pc_WidgetInit->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelInitValue->setEnabled(true);
         this->mpc_Ui->pc_WidgetInit->setEnabled(true);
      }
      //Value
      tgl_assert(m_LoadGeneric(this->mpc_Ui->pc_WidgetInit, this->mc_DataOSCSignalCommon.c_DataSetValues[0],
                               this->mc_DataOSCSignalCommon.f64_Factor, this->mc_DataOSCSignalCommon.f64_Offset,
                               this->mc_DataOSCSignal.u16_ComBitLength,
                               &this->mc_DataOSCSignalCommon.c_MinValue,
                               &this->mc_DataOSCSignalCommon.c_MaxValue) == C_NO_ERR);
      break;
   case eCHA_UNIT:
      //Restrictions
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_LabelUnit->setEnabled(false);
         this->mpc_Ui->pc_LineEditUnit->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelUnit->setEnabled(true);
         this->mpc_Ui->pc_LineEditUnit->setEnabled(true);
      }
      //Value
      this->mpc_Ui->pc_LineEditUnit->setText(this->mc_DataOSCSignalCommon.c_Unit.c_str());
      break;
   case eCHA_COMMENT:
      //Restrictions
      //Value
      this->mpc_Ui->pc_TextEditComment->setText(this->mc_DataOSCSignalCommon.c_Comment.c_str());
      break;
   case eCHA_MUX_VALUE:
      //Restrictions
      if (this->mc_MessageId.e_ComProtocol == C_OSCCanProtocol::eLAYER2)
      {
         if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXED_SIGNAL)
         {
            this->mpc_Ui->pc_SpinBoxMuxValue->setEnabled(true);
            this->mpc_Ui->pc_LabelMuxValue->setEnabled(true);
         }
         else
         {
            this->mpc_Ui->pc_SpinBoxMuxValue->setEnabled(false);
            this->mpc_Ui->pc_LabelMuxValue->setEnabled(false);
         }
         this->mpc_Ui->pc_SpinBoxMuxValue->setVisible(true);
         this->mpc_Ui->pc_LabelMuxValue->setVisible(true);
      }
      else
      {
         this->mpc_Ui->pc_SpinBoxMuxValue->setVisible(false);
         this->mpc_Ui->pc_LabelMuxValue->setVisible(false);
      }
      //Value
      this->mpc_Ui->pc_SpinBoxMuxValue->setValue(this->mc_DataOSCSignal.u16_MultiplexValue);
      break;
   case eCHA_BYTE_ORDER:
      //Restrictions
      //Value
      switch (this->mc_DataOSCSignal.e_ComByteOrder)
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
      break;
   case eCHA_MIN:
      //Restrictions
      if (this->mc_DataUiSignalCommon.q_AutoMinMaxActive)
      {
         this->mpc_Ui->pc_LabelMin->setEnabled(false);
         this->mpc_Ui->pc_WidgetMin->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelMin->setEnabled(true);
         this->mpc_Ui->pc_WidgetMin->setEnabled(true);
      }
      //Value
      tgl_assert(m_LoadGeneric(this->mpc_Ui->pc_WidgetMin, this->mc_DataOSCSignalCommon.c_MinValue,
                               this->mc_DataOSCSignalCommon.f64_Factor, this->mc_DataOSCSignalCommon.f64_Offset,
                               this->mc_DataOSCSignal.u16_ComBitLength, NULL,
                               &this->mc_DataOSCSignalCommon.c_MaxValue) == C_NO_ERR);
      break;
   case eCHA_MAX:
      //Restrictions
      if (this->mc_DataUiSignalCommon.q_AutoMinMaxActive)
      {
         this->mpc_Ui->pc_LabelMax->setEnabled(false);
         this->mpc_Ui->pc_WidgetMax->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelMax->setEnabled(true);
         this->mpc_Ui->pc_WidgetMax->setEnabled(true);
      }
      //Value
      tgl_assert(m_LoadGeneric(this->mpc_Ui->pc_WidgetMax, this->mc_DataOSCSignalCommon.c_MaxValue,
                               this->mc_DataOSCSignalCommon.f64_Factor, this->mc_DataOSCSignalCommon.f64_Offset,
                               this->mc_DataOSCSignal.u16_ComBitLength, &this->mc_DataOSCSignalCommon.c_MinValue,
                               NULL) == C_NO_ERR);
      break;
   case eCHA_AUTO_MIN_MAX:
      //Restrictions
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_CheckBoxAutoMinMax->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_CheckBoxAutoMinMax->setEnabled(true);
      }
      //Value
      this->mpc_Ui->pc_CheckBoxAutoMinMax->setChecked(this->mc_DataUiSignalCommon.q_AutoMinMaxActive);
      break;
   case eCHA_FACTOR:
      //Restrictions
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_LabelFactor->setEnabled(false);
         this->mpc_Ui->pc_DoubleSpinBoxFactor->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelFactor->setEnabled(true);
         this->mpc_Ui->pc_DoubleSpinBoxFactor->setEnabled(true);
      }
      //Factor needs to be above zero
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_DoubleSpinBoxFactor->SetMinimumCustom(C_OgeSpxFactor::mhf64_FACTOR_MIN);
      this->mpc_Ui->pc_DoubleSpinBoxFactor->SetMaximumCustom(std::numeric_limits<float64>::max());
      //Value
      this->mpc_Ui->pc_DoubleSpinBoxFactor->setValue(this->mc_DataOSCSignalCommon.f64_Factor);
      break;
   case eCHA_OFFSET:
      //Restrictions
      if (this->mc_DataOSCSignal.e_MultiplexerType == C_OSCCanSignal::eMUX_MULTIPLEXER_SIGNAL)
      {
         this->mpc_Ui->pc_LabelOffset->setEnabled(false);
         this->mpc_Ui->pc_DoubleSpinBoxOffset->setEnabled(false);
      }
      else
      {
         this->mpc_Ui->pc_LabelOffset->setEnabled(true);
         this->mpc_Ui->pc_DoubleSpinBoxOffset->setEnabled(true);
      }
      this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMinimumCustom(std::numeric_limits<float64>::lowest());
      this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMaximumCustom(std::numeric_limits<float64>::max());
      //Value
      this->mpc_Ui->pc_DoubleSpinBoxOffset->setValue(this->mc_DataOSCSignalCommon.f64_Offset);
      break;
   case eCHA_MLV:
      //Update UI via signal
      Q_EMIT (this->SigUpdateMlv(this->mc_MessageId, this->mu32_SignalIndex));
      break;
   default:
      break;
   }
   this->m_UpdateErrorForChange(oe_Change);
   //Reactivate GUI change detection
   m_ConnectAll();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle UI updates based on the specified change

   \param[in]  oe_Change   Detected UI change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_UpdateErrorForChange(const C_SdBueSignalPropertiesWidget::E_Change oe_Change)
{
   switch (oe_Change)
   {
   case eCHA_NAME:
      //Check
      m_CheckSignalName(false);
      break;
   case eCHA_MUX_TYPE:
      //Check
      m_CheckMUXType(false);
      break;
   case eCHA_START_BIT:
      //Check
      m_CheckMessagePosition(false);
      break;
   case eCHA_LENGTH:
      //Check
      m_CheckMessagePosition(false);
      break;
   case eCHA_MUX_VALUE:
      //Check
      m_CheckMUXValue(false);
      break;
   case eCHA_VALUE_TYPE:
   case eCHA_INIT:
   case eCHA_UNIT:
   case eCHA_COMMENT:
   case eCHA_BYTE_ORDER:
   case eCHA_MIN:
   case eCHA_MAX:
   case eCHA_AUTO_MIN_MAX:
   case eCHA_FACTOR:
   case eCHA_OFFSET:
   case eCHA_MLV:
      //No check necessary
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle signals based on the current change

   \param[in]  oe_Change   Detected UI change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_SendSignalForChange(const C_SdBueSignalPropertiesWidget::E_Change oe_Change)
{
   switch (oe_Change)
   {
   case eCHA_NAME:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      Q_EMIT (this->SigNameChanged(this->mc_MessageId));
      break;
   case eCHA_MUX_TYPE:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      Q_EMIT (this->SigNameChanged(this->mc_MessageId));
      break;
   case eCHA_VALUE_TYPE:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      break;
   case eCHA_START_BIT:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      Q_EMIT (this->SigStartBitChanged(this->mc_MessageId));
      break;
   case eCHA_LENGTH:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      break;
   case eCHA_MUX_VALUE:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      break;
   case eCHA_INIT:
   case eCHA_UNIT:
   case eCHA_COMMENT:
   case eCHA_BYTE_ORDER:
   case eCHA_MIN:
   case eCHA_MAX:
   case eCHA_AUTO_MIN_MAX:
   case eCHA_FACTOR:
   case eCHA_OFFSET:
      //No signal necessary
      break;
   case eCHA_MLV:
      Q_EMIT (this->SigRecheckError(this->mc_MessageId));
      break;
   default:
      break;
   }
   Q_EMIT (this->SigChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Connect all update functions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_ConnectAll(void) const
{
   //Special
   connect(this->mpc_Ui->pc_LineEditName, &QLineEdit::textChanged, this,
           &C_SdBueSignalPropertiesWidget::m_HandleNameChangeWithoutSignal);
   //connects for RegisterChange
   connect(this->mpc_Ui->pc_LineEditName, &QLineEdit::editingFinished, this,
           &C_SdBueSignalPropertiesWidget::m_HandleNameChangeWithSignal);
   connect(this->mpc_Ui->pc_LineEditUnit, &QLineEdit::textChanged, this,
           &C_SdBueSignalPropertiesWidget::m_HandleUnitChange);
   connect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
           &C_SdBueSignalPropertiesWidget::m_HandleCommentChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_DoubleSpinBoxFactor, static_cast<void (QDoubleSpinBox::*)(
                                                                float64)>(&QDoubleSpinBox::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleFactorChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_DoubleSpinBoxOffset, static_cast<void (QDoubleSpinBox::*)(
                                                                float64)>(&QDoubleSpinBox::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleOffsetChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxByteOrder, static_cast<void (QComboBox::*)(
                                                              sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleByteOrderChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxMuxType, static_cast<void (QComboBox::*)(
                                                            sintn)>(&QComboBox::currentIndexChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleMuxTypeChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxMuxValue, static_cast<void (QSpinBox::*)(
                                                            sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleMuxValueChange);
   connect(this->mpc_Ui->pc_CheckBoxAutoMinMax, &C_OgeChxProperties::toggled, this,
           &C_SdBueSignalPropertiesWidget::m_HandleAutoMinMaxCheckBoxChange);
   connect(this->mpc_Ui->pc_WidgetMin, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
           &C_SdBueSignalPropertiesWidget::m_HandleMinChange);
   connect(this->mpc_Ui->pc_WidgetMax, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
           &C_SdBueSignalPropertiesWidget::m_HandleMaxChange);
   connect(this->mpc_Ui->pc_WidgetInit, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
           &C_SdBueSignalPropertiesWidget::m_HandleInitChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                         sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleValueTypeChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxLength, static_cast<void (QSpinBox::*)(
                                                          sintn)>(&QSpinBox::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleValueLengthChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_SpinBoxStartBit, static_cast<void (QSpinBox::*)(
                                                            sintn)>(&C_OgeSpxNumber::valueChanged), this,
           &C_SdBueSignalPropertiesWidget::m_HandleStartBitChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Disconnect all update functions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueSignalPropertiesWidget::m_DisconnectAll(void) const
{
   //Special
   disconnect(this->mpc_Ui->pc_LineEditName, &QLineEdit::textChanged, this,
              &C_SdBueSignalPropertiesWidget::m_HandleNameChangeWithoutSignal);
   //disconnects for UnRegisterChange
   disconnect(this->mpc_Ui->pc_LineEditName, &QLineEdit::editingFinished, this,
              &C_SdBueSignalPropertiesWidget::m_HandleNameChangeWithSignal);
   disconnect(this->mpc_Ui->pc_LineEditUnit, &QLineEdit::textChanged, this,
              &C_SdBueSignalPropertiesWidget::m_HandleUnitChange);
   disconnect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
              &C_SdBueSignalPropertiesWidget::m_HandleCommentChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_DoubleSpinBoxFactor, static_cast<void (QDoubleSpinBox::*)(
                                                                   float64)>(&QDoubleSpinBox::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleFactorChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_DoubleSpinBoxOffset, static_cast<void (QDoubleSpinBox::*)(
                                                                   float64)>(&QDoubleSpinBox::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleOffsetChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxByteOrder, static_cast<void (QComboBox::*)(
                                                                 sintn)>(&QComboBox::currentIndexChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleByteOrderChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxMuxType, static_cast<void (QComboBox::*)(
                                                               sintn)>(&QComboBox::currentIndexChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleMuxTypeChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxMuxValue, static_cast<void (QSpinBox::*)(
                                                               sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleMuxValueChange);
   disconnect(this->mpc_Ui->pc_CheckBoxAutoMinMax, &C_OgeChxProperties::toggled, this,
              &C_SdBueSignalPropertiesWidget::m_HandleAutoMinMaxCheckBoxChange);
   disconnect(this->mpc_Ui->pc_WidgetMin, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
              &C_SdBueSignalPropertiesWidget::m_HandleMinChange);
   disconnect(this->mpc_Ui->pc_WidgetMax, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
              &C_SdBueSignalPropertiesWidget::m_HandleMaxChange);
   disconnect(this->mpc_Ui->pc_WidgetInit, &C_OgeWiSpinBoxGroup::SigValueChanged, this,
              &C_SdBueSignalPropertiesWidget::m_HandleInitChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxType, static_cast<void (QComboBox::*)(
                                                            sintn)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleValueTypeChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxLength, static_cast<void (QSpinBox::*)(
                                                             sintn)>(&QSpinBox::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleValueLengthChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_SpinBoxStartBit, static_cast<void (QSpinBox::*)(
                                                               sintn)>(&C_OgeSpxNumber::valueChanged), this,
              &C_SdBueSignalPropertiesWidget::m_HandleStartBitChange);
}
