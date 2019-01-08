//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Simple transparent, text button with border for edit mode (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBTEXTWITHBORDEREDIT_H
#define C_OGEPUBTEXTWITHBORDEREDIT_H

/* -- Includes ------------------------------------------------------------- */
#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubTextWithBorderEdit :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubTextWithBorderEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
