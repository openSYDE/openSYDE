//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standalone HALC channel config with parameter Ids
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGSTANDALONECHANNEL_H
#define C_OSCHALCCONFIGSTANDALONECHANNEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigStandaloneChannel
{
public:
   C_OscHalcConfigStandaloneChannel(void);

   std::vector<stw::scl::C_SclString> c_ParameterIds;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
