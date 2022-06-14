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

#include "stwtypes.h"

#include "TGLTasks.h"

#include "C_PuiSvDbDataElement.h"
#include "C_PuiSvDbWidgetBase.h"
#include "C_OSCNodeDataPoolListElementId.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"
#include "C_PuiSvDbDataElementContent.h"
#include "C_PuiSvDbDataElementScaling.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementHandler :
   public C_PuiSvDbDataElement
{
public:
   C_PuiSvDbDataElementHandler(const stw_types::uint32 & oru32_ViewIndex,
                               const stw_types::uint32 & oru32_DashboardIndex,
                               const stw_types::sint32 & ors32_DataIndex, const E_Type & ore_Type,
                               const stw_types::uint32 ou32_MaximumDataElements, const bool oq_ReadItem);

   stw_types::sint32 RegisterDataPoolElement(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
                                             const C_PuiSvDbDataElementScaling & orc_DataPoolElementScaling);
   virtual void RegisterDataPoolElementCyclicError(
      const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId, const stw_types::uint8 ou8_ErrorCode);
   void RemoveDataPoolElement(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId);
   void ClearDataPoolElements(void);
   void UpdateElementTransmissionConfiguration(void);
   stw_types::uint32 GetWidgetDataPoolElementCount(void) const;
   stw_types::sint32 GetDataPoolElementIndex(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                             C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId) const;
   stw_types::sint32 GetWidgetDataPoolElementIndex(const C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolId,
                                                   stw_types::uint32 & oru32_WidgetDataPoolElementIndex) const;
   stw_types::sint32 GetDataPoolElementScaling(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                               C_PuiSvDbDataElementScaling & orc_DataPoolScaling) const;
   bool IsDataElementRegistered(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId) const;

   void InsertNewValueIntoQueue(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
                                const C_PuiSvDbDataElementContent & orc_NewValue);
   void AddNewNvmValueIntoQueue(const C_PuiSvDbNodeDataPoolListElementId & orc_WidgetDataPoolElementId,
                                const C_PuiSvDbDataElementContent & orc_NewValue);
   QString GetUnscaledValueAsScaledString(const stw_types::float64 of64_Value,
                                          const stw_types::uint32 ou32_IndexElement = 0UL) const;
   QString GetUnscaledValueInRangeAsScaledString(const stw_types::float64 of64_Value,
                                                 const stw_types::uint32 ou32_IndexElement = 0UL, stw_types::float64 * const opf64_Progress =
                                                    NULL) const;
   virtual void SetErrorForInvalidDlc(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ElementId,
                                      const stw_types::uint8 ou8_DLC) = 0;

protected:
   virtual stw_types::sint32 m_GetLastValue(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                            std::vector<stw_types::float64> & orc_Values,
                                            std::vector<QString> * const opc_StringifiedValues,
                                            const bool oq_UseScaling);
   virtual stw_types::sint32 m_GetLastValue(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                            stw_types::float64 & orf64_Value, const bool oq_UseScaling);
   stw_types::sint32 m_GetAllValues(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                    QVector<stw_types::float64> & orc_Values,
                                    QVector<stw_types::uint32> & orc_Timestamps, const bool oq_UseScaling = true);
   stw_types::sint32 m_GetLastNvmValue(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                       stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value);
   stw_types::sint32 m_GetTimoutPercentage100(const stw_types::uint32 ou32_DataElementIndex,
                                              stw_types::uint8 & oru8_TimoutPercentage100) const;
   stw_types::sint32 m_ScaleMinMax(const stw_types::uint32 ou32_WidgetDataPoolElementIndex,
                                   stw_types::float64 & orf64_Min, stw_types::float64 & orf64_Max) const;
   void m_UpdateDataPoolElementTimeoutAndValidFlag(void);
   void m_UpdateDataPoolElementTimeoutAndValidFlag(
      const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id);

   virtual bool m_CheckHasValidElements(QString & orc_FirstInvalidElementName) const;
   bool m_CheckManualReadRequired(void) const;
   bool m_CheckHasAnyRequiredNodesActive(void) const;
   bool m_CheckHasAnyRequiredNodesValidDashboardRouting(void) const;
   bool m_CheckHasAnyRequiredBusesConnected(void) const;
   bool m_CheckNodeActive(const stw_types::uint32 ou32_NodeIndex) const;
   bool m_CheckNodeHasAnyRequiredValidDashboardRouting(const stw_types::uint32 ou32_NodeIndex) const;
   bool m_CheckIsOnTrigger(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;
   bool m_CheckElementAlreadyRead(const stw_types::uint32 ou32_ItemIndex,
                                  const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id) const;

   virtual void m_DataPoolElementsChanged(void);

   void m_GetErrorDescriptionForManualOperation(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC,
                                                QString & orc_Description, QString & orc_Details,
                                                stw_types::sint32 & ors32_TextHeight) const;

   const QMap<C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32> & m_GetMappingDpElementToDataSerie(void) const;
   void m_GetAllRegisteredElements(std::vector<C_PuiSvDbNodeDataPoolListElementId> & orc_RegisteredElements) const;

   const bool mq_ReadItem;

private:
   const stw_types::uint32 mu32_WidgetDataPoolElementCountMaximum;
   stw_types::uint32 mu32_WidgetDataPoolElementCount;
   // For each datapool element an own list
   QVector<QList<C_PuiSvDbDataElementContent> > mc_VecDataValues;
   QVector<C_PuiSvDbDataElementContent> mc_VecNvmValue;
   QVector<bool> mc_VecNvmValueReceived;
   // Mapping of datapool element id to index of datapool element list in mc_VecDataValues
   QMap<C_PuiSvDbNodeDataPoolListElementId, stw_types::uint32> mc_MappingDpElementToDataSerie;
   QMap<C_PuiSvDbNodeDataPoolListElementId,
        C_PuiSvDbDataElementScaling> mc_MappingDpElementToScaling; ///< Configured Scaling
   ///< configuration for
   ///< each data element
   std::vector<C_PuiSvDbDataElementScaling> mc_UsedScaling; ///< Scaling configuration for
   ///< real calculation
   std::vector<stw_types::uint32> mc_DataPoolElementTimeoutsMs; ///< Timouts for each data
   ///< element
   std::vector<stw_types::uint32> mc_LastDataPoolElementTimeStampsMs; ///< Time stamp of last received
   ///< data
   std::vector<bool> mc_LastDataPoolElementTimeStampsValid; ///< Valid flags for time stamp
   ///< of last received data
   std::vector<bool> mc_DataPoolElementValid; ///< Valid flags for each data
   ///< element
   std::vector<std::vector<
                  stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type> > mc_MinimumType; ///< Minimal required type
   ///< after scaling

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSection;

   void m_SetWidgetDataPoolElementCount(const stw_types::uint32 ou32_Count);
   static QString mh_GetStringForScaledValue(const stw_types::float64 of64_Value,
                                             const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type);

   //Avoid call
   C_PuiSvDbDataElementHandler(const C_PuiSvDbDataElementHandler &);
   C_PuiSvDbDataElementHandler & operator =(const C_PuiSvDbDataElementHandler &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
