//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing information and offering configuration of one chart data series

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QMouseEvent>
#include "stwerrors.h"

#include "C_SyvDaChaDataItemWidget.h"
#include "ui_C_SyvDaChaDataItemWidget.h"

#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_UtiStyleSheets.h"
#include "C_OgeWiUtil.h"
#include "C_SyvUtil.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_OSCNodeDataPoolContentUtil.h"
#include "C_PuiSvHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
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

   \param[in]      ou32_ViewIndex   Index of system view
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaDataItemWidget::C_SyvDaChaDataItemWidget(const uint32 ou32_ViewIndex, QWidget * const opc_Parent) :
   stw_opensyde_gui_elements::C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaChaDataItemWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_DataPoolElementConfigIndex(0U),
   mq_Selected(false),
   mq_Warning(false),
   mq_Error(false),
   mq_Invalid(false),
   mc_ToolTipErrorTextHeading(""),
   mc_ToolTipErrorText(""),
   me_PlaceholderDataPoolType(C_OSCNodeDataPool::eDIAG),
   me_MeasurementValueState(eMEASUREMENT_VAL_DEACTIVATED)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_CheckBoxDataSerieActive->setChecked(true);
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Active", true);

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Transparency", "255");

   // prepare the icons
   // Default icon
   this->mc_IconParameter.load(":/images/system_definition/IconParameter.svg");
   this->mc_IconSignal.load(":/images/system_definition/IconSignal.svg");
   this->mc_IconVariable.load(":/images/system_definition/IconVariable.svg");
   this->mc_IconHALInput.load(":/images/system_definition/NodeEdit/halc/InputSmallActive.svg");
   this->mc_IconHALOutput.load(":/images/system_definition/NodeEdit/halc/OutputSmallActive.svg");
   this->mc_IconHALOther.load(":/images/system_definition/NodeEdit/halc/OtherSmallActive.svg");
   // Inactive icon
   this->mc_IconParameterInactive.load(":/images/system_definition/IconParameterInactive.svg");
   this->mc_IconSignalInactive.load(":/images/system_definition/IconSignalInactive.svg");
   this->mc_IconVariableInactive.load(":/images/system_definition/IconVariableInactive.svg");
   this->mc_IconHALInputInactive.load(":/images/system_definition/NodeEdit/halc/InputSmallInactive.svg");
   this->mc_IconHALOutputInactive.load(":/images/system_definition/NodeEdit/halc/OutputSmallInactive.svg");
   this->mc_IconHALOtherInactive.load(":/images/system_definition/NodeEdit/halc/OtherSmallInactive.svg");
   // Waring icon
   this->mc_IconParameterWarning.load(":/images/system_definition/IconParameterWarning.svg");
   this->mc_IconSignalWarning.load(":/images/system_definition/IconSignalWarning.svg");
   this->mc_IconVariableWarning.load(":/images/system_definition/IconVariableWarning.svg");
   this->mc_IconHALOtherWarning.load(":/images/system_definition/NodeEdit/halc/OtherSmallWarning.svg");
   // Error icon
   this->mc_IconParameterError.load(":/images/system_definition/IconParameterError.svg");
   this->mc_IconSignalError.load(":/images/system_definition/IconSignalError.svg");
   this->mc_IconVariableError.load(":/images/system_definition/IconVariableError.svg");
   this->mc_IconHALInputError.load(":/images/system_definition/NodeEdit/halc/InputSmallError.svg");
   this->mc_IconHALOutputError.load(":/images/system_definition/NodeEdit/halc/OutputSmallError.svg");
   this->mc_IconHALOtherError.load(":/images/system_definition/NodeEdit/halc/OtherSmallError.svg");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaChaDataItemWidget::~C_SyvDaChaDataItemWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes the widget and fills all elements

   \param[in]  ou32_DataPoolElementConfigIndex     Data pool element configuration identification index
   \param[in]  orc_DataPoolElementId               Datapool element identification
   \param[in]  orc_DisplayName                     Name to display
   \param[in]  oq_Active                           Flag if data series is active / visible
   \param[in]  orc_DataPoolElementColor            Associated color to the datapool element
   \param[in]  orc_Unit                            Unit of current scaling
   \param[in]  oq_Warning                          Flag if a warning for this data element was detected
   \param[in]  oq_Invalid                          Flag if data element is invalid and invalid placeholder info
                                                   shall be used
   \param[in]  oe_InvalidPlaceholderDataPoolType   Placeholder datapool type of data element in case of
                                                   invalid data element
   \param[in]  orc_ToolTipErrorTextHeading         Heading of tool tip in case of a warning
   \param[in]  orc_ToolTipErrorText                Text of tool tip in case of a warning
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::InitWidget(const uint32 ou32_DataPoolElementConfigIndex,
                                          const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                          const QString & orc_DisplayName, const bool oq_Active,
                                          const QColor & orc_DataPoolElementColor, const QString & orc_Unit,
                                          const bool oq_Warning, const bool oq_Invalid,
                                          const C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType,
                                          const QString & orc_ToolTipErrorTextHeading,
                                          const QString & orc_ToolTipErrorText)
{
   QString c_Name = orc_DisplayName;
   const C_OSCNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                 orc_DataPoolElementId.u32_DataPoolIndex,
                                                                 orc_DataPoolElementId.u32_ListIndex,
                                                                 orc_DataPoolElementId.u32_ElementIndex);
   const C_OSCNodeDataPool * const pc_Datapool =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_DataPoolElementId.u32_NodeIndex,
                                                      orc_DataPoolElementId.u32_DataPoolIndex);

   this->UpdateColor(orc_DataPoolElementColor);

   this->mc_DataPoolElementId = orc_DataPoolElementId;
   this->mq_Warning = oq_Warning;
   this->mq_Invalid = oq_Invalid;

   if (this->mq_Warning == true)
   {
      this->mc_ToolTipErrorTextHeading = orc_ToolTipErrorTextHeading;
      this->mc_ToolTipErrorText = orc_ToolTipErrorText;

      if (this->mq_Invalid == true)
      {
         this->me_PlaceholderDataPoolType = oe_InvalidPlaceholderDataPoolType;
      }
   }

   // Save all data
   this->mu32_DataPoolElementConfigIndex = ou32_DataPoolElementConfigIndex;

   if (orc_DisplayName.compare("") == 0)
   {
      if (pc_OscElement != NULL)
      {
         if (pc_Datapool != NULL)
         {
            if (pc_Datapool->e_Type == C_OSCNodeDataPool::eHALC)
            {
               c_Name = C_PuiSvHandler::h_GetShortNamespace(orc_DataPoolElementId);
            }
            else
            {
               c_Name = pc_OscElement->c_Name.c_str();
               if (orc_DataPoolElementId.GetUseArrayElementIndex())
               {
                  c_Name = static_cast<QString>("%1[%2]").arg(c_Name).arg(orc_DataPoolElementId.GetArrayElementIndex());
               }
            }
         }
      }
   }

   this->mpc_Ui->pc_LabelDataSerieName->setText(c_Name);

   this->mc_Unit = orc_Unit;
   if (oq_Warning == false)
   {
      QString c_Value = "0";
      if (pc_OscElement != NULL)
      {
         // Set default value
         float64 f64_Value;
         C_OSCNodeDataPoolContent c_Tmp = pc_OscElement->c_MinValue;

         c_Tmp.GetAnyValueAsFloat64(f64_Value, 0);
         if (f64_Value < 0.0)
         {
            // Do not use the minimum
            C_OSCNodeDataPoolContentUtil::h_SetValueInContent(0.0, c_Tmp, 0UL);
         }
         C_SdNdeDpContentUtil::h_GetValueAsScaledString(c_Tmp, pc_OscElement->f64_Factor,
                                                        pc_OscElement->f64_Offset, c_Value, 0UL);
      }
      this->mpc_Ui->pc_LabelValue->setText(c_Value + " " + this->mc_Unit);
   }
   else
   {
      this->mpc_Ui->pc_LabelValue->setVisible(false);
   }

   this->mpc_Ui->pc_CheckBoxDataSerieActive->setChecked(oq_Active);
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Active", oq_Active);

   // Adapt the icon
   this->m_UpdateIcon();

   connect(this->mpc_Ui->pc_CheckBoxDataSerieActive, &QCheckBox::toggled,
           this, &C_SyvDaChaDataItemWidget::m_DataItemToggled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the datapool element configuration index

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element configuration index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::UpdateIndex(const uint32 ou32_DataPoolElementConfigIndex)
{
   this->mu32_DataPoolElementConfigIndex = ou32_DataPoolElementConfigIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the data item

   \param[in]  orc_Color   New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::UpdateColor(const QColor & orc_Color)
{
   // Adapt the color of the color label
   QString c_Style = this->mpc_Ui->pc_LabelColor->styleSheet();

   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, orc_Color);
   this->mpc_Ui->pc_LabelColor->setStyleSheet(c_Style);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the data item

   \param[in]  orc_Value   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::UpdateValue(const QString & orc_Value)
{
   this->mc_CurrentValue = orc_Value;

   if (this->me_MeasurementValueState == eMEASUREMENT_VAL_DEACTIVATED)
   {
      this->mpc_Ui->pc_LabelValue->setText(orc_Value + " " + this->mc_Unit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the measurement value state

   eMEASUREMENT_VAL_DEACTIVATED:    The last received value will be shown
   eMEASUREMENT_VAL_FIRST           The measurement value of the first cursor will be shown
   eMEASUREMENT_VAL_SECOND          The measurement value of the second cursor will be shown

   \param[in]  oe_MeasurementValueState   Selecting which measurement value has to be shown.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::SetMeasurementState(
   const C_SyvDaChaDataItemWidget::E_MeasurementValueState oe_MeasurementValueState)
{
   const C_SyvDaChaDataItemWidget::E_MeasurementValueState e_PreviousState = this->me_MeasurementValueState;

   // Save the new state
   this->me_MeasurementValueState = oe_MeasurementValueState;

   if (this->me_MeasurementValueState == eMEASUREMENT_VAL_DEACTIVATED)
   {
      // In case of an changed measurement state to deactivated, the shown value must be adapted to last received value
      this->UpdateValue(this->mc_CurrentValue);
   }
   else if ((e_PreviousState == eMEASUREMENT_VAL_SECOND) &&
            (this->me_MeasurementValueState == eMEASUREMENT_VAL_FIRST))
   {
      // Special case. The second cursor is not clicked and dragged anymore, show the first cursor value again
      this->UpdateMeasurementValue(eMEASUREMENT_VAL_FIRST, this->mc_MeasurementValue1);
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates one of the shown measurement value of the data item

   \param[in]  oe_MeasurementValueVariant    Selecting which measurement value has to be set.
                                             eMEASUREMENT_VAL_DEACTIVATED is ignored here
   \param[in]  orc_Value                     New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::UpdateMeasurementValue(const E_MeasurementValueState oe_MeasurementValueVariant,
                                                      const QString & orc_Value)
{
   switch (oe_MeasurementValueVariant)
   {
   case eMEASUREMENT_VAL_FIRST:
      this->mc_MeasurementValue1 = orc_Value;
      if (this->me_MeasurementValueState == oe_MeasurementValueVariant)
      {
         this->mpc_Ui->pc_LabelValue->setText(orc_Value + " " + this->mc_Unit + " (t1)");
      }
      break;
   case eMEASUREMENT_VAL_SECOND:
      this->mc_MeasurementValue2 = orc_Value;
      if (this->me_MeasurementValueState == oe_MeasurementValueVariant)
      {
         this->mpc_Ui->pc_LabelValue->setText(orc_Value + " " + this->mc_Unit + " (t2)");
      }
      break;

   case eMEASUREMENT_VAL_DEACTIVATED: // Nothing to do her like in default case
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   \param[in]  osn_Value   Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::UpdateTransparence(const sintn osn_Value) const
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelValue, "Transparency",
                                          QString::number(osn_Value));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Error update for data element

   \param[in]  orc_ErrorText           Error description
   \param[in]  oq_IsTransmissionError  Flag if transmission error occurred
   \param[in]  oq_ErrorActive          Flag if error is active or should be cleared
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::UpdateError(const QString & orc_ErrorText, const bool oq_IsTransmissionError,
                                           const bool oq_ErrorActive)
{
   this->mq_Error = oq_ErrorActive;
   this->mc_ToolTipErrorText = orc_ErrorText;
   if (oq_IsTransmissionError == true)
   {
      this->mc_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Transmission error");
   }
   else
   {
      this->mc_ToolTipErrorTextHeading = C_GtGetText::h_GetText("Signal error");
   }

   // Adapt the icon
   this->m_UpdateIcon();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resets the error state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::ResetError(void)
{
   if (this->mq_Error == true)
   {
      this->mq_Error = false;
      this->mc_ToolTipErrorText = "";
      this->mc_ToolTipErrorTextHeading = "";

      // Adapt the icon
      this->m_UpdateIcon();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the unit of the datapool element

   \return
   Element name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaChaDataItemWidget::GetDataElementName(void) const
{
   return this->mpc_Ui->pc_LabelDataSerieName->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the unit of the datapool element

   \return
   Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaChaDataItemWidget::GetDataElementUnit(void) const
{
   return this->mc_Unit;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the toggled state if the element is active or not

   \retval   true    Element is enabled
   \retval   false   Element is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataItemWidget::GetDataElementToggledState(void) const
{
   return this->mpc_Ui->pc_CheckBoxDataSerieActive->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new unit

   \param[in]  orc_DisplayName   Display name
   \param[in]  orc_Unit          Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::SetDataElementUnit(const QString & orc_DisplayName, const QString & orc_Unit)
{
   this->mc_Unit = orc_Unit;
   this->mpc_Ui->pc_LabelDataSerieName->setText(orc_DisplayName);
   this->mpc_Ui->pc_LabelValue->setText("N/A " + this->mc_Unit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the data element widget selection

   \param[in]  oq_Selected    Flag if selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::SetDataElementSelected(const bool oq_Selected)
{
   this->mq_Selected = oq_Selected;

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Selected", oq_Selected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaChaDataItemWidget::event(QEvent * const opc_Event)
{
   // Update tool tip
   if (opc_Event->type() == QEvent::ToolTip)
   {
      QString c_ToolTip;
      QString c_ToolTipHeading = "";
      stw_opensyde_gui::C_NagToolTip::E_Type e_Type = C_NagToolTip::eDEFAULT;

      if ((this->mq_Warning == true) || (this->mq_Error == true))
      {
         c_ToolTip = this->mc_ToolTipErrorText;
         c_ToolTipHeading = this->mc_ToolTipErrorTextHeading;

         if (this->mq_Error == true)
         {
            e_Type = C_NagToolTip::eERROR;
         }
         else
         {
            e_Type = C_NagToolTip::eWARNING;
         }
      }
      else
      {
         //get element name as heading
         if (C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mc_DataPoolElementId) != NULL)
         {
            const QString c_Name =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mc_DataPoolElementId)->c_Name.
               c_str();
            if (this->mc_DataPoolElementId.GetUseArrayElementIndex())
            {
               c_ToolTipHeading = static_cast<QString>("%1[%2]").arg(c_Name).arg(
                  this->mc_DataPoolElementId.GetArrayElementIndex());
            }
            else
            {
               c_ToolTipHeading = c_Name;
            }
         }

         c_ToolTip = C_SyvUtil::h_GetCommonDashboardItemToolTip(this->mu32_ViewIndex,
                                                                this->mc_DataPoolElementId);
      }

      this->SetToolTipInformation(c_ToolTipHeading, c_ToolTip, e_Type);
   }

   return C_OgeWiWithToolTip::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);
   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden mouse press event

   Selecting or unselecting the widget

   \param[in,out]  opc_Event  Pointer to mouse event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::mousePressEvent(QMouseEvent * const opc_Event)
{
   if ((this->mq_Selected == false) &&
       ((opc_Event->buttons() == static_cast<sintn>(Qt::LeftButton)) ||
        (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton))))
   {
      Q_EMIT (this->SigDataItemSelected(this->mu32_DataPoolElementConfigIndex));

      if (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton))
      {
         // Context menu shall be available anyway
         QWidget::mousePressEvent(opc_Event);
      }
   }
   else
   {
      QWidget::mousePressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::m_DataItemToggled(const bool oq_Checked)
{
   this->m_UpdateIcon();

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Active", oq_Checked);

   Q_EMIT (this->SigDataItemToggled(this->mu32_DataPoolElementConfigIndex, oq_Checked));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaChaDataItemWidget::m_UpdateIcon(void) const
{
   stw_opensyde_core::C_OSCNodeDataPool::E_Type e_Type = stw_opensyde_core::C_OSCNodeDataPool::eDIAG;
   const bool q_Inacitve = !this->mpc_Ui->pc_CheckBoxDataSerieActive->isChecked();

   if (this->mq_Invalid == false)
   {
      // Get the datapool type if the datapool element is valid
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mc_DataPoolElementId.u32_NodeIndex, this->mc_DataPoolElementId.u32_DataPoolIndex);

      if (pc_DataPool != NULL)
      {
         e_Type = pc_DataPool->e_Type;
      }
   }
   else
   {
      e_Type = this->me_PlaceholderDataPoolType;
   }

   // Adapt the icon
   if (e_Type == C_OSCNodeDataPool::eDIAG)
   {
      // Diag datapool
      if (q_Inacitve == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconVariableInactive);
      }
      else if (this->mq_Error == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconVariableError);
      }
      else if (this->mq_Warning == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconVariableWarning);
      }
      else
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconVariable);
      }
   }
   else if (e_Type == C_OSCNodeDataPool::eNVM)
   {
      // NVM datapool
      if (q_Inacitve == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconParameterInactive);
      }
      else if (this->mq_Error == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconParameterError);
      }
      else if (this->mq_Warning == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconParameterWarning);
      }
      else
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconParameter);
      }
   }
   else if (e_Type == C_OSCNodeDataPool::eCOM)
   {
      // COMM datapool
      if (q_Inacitve == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconSignalInactive);
      }
      else if (this->mq_Error == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconSignalError);
      }
      else if (this->mq_Warning == true)
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconSignalWarning);
      }
      else
      {
         this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconSignal);
      }
   }
   else
   {
      // HAL datapool
      const C_OSCHalcDefDomain::E_Category e_Category = C_PuiSdHandler::h_GetInstance()->GetDomainCategoryFromDpId(
         this->mc_DataPoolElementId, this->mc_DataPoolElementId.GetArrayElementIndexOrZero());

      switch (e_Category)
      {
      case C_OSCHalcDefDomain::eCA_INPUT:
         if (q_Inacitve == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALInputInactive);
         }
         else if (this->mq_Error == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALInputError);
         }
         else if (this->mq_Warning == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOtherWarning);
         }
         else
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALInput);
         }
         break;
      case C_OSCHalcDefDomain::eCA_OUTPUT:
         if (q_Inacitve == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOutputInactive);
         }
         else if (this->mq_Error == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOutputError);
         }
         else if (this->mq_Warning == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOtherWarning);
         }
         else
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOutput);
         }
         break;
      case C_OSCHalcDefDomain::eCA_OTHER:
         if (q_Inacitve == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOtherInactive);
         }
         else if (this->mq_Error == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOtherError);
         }
         else if (this->mq_Warning == true)
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOtherWarning);
         }
         else
         {
            this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOther);
         }
         break;
      default:
         // Nothing to do
         break;
      }
   }
}
