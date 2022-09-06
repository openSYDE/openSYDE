//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System views data manager (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVHANDLER_H
#define C_PUISVHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QObject>
#include "C_OSCNode.h"
#include "C_PuiSvData.h"
#include "C_PuiSvLastKnownHalElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvHandler :
   public QObject
{
   Q_OBJECT

public:
   //File
   stw_types::sint32 LoadFromFile(const QString & orc_Path);
   stw_types::sint32 SaveToFile(const QString & orc_Path, const bool oq_UseDeprecatedV1Format);
   bool HasHashChanged(void) const;

   //Get
   const C_PuiSvData * GetView(const stw_types::uint32 ou32_Index) const;
   stw_types::uint32 GetViewCount(void) const;
   const std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> & GetLastKnownHalcCrcs(void) const;
   bool GetServiceModeActive(void) const;
   stw_types::sint32 GetNodeActiveFlagsWithSquadAdaptions(const stw_types::uint32 ou32_ViewIndex,
                                                          std::vector<stw_types::uint8> & orc_ActiveFlags);

   //Set
   void SetServiceModeActive(const bool oq_NewValue);
   void SetLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                            C_PuiSvLastKnownHalElementId> & orc_Value);
   stw_types::sint32 SetViewName(const stw_types::uint32 ou32_Index, const QString & orc_Name);
   stw_types::sint32 SetViewNodeCheckedState(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_NodeIndex, const bool oq_Checked);
   stw_types::sint32 SetViewPCBox(const stw_types::uint32 ou32_Index, const C_PuiBsBox & orc_Box);
   stw_types::sint32 SetViewPCConnection(const stw_types::uint32 ou32_Index, const C_PuiBsLineBase & orc_Line);
   stw_types::sint32 SetViewPCConnected(const stw_types::uint32 ou32_Index, const bool oq_Connected,
                                        const stw_types::uint32 ou32_BusIndex);
   stw_types::sint32 SetViewPCCANDll(const stw_types::uint32 ou32_Index, const C_PuiSvPc::E_CANDllType oe_Type,
                                     const QString & orc_DllPath);
   stw_types::sint32 SetViewServiceModeActive(const stw_types::uint32 ou32_Index, const bool oq_NewValue);
   stw_types::sint32 SetViewServiceModeSetupActive(const stw_types::uint32 ou32_Index, const bool oq_NewValue);
   stw_types::sint32 SetViewServiceModeUpdateActive(const stw_types::uint32 ou32_Index, const bool oq_NewValue);
   stw_types::sint32 SetViewServiceModeDashboardActive(const stw_types::uint32 ou32_Index, const bool oq_NewValue);
   stw_types::sint32 SetDashboardName(const stw_types::uint32 ou32_ViewIndex,
                                      const stw_types::uint32 ou32_DashboardIndex, const QString & orc_Name);
   stw_types::sint32 SetDashboardComment(const stw_types::uint32 ou32_ViewIndex,
                                         const stw_types::uint32 ou32_DashboardIndex, const QString & orc_Comment);
   stw_types::sint32 SetDashboardType(const stw_types::uint32 ou32_ViewIndex,
                                      const stw_types::uint32 ou32_DashboardIndex,
                                      const C_PuiSvDashboard::E_TabType oe_Type);
   stw_types::sint32 SetDashboardActive(const stw_types::uint32 ou32_ViewIndex,
                                        const stw_types::uint32 ou32_DashboardIndex, const bool oq_Active);
   stw_types::sint32 SetDashboardTabIndex(const stw_types::uint32 ou32_ViewIndex,
                                          const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::sint32 os32_Value);
   stw_types::sint32 SetDashboardWidget(const stw_types::uint32 ou32_ViewIndex,
                                        const stw_types::uint32 ou32_DashboardIndex,
                                        const stw_types::uint32 ou32_WidgetIndex,
                                        const C_PuiSvDbWidgetBase * const opc_Box,
                                        const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 SetDashboardBoundary(const stw_types::uint32 ou32_ViewIndex,
                                          const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 SetDashboardImage(const stw_types::uint32 ou32_ViewIndex,
                                       const stw_types::uint32 ou32_DashboardIndex,
                                       const stw_types::uint32 & oru32_Index, const C_PuiBsImage & orc_Data);
   stw_types::sint32 SetDashboardLineArrow(const stw_types::uint32 ou32_ViewIndex,
                                           const stw_types::uint32 ou32_DashboardIndex,
                                           const stw_types::uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 SetDashboardTextElement(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_DashboardIndex,
                                             const stw_types::uint32 & oru32_Index,
                                             const C_PuiBsTextElement & orc_Data);
   stw_types::sint32 SetViewUpdateRateFast(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint16 ou16_Value);
   stw_types::sint32 SetViewUpdateRateMedium(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint16 ou16_Value);
   stw_types::sint32 SetViewUpdateRateSlow(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint16 ou16_Value);
   stw_types::sint32 SetViewDeviceConfigSelectedBitRate(const stw_types::uint32 ou32_ViewIndex,
                                                        const stw_types::uint32 ou32_Value);
   stw_types::sint32 SetViewDeviceConfigMode(const stw_types::uint32 ou32_ViewIndex,
                                             const C_PuiSvData::E_DeviceConfigurationMode oe_Value);
   stw_types::sint32 SetViewDarkModeActive(const stw_types::uint32 ou32_ViewIndex, const bool oq_Value);
   stw_types::sint32 SetViewReadRailAssignment(const stw_types::uint32 ou32_ViewIndex,
                                               const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                               const C_PuiSvReadDataConfiguration & orc_Config);
   stw_types::sint32 SetNodeUpdateInformation(const stw_types::uint32 ou32_ViewIndex,
                                              const std::vector<C_PuiSvNodeUpdate> & orc_NodeUpdateInformation);
   stw_types::sint32 SetNodeUpdateInformation(const stw_types::uint32 ou32_ViewIndex,
                                              const stw_types::uint32 ou32_NodeIndex,
                                              const C_PuiSvNodeUpdate & orc_NodeUpdateInformation);
   stw_types::sint32 SetNodeUpdateInformationPath(const stw_types::uint32 ou32_ViewIndex,
                                                  const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_Index, const QString & orc_Value,
                                                  const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 SetNodeUpdateInformationParamInfo(const stw_types::uint32 ou32_ViewIndex,
                                                       const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_Index,
                                                       const C_PuiSvNodeUpdateParamInfo & orc_Value);
   stw_types::sint32 SetNodeUpdateInformationPemFilePath(const stw_types::uint32 ou32_ViewIndex,
                                                         const stw_types::uint32 ou32_NodeIndex,
                                                         const QString & orc_Value);
   stw_types::sint32 SetNodeUpdateInformationSkipUpdateOfPath(const stw_types::uint32 ou32_ViewIndex,
                                                              const stw_types::uint32 ou32_NodeIndex,
                                                              const stw_types::uint32 ou32_Index,
                                                              const bool oq_SkipFile,
                                                              const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 SetNodeUpdateInformationSkipUpdateOfParamInfo(const stw_types::uint32 ou32_ViewIndex,
                                                                   const stw_types::uint32 ou32_NodeIndex,
                                                                   const stw_types::uint32 ou32_Index,
                                                                   const bool oq_SkipFile);
   stw_types::sint32 SetNodeUpdateInformationSkipUpdateOfPemFile(const stw_types::uint32 ou32_ViewIndex,
                                                                 const stw_types::uint32 ou32_NodeIndex,
                                                                 const bool oq_SkipFile);
   stw_types::sint32 SetNodeUpdateInformationStates(const stw_types::uint32 ou32_ViewIndex,
                                                    const stw_types::uint32 ou32_NodeIndex,
                                                    const C_PuiSvNodeUpdate::E_StateSecurity oe_StateSecurity,
                                                    const C_PuiSvNodeUpdate::E_StateDebugger oe_StateDebugger);
   stw_types::sint32 SetNodeUpdateInformationParamInfoContent(const stw_types::uint32 ou32_ViewIndex,
                                                              const stw_types::uint32 ou32_NodeIndex,
                                                              const stw_types::uint32 ou32_Index,
                                                              const QString & orc_FilePath,
                                                              const stw_types::uint32 ou32_LastKnownCrc);

   //Add
   void AddView(const C_PuiSvData & orc_View, const bool oq_AutoAdaptName, const bool oq_AutoAdaptContent);
   stw_types::sint32 SetView(const stw_types::uint32 ou32_Index, const C_PuiSvData & orc_View);
   stw_types::sint32 InsertView(const stw_types::uint32 ou32_Index, const C_PuiSvData & orc_View,
                                const bool oq_AutoAdaptName, const bool oq_AutoAdaptContent);
   stw_types::sint32 AddViewReadRailItem(const stw_types::uint32 ou32_ViewIndex,
                                         const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                         const C_PuiSvReadDataConfiguration & orc_Config);
   stw_types::sint32 AddNodeUpdateInformationPath(const stw_types::uint32 ou32_ViewIndex,
                                                  const stw_types::uint32 ou32_NodeIndex, const QString & orc_Value,
                                                  const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 AddNodeUpdateInformationParamInfo(const stw_types::uint32 ou32_ViewIndex,
                                                       const stw_types::uint32 ou32_NodeIndex,
                                                       const C_PuiSvNodeUpdateParamInfo & orc_Value);
   stw_types::sint32 AddDashboard(const stw_types::uint32 ou32_ViewIndex, const C_PuiSvDashboard & orc_Dashboard,
                                  const bool oq_AutoAdapt);
   stw_types::sint32 InsertDashboard(const stw_types::uint32 ou32_ViewIndex,
                                     const stw_types::uint32 ou32_DashboardIndex,
                                     const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt,
                                     const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                C_PuiSvReadDataConfiguration> * const opc_Rails);
   stw_types::sint32 AddDashboardWidget(const stw_types::uint32 ou32_ViewIndex,
                                        const stw_types::uint32 ou32_DashboardIndex,
                                        const C_PuiSvDbWidgetBase * const opc_Box,
                                        const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 InsertDashboardWidget(const stw_types::uint32 ou32_ViewIndex,
                                           const stw_types::uint32 ou32_DashboardIndex,
                                           const stw_types::uint32 ou32_WidgetIndex,
                                           const C_PuiSvDbWidgetBase * const opc_Box,
                                           const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 AddDashboardBoundary(const stw_types::uint32 ou32_ViewIndex,
                                          const stw_types::uint32 ou32_DashboardIndex,
                                          const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 InsertDashboardBoundary(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_DashboardIndex,
                                             const stw_types::uint32 & oru32_Index, const C_PuiBsBoundary & orc_Data);
   stw_types::sint32 AddDashboardImage(const stw_types::uint32 ou32_ViewIndex,
                                       const stw_types::uint32 ou32_DashboardIndex, const C_PuiBsImage & orc_Data);
   stw_types::sint32 InsertDashboardImage(const stw_types::uint32 ou32_ViewIndex,
                                          const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 & oru32_Index, const C_PuiBsImage & orc_Data);
   stw_types::sint32 AddDashboardLineArrow(const stw_types::uint32 ou32_ViewIndex,
                                           const stw_types::uint32 ou32_DashboardIndex,
                                           const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 InsertDashboardLineArrow(const stw_types::uint32 ou32_ViewIndex,
                                              const stw_types::uint32 ou32_DashboardIndex,
                                              const stw_types::uint32 & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   stw_types::sint32 AddDashboardTextElement(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_DashboardIndex,
                                             const C_PuiBsTextElement & orc_Data);
   stw_types::sint32 InsertDashboardTextElement(const stw_types::uint32 ou32_ViewIndex,
                                                const stw_types::uint32 ou32_DashboardIndex,
                                                const stw_types::uint32 & oru32_Index,
                                                const C_PuiBsTextElement & orc_Data);

   //Delete
   stw_types::sint32 DeleteView(const stw_types::uint32 ou32_Index);
   stw_types::sint32 RemoveViewReadRailItem(const stw_types::uint32 ou32_ViewIndex,
                                            const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id);
   stw_types::sint32 RemoveNodeUpdateInformationPath(const stw_types::uint32 ou32_ViewIndex,
                                                     const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_Index,
                                                     const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 RemoveNodeUpdateInformationParamInfo(const stw_types::uint32 ou32_ViewIndex,
                                                          const stw_types::uint32 ou32_NodeIndex,
                                                          const stw_types::uint32 ou32_Index);
   stw_types::sint32 RemoveNodeUpdateInformationPemFilePath(const stw_types::uint32 ou32_ViewIndex,
                                                            const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 ClearNodeUpdateInformationAsAppropriate(const stw_types::uint32 ou32_ViewIndex,
                                                             const stw_types::uint32 ou32_NodeIndex,
                                                             const C_PuiSvNodeUpdate::E_GenericFileType oe_Type);
   stw_types::sint32 ClearNodeUpdateInformationParamPaths(const stw_types::uint32 ou32_ViewIndex,
                                                          const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 DeleteDashboard(const stw_types::uint32 ou32_ViewIndex,
                                     const stw_types::uint32 ou32_DashboardIndex);
   stw_types::sint32 DeleteDashboardWidget(const stw_types::uint32 ou32_ViewIndex,
                                           const stw_types::uint32 ou32_DashboardIndex,
                                           const stw_types::uint32 ou32_WidgetIndex,
                                           const C_PuiSvDbDataElement::E_Type oe_Type);
   stw_types::sint32 DeleteDashboardBoundary(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_DashboardIndex,
                                             const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteDashboardImage(const stw_types::uint32 ou32_ViewIndex,
                                          const stw_types::uint32 ou32_DashboardIndex,
                                          const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteDashboardLineArrow(const stw_types::uint32 ou32_ViewIndex,
                                              const stw_types::uint32 ou32_DashboardIndex,
                                              const stw_types::uint32 & oru32_Index);
   stw_types::sint32 DeleteDashboardTextElement(const stw_types::uint32 ou32_ViewIndex,
                                                const stw_types::uint32 ou32_DashboardIndex,
                                                const stw_types::uint32 & oru32_Index);
   void Clear(void);

   //Move
   stw_types::sint32 MoveView(const stw_types::uint32 ou32_StartIndex, const stw_types::uint32 ou32_TargetIndex);

   //Sync to system definition
   stw_types::sint32 SyncDashboardScalingInformation(const stw_types::uint32 ou32_ViewIndex,
                                                     const stw_types::uint32 ou32_DashboardIndex);

   //Error
   void UpdateSystemDefintionErrors(void);
   bool GetErrorNode(const stw_types::uint32 ou32_Index) const;
   bool GetErrorBus(const stw_types::uint32 ou32_Index) const;
   stw_types::sint32 CheckViewError(const stw_types::uint32 ou32_Index, bool * const opq_NameInvalid,
                                    bool * const opq_PCNotConnected, bool * const opq_RoutingInvalid,
                                    bool * const opq_RoutingUpdateInvalid, bool * const opq_RoutingDashboardInvalid,
                                    bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                                    std::vector<QString> * const opc_RoutingErrorDetails,
                                    QString * const opc_SetupRoutingWarningDetails);
   stw_types::sint32 CheckViewReconnectNecessary(const stw_types::uint32 ou32_ViewIndex, bool & orq_ReconnectNecessary);
   stw_types::sint32 CheckViewNodeDashboardRoutingError(const stw_types::uint32 ou32_ViewIndex,
                                                        const stw_types::uint32 ou32_NodeIndex,
                                                        bool & orq_RoutingDashboardError);
   stw_types::sint32 GetViewNodeDashboardRoutingErrors(const stw_types::uint32 ou32_ViewIndex,
                                                       std::set<stw_types::uint32> & orc_NodesWithErrors);
   stw_types::sint32 GetViewRelevantNodesForDashboardRouting(const stw_types::uint32 ou32_ViewIndex,
                                                             std::set<stw_types::uint32> & orc_RelevantNodes);

   //Misc
   stw_types::sint32 CheckAndHandleNewElement(const C_PuiSvDbNodeDataPoolListElementId & orc_NewId);
   stw_types::sint32 CalcViewRoutingCrcName(const stw_types::uint32 ou32_ViewIndex, const QString & orc_NodeName,
                                            stw_types::uint32 & oru32_Crc) const;
   stw_types::sint32 CalcViewRoutingCrcIndex(const stw_types::uint32 ou32_ViewIndex,
                                             const stw_types::uint32 ou32_NodeIndex,
                                             stw_types::uint32 & oru32_Crc) const;
   static QString h_GetNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   static QString h_GetShortNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   bool CheckBusDisabled(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_BusIndex) const;
   stw_types::sint32 ClearViewDashboardParamDataPoolElements(const stw_types::uint32 ou32_ViewIndex,
                                                             const stw_types::uint32 ou32_DashboardIndex,
                                                             const stw_types::uint32 ou32_ParamWidgetIndex);
   stw_types::sint32 AddViewDashboardParamNewDataPoolElement(const stw_types::uint32 ou32_ViewIndex,
                                                             const stw_types::uint32 ou32_DashboardIndex,
                                                             const stw_types::uint32 ou32_ParamWidgetIndex,
                                                             const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_NewId, const stw_opensyde_core::C_OSCNodeDataPoolContent * const opc_Content);
   stw_types::uint32 GetViewHash(const stw_types::uint32 ou32_ViewIndex) const;

   static C_PuiSvHandler * h_GetInstance(void);
   static void h_Destroy(void);

protected:
   std::vector<C_PuiSvData> mc_Views;

   stw_types::sint32 m_LoadFromFile(const QString & orc_Path,
                                    const std::vector<stw_opensyde_core::C_OSCNode> & orc_OSCNodes);
   void m_AddLastKnownHalcCrc(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                              const C_PuiSvLastKnownHalElementId & orc_Crc);

   //Avoid call (protected access for test)
   explicit C_PuiSvHandler(QObject * const opc_Parent = NULL);
   ~C_PuiSvHandler(void); //lint !e1768 see comment above

private:
   //Avoid call
   C_PuiSvHandler(const C_PuiSvHandler &);
   C_PuiSvHandler & operator =(const C_PuiSvHandler &);

   //Sync to system definition
   void m_OnSyncNodeAdded(const stw_types::uint32 ou32_Index);
   void m_OnSyncNodeHALC(const stw_types::uint32 ou32_Index);
   void m_OnSyncNodeAboutToBeDeleted(const stw_types::uint32 ou32_Index);
   void m_OnSyncBusAdded(const stw_types::uint32 ou32_Index);
   void m_OnSyncBusDeleted(const stw_types::uint32 ou32_Index);
   void m_OnSyncNodeDataPoolAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void m_OnSyncNodeDataPoolMoved(const stw_types::uint32 ou32_NodeIndex,
                                  const stw_types::uint32 ou32_DataPoolSourceIndex,
                                  const stw_types::uint32 ou32_DataPoolTargetIndex);
   void m_OnSyncNodeDataPoolAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex);
   void m_OnSyncNodeApplicationAdded(const stw_types::uint32 ou32_NodeIndex,
                                     const stw_types::uint32 ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationMoved(const stw_types::uint32 ou32_NodeIndex,
                                     const stw_types::uint32 ou32_ApplicationSourceIndex,
                                     const stw_types::uint32 ou32_ApplicationTargetIndex);
   void m_OnSyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationAboutToBeChangedFromParamSetHALC(const stw_types::uint32 ou32_NodeIndex,
                                                                const stw_types::uint32 ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationChangedToParamSetHALC(const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationResultPathSizeChanged(const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_ApplicationIndex,
                                                     const stw_types::uint32 ou32_OldSize,
                                                     const stw_types::uint32 ou32_NewSize);
   void m_OnSyncNodeDataPoolListAdded(const stw_types::uint32 ou32_NodeIndex,
                                      const stw_types::uint32 ou32_DataPoolIndex,
                                      const stw_types::uint32 ou32_ListIndex);
   void m_OnSyncNodeDataPoolListMoved(const stw_types::uint32 ou32_NodeIndex,
                                      const stw_types::uint32 ou32_DataPoolIndex,
                                      const stw_types::uint32 ou32_ListSourceIndex,
                                      const stw_types::uint32 ou32_ListTargetIndex);
   void m_OnSyncNodeDataPoolListAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                 const stw_types::uint32 ou32_DataPoolIndex,
                                                 const stw_types::uint32 ou32_ListIndex);
   void m_OnSyncNodeDataPoolListDataSetAdded(const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex,
                                             const stw_types::uint32 ou32_ListIndex,
                                             const stw_types::uint32 ou32_DataSetIndex);
   void m_OnSyncNodeDataPoolListDataSetMoved(const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex,
                                             const stw_types::uint32 ou32_ListIndex,
                                             const stw_types::uint32 ou32_DataSetSourceIndex,
                                             const stw_types::uint32 ou32_DataSetTargetIndex);
   void m_OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                        const stw_types::uint32 ou32_DataPoolIndex,
                                                        const stw_types::uint32 ou32_ListIndex,
                                                        const stw_types::uint32 ou32_DataSetIndex);
   void m_OnSyncNodeDataPoolListElementAdded(const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex,
                                             const stw_types::uint32 ou32_ListIndex,
                                             const stw_types::uint32 ou32_ElementIndex);
   void m_OnSyncNodeDataPoolListElementMoved(const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint32 ou32_DataPoolIndex,
                                             const stw_types::uint32 ou32_ListIndex,
                                             const stw_types::uint32 ou32_ElementSourceIndex,
                                             const stw_types::uint32 ou32_ElementTargetIndex);
   void m_OnSyncNodeDataPoolListElementArrayChanged(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex,
                                                    const stw_types::uint32 ou32_ListIndex,
                                                    const stw_types::uint32 ou32_ElementIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                    const bool oq_IsArray, const stw_types::uint32 ou32_ArraySize,
                                                    const bool oq_IsString);
   void m_OnSyncNodeDataPoolListElementAccessChanged(const stw_types::uint32 ou32_NodeIndex,
                                                     const stw_types::uint32 ou32_DataPoolIndex,
                                                     const stw_types::uint32 ou32_ListIndex,
                                                     const stw_types::uint32 ou32_ElementIndex,
                                                     const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access oe_Access);
   void m_OnSyncNodeDataPoolListElementAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                        const stw_types::uint32 ou32_DataPoolIndex,
                                                        const stw_types::uint32 ou32_ListIndex,
                                                        const stw_types::uint32 ou32_ElementIndex);
   void m_OnSyncClear(void);

   //Other
   stw_types::uint32 m_CalcHashSystemViews(void) const;
   void m_FixInvalidRailConfig(void);
   void m_HandleCompatibilityChart(void);
   stw_types::sint32 m_CheckRoutingDetails(const stw_types::uint32 ou32_ViewIndex,
                                           const std::vector<stw_types::uint8> & orc_CheckedNodeActiveFlags,
                                           std::map<stw_types::uint32,
                                                    QString> & orc_SetupWarningRoutingDetails,
                                           std::vector< std::map<stw_types::uint32,
                                                                 QString> > & orc_ErrorRoutingDetails,
                                           std::set<stw_types::uint32> & orc_NodesWithDashboardRoutingError,
                                           std::set<stw_types::uint32> & orc_NodesRelevantForDashboardRouting)
   const;
   stw_types::sint32 m_CheckRouting(const stw_types::uint32 ou32_ViewIndex,
                                    const std::vector<stw_types::uint8> & orc_CheckedNodeActiveFlags,
                                    QString & orc_SetupWarningMessage, std::vector<QString> & orc_ErrorMessages,
                                    std::set<stw_types::uint32> & orc_NodesWithDashboardRoutingError,
                                    std::set<stw_types::uint32> & orc_NodesRelevantForDashboardRouting) const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingViewNames(void) const;

   static C_PuiSvHandler * mhpc_Singleton;
   std::vector<bool> mc_SdNodeErrors;
   std::vector<bool> mc_SdBusErrors;
   stw_types::uint32 mu32_CalculatedHashSystemViews;
   stw_types::uint32 mu32_PreviousSystemDefintionHash;

   bool mq_IsServiceModeActive;
   std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> mc_LastKnownHalcCrcs;

   class C_PuiSvViewErrorDetails
   {
   public:
      bool q_NameInvalid;
      bool q_PCNotConnected;
      bool q_RoutingInvalid;
      bool q_UpdateRoutingError;
      bool q_SysDefInvalid;
      bool q_NoNodesActive;
      QString c_RoutingSetupWarningMessage;
      std::vector<QString> c_RoutingErrorMessages;
      std::vector<stw_types::uint8> c_ResultingNodeActiveStatus;
      std::set<stw_types::uint32> c_ResultNodesWithDashboardRoutingError;
      std::set<stw_types::uint32> c_ResultNodesRelevantForDashboardRouting;

      void GetResults(bool * const opq_NameInvalid, bool * const opq_PCNotConnected, bool * const opq_RoutingInvalid,
                      bool * const opq_RoutingUpdateInvalid, bool * const opq_RoutingDashboardInvalid,
                      bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                      std::vector<QString> * const opc_RoutingErrorDetails,
                      QString * const opc_SetupRoutingWarningDetails) const;
   };

   QMap<stw_types::uint32, C_PuiSvViewErrorDetails> mc_PreviousErrorCheckResults;
   QMap<stw_types::uint32, std::vector<stw_types::uint8> > mc_PreviousNodeActiveFlagsWithSquadAdaptionsResults;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
