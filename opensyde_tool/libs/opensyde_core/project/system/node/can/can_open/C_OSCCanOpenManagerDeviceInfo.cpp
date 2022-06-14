//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager device specific data information

   Data class for CANopen manager device specific data information

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_OSCCanOpenManagerDeviceInfo.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;

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
C_OSCCanOpenManagerDeviceInfo::C_OSCCanOpenManagerDeviceInfo() :
   q_DeviceOptional(false),
   q_NoInitialization(false),
   q_FactorySettingsActive(true),
   u8_ResetNodeObjectDictionarySubIndex(0),
   q_EnableHeartbeatProducing(true),
   u16_HeartbeatProducerTimeMs(100),
   q_UseOpenSYDENodeID(true),
   u8_NodeIDValue(0),
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
void C_OSCCanOpenManagerDeviceInfo::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_EDSFileName.c_str(), this->c_EDSFileName.Length(), oru32_HashValue);
   this->c_EDSFileContent.CalcHash(oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_DeviceOptional, sizeof(this->q_DeviceOptional),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_NoInitialization, sizeof(this->q_NoInitialization),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_FactorySettingsActive, sizeof(this->q_FactorySettingsActive),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_ResetNodeObjectDictionarySubIndex,
                                      sizeof(this->u8_ResetNodeObjectDictionarySubIndex),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_EnableHeartbeatProducing, sizeof(this->q_EnableHeartbeatProducing),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_HeartbeatProducerTimeMs, sizeof(this->u16_HeartbeatProducerTimeMs),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_UseOpenSYDENodeID, sizeof(this->q_UseOpenSYDENodeID),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_NodeIDValue, sizeof(this->u8_NodeIDValue),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_EnableHeartbeatConsuming, sizeof(this->q_EnableHeartbeatConsuming),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_HeartbeatConsumerTimeMs, sizeof(this->u16_HeartbeatConsumerTimeMs),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_EnableHeartbeatConsumingAutoCalculation,
                                      sizeof(this->q_EnableHeartbeatConsumingAutoCalculation),
                                      oru32_HashValue);
}
