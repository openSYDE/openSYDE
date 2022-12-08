//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design displaying only an icon

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPUBICONONLY_H
#define C_OGEPUBICONONLY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconOnly :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubIconOnly(QWidget * const opc_Parent = NULL);
   void SetCustomIcons(const QString & orc_IconPathNormal, const QString & orc_IconPathHovered,
                       const QString & orc_IconPathClicked, const QString & orc_IconPathDisabled);

protected:
   bool event(QEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   bool mq_Hovered;
   QPixmap mc_IconNormal;
   QPixmap mc_IconHovered;
   QPixmap mc_IconClicked;
   QPixmap mc_IconDisabled;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
