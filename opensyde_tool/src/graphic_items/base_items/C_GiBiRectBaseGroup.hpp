//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all graphic items which are rectangle based (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIBIRECTBASEGROUP_HPP
#define C_GIBIRECTBASEGROUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsItemGroup>
#include <QSizeF>

#include "stwtypes.hpp"

#include "C_PuiBsBox.hpp"
#include "C_GiUnique.hpp"
#include "C_GiBiBase.hpp"
#include "C_GiBiSizeableItem.hpp"
#include "C_GiPointInteraction.hpp"
#include "C_GiBiCustomMouseItem.hpp"
#include "C_GiBiConnectableItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiBiRectBaseGroup :
   public C_GiBiConnectableItem,
   public stw::opensyde_gui_logic::C_GiUnique,
   public C_GiBiCustomMouseItem,
   public C_GiBiBase,
   public QGraphicsItemGroup
{
   Q_OBJECT

public:
   C_GiBiRectBaseGroup(const uint64_t & oru64_Id, const float64_t of64_MinWidth, const float64_t of64_MinHeight,
                       const float64_t of64_ActionPointOffset, const bool oq_KeepAspectRatio,
                       QGraphicsItem * const opc_Parent = NULL, const QPointF & orc_PosOffset = QPointF(-1.0, -1.0));
   ~C_GiBiRectBaseGroup() override;

   void RestoreDefaultCursor(void) override;
   void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) override;
   void SetDefaultCursor(const QCursor & orc_Value) override;

   void paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
              QWidget * const opc_Widget = NULL) override;

   QRectF boundingRect() const override;
   QRectF GetVisibleBoundingRect() const;
   void FindClosestPoint(const QPointF & orc_ScenePoint, QPointF & orc_Closest) const override;

   //GI base
   void SetZetValueCustom(const float64_t of64_ZetValue) override;

   void SetResizing(const bool oq_Active);
   QSizeF GetSize(void) const;

   void LoadBasicData(const stw::opensyde_gui_logic::C_PuiBsBox & orc_Data);
   void UpdateBasicData(stw::opensyde_gui_logic::C_PuiBsBox & orc_Data) const;
   void ApplySizeChange(const QPointF & orc_NewPos, const QSizeF & orc_NewSize);
   virtual void CopyStyle(const QGraphicsItem * const opc_GuidelineItem);
   virtual void UpdateTransform(const QTransform & orc_Transform);

   bool IsResizeCursorNecessary(const QPointF & orc_ScenePos, Qt::CursorShape & ore_Cursor) const;
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSelectionChange(const bool & orq_State);
   void SigItemWasMoved(const QPointF & orc_PositionDifference);
   void SigItemWasResized(const QPointF & orc_OldPos, const QSizeF & orc_OldSize, const QPointF & orc_NewPos,
                          const QSizeF & orc_NewSize);

protected:
   QVariant itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value) override;
   void mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseMoveEvent(QGraphicsSceneMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QGraphicsSceneMouseEvent * const opc_Event) override;

   bool sceneEventFilter(QGraphicsItem * const opc_Watched, QEvent * const opc_Event) override;

   void m_SetBiggestItem(C_GiBiSizeableItem & orc_Item);
   virtual void m_ResizeUpdateItems(const float64_t of64_DiffWidth, const float64_t of64_DiffHeight) = 0;

   void m_BiggestItemChanged(void);
   void m_BlockMoveAndResize(void);

   float64_t m_GetInteractionPointSceneWidth(void) const;

private:
   //Avoid call
   C_GiBiRectBaseGroup(const C_GiBiRectBaseGroup &);
   C_GiBiRectBaseGroup & operator =(const C_GiBiRectBaseGroup &) &; //lint !e1511 //we want to hide the base func.

   void m_InitActionPoints(void);
   void m_UpdateActionPoints(void);
   QRectF m_GetBiggestSubItemBoundingRect(void) const;
   void m_SetAdaptedPos(const QPointF & orc_Pos);

   QVector<C_GiPointInteraction *> mc_ActionPoints;
   bool mq_ResizingActive;
   int32_t ms32_ActiveResizeMode;
   QRectF mc_ShowBoundingRect;
   C_GiBiSizeableItem * mpc_BiggestSubItem;
   float64_t mf64_AspectRatio;
   QPointF mc_LastKnownPosition;
   QSizeF mc_LastKnownSize;
   bool mq_BlockMoveAndResize;

   const bool mq_KeepAspectRatio;
   const float64_t mf64_ActionPointOffset;
   const float64_t mf64_MinWidth;
   const float64_t mf64_MinHeight;
   const QPointF mc_PosOffset;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
