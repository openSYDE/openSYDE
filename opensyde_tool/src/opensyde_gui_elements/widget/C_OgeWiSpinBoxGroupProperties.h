//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box types group with properties styling (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.04.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWISPINBOXGROUPPROPERTIES_H
#define C_OGEWISPINBOXGROUPPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeWiSpinBoxGroup.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiSpinBoxGroupProperties :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_OgeWiSpinBoxGroupProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
