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
#include "stwtypes.hpp"
#include "C_OscCanOpenObjectDictionary.hpp"
#include "C_OscCanOpenManagerMappableSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenManagerDeviceInfo
{
public:
   C_OscCanOpenManagerDeviceInfo();

   void CalcHash(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_EdsFileName;           ///< File name of loaded EDS file
   C_OscCanOpenObjectDictionary c_EdsFileContent; ///< Content of loaded EDS file, parsed for
   ///< GUI usage
   std::vector<C_OscCanOpenManagerMappableSignal> c_EdsFileMappableSignals; ///< All parsed and valid mappable signals
   bool q_DeviceOptional;                                                   ///< Flag for CANopen manager to consider
   ///< this device
   ///< optional
   bool q_NoInitialization;                        ///< Flag for CANopen manager to skip initialization for this device
   bool q_FactorySettingsActive;                   ///< Reset device to default communication settings before starting
                                                   // configuration
   uint8_t u8_ResetNodeObjectDictionarySubIndex;   ///< Selected object dictionary sub index for reset node
   bool q_EnableHeartbeatProducing;                ///< Enable CANopen heartbeat producing
   uint16_t u16_HeartbeatProducerTimeMs;           ///< Heartbeat producing frequency in milliseconds
   bool q_UseOpenSydeNodeId;                       ///< Use openSYDE node ID instead of CANopen node ID
   uint8_t u8_NodeIdValue;                         ///< Value of CANopen node ID, valid range 1-127
   bool q_EnableHeartbeatConsuming;                ///< Enable CANopen heartbeat consuming
   uint16_t u16_HeartbeatConsumerTimeMs;           ///< Heartbeat consumer frequency in milliseconds
   bool q_EnableHeartbeatConsumingAutoCalculation; ///< Enable CANopen heartbeat consuming automatic calculation
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
