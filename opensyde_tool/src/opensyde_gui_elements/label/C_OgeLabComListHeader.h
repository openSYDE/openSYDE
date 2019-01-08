//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for com list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABCOMLISTHEADER_H
#define C_OGELABCOMLISTHEADER_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabComListHeader :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabComListHeader(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
