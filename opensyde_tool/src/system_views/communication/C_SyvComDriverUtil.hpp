//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for C_OscComDriver (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERUTIL_H
#define C_SYVCOMDRIVERUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_OscSystemDefinition.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_Can.hpp"
#include "C_OscIpDispatcherWinSock.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDriverUtil
{
public:
   static int32_t h_GetOscComDriverParamFromView(const uint32_t ou32_ViewIndex, uint32_t & oru32_ActiveBusIndex,
                                                 std::vector<uint8_t> & orc_ActiveNodes,
                                                 stw::can::C_Can ** const oppc_CanDispatcher,
                                                 stw::opensyde_core::C_OscIpDispatcherWinSock ** const oppc_IpDispatcher, const bool oq_InitCan, const bool oq_IgnoreUpdateRoutingErrors, bool * const opq_DashboardRoutingErrors);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
