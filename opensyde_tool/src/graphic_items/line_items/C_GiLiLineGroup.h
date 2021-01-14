//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete line with all functionality (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GILILINEGROUP_H
#define C_GILILINEGROUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItemGroup>
#include <QTimer>

#include "stwtypes.h"

#include "C_GiLiLine.h"
#include "C_GiBiBase.h"
#include "C_PuiBsLineBase.h"
#include "C_GiBiCustomMouseItem.h"
#include "C_GiLiInteractionPoint.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiLiLineGroup :
   public C_GiBiConnectableItem,
   public C_GiBiCustomMouseItem,
   public C_GiBiBase,
   public QGraphicsItemGroup
{
   Q_OBJECT

public:
   C_GiLiLineGroup(const std::vector<QPointF> * const opc_Points = NULL, const bool & orq_MiddleLine = false,
                   QGraphicsItem * const opc_Parent = NULL);
   virtual ~C_GiLiLineGroup(void);

   stw_types::sint32 BendLine(const QPointF & orc_ScenePos, const stw_types::sint32 * const ops32_Index = NULL);
   stw_types::sint32 RemoveBend(const QPointF & orc_ScenePos, const stw_types::sint32 * const ops32_Index = NULL);
   virtual void OnPointChange(const stw_types::sint32 & ors32_PointIndex);
   virtual QRectF boundingRect() const override;
   virtual void RestoreDefaultCursor(void) override;
   virtual void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   virtual void SetDefaultCursor(const QCursor & orc_Value) override;
   virtual void CopyStyle(const QGraphicsItem * const opc_GuidelineItem);

   //GI base
   virtual void SetZValueCustom(const stw_types::float64 of64_ZValue) override;

   // Wrapper calls for C_GiLiLine
   virtual void SetWidth(const stw_types::sint32 & ors32_Width);
   void SetColor(const QColor & orc_Color);
   void SetMiddleLineColor(const QColor & orc_Color);
   void SetMiddleLine(const bool & orq_MiddleLine);
   stw_types::sint32 GetWidth(void) const;
   QColor GetColor(void) const;
   QColor GetMiddleLineColor(void) const;
   QVector<C_GiLiLineConnection *> GetLines(void) const;
   bool GetMiddleLine(void) const;
   stw_types::sintn GetNumberPoints(void) const;
   virtual QPointF GetPos(void) const;
   virtual void SetDisabledLook(const bool oq_Disabled);
   //lint -e{1735} Suppression, because default parameters are identical
   virtual void SetAnimated(const bool oq_Active, const bool oq_Inverse = false, const bool oq_SpeedUp = false,
                            const QPolygonF oc_Polygon = QPolygonF(), const bool oq_ShowOrignalLine = true);

   virtual void SetResizing(const bool & orq_ResizeActive);
   virtual void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const;
   void FindClosestConnection(const QPointF & orc_ScenePoint, stw_types::sint32 & ors32_Index) const;
   void UpdatePoint(const stw_types::sint32 & ors32_Index, const QPointF & orc_Pos,
                    const bool oq_BlockTriggerOfChangedSignal = false);
   QPointF GetPointScenePos(const stw_types::sint32 os32_Index) const;
   void GetPointPos(const stw_types::sint32 & ors32_Index, QPointF & orc_Pos) const;
   bool CheckBendPointAt(const QPointF & orc_ScenePoint) const;

   virtual void TriggerSigChangedGraphic(void);

   virtual void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                      QWidget * const opc_Widget) override;
   virtual QPainterPath shape(void) const;
   void UpdateTransform(const QTransform & orc_Transform);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void ClickActive(const stw_types::sint32 & ors32_PointIndex, const bool & orq_Active, const QPointF & orc_Pos);
   void SigItemWasMoved(const QPointF & orc_PositionDifference);
   void SigItemWasResized(const stw_types::sint32 & ors32_InteractionPointIndex,
                          const QPointF & orc_PositionDifference);
   void SigSubItemMoves(const stw_types::sint32 & ors32_LineIndex, const QPointF & orc_PositionDifference);
   void SigSubItemWasMoved(const stw_types::sint32 & ors32_LineIndex, const QPointF & orc_PositionDifference);

protected:
   QVector<C_GiLiInteractionPoint *> mc_Points; ///< Interaction points of segmented line
   C_GiLiLine * mpc_LinePath;

   virtual void m_InitPoint(const stw_types::sint32 & ors32_Index, const QPointF & orc_Pos);
   virtual void m_AddPointAt(const stw_types::sint32 & ors32_Index, const QPointF & orc_Pos);
   virtual void m_RemovePointAt(const stw_types::sint32 & ors32_Index);

   void m_LoadBasicData(const stw_opensyde_gui_logic::C_PuiBsLineBase & orc_Data);
   void m_UpdateBasicData(stw_opensyde_gui_logic::C_PuiBsLineBase & orc_Data) const;
   void m_Init(const std::vector<QPointF> & orc_Points);

   virtual QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   virtual void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   enum E_MoveMode
   {
      eNO_ELEMENT,
      ePOINT,
      eLINE,
      eALL
   };

   E_MoveMode me_ActiveResizeMode;       ///< Currently active resize mode
   stw_types::sintn msn_ActiveItemIndex; ///< Currently active item index (Mainly for resize mode)

   static const stw_types::float64 mhf64_MaxDistToAlign;

private:
   //Avoid call
   C_GiLiLineGroup(const C_GiLiLineGroup &);
   C_GiLiLineGroup & operator =(const C_GiLiLineGroup &); //lint !e1511 //we want to hide the base func.

   void m_SetInteractionVisibility(const bool & orq_Visible);
   void m_HideInteraction(const bool & orq_Invisible);
   void m_CheckLineGrid(const QPointF & orc_MouseScenePos);
   static bool mh_Near(const stw_types::float64 of64_Exact, const stw_types::float64 of64_Eval);

   QPointF mc_LastKnownPosition;
   bool mq_BlockChangeSignal;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif // C_LINEGRAPHICSITEMGROUP_H
