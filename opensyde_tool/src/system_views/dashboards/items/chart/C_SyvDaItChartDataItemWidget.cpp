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

#include "C_SyvDaItChartDataItemWidget.h"
#include "ui_C_SyvDaItChartDataItemWidget.h"

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
C_SyvDaItChartDataItemWidget::C_SyvDaItChartDataItemWidget(const uint32 ou32_ViewIndex, QWidget * const opc_Parent) :
   stw_opensyde_gui_elements::C_OgeWiWithToolTip(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItChartDataItemWidget),
   mu32_ViewIndex(ou32_ViewIndex),
   mq_Selected(false),
   mq_Warning(false),
   mq_Error(false),
   mq_Invalid(false),
   mc_ToolTipErrorTextHeading(""),
   mc_ToolTipErrorText(""),
   me_PlaceholderDataPoolType(C_OSCNodeDataPool::eDIAG)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_CheckBoxDataSerieActive->setChecked(true);

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "Transparency", "255");

   // prepare the icons
   // Default icon
   this->mc_IconParameter.load(":/images/system_definition/IconParameter.svg");
   this->mc_IconSignal.load(":/images/system_definition/IconSignal.svg");
   this->mc_IconVariable.load(":/images/system_definition/IconVariable.svg");
   this->mc_IconHALInput.load(":/images/system_definition/NodeEdit/halc/InputSmallActive.svg");
   this->mc_IconHALOutput.load(":/images/system_definition/NodeEdit/halc/OutputSmallActive.svg");
   this->mc_IconHALOther.load(":/images/system_definition/NodeEdit/halc/OtherSmallActive.svg");
   // Waring icon
   this->mc_IconParameterWarning.load(":/images/system_definition/IconParameterWarning.svg");
   this->mc_IconSignalWarning.load(":/images/system_definition/IconSignalWarning.svg");
   this->mc_IconVariableWarning.load(":/images/system_definition/IconVariableWarning.svg");
   this->mc_IconHALInputWarning.load(":/images/system_definition/InputSmallInactive.svg");
   this->mc_IconHALOutputWarning.load(":/images/system_definition/OutputSmallInactive.svg");
   this->mc_IconHALOtherWarning.load(":/images/system_definition/OtherSmallInactive.svg");
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
C_SyvDaItChartDataItemWidget::~C_SyvDaItChartDataItemWidget()
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
void C_SyvDaItChartDataItemWidget::InitWidget(const uint32 ou32_DataPoolElementConfigIndex,
                                              const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
                                              const QString & orc_DisplayName, const bool oq_Active,
                                              const QColor & orc_DataPoolElementColor, const QString & orc_Unit,
                                              const bool oq_Warning, const bool oq_Invalid,
                                              const C_OSCNodeDataPool::E_Type oe_InvalidPlaceholderDataPoolType,
                                              const QString & orc_ToolTipErrorTextHeading,
                                              const QString & orc_ToolTipErrorText)
{
   QString c_Name = orc_DisplayName;
   // Adapt the color of the color label
   QString c_Style = this->mpc_Ui->pc_LabelColor->styleSheet();
   const C_OSCNodeDataPoolListElement * const pc_OscElement =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_DataPoolElementId.u32_NodeIndex,
                                                                 orc_DataPoolElementId.u32_DataPoolIndex,
                                                                 orc_DataPoolElementId.u32_ListIndex,
                                                                 orc_DataPoolElementId.u32_ElementIndex);
   const C_OSCNodeDataPool * pc_Datapool =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_DataPoolElementId.u32_NodeIndex,
                                                      orc_DataPoolElementId.u32_DataPoolIndex);

   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, orc_DataPoolElementColor);

   this->mpc_Ui->pc_LabelColor->setStyleSheet(c_Style);

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

   // Adapt the icon
   this->m_UpdateIcon();

   this->mpc_Ui->pc_CheckBoxDataSerieActive->setChecked(oq_Active);

   connect(this->mpc_Ui->pc_CheckBoxDataSerieActive, &QCheckBox::toggled,
           this, &C_SyvDaItChartDataItemWidget::m_DataItemToggled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the datapool element configuration index

   \param[in]  ou32_DataPoolElementConfigIndex  Datapool element configuration index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataItemWidget::UpdateIndex(const uint32 ou32_DataPoolElementConfigIndex)
{
   this->mu32_DataPoolElementConfigIndex = ou32_DataPoolElementConfigIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the data item

   \param[in]  orc_Value   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataItemWidget::UpdateValue(const QString & orc_Value) const
{
   this->mpc_Ui->pc_LabelValue->setText(orc_Value + " " + this->mc_Unit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of the color transparency value configured by the actual timeout state

   \param[in]  osn_Value   Value for transparency (0..255)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataItemWidget::UpdateTransparence(const sintn osn_Value) const
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
void C_SyvDaItChartDataItemWidget::UpdateError(const QString & orc_ErrorText, const bool oq_IsTransmissionError,
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
void C_SyvDaItChartDataItemWidget::ResetError(void)
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
QString C_SyvDaItChartDataItemWidget::GetDataElementName(void) const
{
   return this->mpc_Ui->pc_LabelDataSerieName->text();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the unit of the datapool element

   \return
   Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItChartDataItemWidget::GetDataElementUnit(void) const
{
   return this->mc_Unit;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the toggled state if the element is active or not

   \retval   true    Element is enabled
   \retval   false   Element is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaItChartDataItemWidget::GetDataElementToggledState(void) const
{
   return this->mpc_Ui->pc_CheckBoxDataSerieActive->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a new unit

   \param[in]  orc_DisplayName   Display name
   \param[in]  orc_Unit          Element unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataItemWidget::SetDataElementUnit(const QString & orc_DisplayName, const QString & orc_Unit)
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
void C_SyvDaItChartDataItemWidget::SetDataElementSelected(const bool oq_Selected)
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
bool C_SyvDaItChartDataItemWidget::event(QEvent * const opc_Event)
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
               c_ToolTipHeading = static_cast<QString>("%1[%2]").arg(c_Name).arg(this->mc_DataPoolElementId.GetArrayElementIndex());
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
void C_SyvDaItChartDataItemWidget::paintEvent(QPaintEvent * const opc_Event)
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
void C_SyvDaItChartDataItemWidget::mousePressEvent(QMouseEvent * const opc_Event)
{
   if ((this->mq_Selected == false) &&
       (this->mpc_Ui->pc_CheckBoxDataSerieActive->isChecked() == true) &&
       ((opc_Event->buttons() == static_cast<sintn>(Qt::LeftButton)) ||
        (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton))))
   {
      Q_EMIT this->SigDataItemSelected(this->mu32_DataPoolElementConfigIndex);

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
void C_SyvDaItChartDataItemWidget::m_DataItemToggled(const bool oq_Checked)
{
   Q_EMIT (this->SigDataItemToggled(this->mu32_DataPoolElementConfigIndex, oq_Checked));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItChartDataItemWidget::m_UpdateIcon(void) const
{
   stw_opensyde_core::C_OSCNodeDataPool::E_Type e_Type = stw_opensyde_core::C_OSCNodeDataPool::eDIAG;

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
      if (this->mq_Error == true)
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
      if (this->mq_Error == true)
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
      if (this->mq_Error == true)
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
      uint32 u32_DomainIndex;
      bool q_UseChannelIndex;
      uint32 u32_ChannelIndex;

      C_OSCHalcDefDomain::E_VariableSelector e_Selector;
      uint32 u32_ParameterIndex;
      bool q_UseElementIndex;
      uint32 u32_ParameterElementIndex;
      bool q_IsUseCaseIndex;
      bool q_IsChanNumIndex;

      if (C_PuiSdHandler::h_GetInstance()->TranslateToHALCIndex(this->mc_DataPoolElementId,
                                                                this->mc_DataPoolElementId.GetArrayElementIndexOrZero(),
                                                                u32_DomainIndex, q_UseChannelIndex,
                                                                u32_ChannelIndex, e_Selector, u32_ParameterIndex,
                                                                q_UseElementIndex, u32_ParameterElementIndex,
                                                                q_IsUseCaseIndex, q_IsChanNumIndex) == C_NO_ERR)
      {
         const C_OSCHalcDefDomain * const pc_Domain =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileDataConst(this->mc_DataPoolElementId.u32_NodeIndex,
                                                                        u32_DomainIndex);
         if (pc_Domain->c_Name == "Inputs")
         {
            // Inputs
            if (this->mq_Error == true)
            {
               this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALInputError);
            }
            else if (this->mq_Warning == true)
            {
               this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALInputWarning);
            }
            else
            {
               this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALInput);
            }
         }
         else if (pc_Domain->c_Name == "Outputs")
         {
            // Outputs
            if (this->mq_Error == true)
            {
               this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOutputError);
            }
            else if (this->mq_Warning == true)
            {
               this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOutputWarning);
            }
            else
            {
               this->mpc_Ui->pc_LabelIcon->setPixmap(this->mc_IconHALOutput);
            }
         }
         else
         {
            // Others
            if (this->mq_Error == true)
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
         }
      }
   }
}
