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

#include "stwtypes.h"
#include "C_OSCCanInterfaceId.h"
#include "C_OSCCanOpenManagerDeviceInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenManagerInfo
{
public:
   C_OSCCanOpenManagerInfo(void);

   enum E_NMTErrorBehaviourType ///< type of NMT error behaviour
   {
      eRESTART_ALL_DEVICES,    ///< Trigger restart all CANopen Devices on error
      eRESTART_FAILURE_DEVICE, ///< Trigger restart failure CANopen Device on error
      eSTOP_ALL_DEVICES        ///< Trigger stop all CANopen Devices on error
   };

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   bool q_UseOpenSYDENodeID;                 ///< Use openSYDE node ID instead of CANopen node ID
   stw_types::uint8 u8_NodeIDValue;          ///< Value of CANopen node ID, valid range 1-127
   stw_types::uint16 u16_GlobalSDOTimeoutMs; ///< Timeout for any SDO access from manger to any device
   bool q_AutostartCanOpenManager;           ///< Autostart CANopen manager as soon as all required devices are
   ///< online
   bool q_StartDevices; ///< Setting if CANopen manager starts devices, or each own device
   ///< application handles start
   bool q_NMTStartAll; ///< Only if option start devices active: Starting devices using "NMT
   ///< Start All" command
   E_NMTErrorBehaviourType e_NMTErrorBehaviour;   ///< Error handling selection for NMT
   bool q_EnableHeartbeatProducing;               ///< Enable CANopen heartbeat
   stw_types::uint16 u16_HeartbeatProducerTimeMs; ///< Enable heartbeat frequency in
   ///< milliseconds
   std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo> c_CanOpenDevices; ///< CANopen devices assigned to the
                                                                                  // CANopen manager, grouped by node
                                                                                  // index and interface ID
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
