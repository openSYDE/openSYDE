//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard tabs (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDTAB_H
#define C_OGELABDASHBOARDTAB_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDashboardTab :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabDashboardTab(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
