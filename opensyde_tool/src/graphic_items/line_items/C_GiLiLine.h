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
#include "C_GiBiConnectableItem.h"
#include "C_GiLiInteractionPoint.h"
#include "C_GiLiLineConnection.h"
#include "C_PuiBsLineArrow.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
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
   virtual ~C_GiLiLine();
   void Init(const std::vector<QPointF> & orc_Points);

   virtual stw_types::sintn type() const override;

   void AddPoint(const stw_types::sint32 & ors32_Index, const QPointF & orc_Pos);
   void UpdatePoint(const stw_types::sint32 & ors32_Index, const QPointF & orc_Pos);
   void RemovePoint(const stw_types::sint32 & ors32_Index);

   void SetWidth(const stw_types::sint32 & ors32_Width);
   void SetColor(const QColor & orc_Color);
   void SetMiddleLineColor(const QColor & orc_Color);
   void SetMiddleLine(const bool & orq_MiddleLine);
   void SetLineStyle(const stw_opensyde_gui_logic::C_PuiBsLineArrow::E_LineType & ore_Type);
   void SetOffsetStart(const stw_types::float64 & orf64_Offset);
   void SetOffsetEnd(const stw_types::float64 & orf64_Offset);
   void SetDisabledLook(const bool oq_Disabled);

   stw_types::sint32 GetWidth(void) const;
   QColor GetColor(void) const;
   QColor GetMiddleLineColor(void) const;
   QVector<C_GiLiLineConnection *> GetLines(void) const;
   bool GetMiddleLine(void) const;

   void SetAnimated(const bool oq_Active, const bool oq_Inverse = false, const bool oq_SpeedUp = false,
                    const QPolygonF oc_Polygon = QPolygonF(), const bool oq_ShowOrignalLine = true);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget = NULL) override;

   virtual QPainterPath shape(void) const;

   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const;

   void SetInteractionWidth(const stw_types::float64 & orf64_Value);

protected:
   QVector<QPointF> mc_Points;               ///< Interaction points of segmented line
   QVector<C_GiLiLineConnection *> mc_Lines; ///< Interactable line segments of segmented line

private:
   //Avoid call
   C_GiLiLine(const C_GiLiLine &);
   C_GiLiLine & operator =(const C_GiLiLine &); //lint !e1511 //we want to hide the base func.

   void m_AddLineAt(const stw_types::sint32 & ors32_Index);
   void m_RemoveLineAt(const stw_types::sint32 & ors32_Index);
   void m_UpdateLine(const stw_types::sint32 & ors32_LineIndex);
   void m_ApplyOffset(QPolygonF & orc_Polygon) const;
   void m_UpdateAnimation(void);
   void m_SetColor(const QColor & orc_Color, const bool oq_SendUpdate);
   stw_types::uint8 m_GetAnimationStepCount(void) const;

   bool mq_MiddleLine;        ///< Indicator if middle line should be drawn
   QColor mc_LineColor;       ///< Main line color
   QColor mc_MiddleLineColor; ///< Middle line color
   stw_types::float64 mf64_OffsetStart;
   stw_types::float64 mf64_OffsetEnd;
   stw_types::float64 mf64_InteractionWidth;
   bool mq_DisabledLook;
   bool mq_Animated;
   bool mq_InverseAnimation;
   bool mq_ShowOriginalLine;
   bool mq_SpeedUpAnimation;
   QTimer mc_Timer;
   stw_types::uint8 mu8_AnimationStep;
   static const stw_types::uint8 mhu8_ANIMATION_STEP_COUNT;
   QColor mc_AnimationPackageColor;
   QPolygonF mc_AnimationPolygon;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
