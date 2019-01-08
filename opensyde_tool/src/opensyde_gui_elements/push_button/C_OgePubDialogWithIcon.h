//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Standard dialog button including an icon (header)

   Standard dialog button including an icon (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBDIALOGWITHICON_H
#define C_OGEPUBDIALOGWITHICON_H

/* -- Includes ------------------------------------------------------------- */

#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubDialogWithIcon :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubDialogWithIcon(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
