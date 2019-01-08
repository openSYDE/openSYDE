//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser field for report widgets (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEBREPORT_H
#define C_OGETEBREPORT_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeTebContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTebReport :
   public C_OgeTebContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeTebReport(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
