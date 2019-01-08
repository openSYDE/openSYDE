//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Frame for showing errors (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEERRORFRAME_H
#define C_OGEERRORFRAME_H

/* -- Includes ------------------------------------------------------------- */
#include <QFrame>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeErrorFrame :
   public QFrame
{
   Q_OBJECT

public:
   C_OgeErrorFrame(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
