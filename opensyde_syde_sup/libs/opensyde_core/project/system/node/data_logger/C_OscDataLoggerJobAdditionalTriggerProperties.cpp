//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job additional trigger properties

   Data logger job additional trigger properties

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscDataLoggerJobAdditionalTriggerProperties.hpp"

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
C_OscDataLoggerJobAdditionalTriggerProperties::C_OscDataLoggerJobAdditionalTriggerProperties() :
   q_Enable(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobAdditionalTriggerProperties::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Enable,
                                       sizeof(this->q_Enable),
                                       oru32_HashValue);
   this->c_ElementId.CalcHash(oru32_HashValue);
   this->c_Threshold.CalcHash(oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(
      this->c_Operation.c_str(),
      this->c_Operation.Length(), oru32_HashValue);
}
