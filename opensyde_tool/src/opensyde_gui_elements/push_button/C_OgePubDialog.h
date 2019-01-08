//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Standard dialog button (header)

   Standard dialog button (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBDIALOG_H
#define C_OGEPUBDIALOG_H

/* -- Includes ------------------------------------------------------------- */

#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubDialog :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubDialog(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
