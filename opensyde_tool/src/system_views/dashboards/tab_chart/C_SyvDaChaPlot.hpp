//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       QCustomplot adaption

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDACHAPLOT_HPP
#define C_SYVDACHAPLOT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "qcustomplot.h"

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaChaPlotTriangleItem :
   public QCPAbstractItem
{
   Q_OBJECT

public:
   C_SyvDaChaPlotTriangleItem(const bool oq_RightTriangle, QCustomPlot * const opc_ParentPlot);
   ~C_SyvDaChaPlotTriangleItem(void) override;

   void UpdateColor(const QColor & orc_Color);

   //lint -e{8001,8011,9272}  //name of function and parameters dictated by base class
   float64_t selectTest(const QPointF & orc_Pos, bool oq_OnlySelectable, QVariant * const opc_Details) const override;

   QCPItemPosition * const pc_Position;

protected:
   //lint -e{8001,8011,9272}  //name of function and parameters dictated by base class
   void draw(QCPPainter * const opc_Painter) override;

private:
   QPointF mac_LineArray[3];
   QColor mc_Color;
};

class C_SyvDaChaPlotCursorTag :
   public QObject
{
   Q_OBJECT

public:
   C_SyvDaChaPlotCursorTag(QCPAxis * const opc_ParentAxis, const QString & orc_LabelText);
   ~C_SyvDaChaPlotCursorTag(void) override;

   void UpdateColors(const QColor & orc_LabelColor, const QColor & orc_LineColor);
   void UpdatePosition(const float64_t of64_PosHorizontal);
   void SetVisible(const bool oq_Visible);
   bool ContainsItem(const QCPAbstractItem * const opc_Item) const;

private:
   QCPAxis * mpc_Axis;
   QCPItemTracer * mpc_Tracer;
   QCPItemLine * mpc_Marker;
   QCPItemText * mpc_Label;
   C_SyvDaChaPlotTriangleItem * mpc_TriangleLeft;
   C_SyvDaChaPlotTriangleItem * mpc_TriangleRight;
};

class C_SyvDaChaPlotCursorItem :
   public QCPItemLine
{
   Q_OBJECT

public:
   C_SyvDaChaPlotCursorItem(QCustomPlot * const opc_ParentPlot, const float64_t of64_InitHorizontalPos,
                            const QString & orc_LabelText);
   ~C_SyvDaChaPlotCursorItem(void) override;

   void UpdateColors(const QColor & orc_LabelColor, const QColor & orc_LineColor);
   void UpdatePosition(const float64_t of64_PosHorizontal);
   bool ContainsItem(const QCPAbstractItem * const opc_Item) const;
   void SetItemsNextToPlotVisible(const bool oq_Visible);

private:
   C_SyvDaChaPlotCursorTag * mpc_CursorTag;

   const int32_t ms32_WidthLineSelected;
   const int32_t ms32_WidthLineDefault;
   bool mq_ItemsNextToPlotVisible;
};

class C_SyvDaChaPlot :
   public QCustomPlot
{
   Q_OBJECT

public:
   C_SyvDaChaPlot(QWidget * const opc_Parent = NULL);
   ~C_SyvDaChaPlot(void) override;

   void RegisterCursorItem(C_SyvDaChaPlotCursorItem * const opc_CursorItem);
   void RemoveCursorItem(C_SyvDaChaPlotCursorItem * const opc_CursorItem);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigCursorItemClicked(const C_SyvDaChaPlotCursorItem * const opc_Line);
   void SigCursorItemMovedOnHorizontalAxis(const C_SyvDaChaPlotCursorItem * const opc_Line,
                                           const float64_t of64_PosHorizontal);
   void SigCursorItemReleased(C_SyvDaChaPlotCursorItem * const opc_Line);

protected:
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   void wheelEvent(QWheelEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   bool mq_DragMouseCursorAdapted;
   QList<C_SyvDaChaPlotCursorItem * > mc_RegisteredCursorItems;
   C_SyvDaChaPlotCursorItem * mpc_ClickedCursor;
   QPoint mc_LastItemLineEventPos;

   //Avoid call
   C_SyvDaChaPlot(const C_SyvDaChaPlot &);
   C_SyvDaChaPlot & operator =(const C_SyvDaChaPlot &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
