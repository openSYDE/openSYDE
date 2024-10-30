//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job information

   Data logger job information

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscDataLoggerJob.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
bool C_OscDataLoggerJob::hq_AllowDataloggerFeature = false;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDataLoggerJob::C_OscDataLoggerJob() :
   q_IsEnabled(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJob::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   stw::scl::C_SclChecksums::CalcCRC32(&this->q_IsEnabled, sizeof(this->q_IsEnabled), oru32_HashValue);

   this->c_Properties.CalcHash(oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_ConfiguredDataElements.size(); ++u32_Counter)
   {
      this->c_ConfiguredDataElements[u32_Counter].CalcHash(oru32_HashValue);
   }
}
