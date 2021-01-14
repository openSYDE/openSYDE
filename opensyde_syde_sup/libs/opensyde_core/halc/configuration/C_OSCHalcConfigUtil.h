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
#include "stwtypes.h"

#include "CSCLString.h"

#include "C_OSCHalcConfig.h"
#include "C_OSCHalcConfigStandalone.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigUtil
{
public:
   C_OSCHalcConfigUtil();

   static void h_GetConfigStandalone(const C_OSCHalcConfig & orc_Config,
                                     C_OSCHalcConfigStandalone & orc_ConfigStandalone);
   static stw_types::sint32 h_GetConfigStandaloneChannel(const C_OSCHalcConfig & orc_Config,
                                                         const stw_types::uint32 ou32_DomainIndex,
                                                         const stw_types::uint32 ou32_ChannelIndex,
                                                         const bool oq_DomainOnly,
                                                         C_OSCHalcConfigStandalone & orc_ConfigStandalone);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
