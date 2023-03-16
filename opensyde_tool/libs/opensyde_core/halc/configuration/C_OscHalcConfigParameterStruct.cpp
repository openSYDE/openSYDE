//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter struct config data for HALC configuration

   Group domain parameter struct config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscHalcConfigParameterStruct.hpp"

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
C_OscHalcConfigParameterStruct::C_OscHalcConfigParameterStruct(void) :
   C_OscHalcConfigParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfigParameterStruct::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscHalcConfigParameter::CalcHash(oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->c_ParameterElements.size(); ++u32_It)
   {
      this->c_ParameterElements[u32_It].CalcHash(oru32_HashValue);
   }
}
