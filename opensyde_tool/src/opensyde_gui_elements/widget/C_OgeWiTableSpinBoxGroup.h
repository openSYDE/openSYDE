//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for table spin boxes (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWITABLESPINBOXGROUP_H
#define C_OGEWITABLESPINBOXGROUP_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeWiSpinBoxGroup.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiTableSpinBoxGroup :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   C_OgeWiTableSpinBoxGroup(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
