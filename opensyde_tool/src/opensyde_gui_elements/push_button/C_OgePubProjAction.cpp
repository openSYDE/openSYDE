//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for main page project operations (implementation)

   Push button for main page project operations:
   New Project, Open Project, Save As

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "C_OgePubProjAction.hpp"
#include "constants.hpp"

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
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgePubProjAction::C_OgePubProjAction(QWidget * const opc_Parent) :
   C_OgePubToolTipBase(opc_Parent)
{
   this->ms32_IconPaddingLeft = 26;
   this->setIconSize(mc_ICON_SIZE_24);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw icon

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubProjAction::paintEvent(QPaintEvent * const opc_Event)
{
   QPushButton::paintEvent(opc_Event);

   // draw the icon
   if (this->isEnabled() == true)
   {
      QPainter c_Painter(this);
      QPixmap c_PixmapEnabled;
      c_PixmapEnabled = this->mc_IconEnabled.pixmap(this->iconSize());
      //Calculate icon top left based on fixed center
      c_Painter.drawPixmap(this->ms32_IconPaddingLeft - (this->iconSize().width() / 2),
                           21 - (this->iconSize().height() / 2), c_PixmapEnabled);
   }
   else
   {
      QPainter c_Painter(this);
      QPixmap c_PixmapDisabled;
      c_PixmapDisabled = this->mc_IconDisabled.pixmap(this->iconSize());
      //Calculate icon top left based on fixed center
      c_Painter.drawPixmap(this->ms32_IconPaddingLeft - (this->iconSize().width() / 2),
                           21 - (this->iconSize().height() / 2), c_PixmapDisabled);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setting the Icon for Enabled and Disabled mode

   \param[in]  orc_IconPathEnabled   String for Enabled Icon Path
   \param[in]  orc_IconPathDisabled  String for Disabled Icon Path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubProjAction::SetCustomIcon(const QString & orc_IconPathEnabled, const QString & orc_IconPathDisabled)
{
   mc_IconEnabled.addPixmap(static_cast<QPixmap>(orc_IconPathEnabled));
   mc_IconDisabled.addPixmap(static_cast<QPixmap>(orc_IconPathDisabled));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setting the Icon padding left value

   \param[in]  os32_Value   Padding value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubProjAction::SetIconPaddingLeft(const int32_t os32_Value)
{
   this->ms32_IconPaddingLeft = os32_Value;
}
