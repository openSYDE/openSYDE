//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class for CAN communication message related, additional information UI part (implementation)

   Data class for CAN communication message related, additional information UI part

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSdNodeCanMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     13.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdNodeCanMessage::C_PuiSdNodeCanMessage(void) :
   q_UseAutoReceiveTimeout(true)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     22.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdNodeCanMessage::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(&q_UseAutoReceiveTimeout, sizeof(q_UseAutoReceiveTimeout), oru32_HashValue);
}
