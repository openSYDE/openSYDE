//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Button with two states bold (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBTOGGLEBOLD_H
#define C_OGEPUBTOGGLEBOLD_H

/* -- Includes ------------------------------------------------------------- */
#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubToggleBold :
   public QPushButton
{
   Q_OBJECT

public:
   C_OgePubToggleBold(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
