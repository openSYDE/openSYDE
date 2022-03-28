//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete line as QGraphicsPathItem for the clean visualisation of bended lines (implementation)

   Complete line

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cmath>
#include <limits>
#include <QPainter>

#include "constants.h"
#include "C_GiCustomFunctions.h"
#include "C_GiLiLine.h"
#include "gitypes.h"
#include "C_GiBiLineBounding.h"
#include "C_OSCUtils.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::uint8 C_GiLiLine::mhu8_ANIMATION_STEP_COUNT = 60;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     opc_Points     Points for line
   \param[in]     orq_MiddleLine Indicator if line(s) should have a different colored central line
   \param[in,out] opc_Parent     Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiLine::C_GiLiLine(const std::vector<QPointF> * const opc_Points, const bool & orq_MiddleLine,
                       QGraphicsItem * const opc_Parent) :
   QGraphicsPathItem(opc_Parent),
   mq_MiddleLine(orq_MiddleLine),
   mc_LineColor(QColor(224, 159, 90)),
   mc_MiddleLineColor(Qt::white),
   mf64_OffsetStart(0.0),
   mf64_OffsetEnd(0.0),
   mf64_InteractionWidth(0.0),
   mq_DisabledLook(false),
   mq_Animated(false),
   mq_InverseAnimation(false),
   mq_ShowOriginalLine(true),
   mq_SpeedUpAnimation(false),
   mu8_AnimationStep(0),
   mc_AnimationPackageColor(QColor(156, 60, 60))
{
   //Pen (FIRST!)
   {
      QPen c_Pen = this->pen();
      c_Pen.setWidth(10);
      if (this->mq_MiddleLine)
      {
         c_Pen.setColor(QColor(115, 115, 185));
      }
      else
      {
         c_Pen.setColor(QColor(224, 159, 90));
      }
      c_Pen.setCapStyle(Qt::FlatCap);
      c_Pen.setJoinStyle(Qt::RoundJoin);
      this->setPen(c_Pen);
   }

   if (opc_Points != NULL)
   {
      Init(*opc_Points);
   }

   //Animation timer
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_GiLiLine::m_UpdateAnimation);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiLine::~C_GiLiLine()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize class

   \param[in] orc_Points Initial points
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::Init(const std::vector<QPointF> & orc_Points)
{
   QPointF c_AdaptedScenePos;

   //Points
   this->mc_Points.clear();
   this->mc_Points.reserve(orc_Points.size());
   for (uint32 u32_ItPoint = 0; u32_ItPoint < orc_Points.size(); ++u32_ItPoint)
   {
      c_AdaptedScenePos = this->mapToScene(orc_Points[u32_ItPoint]);
      C_GiCustomFunctions::h_AdaptMouseRangePos(c_AdaptedScenePos);
      this->mc_Points.insert(u32_ItPoint, this->mapFromScene(c_AdaptedScenePos));
   }

   //Lines
   mc_Lines.clear();
   mc_Lines.reserve(mc_Points.size() - 1);
   for (sint32 s32_ItPoint = 0L; (s32_ItPoint + 1) < mc_Points.size(); ++s32_ItPoint)
   {
      this->m_AddLineAt(s32_ItPoint);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiLiLine::type() const
{
   return msn_GRAPHICS_ITEM_LINE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper to handle line creation

   \param[in] ors32_Index Index of creation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::m_AddLineAt(const stw_types::sint32 & ors32_Index)
{
   if (ors32_Index >= 0L)
   {
      if (ors32_Index <= mc_Lines.size())
      {
         mc_Lines.insert(ors32_Index, new C_GiLiLineConnection(ors32_Index, this));
         m_UpdateLine(ors32_Index - 1L);
         m_UpdateLine(ors32_Index);

         //Get consistent look
         mc_Lines[ors32_Index]->SetColor(this->GetColor());
         mc_Lines[ors32_Index]->SetWidth(this->GetWidth());
         mc_Lines[ors32_Index]->setOpacity(0.01);
         //Update Line Index
         for (sint32 s32_ItLine = ors32_Index; s32_ItLine < mc_Lines.size(); ++s32_ItLine)
         {
            mc_Lines[s32_ItLine]->SetLineIndex(s32_ItLine);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper to handle line removal

   \param[in] ors32_Index Index of line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::m_RemoveLineAt(const sint32 & ors32_Index)
{
   if (ors32_Index >= 0L)
   {
      if (ors32_Index < mc_Lines.size())
      {
         //Remove
         mc_Lines[ors32_Index]->deleteLater();
         mc_Lines.remove(ors32_Index);
         //Update Line Index
         for (sint32 s32_ItLine = ors32_Index; s32_ItLine < mc_Lines.size(); ++s32_ItLine)
         {
            mc_Lines[s32_ItLine]->SetLineIndex(s32_ItLine);
         }
         //Signal change
         this->TriggerSigChangedGraphic();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Helper to handle line update

   \param[in] ors32_LineIndex Index of changed line
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::m_UpdateLine(const stw_types::sint32 & ors32_LineIndex)
{
   if (ors32_LineIndex >= 0L)
   {
      if ((ors32_LineIndex < mc_Lines.size()) && (static_cast<sintn>(ors32_LineIndex + 1L) < mc_Points.size()))
      {
         //Deactivate signal to avoid to trigger change of line again
         mc_Lines[ors32_LineIndex]->AdaptLine(QLineF(-mc_Lines[ors32_LineIndex]->pos() +
                                                     mc_Points[ors32_LineIndex],
                                                     -mc_Lines[ors32_LineIndex]->pos() +
                                                     mc_Points[static_cast<sintn>(ors32_LineIndex + 1L)]));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply offset to line ends

   \param[in,out] orc_Polygon Polygon to adapt
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::m_ApplyOffset(QPolygonF & orc_Polygon) const
{
   if (orc_Polygon.size() >= 2)
   {
      //First line
      QPointF c_LineStart;
      QPointF c_LineEnd;
      QPointF c_LineDiff;
      QPointF c_AdaptedLineDiff;
      if (C_OSCUtils::h_IsFloat64NearlyEqual(this->mf64_OffsetStart, 0.0) == false)
      {
         c_LineStart = orc_Polygon[0];
         c_LineEnd = orc_Polygon[1];
         c_LineDiff = c_LineEnd - c_LineStart;
         c_AdaptedLineDiff = stw_opensyde_gui_logic::C_GiBiLineBounding::h_AdaptVecToWidth(c_LineDiff,
                                                                                           this->mf64_OffsetStart);
         orc_Polygon[0] = c_LineStart + c_AdaptedLineDiff;
      }

      if (C_OSCUtils::h_IsFloat64NearlyEqual(this->mf64_OffsetEnd, 0.0) == false)
      {
         //Second line
         c_LineStart = orc_Polygon[orc_Polygon.size() - 2];
         c_LineEnd = orc_Polygon[orc_Polygon.size() - 1];
         c_LineDiff = c_LineStart - c_LineEnd;
         c_AdaptedLineDiff = stw_opensyde_gui_logic::C_GiBiLineBounding::h_AdaptVecToWidth(c_LineDiff,
                                                                                           this->mf64_OffsetEnd);
         orc_Polygon[orc_Polygon.size() - 1] = c_LineEnd + c_AdaptedLineDiff;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  First draft for animation steps
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::m_UpdateAnimation(void)
{
   if (this->mc_Points.size() > 0)
   {
      ++this->mu8_AnimationStep;
      if (this->mu8_AnimationStep >= C_GiLiLine::mhu8_ANIMATION_STEP_COUNT)
      {
         this->mu8_AnimationStep = 0;
      }
      this->update();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set color

   \param[in] orc_Color     New color
   \param[in] oq_SendUpdate Send update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::m_SetColor(const QColor & orc_Color, const bool oq_SendUpdate)
{
   QPen c_Pen = this->pen();

   c_Pen.setColor(orc_Color);
   for (sint32 s32_ItLine = 0; s32_ItLine < mc_Lines.size(); ++s32_ItLine)
   {
      mc_Lines[s32_ItLine]->SetColor(orc_Color);
   }
   mc_AnimationPackageColor = QColor(255 - orc_Color.red(), 255 - orc_Color.green(), 255 - orc_Color.blue());
   this->setPen(c_Pen);
   //Signal change
   if (oq_SendUpdate == true)
   {
      this->TriggerSigChangedGraphic();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get desired animation step count

   \return
   Desired animation step count
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_GiLiLine::m_GetAnimationStepCount(void) const
{
   uint8 u8_Retval;

   if (this->mq_SpeedUpAnimation == true)
   {
      u8_Retval = C_GiLiLine::mhu8_ANIMATION_STEP_COUNT * 4U;
   }
   else
   {
      u8_Retval = C_GiLiLine::mhu8_ANIMATION_STEP_COUNT;
   }
   return u8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set interaction point width

   \param[in] orf64_Value New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetInteractionWidth(const stw_types::float64 & orf64_Value)
{
   this->prepareGeometryChange();
   this->mf64_InteractionWidth = orf64_Value;
   for (QVector<C_GiLiLineConnection *>::iterator pc_ItLine = this->mc_Lines.begin(); pc_ItLine != this->mc_Lines.end();
        ++pc_ItLine)
   {
      C_GiLiLineConnection * const pc_Line = *pc_ItLine;
      if (pc_Line != NULL)
      {
         pc_Line->SetInteractionWidth(orf64_Value);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new point with specific coordinates

   \param[in] ors32_Index Index of changed point
   \param[in] orc_Pos     New position of point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::AddPoint(const sint32 & ors32_Index, const QPointF & orc_Pos)
{
   this->mc_Points.insert(ors32_Index, orc_Pos);

   this->m_AddLineAt(ors32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update of specific point coordinates

   \param[in] ors32_Index Index of changed point
   \param[in] orc_Pos     New position of point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::UpdatePoint(const sint32 & ors32_Index, const QPointF & orc_Pos)
{
   if (this->mc_Points.size() > ors32_Index)
   {
      this->mc_Points[ors32_Index] = orc_Pos;

      //previous line
      this->m_UpdateLine(ors32_Index - 1L);
      //next line
      this->m_UpdateLine(ors32_Index);

      //Update bounding rect
      {
         QPainterPath c_LinePath;
         const QPolygonF c_Polygon(this->mc_Points);
         c_LinePath.addPolygon(c_Polygon);
         this->setPath(c_LinePath);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes a specific point

   \param[in] ors32_Index Index of point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::RemovePoint(const sint32 & ors32_Index)
{
   //Only delete if there are enough points for a line left afterwards
   if (this->mc_Points.size() > 2L)
   {
      this->mc_Points.remove(ors32_Index);

      this->m_RemoveLineAt(ors32_Index - 1L);
      //update last line
      this->m_UpdateLine(ors32_Index - 1L);
      //if first delete next line instead
      if (ors32_Index == 0L)
      {
         this->m_RemoveLineAt(ors32_Index);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set line width

   \param[in] ors32_Width New line width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetWidth(const stw_types::sint32 & ors32_Width)
{
   QPen c_Pen = this->pen();

   c_Pen.setWidth(ors32_Width);
   for (sint32 s32_ItLine = 0; s32_ItLine < mc_Lines.size(); ++s32_ItLine)
   {
      mc_Lines[s32_ItLine]->SetWidth(ors32_Width);
   }
   this->setPen(c_Pen);
   //Signal change
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set color

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetColor(const QColor & orc_Color)
{
   this->mc_LineColor = orc_Color;

   this->m_SetColor(orc_Color, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set middle line color

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetMiddleLineColor(const QColor & orc_Color)
{
   this->mc_MiddleLineColor = orc_Color;
   //Signal change
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set middle line active

   \param[in] orq_MiddleLine New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetMiddleLine(const bool & orq_MiddleLine)
{
   this->mq_MiddleLine = orq_MiddleLine;
   //Signal change
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set line style

   \param[in] ore_Type New line style
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetLineStyle(const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType & ore_Type)
{
   QPen c_Pen = this->pen();

   switch (ore_Type)
   {
   case stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType::eDASH:
      c_Pen.setStyle(Qt::DashLine);
      break;
   case stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType::eDOT:
      c_Pen.setStyle(Qt::DotLine);
      break;
   case stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType::eDASH_DOT:
      c_Pen.setStyle(Qt::DashDotLine);
      break;
   case stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType::eDASH_DOT_DOT:
      c_Pen.setStyle(Qt::DashDotDotLine);
      break;
   case stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType::eSOLID:
   default:
      c_Pen.setStyle(Qt::SolidLine);
      break;
   }
   this->setPen(c_Pen);
   //Signal change
   this->TriggerSigChangedGraphic();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set offset for line start

   \param[in] orf64_Offset New offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetOffsetStart(const float64 & orf64_Offset)
{
   this->mf64_OffsetStart = orf64_Offset;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set offset for line end

   \param[in] orf64_Offset New offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetOffsetEnd(const float64 & orf64_Offset)
{
   this->mf64_OffsetEnd = orf64_Offset;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set item disabled look flag

   \param[in] oq_Disabled Flag if item is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::SetDisabledLook(const bool oq_Disabled)
{
   if (oq_Disabled == true)
   {
      this->m_SetColor(C_Uti::h_GetDisabledColorLook(this->mc_LineColor), false);
   }
   else
   {
      this->m_SetColor(this->mc_LineColor, false);
   }

   this->mq_DisabledLook = oq_Disabled;
   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get width

   \return Line width
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiLine::GetWidth() const
{
   return this->pen().width();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get color

   \return Line color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiLiLine::GetColor() const
{
   return this->pen().color();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get middle line color

   \return Middle line color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiLiLine::GetMiddleLineColor() const
{
   return this->mc_MiddleLineColor;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get lines

   \return  Interactable segments of line
*/
//----------------------------------------------------------------------------------------------------------------------
QVector<C_GiLiLineConnection *> C_GiLiLine::GetLines(void) const
{
   return mc_Lines;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get middle line indicator

   \retval  true     has middle line
   \retval  false    no middle line
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiLine::GetMiddleLine(void) const
{
   return mq_MiddleLine;
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
void C_GiLiLine::SetAnimated(const bool oq_Active, const bool oq_Inverse, const bool oq_SpeedUp,
                             const QPolygonF oc_Polygon, const bool oq_ShowOrignalLine)
{
   this->mq_Animated = oq_Active;
   this->mq_SpeedUpAnimation = oq_SpeedUp;
   this->mq_InverseAnimation = oq_Inverse;
   this->mq_ShowOriginalLine = oq_ShowOrignalLine;
   this->mc_AnimationPolygon = oc_Polygon;
   if (this->mq_Animated == true)
   {
      if (this->mc_Timer.isActive() == false)
      {
         //Update interval
         this->mc_Timer.setInterval(1000 / static_cast<sintn>(this->m_GetAnimationStepCount()));
         //Start timer
         this->mc_Timer.start();
      }
   }
   else
   {
      this->mc_Timer.stop();
      this->update();
   }
   //Use original color if only original line is visible
   if (this->mq_ShowOriginalLine == false)
   {
      this->mc_AnimationPackageColor = this->GetColor();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten paint event slot

   Here: Draw segmented line

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                       QWidget * const opc_Widget)
{
   QPainterPath c_LinePath;
   QPen c_Pen;

   {
      QPolygonF c_Polygon(this->mc_Points);
      m_ApplyOffset(c_Polygon);
      c_LinePath.addPolygon(c_Polygon);
      this->setPath(c_LinePath);

      if (this->mq_ShowOriginalLine == true)
      {
         QGraphicsPathItem::paint(opc_Painter, opc_Option, opc_Widget);

         if (this->mq_MiddleLine)
         {
            opc_Painter->save();
            c_Pen = this->pen();
            c_Pen.setWidth(std::max(1, c_Pen.width() / 3));
            if (this->mq_DisabledLook == true)
            {
               c_Pen.setColor(C_Uti::h_GetDisabledColorLook(mc_MiddleLineColor));
            }
            else
            {
               c_Pen.setColor(mc_MiddleLineColor);
            }
            opc_Painter->setPen(c_Pen);
            opc_Painter->drawPath(c_LinePath);
            opc_Painter->restore();
         }
      }

      //Draw current animation on top of existing line
      if (this->mq_Animated == true)
      {
         //Alternative pattern
         //const float64 f64_Length = 200;
         //QVector<qreal> c_Pattern;
         //Point 1
         //c_Pattern.push_back(static_cast<float64>(this->mu8_AnimationStep) /
         //                    static_cast<float64>(C_GiLiLine::mhu8_AnimationStepCount) * f64_Length);
         //Long pause
         //c_Pattern.push_back(f64_Length - c_Pattern[0]);
         //Animated package color
         //QPen c_Pen(QBrush(C_GiLiLine::mhc_AnimationPackageColor), this->GetWidth(), Qt::CustomDashLine);
         //Offset representing the current animation state, f64_Length = sum of pattern
         //c_Pen.setDashOffset(static_cast<float64>(this->mu8_AnimationStep) *
         //                    (f64_Length / static_cast<float64>(C_GiLiLine::mhu8_AnimationStepCount)));
         //c_Pen.setDashPattern(c_Pattern);

         QVector<qreal> c_Pattern;
         float64 f64_Offset;
         QColor c_AnimatedColor;
         const uint8 u8_ANIMATION_STEP_COUNT_OVERRIDE = C_GiLiLine::mhu8_ANIMATION_STEP_COUNT;
         const float64 f64_PenWidth = static_cast<float64>(this->GetWidth());

         if (this->mq_MiddleLine == true)
         {
            c_AnimatedColor = this->GetColor();
         }
         else
         {
            c_AnimatedColor = C_GiLiLine::mc_AnimationPackageColor;
         }
         //Point 1
         c_Pattern.push_back(24.0 / f64_PenWidth);
         //Long pause
         c_Pattern.push_back(24.0 / f64_PenWidth);
         //Animated package color
         c_Pen =
            QPen(static_cast<QBrush>(c_AnimatedColor),
                 static_cast<qreal>(this->GetWidth()), Qt::CustomDashLine, Qt::FlatCap, Qt::RoundJoin);
         //Offset representing the current animation state, 48 / f64_PenWidth = sum of pattern
         if (this->mq_InverseAnimation == false)
         {
            f64_Offset = static_cast<float64>(this->mu8_AnimationStep) *
                         ((48.0 / f64_PenWidth) / static_cast<float64>(u8_ANIMATION_STEP_COUNT_OVERRIDE));
         }
         else
         {
            const uint8 u8_StepsTillFinished = u8_ANIMATION_STEP_COUNT_OVERRIDE - this->mu8_AnimationStep;
            f64_Offset = static_cast<float64>(u8_StepsTillFinished) *
                         ((48.0 / f64_PenWidth) / static_cast<float64>(u8_ANIMATION_STEP_COUNT_OVERRIDE));
         }
         c_Pen.setDashOffset(f64_Offset);
         c_Pen.setDashPattern(c_Pattern);
         opc_Painter->save();
         opc_Painter->setPen(c_Pen);
         if (this->mc_AnimationPolygon.size() == 0)
         {
            opc_Painter->drawPath(c_LinePath);
         }
         else
         {
            QPainterPath c_Path;
            c_Path.addPolygon(this->mc_AnimationPolygon);
            opc_Painter->drawPath(c_Path);
         }
         opc_Painter->restore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return more accurate shape

   \return  The painter path as polygon
*/
//----------------------------------------------------------------------------------------------------------------------
QPainterPath C_GiLiLine::shape(void) const
{
   stw_opensyde_gui_logic::C_GiBiLineBounding c_LineBounding(this->mc_Points,
                                                             static_cast<stw_types::float64>(this->GetWidth()),
                                                             this->mf64_InteractionWidth);
   return c_LineBounding.GetShape();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find closest point in shape to scene position

   \param[in]  orc_ScenePoint Scene position
   \param[out] orc_Closest    Closest point in shape
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiLine::FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const
{
   float64 f64_Best = std::numeric_limits<float64>::max();

   orc_Closest = orc_ScenePoint;
   if (mc_Points.size() > 0)
   {
      QPointF c_CurProj;
      float64 f64_CurDist;
      QPointF c_CurP2;
      QPointF c_CurP1 = mc_Points[0];
      for (sint32 s32_ItPoint = 1; s32_ItPoint < mc_Points.size(); ++s32_ItPoint)
      {
         c_CurP2 = mc_Points[s32_ItPoint];
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
