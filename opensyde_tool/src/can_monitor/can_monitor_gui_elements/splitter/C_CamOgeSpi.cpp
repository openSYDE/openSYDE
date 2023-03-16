//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Splitter with two-lines-handle (implementation)

   Splitter with two-lines-handle.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "constants.hpp"
#include "C_CamOgeSpi.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor of splitter.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeSpi::C_CamOgeSpi(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent)
{
   this->setHandleWidth(7);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create splitter handle for custom handle

   \return
   splitter handle
*/
//----------------------------------------------------------------------------------------------------------------------
QSplitterHandle * C_CamOgeSpi::createHandle(void)
{
   C_CamOgeSpiHandle * const pc_Retval = new C_CamOgeSpiHandle(this->orientation(), this);

   connect(pc_Retval, &C_CamOgeSpiHandle::SigDoubleClick, this, &C_CamOgeSpi::SigDoubleClick);

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor of handle.

  \param[in]      oe_Orientation    orientation of widgets (horizontal/vertical)
                                       || is a horizontal handle (because widgets are then horizontal arranged)
                                       =  is a vertical handle (because widgets are then vertical arranged)
   \param[in,out] opc_Parent        Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamOgeSpiHandle::C_CamOgeSpiHandle(const Qt::Orientation oe_Orientation, QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden double click event slot

   Here: forward double click

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeSpiHandle::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QSplitterHandle::mouseDoubleClickEvent(opc_Event);
   Q_EMIT (this->SigDoubleClick());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamOgeSpiHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   const int32_t s32_HANDLE_HEIGHT = 30;
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
      const QPoint c_PointTopCenter = this->rect().center() + QPoint(0, s32_HANDLE_HEIGHT / 2);
      const QPoint c_PointBottomCenter = this->rect().center() - QPoint(0, s32_HANDLE_HEIGHT / 2);

      c_Painter.drawLine(c_PointTopCenter + QPoint(1, 0), c_PointBottomCenter + QPoint(1, 0));
      c_Painter.drawLine(c_PointTopCenter - QPoint(1, 0), c_PointBottomCenter - QPoint(1, 0));
   }
   else
   {
      // vertical arranged widgets, i.e. splitter looks like =
      const QPoint c_PointRightCenter = this->rect().center() + QPoint(s32_HANDLE_HEIGHT / 2, 0);
      const QPoint c_PointLeftCenter = this->rect().center() - QPoint(s32_HANDLE_HEIGHT / 2, 0);

      c_Painter.drawLine(c_PointRightCenter + QPoint(0, 1), c_PointLeftCenter + QPoint(0, 1));
      c_Painter.drawLine(c_PointRightCenter - QPoint(0, 1), c_PointLeftCenter - QPoint(0, 1));
   }
}
