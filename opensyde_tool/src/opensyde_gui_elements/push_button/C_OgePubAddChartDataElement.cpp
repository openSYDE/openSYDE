//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button with specific style for use on dashboard chart signal selector widget

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPainter>

#include "C_OgePubAddChartDataElement.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
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
C_OgePubAddChartDataElement::C_OgePubAddChartDataElement(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent),
   mq_DarkMode(false)
{
   this->setIconSize(QSize(13, 13));
   mc_IconEnabledBright.addPixmap(static_cast<QPixmap>("://images/IconAddEnabledDarkMode.svg"));
   mc_IconDisabledBright.addPixmap(static_cast<QPixmap>("://images/IconAddDisabled.svg"));
   mc_IconEnabledDark.addPixmap(static_cast<QPixmap>("://images/IconAddDisabled.svg"));
   mc_IconDisabledDark.addPixmap(static_cast<QPixmap>("://images/IconAddDisabledDarkMode.svg"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set dark mode style for icon

   \param[in]  oq_DarkMode    true: dark mode, false: bright mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubAddChartDataElement::SetDarkMode(const bool oq_DarkMode)
{
   this->mq_DarkMode = oq_DarkMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw icon

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubAddChartDataElement::paintEvent(QPaintEvent * const opc_Event)
{
   QPushButton::paintEvent(opc_Event);

   QPainter c_Painter(this);
   QPixmap c_Pixmap;

   // select correct pixmap
   if (this->isEnabled() == true)
   {
      if (this->mq_DarkMode == true)
      {
         c_Pixmap = this->mc_IconEnabledDark.pixmap(this->iconSize());
      }
      else
      {
         c_Pixmap = this->mc_IconEnabledBright.pixmap(this->iconSize());
      }
   }
   else
   {
      if (this->mq_DarkMode == true)
      {
         c_Pixmap = this->mc_IconDisabledDark.pixmap(this->iconSize());
      }
      else
      {
         c_Pixmap = this->mc_IconDisabledBright.pixmap(this->iconSize());
      }
   }

   // draw the icon
   c_Painter.drawPixmap(10, 8, c_Pixmap);
}
