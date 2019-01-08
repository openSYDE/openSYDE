//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for properties (header)

   Line edit field for properties (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELEPROPERTIES_H
#define C_OGELEPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeProperties :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   C_OgeLeProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
