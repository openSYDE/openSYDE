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
#include "C_PuiSdHandlerHalc.hpp"
#include "C_PuiSdNodeInterfaceAutomaticProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerBusLogic :
   public C_PuiSdHandlerHalc
{
public:
   //Bus general
   uint32_t AddBusAndSort(stw::opensyde_core::C_OscSystemBus & orc_OscBus, const C_PuiSdBus & orc_UiBus,
                          const QString * const opc_Name = NULL, const bool oq_AllowBusIdAdaption = true);
   std::map<stw::scl::C_SclString, bool> GetExistingBusNames(void) const;
   void SetUiBus(const uint32_t ou32_Index, const C_PuiSdBus & orc_Item);
   void SetOscBus(const uint32_t ou32_Index, const stw::opensyde_core::C_OscSystemBus & orc_Item);
   const C_PuiSdBus * GetUiBus(const uint32_t & oru32_Index) const;
   const stw::opensyde_core::C_OscSystemBus * GetOscBus(const uint32_t & oru32_Index) const;
   bool CheckBusNameAvailable(const stw::scl::C_SclString & orc_Name,
                              const uint32_t * const opu32_BusIndexToSkip = NULL,
                              std::vector<stw::scl::C_SclString> * const opc_ExistingNames = NULL) const;
   void RemoveBus(const uint32_t ou32_BusIndex);
   bool CheckBusConflict(const uint32_t ou32_BusIndex) const;
   int32_t CheckBusConflictDetailed(const uint32_t ou32_BusIndex, bool * const opq_NameConflict = NULL,
                                    bool * const opq_NameEmpty = NULL, bool * const opq_IdInvalid = NULL,
                                    std::vector<QString> * const opc_InvalidNodesForBitRate = NULL, std::vector<stw::opensyde_core::C_OscCanProtocol::E_Type> * const opc_InvalidProtocols =
                                       NULL) const;
   uint32_t GetOscBusesSize(void) const;
   int32_t SetAutomaticBusRoutingSettings(const uint32_t ou32_BusIndex);
   int32_t SetAutomaticNodeInterfaceRoutingSettings(const uint32_t ou32_NodeIndex,
                                                    const opensyde_core::C_OscSystemBus::E_Type oe_ComType,
                                                    const uint8_t ou8_InterfaceNumber);
   int32_t SetAutomaticNodeInterfaceRoutingSettings(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex);

   //Connections
   void AddConnection(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                      const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                      const uint32_t ou32_BusIndex);
   void RemoveConnection(const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_Id);
   void ChangeConnection(const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_Id,
                         const uint8_t ou8_NewInterface,
                         const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);
   void ChangeCompleteConnection(const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_PrevId,
                                 const C_PuiSdNodeConnectionId & orc_NewId,
                                 const std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties,
                                 const uint32_t & oru32_BusIndex = 0xFFFFFFFFUL,
                                 const bool oq_IncludeCanOpenSync = true);
   void SetUiNodeConnections(const uint32_t ou32_NodeIndex, const std::vector<C_PuiSdNodeConnection> & orc_Connections);
   void SetUiNodeConnectionId(const uint32_t ou32_NodeIndex, const uint32_t ou32_ConnectionIndex,
                              const C_PuiSdNodeConnectionId & orc_Id);

   //Can protocol getter
   const stw::opensyde_core::C_OscCanProtocol * GetCanProtocol(const uint32_t & oru32_NodeIndex,
                                                               const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
                                                               const uint32_t ou32_DatapoolIndex) const;
   const stw::opensyde_core::C_OscNodeDataPool * GetOscCanDataPool(const uint32_t & oru32_NodeIndex,
                                                                   const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType, const uint32_t & oru32_DatapoolIndex)
   const;
   std::vector<const stw::opensyde_core::C_OscNodeDataPool *> GetOscCanDataPools(const uint32_t & oru32_NodeIndex,
                                                                                 const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType)
   const;
   const C_PuiSdNodeDataPool * GetUiCanDataPool(const uint32_t & oru32_NodeIndex,
                                                const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
                                                const uint32_t & oru32_DatapoolIndex) const;
   const C_PuiSdNodeDataPoolList * GetUiCanDataPoolList(const uint32_t ou32_NodeIndex,
                                                        const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ComType,
                                                        const uint32_t ou32_InterfaceIndex,
                                                        const uint32_t ou32_DatapoolIndex,
                                                        const bool oq_MessageIsTx) const;
   const C_PuiSdNodeCanProtocol * GetUiCanProtocolConst(const uint32_t & oru32_NodeIndex,
                                                        const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType, const uint32_t ou32_DatapoolIndex)
   const;
   const stw::opensyde_core::C_OscCanMessageContainer * GetCanProtocolMessageContainer(const uint32_t & oru32_NodeIndex,
                                                                                       const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType, const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex)
   const;
   std::vector<const stw::opensyde_core::C_OscCanMessageContainer *> GetCanProtocolMessageContainers(
      const uint32_t & oru32_NodeIndex, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
      const uint32_t & oru32_InterfaceIndex) const;
   int32_t GetCanProtocolType(const uint32_t ou32_NodeIndex, const uint32_t ou32_DatapoolIndex,
                              stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType) const;
   const stw::opensyde_core::C_OscCanMessage * GetCanMessage(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   int32_t GetCanMessageComplete(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                 stw::opensyde_core::C_OscCanMessage & orc_Message,
                                 std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                                 std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                 std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals,
                                 const bool oq_ChangeSignalIndicesToOutput = false) const;
   const stw::opensyde_core::C_OscNodeDataPoolList * GetOscCanDataPoolList(const uint32_t & oru32_NodeIndex,
                                                                           const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType, const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
                                                                           const bool & orq_MessageIsTx) const;
   const stw::opensyde_core::C_OscCanSignal * GetCanSignal(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t & oru32_SignalIndex) const;
   const C_PuiSdNodeCanMessage * GetUiCanMessage(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   const C_PuiSdNodeCanSignal * GetUiCanSignal(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t & oru32_SignalIndex) const;
   const stw::opensyde_core::C_OscNodeDataPoolListElement * GetOscCanDataPoolListElement(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t & oru32_SignalIndex) const;
   const C_PuiSdNodeDataPoolListElement  * GetUiCanDataPoolListElement(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t & oru32_SignalIndex)
   const;
   const stw::opensyde_gui_logic::C_PuiSdNodeConnection * GetUiConnection(const uint32_t ou32_NodeIndex,
                                                                          const uint32_t ou32_InterfaceIndex,
                                                                          const stw::opensyde_core::C_OscSystemBus::E_Type oe_Type)
   const;
   int32_t GetCanSignalComplete(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint32_t & oru32_SignalIndex, stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                                C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                                C_PuiSdNodeCanSignal & orc_UiSignal) const;

   //Can protocol setter
   void SetCanProtocolMessageContainerConnected(const uint32_t ou32_NodeIndex,
                                                const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ComType,
                                                const uint32_t ou32_InterfaceIndex, const bool oq_Value);
   int32_t SetCanMessageProperties(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                   const bool & orq_NewMessageIsTx,
                                   const stw::opensyde_core::C_OscCanMessage & orc_Message);
   int32_t SetUiCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const C_PuiSdNodeCanMessage & orc_Message);
   int32_t SetCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const uint32_t & oru32_SignalIndex, const stw::opensyde_core::C_OscCanSignal & orc_OscSignal,
                        const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                        const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                        const C_PuiSdNodeCanSignal & orc_UiSignal);
   int32_t SetCanSignalMuxValue(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint32_t & oru32_SignalIndex, const uint16_t ou16_MultiplexValue);
   int32_t SetCanSignalPosition(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint32_t & oru32_SignalIndex,
                                const stw::opensyde_core::C_OscCanSignal & orc_OscSignal,
                                const C_PuiSdNodeCanSignal & orc_UiSignal);

   //Can protocol add/delete/move
   int32_t AddCanMessage(const uint32_t & oru32_NodeIndex,
                         const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
                         const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
                         const bool & orq_MessageIsTx, const stw::opensyde_core::C_OscCanMessage & orc_Message, const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement>
                         & orc_OscSignalCommons,
                         const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                         const C_PuiSdNodeCanMessage & orc_UiMessage,
                         const bool & orq_AutomatedPropertiesAdaption);
   int32_t InsertCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                            const stw::opensyde_core::C_OscCanMessage & orc_Message, const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement>
                            & orc_OscSignalCommons,
                            const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                            const C_PuiSdNodeCanMessage & orc_UiMessage,
                            const bool & orq_AutomatedPropertiesAdaption);
   int32_t DeleteCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   int32_t AddCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                        const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                        const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                        const C_PuiSdNodeCanSignal & orc_UiSignal);
   int32_t InsertCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint32_t & oru32_SignalIndex, const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                           const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                           const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                           const C_PuiSdNodeCanSignal & orc_UiSignal);
   int32_t DeleteCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint32_t & oru32_SignalIndex);

   //Misc util
   void ConvertElementIndexToSignalIndex(
      const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_NodeDatapoolListElementId,
      stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      uint32_t & oru32_SignalIndex) const;
   int32_t CheckMessageMatch(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId1,
                             const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId2,
                             bool & orq_IsMatch, const bool oq_IgnoreMessageDirection = false) const;
   int32_t MapBusNameToIndex(const QString & orc_BusName, uint32_t & oru32_BusIndex) const;
   int32_t MapBusIndexToName(const uint32_t ou32_BusIndex, QString & orc_BusName) const;
   QString GetCanSignalDisplayName(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                   const uint32_t & oru32_SignalIndex, const bool oq_ToolTip) const;
   QString GetCanMessageDisplayName(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                    const bool oq_ToolTip) const;

protected:
   C_PuiSdHandlerBusLogic(QObject * const opc_Parent = NULL);

   void m_GetExistingMessageNamesProtocol(const uint32_t & oru32_NodeIndex,
                                          const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
                                          const uint32_t & oru32_InterfaceIndex, std::map<stw::scl::C_SclString, bool>
                                          & orc_ExistingNames) const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingMessageNames(const uint32_t & oru32_NodeIndex,
                                                                   const uint32_t & oru32_InterfaceIndex) const;
   std::map<stw::scl::C_SclString, bool> m_GetExistingSignalNames(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   C_PuiSdNodeCanProtocol * m_GetUiCanProtocol(const uint32_t & oru32_NodeIndex,
                                               const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
                                               const uint32_t ou32_DatapoolIndex);
   const C_PuiSdNodeDataPoolList  * m_GetUiCanDataPoolList(const uint32_t & oru32_NodeIndex,
                                                           const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType, const uint32_t & oru32_InterfaceIndex, const uint32_t ou32_DatapoolIndex, const bool & orq_MessageIsTx)
   const;
   int32_t m_InsertUiCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                                const C_PuiSdNodeCanMessage & orc_UiMessage);
   int32_t m_DeleteUiCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   virtual void m_HandleChangeConnectionForCanOpen(const uint32_t ou32_NodeIndex,
                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                   const uint8_t ou8_NewInterface) = 0;
   virtual void m_HandleChangeCompleteConnectionForCanOpen(const uint32_t ou32_NodeIndex,
                                                           const C_PuiSdNodeConnectionId & orc_PrevId,
                                                           const C_PuiSdNodeConnectionId & orc_NewId) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
