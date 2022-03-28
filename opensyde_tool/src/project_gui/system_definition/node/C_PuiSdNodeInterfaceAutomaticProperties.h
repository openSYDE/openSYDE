//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node interface properties which are initially determined automatically
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEINTERFACEAUTOMATICPROPERTIES_H
#define C_PUISDNODEINTERFACEAUTOMATICPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeInterfaceAutomaticProperties
{
public:
   C_PuiSdNodeInterfaceAutomaticProperties();

   stw_types::uint8 u8_NodeId;
   std::vector<stw_types::uint8> c_IP;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
