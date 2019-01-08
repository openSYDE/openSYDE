//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Button in material design without borders and with icon

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgePubIconText.h"

#include <QPainter>

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
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

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgePubIconText::C_OgePubIconText(QWidget * const opc_Parent) :
   C_OgePubStandard(opc_Parent),
   msn_IconSize(36)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets icon size

   \param[in]     osn_IconSize         Icon size

   \created     09.02.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgePubIconText::SetIconSize(const sintn osn_IconSize)
{
   this->msn_IconSize = osn_IconSize;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided paint event

   Draws the element

   \param[in,out] opc_Event  Pointer to paint event

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgePubIconText::paintEvent(QPaintEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   QPainter c_Painter(this);
   QPixmap c_Pixmap;
   QRect c_TextRect;

   //define text rect (without icon)
   c_TextRect = this->rect();
   c_TextRect.setLeft(c_TextRect.left() + this->msn_IconSize + 20);

   // get the correct icon
   if (this->isEnabled() == false)
   {
      c_Pixmap = this->icon().pixmap(this->msn_IconSize, this->msn_IconSize, QIcon::Disabled);
   }
   else
   {
      c_Pixmap = this->icon().pixmap(this->msn_IconSize, this->msn_IconSize, QIcon::Normal);
   }

   c_Painter.save();

   this->m_SetPenColorForFont(&c_Painter);

   c_Painter.drawText(c_TextRect, static_cast<sintn>(Qt::AlignVCenter | Qt::AlignLeft), this->text());

   // draw the icon
   c_Painter.drawPixmap(0, 0, c_Pixmap);

   c_Painter.restore();
}
