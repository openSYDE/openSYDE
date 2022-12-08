//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of system definition toplogy

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANTOPOLOGYCONTEXTMENUMANAGER_H
#define C_SDMANTOPOLOGYCONTEXTMENUMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItem>

#include "stwtypes.hpp"
#include "C_SebTopologyBaseContextMenuManager.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_SdContextMenuEntry.hpp"
#include "C_SebUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManTopologyContextMenuManager :
   public C_SebTopologyBaseContextMenuManager
{
   Q_OBJECT

public:
   C_SdManTopologyContextMenuManager();
   ~C_SdManTopologyContextMenuManager() override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigEdit(QGraphicsItem * const opc_Item);
   void SigInterfaceAssignment(QGraphicsItem * const opc_Item);

protected:
   void m_SetActionsInvisible(void) override;
   bool m_ActivateSpecificActions(void) override;
   bool m_ItemTypeHasSetupStyle(const int32_t os32_ItemType) override;

private:
   QAction * mpc_ActionEdit;
   QAction * mpc_ActionEditSeparator;
   // Custom actions
   QAction * mpc_ActionInterfaceAssignment;

   // Slots for actions
   void m_Edit(void);
   void m_InterfaceAssignment(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
