//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree model for HALC channels overview.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVERVIEWTREEVIEW_H
#define C_SDNDEHALCOVERVIEWTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.h"
#include "C_SdNdeHalcOverviewTreeModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOverviewTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
public:
   C_SdNdeHalcOverviewTreeView(QWidget * const opc_Parent = NULL);

private:
   stw_opensyde_gui_logic::C_SdNdeHalcOverviewTreeModel mc_Model;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
