//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget for selecting modes (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETAWSELECTOR_H
#define C_OGETAWSELECTOR_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeTawToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTawSelector :
   public C_OgeTawToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeTawSelector(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
