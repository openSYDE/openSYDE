//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with arrow heads (header)

   Custom combo box (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECBXARROW_H
#define C_OGECBXARROW_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeCbxIconOnly.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeCbxArrow :
   public C_OgeCbxIconOnly
{
   Q_OBJECT

public:
   C_OgeCbxArrow(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
