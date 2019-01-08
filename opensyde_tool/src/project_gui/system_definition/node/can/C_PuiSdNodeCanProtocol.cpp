//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class for a node CAN communication via a specific CAN communication protocol UI part (implementation)

   Data class for a node CAN communication via a specific CAN communication protocol UI part

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

#include "C_PuiSdNodeCanProtocol.h"

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
C_PuiSdNodeCanProtocol::C_PuiSdNodeCanProtocol(void)
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
void C_PuiSdNodeCanProtocol::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   for (stw_types::uint32 u32_ItContainer = 0; u32_ItContainer < this->c_ComMessages.size(); ++u32_ItContainer)
   {
      const C_PuiSdNodeCanMessageContainer & rc_Container = this->c_ComMessages[u32_ItContainer];
      rc_Container.CalcHash(oru32_HashValue);
   }
}
