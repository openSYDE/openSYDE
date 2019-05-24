//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node connection data for complete reconstruction (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDCOMPLETEBUSCONNECTIONDATA_H
#define C_PUISDCOMPLETEBUSCONNECTIONDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"
#include "C_PuiSdNodeConnectionId.h"
#include "C_PuiSdNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdCompleteBusConnectionData
{
public:
   C_PuiSdCompleteBusConnectionData();

   C_PuiSdNodeConnection c_UIData;
   C_PuiSdNodeConnectionId c_ConnectionId;
   stw_types::uint64 u64_NodeID;
   stw_types::uint64 u64_BusID;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
