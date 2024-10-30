//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view data element (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDATA_HPP
#define C_PUISVDATA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <set>
#include <QMap>
#include <QString>
#include "C_OscViewData.hpp"
#include "C_PuiSvPc.hpp"
#include "C_PuiSvDashboard.hpp"
#include "C_OscViewNodeUpdate.hpp"
#include "C_PuiSvReadDataConfiguration.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvData :
   public stw::opensyde_core::C_OscViewData
{
public:
   enum E_DeviceConfigurationMode
   {
      eDCM_ALL_CONNECTED_INTERFACES,
      eDCM_ONLY_USED_INTERFACES
   };

   C_PuiSvData(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;

   bool GetServiceModeActive(void) const;
   bool GetServiceModeSetupActive(void) const;
   bool GetServiceModeUpdateActive(void) const;
   bool GetServiceModeDashboardActive(void) const;
   void SetServiceModeActive(const bool oq_NewValue);
   void SetServiceModeSetupActive(const bool oq_NewValue);
   void SetServiceModeUpdateActive(const bool oq_NewValue);
   void SetServiceModeDashboardActive(const bool oq_NewValue);

   const C_PuiSvPc & GetPuiPcData(void) const;
   void SetPuiPcData(const C_PuiSvPc & orc_Value);
   bool GetNodeStatusDisplayedAsActive(const uint32_t ou32_NodeIndex) const;
   const std::vector<C_PuiSvDashboard> & GetDashboards(void) const;
   const C_PuiSvDashboard * GetDashboard(const uint32_t ou32_Index) const;
   void SetDashboards(const std::vector<C_PuiSvDashboard> & orc_Value);
   uint16_t GetUpdateRateFast(void) const;
   void SetUpdateRateFast(const uint16_t ou16_Value);
   uint16_t GetUpdateRateMedium(void) const;
   void SetUpdateRateMedium(const uint16_t ou16_Value);
   uint16_t GetUpdateRateSlow(void) const;
   void SetUpdateRateSlow(const uint16_t ou16_Value);
   int32_t GetUpdateRate(const uint8_t ou8_RailIndex, uint16_t & oru16_Value) const;
   void SetDeviceConfigSelectedBitRate(const uint32_t ou32_Value);
   uint32_t GetDeviceConfigSelectedBitRate(void) const;
   void SetDeviceConfigMode(const E_DeviceConfigurationMode oe_Value);
   E_DeviceConfigurationMode GetDeviceConfigMode(void) const;
   bool GetDarkModeActive(void) const;
   void SetDarkModeActive(
      const bool oq_Value);
   const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> & GetReadRailAssignments(
      void) const;
   const std::set<stw::opensyde_core::C_OscNodeDataPoolListElementId> GetWriteAssignments(void) const;
   int32_t GetReadRailAssignment(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                 C_PuiSvReadDataConfiguration & orc_Config) const;
   void SetReadRailAssignments(const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                          C_PuiSvReadDataConfiguration> & orc_Value);
   int32_t SetReadRailAssignment(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                 const C_PuiSvReadDataConfiguration & orc_Config);
   void GetRelevantReadRailAssigmentsForDashboard(const C_PuiSvDashboard & orc_Dashboard,
                                                  QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                                       C_PuiSvReadDataConfiguration> & orc_Rails)
   const;
   void GetAllRegisteredDashboardElements(std::set<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_Ids) const;

   int32_t SetDashboardName(const uint32_t ou32_DashboardIndex, const QString & orc_Name);
   int32_t SetDashboardComment(const uint32_t ou32_DashboardIndex, const QString & orc_Comment);
   int32_t SetDashboardType(const uint32_t ou32_DashboardIndex, const C_PuiSvDashboard::E_TabType oe_Type);
   int32_t SetDashboardActive(const uint32_t ou32_DashboardIndex, const bool oq_Active);
   int32_t SetDashboardTabIndex(const uint32_t ou32_DashboardIndex, const int32_t os32_Value);
   int32_t SetDashboardWidget(const uint32_t ou32_DashboardIndex, const uint32_t ou32_WidgetIndex,
                              const C_PuiSvDbWidgetBase * const opc_Box, const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t SetDashboardBoundary(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                const C_PuiBsBoundary & orc_Data);
   int32_t SetDashboardImage(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                             const C_PuiBsImage & orc_Data);
   int32_t SetDashboardLineArrow(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                 const C_PuiBsLineArrow & orc_Data);
   int32_t SetDashboardTextElement(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                   const C_PuiBsTextElement & orc_Data);

   //Sync to system definition
   void OnSyncNodeAdded(const uint32_t ou32_Index);
   void OnSyncNodeHalc(const uint32_t ou32_Index,
                       const std::map<stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId,
                                      stw::opensyde_core::C_OscNodeDataPoolListElementOptArrayId> & orc_MapCurToNew);
   void OnSyncNodeReplace(const uint32_t ou32_Index);
   void OnSyncNodeAboutToBeDeleted(const uint32_t ou32_Index, const bool oq_OnlyMarkInvalid);
   void OnSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void OnSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                const uint32_t ou32_DataPoolTargetIndex);
   void OnSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void OnSyncNodeApplicationAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationSourceIndex,
                                   const uint32_t ou32_ApplicationTargetIndex);
   void OnSyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc(const uint32_t ou32_NodeIndex,
                                                              const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationChangedToParamSetHalc(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void OnSyncNodeApplicationResultPathSizeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                                                   const uint32_t ou32_OldSize, const uint32_t ou32_NewSize);
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
   void OnSyncElementTypeOrArrayChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
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

   //Specific setter
   void SetNodeCheckedState(const uint32_t ou32_NodeIndex, const uint8_t ou8_Checked);
   void SetPcBox(const C_PuiBsBox & orc_Box);
   void SetPcConnection(const C_PuiBsLineBase & orc_Line);
   void SetPcCanDllType(const C_PuiSvPc::E_CanDllType oe_DllType);
   void SetPcCanDllPath(const QString & orc_DllPath);

   //Add
   int32_t AddReadRailItem(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                           const C_PuiSvReadDataConfiguration & orc_Config);
   void AddDashboard(const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt);
   int32_t InsertDashboard(const uint32_t ou32_Index, const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt,
                           const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                      C_PuiSvReadDataConfiguration> * const opc_Rails);
   int32_t AddDashboardWidget(const uint32_t ou32_DashboardIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                              const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t InsertDashboardWidget(const uint32_t ou32_DashboardIndex, const uint32_t ou32_WidgetIndex,
                                 const C_PuiSvDbWidgetBase * const opc_Box, const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t AddDashboardBoundary(const uint32_t ou32_DashboardIndex, const C_PuiBsBoundary & orc_Data);
   int32_t InsertDashboardBoundary(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                   const C_PuiBsBoundary & orc_Data);
   int32_t AddDashboardImage(const uint32_t ou32_DashboardIndex, const C_PuiBsImage & orc_Data);
   int32_t InsertDashboardImage(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                const C_PuiBsImage & orc_Data);
   int32_t AddDashboardLineArrow(const uint32_t ou32_DashboardIndex, const C_PuiBsLineArrow & orc_Data);
   int32_t InsertDashboardLineArrow(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                    const C_PuiBsLineArrow & orc_Data);
   int32_t AddDashboardTextElement(const uint32_t ou32_DashboardIndex, const C_PuiBsTextElement & orc_Data);
   int32_t InsertDashboardTextElement(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index,
                                      const C_PuiBsTextElement & orc_Data);
   int32_t ClearDashboardParamDataPoolElements(const uint32_t ou32_DashboardIndex,
                                               const uint32_t ou32_ParamWidgetIndex);
   int32_t AddDashboardParamNewDataPoolElement(const uint32_t ou32_DashboardIndex, const uint32_t ou32_ParamWidgetIndex,
                                               const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_NewId,
                                               const stw::opensyde_core::C_OscNodeDataPoolContent * const opc_Content);

   //Delete
   int32_t RemoveReadRailItem(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id);
   int32_t DeleteDashboard(const uint32_t ou32_DashboardIndex);
   int32_t DeleteDashboardWidget(const uint32_t ou32_DashboardIndex, const uint32_t ou32_WidgetIndex,
                                 const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t DeleteDashboardBoundary(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index);
   int32_t DeleteDashboardImage(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index);
   int32_t DeleteDashboardLineArrow(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index);
   int32_t DeleteDashboardTextElement(const uint32_t ou32_DashboardIndex, const uint32_t & oru32_Index);

   //Sync to system definition
   int32_t SyncDashboardScalingInformation(const uint32_t ou32_DashboardIndex);

   //Other
   void ActivateAllRelevantSubDevices(void);
   void FixInvalidRailConfig(const bool oq_PrintLog = true);
   void HandleCompatibilityChart(void);
   void FixDashboardWriteContentType();
   void InitFromSystemDefinition(void);
   bool CheckDashboardName(const QString & orc_Proposal, const uint32_t * const opu32_DashboardIndex) const;
   QString GetUniqueDashboardName(const QString & orc_Proposal) const;
   bool CheckReadUsage(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   bool CheckNonParamReadUsage(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   uint32_t CountReadUsage(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   uint32_t CountCyclicTransmissions(const uint32_t ou32_NodeIndex) const;
   bool CheckNvmParamListUsage(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id) const;

protected:
   //Protected access for tests
   std::vector<C_PuiSvDashboard> mc_Dashboards; ///< Dashboard data

private:
   C_PuiSvPc mc_PuiPcData; ///< Data for PC element

   uint16_t mu16_UpdateRateFast;
   uint16_t mu16_UpdateRateMedium;
   uint16_t mu16_UpdateRateSlow;
   uint32_t mu32_DeviceConfigSelectedBitRate;
   E_DeviceConfigurationMode me_DeviceConfigMode;

   QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> mc_ReadRailAssignments;
   bool mq_DarkModeActive;

   bool mq_IsServiceModeActive;
   bool mq_IsServiceModeSetupActive;
   bool mq_IsServiceModeUpdateActive;
   bool mq_IsServiceModeDashboardActive;

   std::vector<const QString *> m_GetExistingDashboardNames(void) const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingDashboardNamesMap(void) const;
   void m_CheckAllReadRails(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
