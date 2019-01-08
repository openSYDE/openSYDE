//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard label item value (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDLABELVALUE_H
#define C_OGELABDASHBOARDLABELVALUE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLabAdaptiveSize.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabDashboardLabelValue :
   public C_OgeLabAdaptiveSize
{
   Q_OBJECT

public:
   C_OgeLabDashboardLabelValue(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
