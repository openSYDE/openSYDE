//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node connection data for complete reconstruction (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISDCOMPLETEBUSCONNECTIONDATA_H
#define C_PUISDCOMPLETEBUSCONNECTIONDATA_H

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"
#include "C_PuiSdNodeConnectionId.h"
#include "C_PuiSdNode.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiSdCompleteBusConnectionData
{
public:
   C_PuiSdCompleteBusConnectionData();

   C_PuiSdNodeConnection c_UIData;
   C_PuiSdNodeConnectionId c_ConnectionId;
   stw_types::uint64 u64_NodeID;
   stw_types::uint64 u64_BusID;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
