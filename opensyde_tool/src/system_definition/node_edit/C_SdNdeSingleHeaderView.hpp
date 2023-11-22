//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header view for tables with only one header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDESINGLEHEADERVIEW_HPP
#define C_SDNDESINGLEHEADERVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SdNdeDpListTableHeaderView.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeSingleHeaderView :
   public C_SdNdeDpListTableHeaderView
{
   Q_OBJECT

public:
   C_SdNdeSingleHeaderView(const Qt::Orientation & ore_Orientation, QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
