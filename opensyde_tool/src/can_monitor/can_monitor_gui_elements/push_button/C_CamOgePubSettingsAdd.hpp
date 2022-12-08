//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button for "Add Filter" and "Add Database". (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEPUBSETTINGSADD_H
#define C_CAMOGEPUBSETTINGSADD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgePubSettingsAdd :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubSettingsAdd(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
