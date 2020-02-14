//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Update button including an icon (implementation)

   Standard dialog button including an icon.
   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgePubUpdate.h"
#include "constants.h"
#include <QPainter>

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui;

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
C_OgePubUpdate::C_OgePubUpdate(QWidget * const opc_Parent) :
   C_OgePubStandard(opc_Parent)
{
   this->setIconSize(mc_ICON_SIZE_24);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Draw icon

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubUpdate::paintEvent(QPaintEvent * const opc_Event)
{
   QPushButton::paintEvent(opc_Event);

   // draw the icon
   if (this->isEnabled() == true)
   {
      QPainter c_Painter(this);
      QPixmap c_PixmapEnabled;
      c_PixmapEnabled = this->mc_IconEnabled.pixmap(this->iconSize());
      c_Painter.drawPixmap(13, 8, c_PixmapEnabled);
   }
   else
   {
      QPainter c_Painter(this);
      QPixmap c_PixmapDisabled;
      c_PixmapDisabled = this->mc_IconDisabled.pixmap(this->iconSize());
      c_Painter.drawPixmap(13, 8, c_PixmapDisabled);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setting the Icon for Enabled and Disabled mode

   \param[in]  orc_IconPathEnabled   String for Enabled Icon Path
   \param[in]  orc_IconPathDisabled  String for Disabled Icon Path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgePubUpdate::InitCustomIcon(const QString & orc_IconPathEnabled, const QString & orc_IconPathDisabled)
{
   mc_IconEnabled.addPixmap(QPixmap(orc_IconPathEnabled));
   mc_IconDisabled.addPixmap(QPixmap(orc_IconPathDisabled));
}
