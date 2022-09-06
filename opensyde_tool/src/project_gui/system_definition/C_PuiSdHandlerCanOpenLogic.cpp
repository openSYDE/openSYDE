//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen logic part of UI system definition

   CANopen logic part of UI system definition

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_PuiSdHandlerCanOpenLogic.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

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
const C_OSCCanOpenManagerInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManager(const uint32 ou32_NodeIndex,
                                                                              const uint8 ou8_InterfaceNumber) const
{
   const C_OSCCanOpenManagerInfo * pc_Retval = NULL;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::const_iterator c_ItManager = pc_Node->c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != pc_Node->c_CanOpenManagers.end())
      {
         pc_Retval = &c_ItManager->second;
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get OSC node which has the CANopen manager on a specific bus

   \param[in]  ou32_BusIndex  Index of CAN bus

   \return
   NULL Data not found
   Else Valid data
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCNode * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerNodeOnBus(const uint32 ou32_BusIndex) const
{
   const C_OSCNode * pc_Retval = NULL;
   uint32 u32_NodeIndex;

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
sint32 C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerNodeOnBus(const stw_types::uint32 ou32_BusIndex,
                                                              stw_types::uint32 & oru32_ManagerNodeIndex,
                                                              stw_types::uint8 * const opu8_ManagerIntfNumber) const
{
   sint32 s32_Return = C_RANGE;

   uint32 u32_NodeCounter;

   // Search all nodes
   for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_CoreDefinition.c_Nodes.size(); ++u32_NodeCounter)
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_NodeCounter];

      // Only necessary if at least one manager exists
      if (rc_Node.c_CanOpenManagers.size() > 0)
      {
         uint32 u32_IntfCounter;

         for (u32_IntfCounter = 0U; u32_IntfCounter < rc_Node.c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
         {
            const C_OSCNodeComInterfaceSettings & rc_Intf = rc_Node.c_Properties.c_ComInterfaces[u32_IntfCounter];

            if ((rc_Intf.e_InterfaceType == C_OSCSystemBus::eCAN) &&
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
const C_OSCCanOpenManagerDeviceInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerDevice(const uint32 ou32_NodeIndex,
                                                                                          const uint8 ou8_InterfaceNumber,
                                                                                          const C_OSCCanInterfaceId & orc_DeviceId)
const
{
   const C_OSCCanOpenManagerDeviceInfo * pc_Retval = NULL;
   const C_OSCCanOpenManagerInfo * const pc_Manager = this->GetCanOpenManager(ou32_NodeIndex, ou8_InterfaceNumber);

   if (pc_Manager != NULL)
   {
      const std::map<C_OSCCanInterfaceId,
                     C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice = pc_Manager->c_CanOpenDevices.find(
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
const C_OSCCanOpenManagerDeviceInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerDevice(
   const C_OSCCanMessageIdentificationIndices & orc_MessageId) const
{
   const C_OSCCanOpenManagerDeviceInfo * pc_Manager = NULL;

   // The manager must be the only node associated by this message
   // If the message is not associated to CANopen no manager will be found
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(orc_MessageId.u32_NodeIndex);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_ItManager;
      tgl_assert(orc_MessageId.u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size());
      c_ItManager = pc_Node->c_CanOpenManagers.find(
         pc_Node->c_Properties.c_ComInterfaces[orc_MessageId.u32_InterfaceIndex].u8_InterfaceNumber);
      tgl_assert(c_ItManager != pc_Node->c_CanOpenManagers.end());
      if (c_ItManager != pc_Node->c_CanOpenManagers.end())
      {
         const C_OSCCanMessage * const pc_Message = this->GetCanMessage(orc_MessageId);

         tgl_assert(pc_Message != NULL);
         if (pc_Message != NULL)
         {
            const C_OSCCanOpenManagerInfo & rc_CoManagerInfo = c_ItManager->second;
            const std::map<C_OSCCanInterfaceId,
                           C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
               rc_CoManagerInfo.c_CanOpenDevices.find(
                  pc_Message->c_CanOpenManagerOwnerNodeIndex);

            tgl_assert(c_ItDevice != rc_CoManagerInfo.c_CanOpenDevices.end());
            if (c_ItDevice != rc_CoManagerInfo.c_CanOpenDevices.end())
            {
               pc_Manager = &c_ItDevice->second;
            }
         }
      }
   }

   return pc_Manager;
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
sint32 C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerOfDeviceAndId(const stw_types::uint32 ou32_DeviceNodeIndex,
                                                                  stw_types::uint32 * const opu32_ManagerNodeIndex,
                                                                  stw_types::uint8 * const opu8_ManagerInterfaceNumber,
                                                                  stw_opensyde_core::C_OSCCanInterfaceId * const opc_DeviceNodeId)
const
{
   sint32 s32_Retval = C_RANGE;

   if (ou32_DeviceNodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      uint32 u32_NodeCounter;

      s32_Retval = C_CONFIG;

      // Searching in all node CANopen Managers for the searched node as device
      for (u32_NodeCounter = 0U; u32_NodeCounter < this->mc_CoreDefinition.c_Nodes.size(); ++u32_NodeCounter)
      {
         const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_NodeCounter];

         std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_ItManager;
         for (c_ItManager = rc_Node.c_CanOpenManagers.begin(); c_ItManager != rc_Node.c_CanOpenManagers.end();
              ++c_ItManager)
         {
            const C_OSCCanOpenManagerInfo & rc_Manager = c_ItManager->second;
            std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice;

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
const C_OSCCanOpenManagerDeviceInfo * C_PuiSdHandlerCanOpenLogic::GetCanOpenManagerDeviceForAnyManager(
   const uint32 ou32_ManagerNodeIndex, const C_OSCCanInterfaceId & orc_DeviceId) const
{
   const C_OSCCanOpenManagerDeviceInfo * pc_Retval = NULL;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_ManagerNodeIndex);

   if (pc_Node != NULL)
   {
      for (std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_ItManager =
              pc_Node->c_CanOpenManagers.cbegin();
           (c_ItManager != pc_Node->c_CanOpenManagers.cend()) && (pc_Retval == NULL); ++c_ItManager)
      {
         const std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
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
   \param[in]  oq_UseOpenSYDENodeID          Use open SYDE node ID
   \param[in]  ou8_NodeIDValue               Node ID value
   \param[in]  ou16_GlobalSDOTimeoutMs       Global SDO timeout ms
   \param[in]  oq_AutostartCanOpenManager    Autostart CANopen manager
   \param[in]  oq_StartDevices               Start devices
   \param[in]  oq_NMTStartAll                NMT start all
   \param[in]  oe_NMTErrorBehaviour          NMT error behaviour

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerCommonProperties(const uint32 ou32_NodeIndex,
                                                                     const uint8 ou8_InterfaceNumber,
                                                                     const bool oq_UseOpenSYDENodeID,
                                                                     const uint8 ou8_NodeIDValue,
                                                                     const uint16 ou16_GlobalSDOTimeoutMs,
                                                                     const bool oq_AutostartCanOpenManager,
                                                                     const bool oq_StartDevices,
                                                                     const bool oq_NMTStartAll,
                                                                     const C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType oe_NMTErrorBehaviour)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         c_ItManager->second.q_UseOpenSYDENodeID = oq_UseOpenSYDENodeID;
         c_ItManager->second.u8_NodeIDValue = ou8_NodeIDValue;
         c_ItManager->second.u16_GlobalSDOTimeoutMs = ou16_GlobalSDOTimeoutMs;
         c_ItManager->second.q_AutostartCanOpenManager = oq_AutostartCanOpenManager;
         c_ItManager->second.q_StartDevices = oq_StartDevices;
         c_ItManager->second.q_NMTStartAll = oq_NMTStartAll;
         c_ItManager->second.e_NMTErrorBehaviour = oe_NMTErrorBehaviour;
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
sint32 C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerProducerHeartbeat(const uint32 ou32_NodeIndex,
                                                                      const uint8 ou8_InterfaceNumber,
                                                                      const uint16 ou16_HeartbeatProducerTimeMs,
                                                                      const bool oq_EnableHeartbeatProducing)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         c_ItManager->second.u16_HeartbeatProducerTimeMs = ou16_HeartbeatProducerTimeMs;
         c_ItManager->second.q_EnableHeartbeatProducing = oq_EnableHeartbeatProducing;

         //update consumer time of devices
         for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItDevice != c_ItManager->second.c_CanOpenDevices.end(); ++c_ItDevice)
         {
            if (c_ItDevice->second.q_EnableHeartbeatConsumingAutoCalculation == true)
            {
               //auto calculation of consumer time
               const float32 f32_Temp = static_cast<float32>(ou16_HeartbeatProducerTimeMs);
               const float32 f32_Result = f32_Temp * mf32_HEARTBEAT_CONSUMER_TIME_FACTOR;
               const uint16 u16_Result = static_cast<uint16>(f32_Result);

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
sint32 C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerDeviceCommonProperties(const uint32 ou32_NodeIndex,
                                                                           const uint8 ou8_InterfaceNumber,
                                                                           const C_OSCCanInterfaceId & orc_DeviceId,
                                                                           const bool oq_DeviceOptional,
                                                                           const bool oq_NoInitialization,
                                                                           const bool oq_FactorySettingsActive,
                                                                           const uint8 ou8_ResetNodeObjectDictionarySubIndex, const bool oq_EnableHeartbeatProducing, const uint16 ou16_HeartbeatProducerTimeMs, const bool oq_EnableHeartbeatConsuming, const stw_types::uint16 ou16_HeartbeatConsumerTimeMs,
                                                                           const bool oq_EnableHeartbeatConsumingAutoCalculation)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::iterator c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.
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
   \param[in]  oq_UseOpenSYDENodeID    Use open SYDE node ID
   \param[in]  ou8_NodeIDValue         Node ID value

   \return
   STW error codes

   \retval   C_NO_ERR   Operation success
   \retval   C_RANGE    Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSdHandlerCanOpenLogic::SetCanOpenManagerDeviceNodeId(const uint32 ou32_NodeIndex,
                                                                 const uint8 ou8_InterfaceNumber,
                                                                 const C_OSCCanInterfaceId & orc_DeviceId,
                                                                 const bool oq_UseOpenSYDENodeID,
                                                                 const uint8 ou8_NodeIDValue)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::iterator c_ItSubDevice = c_ItManager->second.c_CanOpenDevices.
                                                                                 find(orc_DeviceId);
         if (c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItSubDevice->second.q_UseOpenSYDENodeID = oq_UseOpenSYDENodeID;
            c_ItSubDevice->second.u8_NodeIDValue = ou8_NodeIDValue;
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
sint32 C_PuiSdHandlerCanOpenLogic::AddCanOpenManager(const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceNumber,
                                                     const C_OSCCanOpenManagerInfo & orc_Config,
                                                     bool & orq_DatapoolChanged)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::const_iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager == rc_Node.c_CanOpenManagers.end())
      {
         uint32 u32_InterfaceIndex = 0UL;
         rc_Node.c_CanOpenManagers[ou8_InterfaceNumber] = orc_Config;
         orq_DatapoolChanged = this->m_HandleCreateCanOpenDatapool(ou32_NodeIndex);
         if (this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, ou8_InterfaceNumber,
                                                      u32_InterfaceIndex) == C_NO_ERR)
         {
            this->SetCanProtocolMessageContainerConnected(ou32_NodeIndex,
                                                          C_OSCCanProtocol::eCAN_OPEN,
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
sint32 C_PuiSdHandlerCanOpenLogic::DeleteCanOpenManager(const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceNumber,
                                                        const bool oq_AllowDatapoolRemove, bool & orq_DatapoolChanged)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::const_iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         uint32 u32_InterfaceIndex = 0UL;
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
                                                          C_OSCCanProtocol::eCAN_OPEN,
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
sint32 C_PuiSdHandlerCanOpenLogic::AddCanOpenManagerDevice(const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceNumber,
                                                           const C_OSCCanInterfaceId & orc_DeviceId,
                                                           const C_OSCCanOpenManagerDeviceInfo & orc_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItSubDevice =
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
sint32 C_PuiSdHandlerCanOpenLogic::DeleteCanOpenManagerDevice(const uint32 ou32_NodeIndex,
                                                              const uint8 ou8_InterfaceNumber,
                                                              const C_OSCCanInterfaceId & orc_DeviceId)
{
   sint32 s32_Retval;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         const std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItSubDevice =
            c_ItManager->second.c_CanOpenDevices.
            find(orc_DeviceId);
         if (c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end())
         {
            c_ItManager->second.c_CanOpenDevices.erase(c_ItSubDevice);
            s32_Retval = this->m_DeleteCanOpenManagerMessages(ou32_NodeIndex, ou8_InterfaceNumber, orc_DeviceId);
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
sint32 C_PuiSdHandlerCanOpenLogic::DeleteAllCanOpenManagerDevices(const uint32 ou32_NodeIndex,
                                                                  const uint8 ou8_InterfaceNumber)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      const std::map<stw_types::uint8,
                     C_OSCCanOpenManagerInfo>::const_iterator c_ItManager = rc_Node.c_CanOpenManagers.find(
         ou8_InterfaceNumber);
      if (c_ItManager != rc_Node.c_CanOpenManagers.end())
      {
         //Delete items (handle DP)
         std::vector<C_OSCCanInterfaceId> c_Items;
         c_Items.reserve(c_ItManager->second.c_CanOpenDevices.size());
         for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItDevice != c_ItManager->second.c_CanOpenDevices.end(); ++c_ItDevice)
         {
            c_Items.push_back(c_ItDevice->first);
         }
         for (uint32 u32_ItDevice = 0UL; u32_ItDevice < c_Items.size(); ++u32_ItDevice)
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
sint32 C_PuiSdHandlerCanOpenLogic::TranslateCanInterfaceIndexToId(const uint32 ou32_NodeIndex,
                                                                  const uint32 ou32_InterfaceIndex,
                                                                  uint8 & oru8_InterfaceId) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      if (ou32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
      {
         const C_OSCNodeComInterfaceSettings & rc_Interface =
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
sint32 C_PuiSdHandlerCanOpenLogic::TranslateCanInterfaceNumberToIndex(const uint32 ou32_NodeIndex,
                                                                      const uint8 ou8_InterfaceNumber,
                                                                      uint32 & oru32_InterfaceIndex) const
{
   sint32 s32_Retval = C_NO_ERR;
   const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32 u32_Index = 0; u32_Index < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_Index)
      {
         const C_OSCNodeComInterfaceSettings & rc_CurComInterface =
            pc_Node->c_Properties.c_ComInterfaces[u32_Index];
         //Check if match
         if ((rc_CurComInterface.e_InterfaceType == C_OSCSystemBus::eCAN) &&
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
sint32 C_PuiSdHandlerCanOpenLogic::m_DeleteCanOpenManagerMessages(const uint32 ou32_NodeIndex,
                                                                  const uint8 ou8_InterfaceNumber,
                                                                  const C_OSCCanInterfaceId & orc_DeviceId)
{
   sint32 s32_Retval = C_NO_ERR;
   uint32 u32_InterfaceIndex = 0UL;

   if (this->TranslateCanInterfaceNumberToIndex(ou32_NodeIndex, ou8_InterfaceNumber, u32_InterfaceIndex) == C_NO_ERR)
   {
      const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);
      if (pc_Node != NULL)
      {
         for (uint32 u32_ItProt = 0UL; (u32_ItProt < pc_Node->c_ComProtocols.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItProt)
         {
            const C_OSCCanProtocol & rc_Prot = pc_Node->c_ComProtocols[u32_ItProt];

            if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
            {
               for (uint32 u32_ItContainer = 0UL;
                    (u32_ItContainer < rc_Prot.c_ComMessages.size()) && (s32_Retval == C_NO_ERR); ++u32_ItContainer)
               {
                  const C_OSCCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
                  const C_OSCCanMessageIdentificationIndices c_BaseId(ou32_NodeIndex, C_OSCCanProtocol::eCAN_OPEN,
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
sint32 C_PuiSdHandlerCanOpenLogic::m_DeleteCanOpenManagerMessageVectors(
   const C_OSCCanMessageIdentificationIndices & orc_MessageIdBase, const bool oq_VectorIsTx,
   const std::vector<C_OSCCanMessage> & orc_MessageVector, const C_OSCCanInterfaceId & orc_DeviceId)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItMessage = orc_MessageVector.size(); (u32_ItMessage > 0UL) && (s32_Retval == C_NO_ERR);
        --u32_ItMessage)
   {
      const uint32 u32_MessageIndex = u32_ItMessage - 1UL;
      const C_OSCCanMessage & rc_Message = orc_MessageVector[u32_MessageIndex];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex == orc_DeviceId)
      {
         const C_OSCCanMessageIdentificationIndices c_MessageId(orc_MessageIdBase.u32_NodeIndex,
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
bool C_PuiSdHandlerCanOpenLogic::m_HandleCreateCanOpenDatapool(const uint32 ou32_NodeIndex)
{
   bool q_Retval = false;

   if (this->GetOSCCanDataPools(ou32_NodeIndex,
                                C_OSCCanProtocol::eCAN_OPEN).size() == 0UL)
   {
      tgl_assert(this->AddAutoGenCommDataPool(ou32_NodeIndex,
                                              C_OSCCanProtocol::eCAN_OPEN) == C_NO_ERR);
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
bool C_PuiSdHandlerCanOpenLogic::m_HandleCleanUpCanOpenDatapool(const uint32 ou32_NodeIndex)
{
   bool q_Retval = false;

   if (this->m_CheckCleanUpCanOpenDatapoolPossible(ou32_NodeIndex))
   {
      const C_OSCNode * const pc_Node = this->GetOSCNodeConst(ou32_NodeIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         for (uint32 u32_ItProt = 0UL; u32_ItProt < pc_Node->c_ComProtocols.size(); ++u32_ItProt)
         {
            const C_OSCCanProtocol & rc_Prot = pc_Node->c_ComProtocols[u32_ItProt];
            if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
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
bool C_PuiSdHandlerCanOpenLogic::m_CheckCleanUpCanOpenDatapoolPossible(const uint32 ou32_NodeIndex) const
{
   bool q_Retval = false;
   const C_OSCNode * const pc_Node =  this->GetOSCNodeConst(ou32_NodeIndex);

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
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAdded(const uint32 ou32_Index)
{
   C_PuiSdHandlerHALC::m_HandleSyncNodeAdded(ou32_Index);
   m_HandleSyncNodeAddedForCanOpenDevices(ou32_Index);
   m_HandleSyncNodeAddedForCanOpenMessages(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node added for CANopen devices

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAddedForCanOpenDevices(const uint32 ou32_Index)
{
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::iterator c_ItManager =
              rc_Node.c_CanOpenManagers.begin();
           c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
      {
         std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo> c_Storage;
         for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end();)
         {
            if (c_ItSubDevice->first.u32_NodeIndex >= ou32_Index)
            {
               //Adapt
               C_OSCCanInterfaceId c_Tmp = c_ItSubDevice->first;
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
         for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItNewSubDevice =
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
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAddedForCanOpenMessages(const uint32 ou32_Index)
{
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            for (uint32 u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
            {
               C_OSCCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
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
void C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAddedForCanOpenMessages(const uint32 ou32_Index,
                                                                          std::vector<C_OSCCanMessage> & orc_Messages)
{
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      C_OSCCanMessage & rc_Message = orc_Messages[u32_ItMessage];
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
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeleted(const uint32 ou32_Index)
{
   C_PuiSdHandlerHALC::m_HandleSyncNodeAboutToBeDeleted(ou32_Index);
   m_HandleSyncNodeAboutToBeDeletedForCanOpenDevices(ou32_Index);
   m_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle sync node about to be deleted for CANopen devices

   \param[in]  ou32_Index  Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeletedForCanOpenDevices(const uint32 ou32_Index)
{
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::iterator c_ItManager =
              rc_Node.c_CanOpenManagers.begin();
           c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
      {
         std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo> c_Storage;
         for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
                 c_ItManager->second.c_CanOpenDevices.begin();
              c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end();)
         {
            if (c_ItSubDevice->first.u32_NodeIndex > ou32_Index)
            {
               //Adapt
               C_OSCCanInterfaceId c_Tmp = c_ItSubDevice->first;
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
         for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItNewSubDevice =
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
void C_PuiSdHandlerCanOpenLogic::m_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const uint32 ou32_Index)
{
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            for (uint32 u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
            {
               C_OSCCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
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
void C_PuiSdHandlerCanOpenLogic::mh_HandleSyncNodeAboutToBeDeletedForCanOpenMessages(const uint32 ou32_Index,
                                                                                     std::vector<C_OSCCanMessage> & orc_Messages)
{
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size();)
   {
      C_OSCCanMessage & rc_Message = orc_Messages[u32_ItMessage];
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpen(const uint32 ou32_NodeIndex,
                                                                    const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                    const uint8 ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OSCSystemBus::eCAN)
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenDevices(const uint32 ou32_NodeIndex,
                                                                           const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                           const uint8 ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OSCSystemBus::eCAN)
   {
      for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
      {
         C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::iterator c_ItManager =
                 rc_Node.c_CanOpenManagers.begin();
              c_ItManager != rc_Node.c_CanOpenManagers.end(); ++c_ItManager)
         {
            std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo> c_Storage;
            for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItSubDevice =
                    c_ItManager->second.c_CanOpenDevices.begin();
                 c_ItSubDevice != c_ItManager->second.c_CanOpenDevices.end();)
            {
               if ((c_ItSubDevice->first.u32_NodeIndex == ou32_NodeIndex) &&
                   (c_ItSubDevice->first.u8_InterfaceNumber == orc_PrevId.u8_InterfaceNumber))
               {
                  //Adapt
                  C_OSCCanInterfaceId c_Tmp = c_ItSubDevice->first;
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
            for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::iterator c_ItNewSubDevice =
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenManager(const uint32 ou32_NodeIndex,
                                                                           const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                           const uint8 ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OSCSystemBus::eCAN)
   {
      tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
      if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

         const std::map<stw_types::uint8,
                        C_OSCCanOpenManagerInfo>::const_iterator c_ItPrev = rc_Node.c_CanOpenManagers.find(
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenDeviceMessages(const uint32 ou32_NodeIndex,
                                                                                  const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                                  const uint8 ou8_NewInterface)
{
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
      for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            for (uint32 u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
            {
               C_OSCCanMessageContainer & rc_Container = rc_Prot.c_ComMessages[u32_ItContainer];
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
void C_PuiSdHandlerCanOpenLogic::mh_HandleChangeConnectionForCanOpenDeviceMessages(const uint32 ou32_NodeIndex,
                                                                                   const C_PuiSdNodeConnectionId & orc_PrevId, const uint8 ou8_NewInterface,
                                                                                   std::vector<C_OSCCanMessage> & orc_Messages)
{
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size();)
   {
      C_OSCCanMessage & rc_Message = orc_Messages[u32_ItMessage];
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeConnectionForCanOpenManagerMessages(const uint32 ou32_NodeIndex,
                                                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                                   const uint8 ou8_NewInterface)
{
   if (orc_PrevId.e_InterfaceType == C_OSCSystemBus::eCAN)
   {
      uint32 u32_PrevInterfaceIndex = 0UL;
      uint32 u32_NewInterfaceIndex = 0UL;
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
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInOscDatapool(const uint32 ou32_NodeIndex,
                                                             const uint32 ou32_PrevInterfaceIndex,
                                                             const uint32 ou32_NewInterfaceIndex)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         const C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            tgl_assert(rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size());
            if (rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size())
            {
               C_OSCNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Prot.u32_DataPoolIndex];

               const sint32 s32_PrevTxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_PrevInterfaceIndex,
                                                                                   true);
               const sint32 s32_PrevRxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_PrevInterfaceIndex,
                                                                                   false);
               const sint32 s32_NewTxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                  true);
               const sint32 s32_NewRxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                  false);

               tgl_assert(s32_PrevTxListIndex >= 0);
               tgl_assert(s32_PrevRxListIndex >= 0);
               tgl_assert(s32_NewTxListIndex >= 0);
               tgl_assert(s32_NewRxListIndex >= 0);

               if ((((s32_PrevTxListIndex >= 0) && (s32_PrevRxListIndex >= 0)) && (s32_NewTxListIndex >= 0)) &&
                   (s32_NewRxListIndex >= 0))
               {
                  const uint32 u32_PrevTxListIndex = static_cast<uint32>(s32_PrevTxListIndex);
                  const uint32 u32_PrevRxListIndex = static_cast<uint32>(s32_PrevRxListIndex);
                  const uint32 u32_NewTxListIndex = static_cast<uint32>(s32_NewTxListIndex);
                  const uint32 u32_NewRxListIndex = static_cast<uint32>(s32_NewRxListIndex);

                  tgl_assert(u32_PrevTxListIndex < rc_Datapool.c_Lists.size());
                  tgl_assert(u32_PrevRxListIndex < rc_Datapool.c_Lists.size());
                  tgl_assert(u32_NewTxListIndex < rc_Datapool.c_Lists.size());
                  tgl_assert(u32_NewRxListIndex < rc_Datapool.c_Lists.size());

                  if ((((u32_PrevTxListIndex < rc_Datapool.c_Lists.size()) &&
                        (u32_PrevRxListIndex < rc_Datapool.c_Lists.size())) &&
                       (u32_NewTxListIndex < rc_Datapool.c_Lists.size())) &&
                      (u32_NewRxListIndex < rc_Datapool.c_Lists.size()))
                  {
                     C_OSCNodeDataPoolList & rc_PrevTxList = rc_Datapool.c_Lists[u32_PrevTxListIndex];
                     C_OSCNodeDataPoolList & rc_PrevRxList = rc_Datapool.c_Lists[u32_PrevRxListIndex];
                     C_OSCNodeDataPoolList & rc_NewTxList = rc_Datapool.c_Lists[u32_NewTxListIndex];
                     C_OSCNodeDataPoolList & rc_NewRxList = rc_Datapool.c_Lists[u32_NewRxListIndex];
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
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInUiDatapool(const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_PrevInterfaceIndex,
                                                            const uint32 ou32_NewInterfaceIndex)
{
   tgl_assert((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) && (ou32_NodeIndex < this->mc_UINodes.size()));
   if ((ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()) && (ou32_NodeIndex < this->mc_UINodes.size()))
   {
      const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      C_PuiSdNode & rc_UiNode = this->mc_UINodes[ou32_NodeIndex];
      for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         const C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            tgl_assert(rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size());
            tgl_assert(rc_Prot.u32_DataPoolIndex < rc_UiNode.c_UIDataPools.size());
            if ((rc_Prot.u32_DataPoolIndex < rc_UiNode.c_UIDataPools.size()) &&
                (rc_Prot.u32_DataPoolIndex < rc_Node.c_DataPools.size()))
            {
               const C_OSCNodeDataPool & rc_Datapool = rc_Node.c_DataPools[rc_Prot.u32_DataPoolIndex];

               const sint32 s32_PrevTxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_PrevInterfaceIndex,
                                                                                   true);
               const sint32 s32_PrevRxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_PrevInterfaceIndex,
                                                                                   false);
               const sint32 s32_NewTxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                  true);
               const sint32 s32_NewRxListIndex = C_OSCCanProtocol::h_GetListIndex(rc_Datapool, ou32_NewInterfaceIndex,
                                                                                  false);

               tgl_assert(s32_PrevTxListIndex >= 0);
               tgl_assert(s32_PrevRxListIndex >= 0);
               tgl_assert(s32_NewTxListIndex >= 0);
               tgl_assert(s32_NewRxListIndex >= 0);

               if ((((s32_PrevTxListIndex >= 0) && (s32_PrevRxListIndex >= 0)) && (s32_NewTxListIndex >= 0)) &&
                   (s32_NewRxListIndex >= 0))
               {
                  C_PuiSdNodeDataPool & rc_UiDatapool = rc_UiNode.c_UIDataPools[rc_Prot.u32_DataPoolIndex];
                  const uint32 u32_PrevTxListIndex = static_cast<uint32>(s32_PrevTxListIndex);
                  const uint32 u32_PrevRxListIndex = static_cast<uint32>(s32_PrevRxListIndex);
                  const uint32 u32_NewTxListIndex = static_cast<uint32>(s32_NewTxListIndex);
                  const uint32 u32_NewRxListIndex = static_cast<uint32>(s32_NewRxListIndex);

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
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInOscProtocol(const uint32 ou32_NodeIndex, const uint8 ou8_PrevInterface,
                                                             const uint8 ou8_NewInterface)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
      {
         C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
         if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            tgl_assert(static_cast<uint32>(ou8_PrevInterface) < rc_Prot.c_ComMessages.size());
            tgl_assert(static_cast<uint32>(ou8_NewInterface) < rc_Prot.c_ComMessages.size());
            if ((static_cast<uint32>(ou8_PrevInterface) < rc_Prot.c_ComMessages.size()) &&
                (static_cast<uint32>(ou8_NewInterface) < rc_Prot.c_ComMessages.size()))
            {
               C_OSCCanMessageContainer & rc_PrevInterfaceMessages =
                  rc_Prot.c_ComMessages[static_cast<uint32>(ou8_PrevInterface)];
               C_OSCCanMessageContainer & rc_NewInterfaceMessages =
                  rc_Prot.c_ComMessages[static_cast<uint32>(ou8_NewInterface)];
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
void C_PuiSdHandlerCanOpenLogic::m_MoveMessagesInUiProtocol(const uint32 ou32_NodeIndex, const uint8 ou8_PrevInterface,
                                                            const uint8 ou8_NewInterface)
{
   tgl_assert(ou32_NodeIndex < this->mc_UINodes.size());
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if ((ou32_NodeIndex < this->mc_UINodes.size()) && (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size()))
   {
      C_PuiSdNode & rc_UiNode = this->mc_UINodes[ou32_NodeIndex];
      C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
      tgl_assert(rc_UiNode.c_UICanProtocols.size() == rc_Node.c_ComProtocols.size());
      if (rc_UiNode.c_UICanProtocols.size() == rc_Node.c_ComProtocols.size())
      {
         for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_UiNode.c_UICanProtocols.size(); ++u32_ItProt)
         {
            C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
            if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
            {
               C_PuiSdNodeCanProtocol & rc_UiProt = rc_UiNode.c_UICanProtocols[u32_ItProt];
               tgl_assert(static_cast<uint32>(ou8_PrevInterface) < rc_UiProt.c_ComMessages.size());
               tgl_assert(static_cast<uint32>(ou8_NewInterface) < rc_UiProt.c_ComMessages.size());
               if ((static_cast<uint32>(ou8_PrevInterface) < rc_UiProt.c_ComMessages.size()) &&
                   (static_cast<uint32>(ou8_NewInterface) < rc_UiProt.c_ComMessages.size()))
               {
                  C_PuiSdNodeCanMessageContainer & rc_PrevInterfaceMessages =
                     rc_UiProt.c_ComMessages[static_cast<uint32>(ou8_PrevInterface)];
                  C_PuiSdNodeCanMessageContainer & rc_NewInterfaceMessages =
                     rc_UiProt.c_ComMessages[static_cast<uint32>(ou8_NewInterface)];
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeCompleteConnectionForCanOpen(const uint32 ou32_NodeIndex,
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeCompleteConnectionForCanOpenManager(const uint32 ou32_NodeIndex,
                                                                                   const C_PuiSdNodeConnectionId & orc_PrevId,
                                                                                   const C_PuiSdNodeConnectionId & orc_NewId)
{
   if (orc_PrevId.e_InterfaceType == C_OSCSystemBus::eCAN)
   {
      DeleteAllCanOpenManagerDevices(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber);
   }
   if (orc_NewId.e_InterfaceType == C_OSCSystemBus::eCAN)
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
void C_PuiSdHandlerCanOpenLogic::m_HandleChangeCompleteConnectionForCanOpenDevice(const uint32 ou32_NodeIndex,
                                                                                  const C_PuiSdNodeConnectionId & orc_PrevId)
{
   if (orc_PrevId.e_InterfaceType == C_OSCSystemBus::eCAN)
   {
      const C_OSCCanInterfaceId c_Id(ou32_NodeIndex, orc_PrevId.u8_InterfaceNumber);
      for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
      {
         const C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_ItManager =
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
void C_PuiSdHandlerCanOpenLogic::m_SyncOsyNodeIdChange(const uint32 ou32_NodeIndex, const uint32 ou32_InterfaceIndex,
                                                       const uint8 ou8_NewNodeId)
{
   tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
   if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
   {
      const C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];

      tgl_assert(ou32_InterfaceIndex < rc_OSCNode.c_Properties.c_ComInterfaces.size());
      if (ou32_InterfaceIndex < rc_OSCNode.c_Properties.c_ComInterfaces.size())
      {
         if (rc_OSCNode.c_Properties.c_ComInterfaces[ou32_InterfaceIndex].e_InterfaceType == C_OSCSystemBus::eCAN)
         {
            uint8 u8_InterfaceNumber;

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
void C_PuiSdHandlerCanOpenLogic::m_HandleOsyNodeIdChangeForCanOpenManager(const uint32 ou32_NodeIndex,
                                                                          const uint32 ou32_InterfaceIndex,
                                                                          const uint8 ou8_NewNodeId)
{
   uint8 u8_InterfaceNumber;

   if (this->TranslateCanInterfaceIndexToId(ou32_NodeIndex, ou32_InterfaceIndex, u8_InterfaceNumber) == C_NO_ERR)
   {
      tgl_assert(ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size());
      if (ou32_NodeIndex < this->mc_CoreDefinition.c_Nodes.size())
      {
         C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[ou32_NodeIndex];
         const std::map<stw_types::uint8,
                        C_OSCCanOpenManagerInfo>::iterator c_ItCanOpenManager = rc_OSCNode.c_CanOpenManagers.find(
            u8_InterfaceNumber);
         if (c_ItCanOpenManager != rc_OSCNode.c_CanOpenManagers.end())
         {
            //TODO SSI check
            if (c_ItCanOpenManager->second.q_UseOpenSYDENodeID == true)
            {
               c_ItCanOpenManager->second.u8_NodeIDValue = ou8_NewNodeId;
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
void C_PuiSdHandlerCanOpenLogic::m_HandleOsyNodeIdChangeForCanOpenDevice(const uint32 ou32_NodeIndex,
                                                                         const uint32 ou32_InterfaceIndex,
                                                                         const uint8 ou8_NewNodeId)
{
   uint8 u8_InterfaceNumber;

   if (this->TranslateCanInterfaceIndexToId(ou32_NodeIndex, ou32_InterfaceIndex, u8_InterfaceNumber) == C_NO_ERR)
   {
      const C_OSCCanInterfaceId c_DeviceId(ou32_NodeIndex, u8_InterfaceNumber);
      for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
      {
         C_OSCNode & rc_OSCNode = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (std::map<stw_types::uint8,
                       C_OSCCanOpenManagerInfo>::iterator c_ItCanOpenManager = rc_OSCNode.c_CanOpenManagers.begin();
              c_ItCanOpenManager != rc_OSCNode.c_CanOpenManagers.end(); ++c_ItCanOpenManager)
         {
            const std::map<C_OSCCanInterfaceId,
                           C_OSCCanOpenManagerDeviceInfo>::iterator c_ItCanOpenDevice =
               c_ItCanOpenManager->second.c_CanOpenDevices.find(c_DeviceId);
            if (c_ItCanOpenDevice != c_ItCanOpenManager->second.c_CanOpenDevices.end())
            {
               //TODO SSI check
               if (c_ItCanOpenDevice->second.q_UseOpenSYDENodeID == true)
               {
                  c_ItCanOpenDevice->second.u8_NodeIDValue = ou8_NewNodeId;
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
void C_PuiSdHandlerCanOpenLogic::m_HandleNodeIdChangeForCanOpenMessages(const uint32 ou32_NodeIndex,
                                                                        const uint8 ou8_InterfaceNumber)
{
   for (uint32 u32_ItNode = 0UL; u32_ItNode < this->mc_CoreDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      const C_OSCCanInterfaceId c_DeviceId(ou32_NodeIndex, ou8_InterfaceNumber);
      const C_OSCCanOpenManagerDeviceInfo * const pc_FoundDevice = this->GetCanOpenManagerDeviceForAnyManager(
         u32_ItNode,
         c_DeviceId);
      if (pc_FoundDevice != NULL)
      {
         C_OSCNode & rc_Node = this->mc_CoreDefinition.c_Nodes[u32_ItNode];
         for (uint32 u32_ItProt = 0UL; u32_ItProt < rc_Node.c_ComProtocols.size(); ++u32_ItProt)
         {
            C_OSCCanProtocol & rc_Prot = rc_Node.c_ComProtocols[u32_ItProt];
            if (rc_Prot.e_Type == C_OSCCanProtocol::eCAN_OPEN)
            {
               for (uint32 u32_ItContainer = 0UL; u32_ItContainer < rc_Prot.c_ComMessages.size(); ++u32_ItContainer)
               {
                  C_OSCCanMessageContainer & rc_PrevInterfaceMessages =
                     rc_Prot.c_ComMessages[u32_ItContainer];
                  C_PuiSdHandlerCanOpenLogic::mh_HandleNodeIdChangeForCanOpenMessages(c_DeviceId,
                                                                                      pc_FoundDevice->u8_NodeIDValue,
                                                                                      rc_PrevInterfaceMessages.c_RxMessages);
                  C_PuiSdHandlerCanOpenLogic::mh_HandleNodeIdChangeForCanOpenMessages(c_DeviceId,
                                                                                      pc_FoundDevice->u8_NodeIDValue,
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
void C_PuiSdHandlerCanOpenLogic::mh_HandleNodeIdChangeForCanOpenMessages(const C_OSCCanInterfaceId & orc_DeviceId,
                                                                         const uint8 ou8_NewNodeId,
                                                                         std::vector<C_OSCCanMessage> & orc_Messages)
{
   for (uint32 u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      C_OSCCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      if (rc_Message.c_CanOpenManagerOwnerNodeIndex == orc_DeviceId)
      {
         if (rc_Message.q_CanOpenManagerCobIdIncludesNodeID)
         {
            rc_Message.u32_CanId = rc_Message.u32_CanOpenManagerCobIdOffset + ou8_NewNodeId;
         }
      }
   }
}
