//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom link label for list (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABLISTLINK_H
#define C_OGELABLISTLINK_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabListLink :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabListLink(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
