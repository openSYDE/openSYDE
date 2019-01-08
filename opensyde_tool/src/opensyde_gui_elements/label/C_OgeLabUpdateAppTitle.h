//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for update application title (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABUPDATEAPPTITLE_H
#define C_OGELABUPDATEAPPTITLE_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabUpdateAppTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabUpdateAppTitle(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif