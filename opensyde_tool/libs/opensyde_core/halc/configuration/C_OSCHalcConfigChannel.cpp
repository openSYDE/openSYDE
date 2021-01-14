//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain channel config data for HALC configuration

   Group domain channel config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_OSCUtils.h"
#include "C_OSCHalcConfigChannel.h"

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
C_OSCHalcConfigChannel::C_OSCHalcConfigChannel(void) :
   q_SafetyRelevant(false),
   u32_UseCaseIndex(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigChannel::~C_OSCHalcConfigChannel()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigChannel::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_SafetyRelevant, sizeof(this->q_SafetyRelevant), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_UseCaseIndex, sizeof(this->u32_UseCaseIndex), oru32_HashValue);

   for (uint32 u32_It = 0UL; u32_It < this->c_Parameters.size(); ++u32_It)
   {
      this->c_Parameters[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check config valid

   \param[out]  opq_NameInvalid  Name invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCHalcConfigChannel::CheckConfigValid(bool * const opq_NameInvalid) const
{
   if (opq_NameInvalid != NULL)
   {
      if (C_OSCUtils::h_CheckValidCName(this->c_Name) == false)
      {
         *opq_NameInvalid = true;
      }
      else
      {
         *opq_NameInvalid = false;
      }
   }
}
