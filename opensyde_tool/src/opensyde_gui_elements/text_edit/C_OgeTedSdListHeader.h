//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit for system definition list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEDSDLISTHEADER_H
#define C_OGETEDSDLISTHEADER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeTedListHeaderBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTedSdListHeader :
   public C_OgeTedListHeaderBase
{
   Q_OBJECT

public:
   C_OgeTedSdListHeader(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
