//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all graphic items which are rectangle based (implementation)

   The class offers resize functionality.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPainter>
#include <QCursor>
#include <cmath>

#include "C_GiBiRectBaseGroup.h"
#include "TGLUtils.h"
#include "C_GiCustomFunctions.h"
#include "C_PuiBsLineBase.h"
#include "C_OSCUtils.h"

#include "gitypes.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn msn_IndexTopLeft = 0;
const sintn msn_IndexTopRight = 1;
const sintn msn_IndexBottomLeft = 2;
const sintn msn_IndexBottomRight = 3;
const sintn msn_IndexElementKeepRatioMax = 4;
const sintn msn_IndexTop = 4;
const sintn msn_IndexBottom = 5;
const sintn msn_IndexLeft = 6;
const sintn msn_IndexRight = 7;
const sintn msn_IndexElementMax = 8;

const float64 mf64_BOUNDINGRECT_BORDER = 10.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.
   Function m_SetBiggestItem must be called after construction.

   \param[in]     oru64_ID               Unique ID
   \param[in]     of64_MinWidth          Min width of node
   \param[in]     of64_MinHeight         Min height of node
   \param[in]     of64_ActionPointOffset Action point offset
   \param[in]     oq_KeepAspectRatio     Flag if the rectangle should always keep its initial aspect ratio
   \param[in,out] opc_Parent             Optional pointer to parent
   \param[in]     orc_PosOffset          Optional offset for the position (values must be in 0.0 <= x < 1.0)
                                         Negative value deactivates the function
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiRectBaseGroup::C_GiBiRectBaseGroup(const uint64 & oru64_ID, const float64 of64_MinWidth,
                                         const float64 of64_MinHeight, const stw_types::float64 of64_ActionPointOffset,
                                         const bool oq_KeepAspectRatio, QGraphicsItem * const opc_Parent,
                                         const QPointF & orc_PosOffset) :
   C_GiUnique(oru64_ID),
   QGraphicsItemGroup(opc_Parent),
   mq_ResizingActive(false),
   msn_ActiveResizeMode(msn_IndexElementMax),
   mpc_BiggestSubItem(NULL),
   mc_LastKnownPosition(),
   mc_LastKnownSize(),
   mq_BlockMoveAndResize(false),
   mq_KeepAspectRatio(oq_KeepAspectRatio),
   mf64_ActionPointOffset(of64_ActionPointOffset),
   mf64_MinWidth(of64_MinWidth),
   mf64_MinHeight(of64_MinHeight),
   mc_PosOffset(orc_PosOffset)
{
   sint32 s32_Counter;
   sint32 s32_Limit;

   this->setFlag(ItemIsMovable);
   this->setFlag(ItemIsSelectable);
   this->setFlag(ItemSendsGeometryChanges);
   this->setFlag(ItemIsFocusable);

   if (this->mq_KeepAspectRatio == false)
   {
      s32_Limit = msn_IndexElementMax;
   }
   else
   {
      // only four elements necessary
      s32_Limit = msn_IndexElementKeepRatioMax;
   }

   for (s32_Counter = 0; s32_Counter < s32_Limit; ++s32_Counter)
   {
      // create interaction points
      this->mc_ActionPoints.append(new C_GiPointInteraction());
   }

   //Mouse cursor
   this->mc_DefaultCursor = QCursor(Qt::SizeAllCursor);
   this->RestoreDefaultCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the biggest sub item of the group.

   It is necessary for calculation the correct position of the interaction points and
   the show bounding rect.

   \param[in] orc_Item Sizeable item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::m_SetBiggestItem(C_GiBiSizeableItem & orc_Item)
{
   this->mpc_BiggestSubItem = &orc_Item;

   this->prepareGeometryChange();

   // now we can set up the action points
   this->m_InitActionPoints();

   // update the aspect ratio
   this->mf64_AspectRatio = this->mpc_BiggestSubItem->f64_Width / this->mpc_BiggestSubItem->f64_Height;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::RestoreDefaultCursor(void)
{
   this->setCursor(this->mc_DefaultCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change mouse cursor temporarily

   \param[in] orc_TemporaryCursor New mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::SetTemporaryCursor(const QCursor & orc_TemporaryCursor)
{
   this->setCursor(orc_TemporaryCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set default mouse cursor

   \param[in] orc_Value New default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::SetDefaultCursor(const QCursor & orc_Value)
{
   this->setCursor(orc_Value);
   this->mc_DefaultCursor = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::m_InitActionPoints()
{
   QVector<C_GiPointInteraction *>::iterator pc_ItPoint;

   // set initial coordinates for the action points and the shown bounding rect
   this->m_UpdateActionPoints();

   // set the cursor of all interaction points
   this->mc_ActionPoints[msn_IndexTopLeft]->setCursor(QCursor(Qt::SizeFDiagCursor));
   this->mc_ActionPoints[msn_IndexTopRight]->setCursor(QCursor(Qt::SizeBDiagCursor));

   this->mc_ActionPoints[msn_IndexBottomLeft]->setCursor(QCursor(Qt::SizeBDiagCursor));
   this->mc_ActionPoints[msn_IndexBottomRight]->setCursor(QCursor(Qt::SizeFDiagCursor));

   if (this->mq_KeepAspectRatio == false)
   {
      this->mc_ActionPoints[msn_IndexTop]->setCursor(QCursor(Qt::SizeVerCursor));
      this->mc_ActionPoints[msn_IndexBottom]->setCursor(QCursor(Qt::SizeVerCursor));
      this->mc_ActionPoints[msn_IndexLeft]->setCursor(QCursor(Qt::SizeHorCursor));
      this->mc_ActionPoints[msn_IndexRight]->setCursor(QCursor(Qt::SizeHorCursor));
   }

   // configure the points
   for (pc_ItPoint = this->mc_ActionPoints.begin(); pc_ItPoint != this->mc_ActionPoints.end(); ++pc_ItPoint)
   {
      (*pc_ItPoint)->setVisible(false);

      // add to the groupt
      this->addToGroup((*pc_ItPoint));
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::m_UpdateActionPoints(void)
{
   // adapt the shown bounding rect coordinatesESX-3CM
   this->mc_ShowBoundingRect = this->m_GetBiggestSubItemBoundingRect();
   this->mc_ShowBoundingRect.setX(this->mc_ShowBoundingRect.x() - mf64_ActionPointOffset);
   this->mc_ShowBoundingRect.setY(this->mc_ShowBoundingRect.y() - mf64_ActionPointOffset);
   this->mc_ShowBoundingRect.setWidth(this->mc_ShowBoundingRect.width() + mf64_ActionPointOffset);
   this->mc_ShowBoundingRect.setHeight(this->mc_ShowBoundingRect.height() + mf64_ActionPointOffset);

   this->mc_ActionPoints[msn_IndexTopLeft]->setPos(this->mc_ShowBoundingRect.x(),
                                                   this->mc_ShowBoundingRect.y());
   this->mc_ActionPoints[msn_IndexTopRight]->setPos((this->mc_ShowBoundingRect.width() +
                                                     this->mc_ShowBoundingRect.x()),
                                                    this->mc_ShowBoundingRect.y());

   this->mc_ActionPoints[msn_IndexBottomLeft]->setPos(this->mc_ShowBoundingRect.x(),
                                                      (this->mc_ShowBoundingRect.height() +
                                                       this->mc_ShowBoundingRect.y()));
   this->mc_ActionPoints[msn_IndexBottomRight]->setPos((this->mc_ShowBoundingRect.width() +
                                                        this->mc_ShowBoundingRect.x()),
                                                       (this->mc_ShowBoundingRect.height() +
                                                        this->mc_ShowBoundingRect.y()));

   if (this->mq_KeepAspectRatio == false)
   {
      this->mc_ActionPoints[msn_IndexTop]->setPos(this->mc_ShowBoundingRect.center().x(),
                                                  this->mc_ShowBoundingRect.y());
      this->mc_ActionPoints[msn_IndexBottom]->setPos(this->mc_ShowBoundingRect.center().x(),
                                                     (this->mc_ShowBoundingRect.height() +
                                                      this->mc_ShowBoundingRect.y()));

      this->mc_ActionPoints[msn_IndexLeft]->setPos(this->mc_ShowBoundingRect.x(),
                                                   this->mc_ShowBoundingRect.center().y());
      this->mc_ActionPoints[msn_IndexRight]->setPos((this->mc_ShowBoundingRect.width() +
                                                     this->mc_ShowBoundingRect.x()),
                                                    this->mc_ShowBoundingRect.center().y());
   }
}

//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiBiRectBaseGroup::m_GetBiggestSubItemBoundingRect(void) const
{
   QRectF c_Rect;

   tgl_assert(this->mpc_BiggestSubItem != NULL);

   if (this->mpc_BiggestSubItem != NULL)
   {
      c_Rect = this->mpc_BiggestSubItem->GetSizeRect();
   }

   return c_Rect;
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::m_SetAdaptedPos(const QPointF & orc_Pos)
{
   if ((this->mc_PosOffset.x() >= 0.0) ||
       (this->mc_PosOffset.y() >= 0.0))
   {
      QPointF c_AdaptedPos;

      if (this->mc_PosOffset.x() > 0.0)
      {
         c_AdaptedPos.setX(std::floor(orc_Pos.x()) + this->mc_PosOffset.x());
      }
      else
      {
         c_AdaptedPos.setX(orc_Pos.x());
      }

      if (this->mc_PosOffset.y() > 0.0)
      {
         c_AdaptedPos.setY(std::floor(orc_Pos.y()) + this->mc_PosOffset.y());
      }
      else
      {
         c_AdaptedPos.setY(orc_Pos.y());
      }

      this->setPos(c_AdaptedPos);
   }
   else
   {
      this->setPos(orc_Pos);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::m_BiggestItemChanged(void)
{
   // keep GraphicsScene up to date and get a clean drawing
   this->prepareGeometryChange();

   // update the shown bounding rect and the interaction points
   this->m_UpdateActionPoints();

   // inform the other elements
   Q_EMIT this->SigChangedGraphic();

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::m_BlockMoveAndResize(void)
{
   this->mq_BlockMoveAndResize = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interaction point width

   \return
   Interaction point width
*/
//----------------------------------------------------------------------------------------------------------------------
float64 C_GiBiRectBaseGroup::m_GetInteractionPointSceneWidth(void) const
{
   float64 f64_Retval = 1.0;

   if (this->mc_ActionPoints.size() > 0)
   {
      f64_Retval = mc_ActionPoints[0]->sceneBoundingRect().width();
   }
   return f64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiRectBaseGroup::~C_GiBiRectBaseGroup()
{
   //lint -e{1540}  no memory leak because of the parent of mpc_BiggestSubItem and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten paint event slot

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                                QWidget * const opc_Widget)
{
   QPen c_Pen = opc_Painter->pen();

   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   c_Pen.setColor(mc_STYLE_GUIDE_COLOR_21);
   opc_Painter->setPen(c_Pen);

   if (this->isSelected() == true)
   {
      opc_Painter->drawRect(this->mc_ShowBoundingRect);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get bounding rectangle (estimate of the area painted by the item)

   The real bounding rect is slightly bigger than the shown bounding rect.
   The interaction have to be inside the bounding rect.

   \return
   bounding rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiBiRectBaseGroup::boundingRect() const
{
   QRectF c_BoundingRect = this->mc_ShowBoundingRect;
   float64 f64_Border = mf64_BOUNDINGRECT_BORDER;

   if (this->mc_ActionPoints.size() > 0)
   {
      f64_Border = this->mc_ActionPoints[0]->sceneBoundingRect().width();
   }
   f64_Border = f64_Border / 2.0;
   c_BoundingRect.setX(c_BoundingRect.x() - f64_Border);
   c_BoundingRect.setY(c_BoundingRect.y() - f64_Border);
   c_BoundingRect.setWidth(c_BoundingRect.width() + f64_Border);
   c_BoundingRect.setHeight(c_BoundingRect.height() + f64_Border);

   return c_BoundingRect;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get visible bounding rectangle for better alignment

   \return Visible bounding rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiBiRectBaseGroup::GetVisibleBoundingRect() const
{
   return this->mc_ShowBoundingRect;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find closest point in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] orc_Closest    Closest point in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const
{
   orc_Closest = orc_ScenePoint;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::SetZValueCustom(const float64 of64_ZValue)
{
   this->setZValue(of64_ZValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  De- or activates the resizing of a node

   \param[in] oq_Active True activates the resizing, false deactivates the resizing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::SetResizing(const bool oq_Active)
{
   QVector<C_GiPointInteraction *>::iterator pc_ItPoint;

   this->mq_ResizingActive = oq_Active;

   // configure the points
   for (pc_ItPoint = this->mc_ActionPoints.begin(); pc_ItPoint != this->mc_ActionPoints.end(); ++pc_ItPoint)
   {
      (*pc_ItPoint)->setVisible(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the size of the biggest sub item

   \return  Actual size
*/
//----------------------------------------------------------------------------------------------------------------------
QSizeF C_GiBiRectBaseGroup::GetSize(void) const
{
   QSizeF c_Size(0.0, 0.0);

   tgl_assert(this->mpc_BiggestSubItem != NULL);

   if (this->mpc_BiggestSubItem != NULL)
   {
      c_Size = this->mpc_BiggestSubItem->GetSizeRect().size();
   }

   return c_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for initially loading basic internal data

   \param[in,out] orc_Data Data element to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::LoadBasicData(const stw_opensyde_gui_logic::C_PuiBsBox & orc_Data)
{
   this->m_SetAdaptedPos(orc_Data.c_UIPosition);
   //Don't trigger data update at this point
   this->setZValue(orc_Data.f64_ZOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enter basic line info

   \param[in,out] orc_Data Data element to update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::UpdateBasicData(stw_opensyde_gui_logic::C_PuiBsBox & orc_Data) const
{
   tgl_assert(this->mpc_BiggestSubItem != NULL);

   if (this->mpc_BiggestSubItem != NULL)
   {
      orc_Data.f64_Width = this->mpc_BiggestSubItem->f64_Width;
      orc_Data.f64_Height = this->mpc_BiggestSubItem->f64_Height;
   }
   else
   {
      orc_Data.f64_Width = 0.0;
      orc_Data.f64_Height = 0.0;
   }

   orc_Data.c_UIPosition = this->pos();
   orc_Data.f64_ZOrder = this->zValue();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  External trigger for size change

   \param[in] orc_NewPos  New position
   \param[in] orc_NewSize New size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::ApplySizeChange(const QPointF & orc_NewPos, const QSizeF & orc_NewSize)
{
   QPointF c_Pos = orc_NewPos;
   QSizeF c_Size = orc_NewSize;
   float64 f64_DiffWidth;
   float64 f64_DiffHeight;

   // keep GraphicsScene up to date and get a clean drawing
   this->prepareGeometryChange();

   // check new geometry
   if (c_Pos.x() < (C_GiCustomFunctions::hf64_SceneMinBorderSize + mf64_BOUNDINGRECT_BORDER))
   {
      c_Pos.setX(C_GiCustomFunctions::hf64_SceneMinBorderSize + mf64_BOUNDINGRECT_BORDER);

      if (this->mq_KeepAspectRatio == false)
      {
         c_Size.setWidth(this->m_GetBiggestSubItemBoundingRect().width());
      }
      else
      {
         //Don't change size to keep aspect ratio
      }
   }
   if (c_Pos.y() < (C_GiCustomFunctions::hf64_SceneMinBorderSize + mf64_BOUNDINGRECT_BORDER))
   {
      c_Pos.setY(C_GiCustomFunctions::hf64_SceneMinBorderSize + mf64_BOUNDINGRECT_BORDER);

      if (this->mq_KeepAspectRatio == false)
      {
         c_Size.setHeight(this->m_GetBiggestSubItemBoundingRect().height());
      }
      else
      {
         //Don't change size to keep aspect ratio
      }
   }
   if (c_Size.width() < this->mf64_MinWidth)
   {
      c_Pos.setX(this->x());
      c_Size.setWidth(this->m_GetBiggestSubItemBoundingRect().width());
   }
   if (c_Size.height() < this->mf64_MinHeight)
   {
      c_Pos.setY(this->y());
      c_Size.setHeight(this->m_GetBiggestSubItemBoundingRect().height());
   }

   // set new geometry
   this->m_SetAdaptedPos(c_Pos);
   f64_DiffWidth = c_Size.width() - this->m_GetBiggestSubItemBoundingRect().width();
   f64_DiffHeight = c_Size.height() - this->m_GetBiggestSubItemBoundingRect().height();

   tgl_assert(this->mpc_BiggestSubItem != NULL);
   if (this->mpc_BiggestSubItem != NULL)
   {
      this->mpc_BiggestSubItem->f64_Width = c_Size.width();
      this->mpc_BiggestSubItem->f64_Height = c_Size.height();
      this->mpc_BiggestSubItem->Redraw();
   }

   // update all subitems of derived classes
   this->m_ResizeUpdateItems(f64_DiffWidth, f64_DiffHeight);

   // update the shown bounding rect and the interaction points
   this->m_UpdateActionPoints();

   // inform the other elements
   Q_EMIT this->SigChangedGraphic();

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the style of the provided element

   Warning: Only expected to work if the provided item is of the same type as this element

   \param[in] opc_GuidelineItem Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::CopyStyle(const QGraphicsItem * const opc_GuidelineItem)
{
   Q_UNUSED(opc_GuidelineItem)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal for update of current scaling

   \param[in] orc_Transform Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::UpdateTransform(const QTransform & orc_Transform)
{
   this->prepareGeometryChange();
   for (QVector<C_GiPointInteraction *>::iterator pc_ItActionPoint = this->mc_ActionPoints.begin();
        pc_ItActionPoint != this->mc_ActionPoints.end(); ++pc_ItActionPoint)
   {
      C_GiPointInteraction * const pc_ActionPointP = *pc_ItActionPoint;
      if (pc_ActionPointP != NULL)
      {
         pc_ActionPointP->UpdateTransform(orc_Transform);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten itemChange event slot

   \param[in] oe_Change Indicator what changed
   \param[in] orc_Value Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiBiRectBaseGroup::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   QVariant c_Return;

   //Don't pass bounding rect offset -> currently does worse on border handling
   //Not passing the offset probably just allows a buffer so this function does not adapt the change "the wrong way"
   c_Return = C_GiCustomFunctions::h_ItemChange(oe_Change, orc_Value, this, NULL);

   c_Return = QGraphicsItemGroup::itemChange(oe_Change, c_Return);

   switch (oe_Change)
   {
   case ItemSelectedHasChanged:
      if (orc_Value == false)
      {
         // deactivate resizing always when node is getting unselected
         // activation of resizing must be set by external call
         this->SetResizing(false);
      }
      Q_EMIT this->SigSelectionChange(this->isSelected());
      break;
   case ItemSceneHasChanged:
      if (this->scene() != NULL)
      {
         // item was added to scene
         QVector<C_GiPointInteraction *>::iterator pc_ItPoint;

         // configure the points
         for (pc_ItPoint = this->mc_ActionPoints.begin(); pc_ItPoint != this->mc_ActionPoints.end(); ++pc_ItPoint)
         {
            (*pc_ItPoint)->installSceneEventFilter(this);
         }
      }
      break;
   case ItemSceneChange:
      if (orc_Value.isNull() == true)
      {
         // item was removed from scene
         QVector<C_GiPointInteraction *>::iterator pc_ItPoint;

         // configure the points
         for (pc_ItPoint = this->mc_ActionPoints.begin(); pc_ItPoint != this->mc_ActionPoints.end(); ++pc_ItPoint)
         {
            (*pc_ItPoint)->removeSceneEventFilter(this);
         }
      }
      break;
   case ItemPositionHasChanged:             // use the same signal
   case ItemPositionChange:                 // use the same signal
   case ItemTransformHasChanged:            // use the same signal
   case ItemScaleHasChanged:                // use the same signal
   case ItemTransformOriginPointHasChanged: // use the same signal
      Q_EMIT this->SigChangedGraphic();
      break;
   default:
      break;
   } //lint !e788 //All other cases handled by call of parent

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   this->mc_LastKnownPosition = this->pos();
   this->mc_LastKnownSize = this->GetSize();
   if ((opc_Event->button() == Qt::LeftButton) &&
       (this->mq_ResizingActive == true) &&
       (this->mq_BlockMoveAndResize == false))
   {
      QPointF c_Pos = opc_Event->scenePos();

      if (this->mc_ActionPoints[msn_IndexBottomRight]->IsPointInside(c_Pos) == true)
      {
         this->msn_ActiveResizeMode = msn_IndexBottomRight;
      }
      else if (this->mc_ActionPoints[msn_IndexBottomLeft]->IsPointInside(c_Pos) == true)
      {
         this->msn_ActiveResizeMode = msn_IndexBottomLeft;
      }
      else if (this->mc_ActionPoints[msn_IndexTopLeft]->IsPointInside(c_Pos) == true)
      {
         this->msn_ActiveResizeMode = msn_IndexTopLeft;
      }
      else if (this->mc_ActionPoints[msn_IndexTopRight]->IsPointInside(c_Pos) == true)
      {
         this->msn_ActiveResizeMode = msn_IndexTopRight;
      }
      else if (this->mq_KeepAspectRatio == false)
      {
         if (this->mc_ActionPoints[msn_IndexTop]->IsPointInside(c_Pos) == true)
         {
            this->msn_ActiveResizeMode = msn_IndexTop;
         }
         else if (this->mc_ActionPoints[msn_IndexBottom]->IsPointInside(c_Pos) == true)
         {
            this->msn_ActiveResizeMode = msn_IndexBottom;
         }
         else if (this->mc_ActionPoints[msn_IndexLeft]->IsPointInside(c_Pos) == true)
         {
            this->msn_ActiveResizeMode = msn_IndexLeft;
         }
         else if (this->mc_ActionPoints[msn_IndexRight]->IsPointInside(c_Pos) == true)
         {
            this->msn_ActiveResizeMode = msn_IndexRight;
         }
         else
         {
            // No interaction point clicked
            this->msn_ActiveResizeMode = msn_IndexElementMax;

            QGraphicsItemGroup::mousePressEvent(opc_Event);
         }
      }
      else
      {
         // No interaction point clicked
         this->msn_ActiveResizeMode = msn_IndexElementMax;

         QGraphicsItemGroup::mousePressEvent(opc_Event);
      }
   }
   else
   {
      QGraphicsItemGroup::mousePressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse move event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if ((this->msn_ActiveResizeMode != msn_IndexElementMax) &&
       (this->mq_BlockMoveAndResize == false))
   {
      QPointF c_MousePos = opc_Event->scenePos();
      QPointF c_Delta;
      QSizeF c_Size = this->m_GetBiggestSubItemBoundingRect().size();
      QPointF c_Pos = this->pos();

      C_GiCustomFunctions::h_AdaptMouseRangePos(c_MousePos,
                                                QPointF(mf64_BOUNDINGRECT_BORDER, mf64_BOUNDINGRECT_BORDER));

      c_Delta = c_MousePos - this->mc_ActionPoints[this->msn_ActiveResizeMode]->scenePos();

      if (this->mq_KeepAspectRatio == true)
      {
         C_GiCustomFunctions::E_AspectRatioMovement e_AspectRatioMovement;
         if (this->msn_ActiveResizeMode == msn_IndexBottomLeft)
         {
            e_AspectRatioMovement = C_GiCustomFunctions::eARM_BOTTOM_LEFT;
         }
         else if (this->msn_ActiveResizeMode == msn_IndexTopRight)
         {
            e_AspectRatioMovement = C_GiCustomFunctions::eARM_TOP_RIGHT;
         }
         else if (this->msn_ActiveResizeMode == msn_IndexTopLeft)
         {
            e_AspectRatioMovement = C_GiCustomFunctions::eARM_TOP_LEFT;
         }
         else
         {
            e_AspectRatioMovement = C_GiCustomFunctions::eARM_BOTTOM_RIGHT;
         }
         c_Delta = C_GiCustomFunctions::h_AdaptDeltaForAspectRatio(this->mf64_AspectRatio, e_AspectRatioMovement,
                                                                   c_Delta);
      }

      // adapt size and position
      switch (this->msn_ActiveResizeMode)
      {
      case msn_IndexTopLeft:
         c_Size.setWidth(c_Size.width() - c_Delta.x());
         c_Pos.setX(c_Pos.x() + c_Delta.x());

         c_Size.setHeight(c_Size.height() - c_Delta.y());
         c_Pos.setY(c_Pos.y() + c_Delta.y());
         break;

      case msn_IndexTopRight:
         c_Size.setWidth(c_Size.width() + c_Delta.x());

         c_Size.setHeight(c_Size.height() - c_Delta.y());
         c_Pos.setY(c_Pos.y() + c_Delta.y());
         break;

      case msn_IndexBottomLeft:
         c_Size.setWidth(c_Size.width() - c_Delta.x());
         c_Pos.setX(c_Pos.x() + c_Delta.x());

         c_Size.setHeight(c_Size.height() + c_Delta.y());
         break;

      case msn_IndexBottomRight:
         c_Size.setHeight(c_Size.height() + c_Delta.y());

         c_Size.setWidth(c_Size.width() + c_Delta.x());
         break;

      case msn_IndexRight:
         c_Size.setWidth(c_Size.width() + c_Delta.x());
         break;

      case msn_IndexLeft:
         c_Size.setWidth(c_Size.width() - c_Delta.x());
         c_Pos.setX(c_Pos.x() + c_Delta.x());
         break;

      case msn_IndexBottom:
         c_Size.setHeight(c_Size.height() + c_Delta.y());
         break;

      case msn_IndexTop:
         c_Size.setHeight(c_Size.height() - c_Delta.y());
         c_Pos.setY(c_Pos.y() + c_Delta.y());
         break;

      default:
         break;
      }

      ApplySizeChange(c_Pos, c_Size);
   }
   else
   {
      QGraphicsItemGroup::mouseMoveEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse release event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiRectBaseGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (this->mq_BlockMoveAndResize == false)
   {
      //Signal complete move step
      if (opc_Event->button() == Qt::LeftButton)
      {
         if (this->msn_ActiveResizeMode != msn_IndexElementMax)
         {
            Q_EMIT this->SigItemWasResized(this->mc_LastKnownPosition, this->mc_LastKnownSize,
                                           this->pos(), this->GetSize());
         }
         else
         {
            if (this->pos() != this->mc_LastKnownPosition)
            {
               Q_EMIT this->SigItemWasMoved(this->pos() - this->mc_LastKnownPosition);
            }
         }
      }
      // deactivate resizing
      this->msn_ActiveResizeMode = msn_IndexElementMax;
   }
   else
   {
      this->mq_BlockMoveAndResize = false;
   }

   QGraphicsItemGroup::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten scene event filter event slot

   Prevent action points from receiving events

   \param[in]     opc_Watched Actual event receiver
   \param[in,out] opc_Event   Event identification and information

   \return
   true     Event will be ignored
   false    Event will be forwarded normally
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiBiRectBaseGroup::sceneEventFilter(QGraphicsItem * const opc_Watched, QEvent * const opc_Event)
{
   bool q_Return = true;

   if (opc_Watched->type() == msn_GRAPHICS_ITEM_POINTINTERACTION)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QGraphicsSceneMouseEvent * pc_MouseEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(opc_Event);

      switch (opc_Event->type())
      {
      case QEvent::GraphicsSceneMousePress:
         this->mousePressEvent(pc_MouseEvent);
         break;

      case QEvent::GraphicsSceneMouseRelease:
         this->mouseReleaseEvent(pc_MouseEvent);
         break;

      case QEvent::GraphicsSceneMouseMove:
         this->mouseMoveEvent(pc_MouseEvent);
         break;

      default:
         break;
         //lint -e{788}  not all enum constants are necessary here
      }
   }
   else
   {
      q_Return = QGraphicsItemGroup::sceneEventFilter(opc_Watched, opc_Event);
   }

   return q_Return;
}
