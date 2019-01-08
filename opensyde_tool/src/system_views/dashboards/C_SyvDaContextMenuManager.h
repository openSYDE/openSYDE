//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of dashboard scene (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDACONTEXTMENUMANAGER_H
#define C_SYVDACONTEXTMENUMANAGER_H

/* -- Includes ------------------------------------------------------------- */
#include <QAction>
#include <QList>

#include "C_SebBaseContextMenuManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaContextMenuManager :
   public C_SebBaseContextMenuManager
{
   Q_OBJECT

public:
   C_SyvDaContextMenuManager();
   virtual ~C_SyvDaContextMenuManager();

   QAction * RegisterAction(const QString & orc_Text);
   QAction * RegisterSeperator(void);
   void SetVisibleWithAutoHide(QAction * const opc_Action);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigEditProperties(QGraphicsItem * const opc_Item);

protected:
   virtual void m_SetActionsInvisible(void) override;
   virtual bool m_ActivateSpecificActions(void) override;

private:
   //Avoid call
   C_SyvDaContextMenuManager(const C_SyvDaContextMenuManager &);
   C_SyvDaContextMenuManager & operator =(const C_SyvDaContextMenuManager &);

   QList<QAction *> mc_ListVisibleRegisterdActions;
   QAction * mpc_ActionEdit;
   QAction * mpc_ActionEditSeparator;

   void m_Edit(void);
   void m_HideRegisteredActions(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
