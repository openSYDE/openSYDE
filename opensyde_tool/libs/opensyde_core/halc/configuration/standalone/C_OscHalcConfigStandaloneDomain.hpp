//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Standalone HALC domain config
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGSTANDALONEDOMAIN_HPP
#define C_OSCHALCCONFIGSTANDALONEDOMAIN_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcConfigDomain.hpp"
#include "C_OscHalcConfigStandaloneChannel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigStandaloneDomain :
   public C_OscHalcConfigDomain
{
public:
   C_OscHalcConfigStandaloneDomain(void);
   C_OscHalcConfigStandaloneDomain(const C_OscHalcConfigDomain & orc_ConfigDomain,
                                   const std::vector<C_OscHalcConfigStandaloneChannel> & orc_StandaloneChannels);

   std::vector<C_OscHalcConfigStandaloneChannel> c_StandaloneChannels;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
