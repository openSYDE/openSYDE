//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus logic part of UI system definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERBUSLOGIC_H
#define C_PUISDHANDLERBUSLOGIC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerHALC.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerBusLogic :
   public C_PuiSdHandlerHALC
{
public:
   //Bus general
   stw_types::uint32 AddBusAndSort(stw_opensyde_core::C_OSCSystemBus & orc_OSCBus, const C_PuiSdBus & orc_UIBus,
                                   const QString * const opc_Name = NULL, const bool oq_AllowBusIdAdaption = true);
   std::map<stw_scl::C_SCLString, bool> GetExistingBusNames(void) const;
   void SetUIBus(const stw_types::uint32 ou32_Index, const C_PuiSdBus & orc_Item);
   void SetOSCBus(const stw_types::uint32 ou32_Index, const stw_opensyde_core::C_OSCSystemBus & orc_Item);
   const C_PuiSdBus * GetUIBus(const stw_types::uint32 & oru32_Index) const;
   const stw_opensyde_core::C_OSCSystemBus * GetOSCBus(const stw_types::uint32 & oru32_Index) const;
   bool CheckBusNameAvailable(const stw_scl::C_SCLString & orc_Name,
                              const stw_types::uint32 * const opu32_BusIndexToSkip = NULL,
                              std::vector<stw_scl::C_SCLString> * const opc_ExistingNames = NULL) const;
   void RemoveBus(const stw_types::uint32 ou32_BusIndex);
   bool CheckBusConflict(const stw_types::uint32 ou32_BusIndex) const;
   stw_types::sint32 CheckBusConflictDetailed(const stw_types::uint32 ou32_BusIndex,
                                              bool * const opq_NameConflict = NULL, bool * const opq_NameEmpty = NULL,
                                              bool * const opq_IdInvalid = NULL,
                                              std::vector<QString> * const opc_InvalidNodesForBitRate = NULL, std::vector<stw_opensyde_core::C_OSCCanProtocol::E_Type> * const opc_InvalidProtocols =
                                                 NULL) const;
   stw_types::uint32 GetOSCBusesSize(void) const;

   //Connections
   void AddConnection(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceNumber,
                      const stw_types::uint8 & oru8_NodeId, const stw_types::uint32 ou32_BusIndex);
   void RemoveConnection(const stw_types::uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID);
   void ChangeConnection(const stw_types::uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_ID,
                         const stw_types::uint8 ou8_NewInterface, const stw_types::uint8 & oru8_NodeId);
   void ChangeCompleteConnection(const stw_types::uint32 ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_PrevID,
                                 const C_PuiSdNodeConnectionId & orc_NewID, const stw_types::uint8 & oru8_NodeId,
                                 const stw_types::uint32 & oru32_BusIndex = 0xFFFFFFFFUL);
   void SetUINodeConnections(const stw_types::uint32 ou32_NodeIndex,
                             const std::vector<C_PuiSdNodeConnection> & orc_Connections);
   void SetUINodeConnectionId(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ConnectionIndex,
                              const C_PuiSdNodeConnectionId & orc_Id);

   //Can protocol getter
   const stw_opensyde_core::C_OSCCanProtocol * GetCanProtocol(const stw_types::uint32 & oru32_NodeIndex,
                                                              const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                                              const stw_types::uint32 ou32_DatapoolIndex) const;
   const stw_opensyde_core::C_OSCNodeDataPool * GetOSCCanDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                                                  const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType, const stw_types::uint32 & oru32_DatapoolIndex)
   const;
   std::vector<const stw_opensyde_core::C_OSCNodeDataPool *> GetOSCCanDataPools(
      const stw_types::uint32 & oru32_NodeIndex,
      const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType) const;
   const C_PuiSdNodeDataPool * GetUiCanDataPool(const stw_types::uint32 & oru32_NodeIndex,
                                                const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                                const stw_types::uint32 & oru32_DatapoolIndex) const;
   const C_PuiSdNodeCanProtocol * GetUiCanProtocolConst(const stw_types::uint32 & oru32_NodeIndex,
                                                        const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                                        const stw_types::uint32 ou32_DatapoolIndex)
   const;
   const stw_opensyde_core::C_OSCCanMessageContainer * GetCanProtocolMessageContainer(
      const stw_types::uint32 & oru32_NodeIndex, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
      const stw_types::uint32 & oru32_InterfaceIndex, const stw_types::uint32 ou32_DatapoolIndex) const;
   std::vector<const stw_opensyde_core::C_OSCCanMessageContainer *> GetCanProtocolMessageContainers(
      const stw_types::uint32 & oru32_NodeIndex, const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
      const stw_types::uint32 & oru32_InterfaceIndex) const;
   stw_types::sint32 GetCanProtocolType(const stw_types::uint32 ou32_NodeIndex,
                                        const stw_types::uint32 ou32_DatapoolIndex,
                                        stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType) const;
   const stw_opensyde_core::C_OSCCanMessage * GetCanMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   stw_types::sint32 GetCanMessageComplete(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      stw_opensyde_core::C_OSCCanMessage & orc_Message,
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons,
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
      std::vector<C_PuiSdNodeCanSignal> & orc_UISignals) const;
   const stw_opensyde_core::C_OSCNodeDataPoolList * GetOSCCanDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                                                          const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType, const stw_types::uint32 & oru32_InterfaceIndex, const stw_types::uint32 ou32_DatapoolIndex,
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

   //Can protocol setter
   void SetCanProtocolMessageContainerConnected(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ComType,
                                                const stw_types::uint32 ou32_InterfaceIndex, const bool oq_Value);
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
   stw_types::sint32 SetCanSignalMUXValue(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const stw_types::uint32 & oru32_SignalIndex,
                                          const stw_types::uint16 ou16_MultiplexValue);
   stw_types::sint32 SetCanSignalPosition(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const stw_types::uint32 & oru32_SignalIndex,
                                          const stw_opensyde_core::C_OSCCanSignal & orc_OSCSignal,
                                          const C_PuiSdNodeCanSignal & orc_UISignal);

   //Can protocol add/delete/move
   stw_types::sint32 AddCanMessage(const stw_types::uint32 & oru32_NodeIndex,
                                   const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                   const stw_types::uint32 & oru32_InterfaceIndex,
                                   const stw_types::uint32 ou32_DatapoolIndex, const bool & orq_MessageIsTx,
                                   const stw_opensyde_core::C_OSCCanMessage & orc_Message, const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement>
                                   & orc_OSCSignalCommons,
                                   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                   const C_PuiSdNodeCanMessage & orc_UIMessage,
                                   const bool & orq_AutomatedPropertiesAdaption);
   stw_types::sint32 InsertCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                      const stw_opensyde_core::C_OSCCanMessage & orc_Message, const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement>
                                      & orc_OSCSignalCommons,
                                      const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                      const C_PuiSdNodeCanMessage & orc_UIMessage,
                                      const bool & orq_AutomatedPropertiesAdaption);
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
   stw_types::sint32 CheckMessageMatch(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                       const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId2,
                                       bool & orq_IsMatch, const bool oq_IgnoreMessageDirection = false) const;

   //Misc util
   QString GetCanSignalDisplayName(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                   const stw_types::uint32 & oru32_SignalIndex) const;
   QString GetNamespace(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;
   QString GetSignalNamespace(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id) const;

protected:
   C_PuiSdHandlerBusLogic(QObject * const opc_Parent = NULL);

   void m_GetExistingMessageNamesProtocol(const stw_types::uint32 & oru32_NodeIndex,
                                          const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                          const stw_types::uint32 & oru32_InterfaceIndex, std::map<stw_scl::C_SCLString, bool>
                                          & orc_ExistingNames) const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingMessageNames(const stw_types::uint32 & oru32_NodeIndex,
                                                                  const stw_types::uint32 & oru32_InterfaceIndex) const;
   std::map<stw_scl::C_SCLString, bool> m_GetExistingSignalNames(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   C_PuiSdNodeCanProtocol * m_GetUiCanProtocol(const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                               const stw_types::uint32 ou32_DatapoolIndex);
   const C_PuiSdNodeDataPoolList  * m_GetUiCanDataPoolList(const stw_types::uint32 & oru32_NodeIndex,
                                                           const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType, const stw_types::uint32 & oru32_InterfaceIndex, const stw_types::uint32 ou32_DatapoolIndex, const bool & orq_MessageIsTx)
   const;
   stw_types::sint32 m_InsertUiCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons,
                                          const C_PuiSdNodeCanMessage & orc_UIMessage);
   stw_types::sint32 m_DeleteUiCanMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
