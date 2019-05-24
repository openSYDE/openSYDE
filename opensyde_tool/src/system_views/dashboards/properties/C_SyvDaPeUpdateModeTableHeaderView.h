//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header view for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODETABLEHEADERVIEW_H
#define C_SYVDAPEUPDATEMODETABLEHEADERVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDataPoolListTableHeaderView.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaPeUpdateModeTableHeaderView :
   public C_SdNdeDataPoolListTableHeaderView
{
   Q_OBJECT

public:
   C_SyvDaPeUpdateModeTableHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);

   virtual QSize sizeHint(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
