//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Default label for dashboard, scaling (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDDEFAULT_H
#define C_OGELABDASHBOARDDEFAULT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabAdaptiveSize.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabDashboardDefault :
   public C_OgeLabAdaptiveSize
{
   Q_OBJECT

public:
   C_OgeLabDashboardDefault(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
