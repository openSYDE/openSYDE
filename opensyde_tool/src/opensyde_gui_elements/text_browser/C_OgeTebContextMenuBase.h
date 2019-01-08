//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser with custom context menu (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.07.2018  STW/G.Scupin
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETEBCONTEXTMENUBASE_H
#define C_OGETEBCONTEXTMENUBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QTextBrowser>
#include "C_OgeContextMenu.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTebContextMenuBase :
   public QTextBrowser
{
   Q_OBJECT

public:
   C_OgeTebContextMenuBase(QWidget * const opc_Parent = NULL);

private:
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QString mc_LinkText;
   void m_InitContextMenu(void);
   void m_SetupContextMenu(const QPoint & orc_Pos);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_CopyHyperLink(void) const;
   void m_LinkClicked(const QUrl & orc_Link) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
