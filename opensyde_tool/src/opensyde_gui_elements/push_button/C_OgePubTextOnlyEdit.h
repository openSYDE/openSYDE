//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Simple transparent, text only button for edit mode (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBTEXTONLYEDIT_H
#define C_OGEPUBTEXTONLYEDIT_H

/* -- Includes ------------------------------------------------------------- */
#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubTextOnlyEdit :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubTextOnlyEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
