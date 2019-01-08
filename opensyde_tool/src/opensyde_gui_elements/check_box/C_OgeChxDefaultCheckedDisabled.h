//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom default check box with a disabled checked state (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXDEFAULTCHECKEDDISABLED_H
#define C_OGECHXDEFAULTCHECKEDDISABLED_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeChxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxDefaultCheckedDisabled :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxDefaultCheckedDisabled(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
