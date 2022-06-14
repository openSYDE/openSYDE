//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CANopen manager general data information

   Data class for CANopen manager general data information

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_OSCCanOpenManagerInfo.h"

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
C_OSCCanOpenManagerInfo::C_OSCCanOpenManagerInfo(void) :
   q_UseOpenSYDENodeID(true),
   u8_NodeIDValue(0),
   u16_GlobalSDOTimeoutMs(100),
   q_AutostartCanOpenManager(true),
   q_StartDevices(true),
   q_NMTStartAll(false),
   e_NMTErrorBehaviour(eRESTART_ALL_DEVICES),
   q_EnableHeartbeatProducing(true),
   u16_HeartbeatProducerTimeMs(100)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenManagerInfo::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_UseOpenSYDENodeID, sizeof(this->q_UseOpenSYDENodeID),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_NodeIDValue, sizeof(this->u8_NodeIDValue),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_GlobalSDOTimeoutMs, sizeof(this->u16_GlobalSDOTimeoutMs),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_AutostartCanOpenManager, sizeof(this->q_AutostartCanOpenManager),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_StartDevices, sizeof(this->q_StartDevices),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_NMTStartAll, sizeof(this->q_NMTStartAll),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_NMTErrorBehaviour, sizeof(this->e_NMTErrorBehaviour),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_EnableHeartbeatProducing, sizeof(this->q_EnableHeartbeatProducing),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_HeartbeatProducerTimeMs, sizeof(this->u16_HeartbeatProducerTimeMs),
                                      oru32_HashValue);
   for (std::map<C_OSCCanInterfaceId, C_OSCCanOpenManagerDeviceInfo>::const_iterator c_It =
           this->c_CanOpenDevices.begin();
        c_It != this->c_CanOpenDevices.end(); ++c_It)
   {
      c_It->first.CalcHash(oru32_HashValue);
      c_It->second.CalcHash(oru32_HashValue);
   }
}
