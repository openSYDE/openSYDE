//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain channel config data for HALC configuration

   Group domain channel config data for HALC configuration

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscUtils.hpp"
#include "C_OscHalcConfigChannel.hpp"

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
C_OscHalcConfigChannel::C_OscHalcConfigChannel(void) :
   q_SafetyRelevant(false),
   u32_UseCaseIndex(0UL)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcConfigChannel::~C_OscHalcConfigChannel()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfigChannel::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_SafetyRelevant, sizeof(this->q_SafetyRelevant), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_UseCaseIndex, sizeof(this->u32_UseCaseIndex), oru32_HashValue);

   for (uint32_t u32_It = 0UL; u32_It < this->c_Parameters.size(); ++u32_It)
   {
      this->c_Parameters[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check config valid

   \param[out]  opq_NameInvalid  Name invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfigChannel::CheckConfigValid(bool * const opq_NameInvalid) const
{
   if (opq_NameInvalid != NULL)
   {
      if (C_OscUtils::h_CheckValidCeName(this->c_Name) == false)
      {
         *opq_NameInvalid = true;
      }
      else
      {
         *opq_NameInvalid = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle file load post processing

   \param[in]  oe_SafetyMode  Safety mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfigChannel::HandleFileLoadPostProcessing(const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode)
{
   if (oe_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_SAFE)
   {
      this->q_SafetyRelevant = true;
   }
   else if (oe_SafetyMode == C_OscHalcDefBase::eONE_LEVEL_ALL_NON_SAFE)
   {
      this->q_SafetyRelevant = false;
   }
   else
   {
      //Nothing to do
   }
}
