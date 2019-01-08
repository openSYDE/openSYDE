//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Context menu manager of system definition toplogy

   The context menu manager handles all request for context menus with its actions.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include "C_SdManTopologyContextMenuManager.h"
#include "C_SebUtil.h"
#include "C_GiLiLineGroup.h"
#include "C_GtGetText.h"
#include "gitypes.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     08.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdManTopologyContextMenuManager::C_SdManTopologyContextMenuManager() :
   C_SebTopologyBaseContextMenuManager()
{
   //   // insert actions
   this->mpc_ActionEdit = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText("Edit Properties"), this,
                                                         &C_SdManTopologyContextMenuManager::m_Edit);

   this->mpc_ActionEditSeparator = this->mc_ContextMenu.addSeparator();

   //move to right place
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEdit);
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEditSeparator);

   this->mpc_ActionInterfaceAssignment = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                           "Interface Assignment"),
                                                                        this,
                                                                        &C_SdManTopologyContextMenuManager::m_InterfaceAssignment);

   //move to right place
   this->mc_ContextMenu.insertAction(this->mpc_ActionOrderObjects, this->mpc_ActionInterfaceAssignment);
   m_InsertBendLineActions(this->mpc_ActionOrderObjects);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     10.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdManTopologyContextMenuManager::~C_SdManTopologyContextMenuManager()
{
   //lint -e{1540}  no memory leak because of the parent of all mpc_Action* and the Qt memory management
}

//-----------------------------------------------------------------------------
void C_SdManTopologyContextMenuManager::m_SetActionsInvisible(void)
{
   this->mpc_ActionEdit->setVisible(false);
   this->mpc_ActionInterfaceAssignment->setVisible(false);

   C_SebTopologyBaseContextMenuManager::m_SetActionsInvisible();
}

//-----------------------------------------------------------------------------
bool C_SdManTopologyContextMenuManager::m_ActivateSpecificActions(void)
{
   // specific functionality
   switch (this->mpc_ActiveItem->type())
   {
   case msn_GRAPHICS_ITEM_NODE:
      this->mpc_ActionEdit->setText(C_GtGetText::h_GetText("Edit Node Properties"));
      this->mpc_ActionEdit->setVisible(true);
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionOrderObjects->setVisible(true);
      break;
   // check for bus
   case msn_GRAPHICS_ITEM_BUS:         // buses have the same functionality
   case msn_GRAPHICS_ITEM_CANBUS:      // buses have the same functionality
   case msn_GRAPHICS_ITEM_ETHERNETBUS: // buses have the same functionality
      this->mpc_ActionSetupStyle->setVisible(true);
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionOrderObjects->setVisible(true);
      this->mpc_ActionEdit->setText(C_GtGetText::h_GetText("Edit Bus Properties"));
      this->mpc_ActionEdit->setVisible(true);
      break;
   // check for bus connector
   case msn_GRAPHICS_ITEM_BUS_CONNECT:
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionInterfaceAssignment->setVisible(true);
      break;
   case msn_GRAPHICS_ITEM_LINE_ARROW:
      this->mpc_ActionSetupStyle->setVisible(true);
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionOrderObjects->setVisible(true);
      break;
   case msn_GRAPHICS_ITEM_BOUNDARY:    // boundary and text element have the same functionality
   case msn_GRAPHICS_ITEM_TEXTELEMENT: // boundary and text element have the same functionality
      this->mpc_ActionSetupStyle->setVisible(true);
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionOrderObjects->setVisible(true);
      break;
   case msn_GRAPHICS_ITEM_TEXTELEMENT_BUS:
      this->mpc_ActionSetupStyle->setVisible(true);
      this->mpc_ActionOrderObjects->setVisible(true);
      this->mpc_ActionEdit->setText(C_GtGetText::h_GetText("Edit Bus Properties"));
      this->mpc_ActionEdit->setVisible(true);
      break;
   default:
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionOrderObjects->setVisible(true);
      break;
   }

   C_SebTopologyBaseContextMenuManager::m_ActivateSpecificActions();

   return true;
}

//-----------------------------------------------------------------------------
void C_SdManTopologyContextMenuManager::m_Edit()
{
   Q_EMIT this->SigEdit(this->mpc_ActiveItem);
}

//-----------------------------------------------------------------------------
void C_SdManTopologyContextMenuManager::m_InterfaceAssignment()
{
   Q_EMIT this->SigInterfaceAssignment(this->mpc_ActiveItem);
}
