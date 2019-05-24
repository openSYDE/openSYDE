//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of system view topology

   The context menu manager handles all request for context menus with its actions.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "gitypes.h"
#include "C_SyvSeContextMenuManager.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeContextMenuManager::C_SyvSeContextMenuManager() :
   C_SebTopologyBaseContextMenuManager()
{
   // insert actions
   this->mpc_ActionShowNodeInformation = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                           "Show Node Information"), this,
                                                                        &C_SyvSeContextMenuManager::m_ShowNodeInformation);
   this->mpc_ActionShowCanConfiguration = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                            "Configure PC CAN Interface"), this,
                                                                         &C_SyvSeContextMenuManager::m_ShowCanConfiguration);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvSeContextMenuManager::~C_SyvSeContextMenuManager()
{
   //lint -e{1540}  no memory leak because of the parent of all mpc_Action* and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handles a context menu event and shows the context menu with the necessary actions

   \param[in]     opc_Event            Event identification and information
   \param[in]     orc_SelectedItems    All selected and for the context menu relevant items
   \param[in]     orq_ShowPaste        Indicator if paste functionality is available (Not used here)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeContextMenuManager::HandleContextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event,
                                                       const QList<QGraphicsItem *> & orc_SelectedItems,
                                                       const bool & orq_ShowPaste)
{
   // No generic functions necessary here. Do not call base function.
   bool q_ShowMenu = false;

   Q_UNUSED(orq_ShowPaste)

   // reset all previous configurations
   this->mpc_ActiveItem = NULL;
   // set all actions invisible
   this->m_SetActionsInvisible();

   // save position
   this->mc_ScenePos = opc_Event->scenePos();

   this->mpc_ActionShowNodeInformation->setVisible(false);
   this->mpc_ActionShowCanConfiguration->setVisible(false);

   if (orc_SelectedItems.size() == 1)
   {
      // save the parent of the selected item or selected item
      this->mpc_ActiveItem = C_SebUtil::h_GetHighestParent(orc_SelectedItems[0]);

      if (this->mpc_ActiveItem->type() == msn_GRAPHICS_ITEM_PC)
      {
         // TODO: Deactivated because of problem with no selection of PC in edit mode only
         this->mpc_ActionShowCanConfiguration->setVisible(false);
         q_ShowMenu = true;
      }
      else if (this->mpc_ActiveItem->type() == msn_GRAPHICS_ITEM_NODE)
      {
         // TODO: Deactivated because of problem with no selection of nodes
         this->mpc_ActionShowNodeInformation->setVisible(false);
         q_ShowMenu = true;
      }
      else
      {
         q_ShowMenu = this->m_ActivateSpecificActions();
      }
   }

   if ((q_ShowMenu == true) && (this->mc_ContextMenu.isEmpty() == false))
   {
      // show the menu
      this->mc_ContextMenu.popup(opc_Event->screenPos());
   }
}
//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeContextMenuManager::m_ShowNodeInformation(void)
{
   Q_EMIT this->SigShowNodeInformation(this->mpc_ActiveItem);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvSeContextMenuManager::m_ShowCanConfiguration(void)
{
   Q_EMIT this->SigShowCanConfiguration(this->mpc_ActiveItem);
}
