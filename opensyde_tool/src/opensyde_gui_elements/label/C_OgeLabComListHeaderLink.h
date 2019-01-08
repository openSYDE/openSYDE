//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for a link in com list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABCOMLISTHEADERLINK_H
#define C_OGELABCOMLISTHEADERLINK_H

/* -- Includes ------------------------------------------------------------- */

#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabComListHeaderLink :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabComListHeaderLink(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
