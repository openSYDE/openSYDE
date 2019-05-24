//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI image data: stores UI information (implementation)

   UI image data: stores UI information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_PuiBsImage.h"

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
C_PuiBsImage::C_PuiBsImage() :
   C_PuiBsBox()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiBsImage::CalcHash(uint32 & oru32_HashValue) const
{
   // No calculation of c_UIImagePixmap necessary. Image can not be changed after creating it and
   // size is checked by base class

   C_PuiBsBox::CalcHash(oru32_HashValue);
}
