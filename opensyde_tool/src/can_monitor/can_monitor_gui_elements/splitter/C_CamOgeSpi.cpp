//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Splitter with two-lines-handle (implementation)

   Splitter with two-lines-handle.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.11.2018  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>

#include "constants.h"
#include "C_CamOgeSpi.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor of splitter.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     20.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgeSpi::C_CamOgeSpi(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent)
{
   this->setHandleWidth(7);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create splitter handle

   \created     20.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
QSplitterHandle * C_CamOgeSpi::createHandle(void)
{
   C_CamOgeSpiHandle * const pc_Retval = new C_CamOgeSpiHandle(this->orientation(), this);

   connect(pc_Retval, &C_CamOgeSpiHandle::SigDoubleClick, this, &C_CamOgeSpi::SigDoubleClick);

   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor of handle.

  \param[in]      oe_Orientation    orientation of widgets (horizontal/vertical)
                                       || is a horizontal handle (because widgets are then horizontal arranged)
                                       =  is a vertical handle (because widgets are then vertical arranged)
   \param[in,out] opc_Parent        Optional pointer to parent

   \created     20.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
C_CamOgeSpiHandle::C_CamOgeSpiHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overridden double click event slot

   Here: forward double click

   \param[in,out] opc_Event Event identification and information

   \created     20.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamOgeSpiHandle::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QSplitterHandle::mouseDoubleClickEvent(opc_Event);
   Q_EMIT (this->SigDoubleClick());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overridden paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event

   \created     20.11.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamOgeSpiHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   const stw_types::sintn sn_HandleHeight = 30;
   QPainter c_Painter(this);

   // Draw Background
   c_Painter.setPen(QPen(mc_STYLE_GUIDE_COLOR_8, 1.0));
   c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_9);
   // No idea why but these adjustments are necessary
   c_Painter.drawRect(this->rect().adjusted(0, 0, -1, -1));

   // Draw Lines
   c_Painter.setPen(mc_STYLE_GUIDE_COLOR_11);
   c_Painter.setBrush(Qt::transparent);

   // Different orientations need different handle
   if (this->orientation() == Qt::Horizontal)
   {
      // horizontal arranged widgets, i.e. splitter looks like ||
      const QPoint c_PTopCenter = this->rect().center() + QPoint(0, sn_HandleHeight / 2);
      const QPoint c_PBottomCenter = this->rect().center() - QPoint(0, sn_HandleHeight / 2);

      c_Painter.drawLine(c_PTopCenter + QPoint(1, 0), c_PBottomCenter + QPoint(1, 0));
      c_Painter.drawLine(c_PTopCenter - QPoint(1, 0), c_PBottomCenter - QPoint(1, 0));
   }
   else
   {
      // vertical arranged widgets, i.e. splitter looks like =
      const QPoint c_PRightCenter = this->rect().center() + QPoint(sn_HandleHeight / 2, 0);
      const QPoint c_PLeftCenter = this->rect().center() - QPoint(sn_HandleHeight / 2, 0);

      c_Painter.drawLine(c_PRightCenter + QPoint(0, 1), c_PLeftCenter + QPoint(0, 1));
      c_Painter.drawLine(c_PRightCenter - QPoint(0, 1), c_PLeftCenter - QPoint(0, 1));
   }
}
