//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen logic part of UI system definition

   CANopen logic part of UI system definition

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_PuiSdHandlerCanOpenLogic.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen manager

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManager(const uint32_t ou32_NodeIndex,
                                                                              const uint8_t ou8_InterfaceNumber) const
{
   const C_OscCanOpenManagerInfo * pc_Retval = NULL;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::const_iterator c_ItManager = pc_Node->c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != pc_Node->c_CanOpenManagers.end())
      {
         pc_Retval = &c_ItManager->second;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get CANopen manager device by a specific CAN message Id

   \param[in]       orc_MessageId     Message id

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManager(
   const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   const C_OscCanOpenManagerInfo * pc_Manager = NULL;

   // The manager must be the only node associated by this message
   // If the message is not associated to CANopen no manager will be found
   const C_OscNode * const pc_Node = this->GetOscNodeConst(orc_MessageId.u32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager;
      tgl_assert(orc_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      c_ItManager = pc_Node->c_CanOpenManagers.find(
         pc_Node->c_Properties.c_ComInterfaces[orc_MessageId.u32_InterfaceIndex].u8_InterfaceNumber);
      tgl_assert(c_ItManager != pc_Node->c_CanOpenManagers.end());
      if (c_ItManager != pc_Node->c_CanOpenManagers.end())
      {
         pc_Manager = &c_ItManager->second;
      }
   }

   return pc_Manager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC node which has the CANopen manager on a specific bus

   \param[in]  ou32_BusIndex  Index of CAN bus

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNode * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerNodeOnBus(const uint32_t ou32_BusIndex) const
{
   const C_OscNode * pc_Retval = NULL;
   uint32_t u32_NodeIndex;

   if (this->GetCanOpenManagerNodeOnBus(ou32_BusIndex, u32_NodeIndex, NULL) == C_NO_ERR)
   {
      pc_Retval = &this->mc_CoreDefinition.c_Nodes[u32_NodeIndex];
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get node index which has the CANopen manager on a specific bus

   \param[in]  ou32_BusIndex           Index of CAN bus
   \param[out] oru32_ManagerNodeIndex  Found index of CANopen Manager node
   \param[out] opu8_ManagerIntfNumber  Optional parameter for used interface number of CANopen Manager

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid or no manager available
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerNodeOnBus(const uint32_t ou32_BusIndex,
                                                               uint32_t & oru32_ManagerNodeIndex,
                                                               uint8_t * const opu8_ManagerIntfNumber) const
{
   int32_t s32_Return = C_RANGE;

   uint32_t u32_NodeCounter;

   // Search all nodes
   for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_CoreDefinition.c_Nodes.size(); ++u32_NodeCounter)
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_NodeCounter];

      // Only necessary if at least one manager exists
      if (rc_Node.c_CanOpenManagers.size() > 0)
      {
         uint32_t u32_IntfCounter;

         for (u32_IntfCounter = 0U; u32_IntfCounter < rc_Node.c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
         {
            const C_OscNodeComInterfaceSettings & rc_Intf = rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter];

            if ((rc_Intf.e_InterfaceType == C_OscSystemBus::eCAN) &&
                (rc_Intf.GetBusConnected() == true) &&
                (rc_Intf.u32_BusIndex == ou32_BusIndex))
            {
               // Interface is a potential candidate
               if (rc_Node.c_CanOpenManagers.find(rc_Intf.u8_InterfaceNumber) != rc_Node.c_CanOpenManagers.end())
               {
                  // CANopen manager on this bus found
                  oru32_ManagerNodeIndex = u32_NodeCounter;
                  if (opu8_ManagerIntfNumber != NULL)
                  {
                     *opu8_ManagerIntfNumber = rc_Intf.u8_InterfaceNumber;
                  }
                  s32_Return = C_NO_ERR;
                  break;
               }
            }
         }

         if (s32_Return == C_NO_ERR)
         {
            break;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen manager device

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device ID

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerDeviceInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerDevice(const uint32_t ou32_NodeIndex,
                                                                                          const uint8_t ou8_InterfaceNumber,
                                                                                          const C_OscCanInterfaceId & orc_DeviceId)
const
{
   const C_OscCanOpenManagerDeviceInfo * pc_Retval = NULL;
   const C_OscCanOpenManagerInfo * const pc_Manager = this->GetCanOpenManager(ou32_NodeIndex, ou8_InterfaceNumber);

   if (pc_Manager != NULL)
   {
      const std::map<C_OscCanInterfaceId,
                     C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice = pc_Manager->c_CanOpenDevices.find(
         orc_DeviceId);
      if (c_ItDevice != pc_Manager->c_CanOpenDevices.end())
      {
         pc_Retval = &c_ItDevice->second;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen manager device by a specific CAN message Id

   \param[in]       orc_MessageId     Message id

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerDeviceInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerDevice(
   const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   const C_OscCanOpenManagerDeviceInfo * pc_Device = NULL;
   const C_OscCanOpenManagerInfo * const pc_CoManagerInfo = this->GetCanOpenManager(orc_MessageId);

   // The manager must be the only node associated by this message
   // If the message is not associated to CANopen no manager will be found
   if (pc_CoManagerInfo != NULL)
   {
      const C_OscCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);

      tgl_assert(pc_Message != NULL);
      if (pc_Message != NULL)
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
            pc_CoManagerInfo->c_CanOpenDevices.find(
               pc_Message->c_CanOpenManagerOwnerNodeIndex);

         tgl_assert(c_ItDevice != pc_CoManagerInfo->c_CanOpenDevices.end());
         if (c_ItDevice != pc_CoManagerInfo->c_CanOpenDevices.end())
         {
            pc_Device = &c_ItDevice->second;
         }
      }
   }

   return pc_Device;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the node index of the CANopen Manager which has the device assigned

   Output parameter are only valid if return value is C_NO_ERR

   \param[in]       ou32_DeviceNodeIndex          Node index of the device for checking
   \param[out]      opu32_ManagerNodeIndex        Node index of the found CANopen Manager.
   \param[out]      opu8_ManagerInterfaceNumber   CANopen Manager interface number on which the device is assigned to
   \param[out]      opc_DeviceNodeId              Device node id which is assigned to the manager on this interface

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
   \retval   C_CONFIG   Device is no device and is not assigned to a CANopen Manager
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerOfDeviceAndId(const uint32_t ou32_DeviceNodeIndex,
                                                                   uint32_t * const opu32_ManagerNodeIndex,
                                                                   uint8_t * const opu8_ManagerInterfaceNumber,
                                                                   stw::opensyde_core::C_OscCanInterfaceId * const opc_DeviceNodeId)
const
{
   int32_t s32_Retval = C_RANGE;

   if (ou32_DeviceNodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      uint32_t u32_NodeCounter;

      s32_Retval = C_CONFIG;

      // Searching in all node CANopen Managers for the searched node as device
      for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_CoreDefinition.c_Nodes.size(); ++u32_NodeCounter)
      {
         const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_NodeCounter];

         std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager;
         for (c_ItManager = rc_Node.c_CanOpenManagers.begin(); c_ItManager != rc_Node.c_CanOpenManagers.end();
              ++c_ItManager)
         {
            const C_OscCanOpenManagerInfo & rc_Manager = c_ItManager->second;
            std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice;

            for (c_ItDevice = rc_Manager.c_CanOpenDevices.begin(); c_ItDevice != rc_Manager.c_CanOpenDevices.end();
                 ++c_ItDevice)
            {
               if (c_ItDevice->first.u32_NodeIndex == ou32_DeviceNodeIndex)
               {
                  if (opu32_ManagerNodeIndex != NULL)
                  {
                     *opu32_ManagerNodeIndex = u32_NodeCounter;
                  }
                  if (opc_DeviceNodeId != NULL)
                  {
                     *opc_DeviceNodeId = c_ItDevice->first;
                  }
                  if (opu8_ManagerInterfaceNumber != NULL)
                  {
                     *opu8_ManagerInterfaceNumber = c_ItManager->first;
                  }
                  s32_Retval = C_NO_ERR;
                  break;
               }
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen manager device for any manager

   \param[in]  ou32_ManagerNodeIndex   Manager node index
   \param[in]  orc_DeviceId            Device id

   \return
   CANopen manager device for any manager
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerDeviceInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerDeviceForAnyManager(
   const uint32_t ou32_ManagerNodeIndex, const C_OscCanInterfaceId & orc_DeviceId) const
{
   const C_OscCanOpenManagerDeviceInfo * pc_Retval = NULL;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_ManagerNodeIndex);

   if (pc_Node != NULL)
   {
      for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager =
              pc_Node->c_CanOpenManagers.cbegin();
           (c_ItManager != pc_Node->c_CanOpenManagers.cend()) && (pc_Retval == NULL); ++c_ItManager)
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
            c_ItManager->second.c_CanOpenDevices.find(
               orc_DeviceId);
         if (c_ItDevice != c_ItManager->second.c_CanOpenDevices.cend())
         {
            pc_Retval = &c_ItDevice->second;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen manager common properties

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou8_InterfaceNumber           Interface number
   \param[in]  oq_UseOpenSydeNodeId          Use open SYDE node ID
   \param[in]  ou8_NodeIdValue               Node ID value
   \param[in]  ou16_GlobalSdoTimeoutMs       Global SDO timeout ms
   \param[in]  oq_AutostartCanOpenManager    Autostart CANopen manager
   \param[in]  oq_StartDevices               Start devices
   \param[in]  oq_NmtStartAll                NMT start all
   \param[in]  oe_NmtErrorBehaviour          NMT error behaviour

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerCommonProperties(const uint32_t ou32_NodeIndex,
                                                                      const uint8_t ou8_InterfaceNumber,
                                                                      const bool oq_UseOpenSydeNodeId,
                                                                      const uint8_t ou8_NodeIdValue,
                                                                      const uint16_t ou16_GlobalSdoTimeoutMs,
                                                                      const bool oq_AutostartCanOpenManager,
                                                                      const bool oq_StartDevices,
                                                                      const bool oq_NmtStartAll,
                                                                      const C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType oe_NmtErrorBehaviour)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         c_ItManager->second.q_UseOpenSydeNodeId = oq_UseOpenSydeNodeId;
         c_ItManager->second.u8_NodeIdValue = ou8_NodeIdValue;
         c_ItManager->second.u16_GlobalSdoTimeoutMs = ou16_GlobalSdoTimeoutMs;
         c_ItManager->second.q_AutostartCanOpenManager = oq_AutostartCanOpenManager;
         c_ItManager->second.q_StartDevices = oq_StartDevices;
         c_ItManager->second.q_NmtStartAll = oq_NmtStartAll;
         c_ItManager->second.e_NmtErrorBehaviour = oe_NmtErrorBehaviour;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen manager producer heartbeat

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou8_InterfaceNumber           Interface number
   \param[in]  ou16_HeartbeatProducerTimeMs  Heartbeat producer time ms
   \param[in]  oq_EnableHeartbeatProducing   Heartbeat producing enabled flag

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerProducerHeartbeat(const uint32_t ou32_NodeIndex,
                                                                       const uint8_t ou8_InterfaceNumber,
                                                                       const uint16_t ou16_HeartbeatProducerTimeMs,
                                                                       const bool oq_EnableHeartbeatProducing)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         c_ItManager->second.u16_HeartbeatProducerTimeMs = ou16_HeartbeatProducerTimeMs;
         c_ItManager->second.q_EnableHeartbeatProducing = oq_EnableHeartbeatProducing;

         //update consumer time of devices
         for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItDevice != c_ItManager->second.c_CanOpenDevices.end(); ++c_ItDevice)
         {
            if (c_ItDevice->second.q_EnableHeartbeatConsumingAutoCalculation == true)
            {
               //auto calculation of consumer time
               const float32_t f32_Temp = static_cast<float32_t>(ou16_HeartbeatProducerTimeMs);
               const float32_t f32_Result = f32_Temp * mf32_HEARTBEAT_CONSUMER_TIME_FACTOR;
               const uint16_t u16_Result = static_cast<uint16_t>(f32_Result);

               c_ItDevice->second.u16_HeartbeatConsumerTimeMs = u16_Result;

               //disable device HB consuming if manager HB producing is disabled
               if ((c_ItManager->second.q_EnableHeartbeatProducing == false) &&
                   (c_ItDevice->second.q_EnableHeartbeatConsuming == true))
               {
                  c_ItDevice->second.q_EnableHeartbeatConsuming = false;
               }
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen manager common properties

   \param[in]  ou32_NodeIndex                Node index
   \param[in]  ou8_InterfaceNumber           Interface number
   \param[in]  ou32_PdoSyncCyclePeriodUs     PDO SYNC cycle period time in us
   \param[in]  ou32_PdoWindowLengthTimeUs    PDO SYNC window length time in us
   \param[in]  oq_EnablePdoSyncProducing     Flag if PDO SYNC is enabled for the CANopen manager interface

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerPdoSync(const uint32_t ou32_NodeIndex,
                                                             const uint8_t ou8_InterfaceNumber,
                                                             const uint32_t ou32_PdoSyncCyclePeriodUs,
                                                             const uint32_t ou32_PdoWindowLengthTimeUs,
                                                             const bool oq_EnablePdoSyncProducing)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const bool q_PeriodChanged = c_ItManager->second.u32_SyncCyclePeriodUs != ou32_PdoSyncCyclePeriodUs;
         c_ItManager->second.q_ProduceSyncMessage = oq_EnablePdoSyncProducing;
         c_ItManager->second.u32_SyncCyclePeriodUs = ou32_PdoSyncCyclePeriodUs;
         c_ItManager->second.u32_SyncWindowLengthUs = ou32_PdoWindowLengthTimeUs;

         if (q_PeriodChanged == true)
         {
            // If the period has changed, the automatic set timeout values must be updated
            this->m_HandlePdoSyncChangeForCanOpenMessages(ou32_NodeIndex, ou8_InterfaceNumber);
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen manager device common properties

   \param[in]  ou32_NodeIndex                               Node index
   \param[in]  ou8_InterfaceNumber                          Interface number
   \param[in]  orc_DeviceId                                 Device id
   \param[in]  oq_DeviceOptional                            Device optional
   \param[in]  oq_NoInitialization                          No initialization
   \param[in]  oq_FactorySettingsActive                     Factory settings active
   \param[in]  ou8_ResetNodeObjectDictionarySubIndex        Reset node object dictionary sub index
   \param[in]  oq_EnableHeartbeatProducing                  Enable heartbeat producing
   \param[in]  ou16_HeartbeatProducerTimeMs                 Heartbeat producer time ms
   \param[in]  oq_EnableHeartbeatConsuming                  Enable heartbeat consuming
   \param[in]  ou16_HeartbeatConsumerTimeMs                 Heartbeat consumer time ms
   \param[in]  oq_EnableHeartbeatConsumingAutoCalculation   Enable heartbeat consuming autocalculation

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerDeviceCommonProperties(const uint32_t ou32_NodeIndex,
                                                                            const uint8_t ou8_InterfaceNumber,
                                                                            const C_OscCanInterfaceId & orc_DeviceId,
                                                                            const bool oq_DeviceOptional,
                                                                            const bool oq_NoInitialization,
                                                                            const bool oq_FactorySettingsActive,
                                                                            const uint8_t ou8_ResetNodeObjectDictionarySubIndex, const bool oq_EnableHeartbeatProducing, const uint16_t ou16_HeartbeatProducerTimeMs, const bool oq_EnableHeartbeatConsuming, const uint16_t ou16_HeartbeatConsumerTimeMs,
                                                                            const bool oq_EnableHeartbeatConsumingAutoCalculation)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::iterator c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.
                                                                                 find(orc_DeviceId);
         if (c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItSubDevice->second.q_DeviceOptional = oq_DeviceOptional;
            c_ItSubDevice->second.q_NoInitialization = oq_NoInitialization;
            c_ItSubDevice->second.q_FactorySettingsActive = oq_FactorySettingsActive;
            c_ItSubDevice->second.u8_ResetNodeObjectDictionarySubIndex = ou8_ResetNodeObjectDictionarySubIndex;
            c_ItSubDevice->second.q_EnableHeartbeatProducing = oq_EnableHeartbeatProducing;
            c_ItSubDevice->second.u16_HeartbeatProducerTimeMs = ou16_HeartbeatProducerTimeMs;
            c_ItSubDevice->second.q_EnableHeartbeatConsuming = oq_EnableHeartbeatConsuming;
            c_ItSubDevice->second.u16_HeartbeatConsumerTimeMs = ou16_HeartbeatConsumerTimeMs;
            c_ItSubDevice->second.q_EnableHeartbeatConsumingAutoCalculation =
               oq_EnableHeartbeatConsumingAutoCalculation;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set CANopen manager device node id properties

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou8_InterfaceNumber     Interface number
   \param[in]  orc_DeviceId            Device id
   \param[in]  oq_UseOpenSydeNodeId    Use open SYDE node ID
   \param[in]  ou8_NodeIdValue         Node ID value

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerDeviceNodeId(const uint32_t ou32_NodeIndex,
                                                                  const uint8_t ou8_InterfaceNumber,
                                                                  const C_OscCanInterfaceId & orc_DeviceId,
                                                                  const bool oq_UseOpenSydeNodeId,
                                                                  const uint8_t ou8_NodeIdValue)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::iterator c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.
                                                                                 find(orc_DeviceId);
         if (c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItSubDevice->second.q_UseOpenSydeNodeId = oq_UseOpenSydeNodeId;
            c_ItSubDevice->second.u8_NodeIdValue = ou8_NodeIdValue;
            m_HandleNodeIdChangeForCanOpenMessages(orc_DeviceId.u32_NodeIndex, orc_DeviceId.u8_InterfaceNumber);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add CANopen manager

   \param[in]   ou32_NodeIndex         Node index
   \param[in]   ou8_InterfaceNumber    Interface number
   \param[in]   orc_Config             Config
   \param[out]  orq_DatapoolChanged    Indicator if datapool changed

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::AddCanOpenManager(const uint32_t ou32_NodeIndex, const uint8_t ou8_InterfaceNumber,
                                                      const C_OscCanOpenManagerInfo & orc_Config,
                                                      bool & orq_DatapoolChanged)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::const_iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager == rc_Node.c_CanOpenManagers.end())
      {
         uint32_t u32_InterfaceIndex = 0UL;
         rc_Node.c_CanOpenManagers[ou8_InterfaceNumber] = orc_Config;
         orq_DatapoolChanged = this->m_HandleCreateCanOpenDatapool(ou32_NodeIndex);
         if (this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, ou8_InterfaceNumber,
                                                      u32_InterfaceIndex) == C_NO_ERR)
         {
            this->SetCanProtocolMessageContainerConnected(ou32_NodeIndex,
                                                          C_OscCanProtocol::eCAN_OPEN,
                                                          u32_InterfaceIndex,
                                                          true);
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete CANopen manager

   \param[in]   ou32_NodeIndex            Node index
   \param[in]   ou8_InterfaceNumber       Interface number
   \param[in]   oq_AllowDatapoolRemove    Flag if datapool remove is allowed
   \param[out]  orq_DatapoolChanged       Indicator if datapool changed

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::DeleteCanOpenManager(const uint32_t ou32_NodeIndex,
                                                         const uint8_t ou8_InterfaceNumber,
                                                         const bool oq_AllowDatapoolRemove, bool & orq_DatapoolChanged)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::const_iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         uint32_t u32_InterfaceIndex = 0UL;
         tgl_assert(this->DeleteAllCanOpenManagerDevices(ou32_NodeIndex, ou8_InterfaceNumber) == C_NO_ERR);
         //Delete manager
         rc_Node.c_CanOpenManagers.erase(c_ItManager);
         //Delete DP
         if (oq_AllowDatapoolRemove)
         {
            orq_DatapoolChanged = this->m_HandleCleanUpCanOpenDatapool(ou32_NodeIndex);
         }
         else
         {
            orq_DatapoolChanged = false;
         }
         if (this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, ou8_InterfaceNumber,
                                                      u32_InterfaceIndex) == C_NO_ERR)
         {
            this->SetCanProtocolMessageContainerConnected(ou32_NodeIndex,
                                                          C_OscCanProtocol::eCAN_OPEN,
                                                          u32_InterfaceIndex,
                                                          false);
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add CANopen manager device

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device ID
   \param[in]  orc_Config           Config

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::AddCanOpenManagerDevice(const uint32_t ou32_NodeIndex,
                                                            const uint8_t ou8_InterfaceNumber,
                                                            const C_OscCanInterfaceId & orc_DeviceId,
                                                            const C_OscCanOpenManagerDeviceInfo & orc_Config)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItSubDevice =
            c_ItManager->second.c_CanOpenDevices.
            find(orc_DeviceId);
         if (c_ItSubDevice == c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItManager->second.c_CanOpenDevices[orc_DeviceId] = orc_Config;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Replace can open manager device

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device id
   \param[in]  orc_Config           Config

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::ReplaceCanOpenManagerDevice(const uint32_t ou32_NodeIndex,
                                                                const uint8_t ou8_InterfaceNumber,
                                                                const C_OscCanInterfaceId & orc_DeviceId,
                                                                const C_OscCanOpenManagerDeviceInfo & orc_Config)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
            c_ItManager->second.c_CanOpenDevices.
            find(orc_DeviceId);
         if (c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItSubDevice->second = orc_Config;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete CANopen manager device

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device ID

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::DeleteCanOpenManagerDevice(const uint32_t ou32_NodeIndex,
                                                               const uint8_t ou8_InterfaceNumber,
                                                               const C_OscCanInterfaceId & orc_DeviceId)
{
   int32_t s32_Retval;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OscCanInterfaceId,
                        C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItSubDevice =
            c_ItManager->second.c_CanOpenDevices.
            find(orc_DeviceId);
         if (c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItManager->second.c_CanOpenDevices.erase(c_ItSubDevice);
            s32_Retval = this->DeleteCanOpenManagerMessages(ou32_NodeIndex, ou8_InterfaceNumber, orc_DeviceId);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete all CANopen manager devices

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::DeleteAllCanOpenManagerDevices(const uint32_t ou32_NodeIndex,
                                                                   const uint8_t ou8_InterfaceNumber)
{
   int32_t s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<uint8_t,
                     C_OscCanOpenManagerInfo>::const_iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         //Delete items (handle DP)
         std::vector<C_OscCanInterfaceId> c_Items;
         c_Items.reserve(c_ItManager->second.c_CanOpenDevices.size());
         for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItDevice != c_ItManager->second.c_CanOpenDevices.end(); ++c_ItDevice)
         {
            c_Items.push_back(c_ItDevice->first);
         }
         for (uint32_t u32_ItDevice = 0UL; u32_ItDevice < c_Items.size(); ++u32_ItDevice)
         {
            tgl_assert(this->DeleteCanOpenManagerDevice(ou32_NodeIndex, ou8_InterfaceNumber,
                                                        c_Items[u32_ItDevice]) == C_NO_ERR);
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete CANopen manager messages

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_DeviceId         Device ID

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::DeleteCanOpenManagerMessages(const uint32_t ou32_NodeIndex,
                                                                 const uint8_t ou8_InterfaceNumber,
                                                                 const C_OscCanInterfaceId & orc_DeviceId)
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_InterfaceIndex = 0UL;

   if (this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, ou8_InterfaceNumber, u32_InterfaceIndex) == C_NO_ERR)
   {
      const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         for (uint32_t u32_ItProt = 0UL; (u32_ItProt < pc_Node->c_ComProtocols.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItProt)
         {
            const C_OscCanProtocol & rc_Prot = pc_Node->c_ComProtocols[u32_ItProt];

            if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
            {
               for (uint32_t u32_ItContainer = 0UL;
                    (u32_ItContainer < rc_Prot.c_ComMessages.size()) && (s32_Retval == C_NO_ERR); ++u32_ItContainer)
               {
                  const C_OscCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
                  const C_OscCanMessageIdentificationIndices c_BaseId(ou32_NodeIndex, C_OscCanProtocol::eCAN_OPEN,
                                                                      u32_InterfaceIndex, rc_Prot.u32_DataPoolIndex,
                                                                      false);

                  s32_Retval = this->m_DeleteCanOpenManagerMessageVectors(c_BaseId, false, rc_Container.c_RxMessages,
                                                                          orc_DeviceId);
                  if (s32_Retval == C_NO_ERR)
                  {
                     s32_Retval = this->m_DeleteCanOpenManagerMessageVectors(c_BaseId, true, rc_Container.c_TxMessages,
                                                                             orc_DeviceId);
                  }
               }
            }
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate can interface index to ID

   \param[in]   ou32_NodeIndex         Node index
   \param[in]   ou32_InterfaceIndex    Interface index
   \param[out]  oru8_InterfaceId       Interface ID

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::TranslateCanInterfaceIndexToId(const uint32_t ou32_NodeIndex,
                                                                   const uint32_t ou32_InterfaceIndex,
                                                                   uint8_t & oru8_InterfaceId) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (ou32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OscNodeComInterfaceSettings & rc_Interface =
            pc_Node->c_Properties.c_ComInterfaces[ou32_InterfaceIndex];
         oru8_InterfaceId = rc_Interface.u8_InterfaceNumber;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate can interface number to index

   \param[in]   ou32_NodeIndex         Node index
   \param[in]   ou8_InterfaceNumber    Interface number
   \param[out]  oru32_InterfaceIndex   Interface index

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::TranslateCanInterfaceNumberToIndex(const uint32_t ou32_NodeIndex,
                                                                       const uint8_t ou8_InterfaceNumber,
                                                                       uint32_t & oru32_InterfaceIndex) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_Index = 0; u32_Index < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_Index)
      {
         const C_OscNodeComInterfaceSettings & rc_CurComInterface =
            pc_Node->c_Properties.c_ComInterfaces[u32_Index];
         //Check if match
         if ((rc_CurComInterface.e_InterfaceType == C_OscSystemBus::eCAN) &&
             (ou8_InterfaceNumber == rc_CurComInterface.u8_InterfaceNumber))
         {
            oru32_InterfaceIndex = u32_Index;
         }
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSdHandlerCanOpenLogic::C_PuiSdHandlerCanOpenLogic(QObject * const opc_Parent) :
   C_PuiSdHandlerBusLogic(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete CANopen manager message vectors

   \param[in]  orc_MessageIdBase    Message ID base
   \param[in]  oq_VectorIsTx        Vector is TX
   \param[in]  orc_MessageVector    Message vector
   \param[in]  orc_DeviceId         Device ID

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_PuiSdHandlerCanOpenLogic::m_DeleteCanOpenManagerMessageVectors(
   const C_OscCanMessageIdentificationIndices & orc_MessageIdBase, const bool oq_VectorIsTx,
   const std::vector<C_OscCanMessage> & orc_MessageVector, const C_OscCanInterfaceId & orc_DeviceId)
{
   int32_t s32_Retval = C_NO_ERR;

   for (uint32_t u32_ItMessage = orc_MessageVector.size(); (u32_ItMessage > 0UL) && (s32_Retval == C_NO_ERR);
        --u32_ItMessage)
   {
      const uint32_t u32_MessageIndex = u32_ItMessage - 1UL;
      const C_OscCanMessage & rc_Message = orc_MessageVector[u32_MessageIndex];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex == orc_DeviceId)
      {
         const C_OscCanMessageIdentificationIndices c_MessageId(orc_MessageIdBase.u32_NodeIndex,
                                                                orc_MessageIdBase.e_ComProtocol,
                                                                orc_MessageIdBase.u32_InterfaceIndex,
                                                                orc_MessageIdBase.u32_DatapoolIndex, oq_VectorIsTx,
                                                                u32_MessageIndex);
         s32_Retval = this->DeleteCanMessage(c_MessageId);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle create CANopen datapool

   \param[in]  ou32_NodeIndex    Node index

   \return
   Flags

   \retval   True    Change happened
   \retval   False   No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerCanOpenLogic::m_HandleCreateCanOpenDatapool(const uint32_t ou32_NodeIndex)
{
   bool q_Retval = false;

   if (this->GetOscCanDataPools(ou32_NodeIndex,
                                C_OscCanProtocol::eCAN_OPEN).size() == 0UL)
   {
      tgl_assert(this->AddAutoGenCommDataPool(ou32_NodeIndex,
                                              C_OscCanProtocol::eCAN_OPEN) == C_NO_ERR);
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle clean up CANopen datapool

   \param[in]  ou32_NodeIndex    Node index

   \return
   Flags

   \retval   True    Change happened
   \retval   False   No change
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerCanOpenLogic::m_HandleCleanUpCanOpenDatapool(const uint32_t ou32_NodeIndex)
{
   bool q_Retval = false;

   if (this->m_CheckCleanUpCanOpenDatapoolPossible(ou32_NodeIndex))
   {
      const C_OscNode * const pc_Node = this->GetOscNodeConst(ou32_NodeIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         for (uint32_t u32_ItProt = 0UL; u32_ItProt < pc_Node->c_ComProtocols.size(); ++u32_ItProt)
         {
            const C_OscCanProtocol & rc_Prot = pc_Node->c_ComProtocols[u32_ItProt];
            if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
            {
               tgl_assert(rc_Prot.u32_DataPoolIndex < pc_Node->c_DataPools.size());
               if (rc_Prot.u32_DataPoolIndex < pc_Node->c_DataPools.size())
               {
                  tgl_assert(this->RemoveDataPool(ou32_NodeIndex, rc_Prot.u32_DataPoolIndex) == C_NO_ERR);
               }
               q_Retval = true;
               break;
            }
         }
         tgl_assert(q_Retval);
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check clean up CANopen datapool possible

   \param[in]  ou32_NodeIndex    Node index

   \return
   Flags

   \retval   True    Clean up CANopen datapool possible
   \retval   False   Clean up CANopen datapool not possible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSdHandlerCanOpenLogic::m_CheckCleanUpCanOpenDatapoolPossible(const uint32_t ou32_NodeIndex) const
{
   bool q_Retval = false;
   const C_OscNode * const pc_Node =  this->GetOscNodeConst(ou32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      q_Retval = pc_Node->c_CanOpenManagers.size() == 0UL;
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node added

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAdded(const uint32_t ou32_Index)
{
   C_PuiSdHandlerHalc::m_HandleSyncNodeAdded(ou32_Index);
   m_HandleSyncNodeAddedForCanOpenDevices(ou32_Index);
   m_HandleSyncNodeAddedForCanOpenMessages(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node added for CANopen devices

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAddedForCanOpenDevices(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (std::map<uint8_t, C_OscCanOpenManagerInfo>::iterator c_ItManager =
              rc_Node.c_CanOpenManagers.begin();
           c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
      {
         std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo> c_Storage;
         for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end();)
         {
            if (c_ItSubDevice->first.u32_NodeIndex >= ou32_Index)
            {
               //Adapt
               C_OscCanInterfaceId c_Tmp = c_ItSubDevice->first;
               ++c_Tmp.u32_NodeIndex;
               c_Storage[c_Tmp] = c_ItSubDevice->second;
               c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.erase(c_ItSubDevice);
            }
            else
            {
               //No adaptation necessary
               ++c_ItSubDevice;
            }
         }
         //Re-add adapted elements
         for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItNewSubDevice =
                 c_Storage.begin();
              c_ItNewSubDevice != c_Storage.end(); ++c_ItNewSubDevice)
         {
            c_ItManager->second.c_CanOpenDevices[c_ItNewSubDevice->first] = c_ItNewSubDevice->second;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node added for CANopen messages

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAddedForCanOpenMessages(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            for (uint32_t u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
            {
               C_OscCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
               C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAddedForCanOpenMessages(ou32_Index,
                                                                                    rc_Container.c_RxMessages);
               C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAddedForCanOpenMessages(ou32_Index,
                                                                                    rc_Container.c_TxMessages);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node added for CANopen messages

   \param[in]      ou32_Index    Index
   \param[in,out]  orc_Messages  Messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAddedForCanOpenMessages(const uint32_t ou32_Index,
                                                                          std::vector<C_OscCanMessage> & orc_Messages)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      C_OscCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex >= ou32_Index)
      {
         ++rc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle data sync for node about to be deleted

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeleted(const uint32_t ou32_Index)
{
   C_PuiSdHandlerHalc::m_HandleSyncNodeAboutToBeDeleted(ou32_Index);
   m_HandleSyncNodeAboutToBeDeletedForCanOpenDevices(ou32_Index);
   m_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node about to be deleted for CANopen devices

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeletedForCanOpenDevices(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (std::map<uint8_t, C_OscCanOpenManagerInfo>::iterator c_ItManager =
              rc_Node.c_CanOpenManagers.begin();
           c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
      {
         std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo> c_Storage;
         for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end();)
         {
            if (c_ItSubDevice->first.u32_NodeIndex > ou32_Index)
            {
               //Adapt
               C_OscCanInterfaceId c_Tmp = c_ItSubDevice->first;
               --c_Tmp.u32_NodeIndex;
               c_Storage[c_Tmp] = c_ItSubDevice->second;
               c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.erase(c_ItSubDevice);
            }
            else if (c_ItSubDevice->first.u32_NodeIndex == ou32_Index)
            {
               //Should not happen here
               tgl_assert(false);
               //Remove
               c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.erase(c_ItSubDevice);
            }
            else
            {
               //No adaptation necessary
               ++c_ItSubDevice;
            }
         }
         //Re-add adapted elements
         for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItNewSubDevice =
                 c_Storage.begin();
              c_ItNewSubDevice != c_Storage.end(); ++c_ItNewSubDevice)
         {
            c_ItManager->second.c_CanOpenDevices[c_ItNewSubDevice->first] = c_ItNewSubDevice->second;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node about to be deleted for CANopen messages

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const uint32_t ou32_Index)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            for (uint32_t u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
            {
               C_OscCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
               C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(ou32_Index,
                                                                                               rc_Container.c_RxMessages);
               C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(ou32_Index,
                                                                                               rc_Container.c_TxMessages);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node about to be deleted for can open messages

   \param[in]      ou32_Index    Index
   \param[in,out]  orc_Messages  Messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const uint32_t ou32_Index,
                                                                                     std::vector<C_OscCanMessage> & orc_Messages)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size();)
   {
      C_OscCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex > ou32_Index)
      {
         //Adapt
         --rc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex;
         ++u32_ItMessage;
      }
      else if (rc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex == ou32_Index)
      {
         //Should not happen here
         tgl_assert(false);
         //Remove
         orc_Messages.erase(orc_Messages.begin() + u32_ItMessage);
      }
      else
      {
         //No adaptation necessary
         ++u32_ItMessage;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change connection for CANopen

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  ou8_NewInterface  New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpen(const uint32_t ou32_NodeIndex,
                                                                    const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                    const uint8_t ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      m_HandleChangeConnectionForCanOpenDevices(ou32_NodeIndex, orc_PrevId, ou8_NewInterface);
      m_HandleChangeConnectionForCanOpenManager(ou32_NodeIndex, orc_PrevId, ou8_NewInterface);
      m_HandleChangeConnectionForCanOpenDeviceMessages(ou32_NodeIndex, orc_PrevId, ou8_NewInterface);
      m_HandleChangeConnectionForCanOpenManagerMessages(ou32_NodeIndex, orc_PrevId, ou8_NewInterface);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change connection for CANopen devices

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  ou8_NewInterface  New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenDevices(const uint32_t ou32_NodeIndex,
                                                                           const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                           const uint8_t ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
      {
         C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (std::map<uint8_t, C_OscCanOpenManagerInfo>::iterator c_ItManager =
                 rc_Node.c_CanOpenManagers.begin();
              c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
         {
            std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo> c_Storage;
            for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
                    c_ItManager->second.c_CanOpenDevices.begin();
                 c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end();)
            {
               if ((c_ItSubDevice->first.u32_NodeIndex == ou32_NodeIndex) &&
                   (c_ItSubDevice->first.u8_InterfaceNumber == orc_PrevId.u8_InterfaceNumber))
               {
                  //Adapt
                  C_OscCanInterfaceId c_Tmp = c_ItSubDevice->first;
                  c_Tmp.u8_InterfaceNumber = ou8_NewInterface;
                  c_Storage[c_Tmp] = c_ItSubDevice->second;
                  c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.erase(c_ItSubDevice);
               }
               else
               {
                  //No adaptation necessary
                  ++c_ItSubDevice;
               }
            }
            //Re-add adapted elements
            for (std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo>::iterator c_ItNewSubDevice =
                    c_Storage.begin();
                 c_ItNewSubDevice != c_Storage.end(); ++c_ItNewSubDevice)
            {
               c_ItManager->second.c_CanOpenDevices[c_ItNewSubDevice->first] = c_ItNewSubDevice->second;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change connection for CANopen manager

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  ou8_NewInterface  New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenManager(const uint32_t ou32_NodeIndex,
                                                                           const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                           const uint8_t ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
      if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

         const std::map<uint8_t,
                        C_OscCanOpenManagerInfo>::const_iterator c_ItPrev = rc_Node.c_CanOpenManagers.find(
            orc_PrevId.u8_InterfaceNumber);
         if (c_ItPrev != rc_Node.c_CanOpenManagers.end())
         {
            bool q_Tmp;
            //Remove before overwriting
            //May not exist
            this->DeleteCanOpenManager(ou32_NodeIndex, ou8_NewInterface, false, q_Tmp);
            //Move
            tgl_assert(this->AddCanOpenManager(ou32_NodeIndex, ou8_NewInterface, c_ItPrev->second,
                                               q_Tmp) == C_NO_ERR);
            //Remove deprecated config
            //Don't use interface as messages should be kept
            rc_Node.c_CanOpenManagers.erase(c_ItPrev);
         } //lint !e438 return value not necessary here
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change connection for CANopen device messages

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  ou8_NewInterface  New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenDeviceMessages(const uint32_t ou32_NodeIndex,
                                                                                  const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                                  const uint8_t ou8_NewInterface)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            for (uint32_t u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
            {
               C_OscCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
               C_PuiSdHandlerCanOpenLogic::mh_HandleChangeConnectionForCanOpenDeviceMessages(ou32_NodeIndex, orc_PrevId,
                                                                                             ou8_NewInterface,
                                                                                             rc_Container.c_RxMessages);
               C_PuiSdHandlerCanOpenLogic::mh_HandleChangeConnectionForCanOpenDeviceMessages(ou32_NodeIndex, orc_PrevId,
                                                                                             ou8_NewInterface,
                                                                                             rc_Container.c_TxMessages);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change connection for CANopen device messages

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_PrevId          Previous ID
   \param[in]      ou8_NewInterface    New interface number
   \param[in,out]  orc_Messages        Messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::mh_HandleChangeConnectionForCanOpenDeviceMessages(const uint32_t ou32_NodeIndex,
                                                                                   const C_PuiSdNodeConnectionId & orc_PrevId, const uint8_t ou8_NewInterface,
                                                                                   std::vector<C_OscCanMessage> & orc_Messages)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size();)
   {
      C_OscCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex.u32_NodeIndex == ou32_NodeIndex)
      {
         if (rc_Message.c_CanOpenManagerOwnerNodeIndex.u8_InterfaceNumber == orc_PrevId.u8_InterfaceNumber)
         {
            rc_Message.c_CanOpenManagerOwnerNodeIndex.u8_InterfaceNumber = ou8_NewInterface;
            ++u32_ItMessage;
         }
         else if (rc_Message.c_CanOpenManagerOwnerNodeIndex.u8_InterfaceNumber == ou8_NewInterface)
         {
            //Should not happen here
            tgl_assert(false);
            orc_Messages.erase(orc_Messages.begin() + u32_ItMessage);
         }
         else
         {
            ++u32_ItMessage;
         }
      }
      else
      {
         ++u32_ItMessage;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change connection for CANopen manager messages

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  ou8_NewInterface  New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenManagerMessages(const uint32_t ou32_NodeIndex,
                                                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                                   const uint8_t ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      uint32_t u32_PrevInterfaceIndex = 0UL;
      uint32_t u32_NewInterfaceIndex = 0UL;
      if ((this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber,
                                                    u32_PrevInterfaceIndex) == C_NO_ERR) &&
          (this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, ou8_NewInterface,
                                                    u32_NewInterfaceIndex) == C_NO_ERR))
      {
         m_MoveMessagesInOscDatapool(ou32_NodeIndex, u32_PrevInterfaceIndex, u32_NewInterfaceIndex);
         m_MoveMessagesInUiDatapool(ou32_NodeIndex, u32_PrevInterfaceIndex, u32_NewInterfaceIndex);
         m_MoveMessagesInOscProtocol(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber, ou8_NewInterface);
         m_MoveMessagesInUiProtocol(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber, ou8_NewInterface);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move messages in core datapool

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_PrevInterfaceIndex    Previous interface index
   \param[in]  ou32_NewInterfaceIndex     New interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInOscDatapool(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_PrevInterfaceIndex,
                                                             const uint32_t ou32_NewInterfaceIndex)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         const C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            tgl_assert(rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size());
            if (rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size())
            {
               C_OscNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Prot.u32_DataPoolIndex];

               const int32_t s32_PrevTxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool,
                                                                                    ou32_PrevInterfaceIndex,
                                                                                    true);
               const int32_t s32_PrevRxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool,
                                                                                    ou32_PrevInterfaceIndex,
                                                                                    false);
               const int32_t s32_NewTxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                   true);
               const int32_t s32_NewRxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                   false);

               tgl_assert(s32_PrevTxListIndex >= 0);
               tgl_assert(s32_PrevRxListIndex >= 0);
               tgl_assert(s32_NewTxListIndex >= 0);
               tgl_assert(s32_NewRxListIndex >= 0);

               if ((((s32_PrevTxListIndex >= 0) && (s32_PrevRxListIndex >= 0)) && (s32_NewTxListIndex >= 0)) &&
                   (s32_NewRxListIndex >= 0))
               {
                  const uint32_t u32_PrevTxListIndex = static_cast<uint32_t>(s32_PrevTxListIndex);
                  const uint32_t u32_PrevRxListIndex = static_cast<uint32_t>(s32_PrevRxListIndex);
                  const uint32_t u32_NewTxListIndex = static_cast<uint32_t>(s32_NewTxListIndex);
                  const uint32_t u32_NewRxListIndex = static_cast<uint32_t>(s32_NewRxListIndex);

                  tgl_assert(u32_PrevTxListIndex < rc_Datapool.c_Lists.size());
                  tgl_assert(u32_PrevRxListIndex < rc_Datapool.c_Lists.size());
                  tgl_assert(u32_NewTxListIndex < rc_Datapool.c_Lists.size());
                  tgl_assert(u32_NewRxListIndex < rc_Datapool.c_Lists.size());

                  if ((((u32_PrevTxListIndex < rc_Datapool.c_Lists.size()) &&
                        (u32_PrevRxListIndex < rc_Datapool.c_Lists.size())) &&
                       (u32_NewTxListIndex < rc_Datapool.c_Lists.size())) &&
                      (u32_NewRxListIndex < rc_Datapool.c_Lists.size()))
                  {
                     C_OscNodeDataPoolList & rc_PrevTxList = rc_Datapool.c_Lists[u32_PrevTxListIndex];
                     C_OscNodeDataPoolList & rc_PrevRxList = rc_Datapool.c_Lists[u32_PrevRxListIndex];
                     C_OscNodeDataPoolList & rc_NewTxList = rc_Datapool.c_Lists[u32_NewTxListIndex];
                     C_OscNodeDataPoolList & rc_NewRxList = rc_Datapool.c_Lists[u32_NewRxListIndex];
                     //Should be handled
                     tgl_assert(rc_NewTxList.c_Elements.size() == 0UL);
                     tgl_assert(rc_NewRxList.c_Elements.size() == 0UL);
                     //Copy
                     rc_NewTxList = rc_PrevTxList;
                     rc_NewRxList = rc_PrevRxList;
                     //Clear
                     rc_PrevTxList.c_Elements.clear();
                     rc_PrevRxList.c_Elements.clear();
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move messages in UI datapool

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_PrevInterfaceIndex    Previous interface index
   \param[in]  ou32_NewInterfaceIndex     New interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInUiDatapool(const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_PrevInterfaceIndex,
                                                            const uint32_t ou32_NewInterfaceIndex)
{
   tgl_assert((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) && (ou32_NodeIndex < this->mc_UiNodes.size()));
   if ((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) && (ou32_NodeIndex < this->mc_UiNodes.size()))
   {
      const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         const C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            tgl_assert(rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size());
            tgl_assert(rc_Prot.u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size());
            if ((rc_Prot.u32_DataPoolIndex < rc_UiNode.c_UiDataPools.size()) &&
                (rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size()))
            {
               const C_OscNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Prot.u32_DataPoolIndex];

               const int32_t s32_PrevTxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool,
                                                                                    ou32_PrevInterfaceIndex,
                                                                                    true);
               const int32_t s32_PrevRxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool,
                                                                                    ou32_PrevInterfaceIndex,
                                                                                    false);
               const int32_t s32_NewTxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                   true);
               const int32_t s32_NewRxListIndex = C_OscCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                   false);

               tgl_assert(s32_PrevTxListIndex >= 0);
               tgl_assert(s32_PrevRxListIndex >= 0);
               tgl_assert(s32_NewTxListIndex >= 0);
               tgl_assert(s32_NewRxListIndex >= 0);

               if ((((s32_PrevTxListIndex >= 0) && (s32_PrevRxListIndex >= 0)) && (s32_NewTxListIndex >= 0)) &&
                   (s32_NewRxListIndex >= 0))
               {
                  C_PuiSdNodeDataPool & rc_UiDatapool = rc_UiNode.c_UiDataPools[rc_Prot.u32_DataPoolIndex];
                  const uint32_t u32_PrevTxListIndex = static_cast<uint32_t>(s32_PrevTxListIndex);
                  const uint32_t u32_PrevRxListIndex = static_cast<uint32_t>(s32_PrevRxListIndex);
                  const uint32_t u32_NewTxListIndex = static_cast<uint32_t>(s32_NewTxListIndex);
                  const uint32_t u32_NewRxListIndex = static_cast<uint32_t>(s32_NewRxListIndex);

                  tgl_assert(u32_PrevTxListIndex < rc_UiDatapool.c_DataPoolLists.size());
                  tgl_assert(u32_PrevRxListIndex < rc_UiDatapool.c_DataPoolLists.size());
                  tgl_assert(u32_NewTxListIndex < rc_UiDatapool.c_DataPoolLists.size());
                  tgl_assert(u32_NewRxListIndex < rc_UiDatapool.c_DataPoolLists.size());

                  if ((((u32_PrevTxListIndex < rc_UiDatapool.c_DataPoolLists.size()) &&
                        (u32_PrevRxListIndex < rc_UiDatapool.c_DataPoolLists.size())) &&
                       (u32_NewTxListIndex < rc_UiDatapool.c_DataPoolLists.size())) &&
                      (u32_NewRxListIndex < rc_UiDatapool.c_DataPoolLists.size()))
                  {
                     C_PuiSdNodeDataPoolList & rc_PrevTxList = rc_UiDatapool.c_DataPoolLists[u32_PrevTxListIndex];
                     C_PuiSdNodeDataPoolList & rc_PrevRxList = rc_UiDatapool.c_DataPoolLists[u32_PrevRxListIndex];
                     C_PuiSdNodeDataPoolList & rc_NewTxList = rc_UiDatapool.c_DataPoolLists[u32_NewTxListIndex];
                     C_PuiSdNodeDataPoolList & rc_NewRxList = rc_UiDatapool.c_DataPoolLists[u32_NewRxListIndex];
                     //Should be handled
                     tgl_assert(rc_NewTxList.c_DataPoolListElements.size() == 0UL);
                     tgl_assert(rc_NewRxList.c_DataPoolListElements.size() == 0UL);
                     //Copy
                     rc_NewTxList = rc_PrevTxList;
                     rc_NewRxList = rc_PrevRxList;
                     //Clear
                     rc_PrevTxList.c_DataPoolListElements.clear();
                     rc_PrevRxList.c_DataPoolListElements.clear();
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move messages in core protocol

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_PrevInterface    Previous interface number
   \param[in]  ou8_NewInterface     New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInOscProtocol(const uint32_t ou32_NodeIndex,
                                                             const uint8_t ou8_PrevInterface,
                                                             const uint8_t ou8_NewInterface)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            tgl_assert(static_cast<uint32_t>(ou8_PrevInterface) < rc_Prot.c_ComMessages.size());
            tgl_assert(static_cast<uint32_t>(ou8_NewInterface) < rc_Prot.c_ComMessages.size());
            if ((static_cast<uint32_t>(ou8_PrevInterface) < rc_Prot.c_ComMessages.size()) &&
                (static_cast<uint32_t>(ou8_NewInterface) < rc_Prot.c_ComMessages.size()))
            {
               C_OscCanMessageContainer & rc_PrevInterfaceMessages =
                  rc_Prot.c_ComMessages[static_cast<uint32_t>(ou8_PrevInterface)];
               C_OscCanMessageContainer & rc_NewInterfaceMessages =
                  rc_Prot.c_ComMessages[static_cast<uint32_t>(ou8_NewInterface)];
               //Should be handled
               tgl_assert(rc_NewInterfaceMessages.c_RxMessages.size() == 0UL);
               tgl_assert(rc_NewInterfaceMessages.c_TxMessages.size() == 0UL);
               //Copy
               rc_NewInterfaceMessages = rc_PrevInterfaceMessages;
               //Clear
               rc_PrevInterfaceMessages.c_RxMessages.clear();
               rc_PrevInterfaceMessages.c_TxMessages.clear();
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Move messages in UI protocol

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_PrevInterface    Previous interface number
   \param[in]  ou8_NewInterface     New interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInUiProtocol(const uint32_t ou32_NodeIndex,
                                                            const uint8_t ou8_PrevInterface,
                                                            const uint8_t ou8_NewInterface)
{
   tgl_assert(ou32_NodeIndex < this->mc_UiNodes.size());
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if ((ou32_NodeIndex < this->mc_UiNodes.size()) && (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()))
   {
      C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      tgl_assert(rc_UiNode.c_UiCanProtocols.size() == rc_Node.c_ComProtocols.size());
      if (rc_UiNode.c_UiCanProtocols.size() == rc_Node.c_ComProtocols.size())
      {
         for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_UiNode.c_UiCanProtocols.size(); ++u32_ItProt)
         {
            C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
            if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
            {
               C_PuiSdNodeCanProtocol & rc_UiProt = rc_UiNode.c_UiCanProtocols[u32_ItProt];
               tgl_assert(static_cast<uint32_t>(ou8_PrevInterface) < rc_UiProt.c_ComMessages.size());
               tgl_assert(static_cast<uint32_t>(ou8_NewInterface) < rc_UiProt.c_ComMessages.size());
               if ((static_cast<uint32_t>(ou8_PrevInterface) < rc_UiProt.c_ComMessages.size()) &&
                   (static_cast<uint32_t>(ou8_NewInterface) < rc_UiProt.c_ComMessages.size()))
               {
                  C_PuiSdNodeCanMessageContainer & rc_PrevInterfaceMessages =
                     rc_UiProt.c_ComMessages[static_cast<uint32_t>(ou8_PrevInterface)];
                  C_PuiSdNodeCanMessageContainer & rc_NewInterfaceMessages =
                     rc_UiProt.c_ComMessages[static_cast<uint32_t>(ou8_NewInterface)];
                  //Should be handled
                  tgl_assert(rc_NewInterfaceMessages.c_RxMessages.size() == 0UL);
                  tgl_assert(rc_NewInterfaceMessages.c_TxMessages.size() == 0UL);
                  //Copy
                  rc_NewInterfaceMessages = rc_PrevInterfaceMessages;
                  //Clear
                  rc_PrevInterfaceMessages.c_RxMessages.clear();
                  rc_PrevInterfaceMessages.c_TxMessages.clear();
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change of complete connection for CANopen

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  orc_NewId         New ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeCompleteConnectionForCanOpen(const uint32_t ou32_NodeIndex,
                                                                            const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                            const C_PuiSdNodeConnectionId & orc_NewId)
{
   m_HandleChangeCompleteConnectionForCanOpenManager(ou32_NodeIndex, orc_PrevId, orc_NewId);
   m_HandleChangeCompleteConnectionForCanOpenDevice(ou32_NodeIndex, orc_PrevId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change of complete connection for CANopen manager

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
   \param[in]  orc_NewId         New ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeCompleteConnectionForCanOpenManager(const uint32_t ou32_NodeIndex,
                                                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                                   const C_PuiSdNodeConnectionId & orc_NewId)
{
   if (orc_PrevId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      DeleteAllCanOpenManagerDevices(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber);
   }
   if (orc_NewId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      DeleteAllCanOpenManagerDevices(ou32_NodeIndex, orc_NewId.u8_InterfaceNumber);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle change of complete connection for CANopen device

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  orc_PrevId        Previous ID
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeCompleteConnectionForCanOpenDevice(const uint32_t ou32_NodeIndex,
                                                                                  const C_PuiSdNodeConnectionId & orc_PrevId)
{
   if (orc_PrevId.e_InterfaceType == C_OscSystemBus::eCAN)
   {
      const C_OscCanInterfaceId c_Id(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber);
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
      {
         const C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItManager =
                 rc_Node.c_CanOpenManagers.begin();
              c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
         {
            this->DeleteCanOpenManagerDevice(u32_ItNode, c_ItManager->first, c_Id);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Synchronise openSYDE node id change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou8_NewNodeId        New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_SyncOsyNodeIdChange(const uint32_t ou32_NodeIndex,
                                                       const uint32_t ou32_InterfaceIndex, const uint8_t ou8_NewNodeId)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      tgl_assert(ou32_InterfaceIndex < rc_OscNode.c_Properties.c_ComInterfaces.size());
      if (ou32_InterfaceIndex < rc_OscNode.c_Properties.c_ComInterfaces.size())
      {
         if (rc_OscNode.c_Properties.c_ComInterfaces[ou32_InterfaceIndex].e_InterfaceType == C_OscSystemBus::eCAN)
         {
            uint8_t u8_InterfaceNumber;

            m_HandleOsyNodeIdChangeForCanOpenManager(ou32_NodeIndex, ou32_InterfaceIndex, ou8_NewNodeId);
            m_HandleOsyNodeIdChangeForCanOpenDevice(ou32_NodeIndex, ou32_InterfaceIndex, ou8_NewNodeId);

            if (this->TranslateCanInterfaceIndexToId(ou32_NodeIndex, ou32_InterfaceIndex,
                                                     u8_InterfaceNumber) == C_NO_ERR)
            {
               m_HandleNodeIdChangeForCanOpenMessages(ou32_NodeIndex, u8_InterfaceNumber);
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle openSYDE node id change to CANopen manager

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou8_NewNodeId        New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleOsyNodeIdChangeForCanOpenManager(const uint32_t ou32_NodeIndex,
                                                                          const uint32_t ou32_InterfaceIndex,
                                                                          const uint8_t ou8_NewNodeId)
{
   uint8_t u8_InterfaceNumber;

   if (this->TranslateCanInterfaceIndexToId(ou32_NodeIndex, ou32_InterfaceIndex, u8_InterfaceNumber) == C_NO_ERR)
   {
      tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
      if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
         const std::map<uint8_t,
                        C_OscCanOpenManagerInfo>::iterator c_ItCanOpenManager = rc_OscNode.c_CanOpenManagers.find(
            u8_InterfaceNumber);
         if (c_ItCanOpenManager != rc_OscNode.c_CanOpenManagers.end())
         {
            //TODO SSI check
            if (c_ItCanOpenManager->second.q_UseOpenSydeNodeId == true)
            {
               c_ItCanOpenManager->second.u8_NodeIdValue = ou8_NewNodeId;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle openSYDE node id change for CANopen device

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_InterfaceIndex  Interface index
   \param[in]  ou8_NewNodeId        New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleOsyNodeIdChangeForCanOpenDevice(const uint32_t ou32_NodeIndex,
                                                                         const uint32_t ou32_InterfaceIndex,
                                                                         const uint8_t ou8_NewNodeId)
{
   uint8_t u8_InterfaceNumber;

   if (this->TranslateCanInterfaceIndexToId(ou32_NodeIndex, ou32_InterfaceIndex, u8_InterfaceNumber) == C_NO_ERR)
   {
      const C_OscCanInterfaceId c_DeviceId(ou32_NodeIndex, u8_InterfaceNumber);
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
      {
         C_OscNode & rc_OscNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (std::map<uint8_t,
                       C_OscCanOpenManagerInfo>::iterator c_ItCanOpenManager = rc_OscNode.c_CanOpenManagers.begin();
              c_ItCanOpenManager != rc_OscNode.c_CanOpenManagers.end(); ++c_ItCanOpenManager)
         {
            const std::map<C_OscCanInterfaceId,
                           C_OscCanOpenManagerDeviceInfo>::iterator c_ItCanOpenDevice =
               c_ItCanOpenManager->second.c_CanOpenDevices.find(c_DeviceId);
            if (c_ItCanOpenDevice != c_ItCanOpenManager->second.c_CanOpenDevices.end())
            {
               //TODO SSI check
               if (c_ItCanOpenDevice->second.q_UseOpenSydeNodeId == true)
               {
                  c_ItCanOpenDevice->second.u8_NodeIdValue = ou8_NewNodeId;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle node id change to CANopen message

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleNodeIdChangeForCanOpenMessages(const uint32_t ou32_NodeIndex,
                                                                        const uint8_t ou8_InterfaceNumber)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      const C_OscCanInterfaceId c_DeviceId(ou32_NodeIndex, ou8_InterfaceNumber);
      const C_OscCanOpenManagerDeviceInfo * const pc_FoundDevice = this->GetCanOpenManagerDeviceForAnyManager(
         u32_ItNode,
         c_DeviceId);
      if (pc_FoundDevice != NULL)
      {
         C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (uint32_t u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
         {
            C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
            if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
            {
               for (uint32_t u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
               {
                  C_OscCanMessageContainer & rc_PrevInterfaceMessages =
                     rc_Prot.c_ComMessages[u32_ItContainer];
                  C_PuiSdHandlerCanOpenLogic::mh_HandleNodeIdChangeForCanOpenMessages(c_DeviceId,
                                                                                      pc_FoundDevice->u8_NodeIdValue,
                                                                                      rc_PrevInterfaceMessages.c_RxMessages);
                  C_PuiSdHandlerCanOpenLogic::mh_HandleNodeIdChangeForCanOpenMessages(c_DeviceId,
                                                                                      pc_FoundDevice->u8_NodeIdValue,
                                                                                      rc_PrevInterfaceMessages.c_TxMessages);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle node id change for CANopen messages

   \param[in]      orc_DeviceId     Device id
   \param[in]      ou8_NewNodeId    New node id
   \param[in,out]  orc_Messages     Messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::mh_HandleNodeIdChangeForCanOpenMessages(const C_OscCanInterfaceId & orc_DeviceId,
                                                                         const uint8_t ou8_NewNodeId,
                                                                         std::vector<C_OscCanMessage> & orc_Messages)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      C_OscCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex == orc_DeviceId)
      {
         if (rc_Message.q_CanOpenManagerCobIdIncludesNodeId)
         {
            rc_Message.u32_CanId = rc_Message.u32_CanOpenManagerCobIdOffset + ou8_NewNodeId;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle PDO SYNC change to CANopen message

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandlePdoSyncChangeForCanOpenMessages(const uint32_t ou32_NodeIndex,
                                                                         const uint8_t ou8_InterfaceNumber)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OscNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      uint32_t u32_InterfaceIndex;

      // Searching the matching interface index
      for (u32_InterfaceIndex = 0U; u32_InterfaceIndex < rc_Node.c_Properties.c_ComInterfaces.size();
           ++u32_InterfaceIndex)
      {
         if ((rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceIndex].e_InterfaceType == C_OscSystemBus::eCAN) &&
             (rc_Node.c_Properties.c_ComInterfaces[u32_InterfaceIndex].u8_InterfaceNumber == ou8_InterfaceNumber))
         {
            // Get the CANopen manager
            const std::map<uint8_t, C_OscCanOpenManagerInfo>::const_iterator c_ItCoManager =
               rc_Node.c_CanOpenManagers.find(ou8_InterfaceNumber);

            if (c_ItCoManager != rc_Node.c_CanOpenManagers.end())
            {
               uint32_t u32_ProtCounter;
               // The timeout interval must be updated if the SYNC PDO period time was changed
               const uint32_t u32_SyncPeriodMs = c_ItCoManager->second.u32_SyncCyclePeriodUs / 1000U;
               const C_PuiSdNode & rc_UiNode = this->mc_UiNodes[ou32_NodeIndex];

               // Search the CANopen protocol
               for (u32_ProtCounter = 0U; u32_ProtCounter < rc_Node.c_ComProtocols.size(); ++u32_ProtCounter)
               {
                  C_OscCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ProtCounter];
                  if (rc_Prot.e_Type == C_OscCanProtocol::eCAN_OPEN)
                  {
                     // Update RX messages
                     mh_HandlePdoSyncChangeForCanOpenMessages(
                        u32_SyncPeriodMs,
                        rc_Prot.c_ComMessages[u32_InterfaceIndex].c_RxMessages,
                        rc_UiNode.c_UiCanProtocols[u32_ProtCounter].c_ComMessages[u32_InterfaceIndex].c_RxMessages);

                     // Update TX messages
                     mh_HandlePdoSyncChangeForCanOpenMessages(
                        u32_SyncPeriodMs,
                        rc_Prot.c_ComMessages[u32_InterfaceIndex].c_TxMessages,
                        rc_UiNode.c_UiCanProtocols[u32_ProtCounter].c_ComMessages[u32_InterfaceIndex].c_TxMessages);
                  }
               }
            }

            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle PDO SYNC change to CANopen message

   \param[in]       ou32_PdoSyncPeriodMs     New PDO SYNC period in ms
   \param[out]      orc_Messages             Core CAN messages to check and adapt
   \param[in]       orc_UiMessages           UI CAN messages to check for activated automatic timeout mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::mh_HandlePdoSyncChangeForCanOpenMessages(const uint32_t ou32_PdoSyncPeriodMs,
                                                                          std::vector<C_OscCanMessage> & orc_Messages,
                                                                          const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessages)
{
   tgl_assert(orc_Messages.size() == orc_UiMessages.size());
   if (orc_Messages.size() == orc_UiMessages.size())
   {
      uint32_t u32_MsgCounter;

      for (u32_MsgCounter = 0U; u32_MsgCounter < orc_Messages.size(); ++u32_MsgCounter)
      {
         const C_PuiSdNodeCanMessage & rc_UiMsg = orc_UiMessages[u32_MsgCounter];

         // Adaption if automatic timeout mode is active for the message
         if (rc_UiMsg.e_ReceiveTimeoutMode == C_PuiSdNodeCanMessage::eRX_TIMEOUT_MODE_AUTO)
         {
            C_OscCanMessage & rc_Msg = orc_Messages[u32_MsgCounter];

            // Check for the PDO SYNC TX methods
            if (rc_Msg.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_0)
            {
               rc_Msg.u32_TimeoutMs = (ou32_PdoSyncPeriodMs * 3U) + 10U;
            }
            else if (rc_Msg.e_TxMethod == C_OscCanMessage::eTX_METHOD_CAN_OPEN_TYPE_1_TO_240)
            {
               rc_Msg.u32_TimeoutMs =
                  ((ou32_PdoSyncPeriodMs * static_cast<uint32_t>(rc_Msg.u8_CanOpenTxMethodAdditionalInfo)) * 3U) + 10U;
            }
            else
            {
               // Nothing to do
            }
         }
      }
   }
}
