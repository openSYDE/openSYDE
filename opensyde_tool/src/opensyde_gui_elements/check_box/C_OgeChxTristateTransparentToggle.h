//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom transparent tristate check box toggle (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXTRISTATETRANSPARENTTOGGLE_H
#define C_OGECHXTRISTATETRANSPARENTTOGGLE_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeChxTristateBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxTristateTransparentToggle :
   public C_OgeChxTristateBase
{
   Q_OBJECT

public:
   C_OgeChxTristateTransparentToggle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
