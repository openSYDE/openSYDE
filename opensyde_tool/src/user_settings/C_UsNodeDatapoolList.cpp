//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Project node datapool list user settings (implementation)

   Project node datapool list user settings

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.05.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_UsNodeDatapoolList.h"

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

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_UsNodeDatapoolList::C_UsNodeDatapoolList(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current column widths

   \return
   Current column widths

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
const std::vector<sint32> & C_UsNodeDatapoolList::GetColumnWidths(void) const
{
   return this->mc_ColumnWidth;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set column widths

   \param[in] orc_ColumnWidths Last known column widths

   \created     16.05.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_UsNodeDatapoolList::SetColumnSizes(const std::vector<sint32> & orc_ColumnWidths)
{
   this->mc_ColumnWidth = orc_ColumnWidths;
}
