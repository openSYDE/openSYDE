//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit variant to be integrated into a combo box (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELECOMBOBOX_H
#define C_OGELECOMBOBOX_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgeLeContextMenuBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeComboBox :
   public C_OgeLeContextMenuBase
{
public:
   C_OgeLeComboBox(QWidget * const opc_Parent = NULL);

protected:
   virtual void mousePressEvent(QMouseEvent * const opc_Event);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
