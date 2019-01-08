//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Int64 spin box which should avoid a zero value, table style (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXINT64FACTORTABLE_H
#define C_OGESPXINT64FACTORTABLE_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeSpxInt64Factor.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxInt64FactorTable :
   public C_OgeSpxInt64Factor
{
   Q_OBJECT

public:
   C_OgeSpxInt64FactorTable(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
