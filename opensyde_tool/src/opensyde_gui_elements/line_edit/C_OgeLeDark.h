//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit for dark background (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELEDARK_H
#define C_OGELEDARK_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLeContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeDark :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   explicit C_OgeLeDark(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
