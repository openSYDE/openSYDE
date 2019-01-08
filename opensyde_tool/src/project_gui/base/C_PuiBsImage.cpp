//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI image data: stores UI information (implementation)

   UI image data: stores UI information

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiBsImage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     27.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiBsImage::C_PuiBsImage() :
   C_PuiBsBox()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     22.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiBsImage::CalcHash(uint32 & oru32_HashValue) const
{
   // No calculation of c_UIImagePixmap necessary. Image can not be changed after creating it and
   // size is checked by base class

   C_PuiBsBox::CalcHash(oru32_HashValue);
}
