//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Main button in material design of navigation bar

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_OGEPUBUSECASENAV_H
#define C_OGEPUBUSECASENAV_H

/* -- Includes ------------------------------------------------------------- */

#include "precomp_headers.h"
#include "C_OgePubStandard.h"
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubUseCaseNav :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubUseCaseNav(QWidget * const opc_Parent = NULL);

   bool q_LastButton;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
