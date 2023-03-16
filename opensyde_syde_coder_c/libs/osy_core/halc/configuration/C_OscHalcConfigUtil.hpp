//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for HALC configuration related functionality
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGUTIL_H
#define C_OSCHALCCONFIGUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_SclString.hpp"

#include "C_OscHalcConfig.hpp"
#include "C_OscHalcConfigStandalone.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigUtil
{
public:
   C_OscHalcConfigUtil();

   static void h_GetConfigStandalone(const C_OscHalcConfig & orc_Config,
                                     C_OscHalcConfigStandalone & orc_ConfigStandalone);
   static int32_t h_GetConfigStandaloneChannel(const C_OscHalcConfig & orc_Config, const uint32_t ou32_DomainIndex,
                                               const uint32_t ou32_ChannelIndex, const bool oq_DomainOnly,
                                               C_OscHalcConfigStandalone & orc_ConfigStandalone);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
