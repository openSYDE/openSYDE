//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard tabs (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDTAB_H
#define C_OGELABDASHBOARDTAB_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabDashboardTab :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabDashboardTab(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
