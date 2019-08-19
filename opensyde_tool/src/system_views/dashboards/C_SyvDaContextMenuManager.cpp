//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of dashboard scene (implementation)

   Context menu manager of dashboard scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "gitypes.h"
#include "C_GtGetText.h"
#include "C_GiLiLineGroup.h"
#include "C_SyvDaContextMenuManager.h"

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
C_SyvDaContextMenuManager::C_SyvDaContextMenuManager()
{
   // add all actions
   this->mpc_ActionEdit = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                            "Edit Properties"), this,
                                                         &C_SyvDaContextMenuManager::m_Edit);
   this->mpc_ActionEditSeparator = this->mc_ContextMenu.addSeparator();

   //move to right place
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEdit);
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEditSeparator);

   connect(this, &C_SyvDaContextMenuManager::SigContextMenuClosed, this,
           &C_SyvDaContextMenuManager::m_HideRegisteredActions);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaContextMenuManager::~C_SyvDaContextMenuManager()
{
   //lint -e{1540}  no memory leak because of the parent of all mpc_Action* and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds an action to the context menu and returns its pointer (at front)

   \param[in]     orc_Text       Shown text of action

   \return
   Pointer to action
*/
//----------------------------------------------------------------------------------------------------------------------
QAction * C_SyvDaContextMenuManager::RegisterAction(const QString & orc_Text)
{
   QAction * const pc_Retval = this->mc_ContextMenu.addAction(orc_Text);

   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, pc_Retval);
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adds a separator to the context menu and returns its pointer (at front)

   \return
   Pointer to action
*/
//----------------------------------------------------------------------------------------------------------------------
QAction * C_SyvDaContextMenuManager::RegisterSeperator(void)
{
   QAction * const pc_Retval = this->mc_ContextMenu.addSeparator();

   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, pc_Retval);
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the action visible and registers it for hiding after closing the context menu

   \param[in]     opc_Action       Pointer to action

   \return
   Pointer to action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::SetVisibleWithAutoHide(QAction * const opc_Action)
{
   opc_Action->setVisible(true);
   this->mc_ListVisibleRegisterdActions.push_back(opc_Action);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide all actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::m_SetActionsInvisible(void)
{
   C_SebBaseContextMenuManager::m_SetActionsInvisible();
   this->mpc_ActionEdit->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaContextMenuManager::m_ActivateSpecificActions(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiLiLineGroup * pc_LineGroup = dynamic_cast<C_GiLiLineGroup *>(this->mpc_ActiveItem);
   bool q_Return = false;

   // specific functionality
   switch (this->mpc_ActiveItem->type())
   {
   case msn_GRAPHICS_ITEM_LINE_ARROW:
      this->mpc_ActionSetupStyle->setVisible(true);
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionBringToFront->setVisible(true);
      this->mpc_ActionSendToBack->setVisible(true);
      this->mpc_ActionBendLine->setVisible(true);
      q_Return = true;
      break;
   case msn_GRAPHICS_ITEM_IMAGE:
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionBringToFront->setVisible(true);
      this->mpc_ActionSendToBack->setVisible(true);
      q_Return = true;
      break;
   case msn_GRAPHICS_ITEM_BOUNDARY:    // boundary and text element have the same functionality
   case msn_GRAPHICS_ITEM_TEXTELEMENT: // boundary and text element have the same functionality
      this->mpc_ActionSetupStyle->setVisible(true);
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionBringToFront->setVisible(true);
      this->mpc_ActionSendToBack->setVisible(true);
      q_Return = true;
      break;
   case msn_GRAPHICS_ITEM_DB_LABEL:        // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_SPIN_BOX:     // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_PROGRESS_BAR: // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_SLIDER:       // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_TOGGLE:       // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_PIE_CHART:    // Basic functions of dashboard elements are the same
      // Specific functions will be registered by the items with the function RegisterAction if needed
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionBringToFront->setVisible(true);
      this->mpc_ActionSendToBack->setVisible(true);
      this->mpc_ActionEdit->setVisible(true);
      q_Return = true;
      break;
   case msn_GRAPHICS_ITEM_DB_CHART: // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_TABLE: // Basic functions of dashboard elements are the same
   case msn_GRAPHICS_ITEM_DB_PARAM: // Basic functions of dashboard elements are the same
      // Specific functions will be registered by the items with the function RegisterAction if needed
      this->mpc_ActionCut->setVisible(true);
      this->mpc_ActionCopy->setVisible(true);
      this->mpc_ActionDelete->setVisible(true);
      this->mpc_ActionBringToFront->setVisible(true);
      this->mpc_ActionSendToBack->setVisible(true);
      q_Return = true;
      break;
   default:
      break;
   }

   //Setup style
   if (m_ItemTypeHasSetupStyle(this->mpc_ActiveItem->type()))
   {
      this->mpc_ActionSetupStyle->setVisible(true);
   }

   if (pc_LineGroup != NULL)
   {
      // Special case: Points can be removed if there are more than 2 points
      if (pc_LineGroup->GetNumberPoints() > 2)
      {
         //Check bend point
         if (pc_LineGroup->CheckBendPointAt(this->mc_ScenePos) == true)
         {
            this->mpc_ActionRemoveBendLine->setVisible(true);
            q_Return = true;
         }
         else
         {
            this->mpc_ActionRemoveBendLine->setVisible(false);
         }
      }
      else
      {
         this->mpc_ActionRemoveBendLine->setVisible(false);
      }
   }
   else
   {
      this->mpc_ActionRemoveBendLine->setVisible(false);
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the input item type requires a setup style in the context menu

   \param[in] osn_ItemType Item type to check

   \retval   True    Setup style menu is required
   \retval   False   Setup style menu should stay hidden
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaContextMenuManager::m_ItemTypeHasSetupStyle(const stw_types::sintn osn_ItemType)
{
   bool q_Retval;

   switch (osn_ItemType)
   {
   case msn_GRAPHICS_ITEM_LINE_ARROW:
   case msn_GRAPHICS_ITEM_BOUNDARY:
   case msn_GRAPHICS_ITEM_TEXTELEMENT:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Emit edit signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::m_Edit(void)
{
   Q_EMIT this->SigEditProperties(this->mpc_ActiveItem);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::m_HideRegisteredActions(void)
{
   QList<QAction *>::const_iterator c_ItItem;

   for (c_ItItem = this->mc_ListVisibleRegisterdActions.begin();
        c_ItItem != this->mc_ListVisibleRegisterdActions.end();
        ++c_ItItem)
   {
      (*c_ItItem)->setVisible(false);
   }

   this->mc_ListVisibleRegisterdActions.clear();
}
