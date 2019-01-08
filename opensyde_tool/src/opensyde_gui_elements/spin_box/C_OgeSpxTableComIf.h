//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom spin box for table com if(header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXTABLECOMIF_H
#define C_OGESPXTABLECOMIF_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeSpxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxTableComIf :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxTableComIf(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
