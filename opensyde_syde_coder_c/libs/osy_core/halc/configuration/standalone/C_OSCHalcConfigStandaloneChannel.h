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
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigStandaloneChannel
{
public:
   C_OSCHalcConfigStandaloneChannel(void);

   std::vector<stw_scl::C_SCLString> c_ParameterIds;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
