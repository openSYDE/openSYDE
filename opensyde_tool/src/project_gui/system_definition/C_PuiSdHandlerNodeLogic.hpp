//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node logic part of UI system definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERNODELOGIC_HPP
#define C_PUISDHANDLERNODELOGIC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerData.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerNodeLogic :
   public C_PuiSdHandlerData
{
public:
   //SD general
   void SetNameMaxCharLimit(const uint32_t ou32_NewValue);
   uint32_t GetNameMaxCharLimit(void) const;
   void GetNameMaxCharLimitAffectedItems(const uint32_t ou32_NameMaxCharLimit,
                                         std::list<stw::opensyde_core::C_OscSystemNameMaxCharLimitChangeReportItem> & orc_ChangedItems);
   void ApplyNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit);

   //Node general
   bool CheckNodeNameAvailable(const stw::scl::C_SclString & orc_Name,
                               const uint32_t * const opu32_NodeIndexToSkip = NULL,
                               std::vector<stw::scl::C_SclString> * const opc_ExistingNames = NULL) const;
   void SetOscNodeEthernetConfiguration(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                        const std::vector<int32_t> & orc_Ip, const std::vector<int32_t> & orc_NetMask,
                                        const std::vector<int32_t> & orc_DefaultGateway);
   const C_PuiSdNode * GetUiNode(const uint32_t & oru32_Index) const;
   const stw::opensyde_core::C_OscNode * GetOscNodeConst(const uint32_t & oru32_Index) const;
   const stw::opensyde_core::C_OscNodeSquad * GetOscNodeSquadConst(const uint32_t & oru32_Index) const;
   int32_t GetSortedOscNodeConst(const uint32_t & oru32_Index, stw::opensyde_core::C_OscNode & orc_Node) const;
   stw::opensyde_core::C_OscNode * GetOscNode(const uint32_t & oru32_Index);
   stw::opensyde_core::C_OscNodeSquad * GetOscNodeSquad(const uint32_t & oru32_Index);
   int32_t SetStwFlashloaderSettings(const uint32_t ou32_Index,
                                     const stw::opensyde_core::C_OscNodeStwFlashloaderSettings & orc_Settings);
   int32_t SetNodeOpenSydeServerSettings(const uint32_t ou32_Index,
                                         const stw::opensyde_core::C_OscNodeOpenSydeServerSettings & orc_Settings);
   int32_t SetNodeCodeExportSettings(const uint32_t ou32_Index,
                                     const stw::opensyde_core::C_OscNodeCodeExportSettings & orc_Settings);
   int32_t SetOscNodePropertyXappSupport(const uint32_t ou32_NodeIndex, const bool oq_XappSupport);
   void SetOscNodeProperties(const uint32_t ou32_NodeIndex,
                             const stw::opensyde_core::C_OscNodeProperties & orc_Properties);
   void SetOscNodePropertiesDetailed(const uint32_t ou32_NodeIndex, const QString & orc_Name,
                                     const QString & orc_Comment, const stw::opensyde_core::C_OscNodeProperties::E_DiagnosticServerProtocol
                                     oe_DiagnosticServer,
                                     const stw::opensyde_core::C_OscNodeProperties::E_FlashLoaderProtocol oe_FlashLoader, const std::vector<uint8_t> & orc_NodeIds, const std::vector<bool> & orc_UpdateFlags, const std::vector<bool> & orc_RoutingFlags, const std::vector<bool> & orc_DiagnosisFlags);
   void SetUiNodeBox(const uint32_t ou32_NodeIndex, const C_PuiBsBox & orc_Box);
   uint32_t AddNodeAndSort(stw::opensyde_core::C_OscNode & orc_OscNode, const C_PuiSdNode & orc_UiNode,
                           const QString & orc_SubDeviceName, const QString & orc_MainDevice);
   uint32_t AddNodeSquadAndSort(std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes,
                                const std::vector<C_PuiSdNode> & orc_UiNodes,
                                const std::vector<QString> & orc_NodeNames, const QString & orc_MainDevice,
                                const QString & orc_NameProposal = "");
   void RemoveNode(const uint32_t ou32_NodeIndex);
   bool CheckNodeConflict(const uint32_t & oru32_NodeIndex) const;
   void GetSupportedCanBitrates(const std::vector<uint32_t> & orc_Nodes, std::vector<uint32_t> & orc_Bitrates) const;
   void GetSupportedCanFdBitrates(const std::vector<uint32_t> & orc_Nodes, std::vector<uint32_t> & orc_Bitrates) const;
   bool NodeSupportsCanFd(const std::vector<uint32_t> & orc_Nodes,
                          const std::vector<uint32_t> & orc_InterfaceIndexes) const;
   uint32_t GetOscNodesSize(void) const;
   uint32_t GetOscNodeSquadsSize(void) const;
   std::vector<uint32_t> GetAllNodeGroupIndicesUsingNodeIndex(const uint32_t ou32_NodeIndex) const;
   int32_t GetNodeSquadIndexWithNodeIndex(const uint32_t ou32_NodeIndex, uint32_t & oru32_NodeSquadIndex) const;
   void GetNodeToNodeSquadMapping(std::vector<int32_t> & orc_Mapping);
   bool CheckCriticalNameConflict(std::vector<QString> * const opc_CriticalNodeNames,
                                  std::vector<QString> * const opc_CriticalBusNames,
                                  std::vector<QString> * const opc_CriticalDatapoolNamespaceNames) const;
   int32_t MapNodeNameToIndex(const QString & orc_NodeName, uint32_t & oru32_NodeIndex) const;
   int32_t MapNodeIndexToName(const uint32_t ou32_NodeIndex, QString & orc_NodeName) const;
   bool HasNodeAnAvailableFlashloader(const uint32_t ou32_NodeIndex) const;
   bool CheckNodeIndexAssociatedWithAnotherNodeIndex(const uint32_t ou32_FirstNodeIndex,
                                                     const uint32_t ou32_SecondNodeIndex) const;
   void ReplaceNode(const uint32_t ou32_NodeIndex, const opensyde_core::C_OscNode & orc_OscNode,
                    const C_PuiSdNode & orc_UiNode);

   //Datapool
   int32_t AddDataPool(const uint32_t & oru32_NodeIndex, const stw::opensyde_core::C_OscNodeDataPool & orc_OscContent,
                       const C_PuiSdNodeDataPool & orc_UiContent, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocolType =
                          stw::opensyde_core::C_OscCanProtocol::eLAYER2, const bool & orq_AllowNameAdaptation = true,
                       const bool & orq_AllowDataAdaptation = true);
   int32_t AddAutoGenCommDataPool(const uint32_t & oru32_NodeIndex, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocolType =
                                     stw::opensyde_core::C_OscCanProtocol::eLAYER2);
   int32_t InsertDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                          const stw::opensyde_core::C_OscNodeDataPool & orc_OscContent,
                          const C_PuiSdNodeDataPool & orc_UiContent, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocolType =
                             stw::opensyde_core::C_OscCanProtocol::eLAYER2, const bool & orq_AllowNameAdaptation = true,
                          const bool & orq_AllowDataAdaptation = true);
   int32_t RemoveDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                          const bool oq_SuppressSyncSignal = false);
   int32_t SetDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                       const stw::opensyde_core::C_OscNodeDataPool & orc_OscContent,
                       const C_PuiSdNodeDataPool & orc_UiContent, const bool oq_NewComProtocolType = false, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocolType =
                          stw::opensyde_core::C_OscCanProtocol::eLAYER2);
   int32_t UnassignDataPoolApplication(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex);
   int32_t AssignDataPoolApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                     const int32_t os32_ApplicationIndex);
   int32_t AssignAllHalcNvmDataPools(const uint32_t ou32_NodeIndex);
   int32_t GetDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                       stw::opensyde_core::C_OscNodeDataPool & orc_OscContent,
                       C_PuiSdNodeDataPool & orc_UiContent) const;
   stw::scl::C_SclString GetUniqueDataPoolName(const uint32_t & oru32_NodeIndex,
                                               const stw::scl::C_SclString & orc_Proposal) const;
   const stw::opensyde_core::C_OscNodeDataPool * GetOscDataPool(const uint32_t & oru32_NodeIndex,
                                                                const uint32_t & oru32_DataPoolIndex) const;
   const C_PuiSdNodeDataPool * GetUiDataPool(const uint32_t & oru32_NodeIndex,
                                             const uint32_t & oru32_DataPoolIndex) const;
   int32_t GetDataPoolType(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                           stw::opensyde_core::C_OscNodeDataPool::E_Type & ore_Type) const;
   int32_t MoveDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_SourceIndex,
                        const uint32_t ou32_TargetIndex);
   int32_t GetDataPoolIndex(const uint32_t ou32_NodeIndex,
                            const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType,
                            const uint32_t ou32_DataPoolTypeIndex) const;
   int32_t GetDataPoolTypeIndex(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) const;
   int32_t GetDataPoolCount(const uint32_t ou32_NodeIndex,
                            const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType) const;
   bool CheckNodeDataPoolNameAvailable(const uint32_t & oru32_NodeIndex, const stw::scl::C_SclString & orc_Name,
                                       const uint32_t * const opu32_DataPoolIndexToSkip = NULL, std::vector<stw::scl::C_SclString> * const opc_ExistingDatapoolNames =
                                          NULL) const;
   C_PuiSdSharedDatapools & GetSharedDatapools(void);
   const C_PuiSdSharedDatapools & GetSharedDatapoolsConst(void) const;

   //NVM based Datapool
   class C_PuiSdHandlerNodeLogicNvmArea
   {
   public:
      C_PuiSdHandlerNodeLogicNvmArea(void);
      bool operator <(const C_PuiSdHandlerNodeLogicNvmArea & orc_Cmp) const;
      C_PuiSdHandlerNodeLogicNvmArea & operator =(const C_PuiSdHandlerNodeLogicNvmArea & orc_Source) &;

      uint32_t u32_StartAddress;
      uint32_t u32_Size;
      bool q_InRange;
      std::vector<uint32_t> c_DataPoolIndexes;
   };

   bool CheckNodeNvmDataPoolsSizeConflict(const uint32_t ou32_NodeIndex, bool * const opq_SizeConflict = NULL,
                                          bool * const opq_OverlapConflict = NULL) const;
   int32_t GetNodeNvmDataPoolAreas(const uint32_t ou32_NodeIndex,
                                   std::vector<C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas) const;

   //Data block
   int32_t AddApplication(const uint32_t ou32_NodeIndex, stw::opensyde_core::C_OscNodeApplication & orc_OscContent);
   int32_t InsertApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                             stw::opensyde_core::C_OscNodeApplication & orc_OscContent);
   int32_t RemoveApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   int32_t SetApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex,
                          const stw::opensyde_core::C_OscNodeApplication & orc_OscContent);
   const stw::opensyde_core::C_OscNodeApplication * GetApplication(const uint32_t ou32_NodeIndex,
                                                                   const uint32_t ou32_ApplicationIndex)
   const;
   int32_t MoveApplication(const uint32_t ou32_NodeIndex, const uint32_t ou32_SourceIndex,
                           const uint32_t ou32_TargetIndex);
   stw::scl::C_SclString GetUniqueApplicationName(const uint32_t & oru32_NodeIndex,
                                                  const stw::scl::C_SclString & orc_Proposal) const;
   uint8_t GetUniqueApplicationProcessId(const uint32_t ou32_NodeIndex, const uint8_t ou8_Proposal) const;
   int32_t GetNextProgrammableApplicationIndex(const uint32_t ou32_NodeIndex, int32_t & ors32_ApplicationIndex) const;
   int32_t CheckApplicationName(const uint32_t ou32_NodeIndex, const stw::scl::C_SclString & orc_ApplicationName,
                                bool & orq_Valid, const uint32_t * const opu32_SkipApplication = NULL) const;
   std::vector<const stw::opensyde_core::C_OscNodeApplication *> GetProgrammableApplications(
      const uint32_t ou32_NodeIndex) const;
   std::vector<uint32_t> GetFileGenAppIndices(const uint32_t ou32_NodeIndex) const;

   //Datapool list
   int32_t InsertDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_DataPoolListIndex,
                              const stw::opensyde_core::C_OscNodeDataPoolList & orc_OscContent,
                              const C_PuiSdNodeDataPoolList & orc_UiContent,
                              const bool oq_HandleSharedDatapools = true);
   int32_t RemoveDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_DataPoolListIndex, const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                           const uint32_t & oru32_DataPoolListIndex,
                           const stw::opensyde_core::C_OscNodeDataPoolList & orc_OscContent,
                           const C_PuiSdNodeDataPoolList & orc_UiContent, const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListName(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                               const uint32_t & oru32_DataPoolListIndex, const QString & orc_Value,
                               const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListComment(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                  const uint32_t & oru32_DataPoolListIndex, const QString & orc_Value,
                                  const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListNvmSize(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                  const uint32_t & oru32_DataPoolListIndex, const uint32_t ou32_Value,
                                  const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListNvmCrc(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                 const uint32_t & oru32_DataPoolListIndex, const bool oq_Value,
                                 const bool oq_HandleSharedDatapools = true);
   int32_t GetDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                           const uint32_t & oru32_DataPoolListIndex,
                           stw::opensyde_core::C_OscNodeDataPoolList & orc_OscContent,
                           C_PuiSdNodeDataPoolList & orc_UiContent) const;
   int32_t GetDataPoolListDataSet(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                  const uint32_t & oru32_DataPoolListIndex,
                                  const uint32_t & oru32_DataPoolListDataSetIndex,
                                  stw::opensyde_core::C_OscNodeDataPoolDataSet & orc_OscName,
                                  std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> & orc_OscValues)
   const;
   const stw::opensyde_core::C_OscNodeDataPoolDataSet * GetOscDataPoolListDataSet(const uint32_t & oru32_NodeIndex,
                                                                                  const uint32_t & oru32_DataPoolIndex,
                                                                                  const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListDataSetIndex)
   const;
   int32_t SetOscNodeDataPoolDataSet(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_DataPoolListIndex,
                                     const uint32_t & oru32_DataPoolListDataSetIndex,
                                     const stw::opensyde_core::C_OscNodeDataPoolDataSet & orc_OscContent,
                                     const bool oq_HandleSharedDatapools = true);
   int32_t InsertDataPoolListDataSet(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_DataPoolListIndex,
                                     const uint32_t & oru32_DataPoolListDataSetIndex,
                                     const stw::opensyde_core::C_OscNodeDataPoolDataSet & orc_OscName,
                                     const std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> & orc_OscValues,
                                     const bool oq_HandleSharedDatapools = true);
   int32_t RemoveDataPoolListDataSet(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_DataPoolListIndex,
                                     const uint32_t & oru32_DataPoolListDataSetIndex,
                                     const bool oq_HandleSharedDatapools = true);
   int32_t MoveDataPoolListDataSet(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                   const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_SourceIndex,
                                   const uint32_t & oru32_TargetIndex, const bool oq_HandleSharedDatapools = true);
   const stw::opensyde_core::C_OscNodeDataPoolList * GetOscDataPoolList(const uint32_t & oru32_NodeIndex,
                                                                        const uint32_t & oru32_DataPoolIndex,
                                                                        const uint32_t & oru32_DataPoolListIndex)
   const;
   const C_PuiSdNodeDataPoolList * GetUiDataPoolList(const uint32_t & oru32_NodeIndex,
                                                     const uint32_t & oru32_DataPoolIndex,
                                                     const uint32_t & oru32_DataPoolListIndex) const;
   int32_t MoveDataPoolList(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                            const uint32_t & oru32_SourceIndex, const uint32_t & oru32_TargetIndex,
                            const bool oq_HandleSharedDatapools = true);
   bool CheckNodeDataPoolListNameAvailable(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                           const stw::scl::C_SclString & orc_Name,
                                           const uint32_t * const opu32_DataPoolListIndexToSkip = NULL) const;
   bool CheckNodeDataPoolListDataSetNameAvailable(const uint32_t & oru32_NodeIndex,
                                                  const uint32_t & oru32_DataPoolIndex,
                                                  const uint32_t & oru32_ListIndex,
                                                  const stw::scl::C_SclString & orc_Name, const uint32_t *
                                                  const opu32_DataPoolListDataSetIndexToSkip = NULL) const;

   //Datapool list element
   int32_t ReserveDataPoolListElements(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                       const uint32_t & oru32_DataPoolListIndex,
                                       const uint32_t & oru32_AdditionalElements,
                                       const bool oq_HandleSharedDatapools = true);
   int32_t InsertDataPoolListElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_DataPoolListIndex,
                                     const uint32_t & oru32_DataPoolListElementIndex,
                                     const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscContent,
                                     const C_PuiSdNodeDataPoolListElement & orc_UiContent,
                                     const bool oq_HandleSharedDatapools = true);
   int32_t RemoveDataPoolListElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_DataPoolListIndex,
                                     const uint32_t & oru32_DataPoolListElementIndex,
                                     const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                  const uint32_t & oru32_DataPoolListIndex,
                                  const uint32_t & oru32_DataPoolListElementIndex,
                                  const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscContent,
                                  const C_PuiSdNodeDataPoolListElement & orc_UiContent,
                                  const bool oq_HandleSharedDatapools = true);
   int32_t GetDataPoolListElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                  const uint32_t & oru32_DataPoolListIndex,
                                  const uint32_t & oru32_DataPoolListElementIndex,
                                  stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscContent,
                                  C_PuiSdNodeDataPoolListElement & orc_UiContent) const;
   static void h_InitDataElement(const stw::opensyde_core::C_OscNodeDataPool::E_Type & ore_Type, const bool oq_IsSafety,
                                 stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscElement);
   static QString h_GetElementTypeName(const stw::opensyde_core::C_OscNodeDataPool::E_Type & ore_Type);
   int32_t SetDataPoolListElementMinArray(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                          const uint32_t & oru32_DataPoolListIndex,
                                          const uint32_t & oru32_DataPoolListElementIndex,
                                          const uint32_t & oru32_ArrayIndex,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_OscContent,
                                          const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListElementMaxArray(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                          const uint32_t & oru32_DataPoolListIndex,
                                          const uint32_t & oru32_DataPoolListElementIndex,
                                          const uint32_t & oru32_ArrayIndex,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_OscContent,
                                          const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListElementDataSetArray(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                              const uint32_t & oru32_DataPoolListIndex,
                                              const uint32_t & oru32_DataPoolListElementIndex,
                                              const uint32_t & oru32_DataPoolListElementDataSetIndex,
                                              const uint32_t & oru32_ArrayIndex,
                                              const stw::opensyde_core::C_OscNodeDataPoolContent & orc_OscContent,
                                              const bool oq_HandleSharedDatapools = true);
   int32_t SetDataPoolListElementNvmValue(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                          const uint32_t & oru32_DataPoolListIndex,
                                          const uint32_t & oru32_DataPoolListElementIndex,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_OscContent);
   int32_t SetDataPoolListElementNvmValueChanged(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                                 const uint32_t & oru32_DataPoolListIndex,
                                                 const uint32_t & oru32_DataPoolListElementIndex,
                                                 const bool oq_NvmValueChanged);
   const stw::opensyde_core::C_OscNodeDataPoolListElement * GetOscDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                                                      const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListElementIndex)
   const;
   const stw::opensyde_core::C_OscNodeDataPoolListElement * GetOscDataPoolListElement(
      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id) const;
   const C_PuiSdNodeDataPoolListElement * GetUiDataPoolListElement(const uint32_t & oru32_NodeIndex,
                                                                   const uint32_t & oru32_DataPoolIndex,
                                                                   const uint32_t & oru32_DataPoolListIndex,
                                                                   const uint32_t & oru32_DataPoolListElementIndex)
   const;
   int32_t MoveDataPoolListElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                   const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_SourceIndex,
                                   const uint32_t & oru32_TargetIndex, const bool oq_HandleSharedDatapools = true);
   bool CheckNodeDataPoolListElementNameAvailable(const uint32_t & oru32_NodeIndex,
                                                  const uint32_t & oru32_DataPoolIndex,
                                                  const uint32_t & oru32_ListIndex,
                                                  const stw::scl::C_SclString & orc_Name, const uint32_t * const opu32_DataPoolListElementIndexToSkip =
                                                     NULL) const;

protected:
   C_PuiSdHandlerNodeLogic(QObject * const opc_Parent = NULL);

   std::map<stw::scl::C_SclString, bool> m_GetExistingNodeNames(void) const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingNodeApplicationNames(const uint32_t & oru32_NodeIndex)
   const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingNodeDataPoolNames(const uint32_t & oru32_NodeIndex)
   const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingNodeDataPoolListNames(const uint32_t & oru32_NodeIndex,
                                                                            const uint32_t & oru32_DataPoolIndex)
   const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingNodeDataPoolListDataSetNames(const uint32_t & oru32_NodeIndex,
                                                                                   const uint32_t & oru32_DataPoolIndex,
                                                                                   const uint32_t & oru32_DataPoolListIndex)
   const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingNodeDataPoolListVariableNames(const uint32_t & oru32_NodeIndex,
                                                                                    const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_DataPoolListIndex)
   const;
   void m_SyncNodeApplicationAdded(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   void m_SyncNodeApplicationMoved(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationSourceIndex,
                                   const uint32_t ou32_ApplicationTargetIndex);
   void m_SyncNodeApplicationAboutToBeDeleted(const uint32_t ou32_NodeIndex, const uint32_t ou32_ApplicationIndex);
   int32_t m_HandleNodeAutomatedProgrammableApplicationUpdate(const uint32_t ou32_NodeIndex);

   void m_SetUpComDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                           const bool & orq_AllowDataAdaptation = false, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocolType =
                              stw::opensyde_core::C_OscCanProtocol::eLAYER2);
   void m_CleanUpComDataPool(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex);
   virtual void m_SyncOsyNodeIdChange(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                      const uint8_t ou8_NewNodeId) = 0;

private:
   // NVM base Datapool helper functions
   static void mh_GetNodeNvmDataPoolAreas(const stw::opensyde_core::C_OscNode & orc_Node,
                                          std::vector<C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas);
   static void mh_AddAndAdaptNvmDataPoolArea(C_PuiSdHandlerNodeLogicNvmArea & orc_CurrentArea,
                                             std::vector<C_PuiSdHandlerNodeLogicNvmArea> & orc_Areas);
   static void mh_MergeNvmDataPoolAreas(const C_PuiSdHandlerNodeLogicNvmArea & orc_AreaToAdd,
                                        C_PuiSdHandlerNodeLogicNvmArea & orc_AreaToMerge);
   void m_SetOscNodeIds(const uint32_t ou32_NodeIndex, const std::vector<uint8_t> & orc_NodeIds);
   void m_SetOscNodeId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const uint8_t ou8_NodeId);
   void m_SetDataPoolListSharedSync(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                    const uint32_t & oru32_DataPoolListIndex,
                                    const stw::opensyde_core::C_OscNodeDataPoolList & orc_OscContent,
                                    const C_PuiSdNodeDataPoolList & orc_UiContent);
   void m_SetDataPoolListNvmCrcSharedSync(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                          const uint32_t & oru32_DataPoolListIndex, const bool oq_Value);
   void m_SetDataPoolListElementSharedSync(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                           const uint32_t & oru32_DataPoolListIndex,
                                           const uint32_t & oru32_DataPoolListElementIndex,
                                           const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscContent,
                                           const C_PuiSdNodeDataPoolListElement & orc_UiContent);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
