//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with lines (header)

   Custom combo box with lines (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.12.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECBXLINE_H
#define C_OGECBXLINE_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeCbxIconOnly.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeCbxLine :
   public C_OgeCbxIconOnly
{
   Q_OBJECT

public:
   C_OgeCbxLine(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
