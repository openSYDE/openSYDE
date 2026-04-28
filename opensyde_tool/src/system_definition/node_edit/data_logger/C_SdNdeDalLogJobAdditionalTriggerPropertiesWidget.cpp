//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for log job additional trigger properties configuration

   Widget for log job additional trigger properties configuration

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolContentUtil.hpp"
#include "C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget.hpp"
#include "ui_C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget.h"
#include "C_OgeWiUtil.hpp"
#include "C_SdNdeDalLogJobAdditionalTriggerDialog.hpp"
#include "C_SyvDaPeDataElementBrowse.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget(
   QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget),
   me_ConnectState(eCS_DISCONNECTED),
   mu32_NodeIndex(0UL),
   mu32_DataLoggerJobIndex(0UL)

{
   this->mpc_Ui->setupUi(this);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditDataElement, "NoRightBorder", true);
   this->mpc_Ui->pc_LineEditDataElement->setReadOnly(true);

   this->InitStaticNames();
   this->m_InitSupportedOperations();
   this->m_InitSupportedOperationsComboBox();

   connect(this->mpc_Ui->pc_PushButtonDataElement, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_BrowseDataElement);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::~C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save()
{
   const C_OscDataLoggerJob * const pc_DataLoggerJob = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex,
      this->mu32_DataLoggerJobIndex);

   if (pc_DataLoggerJob != NULL)
   {
      C_OscDataLoggerJobAdditionalTriggerProperties c_NewValues =
         pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties;

      c_NewValues.q_Enable =
         this->mpc_Ui->pc_ChkBoxAdditionalTrigger->isChecked();
      c_NewValues.c_ElementId = m_GetSelectedDataElement();
      c_NewValues.c_Operation = m_GetOperationForCore().toStdString().c_str();

      // Apply threshold value
      m_ApplyContentValue(c_NewValues.c_Threshold);

      C_PuiSdHandler::h_GetInstance()->SetDataLoggerAdditionalTriggerProperties(mu32_NodeIndex,
                                                                                mu32_DataLoggerJobIndex,
                                                                                c_NewValues);
      if (this->me_ConnectState == eCS_CONNECTED)
      {
         C_PuiSdHandler::h_GetInstance()->SetDataLoggerAdditionalTriggerExpertModeEnabled(mu32_NodeIndex,
                                                                                          mu32_DataLoggerJobIndex,
                                                                                          true);
      }
      else
      {
         C_PuiSdHandler::h_GetInstance()->SetDataLoggerAdditionalTriggerExpertModeEnabled(mu32_NodeIndex,
                                                                                          mu32_DataLoggerJobIndex,
                                                                                          false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::InitStaticNames() const
{
   this->mpc_Ui->pc_ChkBoxAdditionalTrigger->setText(C_GtGetText::h_GetText("Enabled"));
   this->mpc_Ui->pc_ChkBoxAdditionalTrigger->SetToolTipInformation(C_GtGetText::h_GetText("Additional Trigger"),
                                                                   C_GtGetText::h_GetText(
                                                                      "Activate additional data trigger for logging"));
   this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("Data Element"));
   this->mpc_Ui->pc_LabelDataElement->SetToolTipInformation(C_GtGetText::h_GetText("Data Element"),
                                                            C_GtGetText::h_GetText(
                                                               "Which data element changes should be used to trigger logging"));
   this->mpc_Ui->pc_LabelOperation->setText(C_GtGetText::h_GetText("Operation"));
   this->mpc_Ui->pc_LabelOperation->SetToolTipInformation(C_GtGetText::h_GetText("Operation"),
                                                          C_GtGetText::h_GetText(
                                                             "Which operation should be used for the threshold to trigger logging"));
   this->mpc_Ui->pc_LabelThreshold->setText(C_GtGetText::h_GetText("Threshold"));
   this->mpc_Ui->pc_LabelThreshold->SetToolTipInformation(C_GtGetText::h_GetText("Threshold"),
                                                          C_GtGetText::h_GetText(
                                                             "Threshold for configured data element to reach to trigger logging"));

   this->mpc_Ui->pc_LabelExpertView->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelExpertView->SetFontPixel(12, false, false);
   this->mpc_Ui->pc_LabelExpertView->setText(C_GtGetText::h_GetText("Expert View"));
   this->mpc_Ui->pc_LabelCondition->setText(C_GtGetText::h_GetText("Condition"));
   this->mpc_Ui->pc_PubEdit->setText(C_GtGetText::h_GetText("Edit"));
   this->mpc_Ui->pc_PushButtonDataElement->setText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LineEditDataElement->setText(C_GtGetText::h_GetText(""));

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PubEdit, "Edit_Hyperlink", true);
   this->mpc_Ui->pc_PubEdit->setCursor(Qt::PointingHandCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node data logger indices

   \param[in]  ou32_NodeIndex             Index of node
   \param[in]  ou32_DataLoggerJobIndex    Data logger job index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex,
                                                                             const uint32_t ou32_DataLoggerJobIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mu32_DataLoggerJobIndex = ou32_DataLoggerJobIndex;
   const auto & rc_Expert =
      C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                        mu32_DataLoggerJobIndex)->c_Properties.
      c_AdditionalTriggerProperties.c_ExpertMode;
   if (rc_Expert.q_Enable == true)
   {
      this->me_ConnectState = eCS_CONNECTED;
      this->mpc_Ui->pc_PbExpertView->SetSvg("://images/ToggleOnMsgTransmission.svg");
      if ((rc_Expert.c_TriggerConfiguration.IsEmpty() == true) || (rc_Expert.c_TriggerConfiguration == " "))
      {
         this->mpc_Ui->pc_LabelConditionExp->setText("Empty condition");
      }
      else
      {
         m_SetExpertTriggerCondition();
      }
      this->mpc_Ui->pc_WidgetCondition->setVisible(true);
      this->mpc_Ui->pc_DataWidget->setVisible(false);
   }
   else
   {
      this->me_ConnectState = eCS_DISCONNECTED;
      this->mpc_Ui->pc_PbExpertView->SetSvg("://images/ToggleOffMsgTransmission.svg");
      this->mpc_Ui->pc_WidgetCondition->setVisible(false);
      this->mpc_Ui->pc_DataWidget->setVisible(true);
   }
   this->mpc_Ui->pc_PbExpertView->update();
   this->Reload();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Reload()
{
   const C_OscDataLoggerJob * const pc_DataLoggerJob = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex,
      this->mu32_DataLoggerJobIndex);

   m_DisconnectChangeTriggers();

   if (pc_DataLoggerJob != NULL)
   {
      //Set content first
      this->mpc_Ui->pc_ChkBoxAdditionalTrigger->setChecked(
         pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.q_Enable);
      m_InitDataElement(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId);
      m_SetOperation(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_Operation.c_str());
      m_InitThreshold(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId,
                      pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_Threshold);
      m_InitUnit(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId);

      m_HandleOperationEnabledState(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.q_Enable);
      m_HandleEditFieldsEnabledState(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.q_Enable);
   }

   m_ReconnectChangeTriggers();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten resize event slot

   \param[in,out]   opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   const auto & rc_Expert =
      C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                        mu32_DataLoggerJobIndex)->c_Properties.
      c_AdditionalTriggerProperties.c_ExpertMode;

   if (rc_Expert.q_Enable == true)
   {
      if ((rc_Expert.c_TriggerConfiguration.IsEmpty() == true) || (rc_Expert.c_TriggerConfiguration == " "))
      {
         this->mpc_Ui->pc_LabelConditionExp->setText("Empty condition");
      }
      else
      {
         m_SetExpertTriggerCondition();
      }
   }
   QWidget::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle operation enabled state

   \param[in]  oq_AdditionalTriggerEnabled   Additional trigger enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_HandleOperationEnabledState(
   const bool oq_AdditionalTriggerEnabled) const
{
   bool q_Enabled = false;

   if ((m_CheckSelectableDataElementAvailable() && (m_CurrentElementIsString() == false)) &&
       oq_AdditionalTriggerEnabled)
   {
      q_Enabled = true;
   }

   this->mpc_Ui->pc_ComboBoxOperation->setEnabled(q_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle edit fields enabled state

   \param[in]  oq_Enabled  Enabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_HandleEditFieldsEnabledState(const bool oq_Enabled) const
{
   this->mpc_Ui->pc_LineEditDataElement->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_PushButtonDataElement->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_WidgetThreshold->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_LineEditThreshold->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_PubEdit->setEnabled(oq_Enabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init supported operations
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitSupportedOperations()
{
   this->mc_MapCoreOperationToUi.clear();
   this->mc_MapCoreOperationToUi.emplace_back(std::pair<QString, QString>("<",
                                                                          C_GtGetText::h_GetText("<  (smaller than)")));
   this->mc_MapCoreOperationToUi.emplace_back(std::pair<QString, QString>("<=",
                                                                          C_GtGetText::h_GetText(
                                                                             "<= (smaller than or equal)")));
   this->mc_MapCoreOperationToUi.emplace_back(std::pair<QString, QString>("==",
                                                                          C_GtGetText::h_GetText("== (equal to)")));
   this->mc_MapCoreOperationToUi.emplace_back(std::pair<QString, QString>(">",
                                                                          C_GtGetText::h_GetText(">  (greater than)")));
   this->mc_MapCoreOperationToUi.emplace_back(std::pair<QString, QString>(">=",
                                                                          C_GtGetText::h_GetText(
                                                                             ">= (greater than or equal)")));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init supported operations combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitSupportedOperationsComboBox() const
{
   this->mpc_Ui->pc_ComboBoxOperation->clear();
   for (std::vector<std::pair<QString, QString> >::const_iterator c_It = this->mc_MapCoreOperationToUi.cbegin();
        c_It != this->mc_MapCoreOperationToUi.cend(); ++c_It)
   {
      this->mpc_Ui->pc_ComboBoxOperation->addItem(c_It->second);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init data elements

   \param[in]  orc_Id   Id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitDataElement(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   const C_OscDataLoggerJob * const pc_DataLoggerJob = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex,
      this->mu32_DataLoggerJobIndex);

   if (pc_DataLoggerJob != NULL)
   {
      const C_OscNodeDataPoolListElementOptArrayOptValidId & rc_ElementId =
         pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId;

      // Ensure the data element is valid / still exists
      if ((orc_Id == rc_ElementId) && (rc_ElementId.GetIsValid()))
      {
         const QString c_ElementName = C_PuiSdUtil::h_GetNamespaceDatapoolElement(orc_Id);
         this->m_SetSelectedDataElement(c_ElementName, orc_Id);
      }
      // data element with invalid Id (e.g. when a new log job is added or the trigger condition is cleared while
      // switching from expert to simple view)
      else
      {
         this->m_SetSelectedDataElement("", orc_Id);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init threshold

   \param[in]  orc_Id         Id
   \param[in]  orc_Content    Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitThreshold(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Id, const C_OscNodeDataPoolContent & orc_Content)
{
   const C_OscNodeDataPoolListElement * const pc_El =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id);

   if (pc_El != NULL)
   {
      if (pc_El->q_InterpretAsString)
      {
         this->mpc_Ui->pc_WidgetThreshold->setVisible(false);
         this->mpc_Ui->pc_LineEditThreshold->setVisible(true);
         this->mpc_Ui->pc_LineEditThreshold->setText(C_SdNdeDpContentUtil::h_ConvertToString(orc_Content));
         this->mpc_Ui->pc_LineEditThreshold->setMaxLength(static_cast<int32_t>(orc_Content.GetArraySize()) - 1);
      }
      else
      {
         this->mpc_Ui->pc_WidgetThreshold->setVisible(true);
         this->mpc_Ui->pc_LineEditThreshold->setVisible(false);
         m_InitNonStringThreshold(*pc_El, orc_Content, orc_Id.GetArrayElementIndexOrZero());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init non string threshold

   \param[in]  orc_Element    Element
   \param[in]  orc_Content    Content
   \param[in]  ou32_Index     Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitNonStringThreshold(
   const C_OscNodeDataPoolListElement & orc_Element, const C_OscNodeDataPoolContent & orc_Content,
   const uint32_t ou32_Index)
{
   QVariant c_VariantValue;

   this->mpc_Ui->pc_WidgetThreshold->Init(orc_Element.c_MinValue, orc_Element.c_MaxValue, orc_Element.f64_Factor,
                                          orc_Element.f64_Offset, ou32_Index);

   c_VariantValue = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(orc_Content, orc_Element.f64_Factor,
                                                                          orc_Element.f64_Offset, ou32_Index);
   this->mpc_Ui->pc_WidgetThreshold->SetValue(c_VariantValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init unit

   \param[in]  orc_Id   Id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitUnit(const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
const
{
   const C_OscNodeDataPoolListElement * const pc_El =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id);

   if (pc_El != NULL)
   {
      this->mpc_Ui->pc_WidgetThreshold->SetSuffix(pc_El->c_Unit.c_str());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get selected data element

   \return
   Selected data element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolListElementOptArrayOptValidId C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::
m_GetSelectedDataElement()
const
{
   return this->mc_SelectedOptArrayId;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply content value

   \param[in,out]  orc_Content   Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ApplyContentValue(C_OscNodeDataPoolContent & orc_Content)
const
{
   if (m_CurrentElementIsString() == true)
   {
      orc_Content = m_GetCurrentMinValue();
      C_SdNdeDpContentUtil::h_SetString(this->mpc_Ui->pc_LineEditThreshold->text(), orc_Content);
   }
   else
   {
      const QVariant c_VariantValue = this->mpc_Ui->pc_WidgetThreshold->GetValue();
      tgl_assert(C_SdNdeDpContentUtil::h_SimpleConvertFromVariant(c_VariantValue, orc_Content) == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Additional trigger enabled changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_AdditionalTriggerEnabledChanged()
{
   m_HandleOperationEnabledState(this->mpc_Ui->pc_ChkBoxAdditionalTrigger->isChecked());
   m_HandleEditFieldsEnabledState(this->mpc_Ui->pc_ChkBoxAdditionalTrigger->isChecked());
   if (this->mpc_Ui->pc_ChkBoxAdditionalTrigger->isChecked())
   {
      //Triggers save
      m_DataElementChanged();
   }
   else
   {
      Save();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Data element changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DataElementChanged()
{
   const C_OscNodeDataPoolListElementOptArrayOptValidId c_NewId = m_GetSelectedDataElement();

   if (c_NewId.GetIsValid() == true)
   {
      const C_OscNodeDataPoolListElement * const pc_El =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_NewId);

      if (pc_El != NULL)
      {
         C_OscNodeDataPoolContent c_Tmp = pc_El->c_MinValue;
         // set available initial value
         C_OscNodeDataPoolContentUtil::h_SetValueInContent(0.0, c_Tmp);
         if (pc_El->q_InterpretAsString == false)
         {
            C_OscNodeDataPoolContentUtil::E_ValueChangedTo e_ValueChangedTo;
            // set value in range, leave initial value if in range
            C_OscNodeDataPoolContentUtil::h_SetValueInMinMaxRange(pc_El->c_MinValue,
                                                                  pc_El->c_MaxValue,
                                                                  c_Tmp,
                                                                  e_ValueChangedTo,
                                                                  C_OscNodeDataPoolContentUtil::eTO_ZERO);
         }
         else
         {
            m_SetOperation("==");
         }
         m_HandleOperationEnabledState(this->mpc_Ui->pc_ChkBoxAdditionalTrigger->isChecked());
         m_InitThreshold(c_NewId, c_Tmp);
         m_InitUnit(c_NewId);
      }
      Save();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set operation

   \param[in]  orc_NewValue   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_SetOperation(const QString & orc_NewValue)
{
   bool q_Found = false;

   for (std::vector<std::pair<QString, QString> >::const_iterator c_It = this->mc_MapCoreOperationToUi.cbegin();
        c_It != this->mc_MapCoreOperationToUi.cend(); ++c_It)
   {
      if (c_It->first.compare(orc_NewValue) == 0)
      {
         this->mpc_Ui->pc_ComboBoxOperation->setCurrentIndex(this->mpc_Ui->pc_ComboBoxOperation->findText(c_It->second));

         q_Found = true;
         break;
      }
   }

   // No matching operation found. Reset the combobox index.
   if (!q_Found)
   {
      this->mpc_Ui->pc_ComboBoxOperation->setCurrentIndex(-1);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Disconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DisconnectChangeTriggers() const
{
   disconnect(this->mpc_Ui->pc_ChkBoxAdditionalTrigger, &QCheckBox::checkStateChanged,
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_AdditionalTriggerEnabledChanged);
   disconnect(this->mpc_Ui->pc_ComboBoxOperation,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   disconnect(this->mpc_Ui->pc_LineEditDataElement, &QLineEdit::textChanged, this,
              &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DataElementChanged);
   disconnect(this->mpc_Ui->pc_WidgetThreshold, &C_OgeWiSpinBoxGroup::SigValueChanged,
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   disconnect(this->mpc_Ui->pc_LineEditThreshold, &QLineEdit::textChanged,
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   disconnect(this->mpc_Ui->pc_PbExpertView, &QPushButton::clicked, this,
              &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ToggleExpertView);
   disconnect(this->mpc_Ui->pc_PubEdit, &QPushButton::clicked, this,
              &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_OnEditClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ReconnectChangeTriggers() const
{
   connect(this->mpc_Ui->pc_ChkBoxAdditionalTrigger, &QCheckBox::checkStateChanged,
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_AdditionalTriggerEnabledChanged);
   connect(this->mpc_Ui->pc_ComboBoxOperation,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   connect(this->mpc_Ui->pc_LineEditDataElement, &QLineEdit::textChanged, this,
           &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DataElementChanged);
   connect(this->mpc_Ui->pc_WidgetThreshold, &C_OgeWiSpinBoxGroup::SigValueChanged,
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   connect(this->mpc_Ui->pc_LineEditThreshold, &QLineEdit::textChanged,
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   connect(this->mpc_Ui->pc_PbExpertView, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ToggleExpertView);
   connect(this->mpc_Ui->pc_PubEdit, &QPushButton::clicked, this,
           &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_OnEditClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check data element useable as additional trigger

   \param[in]  orc_Id   Id

   \return
   Flags

   \retval   True    Useable as additional trigger
   \retval   False   Not useable as additional trigger
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::mh_CheckDataElementUseableAsAdditionalTrigger(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   bool q_Retval = false;
   const C_OscNodeDataPoolListElement * const pc_El =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(orc_Id);

   if (pc_El != NULL)
   {
      if ((pc_El->GetArray() && (orc_Id.GetUseArrayElementIndex() == false)) == false)
      {
         q_Retval = true;
      }
      else if (pc_El->q_InterpretAsString)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check selectable data element available

   \return
   Flags

   \retval   True    Selectable data element available
   \retval   False   No selectable data element available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_CheckSelectableDataElementAvailable() const
{
   bool q_Retval = false;

   if (this->mpc_Ui->pc_LineEditDataElement->text().isEmpty() == false)
   {
      q_Retval = true;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Current element is string

   \return
   Flags

   \retval   True    Current element is string
   \retval   False   Current element is not string
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_CurrentElementIsString() const
{
   bool q_Retval = false;
   const C_OscNodeDataPoolListElementOptArrayOptValidId c_NewId = m_GetSelectedDataElement();

   if (c_NewId.GetIsValid() == true)
   {
      const C_OscNodeDataPoolListElement * const pc_El =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_NewId);

      if (pc_El != NULL)
      {
         q_Retval = pc_El->q_InterpretAsString;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current min value

   \return
   Current min value
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_GetCurrentMinValue() const
{
   C_OscNodeDataPoolContent c_Retval;
   const C_OscNodeDataPoolListElementOptArrayOptValidId c_NewId = m_GetSelectedDataElement();

   if (c_NewId.GetIsValid() == true)
   {
      const C_OscNodeDataPoolListElement * const pc_El =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_NewId);

      if (pc_El != NULL)
      {
         c_Retval = pc_El->c_MinValue;
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get operation for core

   \return
   Operation for core
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_GetOperationForCore() const
{
   const QString c_ComboBoxContent = this->mpc_Ui->pc_ComboBoxOperation->itemText(
      this->mpc_Ui->pc_ComboBoxOperation->currentIndex());

   QString c_Retval;

   for (std::vector<std::pair<QString, QString> >::const_iterator c_It = this->mc_MapCoreOperationToUi.cbegin();
        c_It != this->mc_MapCoreOperationToUi.cend(); ++c_It)
   {
      if (c_It->second.compare(c_ComboBoxContent) == 0)
      {
         c_Retval = c_It->first;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! Handle push button Expert View press
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ToggleExpertView()
{
   // Switch from Simple view to Expert view
   if (this->me_ConnectState == eCS_DISCONNECTED)
   {
      this->me_ConnectState = eCS_CONNECTED;
      this->mpc_Ui->pc_PbExpertView->SetSvg("://images/ToggleOnMsgTransmission.svg");

      // copy simple condition to expert condition
      m_ApplyTriggerConditionToExpertView();
      m_SetExpertTriggerCondition();

      this->mpc_Ui->pc_WidgetCondition->setVisible(true);
      this->mpc_Ui->pc_DataWidget->setVisible(false);
   }
   // Switch from Expert view to Simple view
   else
   {
      const auto & rc_Expert =
         C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                           mu32_DataLoggerJobIndex)->c_Properties.
         c_AdditionalTriggerProperties.c_ExpertMode;

      bool q_ToggleView = false;

      // Empty expert condition. Direct switch to simple view
      if ((rc_Expert.c_TriggerConfiguration.IsEmpty() == true) || (rc_Expert.c_TriggerConfiguration == " "))
      {
         q_ToggleView = true;
      }
      // Ask the user to confirm switching the view. This will delete the previously set simple trigger condition
      else
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;

         // Show message
         c_Message.SetHeading(C_GtGetText::h_GetText("Trigger Condition"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Are you sure you want to switch to the simple view? Current condition will be deleted."));
         c_Message.SetOkButtonText(C_GtGetText::h_GetText("Continue"));
         c_Message.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));
         c_Message.SetCustomMinHeight(180, 270);
         e_ReturnMessageBox = c_Message.Execute();

         // Delete simple condition
         if (e_ReturnMessageBox == C_OgeWiCustomMessage::eOK)
         {
            // Reset GUI texts and combobox index
            this->mpc_Ui->pc_LineEditDataElement->setText(C_GtGetText::h_GetText(""));
            this->mpc_Ui->pc_LineEditThreshold->setText(C_GtGetText::h_GetText(""));
            this->mpc_Ui->pc_ComboBoxOperation->setCurrentIndex(-1);
            this->mc_SelectedOptArrayId.MarkInvalid();

            q_ToggleView = true;
         }
      }

      if (q_ToggleView == true)
      {
         this->me_ConnectState = eCS_DISCONNECTED;
         this->mpc_Ui->pc_PbExpertView->SetSvg("://images/ToggleOffMsgTransmission.svg");
         this->mpc_Ui->pc_WidgetCondition->setVisible(false);
         this->mpc_Ui->pc_DataWidget->setVisible(true);
      }
   }
   this->mpc_Ui->pc_PbExpertView->update();
   this->Save();
}
//----------------------------------------------------------------------------------------------------------------------
/*! Handle slot on Edit Clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_OnEditClicked()
{
   // Fetch log job name
   const C_OscDataLoggerJob * const pc_DataLoggerJob = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex,
      this->mu32_DataLoggerJobIndex);

   // Display trigger edit dialog if log job name is valid
   if (pc_DataLoggerJob != NULL)
   {
      const QPointer<C_OgePopUpDialog> c_PopUpCatalog = new C_OgePopUpDialog(this, this);

      C_SdNdeDalLogJobAdditionalTriggerDialog * const pc_AdditionalTriggerDialog =
         new C_SdNdeDalLogJobAdditionalTriggerDialog(*c_PopUpCatalog, pc_DataLoggerJob->c_Properties.c_Name.c_str());

      //Resize
      c_PopUpCatalog->SetSize(QSize(900, 500));
      pc_AdditionalTriggerDialog->SetNodeDataLoggerJob(this->mu32_NodeIndex, this->mu32_DataLoggerJobIndex);

      Q_UNUSED(pc_AdditionalTriggerDialog)
      if (c_PopUpCatalog->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         m_SetExpertTriggerCondition();
      }

      if (c_PopUpCatalog != NULL)
      {
         c_PopUpCatalog->HideOverlay();
         c_PopUpCatalog->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_AdditionalTriggerDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of browse button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_BrowseDataElement(void)
{
   // Dialog opens up for data element selection
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);

   // Single element selection only
   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, 0U, false, false, false, true,
                                                                                 true, true, NULL, false,
                                                                                 this->mu32_NodeIndex, true);

   //Resize
   c_New->SetSize(QSize(800, 800));

   // Save the selected data elements to system definition
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();

      //Cursor
      QApplication::setOverrideCursor(Qt::WaitCursor);

      // Data element was selected from the popup dialog
      // Since it is single selection, only using the first item in the list of data elements i.e c_DataElements[0]
      if (c_DataElements.size() > 0)
      {
         if (c_DataElements[0].GetIsValid() == true)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->CheckAndHandleNewElement(
                          c_DataElements[0]) == C_NO_ERR);

            const C_OscNodeDataPoolListElementOptArrayId & rc_ConfiguredElementOptArrayId = c_DataElements[0];

            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
                  rc_ConfiguredElementOptArrayId.u32_NodeIndex,
                  rc_ConfiguredElementOptArrayId.u32_DataPoolIndex,
                  rc_ConfiguredElementOptArrayId.u32_ListIndex,
                  rc_ConfiguredElementOptArrayId.u32_ElementIndex);

            // QString c_TriggerCondition = this->mpc_Ui->pc_TextEditTriggerCondition->toPlainText();
            if (pc_Element != NULL)
            {
               const QString c_ElementName =
                  C_PuiSdUtil::h_GetNamespaceDatapoolElement(rc_ConfiguredElementOptArrayId);

               this->m_SetSelectedDataElement(c_ElementName, rc_ConfiguredElementOptArrayId);
            }
         }
      }

      //Cursor
      QApplication::restoreOverrideCursor();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      pc_Dialog->PrepareCleanUp();
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set the selected data element

   \param[in]     orc_SelectedDataElement       Selected data element
   \param[in]     orc_SelectedElementId         Selected element Id

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_SetSelectedDataElement(
   const QString & orc_SelectedDataElement, const C_OscNodeDataPoolListElementOptArrayId & orc_SelectedElementId)
{
   this->mc_SelectedOptArrayId = orc_SelectedElementId;
   this->mpc_Ui->pc_LineEditDataElement->setText(orc_SelectedDataElement);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the simple trigger condition to expert condition

   The context for this is mostly when the user toggles from simple to expert view.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ApplyTriggerConditionToExpertView() const
{
   QString c_TriggerCondition("");
   QString c_Threshold("");

   // If all 3 elements of the condition are present
   if ((this->mpc_Ui->pc_LineEditDataElement->text().isEmpty() == false) &&
       (m_GetOperationForCore().isEmpty() == false))
   {
      // Combine all 3 elements of the simple trigger condition: data element, condition & threshold
      if (m_CurrentElementIsString() == true)
      {
         c_Threshold = this->mpc_Ui->pc_LineEditThreshold->text();
      }
      else
      {
         c_Threshold = this->mpc_Ui->pc_WidgetThreshold->GetValue().toString();
      }

      c_TriggerCondition = static_cast<QString>(C_GtGetText::h_GetText("%1 %2 %3")).arg(
         this->mpc_Ui->pc_LineEditDataElement->text(),
         m_GetOperationForCore(),
         c_Threshold);
   }

   // Save the expression
   C_PuiSdHandler::h_GetInstance()->SetDataLoggerAdditionalTriggerExpertModeString(this->mu32_NodeIndex,
                                                                                   this->mu32_DataLoggerJobIndex,
                                                                                   c_TriggerCondition.toStdString());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the expert trigger condition

   if the condition is too long, it is trimmed and appended with ellipses (...)

   NOTE: Node and data logger job indexes must be valid (mu32_NodeIndex, mu32_DataLoggerJobIndex)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_SetExpertTriggerCondition() const
{
   const auto & rc_Expert =
      C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(mu32_NodeIndex,
                                                        mu32_DataLoggerJobIndex)->c_Properties.
      c_AdditionalTriggerProperties.c_ExpertMode;

   // Empty expert condition
   if ((rc_Expert.c_TriggerConfiguration.IsEmpty() == true) || (rc_Expert.c_TriggerConfiguration == " "))
   {
      this->mpc_Ui->pc_LabelConditionExp->setText("Empty condition");
   }
   // Handle long strings (adding  ellipses (...) at the end)
   else
   {
      const QString c_OriginalText = rc_Expert.c_TriggerConfiguration.c_str();
      const QFontMetrics c_FontMetrics(this->mpc_Ui->pc_LabelConditionExp->font());
      const QString c_ElidedText = c_FontMetrics.elidedText(c_OriginalText, Qt::ElideRight,
                                                            this->mpc_Ui->pc_LabelConditionExp->width());
      this->mpc_Ui->pc_LabelConditionExp->setText(c_ElidedText);

      if (c_ElidedText != c_OriginalText)
      {
         this->mpc_Ui->pc_LabelConditionExp->SetToolTipInformation(C_GtGetText::h_GetText(""), c_OriginalText);
      }
      else
      {
         this->mpc_Ui->pc_LabelConditionExp->SetToolTipInformation(C_GtGetText::h_GetText(""), C_GtGetText::h_GetText(
                                                                      ""));
      }
   }
}
