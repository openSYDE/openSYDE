//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager device specific data information
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENMANAGERDEVICEINFO_H
#define C_OSCCANOPENMANAGERDEVICEINFO_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "C_OSCCanOpenManagerMappableSignal.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenManagerDeviceInfo
{
public:
   C_OSCCanOpenManagerDeviceInfo();

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_scl::C_SCLString c_EDSFileName;            ///< File name of loaded EDS file
   C_OSCCanOpenObjectDictionary c_EDSFileContent; ///< Content of loaded EDS file, parsed for
   ///< GUI usage
   std::vector<C_OSCCanOpenManagerMappableSignal> c_EDSFileMappableSignals; ///< All parsed and valid mappable signals
   bool q_DeviceOptional;                                                   ///< Flag for CANopen manager to consider
   ///< this device
   ///< optional
   bool q_NoInitialization; ///< Flag for CANopen manager to skip initialization for this
   ///< device
   bool q_FactorySettingsActive; ///< Reset device to default communication settings before
   ///< starting configuration
   stw_types::uint8 u8_ResetNodeObjectDictionarySubIndex; ///< Selected object dictionary sub index for reset node
   bool q_EnableHeartbeatProducing;                       ///< Enable CANopen heartbeat producing
   stw_types::uint16 u16_HeartbeatProducerTimeMs;         ///< Heartbeat producing frequency in milliseconds
   bool q_UseOpenSYDENodeID;                              ///< Use openSYDE node ID instead of CANopen node ID
   stw_types::uint8 u8_NodeIDValue;                       ///< Value of CANopen node ID, valid range 1-127
   bool q_EnableHeartbeatConsuming;                       ///< Enable CANopen heartbeat consuming
   stw_types::uint16 u16_HeartbeatConsumerTimeMs;         ///< Heartbeat consumer frequency in milliseconds
   bool q_EnableHeartbeatConsumingAutoCalculation;        ///< Enable CANopen heartbeat consuming automatic calculation
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
