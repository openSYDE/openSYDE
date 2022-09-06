//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen logic part of UI system definition
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERCANOPENLOGIC_H
#define C_PUISDHANDLERCANOPENLOGIC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerBusLogic.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerCanOpenLogic :
   public C_PuiSdHandlerBusLogic
{
public:
   //CANOpen getter
   const stw_opensyde_core::C_OSCCanOpenManagerInfo * GetCanOpenManager(const stw_types::uint32 ou32_NodeIndex,
                                                                        const stw_types::uint8 ou8_InterfaceNumber)
   const;
   const stw_opensyde_core::C_OSCNode * GetCanOpenManagerNodeOnBus(const stw_types::uint32 ou32_BusIndex) const;
   stw_types::sint32 GetCanOpenManagerNodeOnBus(const stw_types::uint32 ou32_BusIndex,
                                                stw_types::uint32 & oru32_ManagerNodeIndex,
                                                stw_types::uint8 * const opu8_ManagerIntfNumber) const;
   const stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo * GetCanOpenManagerDevice(
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceNumber,
      const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId) const;
   const stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo * GetCanOpenManagerDevice(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   stw_types::sint32 GetCanOpenManagerOfDeviceAndId(const stw_types::uint32 ou32_DeviceNodeIndex,
                                                    stw_types::uint32 * const opu32_ManagerNodeIndex,
                                                    stw_types::uint8 * const opu8_ManagerInterfaceNumber,
                                                    stw_opensyde_core::C_OSCCanInterfaceId * const opc_DeviceNodeId)
   const;
   const stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo * GetCanOpenManagerDeviceForAnyManager(
      const stw_types::uint32 ou32_ManagerNodeIndex,
      const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId) const;

   //CANOpen setter
   stw_types::sint32 SetCanOpenManagerCommonProperties(const stw_types::uint32 ou32_NodeIndex,
                                                       const stw_types::uint8 ou8_InterfaceNumber,
                                                       const bool oq_UseOpenSYDENodeID,
                                                       const stw_types::uint8 ou8_NodeIDValue,
                                                       const stw_types::uint16 ou16_GlobalSDOTimeoutMs,
                                                       const bool oq_AutostartCanOpenManager,
                                                       const bool oq_StartDevices, const bool oq_NMTStartAll,
                                                       const stw_opensyde_core::C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType oe_NMTErrorBehaviour);
   stw_types::sint32 SetCanOpenManagerProducerHeartbeat(const stw_types::uint32 ou32_NodeIndex,
                                                        const stw_types::uint8 ou8_InterfaceNumber,
                                                        const stw_types::uint16 ou16_HeartbeatProducerTimeMs,
                                                        const bool oq_EnableHeartbeatProducing);
   stw_types::sint32 SetCanOpenManagerDeviceCommonProperties(const stw_types::uint32 ou32_NodeIndex,
                                                             const stw_types::uint8 ou8_InterfaceNumber,
                                                             const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId, const bool oq_DeviceOptional, const bool oq_NoInitialization, const bool oq_FactorySettingsActive, const stw_types::uint8 ou8_ResetNodeObjectDictionarySubIndex, const bool oq_EnableHeartbeatProducing, const stw_types::uint16 ou16_HeartbeatProducerTimeMs, const bool oq_EnableHeartbeatConsuming, const stw_types::uint16 ou16_HeartbeatConsumerTimeMs, const bool oq_EnableHeartbeatConsumingAutoCalculation);
   stw_types::sint32 SetCanOpenManagerDeviceNodeId(const stw_types::uint32 ou32_NodeIndex,
                                                   const stw_types::uint8 ou8_InterfaceNumber,
                                                   const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId,
                                                   const bool oq_UseOpenSYDENodeID,
                                                   const stw_types::uint8 ou8_NodeIDValue);

   //CANOpen add/delete
   stw_types::sint32 AddCanOpenManager(const stw_types::uint32 ou32_NodeIndex,
                                       const stw_types::uint8 ou8_InterfaceNumber,
                                       const stw_opensyde_core::C_OSCCanOpenManagerInfo & orc_Config,
                                       bool & orq_DatapoolChanged);
   stw_types::sint32 DeleteCanOpenManager(const stw_types::uint32 ou32_NodeIndex,
                                          const stw_types::uint8 ou8_InterfaceNumber, const bool oq_AllowDatapoolRemove,
                                          bool & orq_DatapoolChanged);
   stw_types::sint32 AddCanOpenManagerDevice(const stw_types::uint32 ou32_NodeIndex,
                                             const stw_types::uint8 ou8_InterfaceNumber,
                                             const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId,
                                             const stw_opensyde_core::C_OSCCanOpenManagerDeviceInfo & orc_Config);
   stw_types::sint32 DeleteCanOpenManagerDevice(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint8 ou8_InterfaceNumber,
                                                const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId);
   stw_types::sint32 DeleteAllCanOpenManagerDevices(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint8 ou8_InterfaceNumber);

   //CANOpen misc
   stw_types::sint32 TranslateCanInterfaceIndexToId(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint32 ou32_InterfaceIndex,
                                                    stw_types::uint8 & oru8_InterfaceId) const;
   stw_types::sint32 TranslateCanInterfaceNumberToIndex(const stw_types::uint32 ou32_NodeIndex,
                                                        const stw_types::uint8 ou8_InterfaceNumber,
                                                        stw_types::uint32 & oru32_InterfaceIndex) const;

protected:
   C_PuiSdHandlerCanOpenLogic(QObject * const opc_Parent = NULL);

   //CANOpen
   stw_types::sint32 m_DeleteCanOpenManagerMessages(const stw_types::uint32 ou32_NodeIndex,
                                                    const stw_types::uint8 ou8_InterfaceNumber,
                                                    const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId);
   stw_types::sint32 m_DeleteCanOpenManagerMessageVectors(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageIdBase, const bool oq_VectorIsTx,
      const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_MessageVector,
      const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId);
   bool m_HandleCreateCanOpenDatapool(const stw_types::uint32 ou32_NodeIndex);
   bool m_HandleCleanUpCanOpenDatapool(const stw_types::uint32 ou32_NodeIndex);
   bool m_CheckCleanUpCanOpenDatapoolPossible(const stw_types::uint32 ou32_NodeIndex) const;
   virtual void m_HandleSyncNodeAdded(const stw_types::uint32 ou32_Index) override;
   void m_HandleSyncNodeAddedForCanOpenDevices(const stw_types::uint32 ou32_Index);
   void m_HandleSyncNodeAddedForCanOpenMessages(const stw_types::uint32 ou32_Index);
   static void mh_HandleSyncNodeAddedForCanOpenMessages(const stw_types::uint32 ou32_Index,
                                                        std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages);
   virtual void m_HandleSyncNodeAboutToBeDeleted(const stw_types::uint32 ou32_Index) override;
   void m_HandleSyncNodeAboutToBeDeletedForCanOpenDevices(const stw_types::uint32 ou32_Index);
   void m_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const stw_types::uint32 ou32_Index);
   static void mh_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const stw_types::uint32 ou32_Index,
                                                                   std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages);
   virtual void m_HandleChangeConnectionForCanOpen(const stw_types::uint32 ou32_NodeIndex,
                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                   const stw_types::uint8 ou8_NewInterface) override;
   void m_HandleChangeConnectionForCanOpenDevices(const stw_types::uint32 ou32_NodeIndex,
                                                  const C_PuiSdNodeConnectionId & orc_PrevId,
                                                  const stw_types::uint8 ou8_NewInterface);
   void m_HandleChangeConnectionForCanOpenManager(const stw_types::uint32 ou32_NodeIndex,
                                                  const C_PuiSdNodeConnectionId & orc_PrevId,
                                                  const stw_types::uint8 ou8_NewInterface);
   void m_HandleChangeConnectionForCanOpenDeviceMessages(const stw_types::uint32 ou32_NodeIndex,
                                                         const C_PuiSdNodeConnectionId & orc_PrevId,
                                                         const stw_types::uint8 ou8_NewInterface);
   static void mh_HandleChangeConnectionForCanOpenDeviceMessages(const stw_types::uint32 ou32_NodeIndex,
                                                                 const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                 const stw_types::uint8 ou8_NewInterface,
                                                                 std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages);
   void m_HandleChangeConnectionForCanOpenManagerMessages(const stw_types::uint32 ou32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_PrevId,
                                                          const stw_types::uint8 ou8_NewInterface);
   void m_MoveMessagesInOscDatapool(const stw_types::uint32 ou32_NodeIndex,
                                    const stw_types::uint32 ou32_PrevInterfaceIndex,
                                    const stw_types::uint32 ou32_NewInterfaceIndex);
   void m_MoveMessagesInUiDatapool(const stw_types::uint32 ou32_NodeIndex,
                                   const stw_types::uint32 ou32_PrevInterfaceIndex,
                                   const stw_types::uint32 ou32_NewInterfaceIndex);
   void m_MoveMessagesInOscProtocol(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_PrevInterface,
                                    const stw_types::uint8 ou8_NewInterface);
   void m_MoveMessagesInUiProtocol(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_PrevInterface,
                                   const stw_types::uint8 ou8_NewInterface);
   virtual void m_HandleChangeCompleteConnectionForCanOpen(const stw_types::uint32 ou32_NodeIndex,
                                                           const C_PuiSdNodeConnectionId & orc_PrevId,
                                                           const C_PuiSdNodeConnectionId & orc_NewId) override;
   void m_HandleChangeCompleteConnectionForCanOpenManager(const stw_types::uint32 ou32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_PrevId,
                                                          const C_PuiSdNodeConnectionId & orc_NewId);
   void m_HandleChangeCompleteConnectionForCanOpenDevice(const stw_types::uint32 ou32_NodeIndex,
                                                         const C_PuiSdNodeConnectionId & orc_PrevId);
   virtual void m_SyncOsyNodeIdChange(const stw_types::uint32 ou32_NodeIndex,
                                      const stw_types::uint32 ou32_InterfaceIndex,
                                      const stw_types::uint8 ou8_NewNodeId) override;
   void m_HandleOsyNodeIdChangeForCanOpenManager(const stw_types::uint32 ou32_NodeIndex,
                                                 const stw_types::uint32 ou32_InterfaceIndex,
                                                 const stw_types::uint8 ou8_NewNodeId);
   void m_HandleOsyNodeIdChangeForCanOpenDevice(const stw_types::uint32 ou32_NodeIndex,
                                                const stw_types::uint32 ou32_InterfaceIndex,
                                                const stw_types::uint8 ou8_NewNodeId);
   void m_HandleNodeIdChangeForCanOpenMessages(const stw_types::uint32 ou32_NodeIndex,
                                               const stw_types::uint8 ou8_InterfaceNumber);
   static void mh_HandleNodeIdChangeForCanOpenMessages(const stw_opensyde_core::C_OSCCanInterfaceId & orc_DeviceId,
                                                       const stw_types::uint8 ou8_NewNodeId,
                                                       std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
