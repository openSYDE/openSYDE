//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Common used graphics view (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBGRAPHICSVIEW_H
#define C_SEBGRAPHICSVIEW_H

/* -- Includes ------------------------------------------------------------- */

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
#include <QColor>
#include <QRubberBand>

#include "stwtypes.h"

#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebGraphicsView :
   public QGraphicsView
{
   Q_OBJECT

public:
   C_SebGraphicsView(QWidget * const opc_Parent = NULL);
   virtual ~C_SebGraphicsView();

   void SetSceneWithSignals(QGraphicsScene * const opc_Scene);

   // zoom functions
   void SetZoomValue(const stw_types::sintn osn_Percent, const bool & orq_Silent = false);
   stw_types::sintn GetZoomValue(void) const;
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void drawBackground(QPainter * const opc_Painter, const QRectF & orc_Rect) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void wheelEvent(QWheelEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SebGraphicsView(const C_SebGraphicsView &);
   C_SebGraphicsView & operator =(const C_SebGraphicsView &);

   // zoom functionality
   void m_ZoomNotificationTimerEvent(void);
   void m_ScaleBy(const stw_types::float64 of64_ScaleFactor, const bool & orq_Silent = false);
   void m_ZoomIn(void);
   void m_ZoomOut(void);
   void m_UpdateZoomValue(const bool & orq_Silent = false);

   // scene position handling
   void m_DragMove(const QMouseEvent * const opc_Event);
   void m_HorScrollbarChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max);
   void m_VerScrollbarChanged(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max);

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
   stw_types::sintn msn_ZoomValue;
   QPushButton * mpc_ZoomButton;
   QTimer mc_TimerZoomNotification;

   // drawing background
   bool mq_SubtleSurroundGradient;
   bool mq_DrawBackground;
   static const QColor mhc_GradientColorLight;
   static const QColor mhc_GradientColorDark;

   // tool tip
   C_NagToolTip mc_ToolTip;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
