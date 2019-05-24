//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for about dialog (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEPUBABOUT_H
#define C_CAMOGEPUBABOUT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgePubAbout :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubAbout(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
