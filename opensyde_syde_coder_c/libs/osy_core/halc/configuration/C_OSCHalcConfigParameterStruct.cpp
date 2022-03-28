//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter struct config data for HALC configuration

   Group domain parameter struct config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCHalcConfigParameterStruct.h"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigParameterStruct::C_OSCHalcConfigParameterStruct(void) :
   C_OSCHalcConfigParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigParameterStruct::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   C_OSCHalcConfigParameter::CalcHash(oru32_HashValue);

   for (uint32 u32_It = 0UL; u32_It < this->c_ParameterElements.size(); ++u32_It)
   {
      this->c_ParameterElements[u32_It].CalcHash(oru32_HashValue);
   }
}
