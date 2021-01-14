//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of dashboard scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACONTEXTMENUMANAGER_H
#define C_SYVDACONTEXTMENUMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAction>
#include <QList>

#include "C_SebBaseContextMenuManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

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
   virtual bool m_ItemTypeHasSetupStyle(const stw_types::sintn osn_ItemType) override;

private:
   //Avoid call
   C_SyvDaContextMenuManager(const C_SyvDaContextMenuManager &);
   C_SyvDaContextMenuManager & operator =(const C_SyvDaContextMenuManager &); //lint !e1511 //we want to hide the base
                                                                              // func.

   QList<QAction *> mc_ListVisibleRegisterdActions;
   QAction * mpc_ActionEdit;
   QAction * mpc_ActionEditSeparator;

   void m_Edit(void);
   void m_HideRegisteredActions(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
