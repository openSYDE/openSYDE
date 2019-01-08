//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI text element bus data: stores UI information (implementation)

   UI text element bus data: stores UI information

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiSdTextElementBus.h"

#include "CSCLChecksums.h"

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

   \created     15.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSdTextElementBus::C_PuiSdTextElementBus() :
   C_PuiBsTextElement(),
   u32_BusIndex(0U)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     15.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSdTextElementBus::~C_PuiSdTextElementBus()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     15.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSdTextElementBus::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u32_BusIndex, sizeof(this->u32_BusIndex), oru32_HashValue);

   C_PuiBsTextElement::CalcHash(oru32_HashValue);
}
