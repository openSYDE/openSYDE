//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Line edit field for properties with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELEPROPERTIESTOOLTIP_H
#define C_OGELEPROPERTIESTOOLTIP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLePropertiesToolTip :
   public C_OgeLeToolTipBase
{
   Q_OBJECT

public:
   C_OgeLePropertiesToolTip(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
