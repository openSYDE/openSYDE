//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for communication interface settings (implementation)

   Data class for communication interface settings

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstring>
#include "C_OSCNodeComInterfaceSettings.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeComInterfaceSettings::C_OSCNodeComInterfaceSettings(void) :
   e_InterfaceType(C_OSCSystemBus::eCAN),
   u8_InterfaceNumber(0),
   u8_NodeID(0),
   q_IsUpdateEnabled(false),
   q_IsRoutingEnabled(false),
   q_IsDiagnosisEnabled(false),
   q_IsBusConnected(false),
   u32_BusIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeComInterfaceSettings::~C_OSCNodeComInterfaceSettings(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeComInterfaceSettings::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_InterfaceType, sizeof(this->e_InterfaceType), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_InterfaceNumber, sizeof(this->u8_InterfaceNumber), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_NodeID, sizeof(this->u8_NodeID), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->c_Ip.au8_IpAddress[0], sizeof(this->c_Ip.au8_IpAddress), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->c_Ip.au8_NetMask[0], sizeof(this->c_Ip.au8_NetMask), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->c_Ip.au8_DefaultGateway[0], sizeof(this->c_Ip.au8_DefaultGateway),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsUpdateEnabled, sizeof(this->q_IsUpdateEnabled), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsRoutingEnabled, sizeof(this->q_IsRoutingEnabled), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsDiagnosisEnabled, sizeof(this->q_IsDiagnosisEnabled), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsBusConnected, sizeof(this->q_IsBusConnected), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_BusIndex, sizeof(this->u32_BusIndex), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new connection

   \param[in] oru32_BusIndex Bus index to add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeComInterfaceSettings::AddConnection(const uint32 & oru32_BusIndex)
{
   this->u32_BusIndex = oru32_BusIndex;
   this->q_IsBusConnected = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear existing connection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeComInterfaceSettings::RemoveConnection(void)
{
   this->u32_BusIndex = 0;
   this->q_IsBusConnected = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Set defaults
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCNodeComInterfaceSettings::C_IpAddress::C_IpAddress(void)
{
   (void)std::memset(&au8_IpAddress[0], 0, sizeof(au8_IpAddress));
   (void)std::memset(&au8_NetMask[0], 255, sizeof(au8_NetMask));
   au8_NetMask[3] = 0;
   (void)std::memset(&au8_DefaultGateway[0], 0, sizeof(au8_DefaultGateway));
}
