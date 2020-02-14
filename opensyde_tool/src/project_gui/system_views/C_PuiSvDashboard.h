//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDASHBOARD_H
#define C_PUISVDASHBOARD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QString>
#include "C_PuiSvDbChart.h"
#include "C_PuiSvDbLabel.h"
#include "C_PuiSvDbPieChart.h"
#include "C_PuiSvDbSpinBox.h"
#include "C_PuiSvDbSlider.h"
#include "C_PuiSvDbTable.h"
#include "C_PuiSvDbToggle.h"
#include "C_PuiSvDbProgressBar.h"
#include "C_PuiBsElements.h"
#include "C_PuiSvDbDataElement.h"
#include "C_PuiSvDbParam.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDashboard :
   public C_PuiBsElements
{
public:
   C_PuiSvDashboard(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   const QString & GetName(void) const;
   void SetName(const QString & orc_Value);
   const QString & GetComment() const;
   void SetComment(const QString & orc_Value);
   bool GetActive(void) const;
   void SetActive(const bool oq_Value);
   stw_types::sint32 GetTabIndex(void) const;
   void SetTabIndex(const stw_types::sint32 os32_Value);
   const std::vector<C_PuiSvDbChart> & GetCharts(void) const;
   void SetCharts(const std::vector<C_PuiSvDbChart> & orc_Value);
   const C_PuiSvDbChart * GetChart(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbLabel> & GetLabels(void) const;
   void SetLabels(const std::vector<C_PuiSvDbLabel> & orc_Value);
   const C_PuiSvDbLabel * GetLabel(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbParam> & GetParams(void) const;
   void SetParams(const std::vector<C_PuiSvDbParam> & orc_Value);
   const C_PuiSvDbParam * GetParam(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbPieChart> & GetPieCharts(void) const;
   void SetPieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Value);
   const C_PuiSvDbPieChart * GetPieChart(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbSpinBox> & GetSpinBoxes(void) const;
   void SetSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Value);
   const C_PuiSvDbSpinBox * GetSpinBox(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbSlider> & GetSliders(void) const;
   void SetSliders(const std::vector<C_PuiSvDbSlider> & orc_Value);
   const C_PuiSvDbSlider * GetSlider(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbProgressBar> & GetProgressBars(void) const;
   void SetProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Value);
   const C_PuiSvDbProgressBar * GetProgressBar(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbTable> & GetTables(void) const;
   void SetTables(const std::vector<C_PuiSvDbTable> & orc_Value);
   const C_PuiSvDbTable * GetTable(const stw_types::uint32 ou32_Index) const;
   const std::vector<C_PuiSvDbToggle> & GetToggles(void) const;
   void SetToggles(const std::vector<C_PuiSvDbToggle> & orc_Value);
   const C_PuiSvDbToggle * GetToggle(const stw_types::uint32 ou32_Index) const;

   const C_PuiSvDbWidgetBase * GetWidgetBase(const C_PuiSvDbDataElement::E_Type oe_Type,
                                             const stw_types::uint32 ou32_Index) const;
   void GetAllWidgetItems(std::vector<const C_PuiSvDbWidgetBase *> & orc_Output) const;

   //Set
   stw_types::sint32 SetWidget(const stw_types::uint32 ou32_Index, const C_PuiSvDbWidgetBase * const opc_Value,
                               const C_PuiSvDbDataElement::E_Type oe_Type);

   //Sync to system definition
   void OnSyncNodeAdded(const stw_types::uint32 ou32_Index);
   void OnSyncNodeAboutToBeDeleted(const stw_types::uint32 ou32_Index);
   void OnSyncNodeDataPoolAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void OnSyncNodeDataPoolMoved(const stw_types::uint32 ou32_NodeIndex,
                                const stw_types::uint32 ou32_DataPoolSourceIndex,
                                const stw_types::uint32 ou32_DataPoolTargetIndex);
   void OnSyncNodeDataPoolAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex);
   void OnSyncNodeDataPoolListAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                    const stw_types::uint32 ou32_ListIndex);
   void OnSyncNodeDataPoolListMoved(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                    const stw_types::uint32 ou32_ListSourceIndex,
                                    const stw_types::uint32 ou32_ListTargetIndex);
   void OnSyncNodeDataPoolListAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_DataPoolIndex,
                                               const stw_types::uint32 ou32_ListIndex);
   void OnSyncNodeDataPoolListDataSetAdded(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex,
                                           const stw_types::uint32 ou32_ListIndex,
                                           const stw_types::uint32 ou32_DataSetIndex);
   void OnSyncNodeDataPoolListDataSetMoved(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex,
                                           const stw_types::uint32 ou32_ListIndex,
                                           const stw_types::uint32 ou32_DataSetSourceIndex,
                                           const stw_types::uint32 ou32_DataSetTargetIndex);
   void OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                      const stw_types::uint32 ou32_DataPoolIndex,
                                                      const stw_types::uint32 ou32_ListIndex,
                                                      const stw_types::uint32 ou32_DataSetIndex);
   void OnSyncNodeDataPoolListElementAdded(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex,
                                           const stw_types::uint32 ou32_ListIndex,
                                           const stw_types::uint32 ou32_ElementIndex);
   void OnSyncNodeDataPoolListElementMoved(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex,
                                           const stw_types::uint32 ou32_ListIndex,
                                           const stw_types::uint32 ou32_ElementSourceIndex,
                                           const stw_types::uint32 ou32_ElementTargetIndex);
   bool OnSyncNodeDataPoolListElementArrayChanged(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_DataPoolIndex,
                                                  const stw_types::uint32 ou32_ListIndex,
                                                  const stw_types::uint32 ou32_ElementIndex,
                                                  const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                  const bool oq_IsArray, const stw_types::uint32 ou32_ArraySize,
                                                  const bool oq_IsString);
   void OnSyncNodeDataPoolListElementAccessChanged(const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint32 ou32_DataPoolIndex,
                                                   const stw_types::uint32 ou32_ListIndex,
                                                   const stw_types::uint32 ou32_ElementIndex,
                                                   const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access oe_Access);
   void OnSyncNodeDataPoolListElementAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                      const stw_types::uint32 ou32_DataPoolIndex,
                                                      const stw_types::uint32 ou32_ListIndex,
                                                      const stw_types::uint32 ou32_ElementIndex);
   static void h_OnSyncNodeAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                 const stw_types::uint32 ou32_Index);
   static void h_OnSyncNodeAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                            const stw_types::uint32 ou32_Index);
   static void h_OnSyncNodeDataPoolAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                         const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                         const stw_types::uint32 ou32_NodeIndex,
                                         const stw_types::uint32 ou32_DataPoolSourceIndex,
                                         const stw_types::uint32 ou32_DataPoolTargetIndex);
   static void h_OnSyncNodeDataPoolAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex);
   static void h_OnSyncNodeDataPoolListAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                             const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex,
                                             const stw_types::uint32 ou32_ListIndex);
   static void h_OnSyncNodeDataPoolListMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                             const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex,
                                             const stw_types::uint32 ou32_ListSourceIndex,
                                             const stw_types::uint32 ou32_ListTargetIndex);
   static void h_OnSyncNodeDataPoolListAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                        const stw_types::uint32 ou32_NodeIndex,
                                                        const stw_types::uint32 ou32_DataPoolIndex,
                                                        const stw_types::uint32 ou32_ListIndex);
   static void h_OnSyncNodeDataPoolListElementAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex,
                                                    const stw_types::uint32 ou32_ListIndex,
                                                    const stw_types::uint32 ou32_ElementIndex);
   static void h_OnSyncNodeDataPoolListElementMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex,
                                                    const stw_types::uint32 ou32_ListIndex,
                                                    const stw_types::uint32 ou32_ElementSourceIndex,
                                                    const stw_types::uint32 ou32_ElementTargetIndex);
   static void h_OnSyncNodeDataPoolListElementAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                               const stw_types::uint32 ou32_NodeIndex,
                                                               const stw_types::uint32 ou32_DataPoolIndex,
                                                               const stw_types::uint32 ou32_ListIndex,
                                                               const stw_types::uint32 ou32_ElementIndex);

   void SyncScalingInformation(void);

   //Add
   stw_types::sint32 AddWidget(const C_PuiSvDbWidgetBase * const opc_Box, const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 InsertWidget(const stw_types::uint32 ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                  const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 ClearParamDataPoolElements(const stw_types::uint32 ou32_ParamWidgetIndex);
   stw_types::sint32 AddParamNewDataPoolElement(const stw_types::uint32 ou32_ParamWidgetIndex,
                                                const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_NewId,
                                                const stw_opensyde_core::C_OSCNodeDataPoolContent * const opc_Content);

   //Delete
   stw_types::sint32 DeleteWidget(const stw_types::uint32 ou32_WidgetIndex, const C_PuiSvDbDataElement::E_Type oe_Type);

   //Check
   void CheckError(bool * const opq_InvalidDataElements, bool * const opq_MissingDataElements) const;
   bool CheckNvmParamListUsage(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id) const;

   //Util
   bool DiscardInvalidIndices(void);
   static C_PuiSvDbDataElement::E_Type h_GetWidgetType(const C_PuiSvDbWidgetBase * const opc_Box);

   //Clear
   virtual void Clear(void);
   virtual stw_types::uint32 Count(void) const;

protected:
   std::vector<C_PuiSvDbChart> mc_Charts;
   std::vector<C_PuiSvDbLabel> mc_Labels;
   std::vector<C_PuiSvDbPieChart> mc_PieCharts;
   std::vector<C_PuiSvDbProgressBar> mc_ProgressBars;
   std::vector<C_PuiSvDbSpinBox> mc_SpinBoxes;
   std::vector<C_PuiSvDbSlider> mc_Sliders;
   std::vector<C_PuiSvDbTable> mc_Tables;
   std::vector<C_PuiSvDbToggle> mc_Toggles;
   std::vector<C_PuiSvDbParam> mc_ParamWidgets;

private:
   QString mc_Name;
   QString mc_Comment;
   bool mq_Active;
   stw_types::sint32 ms32_TabIndex;

   void m_GetAllWidgetItems(std::vector<C_PuiSvDbWidgetBase *> & orc_Output);
   static void mh_MarkInvalid(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId);
   static stw_types::uint32 mh_GetBestIndexForNewItem(const C_PuiSvDbParam & orc_ParamWidget,
                                                      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_NewId, stw_types::sint32 * const ops32_DatasetIndex);
   void m_SyncCleanUpParams(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
