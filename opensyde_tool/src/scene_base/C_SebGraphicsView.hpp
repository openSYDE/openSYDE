//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common used graphics view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBGRAPHICSVIEW_H
#define C_SEBGRAPHICSVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
#include <QColor>
#include <QRubberBand>

#include "stwtypes.hpp"

#include "C_NagToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebGraphicsView :
   public QGraphicsView
{
   Q_OBJECT

public:
   C_SebGraphicsView(QWidget * const opc_Parent = NULL);
   ~C_SebGraphicsView() override;

   // zoom functions
   void SetZoomValue(const int32_t os32_Percent, const bool & orq_Silent = false);
   int32_t GetZoomValue(void) const;
   void UpdateTransform(void) const;

   // position configuration
   void SetViewPos(const QPoint & orc_Pos);
   QPoint GetViewPos(void) const;

   // drawing background
   void SetDrawingBackground(const bool oq_Active);
   void SetSubtleSurroundGradient(const bool & orq_SubtleSurroundGradientActive);

   // tool tip
   void ShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading, const QString & orc_Content,
                    const C_NagToolTip::E_Type oe_Type);
   void HideToolTip(void);

   //Dark mode
   void SetDarkMode(const bool oq_DarkMode);

   //Scene
   void SetSceneAndConnect(QGraphicsScene * const opc_Scene);
   void ScrollTo(const QPointF & orc_ScenePosition, const QSizeF & orc_Size);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigShowToolTip(const QPointF & orc_ScenePos);

protected:
   void drawBackground(QPainter * const opc_Painter, const QRectF & orc_Rect) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void wheelEvent(QWheelEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

private:
   //Avoid call
   C_SebGraphicsView(const C_SebGraphicsView &);
   C_SebGraphicsView & operator =(const C_SebGraphicsView &) &;

   // zoom functionality
   void m_ZoomNotificationTimerEvent(void);
   void m_ScaleBy(const float64_t of64_ScaleFactor, const bool & orq_Silent = false);
   void m_ZoomIn(void);
   void m_ZoomOut(void);
   void m_UpdateZoomValue(const bool & orq_Silent = false);

   // scene position handling
   void m_DragMove(const QMouseEvent * const opc_Event);
   void m_HorScrollbarChanged(const int32_t os32_Min, const int32_t os32_Max);
   void m_VerScrollbarChanged(const int32_t os32_Min, const int32_t os32_Max);

   // proxy widget handling
   void m_SetProxyWidgetInteractionActive(const bool oq_Active);
   bool mq_ProxyWidgetInteractionActive;

   // scene position handling
   QPoint mc_ViewPortPos;
   bool mq_ViewPortPosHorSet;
   bool mq_ViewPortPosVerSet;
   bool mq_ScrollingActive;
   bool mq_DarkMode;
   QMouseEvent mc_LastMouseEvent;
   QPointF mc_DragMoveDistance;

   // zoom functionality
   int32_t ms32_ZoomValue;
   QPushButton * mpc_ZoomButton;
   QTimer mc_TimerZoomNotification;

   // drawing background
   bool mq_SubtleSurroundGradient;
   bool mq_DrawBackground;
   static const QColor mhc_GRADIENT_COLOR_LIGHT;
   static const QColor mhc_GRADIENT_COLOR_DARK;

   // tool tip
   C_NagToolTip mc_ToolTip;

   // cursor handling
   bool mq_OverrideCursorActive;
   Qt::CursorShape me_CurrentOverrideCursor;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
