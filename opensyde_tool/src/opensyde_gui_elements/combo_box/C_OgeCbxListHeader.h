//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box for list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECBXLISTHEADER_H
#define C_OGECBXLISTHEADER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeCbxIconOnly.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeCbxListHeader :
   public C_OgeCbxIconOnly
{
   Q_OBJECT

public:
   C_OgeCbxListHeader(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
