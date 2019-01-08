//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       UI class for dashboard table data (implementation)

   UI class for dashboard table data

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "CSCLChecksums.h"
#include "C_PuiSvDbTable.h"

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

   \created     29.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbTable::C_PuiSvDbTable(void) :
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
void C_PuiSvDbTable::CalcHash(stw_types::uint32 & oru32_HashValue) const
{
   for (stw_types::uint32 u32_ItCol = 0; u32_ItCol < this->c_ColumnWidth.size(); ++u32_ItCol)
   {
      const stw_types::sint32 & rs32_ColWidth = this->c_ColumnWidth[u32_ItCol];
      stw_scl::C_SCLChecksums::CalcCRC32(&rs32_ColWidth, sizeof(rs32_ColWidth), oru32_HashValue);
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
bool C_PuiSvDbTable::IsReadElement(void) const
{
   return true;
}
