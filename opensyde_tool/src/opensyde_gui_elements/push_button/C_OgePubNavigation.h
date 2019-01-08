//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for navigation (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBNAVIGATION_H
#define C_OGEPUBNAVIGATION_H

/* -- Includes ------------------------------------------------------------- */
#include <QPushButton>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubNavigation :
   public QPushButton
{
   Q_OBJECT

public:
   explicit C_OgePubNavigation(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
