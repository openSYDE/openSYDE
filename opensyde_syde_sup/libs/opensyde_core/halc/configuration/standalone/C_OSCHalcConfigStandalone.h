//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Complete standalone HALC config
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGSTANDALONE_H
#define C_OSCHALCCONFIGSTANDALONE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcConfigStandaloneDomain.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigStandalone
{
public:
   C_OSCHalcConfigStandalone(void);

   stw_scl::C_SCLString c_DeviceType;
   stw_types::uint32 u32_DefinitionContentVersion;
   std::vector<C_OSCHalcConfigStandaloneDomain> c_Domains;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
