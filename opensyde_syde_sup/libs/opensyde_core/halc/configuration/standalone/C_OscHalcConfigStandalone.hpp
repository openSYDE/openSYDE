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
#include "C_OscHalcConfigStandaloneDomain.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigStandalone
{
public:
   C_OscHalcConfigStandalone(void);

   stw::scl::C_SclString c_DeviceType;
   uint32_t u32_DefinitionContentVersion;
   std::vector<C_OscHalcConfigStandaloneDomain> c_Domains;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
