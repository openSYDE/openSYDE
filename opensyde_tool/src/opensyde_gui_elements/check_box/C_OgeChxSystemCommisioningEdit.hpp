//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Checkbox style for system commisioning screen, edit mode
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECHXSYSTEMCOMMISIONINGEDIT_H
#define C_OGECHXSYSTEMCOMMISIONINGEDIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeChxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeChxSystemCommisioningEdit :
   public C_OgeChxToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeChxSystemCommisioningEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
