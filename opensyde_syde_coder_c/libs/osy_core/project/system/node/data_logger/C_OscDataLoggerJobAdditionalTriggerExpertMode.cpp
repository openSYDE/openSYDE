//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data logger job additional trigger expert mode properties

   Data logger job additional trigger expert mode properties

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscDataLoggerJobAdditionalTriggerExpertMode.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
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
C_OscDataLoggerJobAdditionalTriggerExpertMode::C_OscDataLoggerJobAdditionalTriggerExpertMode() :
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
void C_OscDataLoggerJobAdditionalTriggerExpertMode::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Enable,
                                       sizeof(this->q_Enable),
                                       oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(
      this->c_TriggerConfiguration.c_str(),
      this->c_TriggerConfiguration.Length(), oru32_HashValue);
   for (uint32_t u32_It = 0UL; u32_It < this->c_TriggerDataElementIds.size(); ++u32_It)
   {
      this->c_TriggerDataElementIds[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Replace ui var names

   \param[in,out]  orc_Expression   Expression
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDataLoggerJobAdditionalTriggerExpertMode::h_ReplaceUiVarNames(C_SclString & orc_Expression)
{
   orc_Expression.ReplaceAll("::", ".");
   orc_Expression.ReplaceAll("[", ".");
   orc_Expression.ReplaceAll("]", "");
}
