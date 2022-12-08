//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager general data information
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENMANAGERINFO_H
#define C_OSCCANOPENMANAGERINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "stwtypes.hpp"
#include "C_OscCanInterfaceId.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenManagerInfo
{
public:
   C_OscCanOpenManagerInfo(void);

   enum E_NmtErrorBehaviourType ///< type of NMT error behavior
   {
      eRESTART_ALL_DEVICES,    ///< Trigger restart all CANopen Devices on error
      eRESTART_FAILURE_DEVICE, ///< Trigger restart failure CANopen Device on error
      eSTOP_ALL_DEVICES        ///< Trigger stop all CANopen Devices on error
   };

   void CalcHash(uint32_t & oru32_HashValue) const;
   void CheckErrorManager(bool * const opq_CoNodeIdConflict, bool * const opq_CoManagerNodeIdInvalid,
                          bool * const opq_CoDevicesNodeIdInvalid, bool * const opq_HearbeatTimeInvalid,
                          const bool oq_CheckDeviceToDeviceErrors) const;
   int32_t CheckErrorDeviceCoNodeId(const C_OscCanInterfaceId & orc_DeviceId, bool * const opq_CoNodeIdConflict,
                                    bool * const opq_CoNodeIdInvalid, const bool oq_CheckDeviceToDeviceErrors) const;
   int32_t CheckErrorDeviceHeartbeat(const C_OscCanInterfaceId & orc_DeviceId,
                                     bool * const opq_HearbeatTimeInvalid) const;

   bool q_UseOpenSydeNodeId;        ///< Use openSYDE node ID instead of CANopen node ID
   uint8_t u8_NodeIdValue;          ///< Value of CANopen node ID, valid range 1-127
   uint16_t u16_GlobalSdoTimeoutMs; ///< Timeout for any SDO access from manger to any device
   bool q_AutostartCanOpenManager;  ///< Autostart CANopen manager as soon as all required devices are online
   bool q_StartDevices;             ///< Setting if CANopen manager starts devices, or each own device application
                                    // handles start
   bool q_NmtStartAll;              ///< Only if option start devices active: Starting devices using "NMT Start All"
                                    // command

   E_NmtErrorBehaviourType e_NmtErrorBehaviour; ///< Error handling selection for NMT
   bool q_EnableHeartbeatProducing;             ///< Enable CANopen heartbeat
   uint16_t u16_HeartbeatProducerTimeMs;        ///< Enable heartbeat frequency in milliseconds

   bool q_ProduceSyncMessage;       ///< Enable CANopen produce SYNC message
   uint32_t u32_SyncCyclePeriodUs;  ///< Setting for SYNC message cycle time in us
   uint32_t u32_SyncWindowLengthUs; ///< Setting for SYNC message window length in us

   std::map<C_OscCanInterfaceId, C_OscCanOpenManagerDeviceInfo> c_CanOpenDevices; ///< CANopen devices assigned to the
                                                                                  // CANopen manager, grouped by node
                                                                                  // index and interface ID
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
