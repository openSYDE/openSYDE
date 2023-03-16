//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition domain channel use-case

   HALC definition domain channel use-case

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscHalcDefChannelUseCase.hpp"

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
C_OscHalcDefChannelUseCase::C_OscHalcDefChannelUseCase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefChannelUseCase::~C_OscHalcDefChannelUseCase()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefChannelUseCase::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Id.c_str(), this->c_Id.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Display.c_str(), this->c_Display.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);

   c_Value.CalcHash(oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->c_Availability.size(); ++u32_It)
   {
      this->c_Availability[u32_It].CalcHash(oru32_HashValue);
   }

   for (uint32_t u32_It = 0UL; u32_It < this->c_DefaultChannels.size(); ++u32_It)
   {
      stw::scl::C_SclChecksums::CalcCRC32(&this->c_DefaultChannels[u32_It], sizeof(uint32_t),
                                          oru32_HashValue);
   }
}
