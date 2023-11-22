//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header view for dashboard properties table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODETABLEHEADERVIEW_HPP
#define C_SYVDAPEUPDATEMODETABLEHEADERVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDpListTableHeaderView.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaPeUpdateModeTableHeaderView :
   public C_SdNdeDpListTableHeaderView
{
   Q_OBJECT

public:
   C_SyvDaPeUpdateModeTableHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);

   QSize sizeHint(void) const override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
