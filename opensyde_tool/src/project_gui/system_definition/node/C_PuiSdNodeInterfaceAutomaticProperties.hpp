//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node interface properties which are initially determined automatically
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODEINTERFACEAUTOMATICPROPERTIES_HPP
#define C_PUISDNODEINTERFACEAUTOMATICPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeInterfaceAutomaticProperties
{
public:
   C_PuiSdNodeInterfaceAutomaticProperties();

   uint8_t u8_NodeId;
   std::vector<uint8_t> c_Ip;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
