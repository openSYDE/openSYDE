//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Bold label for group heading (header)

   Bold label for group heading (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABHEADINGGROUPBOLD_H
#define C_OGELABHEADINGGROUPBOLD_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeLabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabHeadingGroupBold :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabHeadingGroupBold(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
