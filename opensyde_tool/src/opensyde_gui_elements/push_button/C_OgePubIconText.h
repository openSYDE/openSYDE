//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders and with icon

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPUBICONTEXT_H
#define C_OGEPUBICONTEXT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.h"
#include "C_OgePubStandard.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconText :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubIconText(QWidget * const opc_Parent = NULL);

   void SetIconSize(const stw_types::sintn osn_IconSize);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

   stw_types::sintn msn_IconSize;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
