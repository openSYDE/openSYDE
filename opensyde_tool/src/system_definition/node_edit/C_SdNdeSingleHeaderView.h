//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Header view for tables with only one header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDESINGLEHEADERVIEW_H
#define C_SDNDESINGLEHEADERVIEW_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SdNdeDataPoolListTableHeaderView.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeSingleHeaderView :
   public C_SdNdeDataPoolListTableHeaderView
{
   Q_OBJECT

public:
   C_SdNdeSingleHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
