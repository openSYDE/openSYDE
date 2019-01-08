//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Header view for tables with both headers (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTDATASETHEADERVIEW_H
#define C_SDNDEDATAPOOLLISTDATASETHEADERVIEW_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SdNdeDataPoolListTableHeaderView.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolListDataSetHeaderView :
   public C_SdNdeDataPoolListTableHeaderView
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListDataSetHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
