//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Checkbox for setting widgets. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.11.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGECHXSETTINGS_H
#define C_CAMOGECHXSETTINGS_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeChxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeChxSettings :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeChxSettings(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
