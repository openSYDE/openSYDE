//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Standard configure button (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.07.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBCONFIGURE_H
#define C_OGEPUBCONFIGURE_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubConfigure :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_OgePubConfigure(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
