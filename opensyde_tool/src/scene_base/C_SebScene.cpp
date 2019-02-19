//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Common graphics scene with basic functionality (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <QFileDialog>
#include <QImageReader>
#include <QElapsedTimer>

#include "stwtypes.h"
#include "gitypes.h"
#include "constants.h"

#include "C_SebScene.h"

#include "C_GtGetText.h"
#include "C_GiBiCustomMouseItem.h"
#include "C_GiUnique.h"
#include "TGLUtils.h"
#include "C_GiCustomFunctions.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_tgl;

/* -- Module Global Constants ---------------------------------------------- */
const float64 mf64_SCENE_RECT_BORDER = 0.0; //SSI: no border needed so far
const QString C_SebScene::mhc_BOUNDARY = "Boundary";
const QString C_SebScene::mhc_TEXT_ELEMENT = "Text element";
const QString C_SebScene::mhc_LINE = "Line/Arrow";
const QString C_SebScene::mhc_IMAGE = "Image";
const float64 C_SebScene::mhf64_MovingRange = 10.0;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SebScene::C_SebScene(QObject * const opc_Parent) :
   QGraphicsScene(opc_Parent),
   mq_BlockContextMenu(false),
   mq_ProxyWidgetInteractionActive(false),
   mpc_CurrentHoverItem(NULL),
   mu64_LastUnusedUniqueID(0),
   mf64_ZOrderHigh(mf64_ZORDER_INIT_HIGH),
   mf64_ZOrderLow(mf64_ZORDER_INIT_LOW),
   mq_RubberBandActive(false),
   mq_LeftButtonPressed(false),
   mq_DrawCustomBackground(true),
   mq_DarkModeActive(false),
   mc_LastKnownMouseScenePosition(0.0, 0.0)
{
   connect(this, &QGraphicsScene::changed, this, &C_SebScene::m_AdaptSceneRect);

   // configure background drawing
   this->mc_ImgDrawBackground.load(":images/DrawBackground.png");
   this->mc_ImgDrawBackgroundDark.load("://images/DrawBackgroundDark.png");
   m_HandleBackground();

   //Timers
   this->mc_ToolTipTimer.setInterval(static_cast<sintn>(ms32_TOOL_TIP_DELAY));
   this->mc_ToolTipTimer.setSingleShot(true);
   connect(&mc_ToolTipTimer, &QTimer::timeout, this, &C_SebScene::m_TriggerToolTip);

   connect(this, &C_SebScene::selectionChanged, this, &C_SebScene::m_OnSelectionChange);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     10.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SebScene::~C_SebScene()
{
   //lint -e{1540}  no memory leak because of the parents of the items and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the drawing of the background active

   \param[in]  oq_Active   Flag for drawing the background

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::SetDrawingBackground(const bool oq_Active)
{
   this->mq_DrawCustomBackground = oq_Active;
   m_HandleBackground();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the minimum required scene rectangle size for all elements

   \return
   Minimum scene rectangle size

   \created     23.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QRectF C_SebScene::GetMinimumSceneRect(void) const
{
   return mc_MinimumSceneRect;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activate blocking of the context menu for one call

   \created     26.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::BlockContextMenu(void)
{
   this->mq_BlockContextMenu = true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check scene if tool tip for current position is necessary

   \param[in] orc_ScenePos Scene position to check for tool tip

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::DisplayToolTip(const QPointF & orc_ScenePos)
{
   //Check if item has tool tip
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   //lint -e{740}  no problem because of common base class
   const C_GiBiCustomToolTip * pc_ToolTip =
      dynamic_cast<C_GiBiCustomToolTip *>(C_SebUtil::h_GetHighestParent(this->itemAt(orc_ScenePos,
                                                                                     QTransform())));

   if (pc_ToolTip != NULL)
   {
      //Do not reopen tool tip if there already is an active tool tip
      if (mpc_CurrentHoverItem != pc_ToolTip)
      {
         //Update tool tip
         m_UpdateHints();
         if (pc_ToolTip->NotEmpty())
         {
            mpc_CurrentHoverItem = pc_ToolTip;
            Q_EMIT this->SigShowToolTip(orc_ScenePos, pc_ToolTip->GetCurrentToolTipHeading(),
                                        pc_ToolTip->GetCurrentToolTipContent(), pc_ToolTip->GetCurrentToolTipType());
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check scene if tool tip for current position is necessary

   Display tool tip for set object

   \param[in] orc_ScenePos Scene position to check for tool tip

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::DisplaySpecificItemToolTip(const QPointF & orc_ScenePos)
{
   if (this->mpc_CurrentHoverItem != NULL)
   {
      //Update tool tip
      m_UpdateHints();
      if (this->mpc_CurrentHoverItem->NotEmpty())
      {
         Q_EMIT this->SigShowToolTip(orc_ScenePos, this->mpc_CurrentHoverItem->GetCurrentToolTipHeading(),
                                     this->mpc_CurrentHoverItem->GetCurrentToolTipContent(),
                                     this->mpc_CurrentHoverItem->GetCurrentToolTipType());
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Checks if the cursor position is relevant for the proxy widget

   In base is no item relevant. Function returns always false.
   Derived classes must overwrite the function if needed.

   \param[in]     orc_ScenePos     Scene position of cursor

   \return
   true     Cursor is on a relevant position
   false    Cursor is not on a relevant position

   \created     04.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SebScene::IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos)
{
   Q_UNUSED(orc_ScenePos)

   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Signal for update of current scaling

   \param[in] orc_Transform Current scaling

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::UpdateTransform(const QTransform & orc_Transform)
{
   QList<QGraphicsItem *> c_Items = this->items();
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      C_GiBiRectBaseGroup * pc_Rect;
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_GiLiLineGroup * const pc_Line = dynamic_cast<C_GiLiLineGroup *>(pc_Parent);
      if (pc_Line != NULL)
      {
         pc_Line->UpdateTransform(orc_Transform);
      }
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      pc_Rect = dynamic_cast<C_GiBiRectBaseGroup *>(pc_Parent);
      if (pc_Rect != NULL)
      {
         pc_Rect->UpdateTransform(orc_Transform);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dark mode active

   \param[in] oq_Value Dark mode active

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::SetDarkModeActive(const bool oq_Value)
{
   this->mq_DarkModeActive = oq_Value;
   m_HandleBackground();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add bend point to line

   \param[in,out] opc_Item     Line item
   \param[in]     orc_ScenePos Scene position
   \param[in]     opc_Index    Optional specific index to add point add

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SebScene::BendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos,
                            const stw_types::sint32 * const opc_Index)
{
   sint32 s32_Retval = -1;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiLiLineGroup * const pc_Line = dynamic_cast<C_GiLiLineGroup *>(opc_Item);

   if (pc_Line != NULL)
   {
      s32_Retval = pc_Line->BendLine(orc_ScenePos, opc_Index);
      Q_EMIT this->SigTriggerUpdateTransform();
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Remove bend point of line

   \param[in,out] opc_Item     Line item
   \param[in]     orc_ScenePos Scene position
   \param[in]     opc_Index    Optional specific index which point to remove

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_SebScene::RemoveBendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos,
                                  const sint32 * const opc_Index) const
{
   sint32 s32_Retval = -1;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiLiLineGroup * const pc_Line = dynamic_cast<C_GiLiLineGroup *>(opc_Item);

   if (pc_Line != NULL)
   {
      s32_Retval = pc_Line->RemoveBend(orc_ScenePos, opc_Index);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get item identified by unique item ID

   \param[in] oru64_ID Unique item ID

   \return
   NULL: not found
   else: pointer to item

   \created     21.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QGraphicsItem * C_SebScene::GetItemByID(const uint64 & oru64_ID) const
{
   QGraphicsItem * pc_Retval = NULL;
   QGraphicsItem * pc_CurItemParent;
   C_GiUnique * pc_Unique;

   QList<QGraphicsItem *> c_Items = this->items();

   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end(); ++c_ItItem)
   {
      pc_CurItemParent = C_SebUtil::h_GetHighestParent(*c_ItItem);
      if (pc_CurItemParent != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         //lint -e{740}  no problem because of common base class
         pc_Unique = dynamic_cast<C_GiUnique *>(pc_CurItemParent);
         if (pc_Unique != NULL)
         {
            if (pc_Unique->CheckMatch(oru64_ID) == true)
            {
               pc_Retval = pc_CurItemParent;
            }
         }
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
void C_SebScene::m_InitSceneContextMenuManager(void)
{
   // can not be initialized in the constructor because of usage of pure virtual functions
   // configure context menu
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigBringToFront,
           this, &C_SebScene::m_BringToFront);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigBringForward,
           this, &C_SebScene::m_BringForward);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigSendBackward,
           this, &C_SebScene::m_SendBackward);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigSendToBack,
           this, &C_SebScene::m_SendToBack);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigAlign,
           this, &C_SebScene::m_Align);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigDelete,
           this, &C_SebScene::m_DeleteSlot);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigPaste,
           this, &C_SebScene::m_PasteClick);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigCopy,
           this, &C_SebScene::m_Copy);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigCut,
           this, &C_SebScene::m_Cut);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigBendLine,
           this, &C_SebScene::m_BendLine);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigRemoveBendLine,
           this, &C_SebScene::m_RemoveBendLine);
   connect(this->m_GetContextMenuManager(), &C_SebBaseContextMenuManager::SigSetupStyle,
           this, &C_SebScene::m_SetupStyle);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press release event slot

   \param[in,out] opc_KeyEvent  Key event identification and information

   \created     24.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOriginal = true;
   const bool q_ShiftModifier = C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ShiftModifier);
   const bool q_AltModifier = C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::AltModifier);
   const bool q_CtrlModifier = C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier);

   if (this->mq_ProxyWidgetInteractionActive == false)
   {
      //Debug commands
      if ((q_ShiftModifier == true) &&
          (q_CtrlModifier == true) &&
          (q_AltModifier == true))
      {
         switch (opc_KeyEvent->key())
         {
         case Qt::Key_O:
            //Undo redo command view
            if (this->m_GetUndoManager() != NULL)
            {
               this->m_GetUndoManager()->ToggleCommandDisplay();
            }
            break;
         default:
            break;
         }
      }
      //Debug commands end

      //alt
      if (q_AltModifier == true)
      {
         if (q_ShiftModifier == true)
         {
            switch (opc_KeyEvent->key())
            {
            case Qt::Key_Backspace:
               if ((this->m_IsUndoAvailable() == true) && (this->m_GetUndoManager() != NULL))
               {
                  // redo
                  this->m_GetUndoManager()->DoRedo();
               }
               break;
            default:
               break;
            }
         }
         else
         {
            switch (opc_KeyEvent->key())
            {
            case Qt::Key_Backspace:
               if ((this->m_IsUndoAvailable() == true) && (this->m_GetUndoManager() != NULL))
               {
                  // undo
                  this->m_GetUndoManager()->DoUndo();
               }
               break;
            default:
               break;
            }
         }
      }

      //shift
      if (q_ShiftModifier == true)
      {
         switch (opc_KeyEvent->key())
         {
         case Qt::Key_Delete:
            // cut
            this->m_Cut();
            break;
         case Qt::Key_Insert:
            // paste
            this->m_PasteKey();
            break;
         default:
            break;
         }
      }

      //ctrl
      if (q_CtrlModifier == true)
      {
         switch (opc_KeyEvent->key())
         {
         case Qt::Key_Z:
            if ((this->m_IsUndoAvailable() == true) && (this->m_GetUndoManager() != NULL))
            {
               // undo
               this->m_GetUndoManager()->DoUndo();
            }
            break;
         case Qt::Key_Y:
            if ((this->m_IsUndoAvailable() == true) && (this->m_GetUndoManager() != NULL))
            {
               // redo
               this->m_GetUndoManager()->DoRedo();
            }
            break;
         case Qt::Key_X:
            // cut
            this->m_Cut();
            break;
         case Qt::Key_Insert:
         case Qt::Key_C:
            // copy
            this->m_Copy();
            break;
         case Qt::Key_V:
            // paste
            this->m_PasteKey();
            break;
         case Qt::Key_A:
            // select all items
            this->m_SelectAll();
            break;
         // Moving of items
         case Qt::Key_Left:
            this->m_MoveSelectedItems(QPointF(-1.0, 0.0));
            if (this->m_GetUndoManager() != NULL)
            {
               this->m_GetUndoManager()->RegisterMove();
            }
            q_CallOriginal = false;
            break;
         case Qt::Key_Right:
            this->m_MoveSelectedItems(QPointF(1.0, 0.0));
            if (this->m_GetUndoManager() != NULL)
            {
               this->m_GetUndoManager()->RegisterMove();
            }
            q_CallOriginal = false;
            break;
         case Qt::Key_Up:
            this->m_MoveSelectedItems(QPointF(0.0, -1.0));
            if (this->m_GetUndoManager() != NULL)
            {
               this->m_GetUndoManager()->RegisterMove();
            }
            q_CallOriginal = false;
            break;
         case Qt::Key_Down:
            this->m_MoveSelectedItems(QPointF(0.0, 1.0));
            if (this->m_GetUndoManager() != NULL)
            {
               this->m_GetUndoManager()->RegisterMove();
            }
            q_CallOriginal = false;
            break;
         default:
            break;
         }
      }
      else
      {
         switch (opc_KeyEvent->key())
         {
         case Qt::Key_Delete:
            this->m_Delete();
            break;
         // Moving of items
         case Qt::Key_Left:
            //Only block key event if useful
            if (this->selectedItems().size() > 0)
            {
               this->m_MoveSelectedItems(QPointF(-mhf64_MovingRange, 0.0));
               if (this->m_GetUndoManager() != NULL)
               {
                  this->m_GetUndoManager()->RegisterMove();
               }
               q_CallOriginal = false;
            }
            break;
         case Qt::Key_Right:
            //Only block key event if useful
            if (this->selectedItems().size() > 0)
            {
               this->m_MoveSelectedItems(QPointF(mhf64_MovingRange, 0.0));
               if (this->m_GetUndoManager() != NULL)
               {
                  this->m_GetUndoManager()->RegisterMove();
               }
               q_CallOriginal = false;
            }
            break;
         case Qt::Key_Up:
            //Only block key event if useful
            if (this->selectedItems().size() > 0)
            {
               this->m_MoveSelectedItems(QPointF(0.0, -mhf64_MovingRange));
               if (this->m_GetUndoManager() != NULL)
               {
                  this->m_GetUndoManager()->RegisterMove();
               }
               q_CallOriginal = false;
            }
            break;
         case Qt::Key_Down:
            //Only block key event if useful
            if (this->selectedItems().size() > 0)
            {
               this->m_MoveSelectedItems(QPointF(0.0, mhf64_MovingRange));
               if (this->m_GetUndoManager() != NULL)
               {
                  this->m_GetUndoManager()->RegisterMove();
               }
               q_CallOriginal = false;
            }
            break;
         default:
            break;
         }
      }
   }

   if (q_CallOriginal == true)
   {
      QGraphicsScene::keyPressEvent(opc_KeyEvent);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten focus out event slot

   Here: Remove selection of items

   \param[in,out] opc_Event Event identification and information

   \created     30.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::focusOutEvent(QFocusEvent * const opc_Event)
{
   if (opc_Event->reason() != Qt::PopupFocusReason)
   {
      this->clearSelection();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag move event

   \param[in,out] opc_Event Event identification and information

   \created     11.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::dragMoveEvent(QGraphicsSceneDragDropEvent * const opc_Event)
{
   if (mq_ProxyWidgetInteractionActive == false)
   {
      if (this->IsAnyItemAddable() == true)
      {
         if (opc_Event->mimeData()->hasFormat("text/plain") == true)
         {
            opc_Event->acceptProposedAction();
         }
         else if (opc_Event->mimeData()->hasUrls() == true)
         {
            // only image files are allowed
            QString c_Text = opc_Event->mimeData()->text();
            if (C_SebUtil::h_CheckFilePathForImage(c_Text) == true)
            {
               opc_Event->acceptProposedAction();
            }
            else
            {
               QGraphicsScene::dragMoveEvent(opc_Event);
            }
         }
         else
         {
            QGraphicsScene::dragMoveEvent(opc_Event);
         }
      }
      else
      {
         QGraphicsScene::dragMoveEvent(opc_Event);
      }
   }
   else
   {
      QGraphicsScene::dragMoveEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drop event

   \param[in,out] opc_Event Event identification and information

   \created     10.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::dropEvent(QGraphicsSceneDragDropEvent * const opc_Event)
{
   if (mq_ProxyWidgetInteractionActive == false)
   {
      if (this->IsAnyItemAddable() == true)
      {
         if (m_AddOfMime(opc_Event->mimeData(), opc_Event->scenePos()) == true)
         {
            opc_Event->acceptProposedAction();
         }
      }
   }
   QGraphicsScene::dropEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   \param[in,out] opc_Event Event identification and information

   \created     25.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (mq_ProxyWidgetInteractionActive == false)
   {
      if ((mq_LeftButtonPressed == true) && (this->selectedItems().size() > 1))
      {
         const QPointF c_CurMouseScenePosition = opc_Event->scenePos();
         this->m_MoveSelectedItems(c_CurMouseScenePosition - this->mc_LastKnownMouseScenePosition);
         this->mc_LastKnownMouseScenePosition = c_CurMouseScenePosition;
      }
      else
      {
         QGraphicsScene::mouseMoveEvent(opc_Event);
      }
   }
   else
   {
      QGraphicsScene::mouseMoveEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information

   \created     09.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (mq_ProxyWidgetInteractionActive == false)
   {
      if (opc_Event->button() == Qt::RightButton)
      {
         if (this->IsSceneRubberBandAvailable() == true)
         {
            // select item if not selected yet
            QList<QGraphicsItem *>::const_iterator c_ItItem;
            const QList<QGraphicsItem *> & rc_Items = this->items();
            QGraphicsItem * pc_Item;
            bool q_ItemFound = false;

            // search a item which lies under the cursor
            for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
            {
               if ((*c_ItItem)->isUnderMouse() == true)
               {
                  pc_Item = C_SebUtil::h_GetHighestParent(*c_ItItem);

                  if (pc_Item != NULL)
                  {
                     // check element
                     if (pc_Item->isSelected() == false)
                     {
                        if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == false)
                        {
                           // if the ctrl key was not pressed and the item is not already selected, it is a new
                           // selection
                           this->m_UpdateSelection(pc_Item, true);
                        }
                        else
                        {
                           this->m_UpdateSelection(pc_Item);
                        }
                     }

                     q_ItemFound = true;
                  }
                  break;
               }
            }
            if (q_ItemFound == false)
            {
               this->clearSelection();
               QGraphicsScene::mousePressEvent(opc_Event);
            }
         }
         else
         {
            QGraphicsScene::mousePressEvent(opc_Event);
         }
      }
      else
      {
         if (opc_Event->button() == Qt::LeftButton)
         {
            QList<QGraphicsItem *>::const_iterator c_ItItem;
            const QList<QGraphicsItem *> & rc_Items = this->items();

            this->mq_RubberBandActive = true;

            // search a item which lies under the cursor
            for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
            {
               const QGraphicsItem * const pc_Item = C_SebUtil::h_GetHighestParent(*c_ItItem);
               if (pc_Item->isUnderMouse() == true)
               {
                  //Custom rubberband flag
                  this->mq_RubberBandActive = false;
                  this->mq_LeftButtonPressed = true;
                  mc_LastKnownMouseScenePosition = opc_Event->scenePos();
                  break;
               }
            }
         }
         QGraphicsScene::mousePressEvent(opc_Event);
      }
   }
   else
   {
      QGraphicsScene::mousePressEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse release event slot

   \param[in,out] opc_Event Event identification and information

   \created     25.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   QGraphicsScene::mouseReleaseEvent(opc_Event);

   if (mq_ProxyWidgetInteractionActive == false)
   {
      //Custom rubberband flag
      this->mq_RubberBandActive = false;
      this->mq_LeftButtonPressed = false;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten context menu event

   \param[in,out] opc_Event Event identification and information

   \created     02.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event)
{
   if (this->mq_BlockContextMenu == false)
   {
      C_SebBaseContextMenuManager * const pc_ContextMenuManager = this->m_GetContextMenuManager();

      if (pc_ContextMenuManager != NULL)
      {
         pc_ContextMenuManager->HandleContextMenuEvent(opc_Event,
                                                       this->selectedItems(),
                                                       this->m_IsPastePossible());
      }
   }

   this->mq_BlockContextMenu = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new text element to scene and connect signals

   \param[in,out] opc_Item  Text element

   \created     04.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddTextElementToScene(C_GiBiTextElement * const opc_Item)
{
   m_AddRectBaseGroupToScene(opc_Item);
   m_AddAnyItemToScene(opc_Item);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new line arrow to scene and connect signals

   \param[in,out] opc_Item Line arrow item

   \created     27.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddLineArrowToScene(C_GiBiArrow * const opc_Item)
{
   m_AddLineGroupToScene(opc_Item);
   m_AddAnyItemToScene(opc_Item);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new boundary to scene and connect signals

   \param[in,out] opc_Item  Boundary item

   \created     27.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddBoundaryToScene(C_GiBiBoundary * const opc_Item)
{
   m_AddRectBaseGroupToScene(opc_Item);
   m_AddAnyItemToScene(opc_Item);
}
//-----------------------------------------------------------------------------
/*!
   \brief   Add new image to scene and connect signals

   \param[in,out] opc_Item  Image group item

   \created     08.11.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddImageGroupToScene(C_GiBiImageGroup * const opc_Item)
{
   m_AddRectBaseGroupToScene(opc_Item);
   m_AddAnyItemToScene(opc_Item);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new rectangle based item to scene and connect signals

   \param[in,out] opc_Item Rectangle based item

   \created     22.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddRectBaseGroupToScene(C_GiBiRectBaseGroup * const opc_Item)
{
   //Move
   connect(opc_Item, &C_GiBiRectBaseGroup::SigItemWasMoved, this, &C_SebScene::m_HandleRevertableMove);
   //Resize
   connect(opc_Item, &C_GiBiRectBaseGroup::SigItemWasResized, this, &C_SebScene::m_HandleRevertableResizeRectangle);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add new line based item to scene and connect signals

   \param[in,out] opc_Item  Line based item

   \created     22.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddLineGroupToScene(C_GiLiLineGroup * const opc_Item)
{
   //Move
   connect(opc_Item, &C_GiLiLineGroup::SigItemWasMoved, this, &C_SebScene::m_HandleRevertableMove);
   connect(opc_Item, &C_GiLiLineGroup::SigSubItemWasMoved, this, &C_SebScene::m_HandleSubLineMove);
   //Resize
   connect(opc_Item, &C_GiLiLineGroup::SigItemWasResized, this, &C_SebScene::m_HandleRevertableResizeLine);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add any item and do general actions

   \param[in,out] opc_Item Pointer to new item

   \created     19.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddAnyItemToScene(QGraphicsItem * const opc_Item)
{
   this->addItem(opc_Item);
   m_SetItemSelectionAndMoveAvailability(opc_Item);
   //Trigger update of all existing (and new!) interaction points
   Q_EMIT this->SigTriggerUpdateTransform();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set initial state of actions select and move

   \param[in,out] opc_Item Current item

   \created     26.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_SetItemSelectionAndMoveAvailability(QGraphicsItem * const opc_Item) const
{
   if (opc_Item != NULL)
   {
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(opc_Item);

      if (pc_Parent != NULL)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         //lint -e{740}  no problem because of common base class
         C_GiBiCustomMouseItem * const pc_Special = dynamic_cast<C_GiBiCustomMouseItem *>(pc_Parent);
         bool q_Selectable = false;
         bool q_Moveable = false;
         QCursor c_NewCursor;
         if (this->IsItemSelectable(pc_Parent) == true)
         {
            q_Selectable = true;
            c_NewCursor = QCursor(Qt::SizeAllCursor);
         }
         else
         {
            q_Selectable = false;
            c_NewCursor = QCursor(Qt::ArrowCursor);
         }
         pc_Parent->setFlag(QGraphicsItem::ItemIsSelectable, q_Selectable);
         if (pc_Special != NULL)
         {
            pc_Special->SetDefaultCursor(c_NewCursor);
         }
         else
         {
            pc_Parent->setCursor(c_NewCursor);
         }
         if (this->IsItemMovable(pc_Parent) == true)
         {
            q_Moveable = true;
         }
         else
         {
            q_Moveable = false;
         }
         pc_Parent->setFlag(QGraphicsItem::ItemIsMovable, q_Moveable);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle after move action

   \param[in] orc_PositionDifference Position difference of move

   \created     21.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleRevertableMove(const QPointF & orc_PositionDifference)
{
   if (this->m_GetUndoManager() != NULL)
   {
      if (this->selectedItems().size() == 1)
      {
         this->m_GetUndoManager()->RegisterCompleteMoveStep(this->selectedItems(), orc_PositionDifference);
      }
      else
      {
         this->m_GetUndoManager()->RegisterMove();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle after resize action

   \param[in] ors32_InteractionPointIndex Interaction point index
   \param[in] orc_PositionDifference      Position difference of resize

   \created     21.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleRevertableResizeLine(const sint32 & ors32_InteractionPointIndex,
                                              const QPointF & orc_PositionDifference)
{
   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->RegisterResizeLine(this->selectedItems(), ors32_InteractionPointIndex,
                                                   orc_PositionDifference);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle after sub line move action

   \param[in] ors32_SubLineIndex     Sub line index
   \param[in] orc_PositionDifference Position difference of resize

   \created     25.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleSubLineMove(const sint32 & ors32_SubLineIndex, const QPointF & orc_PositionDifference)
{
   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->RegisterMoveSubLine(this->selectedItems(), orc_PositionDifference, ors32_SubLineIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register resize action

   \param[in]     orc_OldPos  Old position
   \param[in]     orc_OldSize Old size
   \param[in]     orc_NewPos  New position
   \param[in]     orc_NewSize New size

   \created     22.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleRevertableResizeRectangle(const QPointF & orc_OldPos, const QSizeF & orc_OldSize,
                                                   const QPointF & orc_NewPos, const QSizeF & orc_NewSize)
{
   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->RegisterResizeRectangle(this->selectedItems(), orc_OldPos, orc_OldSize, orc_NewPos,
                                                        orc_NewSize);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select new item only

   \param[in, out] opc_Item          Item to be selected
   \param[in]      oq_ClearSelection Flag if the old selection have to be cleared

   \created     25.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_UpdateSelection(QGraphicsItem * const opc_Item, const bool oq_ClearSelection)
{
   if (oq_ClearSelection == true)
   {
      this->clearSelection();
   }

   opc_Item->setSelected(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select all items

   \created     16.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_SelectAll(void) const
{
   QList<QGraphicsItem *> c_ListItems = this->items();

   for (QList<QGraphicsItem *>::const_iterator c_ItItem = c_ListItems.begin(); c_ItItem != c_ListItems.end();
        ++c_ItItem)
   {
      QGraphicsItem * pc_Graphics = *c_ItItem;
      if (pc_Graphics != NULL)
      {
         pc_Graphics->setSelected(true);
      }
   }
}

//-----------------------------------------------------------------------------
bool C_SebScene::m_IsRubberBandActive(void) const
{
   return this->mq_RubberBandActive;
}

//-----------------------------------------------------------------------------
/*!
   \brief   React to text interaction mode change

   \param[in] orq_On Flag if text interaction mode is currently active

   \created     12.12.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleProxyWidgetInteractionChange(const bool & orq_On)
{
   this->mq_ProxyWidgetInteractionActive = orq_On;
   if (this->mq_ProxyWidgetInteractionActive == true)
   {
      //deactivate multiselect
   }
   else
   {
      //reactivate multiselect
   }
   Q_EMIT this->SigProxyWidgetInteractionActive(orq_On);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add image to scene

   \param[in] orc_Path     Image path
   \param[in] orc_Position Image scene position

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddImage(const QString & orc_Path, const QPointF & orc_Position)
{
   Q_UNUSED(orc_Path)
   Q_UNUSED(orc_Position)
   //Add image if necessary
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adds a new image

   \param[in] orc_Pos        Position to place item at

   \created     12.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AddImageWithFileDialog(const QPointF & orc_Pos)
{
   const QString c_Filter =
      QString(C_GtGetText::h_GetText("Images (%1)")).arg(C_SebScene::mh_GetImageFileTypesFilter());
   const QString c_Dir = "";
   QString c_File;
   const QList<QGraphicsView *> & rc_Views = this->views();

   // convert coordinates
   tgl_assert(rc_Views.size() > 0);

   c_File = QFileDialog::getOpenFileName(rc_Views[0], C_GtGetText::h_GetText("Load Image"), c_Dir, c_Filter);

   if (c_File.compare("") != 0)
   {
      this->m_AddImage(c_File, orc_Pos);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Map scene position to screen position

   \param[in]       orc_ScenePos    Scene position
   \param[in,out]   orc_GlobalPos   Screen position

   \created     22.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_MapToGlobal(const QPointF & orc_ScenePos, QPoint & orc_GlobalPos) const
{
   const QList<QGraphicsView *> c_Views = this->views();

   // convert coordinates
   tgl_assert(c_Views.size() > 0);
   // adapt the coordinates if zoom was used
   orc_GlobalPos = c_Views[0]->mapFromScene(orc_ScenePos);
   // adapt the coordinates to screen coordinates for the context menu
   orc_GlobalPos = c_Views[0]->mapToGlobal(orc_GlobalPos);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Map screen position to scene position

   \param[in]       orc_GlobalPos   Screen position
   \param[in,out]   orc_ScenePos    Scene position

   \created     22.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_MapFromGlobal(const QPoint & orc_GlobalPos, QPointF & orc_ScenePos) const
{
   const QList<QGraphicsView *> c_Views = this->views();

   // convert coordinates
   tgl_assert(c_Views.size() > 0);
   // adapt the coordinates if zoom was used
   QPoint c_Tmp = c_Views[0]->mapFromGlobal(orc_GlobalPos);
   // adapt the coordinates to screen coordinates for the context menu
   orc_ScenePos = c_Views[0]->mapToScene(c_Tmp);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete selected items

   \created     16.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_Delete(const bool oq_NoUserConfirm)
{
   QList<QGraphicsItem *> c_SelectedItems = this->selectedItems();

   if (c_SelectedItems.size() > 0)
   {
      bool q_Continue;

      if (oq_NoUserConfirm == true)
      {
         q_Continue = true;
      }
      else
      {
         //Ask for user confirmation
         q_Continue = m_HandleDeleteUserConfirmation(c_SelectedItems);
      }
      if (q_Continue == true)
      {
         QList<QGraphicsItem *>::iterator c_ItItem = c_SelectedItems.begin();
         //Reset iterator
         // check for not deletable items
         while (c_ItItem != c_SelectedItems.end())
         {
            if (this->IsItemDeletable(*c_ItItem) == false)
            {
               // remove item from list
               c_ItItem = c_SelectedItems.erase(c_ItItem);
            }
            else
            {
               ++c_ItItem;
            }
         }

         if (this->m_GetUndoManager() != NULL)
         {
            this->m_GetUndoManager()->DoDelete(c_SelectedItems);
         }
      }
      else
      {
         //Restore original focus
         for (QList<QGraphicsItem *>::iterator c_ItItem = c_SelectedItems.begin(); c_ItItem != c_SelectedItems.end();
              ++c_ItItem)
         {
            QGraphicsItem * const pc_CurItem = C_SebUtil::h_GetHighestParent(*c_ItItem);
            if (this->IsItemSelectable(pc_CurItem) == true)
            {
               pc_CurItem->setSelected(true);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear all items

   Hint: leaves all items which are not included in the system definition

   \created     16.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_Clear(void)
{
   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->DoDelete(this->items());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete line based item of scene and disconnect signals

   \param[in,out] opc_Item  Line based item

   \created     22.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_RemoveLineGroupOfScene(C_GiLiLineGroup * const opc_Item)
{
   //Move
   disconnect(opc_Item, &C_GiLiLineGroup::SigItemWasMoved, this, &C_SebScene::m_HandleRevertableMove);
   disconnect(opc_Item, &C_GiLiLineGroup::SigSubItemWasMoved, this, &C_SebScene::m_HandleSubLineMove);
   //Resize
   disconnect(opc_Item, &C_GiLiLineGroup::SigItemWasResized, this, &C_SebScene::m_HandleRevertableResizeLine);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete rectangle based item of scene and disconnect signals

   \param[in,out] opc_Item Rectangle based item

   \created     22.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_RemoveRectBaseGroupOfScene(C_GiBiRectBaseGroup * const opc_Item)
{
   //Move
   disconnect(opc_Item, &C_GiBiRectBaseGroup::SigItemWasMoved, this, &C_SebScene::m_HandleRevertableMove);
   //Resize
   disconnect(opc_Item, &C_GiBiRectBaseGroup::SigItemWasResized, this,
              &C_SebScene::m_HandleRevertableResizeRectangle);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste origin items

   \param[in] opc_Pos   Optional position where items shall be paste

   \created     01.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_Paste(const QPointF * const opc_Pos)
{
   if (this->m_IsPastePossible() == true)
   {
      CopyFromManagerToScene(opc_Pos);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if paste is possible

   Warning: also has to check copy paste manager!

   \return
   True  Valid
   False Invalid

   \created     11.12.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SebScene::m_IsPastePossible(void)
{
   bool q_Retval = false;
   C_SebBaseCopyPasteManager * const pc_CopyPasteManager = this->m_GetCopyPasteManager();

   if (pc_CopyPasteManager != NULL)
   {
      q_Retval = pc_CopyPasteManager->CheckValidContentAndPrepareData();
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if undo currently possible

   \return
   True  Undo possible
   False Undo impossible

   \created     28.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SebScene::m_IsUndoAvailable(void) const
{
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional check for user confirmation on delete action

   \return
   true  Continue
   false Abort

   \created     12.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SebScene::m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const
{
   Q_UNUSED(orc_SelectedItems)
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste objects of clipboard

   \param[in] opc_Pos Optional position to paste at (Otherwise current mouse cursor position is chosen)

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_PasteOfClipBoard(const QPointF * const opc_Pos)
{
   //Default no action
   Q_UNUSED(opc_Pos)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update hints to reality

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_UpdateHints(void) const
{
   C_GiBiCustomToolTip * pc_CustomToolTip;

   QList<QGraphicsItem *> c_Items = this->items();

   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
        ++c_ItItem)
   {
      try
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         //lint -e{740}  no problem because of common base class
         pc_CustomToolTip = dynamic_cast<C_GiBiCustomToolTip *>(C_SebUtil::h_GetHighestParent(*c_ItItem));
         if (pc_CustomToolTip != NULL)
         {
            pc_CustomToolTip->GenerateHint();
         }
      }
      catch (...)
      {
         //Cast failed
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle tool tip hide event

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleHideToolTip(void)
{
   mpc_CurrentHoverItem = NULL;
   mc_ToolTipTimer.stop();
   Q_EMIT this->SigHideToolTip();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Start timer for possible tool tip

   \param[in] orc_ScenePos Scene position to start tool tip at
   \param[in] opc_Item     Item to display tooltip for

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_StartTimerForToolTip(const QPointF & orc_ScenePos, const C_GiBiCustomToolTip * const opc_Item)
{
   this->mc_ToolTipPos = orc_ScenePos;
   mpc_CurrentHoverItem = opc_Item;
   this->mc_ToolTipTimer.start();
}

//-----------------------------------------------------------------------------
void C_SebScene::m_StartTimerForToolTipDirect(void)
{
   this->mc_ToolTipTimer.start();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Stop tool tip

   \created     15.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_StopToolTip(void)
{
   this->m_HandleHideToolTip();
}

//-----------------------------------------------------------------------------
void C_SebScene::m_BendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiLiLineGroup * const pc_Line = dynamic_cast<C_GiLiLineGroup *>(opc_Item);

   if ((pc_Line != NULL) && (this->m_GetUndoManager() != NULL))
   {
      this->m_GetUndoManager()->DoBendLine(pc_Line, orc_ScenePos);
   }
}

//-----------------------------------------------------------------------------
void C_SebScene::m_RemoveBendLine(QGraphicsItem * const opc_Item, const QPointF & orc_ScenePos)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiLiLineGroup * const pc_Line = dynamic_cast<C_GiLiLineGroup *>(opc_Item);

   if ((pc_Line != NULL) && (this->m_GetUndoManager() != NULL))
   {
      this->m_GetUndoManager()->DoRemoveBendLine(pc_Line, orc_ScenePos);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call setup style dialog

   \param[in,out] opc_Item Item to change style for

   \return
   False Failure or abort by user
   True  Success and accept

   \created     25.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SebScene::m_CallSetupStyle(QGraphicsItem * const opc_Item) const
{
   bool q_Retval = false;
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_GiBiBoundary * const pc_Boundary = dynamic_cast<C_GiBiBoundary *>(opc_Item);

   if (pc_Boundary != NULL)
   {
      q_Retval = pc_Boundary->OpenStyleDialog(this->mq_DarkModeActive);
   }
   else
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      C_GiBiTextElement * const pc_TextElement = dynamic_cast<C_GiBiTextElement *>(opc_Item);

      if (pc_TextElement != NULL)
      {
         q_Retval = pc_TextElement->OpenStyleDialog(this->mq_DarkModeActive);
      }
      else
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         C_GiBiArrow * const pc_Arrow = dynamic_cast<C_GiBiArrow *>(opc_Item);

         if (pc_Arrow != NULL)
         {
            q_Retval = pc_Arrow->OpenStyleDialog(this->mq_DarkModeActive);
         }
      }
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get new unique ID

   \param[in] opc_IDMap   Optional map for item to id
   \param[in] os32_Type   Item type, warning: only casted enum values accepted
   \param[in] oru32_Index Optional current index

   \return
   New unique ID

   \created     18.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint64 C_SebScene::m_GetNewUniqueID(const QMap<C_PuiBsTemporaryDataID,
                                               uint64> * const opc_IDMap, const sint32 os32_Type,
                                    const uint32 & oru32_Index)
{
   uint64 u64_Retval;

   if (opc_IDMap != NULL)
   {
      //Get mapped ID and use last unused unique ID as default value to know if there is no mapped value
      u64_Retval = opc_IDMap->value(C_PuiBsTemporaryDataID(os32_Type, oru32_Index), this->mu64_LastUnusedUniqueID);
      //If there is no mapped value the last unused value is used therefore the last unused value has to be increased
      if (u64_Retval == this->mu64_LastUnusedUniqueID)
      {
         ++mu64_LastUnusedUniqueID;
      }
   }
   else
   {
      u64_Retval = this->mu64_LastUnusedUniqueID;
      ++mu64_LastUnusedUniqueID;
   }
   return u64_Retval;
}

//-----------------------------------------------------------------------------
QRectF C_SebScene::m_ItemsBoundRect(void) const
{
   // the original function itemsBoundingRect calculates the size with graphic items in graphics proxy widgets
   // this is a bug
   const QList<QGraphicsItem *> & rc_Items = this->items();

   QList<QGraphicsItem *>::const_iterator c_ItItem;
   QRectF c_Rect(0.0, 0.0, 50.0, 50.0);

   // get the area of all items
   for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      const QPointF c_Pos = (*c_ItItem)->scenePos();
      QPointF c_PosBottomRight;

      c_PosBottomRight.setX(c_Pos.x() + (*c_ItItem)->boundingRect().size().width());
      c_PosBottomRight.setY(c_Pos.y() + (*c_ItItem)->boundingRect().size().height());

      if (c_PosBottomRight.x() > c_Rect.width())
      {
         c_Rect.setWidth(c_PosBottomRight.x());
      }
      if (c_PosBottomRight.y() > c_Rect.height())
      {
         c_Rect.setHeight(c_PosBottomRight.y());
      }
   }

   return c_Rect;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt scene rectangle

   The scene rectangle will be adpated dependent of the existing items and
   a additional border area.

   \created     26.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_AdaptSceneRect(void)
{
   QRectF c_Rect = m_ItemsBoundRect();
   const QPointF c_Point = c_Rect.topLeft();
   const QSizeF c_Size = c_Rect.size();

   //move to 0/0
   c_Rect.setX(0.0);
   c_Rect.setY(0.0);

   //add offset (for better handling in scrollbar area)
   c_Rect.setWidth((c_Point.x() + c_Size.width()) + mf64_SCENE_RECT_BORDER);
   c_Rect.setHeight((c_Point.y() + c_Size.height()) + mf64_SCENE_RECT_BORDER);

   // save the rectangle as minimum size
   this->mc_MinimumSceneRect = c_Rect;

   if (this->sceneRect().contains(c_Rect) == false)
   {
      //resize scene if it is to small but not to big because of zooming
      setSceneRect(c_Rect);
   }
}

//-----------------------------------------------------------------------------
void C_SebScene::m_MoveSelectedItems(const QPointF & orc_Delta)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   QList<QGraphicsItem *> c_MovingItems;

   QList<QGraphicsItem *>::const_iterator c_ItItem;
   QPointF c_AdaptedDelta = orc_Delta;

   // check the delta
   for (c_ItItem = rc_SelectedItems.begin(); c_ItItem != rc_SelectedItems.end(); ++c_ItItem)
   {
      // Is the item movable?
      if (this->IsItemMovable((*c_ItItem)) == true)
      {
         float64 f64_NewValue = (*c_ItItem)->mapToScene((*c_ItItem)->boundingRect().topLeft()).x() + c_AdaptedDelta.x();

         if (f64_NewValue < C_GiCustomFunctions::hf64_SceneMinBorderSize)
         {
            c_AdaptedDelta.setX(c_AdaptedDelta.x() + (C_GiCustomFunctions::hf64_SceneMinBorderSize - f64_NewValue));
         }

         f64_NewValue = (*c_ItItem)->mapToScene((*c_ItItem)->boundingRect().topLeft()).y() + c_AdaptedDelta.y();

         if (f64_NewValue < C_GiCustomFunctions::hf64_SceneMinBorderSize)
         {
            c_AdaptedDelta.setY(c_AdaptedDelta.y() + (C_GiCustomFunctions::hf64_SceneMinBorderSize - f64_NewValue));
         }
         c_MovingItems.push_back(*c_ItItem);
      }
   }

   if ((c_MovingItems.size() > 0) && (this->m_GetUndoManager() != NULL))
   {
      this->m_GetUndoManager()->DoMove(c_MovingItems, c_AdaptedDelta);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste origin items by key press

   \created     06.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_PasteKey(void)
{
   this->m_Paste(NULL);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Paste origin items by context menu click

   \created     06.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_PasteClick()
{
   QPointF c_CursorPos;

   // get the position
   m_MapFromGlobal(QCursor::pos(), c_CursorPos);
   // paste to the actual mouse position
   this->m_Paste(&c_CursorPos);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Align selected items

   \param[in] opc_GuidelineItem Item to use as guideline
   \param[in] ore_Alignment     Alignment type

   \created     07.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_Align(const QGraphicsItem * const opc_GuidelineItem, const E_Alignment & ore_Alignment)
{
   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->DoAlign(this->selectedItems(), opc_GuidelineItem, ore_Alignment);
   }
}

//-----------------------------------------------------------------------------
void C_SebScene::m_BringToFront(void)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   QList<QGraphicsItem *>::const_iterator c_ItItem;
   float64 f64_LowestZOrder = this->mf64_ZOrderHigh;
   float64 f64_Diff;

   for (c_ItItem = rc_SelectedItems.begin(); c_ItItem != rc_SelectedItems.end(); ++c_ItItem)
   {
      if (this->IsZOrderChangeable(*c_ItItem) == true)
      {
         // search the lowest z value of all selected items
         if ((*c_ItItem)->zValue() < f64_LowestZOrder)
         {
            f64_LowestZOrder = (*c_ItItem)->zValue();
         }
      }
   }
   // The lowest of the selected item must be above the previous highest item
   f64_Diff = (this->mf64_ZOrderHigh - f64_LowestZOrder) + 1.0;

   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->AdaptZOrder(rc_SelectedItems,
                                            this->items(), f64_Diff, this->mf64_ZOrderHigh, this->mf64_ZOrderLow);
   }
}

//-----------------------------------------------------------------------------
void C_SebScene::m_BringForward(void)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->AdaptZOrder(rc_SelectedItems,
                                            this->items(), 1.0, this->mf64_ZOrderHigh, this->mf64_ZOrderLow);
   }
}

//-----------------------------------------------------------------------------
void C_SebScene::m_SendBackward(void)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->AdaptZOrder(rc_SelectedItems,
                                            this->items(), -1.0, this->mf64_ZOrderHigh, this->mf64_ZOrderLow);
   }
}

//-----------------------------------------------------------------------------
void C_SebScene::m_SendToBack(void)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   QList<QGraphicsItem *>::const_iterator c_ItItem;
   float64 f64_HighestZOrder = this->mf64_ZOrderLow;
   float64 f64_Diff;

   for (c_ItItem = rc_SelectedItems.begin(); c_ItItem != rc_SelectedItems.end(); ++c_ItItem)
   {
      if (this->IsZOrderChangeable(*c_ItItem) == true)
      {
         // search the highest z value of all selected items
         if ((*c_ItItem)->zValue() > f64_HighestZOrder)
         {
            f64_HighestZOrder = (*c_ItItem)->zValue();
         }
      }
   }
   // The highest of the selected item must be below the previous lowest item
   f64_Diff = ((f64_HighestZOrder - this->mf64_ZOrderLow) + 1.0) * (-1.0);

   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->AdaptZOrder(rc_SelectedItems,
                                            this->items(), f64_Diff, this->mf64_ZOrderHigh, this->mf64_ZOrderLow);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle setup style action

   \param[in,out] opc_Item Item to change style for

   \created     25.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_SetupStyle(QGraphicsItem * const opc_Item)
{
   C_SebUnoBaseManager * const pc_UndoManager = this->m_GetUndoManager();

   if (pc_UndoManager != NULL)
   {
      bool q_Apply;

      QList<QGraphicsItem *> c_Items;

      //Undo
      c_Items.push_back(opc_Item);
      pc_UndoManager->SaveStyleInformation(c_Items, this->mq_DarkModeActive);

      q_Apply = m_CallSetupStyle(opc_Item);
      if (q_Apply == true)
      {
         //Undo
         pc_UndoManager->RegisterStyleChange();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Now show tool tip

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_TriggerToolTip(void)
{
   this->DisplaySpecificItemToolTip(mc_ToolTipPos);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle background state

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_HandleBackground(void)
{
   if (this->mq_DrawCustomBackground == true)
   {
      if (this->mq_DarkModeActive == true)
      {
         this->setBackgroundBrush(this->mc_ImgDrawBackgroundDark);
      }
      else
      {
         this->setBackgroundBrush(this->mc_ImgDrawBackground);
      }
   }
   else
   {
      if (this->mq_DarkModeActive == true)
      {
         this->setBackgroundBrush(mc_STYLE_GUIDE_COLOR_52);
      }
      else
      {
         this->setBackgroundBrush(Qt::NoBrush);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle change of selection

   \created     04.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_OnSelectionChange(void)
{
   // Register selection change in move because different items could be affected by move.
   // Move must be registered if selection changed. Last chance.
   if (this->m_GetUndoManager() != NULL)
   {
      this->m_GetUndoManager()->RegisterMove();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for delete action

   \created     12.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebScene::m_DeleteSlot(void)
{
   m_Delete();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get supported image types as filter

   \return
   Supported image types as filter

   \created     21.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SebScene::mh_GetImageFileTypesFilter(void)
{
   QString c_Retval = "";
   const QList<QByteArray> c_Types = QImageReader::supportedImageFormats();

   for (QList<QByteArray>::const_iterator c_It = c_Types.begin(); c_It != c_Types.end(); ++c_It)
   {
      const QString c_CurrentType(*c_It);
      c_Retval += QString("*.%1; ").arg(c_CurrentType);
   }
   return c_Retval;
}
