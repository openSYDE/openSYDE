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
#include "C_PuiSvDashboard.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaDashboardSnapshot :
   public C_PuiSvDashboard
{
public:
   C_SyvDaDashboardSnapshot(void);

   virtual void SetDataPositionOffset(const QPointF & orc_NewPos) override;
   virtual void SetDataZOffset(const stw_types::float64 of64_HighestUsedZValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
