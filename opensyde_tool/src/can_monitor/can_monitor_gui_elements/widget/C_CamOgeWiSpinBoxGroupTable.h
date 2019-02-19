//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box group style for table (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEWISPINBOXGROUPTABLE_H
#define C_CAMOGEWISPINBOXGROUPTABLE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeWiSpinBoxGroup.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeWiSpinBoxGroupTable :
   public C_OgeWiSpinBoxGroup
{
   Q_OBJECT

public:
   explicit C_CamOgeWiSpinBoxGroupTable(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
