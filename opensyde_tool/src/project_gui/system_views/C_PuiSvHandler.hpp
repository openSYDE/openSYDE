//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System views data manager (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVHANDLER_HPP
#define C_PUISVHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QObject>
#include "C_OscNode.hpp"
#include "C_PuiSvData.hpp"
#include "C_PuiSvLastKnownHalElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvHandler :
   public QObject
{
   Q_OBJECT

public:
   //File
   int32_t LoadFromFile(const QString & orc_Path);
   int32_t SaveToFile(const QString & orc_Path, const bool oq_UseDeprecatedV1Format);
   bool HasHashChanged(void) const;

   //Get
   const C_PuiSvData * GetView(const uint32_t ou32_Index) const;
   uint32_t GetViewCount(void) const;
   const std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> & GetLastKnownHalcCrcs(void) const;
   bool GetServiceModeActive(void) const;
   int32_t GetNodeActiveFlagsWithSquadAdaptions(const uint32_t ou32_ViewIndex, std::vector<uint8_t> & orc_ActiveFlags);

   //Set
   void SetServiceModeActive(const bool oq_NewValue);
   void SetLastKnownHalcCrcs(const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                            C_PuiSvLastKnownHalElementId> & orc_Value);
   int32_t SetViewName(const uint32_t ou32_Index, const QString & orc_Name);
   int32_t SetViewNodeCheckedState(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex, const bool oq_Checked);
   int32_t SetViewPcBox(const uint32_t ou32_Index, const C_PuiBsBox & orc_Box);
   int32_t SetViewPcConnection(const uint32_t ou32_Index, const C_PuiBsLineBase & orc_Line);
   int32_t SetViewPcConnected(const uint32_t ou32_Index, const bool oq_Connected, const uint32_t ou32_BusIndex);
   int32_t SetViewPcCanDll(const uint32_t ou32_Index, const C_PuiSvPc::E_CanDllType oe_Type,
                           const QString & orc_DllPath);
   int32_t SetViewServiceModeActive(const uint32_t ou32_Index, const bool oq_NewValue);
   int32_t SetViewServiceModeSetupActive(const uint32_t ou32_Index, const bool oq_NewValue);
   int32_t SetViewServiceModeUpdateActive(const uint32_t ou32_Index, const bool oq_NewValue);
   int32_t SetViewServiceModeDashboardActive(const uint32_t ou32_Index, const bool oq_NewValue);
   int32_t SetDashboardName(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                            const QString & orc_Name);
   int32_t SetDashboardComment(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                               const QString & orc_Comment);
   int32_t SetDashboardType(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                            const C_PuiSvDashboard::E_TabType oe_Type);
   int32_t SetDashboardActive(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex, const bool oq_Active);
   int32_t SetDashboardTabIndex(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                const int32_t os32_Value);
   int32_t SetDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                              const uint32_t ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                              const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t SetDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                const uint32_t & oru32_Index, const C_PuiBsBoundary & orc_Data);
   int32_t SetDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                             const uint32_t & oru32_Index, const C_PuiBsImage & orc_Data);
   int32_t SetDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                 const uint32_t & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   int32_t SetDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                   const uint32_t & oru32_Index, const C_PuiBsTextElement & orc_Data);
   int32_t SetViewUpdateRateFast(const uint32_t ou32_ViewIndex, const uint16_t ou16_Value);
   int32_t SetViewUpdateRateMedium(const uint32_t ou32_ViewIndex, const uint16_t ou16_Value);
   int32_t SetViewUpdateRateSlow(const uint32_t ou32_ViewIndex, const uint16_t ou16_Value);
   int32_t SetViewDeviceConfigSelectedBitRate(const uint32_t ou32_ViewIndex, const uint32_t ou32_Value);
   int32_t SetViewDeviceConfigMode(const uint32_t ou32_ViewIndex,
                                   const C_PuiSvData::E_DeviceConfigurationMode oe_Value);
   int32_t SetViewDarkModeActive(const uint32_t ou32_ViewIndex, const bool oq_Value);
   int32_t SetViewReadRailAssignment(const uint32_t ou32_ViewIndex,
                                     const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                     const C_PuiSvReadDataConfiguration & orc_Config);
   int32_t SetNodeUpdateInformation(const uint32_t ou32_ViewIndex,
                                    const std::vector<stw::opensyde_core::C_OscViewNodeUpdate> & orc_NodeUpdateInformation);
   int32_t SetNodeUpdateInformation(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                    const stw::opensyde_core::C_OscViewNodeUpdate & orc_NodeUpdateInformation);
   int32_t SetNodeUpdateInformationPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                        const uint32_t ou32_Index, const QString & orc_Value,
                                        const stw::opensyde_core::C_OscViewNodeUpdate::E_GenericFileType oe_Type);
   int32_t SetNodeUpdateInformationParamInfo(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                             const uint32_t ou32_Index,
                                             const stw::opensyde_core::C_OscViewNodeUpdateParamInfo & orc_Value);
   int32_t SetNodeUpdateInformationPemFilePath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                               const QString & orc_Value);
   int32_t SetNodeUpdateInformationSkipUpdateOfPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                    const uint32_t ou32_Index, const bool oq_SkipFile,
                                                    const stw::opensyde_core::C_OscViewNodeUpdate::E_GenericFileType oe_Type);
   int32_t SetNodeUpdateInformationSkipUpdateOfParamInfo(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                         const uint32_t ou32_Index, const bool oq_SkipFile);
   int32_t SetNodeUpdateInformationSkipUpdateOfPemFile(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                       const bool oq_SkipFile);
   int32_t SetNodeUpdateInformationStates(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                          const stw::opensyde_core::C_OscViewNodeUpdate::E_StateSecurity oe_StateSecurity, const stw::opensyde_core::C_OscViewNodeUpdate::E_StateDebugger oe_StateDebugger);
   int32_t SetNodeUpdateInformationParamInfoContent(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                    const uint32_t ou32_Index, const QString & orc_FilePath,
                                                    const uint32_t ou32_LastKnownCrc);

   //Add
   void AddView(const C_PuiSvData & orc_View, const bool oq_AutoAdaptName, const bool oq_AutoAdaptContent);
   int32_t SetView(const uint32_t ou32_Index, const C_PuiSvData & orc_View);
   int32_t InsertView(const uint32_t ou32_Index, const C_PuiSvData & orc_View, const bool oq_AutoAdaptName,
                      const bool oq_AutoAdaptContent);
   int32_t AddViewReadRailItem(const uint32_t ou32_ViewIndex,
                               const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                               const C_PuiSvReadDataConfiguration & orc_Config);
   int32_t AddNodeUpdateInformationPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                        const QString & orc_Value,
                                        const stw::opensyde_core::C_OscViewNodeUpdate::E_GenericFileType oe_Type);
   int32_t AddNodeUpdateInformationParamInfo(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                             const stw::opensyde_core::C_OscViewNodeUpdateParamInfo & orc_Value);
   int32_t AddDashboard(const uint32_t ou32_ViewIndex, const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt);
   int32_t InsertDashboard(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                           const C_PuiSvDashboard & orc_Dashboard, const bool oq_AutoAdapt,
                           const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
                                      C_PuiSvReadDataConfiguration> * const opc_Rails);
   int32_t AddDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                              const C_PuiSvDbWidgetBase * const opc_Box, const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t InsertDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                 const uint32_t ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                 const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t AddDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                const C_PuiBsBoundary & orc_Data);
   int32_t InsertDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                   const uint32_t & oru32_Index, const C_PuiBsBoundary & orc_Data);
   int32_t AddDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                             const C_PuiBsImage & orc_Data);
   int32_t InsertDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                const uint32_t & oru32_Index, const C_PuiBsImage & orc_Data);
   int32_t AddDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                 const C_PuiBsLineArrow & orc_Data);
   int32_t InsertDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                    const uint32_t & oru32_Index, const C_PuiBsLineArrow & orc_Data);
   int32_t AddDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                   const C_PuiBsTextElement & orc_Data);
   int32_t InsertDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                      const uint32_t & oru32_Index, const C_PuiBsTextElement & orc_Data);

   //Delete
   int32_t DeleteView(const uint32_t ou32_Index);
   int32_t RemoveViewReadRailItem(const uint32_t ou32_ViewIndex,
                                  const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id);
   int32_t RemoveNodeUpdateInformationPath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                           const uint32_t ou32_Index,
                                           const stw::opensyde_core::C_OscViewNodeUpdate::E_GenericFileType oe_Type);
   int32_t RemoveNodeUpdateInformationParamInfo(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                const uint32_t ou32_Index);
   int32_t RemoveNodeUpdateInformationPemFilePath(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex);
   int32_t ClearNodeUpdateInformationAsAppropriate(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                                   const stw::opensyde_core::C_OscViewNodeUpdate::E_GenericFileType oe_Type);
   int32_t ClearNodeUpdateInformationParamPaths(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex);
   int32_t DeleteDashboard(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex);
   int32_t DeleteDashboardWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                 const uint32_t ou32_WidgetIndex, const C_PuiSvDbDataElement::E_Type oe_Type);
   int32_t DeleteDashboardBoundary(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                   const uint32_t & oru32_Index);
   int32_t DeleteDashboardImage(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                const uint32_t & oru32_Index);
   int32_t DeleteDashboardLineArrow(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                    const uint32_t & oru32_Index);
   int32_t DeleteDashboardTextElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                      const uint32_t & oru32_Index);
   void Clear(void);

   //Move
   int32_t MoveView(const uint32_t ou32_StartIndex, const uint32_t ou32_TargetIndex);

   //Sync to system definition
   int32_t SyncDashboardScalingInformation(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex);

   //Error
   void UpdateSystemDefintionErrors(void);
   bool GetErrorNode(const uint32_t ou32_Index) const;
   bool GetErrorBus(const uint32_t ou32_Index) const;
   int32_t CheckViewError(const uint32_t ou32_Index, bool * const opq_NameInvalid, bool * const opq_PcNotConnected,
                          bool * const opq_RoutingInvalid, bool * const opq_RoutingUpdateInvalid,
                          bool * const opq_RoutingDashboardInvalid, bool * const opq_SysDefInvalid,
                          bool * const opq_NoNodesActive, std::vector<QString> * const opc_RoutingErrorDetails,
                          QString * const opc_SetupRoutingWarningDetails);
   int32_t CheckViewReconnectNecessary(const uint32_t ou32_ViewIndex, bool & orq_ReconnectNecessary);
   int32_t CheckViewNodeDashboardRoutingError(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                              bool & orq_RoutingDashboardError);
   int32_t GetViewNodeDashboardRoutingErrors(const uint32_t ou32_ViewIndex, std::set<uint32_t> & orc_NodesWithErrors);
   int32_t GetViewRelevantNodesForDashboardRouting(const uint32_t ou32_ViewIndex,
                                                   std::set<uint32_t> & orc_RelevantNodes);

   //Misc
   int32_t CheckAndHandleNewElement(const C_PuiSvDbNodeDataPoolListElementId & orc_NewId);
   int32_t CalcViewRoutingCrcName(const uint32_t ou32_ViewIndex, const QString & orc_NodeName,
                                  uint32_t & oru32_Crc) const;
   int32_t CalcViewRoutingCrcIndex(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                   uint32_t & oru32_Crc) const;
   static QString h_GetNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   static QString h_GetShortNamespace(const C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   bool CheckBusDisabled(const uint32_t ou32_ViewIndex, const uint32_t ou32_BusIndex) const;
   int32_t ClearViewDashboardParamDataPoolElements(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                   const uint32_t ou32_ParamWidgetIndex);
   int32_t AddViewDashboardParamNewDataPoolElement(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                                   const uint32_t ou32_ParamWidgetIndex,
                                                   const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_NewId,
                                                   const stw::opensyde_core::C_OscNodeDataPoolContent * const opc_Content);
   uint32_t GetViewHash(const uint32_t ou32_ViewIndex) const;

   static C_PuiSvHandler * h_GetInstance(void);
   static void h_Destroy(void);

protected:
   std::vector<C_PuiSvData> mc_Views;

   int32_t m_LoadFromFile(const QString & orc_Path, const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes);
   void m_AddLastKnownHalcCrc(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                              const C_PuiSvLastKnownHalElementId & orc_Crc);

   //Avoid call (protected access for test)
   explicit C_PuiSvHandler(QObject * const opc_Parent = NULL);
   ~C_PuiSvHandler(void) override; //lint !e1768 see comment above

private:
   //Avoid call
   C_PuiSvHandler(const C_PuiSvHandler &);
   C_PuiSvHandler & operator =(const C_PuiSvHandler &) &;

   //Sync to system definition
   void m_OnSyncNodeAdded(const uint32_t ou32_Index);
   void m_OnSyncNodeHalc(const uint32_t ou32_Index);
   void m_OnSyncNodeAboutToBeDeleted(const uint32_t ou32_Index);
   void m_OnSyncBusAdded(const uint32_t ou32_Index);
   void m_OnSyncBusDeleted(const uint32_t ou32_Index);
   void m_OnSyncNodeDataPoolAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void m_OnSyncNodeDataPoolMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolSourceIndex,
                                  const uint32_t ou32_DataPoolTargetIndex);
   void m_OnSyncNodeDataPoolAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);
   void m_OnSyncNodeApplicationAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationSourceIndex,
                                     const uint32_t ou32_ApplicationTargetIndex);
   void m_OnSyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationAboutToBeChangedFromParamSetHalc(const uint32_t ou32_NodeIndex,
                                                                const uint32_t ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationChangedToParamSetHalc(const uint32_t ou32_NodeIndex,
                                                     const uint32_t ou32_ApplicationIndex);
   void m_OnSyncNodeApplicationResultPathSizeChanged(const uint32_t ou32_NodeIndex,
                                                     const uint32_t ou32_ApplicationIndex, const uint32_t ou32_OldSize,
                                                     const uint32_t ou32_NewSize);
   void m_OnSyncNodeDataPoolListAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                      const uint32_t ou32_ListIndex);
   void m_OnSyncNodeDataPoolListMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                      const uint32_t ou32_ListSourceIndex, const uint32_t ou32_ListTargetIndex);
   void m_OnSyncNodeDataPoolListAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                 const uint32_t ou32_ListIndex);
   void m_OnSyncNodeDataPoolListDataSetAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetIndex);
   void m_OnSyncNodeDataPoolListDataSetMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListIndex, const uint32_t ou32_DataSetSourceIndex,
                                             const uint32_t ou32_DataSetTargetIndex);
   void m_OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_DataPoolIndex,
                                                        const uint32_t ou32_ListIndex,
                                                        const uint32_t ou32_DataSetIndex);
   void m_OnSyncNodeDataPoolListElementAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex);
   void m_OnSyncNodeDataPoolListElementMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                             const uint32_t ou32_ListIndex, const uint32_t ou32_ElementSourceIndex,
                                             const uint32_t ou32_ElementTargetIndex);
   void m_OnSyncElementTypeOrArrayChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                          const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_Type,
                                          const bool oq_IsArray, const uint32_t ou32_ArraySize, const bool oq_IsString);
   void m_OnSyncElementRangeChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                    const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                    const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MinElement,
                                    const stw::opensyde_core::C_OscNodeDataPoolContent & orc_MaxElement);
   void m_OnSyncNodeDataPoolListElementAccessChanged(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                     const uint32_t ou32_ListIndex, const uint32_t ou32_ElementIndex,
                                                     const stw::opensyde_core::C_OscNodeDataPoolListElement::E_Access oe_Access);
   void m_OnSyncNodeDataPoolListElementAboutToBeDeleted(const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_DataPoolIndex,
                                                        const uint32_t ou32_ListIndex,
                                                        const uint32_t ou32_ElementIndex);
   void m_OnSyncClear(void);

   //Other
   uint32_t m_CalcHashSystemViews(void) const;
   void m_FixInvalidRailConfig(void);
   void m_HandleCompatibilityChart(void);
   void m_FixDashboardWriteContentType(void);
   int32_t m_CheckRoutingDetails(const uint32_t ou32_ViewIndex, const std::vector<uint8_t> & orc_CheckedNodeActiveFlags,
                                 std::map<uint32_t,
                                          QString> & orc_SetupWarningRoutingDetails, std::vector< std::map<uint32_t,
                                                                                                           QString> > & orc_ErrorRoutingDetails, std::set<uint32_t> & orc_NodesWithDashboardRoutingError, std::set<uint32_t> & orc_NodesRelevantForDashboardRouting)
   const;
   int32_t m_CheckRouting(const uint32_t ou32_ViewIndex, const std::vector<uint8_t> & orc_CheckedNodeActiveFlags,
                          QString & orc_SetupWarningMessage, std::vector<QString> & orc_ErrorMessages,
                          std::set<uint32_t> & orc_NodesWithDashboardRoutingError,
                          std::set<uint32_t> & orc_NodesRelevantForDashboardRouting) const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingViewNames(void) const;

   static C_PuiSvHandler * mhpc_Singleton;
   std::vector<bool> mc_SdNodeErrors;
   std::vector<bool> mc_SdBusErrors;
   uint32_t mu32_CalculatedHashSystemViews;
   uint32_t mu32_PreviousSystemDefintionHash;

   bool mq_IsServiceModeActive;
   std::map<C_PuiSvDbNodeDataPoolListElementId, C_PuiSvLastKnownHalElementId> mc_LastKnownHalcCrcs;

   class C_PuiSvViewErrorDetails
   {
   public:
      bool q_NameInvalid;
      bool q_PcNotConnected;
      bool q_RoutingInvalid;
      bool q_UpdateRoutingError;
      bool q_SysDefInvalid;
      bool q_NoNodesActive;
      QString c_RoutingSetupWarningMessage;
      std::vector<QString> c_RoutingErrorMessages;
      std::vector<uint8_t> c_ResultingNodeActiveStatus;
      std::set<uint32_t> c_ResultNodesWithDashboardRoutingError;
      std::set<uint32_t> c_ResultNodesRelevantForDashboardRouting;

      void GetResults(bool * const opq_NameInvalid, bool * const opq_PcNotConnected, bool * const opq_RoutingInvalid,
                      bool * const opq_RoutingUpdateInvalid, bool * const opq_RoutingDashboardInvalid,
                      bool * const opq_SysDefInvalid, bool * const opq_NoNodesActive,
                      std::vector<QString> * const opc_RoutingErrorDetails,
                      QString * const opc_SetupRoutingWarningDetails) const;
   };

   QMap<uint32_t, C_PuiSvViewErrorDetails> mc_PreviousErrorCheckResults;
   QMap<uint32_t, std::vector<uint8_t> > mc_PreviousNodeActiveFlagsWithSquadAdaptionsResults;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
