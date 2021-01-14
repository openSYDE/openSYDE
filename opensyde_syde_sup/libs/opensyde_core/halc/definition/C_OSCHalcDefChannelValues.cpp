//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group channel values

   Group channel values

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCHalcDefChannelValues.h"

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
/*! \brief  Default constructor/destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcDefChannelValues::C_OSCHalcDefChannelValues(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcDefChannelValues::~C_OSCHalcDefChannelValues()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcDefChannelValues::CalcHash(uint32 & oru32_HashValue) const
{
   for (uint32 u32_It = 0UL; u32_It < this->c_Parameters.size(); ++u32_It)
   {
      this->c_Parameters[u32_It].CalcHash(oru32_HashValue);
   }
   for (uint32 u32_It = 0UL; u32_It < this->c_InputValues.size(); ++u32_It)
   {
      this->c_InputValues[u32_It].CalcHash(oru32_HashValue);
   }
   for (uint32 u32_It = 0UL; u32_It < this->c_OutputValues.size(); ++u32_It)
   {
      this->c_OutputValues[u32_It].CalcHash(oru32_HashValue);
   }
   for (uint32 u32_It = 0UL; u32_It < this->c_StatusValues.size(); ++u32_It)
   {
      this->c_StatusValues[u32_It].CalcHash(oru32_HashValue);
   }
}
