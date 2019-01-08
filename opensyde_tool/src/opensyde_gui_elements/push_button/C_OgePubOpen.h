//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Standard open button (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBOPEN_H
#define C_OGEPUBOPEN_H

/* -- Includes ------------------------------------------------------------- */
#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubOpen :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubOpen(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
