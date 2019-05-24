//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for C_OSCComDriver (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERUTIL_H
#define C_SYVCOMDRIVERUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_OSCSystemDefinition.h"
#include "C_PuiSvHandler.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherWinSock.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDriverUtil
{
public:
   static stw_types::sint32 h_GetOSCComDriverParamFromView(const stw_types::uint32 ou32_ViewIndex,
                                                           stw_types::uint32 & oru32_ActiveBusIndex,
                                                           std::vector<stw_types::uint8> & orc_ActiveNodes,
                                                           stw_can::C_CAN ** const oppc_CanDispatcher,
                                                           stw_opensyde_core::C_OSCIpDispatcherWinSock ** const oppc_IpDispatcher, const bool oq_InitCan = true);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
