//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base implementation of QChartView for showing C_OgeChaChartBase (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OgeChaViewBase.h"
#include "C_Uti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace QtCharts;
using namespace stw_types;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Chart     Pointer to chart
   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeChaViewBase::C_OgeChaViewBase(QChart * const opc_Chart, QWidget * const opc_Parent) :
   QChartView(opc_Chart, opc_Parent),
   mq_IsTouching(false),
   mq_DragMoveActive(false),
   mq_DrawingActive(true),
   mc_DragMovePos(0.0, 0.0),
   me_ZoomMode(eSETTING_ZM_XY)
{
   //this->setRubberBand(QChartView::RectangleRubberBand);
   this->setRubberBand(QChartView::NoRubberBand);

   // RubberBand deactivated
   // QChartView does not use the same rubberband as QGraphicsView. So the QGraphicsView functions and signals
   // does not work
   //this->mpc_RubberBand = this->findChild<QRubberBand *>();
   // Own implementation of RubberBand
   //this->mpc_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in]  oq_Active   Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::SetDrawingActive(const bool oq_Active)
{
   this->mq_DrawingActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current zoom mode

   \param[in]  oe_ZoomMode    Current used zoom mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::SetZoomMode(const C_OgeChaViewBase::E_SettingZoomMode oe_ZoomMode)
{
   this->me_ZoomMode = oe_ZoomMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Don't paint if not visible

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::paintEvent(QPaintEvent * const opc_Event)
{
   if (this->mq_DrawingActive)
   {
      QChartView::paintEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten viewport event slot

   Here: Remember user touch event

   \param[in,out]  opc_Event  Event identification and information

   \retval true      Event was recognized and processed
   \retval false     Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgeChaViewBase::viewportEvent(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::TouchBegin)
   {
      mq_IsTouching = true;
      this->chart()->setAnimationOptions(QChart::NoAnimation);
   }
   return QChartView::viewportEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   Here: ignore all events while user touch event

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (mq_IsTouching == false)
   {
      if ((opc_Event->buttons() == static_cast<sintn>(Qt::LeftButton)) ||
          (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton)))
      {
         this->mq_DragMoveActive = true;
         this->setCursor(Qt::OpenHandCursor);
         mc_DragMovePos = opc_Event->pos();
         opc_Event->accept();
      }
      else
      {
         // RubberBand deactivated
         //this->mpc_RubberBand->show();
         //this->mc_RubberBandStartPos = opc_Event->pos();
         //this->mpc_RubberBand->setGeometry(QRect(this->mc_RubberBandStartPos, QSize()));

         QChartView::mousePressEvent(opc_Event);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse move event slot

   Here: ignore all events while user touch event

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   //update mouse
   if (mq_IsTouching == false)
   {
      if ((opc_Event->buttons() == static_cast<sintn>(Qt::LeftButton)) ||
          (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton)))
      {
         if (this->mq_DragMoveActive)
         {
            this->m_Scroll(mc_DragMovePos, opc_Event->pos());
            mc_DragMovePos = opc_Event->pos();
         }
         opc_Event->accept();
      }
      else
      {
         // Rubberband deactivated
         //this->mpc_RubberBand->setGeometry(QRect(this->mc_RubberBandStartPos, opc_Event->pos()).normalized());

         QChartView::mouseMoveEvent(opc_Event);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse release event slot

   Here: ignore all events while user touch event

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   mq_IsTouching = false;
   this->mq_DragMoveActive = false;
   this->setCursor(Qt::ArrowCursor);
   this->chart()->setAnimationOptions(QChart::NoAnimation);

   if ((opc_Event->button() == Qt::LeftButton) ||
       (opc_Event->button() == Qt::RightButton))
   {
      this->mq_DragMoveActive = false;
      opc_Event->accept();
   }
   else
   {
      // RubberBand deactivated
      /*
      this->mpc_RubberBand->hide();

      if ((this->mpc_RubberBand->width() != 0) &&
          (this->mpc_RubberBand->height() != 0))
      {
         QRectF c_ZoomRect;
         // Rubberband is used
         // Only a manual adaption of the y axis is necessary
         const float64 f64_FactorY = static_cast<float64>(this->mpc_RubberBand->height()) /
                                     this->chart()->plotArea().height();
         const float64 f64_FactorX = static_cast<float64>(this->mpc_RubberBand->width()) /
                                     this->chart()->plotArea().width();
         const QPointF c_ValueStart = this->chart()->mapToValue(QPointF(this->mpc_RubberBand->x(),
                                                                        this->mpc_RubberBand->y()));
         const QPointF c_ValueEnd = this->chart()->mapToValue(this->chart()->plotArea().topLeft());
         const QPointF c_ValueDiff = c_ValueStart - c_ValueEnd;

         std::cout << "Zoom: Height Ru: " << static_cast<float64>(this->mpc_RubberBand->height()) <<
            " Height Pl: " << this->chart()->plotArea().height() <<
            " Width Ru: " << this->mpc_RubberBand->width() <<
            " Width Pl: " << this->chart()->plotArea().width() << std::endl;

         this->m_Scroll(QPointF(this->mpc_RubberBand->x(), 0.0),
                        QPointF(this->chart()->plotArea().x(), 0.0));

         std::cout << "Scroll Sig y: " << c_ValueDiff.y() <<
            " RubTop: " << this->mpc_RubberBand->y() <<
            " PlotTop: " << this->chart()->plotArea().topLeft().y() << std::endl;
         Q_EMIT (this->SigScrolled(0.0, c_ValueDiff.y()));
         Q_EMIT (this->SigYZoomed(f64_FactorY));

         this->chart()->scroll(0.0, ((this->mpc_RubberBand->y() - this->chart()->plotArea().y()) * (-1.0)));

         c_ZoomRect.setWidth(this->chart()->plotArea().width() * f64_FactorX);
         // Scaling of Y axis is done by SigYZoomed
         c_ZoomRect.setHeight(this->chart()->plotArea().height());
         c_ZoomRect.moveTopLeft(this->chart()->plotArea().topLeft());
         this->chart()->zoomIn(c_ZoomRect);

         //this->m_Scroll(QPointF(0.0, this->mpc_RubberBand->y()),
         //               QPointF(0.0, this->chart()->plotArea().y()));
      }
      */

      QChartView::mouseReleaseEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: allow zoom / scroll by keys

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::keyPressEvent(QKeyEvent * const opc_Event)
{
   switch (opc_Event->key())
   {
   case Qt::Key_Plus:
      this->m_Zoom(true, this->chart()->plotArea().center());
      break;
   case Qt::Key_Minus:
      this->m_Zoom(false, this->chart()->plotArea().center());
      break;
   case Qt::Key_0:
      this->chart()->zoomReset();
      Q_EMIT (this->SigZoomReseted());
      break;
   case Qt::Key_Left:
      this->m_Scroll(QPointF(0.0, 0.0), QPointF(-10.0, 0.0));
      break;
   case Qt::Key_Right:
      this->m_Scroll(QPointF(0.0, 0.0), QPointF(10.0, 0.0));
      break;
   case Qt::Key_Up:
      this->m_Scroll(QPointF(0.0, 0.0), QPointF(0.0, 10.0));
      break;
   case Qt::Key_Down:
      this->m_Scroll(QPointF(0.0, 0.0), QPointF(0.0, -10.0));
      break;
   default:
      QChartView::keyPressEvent(opc_Event);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten wheel event slot

   Here: allow zoom

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::wheelEvent(QWheelEvent * const opc_Event)
{
   if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == false)
   {
      // TODO: Zoom on cursor
      this->m_Zoom((opc_Event->delta() > 0), this->chart()->plotArea().center());
      opc_Event->accept();
   }
   else
   {
      QChartView::wheelEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Zooming in or out dependent of the zoom mode

   \param[in]  oq_ZoomIn      true  -> Zooming in
                              false -> Zooming out
   \param[in]  orc_ZoomPos    Center position for zooming (TODO: Not active yet)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::m_Zoom(const bool oq_ZoomIn, const QPointF & orc_ZoomPos) const
{
   float64 f64_Factor;
   QRectF c_RectChart = this->chart()->plotArea();

   if (oq_ZoomIn == true)
   {
      f64_Factor = 0.95;
   }
   else
   {
      f64_Factor = 1.05;
   }

   if ((this->me_ZoomMode == eSETTING_ZM_XY) ||
       (this->me_ZoomMode == eSETTING_ZM_X))
   {
      c_RectChart.setWidth(c_RectChart.width() * f64_Factor);
   }
   if ((this->me_ZoomMode == eSETTING_ZM_XY) ||
       (this->me_ZoomMode == eSETTING_ZM_Y))
   {
      c_RectChart.setHeight(c_RectChart.height() * f64_Factor);

      Q_EMIT (this->SigYZoomed(f64_Factor));
   }

   c_RectChart.moveCenter(orc_ZoomPos);

   this->chart()->zoomIn(c_RectChart);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scrolls the view of the chart

   \param[in]  orc_Start   Start point of scrolling
   \param[in]  orc_End     End point of scrolling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaViewBase::m_Scroll(const QPointF & orc_Start, const QPointF & orc_End) const
{
   const QPointF c_Diff = orc_Start - orc_End;
   const QPointF c_ValueStart = this->chart()->mapToValue(orc_Start);
   const QPointF c_ValueEnd = this->chart()->mapToValue(orc_End);
   const QPointF c_ValueDiff = c_ValueStart - c_ValueEnd;

   Q_EMIT (this->SigScrolled(c_ValueDiff.x(), c_ValueDiff.y()));

   this->chart()->scroll(c_Diff.x(), -c_Diff.y());
}
