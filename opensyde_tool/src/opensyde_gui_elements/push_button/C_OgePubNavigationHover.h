//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Transparent push button with hover state (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBNAVIGATIONHOVER_H
#define C_OGEPUBNAVIGATIONHOVER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubNavigationHover :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubNavigationHover(QWidget * const opc_Parent = NULL);

   void SetActive(const bool oq_Active);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
