//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit with custom context menu (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.05.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELECONTEXTMENUBASE_H
#define C_OGELECONTEXTMENUBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QLineEdit>
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLeContextMenuBase :
   public QLineEdit
{
   Q_OBJECT

public:
   C_OgeLeContextMenuBase(QWidget * const opc_Parent);

private:
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
