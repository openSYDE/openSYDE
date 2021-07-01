//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view data element (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDATA_H
#define C_PUISVDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <set>
#include <QMap>
#include <QString>
#include "C_PuiSvPc.h"
#include "C_PuiSvDashboard.h"
#include "C_PuiSvNodeUpdate.h"
#include "C_PuiSvReadDataConfiguration.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvData
{
public:
   enum E_DeviceConfigurationMode
   {
      eDCM_ALL_CONNECTED_INTERFACES,
      eDCM_ONLY_USED_INTERFACES
   };

   C_PuiSvData(void);
   virtual ~C_PuiSvData(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   stw_types::uint32 CalcUpdateHash(void) const;

   bool GetServiceModeActive(void) const;
   bool GetServiceModeSetupActive(void) const;
   bool GetServiceModeUpdateActive(void) const;
   bool GetServiceModeDashboardActive(void) const;
   void SetServiceModeActive(const bool oq_NewValue);
   void SetServiceModeSetupActive(const bool oq_NewValue);
   void SetServiceModeUpdateActive(const bool oq_NewValue);
   void SetServiceModeDashboardActive(const bool oq_NewValue);

   const C_PuiSvPc & GetPcData(void) const;
   void SetPcData(const C_PuiSvPc & orc_Value);
   const std::vector<stw_types::uint8> & GetNodeActiveFlags(void) const;
   void SetNodeActiveFlags(const std::vector<stw_types::uint8> & orc_Value);
   const std::vector<C_PuiSvNodeUpdate> & GetAllNodeUpdateInformation(void) const;
   void SetNodeUpdateInformation(const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation);
   const C_PuiSvNodeUpdate * GetNodeUpdateInformation(const stw_types::uint32 ou32_NodeIndex) const;
   stw_types::sint32 SetNodeUpdateInformation(const stw_types::uint32 ou32_NodeIndex,
                                              const C_PuiSvNodeUpdate & orc_NodeUpdateInformation);
   stw_types::sint32 SetNodeUpdateInformationPath(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_Index, const QString & orc_Value,
                                                  const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 SetNodeUpdateInformationParamInfo(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_Index,
                                                       const C_PuiSvNodeUpdateParamInfo & orc_Value);
   stw_types::sint32 SetNodeUpdateInformationSkipUpdateOfPath(const stw_types::uint32 ou32_NodeIndex,
                                                              const stw_types::uint32 ou32_Index,
                                                              const bool oq_SkipFile,
                                                              const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 SetNodeUpdateInformationSkipUpdateOfParamInfo(const stw_types::uint32 ou32_NodeIndex,
                                                                   const stw_types::uint32 ou32_Index,
                                                                   const bool oq_SkipFile);
   stw_types::sint32 SetNodeUpdateInformationParamInfoContent(const stw_types::uint32 ou32_NodeIndex,
                                                              const stw_types::uint32 ou32_Index,
                                                              const QString & orc_FilePath,
                                                              const stw_types::uint32 ou32_LastKnownCrc);
   stw_types::sint32 AddNodeUpdateInformationPath(const stw_types::uint32 ou32_NodeIndex, const QString & orc_Value,
                                                  const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 AddNodeUpdateInformationParamInfo(const stw_types::uint32 ou32_NodeIndex,
                                                       const C_PuiSvNodeUpdateParamInfo & orc_Value);
   const QString & GetName(void) const;
   void SetName(const QString & orc_Value);
   const std::vector<C_PuiSvDashboard> & GetDashboards(void) const;
   const C_PuiSvDashboard * GetDashboard(const stw_types::uint32 ou32_Index) const;
   void SetDashboards(const std::vector<C_PuiSvDashboard> & orc_Value);
   stw_types::uint16 GetUpdateRateFast(void) const;
   void SetUpdateRateFast(const stw_types::uint16 ou16_Value);
   stw_types::uint16 GetUpdateRateMedium(void) const;
   void SetUpdateRateMedium(const stw_types::uint16 ou16_Value);
   stw_types::uint16 GetUpdateRateSlow(void) const;
   void SetUpdateRateSlow(const stw_types::uint16 ou16_Value);
   stw_types::sint32 GetUpdateRate(const stw_types::uint8 ou8_RailIndex, stw_types::uint16 & oru16_Value) const;
   void SetDeviceConfigSelectedBitRate(const stw_types::uint32 ou32_Value);
   stw_types::uint32 GetDeviceConfigSelectedBitRate(void) const;
   void SetDeviceConfigMode(const E_DeviceConfigurationMode oe_Value);
   E_DeviceConfigurationMode GetDeviceConfigMode(void) const;
   bool GetDarkModeActive(void) const;
   void SetDarkModeActive(const bool oq_Value);
   const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> & GetReadRailAssignments(
      void) const;
   const std::set<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetWriteAssignments(void) const;
   stw_types::sint32 GetReadRailAssignment(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                           C_PuiSvReadDataConfiguration & orc_Config) const;
   void SetReadRailAssignments(const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                          C_PuiSvReadDataConfiguration> & orc_Value);
   stw_types::sint32 SetReadRailAssignment(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                           const C_PuiSvReadDataConfiguration & orc_Config);
   void GetRelevantReadRailAssigmentsForDashboard(const C_PuiSvDashboard & orc_Dashboard,
                                                  QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                       C_PuiSvReadDataConfiguration> & orc_Rails)
   const;
   void GetAllRegisteredDashboardElements(std::set<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_Ids) const;

   stw_types::sint32 SetDashboardName(const stw_types::uint32 ou32_DashboardIndex, const QString & orc_Name);
   stw_types::sint32 SetDashboardComment(const stw_types::uint32 ou32_DashboardIndex, const QString & orc_Comment);
   stw_types::sint32 SetDashboardType(const stw_types::uint32 ou32_DashboardIndex,
                                      const C_PuiSvDashboard::E_TabType oe_Type);
   stw_types::sint32 SetDashboardActive(const stw_types::uint32 ou32_DashboardIndex, const bool oq_Active);
   stw_types::sint32 SetDashboardTabIndex(const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::sint32 os32_Value);
   stw_types::sint32 SetDashboardWidget(const stw_types::uint32 ou32_DashboardIndex,
                                        const stw_types::uint32 ou32_WidgetIndex,
                                        const C_PuiSvDbWidgetBase * const opc_Box,
                                        const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 SetDashboardBoundary(const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 SetDashboardImage(const stw_types::uint32 ou32_DashboardIndex,
                                       const stw_types::uint32 & oru32_Index, const C_PuiBsImage & orc_Data);
   stw_types::sint32 SetDashboardLineArrow(const stw_types::uint32 ou32_DashboardIndex,
                                           const stw_types::uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 SetDashboardTextElement(const stw_types::uint32 ou32_DashboardIndex,
                                             const stw_types::uint32 & oru32_Index,
                                             const C_PuiBsTextElement & orc_Data);

   //Sync to system definition
   void OnSyncNodeAdded(const stw_types::uint32 ou32_Index);
   void OnSyncNodeHALC(const stw_types::uint32 ou32_Index, const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                                          C_PuiSvDbNodeDataPoolListElementId> & orc_MapCurToNew);
   void OnSyncNodeAboutToBeDeleted(const stw_types::uint32 ou32_Index);
   void OnSyncBusAdded(const stw_types::uint32 ou32_Index);
   void OnSyncBusDeleted(const stw_types::uint32 ou32_Index);
   void OnSyncNodeDataPoolAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void OnSyncNodeDataPoolMoved(const stw_types::uint32 ou32_NodeIndex,
                                const stw_types::uint32 ou32_DataPoolSourceIndex,
                                const stw_types::uint32 ou32_DataPoolTargetIndex);
   void OnSyncNodeDataPoolAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                           const stw_types::uint32 ou32_DataPoolIndex);
   void OnSyncNodeApplicationAdded(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_ApplicationIndex);
   void OnSyncNodeApplicationMoved(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_ApplicationSourceIndex,
                                   const stw_types::uint32 ou32_ApplicationTargetIndex);
   void OnSyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_types::uint32 ou32_ApplicationIndex);
   void OnSyncNodeApplicationAboutToBeChangedFromParamSetHALC(const stw_types::uint32 ou32_NodeIndex,
                                                              const stw_types::uint32 ou32_ApplicationIndex);
   void OnSyncNodeApplicationChangedToParamSetHALC(const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint32 ou32_ApplicationIndex);
   void OnSyncNodeApplicationResultPathSizeChanged(const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint32 ou32_ApplicationIndex,
                                                   const stw_types::uint32 ou32_OldSize,
                                                   const stw_types::uint32 ou32_NewSize);
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
   void OnSyncNodeDataPoolListElementArrayChanged(const stw_types::uint32 ou32_NodeIndex,
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

   //Specific setter
   stw_types::sint32 SetNodeCheckedState(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_Checked);
   void SetPCBox(const C_PuiBsBox & orc_Box);
   void SetPCConnection(const C_PuiBsLineBase & orc_Line);
   void SetPCConnected(const bool oq_Connected, const stw_types::uint32 ou32_BusIndex);
   void SetPCCANDllType(const C_PuiSvPc::E_CANDllType oe_DllType);
   void SetPCCANDllPath(const QString & orc_DllPath);

   //Add
   stw_types::sint32 AddReadRailItem(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                     const C_PuiSvReadDataConfiguration & orc_Config);
   void AddDashboard(const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt);
   stw_types::sint32 InsertDashboard(const stw_types::uint32 ou32_Index, const C_PuiSvDashboard & orc_Dashboard,
                                     const bool oq_AutoAdapt);
   stw_types::sint32 AddDashboardWidget(const stw_types::uint32 ou32_DashboardIndex,
                                        const C_PuiSvDbWidgetBase * const opc_Box,
                                        const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 InsertDashboardWidget(const stw_types::uint32 ou32_DashboardIndex,
                                           const stw_types::uint32 ou32_WidgetIndex,
                                           const C_PuiSvDbWidgetBase * const opc_Box,
                                           const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 AddDashboardBoundary(const stw_types::uint32 ou32_DashboardIndex,
                                          const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 InsertDashboardBoundary(const stw_types::uint32 ou32_DashboardIndex,
                                             const stw_types::uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 AddDashboardImage(const stw_types::uint32 ou32_DashboardIndex, const C_PuiBsImage & orc_Data);
   stw_types::sint32 InsertDashboardImage(const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 & oru32_Index, const C_PuiBsImage & orc_Data);
   stw_types::sint32 AddDashboardLineArrow(const stw_types::uint32 ou32_DashboardIndex,
                                           const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 InsertDashboardLineArrow(const stw_types::uint32 ou32_DashboardIndex,
                                              const stw_types::uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 AddDashboardTextElement(const stw_types::uint32 ou32_DashboardIndex,
                                             const C_PuiBsTextElement & orc_Data);
   stw_types::sint32 InsertDashboardTextElement(const stw_types::uint32 ou32_DashboardIndex,
                                                const stw_types::uint32 & oru32_Index,
                                                const C_PuiBsTextElement & orc_Data);
   stw_types::sint32 ClearDashboardParamDataPoolElements(const stw_types::uint32 ou32_DashboardIndex,
                                                         const stw_types::uint32 ou32_ParamWidgetIndex);
   stw_types::sint32 AddDashboardParamNewDataPoolElement(const stw_types::uint32 ou32_DashboardIndex,
                                                         const stw_types::uint32 ou32_ParamWidgetIndex,
                                                         const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_NewId, const stw_opensyde_core::C_OSCNodeDataPoolContent * const opc_Content);

   //Delete
   stw_types::sint32 RemoveReadRailItem(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id);
   stw_types::sint32 RemoveNodeUpdateInformationPath(const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_Index,
                                                     const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 RemoveNodeUpdateInformationParamInfo(const stw_types::uint32 ou32_NodeIndex,
                                                          const stw_types::uint32 ou32_Index);
   stw_types::sint32 ClearNodeUpdateInformationAsAppropriate(const stw_types::uint32 ou32_NodeIndex,
                                                             const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 ClearNodeUpdateInformationParamPaths(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 DeleteDashboard(const stw_types::uint32 ou32_DashboardIndex);
   stw_types::sint32 DeleteDashboardWidget(const stw_types::uint32 ou32_DashboardIndex,
                                           const stw_types::uint32 ou32_WidgetIndex,
                                           const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 DeleteDashboardBoundary(const stw_types::uint32 ou32_DashboardIndex,
                                             const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteDashboardImage(const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteDashboardLineArrow(const stw_types::uint32 ou32_DashboardIndex,
                                              const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteDashboardTextElement(const stw_types::uint32 ou32_DashboardIndex,
                                                const stw_types::uint32 & oru32_Index);

   //Sync to system definition
   stw_types::sint32 SyncDashboardScalingInformation(const stw_types::uint32 ou32_DashboardIndex);

   //Other
   void FixInvalidRailConfig(const bool oq_PrintLog = true);
   void HandleCompatibilityChart(void);
   void InitFromSystemDefintion(void);
   bool CheckDashboardName(const QString & orc_Proposal, const stw_types::uint32 * const opu32_DashboardIndex) const;
   QString GetUniqueDashboardName(const QString & orc_Proposal) const;
   bool CheckReadUsage(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   bool CheckNonParamReadUsage(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   stw_types::uint32 CountReadUsage(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   stw_types::uint32 CountCyclicTransmissions(const stw_types::uint32 ou32_NodeIndex) const;
   bool CheckNvmParamListUsage(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id) const;

protected:
   //Protected access for tests
   std::vector<stw_types::uint8> mc_NodeActiveFlags; ///< Vector of usage flags.
   ///< Equal to system definition nodes count.
   ///< True: Node used in system view
   ///< False: Node not used in system view
   std::vector<C_PuiSvDashboard> mc_Dashboards; ///< Dashboard data

private:
   QString mc_Name;                                         ///< System view name
   C_PuiSvPc mc_PcData;                                     ///< Data for PC element
   std::vector<C_PuiSvNodeUpdate> mc_NodeUpdateInformation; ///< Vector of node update information.
   ///< Equal to system definition nodes count.
   stw_types::uint16 mu16_UpdateRateFast;
   stw_types::uint16 mu16_UpdateRateMedium;
   stw_types::uint16 mu16_UpdateRateSlow;
   stw_types::uint32 mu32_DeviceConfigSelectedBitRate;
   E_DeviceConfigurationMode me_DeviceConfigMode;

   QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId, C_PuiSvReadDataConfiguration> mc_ReadRailAssignments;
   bool mq_DarkModeActive;

   bool mq_IsServiceModeActive;
   bool mq_IsServiceModeSetupActive;
   bool mq_IsServiceModeUpdateActive;
   bool mq_IsServiceModeDashboardActive;

   std::vector<const QString *> m_GetExistingDashboardNames(void) const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingDashboardNamesMap(void) const;
   void m_CheckAllReadRails(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
