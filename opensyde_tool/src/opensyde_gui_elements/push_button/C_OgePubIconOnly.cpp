//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Button in material design displaying only an icon

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QPainter>
#include <QEvent>
#include "C_OgePubIconOnly.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     27.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgePubIconOnly::C_OgePubIconOnly(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mq_Hovered(false)
{
   this->setAttribute(Qt::WA_Hover, true);
   this->mc_IconNormal = QPixmap(this->size());
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle hover

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     07.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
bool C_OgePubIconOnly::event(QEvent * const opc_Event)
{
   bool q_Retval = C_OgePubToolTipBase::event(opc_Event);

   if (opc_Event->type() == QEvent::HoverLeave)
   {
      this->mq_Hovered = false;
   }
   else if (opc_Event->type() == QEvent::Hide)
   {
      this->mq_Hovered = false;
   }
   else if (opc_Event->type() == QEvent::Show)
   {
      this->mq_Hovered = false;
   }
   else if (opc_Event->type() == QEvent::HoverEnter)
   {
      this->mq_Hovered = true;
   }
   else
   {
      //No handling required
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: Draw icon for complete size

   \param[in,out] opc_Event Event identification and information

   \created     04.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgePubIconOnly::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);
   QPixmap c_Pixmap;
   QSize c_Size = this->size();

   Q_UNUSED(opc_Event)

   // TODO GS deactivated because we need more time to adapt all icon-only-buttons
   if (this->isEnabled() == false)
   {
      c_Pixmap = this->mc_IconDisabled;
   }
   //   else if (this->isDown() == true)
   //   {
   //      c_Pixmap = this->mc_IconClicked;
   //   }
   //   else if (this->mq_Hovered == true)
   //   {
   //      c_Pixmap = this->mc_IconHovered;
   //   }
   else
   {
      c_Pixmap = this->icon().pixmap(c_Size);
   }

   // draw the icon
   c_Painter.drawPixmap(0, 0, c_Pixmap);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set the Icon for Enabled and Disabled mode

   \param[in]     oc_IconPathEnabled      String for Enabled Icon Path
                  oc_IconPathDisabled     String for Disabled Icon Path

   \created     07.08.2018  STW/G.Scupin
*/
//-----------------------------------------------------------------------------
void C_OgePubIconOnly::SetCustomIcons(QString oc_IconPathNormal, QString oc_IconPathHovered, QString oc_IconPathClicked,
                                      QString oc_IconPathDisabled)
{
   QSize c_Size = this->size();

   mc_IconNormal = QIcon(oc_IconPathNormal).pixmap(c_Size);
   this->setIcon(mc_IconNormal);
   // TODO GS deactivated because we need more time to adapt all icon-only-buttons
   Q_UNUSED(oc_IconPathHovered)
   Q_UNUSED(oc_IconPathClicked)
   //   mc_IconHovered = QIcon(oc_IconPathHovered).pixmap(c_Size);
   //   mc_IconClicked = QIcon(oc_IconPathClicked).pixmap(c_Size);
   mc_IconDisabled = QIcon(oc_IconPathDisabled).pixmap(c_Size);
}
