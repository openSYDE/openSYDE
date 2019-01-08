//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Default label for dashboard, scaling (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDDEFAULT_H
#define C_OGELABDASHBOARDDEFAULT_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabAdaptiveSize.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDashboardDefault :
   public C_OgeLabAdaptiveSize
{
   Q_OBJECT

public:
   C_OgeLabDashboardDefault(QWidget * const opc_Parent);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
