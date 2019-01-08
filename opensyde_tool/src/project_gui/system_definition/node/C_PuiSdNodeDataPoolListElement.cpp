//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class for data pool list element UI part (implementation)

   Data class for data pool list element UI part

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiSdNodeDataPoolListElement.h"

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

   \created     16.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdNodeDataPoolListElement::C_PuiSdNodeDataPoolListElement(void) :
   q_AutoMinMaxActive(true),
   q_InterpretAsString(false)
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
void C_PuiSdNodeDataPoolListElement::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_AutoMinMaxActive, sizeof(this->q_AutoMinMaxActive), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_InterpretAsString, sizeof(this->q_InterpretAsString), oru32_HashValue);
}
