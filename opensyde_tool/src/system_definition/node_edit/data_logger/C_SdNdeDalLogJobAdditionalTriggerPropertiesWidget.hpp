//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for log job additional trigger properties configuration
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBADDITIONALTRIGGERPROPERTIESWIDGET_HPP
#define C_SDNDEDALLOGJOBADDITIONALTRIGGERPROPERTIESWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_OscNodeDataPoolListElementOptArrayId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget(void) override;

   void Save(void);
   void InitStaticNames(void) const;
   void SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex);
   void Reload(void);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataLoggerJobIndex;
   std::vector<std::pair<QString, QString> > mc_MapCoreOperationToUi;
   QMap<QString, stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId> mc_MapDataElement;
   Ui::C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget * mpc_Ui;

   //Avoid call
   C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget(const C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget &);
   C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget & operator =(
      const C_SdNdeDalLogJobAdditionalTriggerPropertiesWidget &) &;

   void m_HandleCheckboxEnabledState(void) const;
   void m_HandleOperationEnabledState(const bool oq_AdditionalTriggerEnabled) const;
   void m_HandleEditFieldsEnabledState(const bool oq_Enabled) const;
   void m_InitSupportedOperations();
   void m_InitSupportedOperationsComboBox() const;
   void m_InitDataElements(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id);
   void m_InitDataElementsMap(void);
   void m_InitComboBoxFromDataElementsMap(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id);
   void m_InitThreshold(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id,
                        const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   void m_InitNonStringThreshold(const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_Element,
                                 const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                 const uint32_t ou32_Index);
   void m_InitUnit(const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id) const;
   stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId m_GetSelectedDataElement(void) const;
   void m_ApplyContentValue(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content) const;
   void m_AdditionalTriggerEnabledChanged(void);
   void m_DataElementChanged(void);
   void m_SetOperation(const QString & orc_NewValue);
   void m_DisconnectChangeTriggers(void) const;
   void m_ReconnectChangeTriggers(void) const;
   static bool mh_CheckDataElementUseableAsAdditionalTrigger(
      const stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_Id);
   bool m_CheckSelectableDataElementAvailable(void) const;
   bool m_CurrentElementIsString(void) const;
   stw::opensyde_core::C_OscNodeDataPoolContent m_GetCurrentMinValue(void) const;
   QString m_GetOperationForCore(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
