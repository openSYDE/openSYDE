//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Label with custom context menu (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.06.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGELABCONTEXTMENUBASE_H
#define C_OGELABCONTEXTMENUBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeLabContextMenuBase :
   public QLabel
{
   Q_OBJECT

public:
   explicit C_OgeLabContextMenuBase(QWidget * const opc_Parent = NULL);

private:
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_Copy() const;
   void m_SelectAll();
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
