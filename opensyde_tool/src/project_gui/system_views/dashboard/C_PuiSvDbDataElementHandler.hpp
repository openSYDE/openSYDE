//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base functions and data storing for datapool elements and its content (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENTHANDLER_H
#define C_PUISVDBDATAELEMENTHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVector>
#include <QList>
#include <QMap>

#include "stwtypes.hpp"

#include "TglTasks.hpp"

#include "C_PuiSvDbDataElement.hpp"
#include "C_PuiSvDbWidgetBase.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbDataElementContent.hpp"
#include "C_PuiSvDbDataElementScaling.hpp"
#include "C_PuiSvDbDataElementDisplayFormatterConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementHandler :
   public C_PuiSvDbDataElement
{
public:
   C_PuiSvDbDataElementHandler(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                               const int32_t & ors32_DataIndex, const E_Type & ore_Type,
                               const uint32_t ou32_MaximumDataElements, const bool oq_ReadItem);

   int32_t RegisterDataPoolElement(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
                                   const C_PuiSvDbDataElementScaling & orc_DataPoolElementScaling,
                                   const C_PuiSvDbDataElementDisplayFormatter & orc_DataPoolElementFormatter);
   virtual void RegisterDataPoolElementCyclicError(
      const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const uint8_t ou8_ErrorCode);
   void RemoveDataPoolElement(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId);
   void ClearDataPoolElements(void);
   void UpdateElementTransmissionConfiguration(void);
   uint32_t GetWidgetDataPoolElementCount(void) const;
   int32_t GetDataPoolElementIndex(const uint32_t ou32_WidgetDataPoolElementIndex,
                                   C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId) const;
   int32_t GetWidgetDataPoolElementIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId,
                                         uint32_t & oru32_WidgetDataPoolElementIndex) const;
   int32_t GetDataPoolElementScaling(const uint32_t ou32_WidgetDataPoolElementIndex,
                                     C_PuiSvDbDataElementScaling & orc_DataPoolScaling) const;
   int32_t GetDataPoolElementFormatter(const uint32_t ou32_WidgetDataPoolElementIndex,
                                       C_PuiSvDbDataElementDisplayFormatter & orc_FormatterConfig) const;
   int32_t GetDataPoolElementFormatterConfig(const uint32_t ou32_WidgetDataPoolElementIndex,
                                             C_PuiSvDbDataElementDisplayFormatterConfig & orc_FormatterConfig)
   const;
   bool IsDataElementRegistered(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId) const;

   void InsertNewValueIntoQueue(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
                                const C_PuiSvDbDataElementContent & orc_NewValue);
   void AddNewNvmValueIntoQueue(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
                                const C_PuiSvDbDataElementContent & orc_NewValue);
   QString GetUnscaledValueAsScaledString(const float64_t of64_Value, const uint32_t ou32_IndexElement = 0UL) const;
   QString GetUnscaledValueInRangeAsScaledString(const float64_t of64_Value, const uint32_t ou32_IndexElement = 0UL, float64_t * const opf64_Progress =
                                                    NULL) const;
   virtual void SetErrorForInvalidDlc(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ElementId,
                                      const uint8_t ou8_Dlc) = 0;

protected:
   virtual void m_OnDataElementRegistered(const uint32_t ou32_WidgetDataPoolElementIndex);

   virtual int32_t m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex, QString & orc_ScaledValue,
                                  float64_t * const opf64_UnscaledValueAsFloat);
   virtual int32_t m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                                  std::vector<QString> & orc_ScaledValues, std::vector<float64_t> & orc_UnscaledValues);

   virtual int32_t m_GetLastValue(const uint32_t ou32_WidgetDataPoolElementIndex, std::vector<float64_t> & orc_Values,
                                  const bool oq_UseScaling);
   int32_t m_GetAllValues(const uint32_t ou32_WidgetDataPoolElementIndex, QString & orc_ScaledFormattedLastValue,
                          QVector<float64_t> & orc_ScaledValues, QVector<uint32_t> & orc_Timestamps);
   int32_t m_GetLastNvmValue(const uint32_t ou32_WidgetDataPoolElementIndex,
                             stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value);
   int32_t m_GetTimoutPercentage100(const uint32_t ou32_DataElementIndex, uint8_t & oru8_TimoutPercentage100) const;
   int32_t m_ScaleMinMax(const uint32_t ou32_WidgetDataPoolElementIndex, float64_t & orf64_Min,
                         float64_t & orf64_Max) const;
   void m_UpdateDataPoolElementTimeoutAndValidFlag(void);
   void m_UpdateDataPoolElementTimeoutAndValidFlag(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id);

   virtual bool m_CheckHasValidElements(QString & orc_FirstInvalidElementName) const;
   bool m_CheckManualReadRequired(void) const;
   bool m_CheckHasAnyRequiredNodesActive(void) const;
   bool m_CheckHasAnyRequiredNodesValidDashboardRouting(void) const;
   bool m_CheckHasAnyRequiredBusesConnected(void) const;
   bool m_CheckNodeActive(const uint32_t ou32_NodeIndex) const;
   bool m_CheckNodeHasAnyRequiredValidDashboardRouting(const uint32_t ou32_NodeIndex) const;
   bool m_CheckIsOnTrigger(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;
   bool m_CheckElementAlreadyRead(const uint32_t ou32_ItemIndex,
                                  const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;

   virtual void m_DataPoolElementsChanged(void);

   void m_GetErrorDescriptionForManualOperation(const int32_t os32_Result, const uint8_t ou8_Nrc,
                                                QString & orc_Description, QString & orc_Details,
                                                int32_t & ors32_TextHeight) const;

   const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> & m_GetMappingDpElementToDataSerie(void) const;
   void m_GetAllRegisteredElements(std::vector<C_PuiSvDbNodeDataPoolListElementId> & orc_RegisteredElements) const;

   const bool mq_ReadItem;

private:
   class C_DpElementConfig
   {
   public:
      C_DpElementConfig(void)
      {
      }

      QString GetSingleValueContentFormatted(const C_PuiSvDbDataElementContent & orc_Value, const uint32_t ou32_Index,
                                             float64_t * const opf64_UnscaledValueAsFloat) const;
      std::vector<QString> GetValuesContentFormatted(const C_PuiSvDbDataElementContent & orc_Value,
                                                     std::vector<float64_t> & orc_UnscaledValueAsFloat) const;

      C_PuiSvDbDataElementScaling c_Scaling;
      C_PuiSvDbDataElementDisplayFormatterConfig c_FormatterConfig;
   };

   const uint32_t mu32_WidgetDataPoolElementCountMaximum;
   uint32_t mu32_WidgetDataPoolElementCount;
   // For each datapool element an own list
   QVector<QList<C_PuiSvDbDataElementContent> > mc_VecDataValues;
   QVector<C_PuiSvDbDataElementContent> mc_VecNvmValue;
   QVector<bool> mc_VecNvmValueReceived;
   // Mapping of datapool element id to index of datapool element list in mc_VecDataValues
   QMap<C_PuiSvDbNodeDataPoolListElementId, uint32_t> mc_MappingDpElementToDataSerie;
   QMap<C_PuiSvDbNodeDataPoolListElementId,
        C_DpElementConfig> mc_MappingDpElementToConfig; ///< Configured Scaling
   ///< configuration for
   ///< each data element
   std::vector<C_DpElementConfig> mc_UsedConfig; ///< Scaling configuration and initialized display formatter for
   ///< real calculation
   std::vector<uint32_t> mc_DataPoolElementTimeoutsMs; ///< Timouts for each data
   ///< element
   std::vector<uint32_t> mc_LastDataPoolElementTimeStampsMs; ///< Time stamp of last received
   ///< data
   std::vector<bool> mc_LastDataPoolElementTimeStampsValid; ///< Valid flags for time stamp
   ///< of last received data
   std::vector<bool> mc_DataPoolElementValid; ///< Valid flags for each data
   ///< element
   std::vector<std::vector<
                  stw::opensyde_core::C_OscNodeDataPoolContent::E_Type> > mc_MinimumType; ///< Minimal required type
   ///< after scaling

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSection;

   void m_SetWidgetDataPoolElementCount(const uint32_t ou32_Count);
   static QString mh_GetStringForScaledValue(const float64_t of64_Value,
                                             const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type);

   void m_SaveTimeStamp(const uint32_t ou32_WidgetDataPoolElementIndex, const int32_t os32_ValueIndex);

   //Avoid call
   C_PuiSvDbDataElementHandler(const C_PuiSvDbDataElementHandler &);
   C_PuiSvDbDataElementHandler & operator =(const C_PuiSvDbDataElementHandler &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
