//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDASHBOARD_HPP
#define C_PUISVDASHBOARD_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <set>
#include <QString>
#include "C_PuiSvDbChart.hpp"
#include "C_PuiSvDbLabel.hpp"
#include "C_PuiSvDbPieChart.hpp"
#include "C_PuiSvDbSpinBox.hpp"
#include "C_PuiSvDbSlider.hpp"
#include "C_PuiSvDbTable.hpp"
#include "C_PuiSvDbToggle.hpp"
#include "C_PuiSvDbProgressBar.hpp"
#include "C_PuiBsElements.hpp"
#include "C_PuiSvDbDataElement.hpp"
#include "C_PuiSvDbParam.hpp"
#include "C_PuiSvDbTabChart.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDashboard :
   public C_PuiBsElements
{
public:
   enum E_TabType
   {
      eSCENE,
      eCHART
   };

   C_PuiSvDashboard(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;

   const QString & GetName(void) const;
   void SetName(const QString & orc_Value);
   const QString & GetComment() const;
   void SetComment(const QString & orc_Value);
   E_TabType GetType(void) const;
   void SetType(const E_TabType oe_Type);
   bool GetActive(void) const;
   void SetActive(const bool oq_Value);
   int32_t GetTabIndex(void) const;
   void SetTabIndex(const int32_t os32_Value);
   const std::vector<C_PuiSvDbChart> & GetCharts(void) const;
   void SetCharts(const std::vector<C_PuiSvDbChart> & orc_Value);
   const C_PuiSvDbTabChart & GetTabChart(void) const;
   void SetTabChart(const C_PuiSvDbTabChart & orc_Value);
   const C_PuiSvDbChart * GetChart(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbLabel> & GetLabels(void) const;
   void SetLabels(const std::vector<C_PuiSvDbLabel> & orc_Value);
   const C_PuiSvDbLabel * GetLabel(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbParam> & GetParams(void) const;
   void SetParams(const std::vector<C_PuiSvDbParam> & orc_Value);
   const C_PuiSvDbParam * GetParam(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbPieChart> & GetPieCharts(void) const;
   void SetPieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Value);
   const C_PuiSvDbPieChart * GetPieChart(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbSpinBox> & GetSpinBoxes(void) const;
   void SetSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Value);
   const C_PuiSvDbSpinBox * GetSpinBox(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbSlider> & GetSliders(void) const;
   void SetSliders(const std::vector<C_PuiSvDbSlider> & orc_Value);
   const C_PuiSvDbSlider * GetSlider(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbProgressBar> & GetProgressBars(void) const;
   void SetProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Value);
   const C_PuiSvDbProgressBar * GetProgressBar(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbTable> & GetTables(void) const;
   void SetTables(const std::vector<C_PuiSvDbTable> & orc_Value);
   const C_PuiSvDbTable * GetTable(const uint32_t ou32_Index) const;
   const std::vector<C_PuiSvDbToggle> & GetToggles(void) const;
   void SetToggles(const std::vector<C_PuiSvDbToggle> & orc_Value);
   const C_PuiSvDbToggle * GetToggle(const uint32_t ou32_Index) const;

   const C_PuiSvDbWidgetBase * GetWidgetBase(const C_PuiSvDbDataElement::E_Type oe_Type,
                                             const uint32_t ou32_Index) const;
   void GetAllWidgetItems(std::vector<const C_PuiSvDbWidgetBase *> & orc_Output) const;
   void GetAllRegisteredDashboardElements(std::set<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_Ids) const;
   void GetAllRegisteredDashboardElementsGuiId(std::set<C_PuiSvDbNodeDataPoolListElementId> & orc_Ids) const;

   //Set
   int32_t SetWidget(const uint32_t ou32_Index, const C_PuiSvDbWidgetBase * const opc_Value,
                     const C_PuiSvDbDataElement::E_Type oe_Type);

   //Sync to system definition
   void OnSyncNodeAdded(const uint32_t ou32_Index);
   void OnSyncNodeHalc(const uint32_t ou32_Index, const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                                 C_PuiSvDbNodeDataPoolListElementId> & orc_MapCurToNew);
   void OnSyncNodeAboutToBeDeleted(const uint32_t ou32_Index);
   void OnSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void OnSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                const uint32_t ou32_DataPoolTargetIndex);
   void OnSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void OnSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                    const uint32_t ou32_ListIndex);
   void OnSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                    const uint32_t ou32_ListSourceIndex, const uint32_t ou32_ListTargetIndex);
   void OnSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                               const uint32_t ou32_ListIndex);
   void OnSyncNodeDataPoolListDataSetAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                           const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetIndex);
   void OnSyncNodeDataPoolListDataSetMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                           const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetSourceIndex,
                                           const uint32_t ou32_DataSetTargetIndex);
   void OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                      const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetIndex);
   void OnSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                           const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   void OnSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                           const uint32_t ou32_ListIndex, const uint32_t ou32_ElementSourceIndex,
                                           const uint32_t ou32_ElementTargetIndex);
   bool OnSyncElementTypeOrArrayChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                        const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                        const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type,
                                        const bool oq_IsArray, const uint32_t ou32_ArraySize, const bool oq_IsString);
   void OnSyncElementRangeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                  const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                  const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MinElement,
                                  const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MaxElement);
   void OnSyncNodeDataPoolListElementAccessChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                   const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                   const stw::opensyde_core::C_OscNodeDataPoolListElement::E_Access oe_Access);
   void OnSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                      const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   static void h_OnSyncNodeAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_Index);
   static void h_OnSyncNodeHalc(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId, const uint32_t ou32_Index,
                                const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                               C_PuiSvDbNodeDataPoolListElementId> & orc_MapCurToNew);
   static void h_OnSyncNodeAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                            const uint32_t ou32_Index);
   static void h_OnSyncNodeDataPoolAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                         const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                         const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                         const uint32_t ou32_DataPoolTargetIndex);
   static void h_OnSyncNodeDataPoolAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolListAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                             const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListIndex);
   static void h_OnSyncNodeDataPoolListMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                             const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListSourceIndex, const uint32_t ou32_ListTargetIndex);
   static void h_OnSyncNodeDataPoolListAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                        const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_DataPoolIndex,
                                                        const uint32_t ou32_ListIndex);
   static void h_OnSyncNodeDataPoolListElementAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                    const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   static void h_OnSyncNodeDataPoolListElementMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                    const uint32_t ou32_ListIndex,
                                                    const uint32_t ou32_ElementSourceIndex,
                                                    const uint32_t ou32_ElementTargetIndex);
   static void h_OnSyncNodeDataPoolListElementAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                               const uint32_t ou32_NodeIndex,
                                                               const uint32_t ou32_DataPoolIndex,
                                                               const uint32_t ou32_ListIndex,
                                                               const uint32_t ou32_ElementIndex);

   void SyncScalingInformation(void);

   //Add
   int32_t AddWidget(const C_PuiSvDbWidgetBase * const opc_Box, const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t InsertWidget(const uint32_t ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                        const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t ClearParamDataPoolElements(const uint32_t ou32_ParamWidgetIndex);
   int32_t AddParamNewDataPoolElement(const uint32_t ou32_ParamWidgetIndex,
                                      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_NewId,
                                      const stw::opensyde_core::C_OscNodeDataPoolContent * const opc_Content);

   //Delete
   void RemoveAllReferencesToElementId(const C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId);
   int32_t DeleteWidget(const uint32_t ou32_WidgetIndex, const C_PuiSvDbDataElement::E_Type oe_Type);

   //Check
   void CheckError(bool * const opq_InvalidDataElements, bool * const opq_MissingDataElements) const;
   bool CheckNvmParamListUsage(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id) const;

   //Util
   bool DiscardInvalidIndices(void);
   void HandleCompatibilityChart(std::vector<C_PuiSvDashboard> & orc_NewCharts);
   void FixDashboardWriteContentType();
   static C_PuiSvDbDataElement::E_Type h_GetWidgetType(const C_PuiSvDbWidgetBase * const opc_Box);

   //Clear
   void Clear(void) override;
   uint32_t Count(void) const override;

protected:
   std::vector<C_PuiSvDbChart> mc_Charts; // deprecated but needed for loading old projects
   std::vector<C_PuiSvDbLabel> mc_Labels;
   std::vector<C_PuiSvDbPieChart> mc_PieCharts;
   std::vector<C_PuiSvDbProgressBar> mc_ProgressBars;
   std::vector<C_PuiSvDbSpinBox> mc_SpinBoxes;
   std::vector<C_PuiSvDbSlider> mc_Sliders;
   std::vector<C_PuiSvDbTable> mc_Tables;
   std::vector<C_PuiSvDbToggle> mc_Toggles;
   std::vector<C_PuiSvDbParam> mc_ParamWidgets;
   C_PuiSvDbTabChart mc_TabChart;

private:
   QString mc_Name;
   QString mc_Comment;
   bool mq_Active;
   int32_t ms32_TabIndex;
   E_TabType me_Type;

   void m_GetAllWidgetItems(std::vector<C_PuiSvDbWidgetBase *> & orc_Output);
   static void mh_MarkInvalid(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId);
   static uint32_t mh_GetBestIndexForNewItem(const C_PuiSvDbParam & orc_ParamWidget,
                                             const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_NewId,
                                             int32_t * const ops32_DatasetIndex);
   void m_SyncCleanUpParams(void);
   static void mh_FixDashboardWriteContentType(C_PuiSvDbWriteWidgetBase & orc_Element);
   void m_SyncWriteElementsOnElementTypeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type);
   static void mh_SyncContentTypeToTypeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                               const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                               const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type,
                                               C_PuiSvDbWriteWidgetBase & orc_Element);
   static void mh_SyncContentToRangeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                            const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                            const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MinElement,
                                            const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MaxElement,
                                            C_PuiSvDbWriteWidgetBase & orc_Element);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
