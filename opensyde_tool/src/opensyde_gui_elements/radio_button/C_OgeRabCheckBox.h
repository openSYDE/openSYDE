//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Radio button check box style (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGERABCHECKBOX_H
#define C_OGERABCHECKBOX_H

/* -- Includes ------------------------------------------------------------- */
#include <QRadioButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeRabCheckBox :
   public QRadioButton
{
   Q_OBJECT

public:
   C_OgeRabCheckBox(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
