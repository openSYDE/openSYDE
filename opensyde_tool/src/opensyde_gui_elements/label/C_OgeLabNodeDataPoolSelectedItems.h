//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label for display of selected items cound in node data pool (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.02.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABNODEDATAPOOLSELECTEDITEMS_H
#define C_OGELABNODEDATAPOOLSELECTEDITEMS_H

/* -- Includes ------------------------------------------------------------- */

#include <QLabel>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabNodeDataPoolSelectedItems :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabNodeDataPoolSelectedItems(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
