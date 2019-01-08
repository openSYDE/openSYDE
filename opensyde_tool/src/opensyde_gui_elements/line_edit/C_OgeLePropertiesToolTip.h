//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for properties with tool tip (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELEPROPERTIESTOOLTIP_H
#define C_OGELEPROPERTIESTOOLTIP_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLePropertiesToolTip :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   C_OgeLePropertiesToolTip(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
