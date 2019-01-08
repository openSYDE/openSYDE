//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Horizontal splitter for navigation (implementation)

   Horizontal splitter for navigation

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.08.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include "stwtypes.h"
#include "constants.h"
#include "C_OgeSpiHorizontalNavigation.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

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

   \created     07.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpiHorizontalNavigation::C_OgeSpiHorizontalNavigation(QWidget * const opc_Parent) :
   C_OgeSpiBase(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Signal show

   \param[in,out] opc_Event Event identification and information

   \created     08.11.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpiHorizontalNavigation::showEvent(QShowEvent * const opc_Event)
{
   C_OgeSpiBase::showEvent(opc_Event);
   Q_EMIT this->SigShow();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create splitter handle

   \created     07.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSplitterHandle * C_OgeSpiHorizontalNavigation::createHandle(void)
{
   return new C_OgeSpiHorizontalNavigationHandle(Qt::Vertical, this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     07.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeSpiHorizontalNavigationHandle::C_OgeSpiHorizontalNavigationHandle(const Qt::Orientation oe_Orientation,
                                                                       QSplitter * const opc_Parent) :
   QSplitterHandle(oe_Orientation, opc_Parent)
{
}
//-----------------------------------------------------------------------------
/*!
   \brief   Overrided paint event

   Draws the background element

   \param[in,out] opc_Event  Pointer to paint event

   \created     07.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeSpiHorizontalNavigationHandle::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   if (orientation() == Qt::Vertical)
   {
      const stw_types::sintn sn_Margins = 11;
      const stw_types::sintn sn_HandleWidth = 20;
      const QPoint c_PLeftCenter = this->rect().center() + QPoint(sn_HandleWidth / 2, 0);
      const QPoint c_PRightCenter = this->rect().center() - QPoint(sn_HandleWidth / 2, 0);
      QPainter c_Painter(this);
      //Lines
      c_Painter.setPen(mc_STYLE_GUIDE_COLOR_2706);
      c_Painter.setBrush(Qt::transparent);
      //Center
      c_Painter.drawLine(this->rect().center() - QPoint((this->rect().width() / 2) - sn_Margins, 0),
                         this->rect().center() + QPoint((this->rect().width() / 2) - sn_Margins, 0));
      //Others
      c_Painter.drawLine(c_PLeftCenter + QPoint(0, 2), c_PRightCenter + QPoint(0, 2));
      c_Painter.drawLine(c_PLeftCenter - QPoint(0, 2), c_PRightCenter - QPoint(0, 2));
   }
   else
   {
      //not relevant
   }
}
