//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Int 64 spin box field for properties (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXINT64PROPERTIES_H
#define C_OGESPXINT64PROPERTIES_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeSpxInt64ToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxInt64Properties :
   public C_OgeSpxInt64ToolTipBase
{
   Q_OBJECT

public:
   C_OgeSpxInt64Properties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif