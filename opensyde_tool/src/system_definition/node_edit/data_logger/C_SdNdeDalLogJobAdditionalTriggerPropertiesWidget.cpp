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
   mu32_NodeIndex(0UL),
   mu32_DataLoggerJobIndex(0UL),
   mpc_Ui(new Ui::C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();
   this->m_InitSupportedOperations();
   this->m_InitSupportedOperationsComboBox();
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
      m_ApplyContentValue(c_NewValues.c_Threshold);
      C_PuiSdHandler::h_GetInstance()->SetDataLoggerAdditionalTriggerProperties(mu32_NodeIndex,
                                                                                mu32_DataLoggerJobIndex,
                                                                                c_NewValues);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::InitStaticNames() const
{
   this->mpc_Ui->pc_ChkBoxAdditionalTrigger->setText(C_GtGetText::h_GetText("Additional Trigger"));
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
      m_InitDataElements(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId);
      m_SetOperation(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_Operation.c_str());
      m_InitThreshold(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId,
                      pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_Threshold);
      m_InitUnit(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.c_ElementId);
      //Handle enabled/disabled second
      m_HandleCheckboxEnabledState();
      m_HandleOperationEnabledState(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.q_Enable);
      m_HandleEditFieldsEnabledState(pc_DataLoggerJob->c_Properties.c_AdditionalTriggerProperties.q_Enable);
   }

   m_ReconnectChangeTriggers();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle checkbox enabled state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_HandleCheckboxEnabledState() const
{
   bool q_Enabled = false;

   if (m_CheckSelectableDataElementAvailable())
   {
      q_Enabled = true;
   }
   this->mpc_Ui->pc_ChkBoxAdditionalTrigger->setEnabled(q_Enabled);
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
   this->mpc_Ui->pc_ComboBoxDataElement->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_WidgetThreshold->setEnabled(oq_Enabled);
   this->mpc_Ui->pc_LineEditStringEdit->setEnabled(oq_Enabled);
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
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitDataElements(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   m_InitDataElementsMap();
   m_InitComboBoxFromDataElementsMap(orc_Id);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init data elements map
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitDataElementsMap()
{
   const C_OscDataLoggerJob * const pc_DataLoggerJob = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(
      this->mu32_NodeIndex,
      this->mu32_DataLoggerJobIndex);

   this->mc_MapDataElement.clear();
   if (pc_DataLoggerJob != NULL)
   {
      for (uint32_t u32_ItEl = 0UL; u32_ItEl < pc_DataLoggerJob->c_ConfiguredDataElements.size(); ++u32_ItEl)
      {
         const C_OscDataLoggerDataElementReference & rc_CurEl = pc_DataLoggerJob->c_ConfiguredDataElements[u32_ItEl];
         if (mh_CheckDataElementUseableAsAdditionalTrigger(rc_CurEl.c_ConfiguredElementId))
         {
            const QString c_NewEntry = C_PuiSdUtil::h_GetNamespaceDatapoolElement(rc_CurEl.c_ConfiguredElementId);
            this->mc_MapDataElement.insert(c_NewEntry, rc_CurEl.c_ConfiguredElementId);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init combo box from data elements map

   \param[in]  orc_Id   Id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_InitComboBoxFromDataElementsMap(
   const C_OscNodeDataPoolListElementOptArrayId & orc_Id)
{
   bool q_FoundSelection = false;
   uint32_t u32_AddedItems = 0UL;
   uint32_t u32_SelectedIndex = 0UL;

   this->mpc_Ui->pc_ComboBoxDataElement->clear();
   for (QMap<QString, stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId>::ConstIterator c_It =
           this->mc_MapDataElement.cbegin();
        c_It != this->mc_MapDataElement.cend(); ++c_It)
   {
      this->mpc_Ui->pc_ComboBoxDataElement->addItem(c_It.key());
      if (c_It.value() == orc_Id)
      {
         q_FoundSelection = true;
         u32_SelectedIndex = u32_AddedItems;
      }
      ++u32_AddedItems;
   }

   if (q_FoundSelection)
   {
      this->mpc_Ui->pc_ComboBoxDataElement->setCurrentIndex(u32_SelectedIndex);
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
         this->mpc_Ui->pc_LineEditStringEdit->setVisible(true);
         this->mpc_Ui->pc_LineEditStringEdit->setText(C_SdNdeDpContentUtil::h_ConvertToString(orc_Content));
         this->mpc_Ui->pc_LineEditStringEdit->setMaxLength(static_cast<int32_t>(orc_Content.GetArraySize()) - 1);
      }
      else
      {
         this->mpc_Ui->pc_WidgetThreshold->setVisible(true);
         this->mpc_Ui->pc_LineEditStringEdit->setVisible(false);
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
C_OscNodeDataPoolListElementOptArrayId C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_GetSelectedDataElement()
const
{
   C_OscNodeDataPoolListElementOptArrayId c_Retval;

   if (this->mpc_Ui->pc_ComboBoxDataElement->currentIndex() >= 0)
   {
      const uint32_t u32_Index = this->mpc_Ui->pc_ComboBoxDataElement->currentIndex();
      uint32_t u32_AddedItems = 0UL;
      for (QMap<QString, stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId>::ConstIterator c_It =
              this->mc_MapDataElement.cbegin();
           c_It != this->mc_MapDataElement.cend(); ++c_It)
      {
         if (u32_Index == u32_AddedItems)
         {
            c_Retval = c_It.value();
         }
         ++u32_AddedItems;
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply content value

   \param[in,out]  orc_Content   Content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ApplyContentValue(C_OscNodeDataPoolContent & orc_Content)
const
{
   if (m_CurrentElementIsString())
   {
      orc_Content = m_GetCurrentMinValue();
      C_SdNdeDpContentUtil::h_SetString(this->mpc_Ui->pc_LineEditStringEdit->text(), orc_Content);
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Data element changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DataElementChanged()
{
   const C_OscNodeDataPoolListElementOptArrayId c_NewId = m_GetSelectedDataElement();
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set operation

   \param[in]  orc_NewValue   New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_SetOperation(const QString & orc_NewValue)
{
   for (std::vector<std::pair<QString, QString> >::const_iterator c_It = this->mc_MapCoreOperationToUi.cbegin();
        c_It != this->mc_MapCoreOperationToUi.cend(); ++c_It)
   {
      if (c_It->first.compare(orc_NewValue) == 0)
      {
         this->mpc_Ui->pc_ComboBoxOperation->setCurrentIndex(this->mpc_Ui->pc_ComboBoxOperation->findText(c_It->second));
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Disconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DisconnectChangeTriggers() const
{
   disconnect(this->mpc_Ui->pc_ChkBoxAdditionalTrigger, &QCheckBox::stateChanged,
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_AdditionalTriggerEnabledChanged);
   disconnect(this->mpc_Ui->pc_ComboBoxOperation,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   disconnect(this->mpc_Ui->pc_ComboBoxDataElement,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DataElementChanged);
   disconnect(this->mpc_Ui->pc_WidgetThreshold, &C_OgeWiSpinBoxGroup::SigValueChanged,
              this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reconnect change triggers
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_ReconnectChangeTriggers() const
{
   connect(this->mpc_Ui->pc_ChkBoxAdditionalTrigger, &QCheckBox::stateChanged,
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_AdditionalTriggerEnabledChanged);
   connect(this->mpc_Ui->pc_ComboBoxOperation,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
   connect(this->mpc_Ui->pc_ComboBoxDataElement,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::m_DataElementChanged);
   connect(this->mpc_Ui->pc_WidgetThreshold, &C_OgeWiSpinBoxGroup::SigValueChanged,
           this, &C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget::Save);
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
   const bool q_Retval = this->mpc_Ui->pc_ComboBoxDataElement->model()->rowCount() > 0;

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
   const C_OscNodeDataPoolListElementOptArrayId c_NewId = m_GetSelectedDataElement();
   const C_OscNodeDataPoolListElement * const pc_El =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_NewId);

   if (pc_El != NULL)
   {
      q_Retval = pc_El->q_InterpretAsString;
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
   const C_OscNodeDataPoolListElementOptArrayId c_NewId = m_GetSelectedDataElement();
   const C_OscNodeDataPoolListElement * const pc_El =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_NewId);

   if (pc_El != NULL)
   {
      c_Retval = pc_El->c_MinValue;
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
