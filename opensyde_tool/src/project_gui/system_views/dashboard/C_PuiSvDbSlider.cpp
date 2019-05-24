//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard slider data (implementation)

   UI class for dashboard slider data

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSvDbSlider.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_logic;

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
C_PuiSvDbSlider::C_PuiSvDbSlider(void) :
   e_Type(C_PuiSvDbSlider::eTYPE_SMALL_COLOR_1),
   q_ShowMinMax(true),
   s32_Value(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbSlider::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_ShowMinMax, sizeof(this->q_ShowMinMax), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->s32_Value, sizeof(this->s32_Value), oru32_HashValue);
   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if item is a read element

   \return
   True  Read element
   False Write element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDbSlider::IsReadElement(void) const
{
   return false;
}
