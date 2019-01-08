//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Vertical splitter (implementation)

   Vertical splitter
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include <QPaintEvent>
#include <QSplitter>
#include <QSplitterHandle>
#include <QLayout>

#include "C_OgeSpiVertical.h"
#include "constants.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const float64 mf64_HANDLE_WIDTH = 5.0;         //px
const float64 mf64_HANDLE_LINES_HEIGHT = 30.0; //px

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     01.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeSpiVertical::C_OgeSpiVertical(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent)
{
   this->setHandleWidth(static_cast<sintn>(mf64_HANDLE_WIDTH));
   this->setChildrenCollapsible(false);

   this->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the drawing color

   \param[in]     orc_Color       New color

   \created     31.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeSpiVertical::SetColor(const QColor & orc_Color) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   // Index 0 is always is hidden, take number 1
   C_OgeSpiVerticalHandle * const pc_Handle = dynamic_cast<C_OgeSpiVerticalHandle *>(this->handle(1));

   if (pc_Handle != NULL)
   {
      pc_Handle->SetColor(orc_Color);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set margins

   \param[in] os32_MarginTop    Margin top
   \param[in] os32_MarginBottom Margin bottom

   \created     27.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpiVertical::SetMargins(const sint32 os32_MarginTop, const sint32 os32_MarginBottom) const
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   // Index 0 is always is hidden, take number 1
   C_OgeSpiVerticalHandle * const pc_Handle = dynamic_cast<C_OgeSpiVerticalHandle *>(this->handle(1));

   if (pc_Handle != NULL)
   {
      pc_Handle->SetMargins(os32_MarginTop, os32_MarginBottom);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     01.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeSpiVerticalHandle::C_OgeSpiVerticalHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent),
   ms32_MarginTop(9),
   ms32_MarginBottom(9)
{
   this->SetColor(mc_STYLE_GUIDE_COLOR_10);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the drawing color

   \param[in]     orc_Color       New color

   \created     31.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgeSpiVerticalHandle::SetColor(const QColor & orc_Color)
{
   this->mc_Color = orc_Color;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set margins

   \param[in] os32_MarginTop    Margin top
   \param[in] os32_MarginBottom Margin bottom

   \created     27.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpiVerticalHandle::SetMargins(const sint32 os32_MarginTop, const sint32 os32_MarginBottom)
{
   this->ms32_MarginTop = os32_MarginTop;
   this->ms32_MarginBottom = os32_MarginBottom;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event

   \created     01.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_OgeSpiVerticalHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (orientation() == Qt::Horizontal)
   {
      QPainter c_Painter(this);
      c_Painter.setPen(this->mc_Color);
      const float64 f64_RectHeight = static_cast<float64>(rect().height());
      const float64 f64_LineTopY = (f64_RectHeight / 2.0) - (mf64_HANDLE_LINES_HEIGHT / 2.0);
      const sintn sn_LineTopY = static_cast<sintn>(f64_LineTopY);
      const float64 f64_LineBottomY = (f64_RectHeight / 2.0) + (mf64_HANDLE_LINES_HEIGHT / 2.0);
      const sintn sn_LineBottomY = static_cast<sintn>(f64_LineBottomY);

      //draw main line (main line is 9px smaller because there is no modification option of splitter layout margins
      c_Painter.drawLine(rect().width() / 2,                                             //p1.x
                         rect().top() + static_cast<sintn>(this->ms32_MarginTop),        //p1.y
                         rect().width() / 2,                                             //p2.x
                         rect().bottom() - static_cast<sintn>(this->ms32_MarginBottom)); //p2.y

      //draw left handle line
      c_Painter.drawLine(rect().left(),   //p1.x
                         sn_LineTopY,     //p1.y
                         rect().left(),   //p2.x
                         sn_LineBottomY); //p2.y

      //draw right handle line
      c_Painter.drawLine(rect().right(),  //p1.x
                         sn_LineTopY,     //p1.y
                         rect().right(),  //p2.x
                         sn_LineBottomY); //p2.y
   }
   else
   {
      //not relevant
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create splitter handle

   \created     01.02.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
QSplitterHandle * C_OgeSpiVertical::createHandle(void)
{
   //technically it is a horizontal splitter
   //but it is better for understanding to call it vertical, because actually the handle is ment and it is has vertical
   // orientation
   return new C_OgeSpiVerticalHandle(Qt::Horizontal, this);
}
