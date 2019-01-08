//-----------------------------------------------------------------------------
/*!
   \file
   \brief       View sub button in material design of navigation bar

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBUSECASENAVVIEWSUB_H
#define C_OGEPUBUSECASENAVVIEWSUB_H

/* -- Includes ------------------------------------------------------------- */

#include "precomp_headers.h"
#include <QPushButton>
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubUseCaseNavViewSub :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgePubUseCaseNavViewSub(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
