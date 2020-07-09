//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standalone HALC domain config
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGSTANDALONEDOMAIN_H
#define C_OSCHALCCONFIGSTANDALONEDOMAIN_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcConfigDomain.h"
#include "C_OSCHalcConfigStandaloneChannel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigStandaloneDomain :
   public C_OSCHalcConfigDomain
{
public:
   C_OSCHalcConfigStandaloneDomain(void);
   C_OSCHalcConfigStandaloneDomain(const C_OSCHalcConfigDomain & orc_ConfigDomain,
                                   const std::vector<C_OSCHalcConfigStandaloneChannel> & orc_StandaloneChannels);

   std::vector<C_OSCHalcConfigStandaloneChannel> c_StandaloneChannels;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
