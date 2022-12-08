//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Common used graphics view (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QString>
#include <QPainter>
#include <QKeyEvent>
#include <QScrollBar>
#include <QMimeData>
#include <QFileInfo>
#include <cmath>

#include "C_SebGraphicsView.hpp"

#include "constants.hpp"
#include "C_SebScene.hpp"
#include "C_SebStyle.hpp"
#include "C_OgePubZoomNotification.hpp"
#include "C_Uti.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t ms32_WHITE_EDGE_WIDTH = 100;
//zoom parameters
const int32_t ms32_MIN_ZOOM_IN_PERCENT = 10;
const int32_t ms32_MAX_ZOOM_IN_PERCENT = 1000;
const int32_t ms32_ZOOM_STEP_IN_PERCENT = 5;
//Background
const QColor C_SebGraphicsView::mhc_GRADIENT_COLOR_LIGHT = mc_STYLE_GUIDE_COLOR_0;
const QColor C_SebGraphicsView::mhc_GRADIENT_COLOR_DARK = mc_STYLE_GUIDE_COLOR_52;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebGraphicsView::C_SebGraphicsView(QWidget * const opc_Parent) :
   QGraphicsView(opc_Parent),
   mq_ProxyWidgetInteractionActive(false),
   mc_ViewPortPos(QPoint(0, 0)),
   mq_ViewPortPosHorSet(false),
   mq_ViewPortPosVerSet(false),
   mq_ScrollingActive(false),
   mq_DarkMode(false),
   mc_LastMouseEvent(QEvent::None, QPointF(), QPointF(), QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier),
   mc_DragMoveDistance(0.0, 0.0),
   ms32_ZoomValue(100),
   mq_SubtleSurroundGradient(false),
   mq_DrawBackground(true)
{
   this->setAcceptDrops(true);
   this->setDragMode(QGraphicsView::NoDrag);
   this->setCacheMode(QGraphicsView::CacheBackground);
   this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

   //zoom under mouse
   this->setTransformationAnchor(AnchorUnderMouse);

   //create zoom notification button
   this->mpc_ZoomButton = new stw::opensyde_gui_elements::C_OgePubZoomNotification(this);
   this->mpc_ZoomButton->setEnabled(false);
   this->mpc_ZoomButton->hide();
   this->mpc_ZoomButton->setGeometry(0, 0, 190, 40);
   this->mpc_ZoomButton->setAttribute(Qt::WA_TransparentForMouseEvents);

   //Init dark mode
   this->SetDarkMode(false);

   //Rubber band style
   this->setStyle(new C_SebStyle(this->style()));

   //init zoom notification timer
   this->mc_TimerZoomNotification.setInterval(2000);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);

   //connect timer
   connect(&this->mc_TimerZoomNotification, &QTimer::timeout, this,
           &C_SebGraphicsView::m_ZoomNotificationTimerEvent);

   connect(this->horizontalScrollBar(), &QScrollBar::rangeChanged,
           this, &C_SebGraphicsView::m_HorScrollbarChanged);
   connect(this->verticalScrollBar(), &QScrollBar::rangeChanged,
           this, &C_SebGraphicsView::m_VerScrollbarChanged);

   this->installEventFilter(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ZoomButton and the Qt memory management
C_SebGraphicsView::~C_SebGraphicsView()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the actual zoom value as percent

   \param[in]  os32_Percent   Percent
   \param[in]  orq_Silent     Flag if user notification should be shown
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::SetZoomValue(const int32_t os32_Percent, const bool & orq_Silent)
{
   const float64_t f64_TargetScale = static_cast<float64_t>(os32_Percent) / 100.0;
   const float64_t f64_ScaleFactor = f64_TargetScale / transform().m11();

   m_ScaleBy(f64_ScaleFactor, orq_Silent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets the actual zoom value as percent

   \return  Actual zoom value
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SebGraphicsView::GetZoomValue(void) const
{
   return this->ms32_ZoomValue;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Manual update of transformation values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::UpdateTransform(void) const
{
   C_SebScene * const pc_Scene = dynamic_cast<C_SebScene *>(this->scene());

   if (pc_Scene != NULL)
   {
      //Register scaling factor
      pc_Scene->UpdateTransform(this->transform());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the actual position of the viewport on the scene

   The position will be set to the scroll bars in the scroll bar event to make sure
   the setting of the position is not to early. If it will be set to early, it
   does not work.

   \param[in]  orc_Pos  New positon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::SetViewPos(const QPoint & orc_Pos)
{
   this->mc_ViewPortPos = orc_Pos;

   this->mq_ViewPortPosHorSet = true;
   this->mq_ViewPortPosVerSet = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets the actual position of the viewport on the scene

   \return  Actual viewport position
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_SebGraphicsView::GetViewPos(void) const
{
   QPoint c_Pos;

   c_Pos.setX(this->horizontalScrollBar()->value());
   c_Pos.setY(this->verticalScrollBar()->value());

   return c_Pos;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the flag for drawing the background

   \param[in]  oq_Active   Flag for drawing the background
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::SetDrawingBackground(const bool oq_Active)
{
   this->mq_DrawBackground = oq_Active;

   stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this, "STYLE_GUIDE_COLOR_12", !oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set flag for subtle surround gradient vs strong top and left gradient

   \param[in]  orq_SubtleSurroundGradientActive    true: subtle surround gradient
                                                   false: strong top and left gradient
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::SetSubtleSurroundGradient(const bool & orq_SubtleSurroundGradientActive)
{
   mq_SubtleSurroundGradient = orq_SubtleSurroundGradientActive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show tool tip

   \param[in]  orc_ScenePos   Scne position to show tool tip at
   \param[in]  orc_Heading    Heading of tool tip
   \param[in]  orc_Content    Content of tool tip
   \param[in]  oe_Type        Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::ShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading,
                                    const QString & orc_Content, const C_NagToolTip::E_Type oe_Type)
{
   const QPointF c_AdaptedScenePos(orc_ScenePos.x(), orc_ScenePos.y());

   mc_ToolTip.SetHeading(orc_Heading);
   mc_ToolTip.SetContent(orc_Content);
   mc_ToolTip.SetType(oe_Type);
   mc_ToolTip.show();
   mc_ToolTip.DoMove(this->mapToGlobal(this->mapFromScene(c_AdaptedScenePos)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Hide tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::HideToolTip(void)
{
   mc_ToolTip.hide();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set dark mode state for scene

   \param[in]  oq_DarkMode    Dark mode active flag
                              true:  Dark mode
                              false: Light mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::SetDarkMode(const bool oq_DarkMode)
{
   this->mq_DarkMode = oq_DarkMode;
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "DarkMode", oq_DarkMode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets the scene and connects with available signals

   For connecting to the signals it has to be a C_SebScene scene.

   \param[in,out]  opc_Scene  New scene
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::SetSceneAndConnect(QGraphicsScene * const opc_Scene)
{
   C_SebScene * const pc_Scene = dynamic_cast<C_SebScene * const>(opc_Scene);

   this->setScene(opc_Scene);

   //Connects
   if (pc_Scene != NULL)
   {
      connect(pc_Scene, &C_SebScene::SigProxyWidgetInteractionActive, this,
              &C_SebGraphicsView::m_SetProxyWidgetInteractionActive);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Scroll to scene position

   \param[in]  orc_ScenePosition    Scene position
   \param[in]  orc_Size             Size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::ScrollTo(const QPointF & orc_ScenePosition, const QSizeF & orc_Size)
{
   if ((this->horizontalScrollBar()->isVisible() == true) || (this->verticalScrollBar()->isVisible() == true))
   {
      this->ensureVisible(QRectF(orc_ScenePosition, orc_Size));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided background paint event

   Draws the background with a gradient and the background image which is defined in
   the ui file.

   \param[in,out]  opc_Painter   Painter
   \param[in]      orc_Rect      Rect
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::drawBackground(QPainter * const opc_Painter, const QRectF & orc_Rect)
{
   QGraphicsView::drawBackground(opc_Painter, orc_Rect);

   if (this->mq_DrawBackground == true)
   {
      QColor c_GradientColorStart;
      QColor c_GradientColorEnd;
      QPen c_Pen;
      QRectF c_Rect;
      QLinearGradient c_Gradient;
      float64_t f64_RectWidth;

      if (mq_SubtleSurroundGradient == false)
      {
         f64_RectWidth = static_cast<float64_t>(ms32_WHITE_EDGE_WIDTH);
      }
      else
      {
         f64_RectWidth = static_cast<float64_t>(ms32_WHITE_EDGE_WIDTH) / 2.0;
      }

      if (this->mq_DarkMode == false)
      {
         c_GradientColorStart = C_SebGraphicsView::mhc_GRADIENT_COLOR_LIGHT;
         c_GradientColorEnd = C_SebGraphicsView::mhc_GRADIENT_COLOR_LIGHT;
      }
      else
      {
         c_GradientColorStart = C_SebGraphicsView::mhc_GRADIENT_COLOR_DARK;
         c_GradientColorEnd = C_SebGraphicsView::mhc_GRADIENT_COLOR_DARK;
      }

      // configure the colors
      c_Pen.setColor(QColor(0, 0, 0, 0));
      opc_Painter->setPen(c_Pen);

      c_GradientColorEnd.setAlpha(0);
      c_Gradient.setColorAt(0.0, c_GradientColorStart);
      c_Gradient.setColorAt(1.0, c_GradientColorEnd);

      // draw the first rectangle
      c_Gradient.setStart(0.0, 0.0);
      c_Gradient.setFinalStop(static_cast<float64_t>(f64_RectWidth), 0.0);
      opc_Painter->setBrush(c_Gradient);

      c_Rect.setX(orc_Rect.x());
      c_Rect.setY(orc_Rect.y());
      c_Rect.setHeight(orc_Rect.height());
      c_Rect.setWidth(f64_RectWidth);

      opc_Painter->drawRect(c_Rect);

      if (mq_SubtleSurroundGradient == true)
      {
         // draw the third rectangle
         c_Gradient.setStart(orc_Rect.topRight());
         c_Gradient.setFinalStop(orc_Rect.topRight().x() - f64_RectWidth, orc_Rect.topRight().y());
         opc_Painter->setBrush(c_Gradient);

         c_Rect.setTopRight(orc_Rect.topRight());

         opc_Painter->drawRect(c_Rect);
      }

      // draw the second rectangle
      c_Gradient.setStart(0.0, 0.0);
      c_Gradient.setFinalStop(0.0, f64_RectWidth);
      opc_Painter->setBrush(c_Gradient);

      c_Rect.setHeight(f64_RectWidth);
      c_Rect.setWidth(orc_Rect.width());

      opc_Painter->drawRect(c_Rect);

      if (mq_SubtleSurroundGradient == true)
      {
         // draw the fourth rectangle
         c_Gradient.setStart(orc_Rect.bottomLeft());
         c_Gradient.setFinalStop(orc_Rect.bottomLeft().x(), orc_Rect.bottomLeft().y() - f64_RectWidth);
         opc_Painter->setBrush(c_Gradient);

         c_Rect.setBottomLeft(orc_Rect.bottomLeft());

         opc_Painter->drawRect(c_Rect);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided resize event

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::resizeEvent(QResizeEvent * const opc_Event)
{
   //move button to center
   this->mpc_ZoomButton->move((this->width() / 2) - (mpc_ZoomButton->width() / 2),
                              (this->height() / 2) - (mpc_ZoomButton->height() / 2));

   //call original event
   QGraphicsView::resizeEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided mouse move event

   Drag and move functionality.

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   if (this->mq_ProxyWidgetInteractionActive == false)
   {
      if (this->mq_ScrollingActive == true)
      {
         this->viewport()->setCursor(Qt::ClosedHandCursor);
         this->m_DragMove(opc_Event);
      }

      this->mc_LastMouseEvent = *opc_Event;
   }

   QGraphicsView::mouseMoveEvent(opc_Event);

   // remove selection (select on release, not on move)

   //SSI: Deactivated for Release 7, buggy
   /*
   if ((this->dragMode() == QGraphicsView::RubberBandDrag) && (QApplication::mouseButtons() == Qt::LeftButton))
   {
      this->scene()->setSelectionArea(QPainterPath());
   }*/
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided mouse press event

   Drag and move functionality.

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::mousePressEvent(QMouseEvent * const opc_Event)
{
   C_SebScene * const pc_Scene = dynamic_cast<C_SebScene *>(this->scene());
   bool q_BlockPressHandling = false;

   if (pc_Scene != NULL)
   {
      q_BlockPressHandling = pc_Scene->IsMousePosRelevantForProxyWidgetInteraction(this->mapToScene(opc_Event->pos()));
   }

   if ((this->mq_ProxyWidgetInteractionActive == false) &&
       (q_BlockPressHandling == false))
   {
      bool q_RubberBandModeAvailable = true;

      if (pc_Scene != NULL)
      {
         if (pc_Scene->IsSceneRubberBandAvailable() == false)
         {
            q_RubberBandModeAvailable = false;
         }
      }

      // store the event
      this->mc_LastMouseEvent = *opc_Event;

      // using buttonS to make sure that really only one of the button is clicked
      if (opc_Event->buttons() == static_cast<int32_t>(Qt::RightButton))
      {
         // deactivate rubberband functionality
         this->setDragMode(QGraphicsView::NoDrag);
         this->mq_ScrollingActive = true;
      }
      else if (q_RubberBandModeAvailable == true)
      {
         if (opc_Event->buttons() == static_cast<int32_t>(Qt::LeftButton))
         {
            // activate rubberband functionality
            this->setDragMode(QGraphicsView::RubberBandDrag);
         }
         else
         {
            //No special handling for other mouse buttons
         }
      }
      else
      {
         // deactivate rubberband functionality
         this->setDragMode(QGraphicsView::NoDrag);
         // and context menu
         // TODO Maybe we need the context menu in this context too
         if (pc_Scene != NULL)
         {
            pc_Scene->BlockContextMenu();
         }
      }
   }

   QGraphicsView::mousePressEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided mouse release event

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   if ((this->mq_ProxyWidgetInteractionActive == false) &&
       (opc_Event->button() == Qt::RightButton))
   {
      this->viewport()->unsetCursor();
      this->mq_ScrollingActive = false;

      // reset the difference point
      this->mc_DragMoveDistance.setX(0.0);
      this->mc_DragMoveDistance.setY(0.0);
   }

   //SSI: Deactivated for Release 7, buggy
   //select area of rubber
   /*
   if (this->dragMode() == QGraphicsView::RubberBandDrag)
   {
      // select area of rubber band on mouse release (standard behavior would be select on mouse move)
      QPainterPath c_PaintPath;
      c_PaintPath.addRect(this->rubberBandRect());
      this->scene()->setSelectionArea(this->mapToScene(c_PaintPath));
   }*/

   QGraphicsView::mouseReleaseEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided key press event

   Zooming functionality.

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::keyPressEvent(QKeyEvent * const opc_Event)
{
   C_SebScene * const pc_Scene = dynamic_cast<C_SebScene *>(this->scene());
   bool q_BlockPressHandling = false;

   if (pc_Scene != NULL)
   {
      q_BlockPressHandling = pc_Scene->IsSelectionRelevantForProxyWidgetInteraction();
   }

   if ((this->mq_ProxyWidgetInteractionActive == false) &&
       (q_BlockPressHandling == false) &&
       (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true))
   {
      switch (opc_Event->key())
      {
      case Qt::Key_BracketRight: // Qt::Key_BracketRight matches the "Not-Num-Plus"-Key
      case Qt::Key_Plus:
         this->m_ZoomIn();
         break;
      case Qt::Key_Minus:
         this->m_ZoomOut();
         break;
      case Qt::Key_0:
         this->SetZoomValue(100);
         break;
      default:
         QGraphicsView::keyPressEvent(opc_Event);
         break;
      }
   }
   else
   {
      QGraphicsView::keyPressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overrided wheel event

   Zooming functionality.

   \param[in,out]  opc_Event  Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::wheelEvent(QWheelEvent * const opc_Event)
{
   if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == true)
   {
      if (opc_Event->angleDelta().y() > 0) // (opc_Event->angleDelta().y() is equal to opc_Event->delta())
      {
         m_ZoomIn();
      }
      else
      {
         m_ZoomOut();
      }
   }
   else
   {
      QGraphicsView::wheelEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten drag enter event slot

   Here: Do not accept external *.syde file here. It will be handled in NagMainWindow

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   QGraphicsView::dragEnterEvent(opc_Event);

   if (opc_Event->mimeData()->hasUrls() == true)
   {
      const QFileInfo c_File(opc_Event->mimeData()->text());

      //Manually prevent accepting openSYDE files as image
      if (c_File.completeSuffix().compare("syde") == 0)
      {
         opc_Event->ignore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten drop event slot

   Here: Set focus back to scene

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::dropEvent(QDropEvent * const opc_Event)
{
   this->setFocus();
   QGraphicsView::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten default event slot

   Here: Handle tool tip

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebGraphicsView::event(QEvent * const opc_Event)
{
   bool q_Retval = QGraphicsView::event(opc_Event);

   if (opc_Event->type() == QEvent::ToolTip)
   {
      QHelpEvent * const pc_Help = dynamic_cast<QHelpEvent *>(opc_Event);
      if (pc_Help != NULL)
      {
         Q_EMIT (this->SigShowToolTip(this->mapToScene(pc_Help->pos())));
         //Accept event because of Qt dynamic tooltip time based on the fact if there was a tooltip in this widget
         // already
         opc_Event->accept();
         q_Retval = true;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot function for zoom notification timer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_ZoomNotificationTimerEvent()
{
   //hide nofitifcation
   this->mpc_ZoomButton->hide();

   //stop timer
   this->mc_TimerZoomNotification.stop();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Scale view by scale factor

   \param[in]  of64_ScaleFactor  Scale Factor
   \param[in]  orq_Silent        Flag if user notification should be shown
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_ScaleBy(const float64_t of64_ScaleFactor, const bool & orq_Silent)
{
   QTransform c_Tranform = this->transform();

   //get absolute factor
   const float64_t f64_Factor =
      c_Tranform.scale(of64_ScaleFactor, of64_ScaleFactor).mapRect(QRectF(0.0, 0.0, 1.0, 1.0)).width();
   const float64_t f64_Value = f64_Factor * 100.0;

   //min limit reached?
   if (f64_Value < static_cast<float64_t>(ms32_MIN_ZOOM_IN_PERCENT))
   {
      if (C_OscUtils::h_IsFloat64NearlyEqual(f64_Value, static_cast<float64_t>(ms32_MIN_ZOOM_IN_PERCENT)) == false)
      {
         //set min and return
         SetZoomValue(ms32_MIN_ZOOM_IN_PERCENT, orq_Silent);
      }
   }
   //max limit reached?
   else if ((f64_Factor * 100.0) > static_cast<float64_t>(ms32_MAX_ZOOM_IN_PERCENT))
   {
      if (C_OscUtils::h_IsFloat64NearlyEqual(f64_Value, static_cast<float64_t>(ms32_MAX_ZOOM_IN_PERCENT)) == false)
      {
         //set max and return
         SetZoomValue(ms32_MAX_ZOOM_IN_PERCENT, orq_Silent);
      }
   }
   else
   {
      //zoom it
      scale(of64_ScaleFactor, of64_ScaleFactor);

      {
         // adapt the scene rectangle
         C_SebScene * const pc_Scene = dynamic_cast<C_SebScene *>(this->scene());
         if (pc_Scene != NULL)
         {
            QPointF c_Point;
            QRectF c_Rect = this->scene()->sceneRect();

            //Register new scaling factor
            pc_Scene->UpdateTransform(this->transform());

            // add this to the rectangle to avoid the view move to left border first
            c_Point = this->mapToScene(this->rect().bottomRight());
            c_Rect.setBottomRight(c_Point);

            // check if new rectangle is bigger than minimum required scene rectangle
            if (c_Rect.contains(pc_Scene->GetMinimumSceneRect()) == true)
            {
               this->scene()->setSceneRect(c_Rect);
            }
         }
      }

      //update zoom value
      m_UpdateZoomValue(orq_Silent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Zoom step in
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_ZoomIn(void)
{
   SetZoomValue(this->ms32_ZoomValue + ms32_ZOOM_STEP_IN_PERCENT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Zoom step out
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_ZoomOut(void)
{
   SetZoomValue(this->ms32_ZoomValue - ms32_ZOOM_STEP_IN_PERCENT);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  UpdateZoomValue

   \param[in]  orq_Silent  Flag if user notification should be shown
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_UpdateZoomValue(const bool & orq_Silent)
{
   ms32_ZoomValue = static_cast<int32_t>(round(transform().m11() * 100.0));

   if (orq_Silent == false)
   {
      QString c_ZoomString = "Zoom: ";
      //update zoomtext
      c_ZoomString = c_ZoomString + QString::number(ms32_ZoomValue) + " %";
      this->mpc_ZoomButton->setText(c_ZoomString);

      //show notification and start timer
      this->mpc_ZoomButton->show();
      this->mc_TimerZoomNotification.start();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Implementation of drag moving

   \param[in]  opc_Event   Event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_DragMove(const QMouseEvent * const opc_Event)
{
   QScrollBar * const pc_HorBar = this->horizontalScrollBar();
   QScrollBar * const pc_VerBar = this->verticalScrollBar();
   const QPointF c_Delta = opc_Event->pos() - this->mc_LastMouseEvent.pos();

   // calculate the complete difference of this move
   this->mc_DragMoveDistance += c_Delta;

   if ((std::fabs(this->mc_DragMoveDistance.x()) > 2.0) ||
       (std::fabs(this->mc_DragMoveDistance.y()) > 2.0))
   {
      C_SebScene * const pc_Scene = dynamic_cast<C_SebScene *>(this->scene());

      if (pc_Scene != NULL)
      {
         // block the context menu till the move is finished
         pc_Scene->BlockContextMenu();
      }
   }

   const bool q_RightToLeft = this->isRightToLeft();

   if (q_RightToLeft == true)
   {
      pc_HorBar->setValue(pc_HorBar->value() + static_cast<int32_t>(c_Delta.x()));
   }
   else
   {
      pc_HorBar->setValue(pc_HorBar->value() - static_cast<int32_t>(c_Delta.x()));
   }

   pc_VerBar->setValue(pc_VerBar->value() - static_cast<int32_t>(c_Delta.y()));
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_HorScrollbarChanged(const int32_t os32_Min, const int32_t os32_Max)
{
   // The position of the viewport with the scrollbars can be changed only if the scrollbars are already
   // visible. Check if position shall be changed and if the minimum and maximum of the scrollbars are different.
   // This is an indicator for visible scroll bars.
   if ((this->mq_ViewPortPosHorSet == true) && (os32_Min != os32_Max))
   {
      this->mq_ViewPortPosHorSet = false;

      this->horizontalScrollBar()->setValue(this->mc_ViewPortPos.x());
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_VerScrollbarChanged(const int32_t os32_Min, const int32_t os32_Max)
{
   // The position of the viewport with the scrollbars can be changed only if the scrollbars are already
   // visible. Check if position shall be changed and if the minimum and maximum of the scrollbars are different.
   // This is an indicator for visible scroll bars.
   if ((this->mq_ViewPortPosVerSet == true) && (os32_Min != os32_Max))
   {
      this->mq_ViewPortPosVerSet = false;

      this->verticalScrollBar()->setValue(this->mc_ViewPortPos.y());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set proxy widget interaction state

   \param[in]  oq_Active   Proxy widget interaction state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebGraphicsView::m_SetProxyWidgetInteractionActive(const bool oq_Active)
{
   this->mq_ProxyWidgetInteractionActive = oq_Active;
}
