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
#include "C_OgePubToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconOnly :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubIconOnly(QWidget * const opc_Parent = NULL);
   void SetCustomIcons(QString oc_IconPathNormal, QString oc_IconPathHovered, QString oc_IconPathClicked,
                       QString oc_IconPathDisabled);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event);
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   bool mq_Hovered;
   QPixmap mc_IconNormal;
   QPixmap mc_IconHovered;
   QPixmap mc_IconClicked;
   QPixmap mc_IconDisabled;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
