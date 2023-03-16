//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard progress bar data (implementation)

   UI class for dashboard progress bar data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SclChecksums.hpp"
#include "C_PuiSvDbProgressBar.hpp"

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
C_PuiSvDbProgressBar::C_PuiSvDbProgressBar() :
   C_PuiSvDbWidgetBase(),
   e_Type(eTYPE_1),
   e_Alignment(eTOP),
   q_ShowMinMax(true),
   q_ShowUnit(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbProgressBar::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Alignment, sizeof(this->e_Alignment), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_ShowMinMax, sizeof(this->q_ShowMinMax), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_ShowUnit, sizeof(this->q_ShowUnit), oru32_HashValue);
   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item is a read element

   \return
   True  Read element
   False Write element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbProgressBar::IsReadElement(void) const
{
   return true;
}
