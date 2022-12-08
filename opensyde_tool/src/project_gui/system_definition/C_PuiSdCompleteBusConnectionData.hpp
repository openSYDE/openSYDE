//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node connection data for complete reconstruction (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDCOMPLETEBUSCONNECTIONDATA_HPP
#define C_PUISDCOMPLETEBUSCONNECTIONDATA_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include "C_PuiSdNodeConnectionId.hpp"
#include "C_PuiSdNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdCompleteBusConnectionData
{
public:
   C_PuiSdCompleteBusConnectionData();

   C_PuiSdNodeConnection c_UiData;
   C_PuiSdNodeConnectionId c_ConnectionId;
   uint64_t u64_NodeId;
   uint64_t u64_BusId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
