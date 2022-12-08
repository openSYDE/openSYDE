//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition domain

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscHalcDefDomain.hpp"

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
C_OscHalcDefDomain::C_OscHalcDefDomain(void) :
   e_Category(C_OscHalcDefDomain::eCA_OTHER)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcDefDomain::~C_OscHalcDefDomain(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcDefDomain::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Id.c_str(), this->c_Id.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_SingularName.c_str(), this->c_SingularName.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Category, sizeof(this->e_Category), oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->c_Channels.size(); ++u32_It)
   {
      this->c_Channels[u32_It].CalcHash(oru32_HashValue);
   }

   for (uint32_t u32_It = 0UL; u32_It < this->c_ChannelUseCases.size(); ++u32_It)
   {
      this->c_ChannelUseCases[u32_It].CalcHash(oru32_HashValue);
   }

   this->c_DomainValues.CalcHash(oru32_HashValue);
   this->c_ChannelValues.CalcHash(oru32_HashValue);
}
