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
   e_LogFileFormat(eLFF_CSV),
   u32_MaxLogFileSizeMb(500),
   e_LocalLogTrigger(eLLT_ON_CHANGE),
   e_ConnectedInterfaceType(C_OscSystemBus::eCAN),
   u8_ConnectedInterfaceNumber(0U)
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
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_LogFileFormat,
                                       sizeof(this->e_LogFileFormat),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_MaxLogFileSizeMb,
                                       sizeof(this->u32_MaxLogFileSizeMb),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_LocalLogTrigger,
                                       sizeof(this->e_LocalLogTrigger),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_ConnectedInterfaceType,
                                       sizeof(this->e_ConnectedInterfaceType),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_ConnectedInterfaceNumber,
                                       sizeof(this->u8_ConnectedInterfaceNumber),
                                       oru32_HashValue);
}
