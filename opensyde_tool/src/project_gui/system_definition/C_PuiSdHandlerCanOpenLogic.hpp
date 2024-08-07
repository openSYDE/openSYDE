//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen logic part of UI system definition
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDHANDLERCANOPENLOGIC_HPP
#define C_PUISDHANDLERCANOPENLOGIC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSdHandlerBusLogic.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdHandlerCanOpenLogic :
   public C_PuiSdHandlerBusLogic
{
public:
   //CANOpen getter
   const stw::opensyde_core::C_OscCanOpenManagerInfo * GetCanOpenManager(const uint32_t ou32_NodeIndex,
                                                                         const uint8_t ou8_InterfaceNumber)
   const;
   const stw::opensyde_core::C_OscCanOpenManagerInfo * GetCanOpenManager(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;
   const stw::opensyde_core::C_OscNode * GetCanOpenManagerNodeOnBus(const uint32_t ou32_BusIndex) const;
   int32_t GetCanOpenManagerNodeOnBus(const uint32_t ou32_BusIndex, uint32_t & oru32_ManagerNodeIndex,
                                      uint8_t * const opu8_ManagerIntfNumber) const;
   const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo * GetCanOpenManagerDevice(const uint32_t ou32_NodeIndex,
                                                                                     const uint8_t ou8_InterfaceNumber,
                                                                                     const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId)
   const;
   const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo * GetCanOpenManagerDevice(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   int32_t GetCanOpenManagerOfDeviceAndId(const uint32_t ou32_DeviceNodeIndex, uint32_t * const opu32_ManagerNodeIndex,
                                          uint8_t * const opu8_ManagerInterfaceNumber,
                                          stw::opensyde_core::C_OscCanInterfaceId * const opc_DeviceNodeId)
   const;
   const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo * GetCanOpenManagerDeviceForAnyManager(
      const uint32_t ou32_ManagerNodeIndex, const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId) const;

   const QString GetBusNameOfConnectedDevice(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber) const;

   //CANOpen setter
   int32_t SetCanOpenManagerCommonProperties(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                             const bool oq_UseOpenSydeNodeId, const uint8_t ou8_NodeIdValue,
                                             const uint16_t ou16_GlobalSdoTimeoutMs,
                                             const bool oq_AutostartCanOpenManager, const bool oq_StartDevices,
                                             const bool oq_NmtStartAll,
                                             const stw::opensyde_core::C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType oe_NmtErrorBehaviour);
   int32_t SetCanOpenManagerProducerHeartbeat(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                              const uint16_t ou16_HeartbeatProducerTimeMs,
                                              const bool oq_EnableHeartbeatProducing);
   int32_t SetCanOpenManagerPdoSync(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                    const uint32_t ou32_PdoSyncCyclePeriodUs, const uint32_t ou32_PdoWindowLengthTimeUs,
                                    const bool oq_EnablePdoSyncProducing);
   int32_t SetCanOpenManagerDeviceCommonProperties(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId,
                                                   const bool oq_DeviceOptional, const bool oq_NoInitialization,
                                                   const bool oq_FactorySettingsActive,
                                                   const uint8_t ou8_ResetNodeObjectDictionarySubIndex,
                                                   const bool oq_EnableHeartbeatProducing,
                                                   const uint16_t ou16_HeartbeatProducerTimeMs,
                                                   const bool oq_EnableHeartbeatConsuming,
                                                   const uint16_t ou16_HeartbeatConsumerTimeMs,
                                                   const bool oq_EnableHeartbeatConsumingAutoCalculation);
   int32_t SetCanOpenManagerDeviceNodeId(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                         const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId,
                                         const bool oq_UseOpenSydeNodeId, const uint8_t ou8_NodeIdValue);

   //CANOpen add/delete
   int32_t AddCanOpenManager(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                             const stw::opensyde_core::C_OscCanOpenManagerInfo & orc_Config,
                             bool & orq_DatapoolChanged);
   int32_t DeleteCanOpenManager(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                const bool oq_AllowDatapoolRemove, bool & orq_DatapoolChanged);
   int32_t AddCanOpenManagerDevice(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                   const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId,
                                   const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_Config);
   int32_t ReplaceCanOpenManagerDevice(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                       const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId,
                                       const stw::opensyde_core::C_OscCanOpenManagerDeviceInfo & orc_Config);
   int32_t DeleteCanOpenManagerDevice(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                      const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId);
   int32_t DeleteAllCanOpenManagerDevices(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber);
   int32_t DeleteCanOpenManagerMessages(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                        const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId);

   //CANOpen misc
   int32_t TranslateCanInterfaceIndexToId(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                          uint8_t & oru8_InterfaceId) const;
   int32_t TranslateCanInterfaceNumberToIndex(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                              uint32_t & oru32_InterfaceIndex) const;

protected:
   C_PuiSdHandlerCanOpenLogic(QObject * const opc_Parent = NULL);

   //CANOpen
   int32_t m_DeleteCanOpenManagerMessageVectors(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageIdBase,
      const bool oq_VectorIsTx, const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_MessageVector,
      const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId);
   bool m_HandleCreateCanOpenDatapool(const uint32_t ou32_NodeIndex);
   bool m_HandleCleanUpCanOpenDatapool(const uint32_t ou32_NodeIndex);
   bool m_CheckCleanUpCanOpenDatapoolPossible(const uint32_t ou32_NodeIndex) const;
   void m_HandleSyncNodeAdded(const uint32_t ou32_Index) override;
   void m_HandleSyncNodeAddedForCanOpenDevices(const uint32_t ou32_Index);
   void m_HandleSyncNodeAddedForCanOpenMessages(const uint32_t ou32_Index);
   static void mh_HandleSyncNodeAddedForCanOpenMessages(const uint32_t ou32_Index,
                                                        std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages);
   void m_HandleSyncNodeAboutToBeDeleted(const uint32_t ou32_Index) override;
   void m_HandleSyncNodeAboutToBeDeletedForCanOpenDevices(const uint32_t ou32_Index);
   void m_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const uint32_t ou32_Index);
   static void mh_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const uint32_t ou32_Index,
                                                                   std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages);
   void m_HandleChangeConnectionForCanOpen(const uint32_t ou32_NodeIndex, const C_PuiSdNodeConnectionId & orc_PrevId,
                                           const uint8_t ou8_NewInterface) override;
   void m_HandleChangeConnectionForCanOpenDevices(const uint32_t ou32_NodeIndex,
                                                  const C_PuiSdNodeConnectionId & orc_PrevId,
                                                  const uint8_t ou8_NewInterface);
   void m_HandleChangeConnectionForCanOpenManager(const uint32_t ou32_NodeIndex,
                                                  const C_PuiSdNodeConnectionId & orc_PrevId,
                                                  const uint8_t ou8_NewInterface);
   void m_HandleChangeConnectionForCanOpenDeviceMessages(const uint32_t ou32_NodeIndex,
                                                         const C_PuiSdNodeConnectionId & orc_PrevId,
                                                         const uint8_t ou8_NewInterface);
   static void mh_HandleChangeConnectionForCanOpenDeviceMessages(const uint32_t ou32_NodeIndex,
                                                                 const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                 const uint8_t ou8_NewInterface,
                                                                 std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages);
   void m_HandleChangeConnectionForCanOpenManagerMessages(const uint32_t ou32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_PrevId,
                                                          const uint8_t ou8_NewInterface);
   void m_MoveMessagesInOscDatapool(const uint32_t ou32_NodeIndex, const uint32_t ou32_PrevInterfaceIndex,
                                    const uint32_t ou32_NewInterfaceIndex);
   void m_MoveMessagesInUiDatapool(const uint32_t ou32_NodeIndex, const uint32_t ou32_PrevInterfaceIndex,
                                   const uint32_t ou32_NewInterfaceIndex);
   void m_MoveMessagesInOscProtocol(const uint32_t ou32_NodeIndex, const uint8_t ou8_PrevInterface,
                                    const uint8_t ou8_NewInterface);
   void m_MoveMessagesInUiProtocol(const uint32_t ou32_NodeIndex, const uint8_t ou8_PrevInterface,
                                   const uint8_t ou8_NewInterface);
   void m_HandleChangeCompleteConnectionForCanOpen(const uint32_t ou32_NodeIndex,
                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                   const C_PuiSdNodeConnectionId & orc_NewId) override;
   void m_HandleChangeCompleteConnectionForCanOpenManager(const uint32_t ou32_NodeIndex,
                                                          const C_PuiSdNodeConnectionId & orc_PrevId,
                                                          const C_PuiSdNodeConnectionId & orc_NewId);
   void m_HandleChangeCompleteConnectionForCanOpenDevice(const uint32_t ou32_NodeIndex,
                                                         const C_PuiSdNodeConnectionId & orc_PrevId);
   void m_SyncOsyNodeIdChange(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                              const uint8_t ou8_NewNodeId) override;
   void m_HandleOsyNodeIdChangeForCanOpenManager(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                                 const uint8_t ou8_NewNodeId);
   void m_HandleOsyNodeIdChangeForCanOpenDevice(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                                const uint8_t ou8_NewNodeId);
   void m_HandleNodeIdChangeForCanOpenMessages(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber);
   static void mh_HandleNodeIdChangeForCanOpenMessages(const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceId,
                                                       const uint8_t ou8_NewNodeId,
                                                       std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages);

   void m_HandlePdoSyncChangeForCanOpenMessages(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber);
   static void mh_HandlePdoSyncChangeForCanOpenMessages(const uint32_t ou32_PdoSyncPeriodMs,
                                                        std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages,
                                                        const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
