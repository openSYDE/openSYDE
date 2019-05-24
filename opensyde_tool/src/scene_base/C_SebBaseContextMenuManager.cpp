//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Context menu manager of base scene (implementation)

   Context menu manager of base scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"

#include "C_SebBaseContextMenuManager.h"

#include "C_GtGetText.h"
#include "gitypes.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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
C_SebBaseContextMenuManager::C_SebBaseContextMenuManager() :
   mpc_ActiveItem(NULL)
{
   // add all actions
   this->mpc_ActionCut = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                           "Cut"), this, &C_SebBaseContextMenuManager::m_Cut,
                                                        static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_X));

   this->mpc_ActionCopy = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                            "Copy"), this, &C_SebBaseContextMenuManager::m_Copy,
                                                         static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_C));

   this->mpc_ActionCopySeparator = this->mc_ContextMenu.addSeparator();

   this->mpc_ActionPaste = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                             "Paste"), this, &C_SebBaseContextMenuManager::m_Paste,
                                                          static_cast<sintn>(Qt::CTRL) + static_cast<sintn>(Qt::Key_V));

   this->mpc_ActionSetupStyle = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                  "Setup Style"), this,
                                                               &C_SebBaseContextMenuManager::m_SetupStyle);
   this->mpc_ActionBendLine = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                "Bend Line at cursor"), this,
                                                             &C_SebBaseContextMenuManager::m_BendLine);

   this->mpc_ActionRemoveBendLine = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                      "Straighten line at cursor"),
                                                                   this,
                                                                   &C_SebBaseContextMenuManager::m_RemoveBendLine);

   //Alignment
   this->mc_AlignmentContextMenu.setTitle(C_GtGetText::h_GetText("Alignment"));
   this->mpc_ActionAlignment = this->mc_ContextMenu.addMenu(&mc_AlignmentContextMenu);

   // init and connect alignment actions
   this->mpc_ActionAlignLeft = this->mc_AlignmentContextMenu.addAction(C_GtGetText::h_GetText(
                                                                          "Align Left"), this,
                                                                       &C_SebBaseContextMenuManager::m_AlignLeft);

   this->mpc_ActionAlignHorizontalCenter =
      this->mc_AlignmentContextMenu.addAction(C_GtGetText::h_GetText("Align Horizontal Center"), this,
                                              &C_SebBaseContextMenuManager::m_AlignHorizontalCenter);

   this->mpc_ActionAlignRight = this->mc_AlignmentContextMenu.addAction(C_GtGetText::h_GetText("Align Right"), this,
                                                                        &C_SebBaseContextMenuManager::m_AlignRight);

   this->mc_AlignmentContextMenu.addSeparator();

   this->mpc_ActionAlignTop = this->mc_AlignmentContextMenu.addAction(C_GtGetText::h_GetText("Align Top"), this,
                                                                      &C_SebBaseContextMenuManager::m_AlignTop);

   this->mpc_ActionAlignVerticalCenter =
      this->mc_AlignmentContextMenu.addAction(C_GtGetText::h_GetText("Align Vertical Center"), this,
                                              &C_SebBaseContextMenuManager::m_AlignVerticalCenter);

   this->mpc_ActionAlignBottom = this->mc_AlignmentContextMenu.addAction(C_GtGetText::h_GetText("Align Bottom"), this,
                                                                         &C_SebBaseContextMenuManager::m_AlignBottom);

   //Z order
   this->mc_OrderObjectContextMenu.setTitle(C_GtGetText::h_GetText("Z-Order"));
   this->mpc_ActionOrderObjects = this->mc_ContextMenu.addMenu(&mc_OrderObjectContextMenu);

   // init and connect order objects actions
   this->mpc_ActionBringToFront = this->mc_OrderObjectContextMenu.addAction(C_GtGetText::h_GetText(
                                                                               "Bring to Front"), this,
                                                                            &C_SebBaseContextMenuManager::SigBringToFront);

   this->mpc_ActionBringForward = this->mc_OrderObjectContextMenu.addAction(C_GtGetText::h_GetText(
                                                                               "Bring Forward"), this,
                                                                            &C_SebBaseContextMenuManager::SigBringForward);

   this->mpc_ActionSendBackward = this->mc_OrderObjectContextMenu.addAction(C_GtGetText::h_GetText(
                                                                               "Send Backward"), this,
                                                                            &C_SebBaseContextMenuManager::SigSendBackward);

   this->mpc_ActionSendToBack = this->mc_OrderObjectContextMenu.addAction(C_GtGetText::h_GetText(
                                                                             "Send to Back"), this,
                                                                          &C_SebBaseContextMenuManager::SigSendToBack);

   this->mpc_ActionDeleteSeparator = this->mc_ContextMenu.addSeparator();

   this->mpc_ActionDelete = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText("Delete"),
                                                           this, &C_SebBaseContextMenuManager::m_Delete,
                                                           static_cast<sintn>(Qt::Key_Delete));

   // connect signal to detect closing the context menu
   connect(&this->mc_ContextMenu, &C_OgeContextMenu::aboutToHide,
           this, &C_SebBaseContextMenuManager::m_ContextMenuClosed);

   m_InsertBendLineActions(this->mpc_ActionOrderObjects);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebBaseContextMenuManager::~C_SebBaseContextMenuManager()
{
   //lint -e{1540}  no memory leak because of the parent of all mpc_Action* and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handles a context menu event and shows the context menu with the necessary actions

   \param[in]     opc_Event            Event identification and information
   \param[in]     orc_SelectedItems    All selected and for the context menu relevant items
   \param[in]     orq_ShowPaste        Indicator if paste functionality is available
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::HandleContextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event,
                                                         const QList<QGraphicsItem *> & orc_SelectedItems,
                                                         const bool & orq_ShowPaste)
{
   bool q_ShowMenu = false;

   // reset all previous configurations
   this->mpc_ActiveItem = NULL;
   // set all actions invisible
   this->m_SetActionsInvisible();

   // save position
   this->mc_ScenePos = opc_Event->scenePos();

   if (orc_SelectedItems.size() == 0)
   {
      // no element was selected
      if (orq_ShowPaste == true)
      {
         this->mpc_ActionPaste->setVisible(true);
         q_ShowMenu = true;
      }
   }
   else if (orc_SelectedItems.size() == 1)
   {
      // save the parent of the selected item or selected item
      this->mpc_ActiveItem = C_SebUtil::h_GetHighestParent(orc_SelectedItems[0]);

      // one element was selected. specific functionality is probably available
      q_ShowMenu = this->m_ActivateSpecificActions();
   }
   else
   {
      // special case: text element and its subelement
      if ((orc_SelectedItems.size() == 2) &&
          (((orc_SelectedItems[0]->type() == msn_GRAPHICS_ITEM_TEXTELEMENT) &&
            (orc_SelectedItems[1]->type() == msn_GRAPHICS_ITEM_TEXT)) ||
           ((orc_SelectedItems[1]->type() == msn_GRAPHICS_ITEM_TEXTELEMENT) &&
            (orc_SelectedItems[0]->type() == msn_GRAPHICS_ITEM_TEXT))))
      {
         // in this case the text element is in the edit mode and the item context menu functionality shall not be
         // available
         q_ShowMenu = false;
      }
      else
      {
         float64 f64_CurZ = std::numeric_limits<float64>::max() * -1.0;
         // more than one element was selected. only common functionality is available
         this->mpc_ActionCut->setVisible(true);
         this->mpc_ActionCopy->setVisible(true);
         this->mpc_ActionDelete->setVisible(true);
         this->mpc_ActionOrderObjects->setVisible(true);
         this->mpc_ActionAlignment->setVisible(true);
         q_ShowMenu = true;
         //Get current guideline item for alignment
         for (sint32 s32_ItItem = 0; s32_ItItem < orc_SelectedItems.size(); ++s32_ItItem)
         {
            QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(orc_SelectedItems[s32_ItItem]);
            if (pc_CurItem->isUnderMouse() == true)
            {
               if (f64_CurZ < pc_CurItem->zValue())
               {
                  this->mpc_ActiveItem = pc_CurItem;
                  f64_CurZ = pc_CurItem->zValue();
               }
            }
         }
         //Check for common setup style (only makes sense for more than one selected item)
         if (orc_SelectedItems.size() > 1)
         {
            const QGraphicsItem * const pc_FirstItem = C_SebUtil::h_GetHighestParent(orc_SelectedItems[0]);
            if (pc_FirstItem != NULL)
            {
               bool q_AllEqual = true;
               for (sint32 s32_ItItem = 0; s32_ItItem < orc_SelectedItems.size(); ++s32_ItItem)
               {
                  const QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(orc_SelectedItems[s32_ItItem]);
                  if (pc_CurItem->type() != pc_FirstItem->type())
                  {
                     q_AllEqual = false;
                     break;
                  }
               }
               if (q_AllEqual)
               {
                  if (m_ItemTypeHasSetupStyle(pc_FirstItem->type()))
                  {
                     this->mpc_ActionSetupStyle->setVisible(true);
                  }
               }
            }
         }
      }
   }

   if ((q_ShowMenu == true) && (this->mc_ContextMenu.isEmpty() == false))
   {
      // show the menu
      this->mc_ContextMenu.popup(opc_Event->screenPos());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_SetActionsInvisible(void)
{
   this->mpc_ActionCut->setVisible(false);
   this->mpc_ActionCopy->setVisible(false);
   this->mpc_ActionPaste->setVisible(false);
   this->mpc_ActionDelete->setVisible(false);
   this->mpc_ActionSetupStyle->setVisible(false);
   this->mpc_ActionOrderObjects->setVisible(false);
   this->mpc_ActionAlignment->setVisible(false);
   this->mpc_ActionBendLine->setVisible(false);
   this->mpc_ActionRemoveBendLine->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if the input item type requires a setup style in the context menu

   \param[in] osn_ItemType Item type to check

   \retval   True    Setup style menu is required
   \retval   False   Setup style menu should stay hidden
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebBaseContextMenuManager::m_ItemTypeHasSetupStyle(const sintn osn_ItemType)
{
   Q_UNUSED(osn_ItemType)
   return false;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_ContextMenuClosed(void)
{
   Q_EMIT this->SigContextMenuClosed();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert bend line actions after specified item

   \param[in,out] opc_Action Add actions after this action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_InsertBendLineActions(QAction * const opc_Action)
{
   this->mc_ContextMenu.removeAction(this->mpc_ActionBendLine);
   this->mc_ContextMenu.removeAction(this->mpc_ActionRemoveBendLine);
   this->mc_ContextMenu.insertAction(opc_Action, this->mpc_ActionBendLine);
   this->mc_ContextMenu.insertAction(opc_Action, this->mpc_ActionRemoveBendLine);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_Cut(void)
{
   Q_EMIT this->SigCut();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_Copy(void)
{
   Q_EMIT this->SigCopy();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_Paste(void)
{
   Q_EMIT this->SigPaste();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_Delete(void)
{
   Q_EMIT this->SigDelete();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_SetupStyle()
{
   Q_EMIT this->SigSetupStyle(this->mpc_ActiveItem);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_BendLine(void)
{
   Q_EMIT this->SigBendLine(this->mpc_ActiveItem, this->mc_ScenePos);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_RemoveBendLine(void)
{
   Q_EMIT this->SigRemoveBendLine(this->mpc_ActiveItem, this->mc_ScenePos);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Align left
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_AlignLeft(void)
{
   Q_EMIT this->SigAlign(this->mpc_ActiveItem, eAL_LEFT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Align horizontal center
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_AlignHorizontalCenter(void)
{
   Q_EMIT this->SigAlign(this->mpc_ActiveItem, eAL_HORIZONTAL_CENTER);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Align right
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_AlignRight(void)
{
   Q_EMIT this->SigAlign(this->mpc_ActiveItem, eAL_RIGHT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Align top
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_AlignTop(void)
{
   Q_EMIT this->SigAlign(this->mpc_ActiveItem, eAL_TOP);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Align vertical center
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_AlignVerticalCenter(void)
{
   Q_EMIT this->SigAlign(this->mpc_ActiveItem, eAL_VERTICAL_CENTER);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Align bottom
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseContextMenuManager::m_AlignBottom(void)
{
   Q_EMIT this->SigAlign(this->mpc_ActiveItem, eAL_BOTTOM);
}
