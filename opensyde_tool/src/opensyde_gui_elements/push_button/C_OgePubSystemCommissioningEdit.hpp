//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Push button style for system commisioning edit bar
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBSYSTEMCOMMISSIONINGEDIT_H
#define C_OGEPUBSYSTEMCOMMISSIONINGEDIT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubSvgIconWithTextBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubSystemCommissioningEdit :
   public C_OgePubSvgIconWithTextBase
{
   Q_OBJECT

public:
   explicit C_OgePubSystemCommissioningEdit(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
