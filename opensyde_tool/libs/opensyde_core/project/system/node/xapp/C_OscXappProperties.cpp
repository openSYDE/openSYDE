//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       X-app properties

   X-app properties

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscXappProperties.hpp"

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
C_OscXappProperties::C_OscXappProperties() :
   u32_PollingIntervalMs(10U),
   u32_DataRequestIntervalMs(100U),
   e_ConnectedInterfaceType(C_OscSystemBus::eCAN),
   u8_ConnectedInterfaceNumber(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXappProperties::Initialize()
{
   u32_PollingIntervalMs = 10U;
   u32_DataRequestIntervalMs = 100U;
   e_ConnectedInterfaceType = C_OscSystemBus::eCAN;
   u8_ConnectedInterfaceNumber = 0U;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscXappProperties::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_PollingIntervalMs,
                                       sizeof(this->u32_PollingIntervalMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_DataRequestIntervalMs,
                                       sizeof(this->u32_DataRequestIntervalMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_ConnectedInterfaceType,
                                       sizeof(this->e_ConnectedInterfaceType),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_ConnectedInterfaceNumber,
                                       sizeof(this->u8_ConnectedInterfaceNumber),
                                       oru32_HashValue);
}
