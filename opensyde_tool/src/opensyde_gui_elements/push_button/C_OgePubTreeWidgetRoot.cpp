//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Button in material design without borders and with icon as a root item for a tree widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>

#include "C_OgePubTreeWidgetRoot.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     27.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgePubTreeWidgetRoot::C_OgePubTreeWidgetRoot(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   msn_IconSize(16)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided paint event

   Draws the element

   \param[in,out] opc_event  Pointer to paint event

   \created     27.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgePubTreeWidgetRoot::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);
   QPixmap c_Pixmap = this->icon().pixmap(this->msn_IconSize, this->msn_IconSize);

   Q_UNUSED(opc_Event)

   c_Painter.save();

   //this->m_SetPenColorForFont(&c_Painter);

   c_Painter.drawText(QPoint(this->msn_IconSize + 5, 19), this->text());

   // draw the icon
   c_Painter.drawPixmap(0, 7, c_Pixmap);

   c_Painter.restore();
}
