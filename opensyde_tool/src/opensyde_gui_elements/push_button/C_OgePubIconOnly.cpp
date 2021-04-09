//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design displaying only an icon

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>
#include <QEvent>
#include "C_OgePubIconOnly.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubIconOnly::C_OgePubIconOnly(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mq_Hovered(false)
{
   this->setAttribute(Qt::WA_Hover, true);
   this->mc_IconNormal = QPixmap(this->size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle hover

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OgePubIconOnly::event(QEvent * const opc_Event)
{
   const bool q_Retval = C_OgePubToolTipBase::event(opc_Event);

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw icon for complete size

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubIconOnly::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);
   QPixmap c_Pixmap;
   const QSize c_Size = this->size();

   Q_UNUSED(opc_Event)

   // TODO icon states: deactivated because we need more time to adapt all icon-only-buttons
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the icon for different modes

   \param[in]  orc_IconPathNormal      String for enabled icon path
   \param[in]  orc_IconPathHovered     String for hovered icon path
   \param[in]  orc_IconPathClicked     String for clicked icon path
   \param[in]  orc_IconPathDisabled    String for disabled icon path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubIconOnly::SetCustomIcons(const QString & orc_IconPathNormal, const QString & orc_IconPathHovered,
                                      const QString & orc_IconPathClicked, const QString & orc_IconPathDisabled)
{
   const QSize c_Size = this->size();

   mc_IconNormal = QIcon(orc_IconPathNormal).pixmap(c_Size);
   this->setIcon(mc_IconNormal);
   // TODO icon states: because we need more time to adapt all icon-only-buttons
   Q_UNUSED(orc_IconPathHovered)
   Q_UNUSED(orc_IconPathClicked)
   //   mc_IconHovered = QIcon(oc_IconPathHovered).pixmap(c_Size);
   //   mc_IconClicked = QIcon(oc_IconPathClicked).pixmap(c_Size);
   mc_IconDisabled = QIcon(orc_IconPathDisabled).pixmap(c_Size);
}
