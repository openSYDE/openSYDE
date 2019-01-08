//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box field for table (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXINT64TABLE_H
#define C_OGESPXINT64TABLE_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeSpxInt64ToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxInt64Table :
   public C_OgeSpxInt64ToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxInt64Table(QWidget * const opc_Parent = NULL, const bool & orq_IsUnsigned = false);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
