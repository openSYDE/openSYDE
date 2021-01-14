//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete line with all functionality (implementation)

   All common events and movement funciontality for lines are implemented here

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsScene>
#include <QPainter>
#include <limits>

#include "constants.h"
#include "C_OSCUtils.h"
#include "C_GiLiLineGroup.h"
#include "C_GiCustomFunctions.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64 C_GiLiLineGroup::mhf64_MaxDistToAlign = 10.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     opc_Points     Optional points for line
   \param[in]     orq_MiddleLine Indicator if line(s) should have a different colored central line
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiLineGroup::C_GiLiLineGroup(const std::vector<QPointF> * const opc_Points, const bool & orq_MiddleLine,
                                 QGraphicsItem * const opc_Parent) :
   QGraphicsItemGroup(opc_Parent),
   mpc_LinePath(new C_GiLiLine(NULL, orq_MiddleLine, opc_Parent)),
   //lint -e{1938}  static const is guaranteed preinitialized before main
   me_ActiveResizeMode(eNO_ELEMENT),
   msn_ActiveItemIndex(0),
   mc_LastKnownPosition(),
   mq_BlockChangeSignal(false)

{
   this->setFlag(ItemIsMovable);
   this->setFlag(ItemIsSelectable);
   this->setFlag(ItemSendsGeometryChanges);

   this->addToGroup(this->mpc_LinePath);

   if (opc_Points != NULL)
   {
      this->m_Init(*opc_Points);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mc_Points & mpc_LinePath and the Qt memory management
C_GiLiLineGroup::~C_GiLiLineGroup(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set interaction elements visible

   \param[in] orq_Visible Visible status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_SetInteractionVisibility(const bool & orq_Visible)
{
   for (sint32 s32_ItPoint = 0L; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
   {
      mc_Points[s32_ItPoint]->setVisible(orq_Visible);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set interaction elements visible but interactable

   \param[in] orq_Invisible Visible status
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_HideInteraction(const bool & orq_Invisible)
{
   for (sint32 s32_ItPoint = 0L; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
   {
      if (orq_Invisible == true)
      {
         mc_Points[s32_ItPoint]->setOpacity(0.0);
      }
      else
      {
         mc_Points[s32_ItPoint]->setOpacity(1.0);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_CheckLineGrid(const QPointF & orc_MouseScenePos)
{
   bool q_Adapted = false;

   this->prepareGeometryChange();

   //Affected lines
   for (sint32 s32_ItPoint = std::max(0, this->msn_ActiveItemIndex - 1);
        (s32_ItPoint <= static_cast<sint32>(this->msn_ActiveItemIndex)) &&
        (s32_ItPoint < static_cast<sint32>(this->mc_Points.size() - 1));
        ++s32_ItPoint)
   {
      QPointF c_LineStart = this->mc_Points[s32_ItPoint]->scenePos();
      QPointF c_LineEnd = this->mc_Points[static_cast<sintn>(s32_ItPoint + 1L)]->scenePos();
      if (s32_ItPoint == this->msn_ActiveItemIndex)
      {
         if (q_Adapted == false)
         {
            c_LineStart = orc_MouseScenePos;
         }
      }
      else
      {
         c_LineEnd = orc_MouseScenePos;
      }
      //Check already aligned
      if ((C_OSCUtils::h_IsFloat64NearlyEqual(c_LineStart.x(), c_LineEnd.x()) == false) ||
          (C_OSCUtils::h_IsFloat64NearlyEqual(c_LineStart.y(), c_LineEnd.y()) == false))
      {
         //Not aligned
         //Check degree
         const float64 f64_DistX = c_LineEnd.x() - c_LineStart.x();
         const float64 f64_DistY = c_LineEnd.y() - c_LineStart.y();
         if (C_GiLiLineGroup::mh_Near(f64_DistY, 0.0))
         {
            //Align y
            if (s32_ItPoint == this->msn_ActiveItemIndex)
            {
               this->UpdatePoint(this->msn_ActiveItemIndex, QPointF(c_LineStart.x(), c_LineEnd.y()), true);
            }
            else
            {
               this->UpdatePoint(this->msn_ActiveItemIndex, QPointF(c_LineEnd.x(), c_LineStart.y()), true);
            }
            q_Adapted = true;
         }
         else if (C_GiLiLineGroup::mh_Near(f64_DistX, 0.0))
         {
            //Align x
            if (s32_ItPoint == this->msn_ActiveItemIndex)
            {
               this->UpdatePoint(this->msn_ActiveItemIndex, QPointF(c_LineEnd.x(), c_LineStart.y()), true);
            }
            else
            {
               this->UpdatePoint(this->msn_ActiveItemIndex, QPointF(c_LineStart.x(), c_LineEnd.y()), true);
            }
            q_Adapted = true;
         }
         else
         {
            //Nothing to do
         }
      }
   }
   if (q_Adapted == false)
   {
      //Do manual move to mouse position
      this->UpdatePoint(this->msn_ActiveItemIndex, orc_MouseScenePos, true);
   }
   //One central change trigger
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiLineGroup::mh_Near(const float64 of64_Exact, const float64 of64_Eval)
{
   bool q_Retval;

   if ((of64_Exact >= (of64_Eval - C_GiLiLineGroup::mhf64_MaxDistToAlign)) &&
       (of64_Exact <= (of64_Eval + C_GiLiLineGroup::mhf64_MaxDistToAlign)))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a bending point

   \param[in] orc_ScenePos Position of the bending point
   \param[in] ops32_Index  Optional specific index to add point add

   \return
   -1: error
   else: index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiLineGroup::BendLine(const QPointF & orc_ScenePos, const sint32 * const ops32_Index)
{
   const QVector<C_GiLiLineConnection *> & rc_VecLines = this->mpc_LinePath->GetLines();

   sint32 s32_Retval = -1;

   //Restrict position
   const QPointF c_AdaptedPos(std::max(20.0, orc_ScenePos.x()), std::max(20.0, orc_ScenePos.y()));

   this->prepareGeometryChange();

   if (ops32_Index == NULL)
   {
      QVector<C_GiLiLineConnection *>::const_iterator pc_ItLine;
      float64 f64_ResultDist;
      float64 f64_MinDist = 1000000.0;

      // get the next line to the point
      for (pc_ItLine = rc_VecLines.begin(); pc_ItLine != rc_VecLines.end(); ++pc_ItLine)
      {
         C_GiBiConnectableItem::h_DistToLine((*pc_ItLine)->line().p1(),
                                             (*pc_ItLine)->line().p2(),
                                             this->mapFromScene(c_AdaptedPos),
                                             &f64_ResultDist, NULL, NULL);

         if (f64_ResultDist < f64_MinDist)
         {
            // set the new minimum distance
            f64_MinDist = f64_ResultDist;
            // set the nearest line index
            s32_Retval = (*pc_ItLine)->GetLineIndex();
            ++s32_Retval;
         }
      }
   }
   else
   {
      s32_Retval = *ops32_Index;
   }

   // save the position
   if (s32_Retval >= 0)
   {
      if (((s32_Retval - 1L) < this->mpc_LinePath->GetLines().size()) &&
          (s32_Retval < this->mc_Points.size()))
      {
         //Insert new interaction point
         this->m_AddPointAt(s32_Retval, c_AdaptedPos);
         //Adapt visibility
         if ((this->mc_Points.size() > 0) && (this->mc_Points.size() > s32_Retval))
         {
            this->mc_Points[s32_Retval]->setVisible(this->mc_Points[0]->isVisible());
         }

         //Signal change
         this->TriggerSigChangedGraphic();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes a bending point

   \param[in] orc_ScenePos Position of the bending point
   \param[in] ops32_Index  Optional specific index which point to remove

   \return
   -1: error
   else: index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiLineGroup::RemoveBend(const QPointF & orc_ScenePos, const sint32 * const ops32_Index)
{
   const QVector<C_GiLiInteractionPoint *> & rc_VecPoints = this->mc_Points;

   sint32 s32_Retval = -1;

   this->prepareGeometryChange();

   if (ops32_Index == NULL)
   {
      QVector<C_GiLiInteractionPoint *>::const_iterator pc_ItPoint;
      float64 f64_ResultDist;
      float64 f64_MinDist = 1000000.0;

      // get the next point to the point
      for (pc_ItPoint = rc_VecPoints.begin(); pc_ItPoint != rc_VecPoints.end(); ++pc_ItPoint)
      {
         C_GiBiConnectableItem::h_DistToPoint((*pc_ItPoint)->scenePos(), orc_ScenePos, f64_ResultDist);

         if (f64_ResultDist < f64_MinDist)
         {
            // set the new minimum distance
            f64_MinDist = f64_ResultDist;
            // set the nearest point index
            s32_Retval = (*pc_ItPoint)->GetPointIndex();
         }
      }
   }
   else
   {
      s32_Retval = *ops32_Index;
   }

   if (s32_Retval >= 0)
   {
      //Only delete if there are enough points for a line left afterwards
      if (mc_Points.size() > 2L)
      {
         this->m_RemovePointAt(s32_Retval);
         this->mpc_LinePath->RemovePoint(s32_Retval);

         //Signal change
         this->TriggerSigChangedGraphic();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle point position change

   \param[in] ors32_PointIndex Index of changed point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::OnPointChange(const stw_types::sint32 & ors32_PointIndex)
{
   //Line(s)
   this->mpc_LinePath->UpdatePoint(ors32_PointIndex, this->mc_Points[ors32_PointIndex]->pos());

   //Signal change
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns an estimate of the area painted by the item

   \return     Rectangle
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiLiLineGroup::boundingRect() const
{
   return this->shape().boundingRect();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore all custom mouse cursors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::RestoreDefaultCursor(void)
{
   const QVector<C_GiLiLineConnection *> & rc_Lines = this->mpc_LinePath->GetLines();

   for (sintn sn_ItPoint = 0; sn_ItPoint < this->mc_Points.size(); ++sn_ItPoint)
   {
      this->mc_Points[sn_ItPoint]->RestoreDefaultCursor();
   }
   for (sintn sn_ItLine = 0; sn_ItLine < rc_Lines.size(); ++sn_ItLine)
   {
      rc_Lines[sn_ItLine]->RestoreDefaultCursor();
   }
   this->setCursor(Qt::ArrowCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwrite cursor

   \param[in] orc_TemporaryCursor Temporary cursor for item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetTemporaryCursor(const QCursor & orc_TemporaryCursor)
{
   const QVector<C_GiLiLineConnection *> & rc_Lines = this->mpc_LinePath->GetLines();

   for (sintn sn_ItPoint = 0; sn_ItPoint < this->mc_Points.size(); ++sn_ItPoint)
   {
      this->mc_Points[sn_ItPoint]->SetTemporaryCursor(orc_TemporaryCursor);
   }
   for (sintn sn_ItLine = 0; sn_ItLine < rc_Lines.size(); ++sn_ItLine)
   {
      rc_Lines[sn_ItLine]->SetTemporaryCursor(orc_TemporaryCursor);
   }
   this->setCursor(orc_TemporaryCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set default mouse cursor

   \param[in] orc_Value New default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetDefaultCursor(const QCursor & orc_Value)
{
   const QVector<C_GiLiLineConnection *> & rc_Lines = this->mpc_LinePath->GetLines();

   for (sintn sn_ItLine = 0; sn_ItLine < rc_Lines.size(); ++sn_ItLine)
   {
      rc_Lines[sn_ItLine]->SetDefaultCursor(orc_Value);
   }
   this->setCursor(orc_Value);
   this->mc_DefaultCursor = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the style of the provided element

   Warning: Only expected to work if the provided item is of the same type as this element

   \param[in] opc_GuidelineItem Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::CopyStyle(const QGraphicsItem * const opc_GuidelineItem)
{
   Q_UNUSED(opc_GuidelineItem)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetZValueCustom(const float64 of64_ZValue)
{
   this->setZValue(of64_ZValue);
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_InitPoint(const sint32 & ors32_Index, const QPointF & orc_Pos)
{
   QPointF c_AdaptedScenePos = this->mapToScene(orc_Pos);

   this->prepareGeometryChange();

   C_GiCustomFunctions::h_AdaptMouseRangePos(c_AdaptedScenePos);
   mc_Points.insert(ors32_Index, new C_GiLiInteractionPoint(ors32_Index, this));
   mc_Points[ors32_Index]->setPos(this->mapFromScene(c_AdaptedScenePos));

   //Update line when interaction point moving
   connect(mc_Points[ors32_Index], &C_GiLiInteractionPoint::PointChanged, this,
           &C_GiLiLineGroup::OnPointChange);

   //Update Point Index
   for (sint32 s32_ItPoint = ors32_Index; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
   {
      mc_Points[s32_ItPoint]->SetPointIndex(s32_ItPoint);
      this->addToGroup(mc_Points[s32_ItPoint]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper to handle point creation

   \param[in] ors32_Index Index of creation
   \param[in] orc_Pos     Position of new point in scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_AddPointAt(const stw_types::sint32 & ors32_Index, const QPointF & orc_Pos)
{
   if (ors32_Index >= 0L)
   {
      if (ors32_Index <= mc_Points.size())
      {
         this->m_InitPoint(ors32_Index, this->mapFromScene(orc_Pos));

         // adapt the lines
         this->mpc_LinePath->AddPoint(ors32_Index, this->mpc_LinePath->mapFromScene(orc_Pos));

         //Signal change
         this->TriggerSigChangedGraphic();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper to handle point removal

   \param[in] ors32_Index Index of point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_RemovePointAt(const stw_types::sint32 & ors32_Index)
{
   if (ors32_Index >= 0L)
   {
      if (ors32_Index < mc_Points.size())
      {
         //Update line when interaction point moving
         disconnect(mc_Points[ors32_Index], &C_GiLiInteractionPoint::PointChanged, this,
                    &C_GiLiLineGroup::OnPointChange);

         //Remove
         this->removeFromGroup(mc_Points[ors32_Index]);
         mc_Points[ors32_Index]->deleteLater();
         mc_Points.remove(ors32_Index);
         //Update Point Index
         for (sint32 s32_ItPoint = ors32_Index; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
         {
            mc_Points[s32_ItPoint]->SetPointIndex(s32_ItPoint);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function for initially loading basic internal data

   \param[in,out] orc_Data Data element to load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_LoadBasicData(const stw_opensyde_gui_logic::C_PuiBsLineBase & orc_Data)
{
   this->SetWidth(orc_Data.s32_UIWidthPixels);
   this->SetColor(orc_Data.c_UIColor);
   //Don't trigger data update at this point
   this->setZValue(orc_Data.f64_ZOrder);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enter basic line info

   \param[in,out] orc_Data Data element to update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_UpdateBasicData(stw_opensyde_gui_logic::C_PuiBsLineBase & orc_Data) const
{
   orc_Data.c_UIColor = this->GetColor();
   orc_Data.s32_UIWidthPixels = this->GetWidth();
   orc_Data.f64_ZOrder = this->zValue();
   orc_Data.c_UIInteractionPoints.clear();
   orc_Data.c_UIInteractionPoints.reserve(this->mc_Points.size());
   for (QVector<C_GiLiInteractionPoint *>::const_iterator pc_ItPoint = this->mc_Points.begin();
        pc_ItPoint != this->mc_Points.end();
        ++pc_ItPoint)
   {
      orc_Data.c_UIInteractionPoints.push_back((*pc_ItPoint)->scenePos());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize class

   \param[in] orc_Points Initial points
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::m_Init(const std::vector<QPointF> & orc_Points)
{
   this->mpc_LinePath->Init(orc_Points);
   //Points
   mc_Points.clear();
   mc_Points.reserve(orc_Points.size());
   for (uint32 u32_ItPoint = 0L; u32_ItPoint < orc_Points.size(); ++u32_ItPoint)
   {
      this->m_InitPoint(static_cast<sint32>(u32_ItPoint), orc_Points[u32_ItPoint]);
   }

   //Selection on creation
   m_SetInteractionVisibility(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten itemChange event slot

   \param[in,out] oe_Change Indicator what changed
   \param[in]     orc_Value Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiLiLineGroup::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   bool q_Restriction;
   QVariant c_Return;

   //Adapt bounding rect by moving all unrelated (unclicked) points in the opposite direction
   if (oe_Change == ItemPositionChange)
   {
      const QPointF c_Delta = orc_Value.toPointF() - this->pos();
      if (this->me_ActiveResizeMode == eLINE)
      {
         this->prepareGeometryChange();

         // move the line by reverting all other points
         for (sint32 s32_Counter = 0; s32_Counter < this->mc_Points.size(); ++s32_Counter)
         {
            if ((s32_Counter != this->msn_ActiveItemIndex) &&
                (s32_Counter != (this->msn_ActiveItemIndex + 1L)))
            {
               this->UpdatePoint(s32_Counter, this->mc_Points[s32_Counter]->scenePos() - c_Delta, true);
            }
         }
         if (this->mc_Points.size() == 2)
         {
            Q_EMIT this->SigSubItemMoves(this->msn_ActiveItemIndex, c_Delta);
         }
      }
      else if (this->me_ActiveResizeMode == ePOINT)
      {
         this->prepareGeometryChange();

         // move the point by reverting all other points
         for (sint32 s32_Counter = 0; s32_Counter < this->mc_Points.size(); ++s32_Counter)
         {
            if (s32_Counter != this->msn_ActiveItemIndex)
            {
               this->UpdatePoint(s32_Counter, this->mc_Points[s32_Counter]->scenePos() - c_Delta, true);
            }
         }
      }
      else
      {
         // Scene movement
         Q_EMIT this->SigSubItemMoves(this->msn_ActiveItemIndex, c_Delta);
      }
   }
   //Check if the new bounding rect intersects with any border
   c_Return = C_GiCustomFunctions::h_ItemChange(oe_Change, orc_Value, this, &q_Restriction);
   //Check if the new bounding rect ran into problems
   if (q_Restriction == true)
   {
      //Revert adapted bounding rect by moving all unrelated (unclicked) points by
      //the difference between the allowed distance and the initial distance
      if (oe_Change == ItemPositionChange)
      {
         const QPointF c_Delta = c_Return.toPointF() - orc_Value.toPointF();
         if (this->me_ActiveResizeMode == eLINE)
         {
            // move the line by reverting all other points
            for (sint32 s32_Counter = 0; s32_Counter < this->mc_Points.size(); ++s32_Counter)
            {
               if ((s32_Counter != this->msn_ActiveItemIndex) &&
                   (s32_Counter != (this->msn_ActiveItemIndex + 1L)))
               {
                  this->UpdatePoint(s32_Counter, this->mc_Points[s32_Counter]->scenePos() - c_Delta, true);
               }
            }
         }
         else if (this->me_ActiveResizeMode == ePOINT)
         {
            // move the point by reverting all other points
            for (sint32 s32_Counter = 0; s32_Counter < this->mc_Points.size(); ++s32_Counter)
            {
               if (s32_Counter != this->msn_ActiveItemIndex)
               {
                  this->UpdatePoint(s32_Counter, this->mc_Points[s32_Counter]->scenePos() - c_Delta, true);
               }
            }
         }
         else
         {
            //Unknown
         }
      }
   }
   c_Return = QGraphicsItemGroup::itemChange(oe_Change, c_Return);

   switch (oe_Change) //lint !e788 //All other cases handled by call of parent
   {
   case ItemSelectedHasChanged:
      if (orc_Value == false)
      {
         this->m_SetInteractionVisibility(false);
         this->me_ActiveResizeMode = eNO_ELEMENT;
      }
      break;
   case ItemPositionChange:
   case ItemPositionHasChanged:
      //Signal change
      this->TriggerSigChangedGraphic();
      break;
   default:
      break;
   }

   return c_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   this->mc_LastKnownPosition = this->pos();
   if (opc_Event->button() == Qt::LeftButton)
   {
      if (this->me_ActiveResizeMode != eALL)
      {
         QPointF c_Pos = opc_Event->scenePos();
         sint32 s32_Counter;
         bool q_FoundPoint = false;

         // check points
         for (s32_Counter = 0; s32_Counter < mc_Points.size(); ++s32_Counter)
         {
            if (this->mc_Points[s32_Counter]->IsPointInside(c_Pos) == true)
            {
               this->me_ActiveResizeMode = ePOINT;
               this->msn_ActiveItemIndex = s32_Counter;
               q_FoundPoint = true;

               Q_EMIT this->ClickActive(s32_Counter, true, c_Pos);
               break;
            }
         }

         // check lines
         if (q_FoundPoint == false)
         {
            const QVector<C_GiLiLineConnection *> & rc_Lines = this->mpc_LinePath->GetLines();

            for (s32_Counter = 0; s32_Counter < rc_Lines.size(); ++s32_Counter)
            {
               if (rc_Lines[s32_Counter]->contains(rc_Lines[s32_Counter]->mapFromScene(c_Pos)) == true)
               {
                  this->me_ActiveResizeMode = eLINE;
                  this->msn_ActiveItemIndex = s32_Counter;

                  break;
               }
            }
         }
         else
         {
            this->mc_LastKnownPosition = this->mc_Points[this->msn_ActiveItemIndex]->pos();
         }
      }
   }
   QGraphicsItemGroup::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse move event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (this->me_ActiveResizeMode == ePOINT)
   {
      m_CheckLineGrid(opc_Event->scenePos());
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
void C_GiLiLineGroup::mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (this->me_ActiveResizeMode == ePOINT)
   {
      Q_EMIT this->ClickActive(this->msn_ActiveItemIndex, false, opc_Event->scenePos());
      Q_EMIT this->SigItemWasResized(this->msn_ActiveItemIndex,
                                     this->mc_Points[this->msn_ActiveItemIndex]->pos() - this->mc_LastKnownPosition);
   }
   else if (this->me_ActiveResizeMode == eLINE)
   {
      if (this->pos() != this->mc_LastKnownPosition)
      {
         Q_EMIT this->SigSubItemWasMoved(this->msn_ActiveItemIndex, this->pos() - this->mc_LastKnownPosition);
      }
   }
   else
   {
      if (this->pos() != this->mc_LastKnownPosition)
      {
         Q_EMIT this->SigItemWasMoved(this->pos() - this->mc_LastKnownPosition);
      }
   }

   if (this->me_ActiveResizeMode != eALL)
   {
      // show the points again
      this->m_HideInteraction(false);

      this->mpc_LinePath->update();
      this->update();
      this->me_ActiveResizeMode = eNO_ELEMENT;
   }

   QGraphicsItemGroup::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the width of the line

   \param[in] ors32_Width New width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetWidth(const sint32 & ors32_Width)
{
   this->mpc_LinePath->SetWidth(ors32_Width);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the color of the main line

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetColor(const QColor & orc_Color)
{
   this->mpc_LinePath->SetColor(orc_Color);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the color of the middle line

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetMiddleLineColor(const QColor & orc_Color)
{
   this->mpc_LinePath->SetMiddleLineColor(orc_Color);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activates or Deactivates the middle line

   \param[in] orq_MiddleLine True: activate middle line, false: deactivate middle line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetMiddleLine(const bool & orq_MiddleLine)
{
   this->mpc_LinePath->SetMiddleLine(orq_MiddleLine);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return the width of the line

   \return  Width
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiLineGroup::GetWidth() const
{
   return this->mpc_LinePath->GetWidth();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return the color of the line

   \return  Color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiLiLineGroup::GetColor() const
{
   return this->mpc_LinePath->GetColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return the color of the middle line

   \return  Color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiLiLineGroup::GetMiddleLineColor() const
{
   return this->mpc_LinePath->GetMiddleLineColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return all used lines

   \return  vector with pointer to all used lines
*/
//----------------------------------------------------------------------------------------------------------------------
QVector<C_GiLiLineConnection *> C_GiLiLineGroup::GetLines() const
{
   return this->mpc_LinePath->GetLines();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return the flag about the usage of the middle line

   \return  Flag if the middle line is used
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiLineGroup::GetMiddleLine() const
{
   return this->mpc_LinePath->GetMiddleLine();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the number of used points

   \return
   Number of used points
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiLiLineGroup::GetNumberPoints(void) const
{
   return this->mc_Points.size();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the position

   \return
   Position
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_GiLiLineGroup::GetPos(void) const
{
   QPointF c_Pos;
   const QVector<C_GiLiLineConnection *> & rc_VecLines = this->mpc_LinePath->GetLines();

   // check if there is a line
   if (rc_VecLines.size() > 0)
   {
      c_Pos = rc_VecLines[0]->GetPos();
   }
   else
   {
      // error case
      c_Pos.setX(0.0);
      c_Pos.setY(0.0);
   }

   return c_Pos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set item disabled look flag

   \param[in] oq_Disabled Flag if item is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetDisabledLook(const bool oq_Disabled)
{
   this->mpc_LinePath->SetDisabledLook(oq_Disabled);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set animation status

   \param[in] oq_Active          Animation active flag
   \param[in] oq_Inverse         Optional flag to animate in inverse direction
   \param[in] oq_SpeedUp         Optional flag to speed up the animation
   \param[in] oc_Polygon         Optional polygon to use for animation
   \param[in] oq_ShowOrignalLine Optional flag to hide or show the original line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetAnimated(const bool oq_Active, const bool oq_Inverse, const bool oq_SpeedUp,
                                  const QPolygonF oc_Polygon, const bool oq_ShowOrignalLine)
{
   if (this->mpc_LinePath != NULL)
   {
      this->mpc_LinePath->SetAnimated(oq_Active, oq_Inverse, oq_SpeedUp, oc_Polygon, oq_ShowOrignalLine);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Flag for displaying interaction points and handling multiselection mode

   false:
   Deactivate interaction points
   Move complete object

   \param[in] orq_ResizeActive Flag if resize mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::SetResizing(const bool & orq_ResizeActive)
{
   this->m_SetInteractionVisibility(orq_ResizeActive);
   if (orq_ResizeActive == false)
   {
      me_ActiveResizeMode = eALL;
   }
   else
   {
      if (me_ActiveResizeMode == eALL)
      {
         me_ActiveResizeMode = eNO_ELEMENT;
      }
   }
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find closest point in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] orc_Closest    Closest point in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const
{
   float64 f64_Best = std::numeric_limits<float64>::max();

   orc_Closest = orc_ScenePoint;
   if (mc_Points.size() > 0)
   {
      QPointF c_CurProj;
      float64 f64_CurDist;
      QPointF c_CurP2;
      QPointF c_CurP1 = mc_Points[0]->scenePos();
      for (sint32 s32_ItPoint = 1; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
      {
         c_CurP2 = mc_Points[s32_ItPoint]->scenePos();
         C_GiBiConnectableItem::h_DistToLine(c_CurP1, c_CurP2, orc_ScenePoint, &f64_CurDist, &c_CurProj, NULL);
         c_CurP1 = c_CurP2;
         if (f64_CurDist < f64_Best)
         {
            f64_Best = f64_CurDist;
            orc_Closest = c_CurProj;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find closest connection in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] ors32_Index    Index of closest connection in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::FindClosestConnection(const QPointF & orc_ScenePoint, sint32 & ors32_Index) const
{
   if (this->mpc_LinePath != NULL)
   {
      float64 f64_Best = std::numeric_limits<float64>::max();
      const QVector<C_GiLiLineConnection *> & rc_Lines = this->mpc_LinePath->GetLines();

      ors32_Index = -1;
      if (mc_Points.size() > 0)
      {
         float64 f64_CurDist;
         QPointF c_CurP2;
         QPointF c_CurP1 = mc_Points[0]->scenePos();
         for (sint32 s32_ItPoint = 1; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
         {
            c_CurP2 = mc_Points[s32_ItPoint]->scenePos();
            C_GiBiConnectableItem::h_DistToLine(c_CurP1, c_CurP2, orc_ScenePoint, &f64_CurDist, NULL, NULL);
            c_CurP1 = c_CurP2;
            if (f64_CurDist < f64_Best)
            {
               f64_Best = f64_CurDist;
               if (rc_Lines.size() > (s32_ItPoint - 1))
               {
                  ors32_Index = s32_ItPoint - 1;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper to handle point update

   \param[in] ors32_Index                    Index of creation
   \param[in] orc_Pos                        Position of new point in scene
   \param[in] oq_BlockTriggerOfChangedSignal Optional flag to disable any changed signals while executing this function
                                             Use-case: avoid changed signals while moving single points
                                             Warning: external call of TriggerSigChangedGraphic will become necessary
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::UpdatePoint(const sint32 & ors32_Index, const QPointF & orc_Pos,
                                  const bool oq_BlockTriggerOfChangedSignal)
{
   if (((ors32_Index >= 0) && (ors32_Index < this->mc_Points.size())) && (this->mpc_LinePath != NULL))
   {
      QPointF c_RestrictedPos = orc_Pos;

      //Block any change triggers
      if (oq_BlockTriggerOfChangedSignal)
      {
         this->mq_BlockChangeSignal = true;
      }

      this->prepareGeometryChange();
      C_GiCustomFunctions::h_AdaptMouseRangePos(c_RestrictedPos);
      c_RestrictedPos = this->mapFromScene(c_RestrictedPos);
      this->mc_Points[ors32_Index]->setPos(c_RestrictedPos);
      this->mpc_LinePath->UpdatePoint(ors32_Index, c_RestrictedPos);

      //Reactivate changed signals
      if (oq_BlockTriggerOfChangedSignal)
      {
         this->mq_BlockChangeSignal = false;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get point position

   \param[in] os32_Index Point index

   \return
   Point scene position
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_GiLiLineGroup::GetPointScenePos(const sint32 os32_Index) const
{
   QPointF c_Retval;

   this->GetPointPos(os32_Index, c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get point position

   \param[in]  ors32_Index Point index
   \param[out] orc_Pos     Point scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::GetPointPos(const sint32 & ors32_Index, QPointF & orc_Pos) const
{
   if ((ors32_Index >= 0) && (ors32_Index < this->mc_Points.size()))
   {
      orc_Pos = this->mc_Points[ors32_Index]->scenePos();
   }
   else
   {
      orc_Pos = QPointF(0.0, 0.0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if there is a bend point at the specified scene position

   \param[in] orc_ScenePoint Scene position to check

   \return
   true:  Bend point found
   false: No bend point at this position
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiLineGroup::CheckBendPointAt(const QPointF & orc_ScenePoint) const
{
   bool q_Retval = false;

   // check points
   for (sint32 s32_Counter = 0; s32_Counter < mc_Points.size(); ++s32_Counter)
   {
      if (this->mc_Points[s32_Counter]->IsPointInside(orc_ScenePoint) == true)
      {
         q_Retval = true;
         break;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper function to trigger changed graphic manually to avoid signals on every possible change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::TriggerSigChangedGraphic(void)
{
   //Only allow changed signal if no explicit block active
   if (this->mq_BlockChangeSignal == false)
   {
      C_GiBiConnectableItem::TriggerSigChangedGraphic();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten paint event slot

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                            QWidget * const opc_Widget)
{
   Q_UNUSED(opc_Option)
   Q_UNUSED(opc_Widget)

   // do not call the original function to avoid drawing the bounding rect
   /* Draw bounding */
   if ((this->isSelected()) && (this->me_ActiveResizeMode == eALL))
   {
      opc_Painter->setPen(QPen(mc_STYLE_GUIDE_COLOR_21));
      opc_Painter->setBrush(Qt::NoBrush);
      opc_Painter->drawPath(this->shape());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return more accurate shape

   \return  The painter path as polygon
*/
//----------------------------------------------------------------------------------------------------------------------
QPainterPath C_GiLiLineGroup::shape(void) const
{
   QPainterPath c_Retval;

   if (this->mpc_LinePath != NULL)
   {
      c_Retval = this->mpc_LinePath->shape();
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Signal for update of current scaling

   \param[in] orc_Transform Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLineGroup::UpdateTransform(const QTransform & orc_Transform)
{
   if (this->mpc_LinePath != NULL)
   {
      float64 f64_Width = 0.0;
      C_GiPointInteraction * pc_ActionPoint;

      this->prepareGeometryChange();
      for (QVector<C_GiLiInteractionPoint *>::iterator pc_ItActionPoint = this->mc_Points.begin();
           pc_ItActionPoint != this->mc_Points.end(); ++pc_ItActionPoint)
      {
         pc_ActionPoint = *pc_ItActionPoint;
         if (pc_ActionPoint != NULL)
         {
            pc_ActionPoint->UpdateTransform(orc_Transform);
            f64_Width = pc_ActionPoint->sceneBoundingRect().width();
         }
      }

      this->mpc_LinePath->SetInteractionWidth(f64_Width);
   }
}
