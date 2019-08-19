//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Horizontal splitter for navigation (implementation)

   Horizontal splitter for navigation

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include "stwtypes.h"
#include "constants.h"
#include "C_OgeSpiHorizontalNavigation.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpiHorizontalNavigation::C_OgeSpiHorizontalNavigation(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create splitter handle for custom handle

   \return
   splitter handle
*/
//----------------------------------------------------------------------------------------------------------------------
QSplitterHandle * C_OgeSpiHorizontalNavigation::createHandle(void)
{
   return new C_OgeSpiHorizontalNavigationHandle(Qt::Vertical, this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeSpiHorizontalNavigationHandle::C_OgeSpiHorizontalNavigationHandle(const Qt::Orientation oe_Orientation,
                                                                       QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overrided paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeSpiHorizontalNavigationHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (orientation() == Qt::Vertical)
   {
      const stw_types::sintn sn_RightBoxWidth = 89;
      const stw_types::sintn sn_Margins = 11;
      const stw_types::sintn sn_HandleWidth = 20;
      const QPoint c_PLeftCenter = this->rect().center() + QPoint(sn_HandleWidth / 2, 0);
      const QPoint c_PRightCenter = this->rect().center() - QPoint(sn_HandleWidth / 2, 0);
      const QPoint c_RightEnd = this->rect().center() + QPoint(this->rect().width() / 2, 0);
      //For some reason this has to be adapted by one to align with the right side
      const QRect c_RightRect(this->rect().topRight() - QPoint(sn_RightBoxWidth - 1, 0), QSize(sn_RightBoxWidth, 2));
      QPainter c_Painter(this);
      //Lines
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_2706);
      c_Painter.setBrush(Qt::NoBrush);
      //Center
      c_Painter.drawLine(this->rect().center() - QPoint((this->rect().width() / 2) - sn_Margins, 0),
                         c_RightEnd);
      //Others
      c_Painter.drawLine(c_PLeftCenter + QPoint(0, 2), c_PRightCenter + QPoint(0, 2));
      c_Painter.drawLine(c_PLeftCenter - QPoint(0, 2), c_PRightCenter - QPoint(0, 2));
      //Right rect
      c_Painter.setPen(Qt::NoPen);
      c_Painter.setBrush(mc_STYLE_GUIDE_COLOR_2706);
      c_Painter.drawRect(c_RightRect);
   }
   else
   {
      //not relevant
   }
}
