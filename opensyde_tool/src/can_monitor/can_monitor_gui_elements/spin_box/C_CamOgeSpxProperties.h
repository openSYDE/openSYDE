//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box design for properties screen (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGESPXPROPERTIES_H
#define C_CAMOGESPXPROPERTIES_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeSpxToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeSpxProperties :
   public C_OgeSpxToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgeSpxProperties(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
