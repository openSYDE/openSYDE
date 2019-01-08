//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Properties radio button style (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.06.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGERABPROPERTIES_H
#define C_OGERABPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeRabToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeRabProperties :
   public C_OgeRabToolTipBase
{
   Q_OBJECT

public:
   C_OgeRabProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
