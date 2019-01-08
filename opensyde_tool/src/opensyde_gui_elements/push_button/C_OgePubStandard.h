//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

#ifndef C_OGEPUBSTANDARD_H
#define C_OGEPUBSTANDARD_H

/* -- Includes ------------------------------------------------------------- */

#include "precomp_headers.h"
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubStandard :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubStandard(QWidget * const opc_Parent = NULL);

protected:
   void m_SetPenColorForFont(QPainter * const opc_Painter) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
