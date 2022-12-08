//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of dashboard scene (implementation)

   Context menu manager of dashboard scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "gitypes.hpp"
#include "C_GtGetText.hpp"
#include "C_GiLiLineGroup.hpp"
#include "C_SyvDaContextMenuManager.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

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
C_SyvDaContextMenuManager::C_SyvDaContextMenuManager() :
   mq_SpecificActionsAvailable(true),
   mq_FurtherActionsWillBeAdded(false)
{
   // add all actions
   this->mpc_ActionEditProperties = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                      "Edit Properties"), this,
                                                                   &C_SyvDaContextMenuManager::m_EditProperties);
   this->mpc_ActionEditContent = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                   "Edit Content"), this,
                                                                &C_SyvDaContextMenuManager::m_EditContent,
                                                                static_cast<int32_t>(Qt::Key_F2));
   this->mpc_ActionEditSeparator = this->mc_ContextMenu.addSeparator();

   //move to right place
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEditProperties);
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEditContent);
   this->mc_ContextMenu.insertAction(this->mpc_ActionCut, this->mpc_ActionEditSeparator);

   connect(this, &C_SyvDaContextMenuManager::SigContextMenuClosed, this,
           &C_SyvDaContextMenuManager::m_HideRegisteredActions);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of all mpc_Action* and the Qt memory management
C_SyvDaContextMenuManager::~C_SyvDaContextMenuManager()
{
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
/*! \brief  Adds an action to the context menu, adds keyboard shortcut and returns its pointer (at front)

   \param[in]  orc_Text       Shown text of action
   \param[in]  orc_Sequence   Sequence

   \return
   Pointer to action
*/
//----------------------------------------------------------------------------------------------------------------------
QAction * C_SyvDaContextMenuManager::RegisterActionWithKeyboardShortcut(const QString & orc_Text,
                                                                        const QKeySequence & orc_Sequence)
{
   QAction * const pc_Retval = this->RegisterAction(orc_Text);

   pc_Retval->setShortcut(orc_Sequence);
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
/*! \brief   Sets the flag if the default actions shall be visible or not

   \param[in]       oq_Available                     Flag if the "normal specific" entries shall be visible or not
   \param[in]       oq_FurtherActionsWillBeAdded     Flag if a specific dashboard element will add further context menu
                                                     entries and the context menu must be showed
                                                     Only relevant if oq_Available is false
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::SetSpecificActionsAvailable(const bool oq_Available,
                                                            const bool oq_FurtherActionsWillBeAdded)
{
   this->mq_SpecificActionsAvailable = oq_Available;
   this->mq_FurtherActionsWillBeAdded = oq_FurtherActionsWillBeAdded;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide all actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::m_SetActionsInvisible(void)
{
   C_SebBaseContextMenuManager::m_SetActionsInvisible();
   this->mpc_ActionEditProperties->setVisible(false);
   this->mpc_ActionEditContent->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activate single item specific actions

   \return
   true     Specific action found
   false    No specific action found
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaContextMenuManager::m_ActivateSpecificActions(void)
{
   bool q_Return = false;

   if (this->mq_SpecificActionsAvailable == true)
   {
      const C_GiLiLineGroup * const pc_LineGroup = dynamic_cast<C_GiLiLineGroup *>(this->mpc_ActiveItem);

      // specific functionality
      switch (this->mpc_ActiveItem->type())
      {
      case ms32_GRAPHICS_ITEM_LINE_ARROW:
         this->mpc_ActionSetupStyle->setVisible(true);
         this->mpc_ActionBendLine->setVisible(true);
         q_Return = true;
         break;
      case ms32_GRAPHICS_ITEM_IMAGE:
         q_Return = true;
         break;
      case ms32_GRAPHICS_ITEM_BOUNDARY:
         this->mpc_ActionSetupStyle->setVisible(true);
         q_Return = true;
         break;
      case ms32_GRAPHICS_ITEM_TEXTELEMENT:
         this->mpc_ActionSetupStyle->setVisible(true);
         this->mpc_ActionEditContent->setVisible(true);
         q_Return = true;
         break;
      case ms32_GRAPHICS_ITEM_DB_LABEL:        // Basic functions of dashboard elements are the same
      case ms32_GRAPHICS_ITEM_DB_SPIN_BOX:     // Basic functions of dashboard elements are the same
      case ms32_GRAPHICS_ITEM_DB_PROGRESS_BAR: // Basic functions of dashboard elements are the same
      case ms32_GRAPHICS_ITEM_DB_SLIDER:       // Basic functions of dashboard elements are the same
      case ms32_GRAPHICS_ITEM_DB_TOGGLE:       // Basic functions of dashboard elements are the same
      case ms32_GRAPHICS_ITEM_DB_PIE_CHART:    // Basic functions of dashboard elements are the same
         // Specific functions will be registered by the items with the function RegisterAction if needed
         this->mpc_ActionEditProperties->setVisible(true);
         q_Return = true;
         break;
      case ms32_GRAPHICS_ITEM_DB_TABLE: // Basic functions of dashboard elements are the same
      case ms32_GRAPHICS_ITEM_DB_PARAM: // Basic functions of dashboard elements are the same
         // Specific functions will be registered by the items with the function RegisterAction if needed
         this->mpc_ActionEditContent->setVisible(true);
         q_Return = true;
         break;
      default:
         break;
      }

      if (q_Return == true)
      {
         // Same actions for all
         this->mpc_ActionCut->setVisible(true);
         this->mpc_ActionCopy->setVisible(true);
         this->mpc_ActionDelete->setVisible(true);
         this->mpc_ActionBringToFront->setVisible(true);
         this->mpc_ActionSendToBack->setVisible(true);
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
   }
   else
   {
      q_Return = this->mq_FurtherActionsWillBeAdded;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the input item type requires a setup style in the context menu

   \param[in] os32_ItemType Item type to check

   \retval   True    Setup style menu is required
   \retval   False   Setup style menu should stay hidden
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaContextMenuManager::m_ItemTypeHasSetupStyle(const int32_t os32_ItemType)
{
   bool q_Retval;

   switch (os32_ItemType)
   {
   case ms32_GRAPHICS_ITEM_LINE_ARROW:
   case ms32_GRAPHICS_ITEM_BOUNDARY:
   case ms32_GRAPHICS_ITEM_TEXTELEMENT:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Emit edit properties signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::m_EditProperties(void)
{
   Q_EMIT (this->SigEditProperties(this->mpc_ActiveItem));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Emit edit content signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaContextMenuManager::m_EditContent(void)
{
   Q_EMIT (this->SigEditContent(this->mpc_ActiveItem));
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
