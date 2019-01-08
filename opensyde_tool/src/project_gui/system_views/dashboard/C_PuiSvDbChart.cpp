//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI class for dashboard chart data (implementation)

   UI class for dashboard chart data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSvDbChart.h"

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
   \brief   Default destructor

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbChart::C_PuiSvDbChart() :
   C_PuiSvDbWidgetBase()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out] oru32_HashValue    Hash value with init [in] value and result [out] value

   \created     29.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbChart::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   for (stw_types::uint32 u32_ItActive = 0; u32_ItActive < this->c_DataPoolElementsActive.size(); ++u32_ItActive)
   {
      const bool q_Data = this->c_DataPoolElementsActive[u32_ItActive];
      stw_scl::C_SCLChecksums::CalcCRC32(&q_Data, sizeof(q_Data), oru32_HashValue);
   }
   C_PuiSvDbWidgetBase::CalcHash(oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if item is a read element

   \return
   True  Read element
   False Write element

   \created     27.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_PuiSvDbChart::IsReadElement(void) const
{
   return true;
}
