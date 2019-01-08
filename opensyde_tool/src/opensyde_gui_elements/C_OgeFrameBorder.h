//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom border draw frame (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEFRAMEBORDER_H
#define C_OGEFRAMEBORDER_H

/* -- Includes ------------------------------------------------------------- */
#include <QFrame>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeFrameBorder :
   public QFrame
{
   Q_OBJECT

public:
   C_OgeFrameBorder(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
