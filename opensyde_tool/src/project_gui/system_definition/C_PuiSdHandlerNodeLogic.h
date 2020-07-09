//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node logic part of UI system definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERNODELOGIC_H
#define C_PUISDHANDLERNODELOGIC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerData.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerNodeLogic :
   public C_PuiSdHandlerData
{
public:
   //Node general
   bool CheckNodeNameAvailable(const stw_scl::C_SCLString & orc_Name,
                               const stw_types::uint32 * const opu32_NodeIndexToSkip = NULL,
                               std::vector<stw_scl::C_SCLString> * const opc_ExistingNames = NULL) const;
   void SetOSCNodeEthernetConfiguration(const stw_types::uint32 ou32_NodeIndex,
                                        const stw_types::uint32 ou32_InterfaceIndex,
                                        const std::vector<stw_types::sint32> & orc_Ip,
                                        const std::vector<stw_types::sint32> & orc_NetMask,
                                        const std::vector<stw_types::sint32> & orc_DefaultGateway);
   const C_PuiSdNode * GetUINode(const stw_types::uint32 & oru32_Index) const;
   const stw_opensyde_core::C_OSCNode * GetOSCNodeConst(const stw_types::uint32 & oru32_Index) const;
   stw_types::sint32 GetSortedOSCNodeConst(const stw_types::uint32 & oru32_Index,
                                           stw_opensyde_core::C_OSCNode & orc_Node) const;
   stw_opensyde_core::C_OSCNode * GetOSCNode(const stw_types::uint32 & oru32_Index);
   stw_types::sint32 SetStwFlashloaderSettings(const stw_types::uint32 ou32_Index,
                                               const stw_opensyde_core::C_OSCNodeStwFlashloaderSettings & orc_Settings);
   stw_types::sint32 SetNodeOpenSYDEServerSettings(const stw_types::uint32 ou32_Index,
                                                   const stw_opensyde_core::C_OSCNodeOpenSYDEServerSettings & orc_Settings);
   stw_types::sint32 SetNodeCodeExportSettings(const stw_types::uint32 ou32_Index,
                                               const stw_opensyde_core::C_OSCNodeCodeExportSettings & orc_Settings);
   void SetOSCNodeProperties(const stw_types::uint32 ou32_NodeIndex,
                             const stw_opensyde_core::C_OSCNodeProperties & orc_Properties);
   void SetOSCNodePropertiesDetailed(const stw_types::uint32 ou32_NodeIndex, const QString & orc_Name,
                                     const QString & orc_Comment, const stw_opensyde_core::C_OSCNodeProperties::E_DiagnosticServerProtocol
                                     oe_DiagnosticServer,
                                     const stw_opensyde_core::C_OSCNodeProperties::E_FlashLoaderProtocol oe_FlashLoader,
                                     const std::vector<stw_types::uint8> & orc_NodeIds,
                                     const std::vector<bool> & orc_UpdateFlags,
                                     const std::vector<bool> & orc_RoutingFlags,
                                     const std::vector<bool> & orc_DiagnosisFlags);
   void SetUINodeBox(const stw_types::uint32 ou32_NodeIndex, const C_PuiBsBox & orc_Box);
   stw_types::uint32 AddNodeAndSort(stw_opensyde_core::C_OSCNode & orc_OSCNode, const C_PuiSdNode & orc_UINode);
   void RemoveNode(const stw_types::uint32 ou32_NodeIndex);
   bool CheckNodeConflict(const stw_types::uint32 & oru32_NodeIndex) const;
   void GetSupportedCanBitrates(const std::vector<stw_types::uint32> & orc_Nodes,
                                std::vector<stw_types::uint32> & orc_Bitrates) const;
   stw_types::uint32 GetOSCNodesSize(void) const;
   bool CheckCriticalNameConflict(std::vector<QString> * const opc_CriticalNodeNames,
                                  std::vector<QString> * const opc_CriticalBusNames,
                                  std::vector<QString> * const opc_CriticalDatapoolNamespaceNames) const;
   stw_types::sint32 MapNodeNameToIndex(const QString & orc_NodeName, stw_types::uint32 & oru32_NodeIndex) const;
   stw_types::sint32 MapNodeIndexToName(const stw_types::uint32 ou32_NodeIndex, QString & orc_NodeName) const;

   //Datapool
   stw_types::sint32 AddDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                 const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                                 const C_PuiSdNodeDataPool & orc_UIContent, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                                    stw_opensyde_core::C_OSCCanProtocol::eLAYER2,
                                 const bool & orq_AllowNameAdaptation = true,
                                 const bool & orq_AllowDataAdaptation = true);
   stw_types::sint32 AddAutoGenCommDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                                               stw_opensyde_core::C_OSCCanProtocol::eLAYER2);
   stw_types::sint32 InsertDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                    const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                                    const C_PuiSdNodeDataPool & orc_UIContent, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                                       stw_opensyde_core::C_OSCCanProtocol::eLAYER2,
                                    const bool & orq_AllowNameAdaptation = true,
                                    const bool & orq_AllowDataAdaptation = true);
   stw_types::sint32 RemoveDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                    const stw_types::uint32 & oru32_DataPoolIndex);
   stw_types::sint32 SetDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                 const stw_types::uint32 & oru32_DataPoolIndex,
                                 const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                                 const C_PuiSdNodeDataPool & orc_UIContent, const bool oq_NewComProtocolType = false, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                                    stw_opensyde_core::C_OSCCanProtocol::eLAYER2);
   stw_types::sint32 UnassignDataPoolApplication(const stw_types::uint32 & oru32_NodeIndex,
                                                 const stw_types::uint32 & oru32_DataPoolIndex);
   stw_types::sint32 AssignDataPoolApplication(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_DataPoolIndex,
                                               const stw_types::sint32 os32_ApplicationIndex);
   stw_types::sint32 GetDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                 const stw_types::uint32 & oru32_DataPoolIndex,
                                 stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                                 C_PuiSdNodeDataPool & orc_UIContent) const;
   stw_scl::C_SCLString GetUniqueDataPoolName(const stw_types::uint32 & oru32_NodeIndex,
                                              const stw_scl::C_SCLString & orc_Proposal) const;
   const stw_opensyde_core::C_OSCNodeDataPool * GetOSCDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                                               const stw_types::uint32 & oru32_DataPoolIndex) const;
   const C_PuiSdNodeDataPool * GetUIDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                             const stw_types::uint32 & oru32_DataPoolIndex) const;
   stw_types::sint32 GetDataPoolType(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_Type) const;
   stw_types::sint32 MoveDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_SourceIndex,
                                  const stw_types::uint32 ou32_TargetIndex);
   stw_types::sint32 GetDataPoolIndex(const stw_types::uint32 ou32_NodeIndex,
                                      const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType,
                                      const stw_types::uint32 ou32_DataPoolTypeIndex) const;
   bool CheckNodeDataPoolNameAvailable(const stw_types::uint32 & oru32_NodeIndex, const stw_scl::C_SCLString & orc_Name,
                                       const stw_types::uint32 * const opu32_DataPoolIndexToSkip = NULL,
                                       std::vector<stw_scl::C_SCLString> * const opc_ExistingDatapoolNames = NULL) const;
   bool CheckNodeNvmDataPoolsSizeConflict(const stw_types::uint32 ou32_NodeIndex) const;
   C_PuiSdSharedDatapools & GetSharedDatapools(void);
   const C_PuiSdSharedDatapools & GetSharedDatapoolsConst(void) const;

   //Data block
   stw_types::sint32 AddApplication(const stw_types::uint32 ou32_NodeIndex,
                                    stw_opensyde_core::C_OSCNodeApplication & orc_OSCContent);
   stw_types::sint32 InsertApplication(const stw_types::uint32 ou32_NodeIndex,
                                       const stw_types::uint32 ou32_ApplicationIndex,
                                       stw_opensyde_core::C_OSCNodeApplication & orc_OSCContent);
   stw_types::sint32 RemoveApplication(const stw_types::uint32 ou32_NodeIndex,
                                       const stw_types::uint32 ou32_ApplicationIndex);
   stw_types::sint32 SetApplication(const stw_types::uint32 ou32_NodeIndex,
                                    const stw_types::uint32 ou32_ApplicationIndex,
                                    const stw_opensyde_core::C_OSCNodeApplication & orc_OSCContent);
   const stw_opensyde_core::C_OSCNodeApplication * GetApplication(const stw_types::uint32 ou32_NodeIndex,
                                                                  const stw_types::uint32 ou32_ApplicationIndex)
   const;
   stw_types::sint32 MoveApplication(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_SourceIndex,
                                     const stw_types::uint32 ou32_TargetIndex);
   stw_scl::C_SCLString GetUniqueApplicationName(const stw_types::uint32 & oru32_NodeIndex,
                                                 const stw_scl::C_SCLString & orc_Proposal) const;
   stw_types::uint8 GetUniqueApplicationProcessId(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint8 ou8_Proposal) const;
   stw_types::sint32 GetNextProgrammableApplicationIndex(const stw_types::uint32 ou32_NodeIndex,
                                                         stw_types::sint32 & ors32_ApplicationIndex) const;
   stw_types::sint32 CheckApplicationName(const stw_types::uint32 ou32_NodeIndex,
                                          const stw_scl::C_SCLString & orc_ApplicationName, bool & orq_Valid,
                                          const stw_types::uint32 * const opu32_SkipApplication = NULL) const;
   std::vector<const stw_opensyde_core::C_OSCNodeApplication *> GetProgrammableApplications(
      const stw_types::uint32 ou32_NodeIndex) const;
   std::vector<stw_types::uint32> GetProgrammableAppIndices(const stw_types::uint32 ou32_NodeIndex) const;

   //Datapool list
   stw_types::sint32 InsertDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                        const stw_types::uint32 & oru32_DataPoolListIndex,
                                        const stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCContent,
                                        const C_PuiSdNodeDataPoolList & orc_UIContent,
                                        const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 RemoveDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                        const stw_types::uint32 & oru32_DataPoolListIndex,
                                        const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     const stw_types::uint32 & oru32_DataPoolListIndex,
                                     const stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCContent,
                                     const C_PuiSdNodeDataPoolList & orc_UIContent,
                                     const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListName(const stw_types::uint32 & oru32_NodeIndex,
                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                         const stw_types::uint32 & oru32_DataPoolListIndex, const QString & orc_Value,
                                         const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListComment(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const QString & orc_Value, const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListNVMSize(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const stw_types::uint32 ou32_Value,
                                            const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListNVMCRC(const stw_types::uint32 & oru32_NodeIndex,
                                           const stw_types::uint32 & oru32_DataPoolIndex,
                                           const stw_types::uint32 & oru32_DataPoolListIndex, const bool oq_Value,
                                           const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 GetDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     const stw_types::uint32 & oru32_DataPoolListIndex,
                                     stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCContent,
                                     C_PuiSdNodeDataPoolList & orc_UIContent) const;
   stw_types::sint32 GetDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const stw_types::uint32 & oru32_DataPoolListDataSetIndex,
                                            stw_opensyde_core::C_OSCNodeDataPoolDataSet & orc_OSCName,
                                            std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_OSCValues)
   const;
   const stw_opensyde_core::C_OSCNodeDataPoolDataSet * GetOSCDataPoolListDataSet(
      const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
      const stw_types::uint32 & oru32_DataPoolListIndex, const stw_types::uint32 & oru32_DataPoolListDataSetIndex)
   const;
   stw_types::sint32 SetOSCNodeDataPoolDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListDataSetIndex,
                                               const stw_opensyde_core::C_OSCNodeDataPoolDataSet & orc_OSCContent,
                                               const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 InsertDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListDataSetIndex,
                                               const stw_opensyde_core::C_OSCNodeDataPoolDataSet & orc_OSCName,
                                               const std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_OSCValues, const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 RemoveDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListDataSetIndex,
                                               const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 MoveDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                             const stw_types::uint32 & oru32_DataPoolIndex,
                                             const stw_types::uint32 & oru32_DataPoolListIndex,
                                             const stw_types::uint32 & oru32_SourceIndex,
                                             const stw_types::uint32 & oru32_TargetIndex,
                                             const bool oq_HandleSharedDatapools = true);
   const stw_opensyde_core::C_OSCNodeDataPoolList * GetOSCDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                                                       const stw_types::uint32 & oru32_DataPoolIndex,
                                                                       const stw_types::uint32 & oru32_DataPoolListIndex)
   const;
   const C_PuiSdNodeDataPoolList * GetUIDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                                     const stw_types::uint32 & oru32_DataPoolListIndex) const;
   stw_types::sint32 MoveDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                      const stw_types::uint32 & oru32_DataPoolIndex,
                                      const stw_types::uint32 & oru32_SourceIndex,
                                      const stw_types::uint32 & oru32_TargetIndex,
                                      const bool oq_HandleSharedDatapools = true);
   bool CheckNodeDataPoolListNameAvailable(const stw_types::uint32 & oru32_NodeIndex,
                                           const stw_types::uint32 & oru32_DataPoolIndex,
                                           const stw_scl::C_SCLString & orc_Name,
                                           const stw_types::uint32 * const opu32_DataPoolListIndexToSkip = NULL) const;
   bool CheckNodeDataPoolListDataSetNameAvailable(const stw_types::uint32 & oru32_NodeIndex,
                                                  const stw_types::uint32 & oru32_DataPoolIndex,
                                                  const stw_types::uint32 & oru32_ListIndex,
                                                  const stw_scl::C_SCLString & orc_Name, const stw_types::uint32 *
                                                  const opu32_DataPoolListDataSetIndexToSkip = NULL) const;

   //Datapool list element
   stw_types::sint32 ReserveDataPoolListElements(const stw_types::uint32 & oru32_NodeIndex,
                                                 const stw_types::uint32 & oru32_DataPoolIndex,
                                                 const stw_types::uint32 & oru32_DataPoolListIndex,
                                                 const stw_types::uint32 & oru32_AdditionalElements,
                                                 const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 InsertDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                               const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCContent,
                                               const C_PuiSdNodeDataPoolListElement & orc_UIContent,
                                               const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 RemoveDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                               const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                            const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCContent,
                                            const C_PuiSdNodeDataPoolListElement & orc_UIContent,
                                            const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 GetDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                            stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCContent,
                                            C_PuiSdNodeDataPoolListElement & orc_UIContent) const;
   static void h_InitDataElement(const stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_Type, const bool oq_IsSafety,
                                 stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCElement);
   static QString h_GetElementTypeName(const stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_Type);
   stw_types::sint32 SetDataPoolListElementMinArray(const stw_types::uint32 & oru32_NodeIndex,
                                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                    const stw_types::uint32 & oru32_ArrayIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent,
                                                    const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListElementMaxArray(const stw_types::uint32 & oru32_NodeIndex,
                                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                    const stw_types::uint32 & oru32_ArrayIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent,
                                                    const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListElementDataSetArray(const stw_types::uint32 & oru32_NodeIndex,
                                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                                        const stw_types::uint32 & oru32_DataPoolListIndex,
                                                        const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                        const stw_types::uint32 & oru32_DataPoolListElementDataSetIndex,
                                                        const stw_types::uint32 & oru32_ArrayIndex,
                                                        const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent, const bool oq_HandleSharedDatapools = true);
   stw_types::sint32 SetDataPoolListElementNVMValue(const stw_types::uint32 & oru32_NodeIndex,
                                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent);
   stw_types::sint32 SetDataPoolListElementNVMValueChanged(const stw_types::uint32 & oru32_NodeIndex,
                                                           const stw_types::uint32 & oru32_DataPoolIndex,
                                                           const stw_types::uint32 & oru32_DataPoolListIndex,
                                                           const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                           const bool oq_NvmValueChanged);
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCDataPoolListElement(
      const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
      const stw_types::uint32 & oru32_DataPoolListIndex,
      const stw_types::uint32 & oru32_DataPoolListElementIndex) const;
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCDataPoolListElement(
      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   const C_PuiSdNodeDataPoolListElement * GetUIDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                                                   const stw_types::uint32 & oru32_DataPoolListIndex,
                                                                   const stw_types::uint32 & oru32_DataPoolListElementIndex)
   const;
   stw_types::sint32 MoveDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                             const stw_types::uint32 & oru32_DataPoolIndex,
                                             const stw_types::uint32 & oru32_DataPoolListIndex,
                                             const stw_types::uint32 & oru32_SourceIndex,
                                             const stw_types::uint32 & oru32_TargetIndex,
                                             const bool oq_HandleSharedDatapools = true);
   bool CheckNodeDataPoolListElementNameAvailable(const stw_types::uint32 & oru32_NodeIndex,
                                                  const stw_types::uint32 & oru32_DataPoolIndex,
                                                  const stw_types::uint32 & oru32_ListIndex,
                                                  const stw_scl::C_SCLString & orc_Name, const stw_types::uint32 * const opu32_DataPoolListElementIndexToSkip =
                                                     NULL) const;

protected:
   C_PuiSdHandlerNodeLogic(QObject * const opc_Parent = NULL);

   std::map<stw_scl::C_SCLString, bool> m_GetExistingNodeNames(void) const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingNodeApplicationNames(const stw_types::uint32 & oru32_NodeIndex)
   const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingNodeDataPoolNames(const stw_types::uint32 & oru32_NodeIndex)
   const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingNodeDataPoolListNames(const stw_types::uint32 & oru32_NodeIndex,
                                                                           const stw_types::uint32 & oru32_DataPoolIndex)
   const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingNodeDataPoolListDataSetNames(
      const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
      const stw_types::uint32 & oru32_DataPoolListIndex) const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingNodeDataPoolListVariableNames(
      const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
      const stw_types::uint32 & oru32_DataPoolListIndex) const;
   void m_SyncNodeApplicationAdded(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_ApplicationIndex);
   void m_SyncNodeApplicationMoved(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_ApplicationSourceIndex,
                                   const stw_types::uint32 ou32_ApplicationTargetIndex);
   void m_SyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_types::uint32 ou32_ApplicationIndex);
   stw_types::sint32 m_HandleNodeAutomatedProgrammableApplicationUpdate(const stw_types::uint32 ou32_NodeIndex);

   void m_SetUpComDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                           const bool & orq_AllowDataAdaptation = false, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                              stw_opensyde_core::C_OSCCanProtocol::eLAYER2);
   void m_CleanUpComDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
