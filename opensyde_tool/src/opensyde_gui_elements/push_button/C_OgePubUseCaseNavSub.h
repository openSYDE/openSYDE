//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Sub button in material design of navigation bar

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_OGEPUBUSECASENAVSUB_H
#define C_OGEPUBUSECASENAVSUB_H

/* -- Includes ------------------------------------------------------------- */

#include "precomp_headers.h"
#include <QPushButton>
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubUseCaseNavSub :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgePubUseCaseNavSub(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
