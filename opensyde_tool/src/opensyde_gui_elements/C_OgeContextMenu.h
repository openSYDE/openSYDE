//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Custom context menu (header)

   Custom context menu (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.07.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGECONTEXTMENU_H
#define C_OGECONTEXTMENU_H

/* -- Includes ------------------------------------------------------------- */

#include <QPaintEvent>
#include <QMenu>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeContextMenu :
   public QMenu
{
   Q_OBJECT

public:
   C_OgeContextMenu(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeContextMenu();

private:
   //Avoid call
   C_OgeContextMenu(const C_OgeContextMenu &);
   C_OgeContextMenu & operator =(const C_OgeContextMenu &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
