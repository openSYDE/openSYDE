//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data element scaling information (implementation)

   Data class for data element scaling information

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_PuiSvDbDataElementScaling.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementScaling::C_PuiSvDbDataElementScaling(void) :
   q_UseDefault(true),
   f64_Factor(1.0),
   f64_Offset(0.0),
   c_Unit("")
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   destructor

   clean up ...
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementScaling::~C_PuiSvDbDataElementScaling(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementScaling::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_UseDefault, sizeof(this->q_UseDefault), oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Factor, sizeof(this->f64_Factor), oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Offset, sizeof(this->f64_Offset), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Unit.toStdString().c_str(), this->c_Unit.length(), oru32_HashValue);
}
