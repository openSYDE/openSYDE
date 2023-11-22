//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of dashboard scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACONTEXTMENUMANAGER_HPP
#define C_SYVDACONTEXTMENUMANAGER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QAction>
#include <QList>

#include "C_SebBaseContextMenuManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaContextMenuManager :
   public C_SebBaseContextMenuManager
{
   Q_OBJECT

public:
   C_SyvDaContextMenuManager();
   ~C_SyvDaContextMenuManager() override;

   QAction * RegisterAction(const QString & orc_Text);
   QAction * RegisterActionWithKeyboardShortcut(const QString & orc_Text, const QKeySequence & orc_Sequence);
   QAction * RegisterSeperator(void);
   void SetVisibleWithAutoHide(QAction * const opc_Action);
   void SetSpecificActionsAvailable(const bool oq_Available, const bool oq_FurtherActionsWillBeAdded);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigEditProperties(QGraphicsItem * const opc_Item);
   void SigEditContent(QGraphicsItem * const opc_Item);

protected:
   void m_SetActionsInvisible(void) override;
   bool m_ActivateSpecificActions(void) override;
   bool m_ItemTypeHasSetupStyle(const int32_t os32_ItemType) override;

private:
   //Avoid call
   C_SyvDaContextMenuManager(const C_SyvDaContextMenuManager &);
   C_SyvDaContextMenuManager & operator =(const C_SyvDaContextMenuManager &) &; //lint !e1511 //we want to hide the base
                                                                                // func.

   QList<QAction *> mc_ListVisibleRegisterdActions;
   QAction * mpc_ActionEditProperties;
   QAction * mpc_ActionEditContent;
   QAction * mpc_ActionEditSeparator;

   bool mq_SpecificActionsAvailable;
   bool mq_FurtherActionsWillBeAdded;

   void m_EditProperties(void);
   void m_EditContent(void);
   void m_HideRegisteredActions(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
