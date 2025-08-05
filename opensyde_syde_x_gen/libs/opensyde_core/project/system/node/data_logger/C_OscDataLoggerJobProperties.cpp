//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job properties

   Data logger job properties

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscDataLoggerJobProperties.hpp"

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
C_OscDataLoggerJobProperties::C_OscDataLoggerJobProperties() :
   c_Name("LogJob"),
   e_UseCase(eUC_MANUAL),
   e_LogFileFormat(eLFF_CSV),
   u32_MaxLogEntries(1000),
   u32_MaxLogDurationSec(60),
   u32_LogIntervalMs(1000),
   e_LocalLogTrigger(eLLT_ON_CHANGE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobProperties::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_UseCase,
                                       sizeof(this->e_UseCase),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_LogFileFormat,
                                       sizeof(this->e_LogFileFormat),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_MaxLogEntries,
                                       sizeof(this->u32_MaxLogEntries),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_MaxLogDurationSec,
                                       sizeof(this->u32_MaxLogDurationSec),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_LogIntervalMs,
                                       sizeof(this->u32_LogIntervalMs),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_LocalLogTrigger,
                                       sizeof(this->e_LocalLogTrigger),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_LogDestinationDirectory.c_str(),
                                       this->c_LogDestinationDirectory.Length(), oru32_HashValue);
   c_AdditionalTriggerProperties.CalcHash(oru32_HashValue);
}
