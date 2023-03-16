//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition generic struct part

   HALC definition generic struct part

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscHalcDefStruct.hpp"

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
C_OscHalcDefStruct::C_OscHalcDefStruct(void) :
   C_OscHalcDefElement()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefStruct::~C_OscHalcDefStruct(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefStruct::CalcHash(uint32_t & oru32_HashValue) const
{
   C_OscHalcDefElement::CalcHash(oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->c_StructElements.size(); ++u32_It)
   {
      this->c_StructElements[u32_It].CalcHash(oru32_HashValue);
   }
}
