//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for dashboard progress bar item value, scaled to maximum size (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABDASHBOARDPROGRESSBARMAXIMUM_H
#define C_OGELABDASHBOARDPROGRESSBARMAXIMUM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLabAdaptiveSize.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabDashboardProgressBarMaximum :
   public C_OgeLabAdaptiveSize
{
   Q_OBJECT

public:
   C_OgeLabDashboardProgressBarMaximum(QWidget * const opc_Parent = NULL);

   void AdjustFontToSize(void) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
