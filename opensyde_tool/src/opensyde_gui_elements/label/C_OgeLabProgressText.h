//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for progress text (under a progress bopperle) (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.10.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABPROGRESSTEXT_H
#define C_OGELABPROGRESSTEXT_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabProgressText :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabProgressText(QWidget * const opc_Parent);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
