//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base implementation of QChartView for showing C_OgeChaChartBase (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OgeChaViewBase.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace QtCharts;
using namespace stw_types;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Chart      Pointer to chart
   \param[in,out] opc_Parent     Optional pointer to parent

   \created     03.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeChaViewBase::C_OgeChaViewBase(QChart * const opc_Chart, QWidget * const opc_Parent) :
   QChartView(opc_Chart, opc_Parent),
   mq_IsTouching(false),
   mq_DragMoveActive(false),
   mc_DragMovePos(0.0, 0.0)
{
   this->setRubberBand(QChartView::RectangleRubberBand);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten viewport event slot

   Here: Remember user touch event

   \param[in,out] opc_event Event identification and information

   \created     20.05.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OgeChaViewBase::viewportEvent(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::TouchBegin)
   {
      mq_IsTouching = true;
      this->chart()->setAnimationOptions(QChart::NoAnimation);
   }
   return QChartView::viewportEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse press event slot

   Here: ignore all events while user touch event

   \param[in,out] opc_event Event identification and information

   \created     20.05.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeChaViewBase::mousePressEvent(QMouseEvent * const opc_Event)
{
   if (mq_IsTouching == false)
   {
      if (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton))
      {
         this->mq_DragMoveActive = true;
         this->setCursor(Qt::OpenHandCursor);
         mc_DragMovePos = opc_Event->pos();
         opc_Event->accept();
      }
      else
      {
         QChartView::mousePressEvent(opc_Event);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse move event slot

   Here: ignore all events while user touch event

   \param[in,out] opc_event Event identification and information

   \created     20.05.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeChaViewBase::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   //update mouse
   if (mq_IsTouching == false)
   {
      if (opc_Event->buttons() == static_cast<sintn>(Qt::RightButton))
      {
         if (this->mq_DragMoveActive)
         {
            QPointF c_Tmp = mc_DragMovePos - opc_Event->pos();
            mc_DragMovePos = opc_Event->pos();
            this->chart()->scroll(c_Tmp.x(), -c_Tmp.y());
         }
         opc_Event->accept();
      }
      else
      {
         QChartView::mouseMoveEvent(opc_Event);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten mouse release event slot

   Here: ignore all events while user touch event

   \param[in,out] opc_event Event identification and information

   \created     20.05.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeChaViewBase::mouseReleaseEvent(QMouseEvent * const opc_Event)
{
   mq_IsTouching = false;
   this->mq_DragMoveActive = false;
   this->setCursor(Qt::ArrowCursor);
   this->chart()->setAnimationOptions(QChart::NoAnimation);

   if (opc_Event->button() == Qt::RightButton)
   {
      this->mq_DragMoveActive = false;
      opc_Event->accept();
   }
   else
   {
      QChartView::mouseReleaseEvent(opc_Event);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: allow zoom / scroll by keys

   \param[in,out] opc_event Event identification and information

   \created     20.05.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeChaViewBase::keyPressEvent(QKeyEvent * const opc_Event)
{
   switch (opc_Event->key())
   {
   case Qt::Key_Plus:
      this->chart()->zoom(1.1);
      break;
   case Qt::Key_Minus:
      this->chart()->zoom(0.9);
      break;
   case Qt::Key_Left:
      this->chart()->scroll(-10.0, 0.0);
      break;
   case Qt::Key_Right:
      this->chart()->scroll(10.0, 0.0);
      break;
   case Qt::Key_Up:
      this->chart()->scroll(0.0, 10.0);
      break;
   case Qt::Key_Down:
      this->chart()->scroll(0.0, -10.0);
      break;
   default:
      QChartView::keyPressEvent(opc_Event);
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten wheel event slot

   Here: allow zoom

   \param[in,out] opc_event Event identification and information

   \created     20.05.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeChaViewBase::wheelEvent(QWheelEvent * const opc_Event)
{
   if (C_Uti::h_CheckKeyModifier(opc_Event->modifiers(), Qt::ControlModifier) == false)
   {
      if (opc_Event->delta() > 0)
      {
         this->chart()->zoom(1.1);
      }
      else
      {
         this->chart()->zoom(0.9);
      }
      opc_Event->accept();
   }
   else
   {
      QChartView::wheelEvent(opc_Event);
   }
}
