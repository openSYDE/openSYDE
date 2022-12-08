//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete line as QGraphicsPathItem for the clean visualisation of bended lines (header)

   Complete line (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILILINE_H
#define C_GILILINE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTimer>
#include <QObject>
#include <QPolygonF>
#include <QFocusEvent>
#include <QGraphicsPathItem>
#include "C_GiBiConnectableItem.hpp"
#include "C_GiLiInteractionPoint.hpp"
#include "C_GiLiLineConnection.hpp"
#include "C_PuiBsLineArrow.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiLine :
   public C_GiBiConnectableItem,
   public QGraphicsPathItem
{
public:
   C_GiLiLine(const std::vector<QPointF> * const opc_Points = NULL, const bool & orq_MiddleLine = false,
              QGraphicsItem * const opc_Parent = NULL);
   ~C_GiLiLine() override;
   void Init(const std::vector<QPointF> & orc_Points);

   int32_t type() const override;

   void AddPoint(const int32_t & ors32_Index, const QPointF & orc_Pos);
   void UpdatePoint(const int32_t & ors32_Index, const QPointF & orc_Pos);
   void RemovePoint(const int32_t & ors32_Index);

   void SetWidth(const int32_t & ors32_Width);
   void SetColor(const QColor & orc_Color);
   void SetMiddleLineColor(const QColor & orc_Color);
   void SetMiddleLine(const bool & orq_MiddleLine);
   void SetLineStyle(const stw::opensyde_gui_logic::C_PuiBsLineArrow::E_LineType & ore_Type);
   void SetOffsetStart(const float64_t & orf64_Offset);
   void SetOffsetEnd(const float64_t & orf64_Offset);
   void SetDisabledLook(const bool oq_Disabled);

   int32_t GetWidth(void) const;
   QColor GetColor(void) const;
   QColor GetMiddleLineColor(void) const;
   QVector<C_GiLiLineConnection *> GetLines(void) const;
   bool GetMiddleLine(void) const;

   void SetAnimated(const bool oq_Active, const bool oq_Inverse = false, const bool oq_SpeedUp = false,
                    const QPolygonF oc_Polygon = QPolygonF(), const bool oq_ShowOrignalLine = true);

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget = NULL) override;

   QPainterPath shape(void) const override;

   void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const override;

   void SetInteractionWidth(const float64_t & orf64_Value);

protected:
   QVector<QPointF> mc_Points;               ///< Interaction points of segmented line
   QVector<C_GiLiLineConnection *> mc_Lines; ///< Interactable line segments of segmented line

private:
   //Avoid call
   C_GiLiLine(const C_GiLiLine &);
   C_GiLiLine & operator =(const C_GiLiLine &) &; //lint !e1511 //we want to hide the base func.

   void m_AddLineAt(const int32_t & ors32_Index);
   void m_RemoveLineAt(const int32_t & ors32_Index);
   void m_UpdateLine(const int32_t & ors32_LineIndex);
   void m_ApplyOffset(QPolygonF & orc_Polygon) const;
   void m_UpdateAnimation(void);
   void m_SetColor(const QColor & orc_Color, const bool oq_SendUpdate);
   uint8_t m_GetAnimationStepCount(void) const;

   bool mq_MiddleLine;        ///< Indicator if middle line should be drawn
   QColor mc_LineColor;       ///< Main line color
   QColor mc_MiddleLineColor; ///< Middle line color
   float64_t mf64_OffsetStart;
   float64_t mf64_OffsetEnd;
   float64_t mf64_InteractionWidth;
   bool mq_DisabledLook;
   bool mq_Animated;
   bool mq_InverseAnimation;
   bool mq_ShowOriginalLine;
   bool mq_SpeedUpAnimation;
   QTimer mc_Timer;
   uint8_t mu8_AnimationStep;
   static const uint8_t mhu8_ANIMATION_STEP_COUNT;
   QColor mc_AnimationPackageColor;
   QPolygonF mc_AnimationPolygon;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
