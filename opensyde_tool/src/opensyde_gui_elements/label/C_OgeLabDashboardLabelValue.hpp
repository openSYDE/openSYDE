//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard label item value (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDLABELVALUE_HPP
#define C_OGELABDASHBOARDLABELVALUE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabAdaptiveSize.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabDashboardLabelValue :
   public C_OgeLabAdaptiveSize
{
   Q_OBJECT

public:
   C_OgeLabDashboardLabelValue(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
