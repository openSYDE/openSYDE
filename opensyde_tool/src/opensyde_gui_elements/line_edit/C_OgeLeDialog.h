//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for dialogs (header)

   Line edit field for dialogs (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELEDIALOG_H
#define C_OGELEDIALOG_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLeContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeDialog :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   C_OgeLeDialog(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
