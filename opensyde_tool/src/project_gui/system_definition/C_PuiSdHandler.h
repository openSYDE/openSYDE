//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI topology: stores additional UI information (header)

   UI topology: stores additional UI information (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDHANDLERH
#define C_PUISDHANDLERH

/* -- Includes ------------------------------------------------------------- */

#include <QObject>
#include <QString>
#include <QFont>
#include <QPixmap>
#include "C_OSCSystemDefinition.h"
#include "C_PuiSdNode.h"
#include "C_PuiSdBus.h"
#include "C_PuiBsBoundary.h"
#include "C_PuiBsImage.h"
#include "C_PuiBsLineArrow.h"
#include "C_PuiBsTextElement.h"
#include "C_PuiSdTextElementBus.h"
#include "stwtypes.h"
#include "C_SdTopologyDataSnapshot.h"
#include "C_PuiBsElements.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdHandler :
   public QObject
{
   Q_OBJECT

public:
   void SetOSCNodeEthernetConfiguration(const stw_types::uint32 ou32_NodeIndex,
                                        const stw_types::uint32 ou32_InterfaceIndex,
                                        const std::vector<stw_types::sint32> & orc_Ip,
                                        const std::vector<stw_types::sint32> & orc_NetMask,
                                        const std::vector<stw_types::sint32> & orc_DefaultGateway);
   void SetUIBus(const stw_types::uint32 ou32_Index, const C_PuiSdBus & orc_Item);
   void SetOSCBus(const stw_types::uint32 ou32_Index, const stw_opensyde_core::C_OSCSystemBus & orc_Item);
   const C_PuiSdNode * GetUINode(const stw_types::uint32 & oru32_Index) const;
   const stw_opensyde_core::C_OSCNode * GetOSCNodeConst(const stw_types::uint32 & oru32_Index) const;
   stw_types::sint32 GetSortedOSCNodeConst(const stw_types::uint32 & oru32_Index,
                                           stw_opensyde_core::C_OSCNode & orc_Node) const;
   stw_opensyde_core::C_OSCNode * GetOSCNode(const stw_types::uint32 & oru32_Index);
   stw_types::sint32 SetStwFlashloaderSettings(const stw_types::uint32 ou32_Index,
                                               const stw_opensyde_core::C_OSCNodeStwFlashloaderSettings & orc_Settings);
   stw_types::sint32 SetNodeOpenSYDEServerSettings(const stw_types::uint32 ou32_Index,
                                                   const stw_opensyde_core::C_OSCNodeOpenSYDEServerSettings & orc_Settings);
   const C_PuiSdBus * GetUIBus(const stw_types::uint32 & oru32_Index) const;
   const stw_opensyde_core::C_OSCSystemBus * GetOSCBus(const stw_types::uint32 & oru32_Index) const;
   bool CheckNodeNameAvailable(const stw_scl::C_SCLString & orc_Name,
                               const stw_types::uint32 * const opu32_NodeIndexToSkip = NULL) const;
   bool CheckBusNameAvailable(const stw_scl::C_SCLString & orc_Name,
                              const stw_types::uint32 * const opu32_BusIndexToSkip = NULL) const;
   bool CheckNodeDataPoolNameAvailable(const stw_types::uint32 & oru32_NodeIndex, const stw_scl::C_SCLString & orc_Name,
                                       const stw_types::uint32 * const opu32_DataPoolIndexToSkip = NULL) const;
   bool CheckNodeDataPoolListNameAvailable(const stw_types::uint32 & oru32_NodeIndex,
                                           const stw_types::uint32 & oru32_DataPoolIndex,
                                           const stw_scl::C_SCLString & orc_Name,
                                           const stw_types::uint32 * const opu32_DataPoolListIndexToSkip = NULL) const;
   bool CheckNodeDataPoolListElementNameAvailable(const stw_types::uint32 & oru32_NodeIndex,
                                                  const stw_types::uint32 & oru32_DataPoolIndex,
                                                  const stw_types::uint32 & oru32_ListIndex,
                                                  const stw_scl::C_SCLString & orc_Name, const stw_types::uint32 * const opu32_DataPoolListElementIndexToSkip =
                                                     NULL) const;
   bool CheckNodeDataPoolListDataSetNameAvailable(const stw_types::uint32 & oru32_NodeIndex,
                                                  const stw_types::uint32 & oru32_DataPoolIndex,
                                                  const stw_types::uint32 & oru32_ListIndex,
                                                  const stw_scl::C_SCLString & orc_Name, const stw_types::uint32 * const opu32_DataPoolListDataSetIndexToSkip =
                                                     NULL) const;

   void AddConnection(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceNumber,
                      const stw_types::uint8 & oru8_NodeId, const stw_types::uint32 ou32_BusIndex);
   void RemoveConnection(const stw_types::uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID);
   void ChangeConnection(const stw_types::uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID,
                         const stw_types::uint8 ou8_NewInterface, const stw_types::uint8 & oru8_NodeId);
   void ChangeCompleteConnection(const stw_types::uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_PrevID,
                                 const C_PuiSdNodeConnectionId & orc_NewID, const stw_types::uint8 & oru8_NodeId,
                                 const stw_types::uint32 & oru32_BusIndex = 0xFFFFFFFFUL);
   void SetOSCNodeProperties(const stw_types::uint32 ou32_NodeIndex,
                             const stw_opensyde_core::C_OSCNodeProperties & orc_Properties);
   void SetOSCNodePropertiesDetailed(const stw_types::uint32 ou32_NodeIndex, const QString & orc_Name,
                                     const QString & orc_Comment,
                                     const stw_opensyde_core::C_OSCNodeProperties::E_DiagnosticServerProtocol oe_DiagnosticServer, const stw_opensyde_core::C_OSCNodeProperties::E_FlashLoaderProtocol oe_FlashLoader, const std::vector<stw_types::uint8> & orc_NodeIds, const std::vector<bool> & orc_UpdateFlags, const std::vector<bool> & orc_RoutingFlags, const std::vector<bool> & orc_DiagnosisFlags);
   void SetUINodeBox(const stw_types::uint32 ou32_NodeIndex, const C_PuiBsBox & orc_Box);
   void SetUINodeConnections(const stw_types::uint32 ou32_NodeIndex,
                             const std::vector<C_PuiSdNodeConnection> & orc_Connections);
   void SetUINodeConnectionId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ConnectionIndex,
                              const C_PuiSdNodeConnectionId & orc_Id);
   stw_types::uint32 AddNodeAndSort(stw_opensyde_core::C_OSCNode & orc_OSCNode, const C_PuiSdNode & orc_UINode);
   void RemoveNode(const stw_types::uint32 ou32_NodeIndex);
   stw_types::uint32 AddBusAndSort(stw_opensyde_core::C_OSCSystemBus & orc_OSCBus, const C_PuiSdBus & orc_UIBus,
                                   const QString * const opc_Name = NULL, const bool oq_AllowBusIdAdaption = true);
   std::map<stw_scl::C_SCLString, bool> GetExistingBusNames(void) const;
   std::vector<const stw_opensyde_core::C_OSCNodeApplication *> GetProgrammableApplications(
      const stw_types::uint32 ou32_NodeIndex) const;
   std::vector<stw_types::uint32> GetProgrammableAppIndices(const stw_types::uint32 ou32_NodeIndex) const;
   void RemoveBus(const stw_types::uint32 ou32_BusIndex);
   bool CheckNodeConflict(const stw_types::uint32 & oru32_NodeIndex)
   const;
   bool CheckNodeNvmDataPoolsSizeConflict(const stw_types::uint32 ou32_NodeIndex) const;
   bool CheckBusConflict(const stw_types::uint32 ou32_BusIndex) const;
   stw_types::sint32 CheckBusConflictDetailed(const stw_types::uint32 ou32_BusIndex,
                                              bool * const opq_NameConflict = NULL, bool * const opq_NameEmpty = NULL,
                                              bool * const opq_IdInvalid = NULL,
                                              std::vector<QString> * const opc_InvalidNodesForBitRate = NULL, std::vector<stw_opensyde_core::C_OSCCanProtocol::E_Type> * const opc_InvalidProtocols =
                                                 NULL) const;
   void GetSupportedCanBitrates(const std::vector<stw_types::uint32> & orc_Nodes,
                                std::vector<stw_types::uint32> & orc_Bitrates) const;
   stw_types::sint32 AddDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                 const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                                 const C_PuiSdNodeDataPool & orc_UIContent, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                                    stw_opensyde_core::C_OSCCanProtocol::eLAYER2,
                                 const bool & orq_AllowNameAdaptation = true,
                                 const bool & orq_AllowDataAdaptation = true);
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
   stw_types::sint32 InsertDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                        const stw_types::uint32 & oru32_DataPoolListIndex,
                                        const stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCContent,
                                        const C_PuiSdNodeDataPoolList & orc_UIContent);
   stw_types::sint32 RemoveDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                        const stw_types::uint32 & oru32_DataPoolListIndex);
   stw_types::sint32 SetDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     const stw_types::uint32 & oru32_DataPoolListIndex,
                                     const stw_opensyde_core::C_OSCNodeDataPoolList & orc_OSCContent,
                                     const C_PuiSdNodeDataPoolList & orc_UIContent);
   stw_types::sint32 SetDataPoolListName(const stw_types::uint32 & oru32_NodeIndex,
                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                         const stw_types::uint32 & oru32_DataPoolListIndex, const QString & orc_Value);
   stw_types::sint32 SetDataPoolListComment(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const QString & orc_Value);
   stw_types::sint32 SetDataPoolListNVMSize(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const stw_types::uint32 ou32_Value);
   stw_types::sint32 SetDataPoolListNVMCRC(const stw_types::uint32 & oru32_NodeIndex,
                                           const stw_types::uint32 & oru32_DataPoolIndex,
                                           const stw_types::uint32 & oru32_DataPoolListIndex, const bool oq_Value);
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
                                               const stw_opensyde_core::C_OSCNodeDataPoolDataSet & orc_OSCContent);
   stw_types::sint32 InsertDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListDataSetIndex,
                                               const stw_opensyde_core::C_OSCNodeDataPoolDataSet & orc_OSCName,
                                               const std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_OSCValues);
   stw_types::sint32 RemoveDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListDataSetIndex);
   stw_types::sint32 MoveDataPoolListDataSet(const stw_types::uint32 & oru32_NodeIndex,
                                             const stw_types::uint32 & oru32_DataPoolIndex,
                                             const stw_types::uint32 & oru32_DataPoolListIndex,
                                             const stw_types::uint32 & oru32_SourceIndex,
                                             const stw_types::uint32 & oru32_TargetIndex);
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
                                      const stw_types::uint32 & oru32_TargetIndex);
   stw_types::sint32 ReserveDataPoolListElements(const stw_types::uint32 & oru32_NodeIndex,
                                                 const stw_types::uint32 & oru32_DataPoolIndex,
                                                 const stw_types::uint32 & oru32_DataPoolListIndex,
                                                 const stw_types::uint32 & oru32_AdditionalElements);
   stw_types::sint32 InsertDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                               const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCContent,
                                               const C_PuiSdNodeDataPoolListElement & orc_UIContent);
   stw_types::sint32 RemoveDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_DataPoolListIndex,
                                               const stw_types::uint32 & oru32_DataPoolListElementIndex);
   stw_types::sint32 SetDataPoolListElement(const stw_types::uint32 & oru32_NodeIndex,
                                            const stw_types::uint32 & oru32_DataPoolIndex,
                                            const stw_types::uint32 & oru32_DataPoolListIndex,
                                            const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                            const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCContent,
                                            const C_PuiSdNodeDataPoolListElement & orc_UIContent);
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
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent);
   stw_types::sint32 SetDataPoolListElementMaxArray(const stw_types::uint32 & oru32_NodeIndex,
                                                    const stw_types::uint32 & oru32_DataPoolIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListIndex,
                                                    const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                    const stw_types::uint32 & oru32_ArrayIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent);
   stw_types::sint32 SetDataPoolListElementDataSetArray(const stw_types::uint32 & oru32_NodeIndex,
                                                        const stw_types::uint32 & oru32_DataPoolIndex,
                                                        const stw_types::uint32 & oru32_DataPoolListIndex,
                                                        const stw_types::uint32 & oru32_DataPoolListElementIndex,
                                                        const stw_types::uint32 & oru32_DataPoolListElementDataSetIndex,
                                                        const stw_types::uint32 & oru32_ArrayIndex,
                                                        const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_OSCContent);
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
                                             const stw_types::uint32 & oru32_TargetIndex);

   //Can protocol getter
   const stw_opensyde_core::C_OSCCanProtocol * GetCanProtocol(const stw_types::uint32 & oru32_NodeIndex,
                                                              const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType)
   const;
   const stw_opensyde_core::C_OSCNodeDataPool * GetOSCCanDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                                                  const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType)
   const;
   const C_PuiSdNodeDataPool * GetUiCanDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                                const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType) const;
   const C_PuiSdNodeCanProtocol * GetUiCanProtocolConst(const stw_types::uint32 & oru32_NodeIndex,
                                                        const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType)
   const;
   const stw_opensyde_core::C_OSCCanMessageContainer * GetCanProtocolMessageContainer(
      const stw_types::uint32 & oru32_NodeIndex, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
      const stw_types::uint32 & oru32_InterfaceIndex) const;
   const stw_opensyde_core::C_OSCCanMessage * GetCanMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   stw_types::sint32 GetCanMessageComplete(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      stw_opensyde_core::C_OSCCanMessage & orc_Message,
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
      std::vector<C_PuiSdNodeCanSignal> & orc_UISignals) const;
   const stw_opensyde_core::C_OSCNodeDataPoolList * GetOSCCanDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                                                          const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType, const stw_types::uint32 & oru32_InterfaceIndex,
                                                                          const bool & orq_MessageIsTx) const;
   const stw_opensyde_core::C_OSCCanSignal * GetCanSignal(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 & oru32_SignalIndex) const;
   const C_PuiSdNodeCanMessage * GetUiCanMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   const C_PuiSdNodeCanSignal * GetUiCanSignal(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 & oru32_SignalIndex) const;
   const stw_opensyde_core::C_OSCNodeDataPoolListElement * GetOSCCanDataPoolListElement(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 & oru32_SignalIndex) const;
   const C_PuiSdNodeDataPoolListElement  * GetUiCanDataPoolListElement(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 & oru32_SignalIndex)
   const;
   const stw_opensyde_gui_logic::C_PuiSdNodeConnection * GetUiConnection(const stw_types::uint32 ou32_NodeIndex,
                                                                         const stw_types::uint32 ou32_InterfaceIndex,
                                                                         const stw_opensyde_core::C_OSCSystemBus::E_Type oe_Type)
   const;
   stw_types::sint32 GetCanSignalComplete(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const stw_types::uint32 & oru32_SignalIndex,
                                          stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                          stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                          C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                          C_PuiSdNodeCanSignal & orc_UISignal) const;
   stw_types::sint32 CheckMessageMatch(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                       const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId2,
                                       bool & orq_IsMatch, const bool oq_IgnoreMessageDirection = false) const;

   //Can protocol setter
   void SetCanProtocolMessageContainerConnected(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ComType,
                                                const stw_types::uint32 ou32_InterfaceIndex, const bool oq_Value);
   stw_types::sint32 ChangeCanProtocolType(const stw_types::uint32 & oru32_NodeIndex,
                                           const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_PreviousComType,
                                           const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_NewComType);
   stw_types::sint32 SetCanMessageProperties(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId, const bool & orq_NewMessageIsTx,
      const stw_opensyde_core::C_OSCCanMessage & orc_Message);
   stw_types::sint32 SetUiCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     const C_PuiSdNodeCanMessage & orc_Message);
   stw_types::sint32 SetCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                  const stw_types::uint32 & oru32_SignalIndex,
                                  const stw_opensyde_core::C_OSCCanSignal & orc_OSCSignal,
                                  const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                  const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                  const C_PuiSdNodeCanSignal & orc_UISignal);
   stw_types::sint32 SetCanSignalCommon(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const stw_types::uint32 & oru32_SignalIndex,
                                        const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                        const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon);
   stw_types::sint32 SetCanSignalPosition(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const stw_types::uint32 & oru32_SignalIndex,
                                          const stw_opensyde_core::C_OSCCanSignal & orc_OSCSignal,
                                          const C_PuiSdNodeCanSignal & orc_UISignal);

   //Can protocol add/delete/move
   stw_types::sint32 AddCanMessage(const stw_types::uint32 & oru32_NodeIndex,
                                   const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                   const stw_types::uint32 & oru32_InterfaceIndex, const bool & orq_MessageIsTx,
                                   const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals, const bool & orq_AutomatedPropertiesAdaption);
   stw_types::sint32 InsertCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                      const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals, const bool & orq_AutomatedPropertiesAdaption);
   stw_types::sint32 DeleteCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   stw_types::sint32 AddCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                  const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                  const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                  const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                  const C_PuiSdNodeCanSignal & orc_UISignal);
   stw_types::sint32 InsertCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     const stw_types::uint32 & oru32_SignalIndex,
                                     const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                     const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                     const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                     const C_PuiSdNodeCanSignal & orc_UISignal);
   stw_types::sint32 DeleteCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     const stw_types::uint32 & oru32_SignalIndex);
   void ConvertElementIndexToSignalIndex(
      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
      stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      stw_types::uint32 & oru32_SignalIndex) const;

   //Misc util
   QString GetNamespace(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   QString GetSignalNamespace(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   static QString h_AutomaticCStringAdaptation(const QString & orc_Input, const bool oq_ReplaceAllCharacters);

   stw_types::uint32 GetOSCBusesSize(void) const;
   stw_types::uint32 GetOSCNodesSize(void) const;
   void Clear(void);
   C_SdTopologyDataSnapshot GetSnapshot(void) const;

   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_Path);
   stw_types::sint32 SaveToFile(const stw_scl::C_SCLString & orc_Path);
   bool HasHashChanged(void) const;
   stw_types::uint32 CalcHashSystemDefinition(void) const;
   const stw_opensyde_core::C_OSCSystemDefinition & GetOSCSystemDefinitionConst(void) const;

   static C_PuiSdHandler * h_GetInstance(void);
   static void h_Destroy(void);

   C_PuiBsElements c_Elements;
   std::vector<C_PuiSdTextElementBus> c_BusTextElements; ///< UI information for bus text elements

protected:
   stw_opensyde_core::C_OSCSystemDefinition mc_CoreDefinition; ///< Core part of system definition data

   std::vector<C_PuiSdNode> mc_UINodes; ///< UI information for nodes (should always stay synchronous with
   ///< core arrays)
   std::vector<C_PuiSdBus> mc_UIBuses; ///< UI information for buses (should always stay synchronous with
   ///< core arrays)

   //Avoid call (protected for test cases and second instance in case of copy TBD: use copy to clipboard instead)
   explicit C_PuiSdHandler(QObject * const opc_Parent = NULL);
   virtual ~C_PuiSdHandler(void);

private:
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
   std::map<stw_scl::C_SCLString, bool> m_GetExistingMessageNames(const stw_types::uint32 & oru32_NodeIndex,
                                                                  const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType, const stw_types::uint32 & oru32_InterfaceIndex)
   const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingSignalNames(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;

   void m_SetUpComDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                           const bool & orq_AllowDataAdaptation = false, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocolType =
                              stw_opensyde_core::C_OSCCanProtocol::eLAYER2);
   void m_CleanUpComDataPool(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex);
   C_PuiSdNodeCanProtocol * m_GetUiCanProtocol(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType);
   const C_PuiSdNodeDataPoolList  * m_GetUiCanDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                                           const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType, const stw_types::uint32 & oru32_InterfaceIndex, const bool & orq_MessageIsTx)
   const;
   stw_types::sint32 m_InsertUiCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                          const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals);
   stw_types::sint32 m_DeleteUiCanMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   void m_SyncNodeApplicationAdded(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_ApplicationIndex);
   void m_SyncNodeApplicationMoved(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_ApplicationSourceIndex,
                                   const stw_types::uint32 ou32_ApplicationTargetIndex);
   void m_SyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_types::uint32 ou32_ApplicationIndex);
   stw_types::sint32 m_HandleNodeAutomatedProgrammableApplicationUpdate(const stw_types::uint32 ou32_NodeIndex);
   void m_FixNameIssues(void);
   void m_FixAddressIssues(void);
   void m_FixCommInconsistencyErrors(void);
   static stw_types::sint32 mh_SortMessagesByName(stw_opensyde_core::C_OSCNode & orc_OSCNode, C_PuiSdNode & orc_UiNode);
   stw_types::uint32 m_GetHashNode(const stw_types::uint32 ou32_NodeIndex) const;
   stw_types::uint32 m_GetHashBus(const stw_types::uint32 ou32_BusIndex) const;

   //Avoid call
   C_PuiSdHandler(const C_PuiSdHandler &);
   C_PuiSdHandler & operator =(const C_PuiSdHandler &);

   static C_PuiSdHandler * mhpc_Singleton;

   stw_types::uint32 mu32_CalculatedHashSystemDefinition;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodesChanged(void);
   void SigBussesChanged(void);
   void SigSyncNodeAdded(const stw_types::uint32 ou32_Index);
   void SigSyncNodeAboutToBeDeleted(const stw_types::uint32 ou32_Index);
   void SigSyncNodeDataPoolAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeDataPoolMoved(const stw_types::uint32 ou32_NodeIndex,
                                 const stw_types::uint32 ou32_DataPoolSourceIndex,
                                 const stw_types::uint32 ou32_DataPoolTargetIndex);
   void SigSyncNodeDataPoolAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeApplicationAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeApplicationMoved(const stw_types::uint32 ou32_NodeIndex,
                                    const stw_types::uint32 ou32_DataPoolSourceIndex,
                                    const stw_types::uint32 ou32_DataPoolTargetIndex);
   void SigSyncNodeApplicationAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint32 ou32_DataPoolIndex);
   void SigSyncNodeDataPoolListAdded(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                     const stw_types::uint32 ou32_ListIndex);
   void SigSyncNodeDataPoolListMoved(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex,
                                     const stw_types::uint32 ou32_ListSourceIndex,
                                     const stw_types::uint32 ou32_ListTargetIndex);
   void SigSyncNodeDataPoolListAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_DataPoolIndex,
                                                const stw_types::uint32 ou32_ListIndex);
   void SigSyncNodeDataPoolListDataSetAdded(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_DataSetIndex);
   void SigSyncNodeDataPoolListDataSetMoved(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_DataSetSourceIndex,
                                            const stw_types::uint32 ou32_DataSetTargetIndex);
   void SigSyncNodeDataPoolListDataSetAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DataPoolIndex,
                                                       const stw_types::uint32 ou32_ListIndex,
                                                       const stw_types::uint32 ou32_DataSetIndex);
   void SigSyncNodeDataPoolListElementAdded(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_ElementIndex);
   void SigSyncNodeDataPoolListElementMoved(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex,
                                            const stw_types::uint32 ou32_ElementSourceIndex,
                                            const stw_types::uint32 ou32_ElementTargetIndex);
   void SigSyncNodeDataPoolListElementChanged(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_types::uint32 ou32_DataPoolIndex,
                                              const stw_types::uint32 ou32_ListIndex,
                                              const stw_types::uint32 ou32_ElementIndex,
                                              const stw_opensyde_core::C_OSCNodeDataPoolContent::E_Type oe_Type,
                                              const bool oq_IsArray, const stw_types::uint32 ou32_ArraySize);
   void SigSyncNodeDataPoolListElementAccessChanged(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_DataPoolIndex,
                                                    const stw_types::uint32 ou32_ListIndex,
                                                    const stw_types::uint32 ou32_ElementIndex,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolListElement::E_Access oe_Access);
   void SigSyncNodeDataPoolListElementAboutToBeDeleted(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint32 ou32_DataPoolIndex,
                                                       const stw_types::uint32 ou32_ListIndex,
                                                       const stw_types::uint32 ou32_ElementIndex);
   void SigSyncBusAdded(const stw_types::uint32 ou32_Index);
   void SigSyncBusDeleted(const stw_types::uint32 ou32_Index);
   void SigSyncClear(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
