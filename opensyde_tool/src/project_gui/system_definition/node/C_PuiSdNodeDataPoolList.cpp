//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data class for data pool list UI part (implementation)

   Data class for data pool list UI part

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

#include "C_PuiSdNodeDataPoolList.h"

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
C_PuiSdNodeDataPoolList::C_PuiSdNodeDataPoolList(void)
{
   c_DataPoolListElements.resize(1);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     22.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSdNodeDataPoolList::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   for (u32_Counter = 0U; u32_Counter < this->c_DataPoolListElements.size(); ++u32_Counter)
   {
      this->c_DataPoolListElements[u32_Counter].CalcHash(oru32_HashValue);
   }
}
