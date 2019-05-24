//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for box based data: stores UI information (implementation)

   UI base class for box based data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiBsBox.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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
C_PuiBsBox::C_PuiBsBox() :
   c_UIPosition(0.0, 0.0),
   f64_Width(0.0),
   f64_Height(0.0),
   f64_ZOrder(0.0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiBsBox::~C_PuiBsBox(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsBox::CalcHash(uint32 & oru32_HashValue) const
{
   float64 f64_Value;

   f64_Value = this->c_UIPosition.x();
   stw_scl::C_SCLChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);
   f64_Value = this->c_UIPosition.y();
   stw_scl::C_SCLChecksums::CalcCRC32(&f64_Value, sizeof(f64_Value), oru32_HashValue);

   stw_scl::C_SCLChecksums::CalcCRC32(&this->f64_Width, sizeof(this->f64_Width), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->f64_Height, sizeof(this->f64_Height), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->f64_ZOrder, sizeof(this->f64_ZOrder), oru32_HashValue);
}
