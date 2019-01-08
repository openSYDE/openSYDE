//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text edit with custom context menu (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.05.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEDBASE_H
#define C_OGETEDBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QTextEdit>
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTedContextMenuBase :
   public QTextEdit
{
   Q_OBJECT

public:
   C_OgeTedContextMenuBase(QWidget * const opc_Parent = NULL);

private:
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_Delete() const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
