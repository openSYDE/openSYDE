//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom properties check box (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECHXPROPERTIES_H
#define C_OGECHXPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeChxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeChxProperties :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   C_OgeChxProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
