//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager device specific data information

   Data class for CANopen manager device specific data information

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscCanOpenManagerDeviceInfo.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanOpenManagerDeviceInfo::C_OscCanOpenManagerDeviceInfo() :
   q_DeviceOptional(false),
   q_NoInitialization(false),
   q_FactorySettingsActive(false),
   u8_ResetNodeObjectDictionarySubIndex(0),
   q_EnableHeartbeatProducing(true),
   u16_HeartbeatProducerTimeMs(100),
   q_UseOpenSydeNodeId(true),
   u8_NodeIdValue(0),
   q_EnableHeartbeatConsuming(true),
   u16_HeartbeatConsumerTimeMs(100),
   q_EnableHeartbeatConsumingAutoCalculation(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenManagerDeviceInfo::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_EdsFileName.c_str(), this->c_EdsFileName.Length(), oru32_HashValue);
   this->c_EdsFileContent.CalcHash(oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_DeviceOptional, sizeof(this->q_DeviceOptional),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_NoInitialization, sizeof(this->q_NoInitialization),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_FactorySettingsActive, sizeof(this->q_FactorySettingsActive),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_ResetNodeObjectDictionarySubIndex,
                                       sizeof(this->u8_ResetNodeObjectDictionarySubIndex),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_EnableHeartbeatProducing, sizeof(this->q_EnableHeartbeatProducing),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_HeartbeatProducerTimeMs, sizeof(this->u16_HeartbeatProducerTimeMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_UseOpenSydeNodeId, sizeof(this->q_UseOpenSydeNodeId),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_NodeIdValue, sizeof(this->u8_NodeIdValue),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_EnableHeartbeatConsuming, sizeof(this->q_EnableHeartbeatConsuming),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_HeartbeatConsumerTimeMs, sizeof(this->u16_HeartbeatConsumerTimeMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_EnableHeartbeatConsumingAutoCalculation,
                                       sizeof(this->q_EnableHeartbeatConsumingAutoCalculation),
                                       oru32_HashValue);
}
