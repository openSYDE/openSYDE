//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Snapshot of dashboard items
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSNAPSHOT_H
#define C_SYVDADASHBOARDSNAPSHOT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDashboard.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardSnapshot :
   public C_PuiSvDashboard
{
public:
   C_SyvDaDashboardSnapshot(void);

   void SetDataPositionOffset(const QPointF & orc_NewPos) override;
   void SetDataZetOffset(const float64_t of64_HighestUsedZetValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
