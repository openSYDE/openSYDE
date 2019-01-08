//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for param widget spin boxes (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.10.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIPARAMSPINBOXGROUP_H
#define C_OGEWIPARAMSPINBOXGROUP_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeWiSpinBoxGroup.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiParamSpinBoxGroup :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_OgeWiParamSpinBoxGroup(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
