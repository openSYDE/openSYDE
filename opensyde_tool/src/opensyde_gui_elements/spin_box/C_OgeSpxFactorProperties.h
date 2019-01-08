//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Factor spin box for properties dialog (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXFACTORPROPERTIES_H
#define C_OGESPXFACTORPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeSpxFactor.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxFactorProperties :
   public C_OgeSpxFactor
{
   Q_OBJECT

public:
   C_OgeSpxFactorProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
