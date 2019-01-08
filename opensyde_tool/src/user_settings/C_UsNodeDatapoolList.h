//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Project node datapool list user settings (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.05.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_USNODEDATAPOOLLIST_H
#define C_USNODEDATAPOOLLIST_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UsNodeDatapoolList
{
public:
   C_UsNodeDatapoolList(void);

   const std::vector<stw_types::sint32> & GetColumnWidths(void) const;
   void SetColumnSizes(const std::vector<stw_types::sint32> & orc_ColumnWidths);

private:
   std::vector<stw_types::sint32> mc_ColumnWidth;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
