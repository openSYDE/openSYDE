//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Snapshot of system definition items (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYDATASNAPSHOT_H
#define C_SDTOPOLOGYDATASNAPSHOT_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "stwtypes.h"
#include "C_PuiSdNode.h"
#include "C_PuiSdBus.h"
#include "C_PuiBsElements.h"
#include "C_PuiSdTextElementBus.h"
#include "C_PuiSdCompleteBusConnectionData.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdTopologyDataSnapshot :
   public C_PuiBsElements
{
public:
   C_SdTopologyDataSnapshot(void);
   virtual ~C_SdTopologyDataSnapshot(void);

   std::vector<C_PuiSdTextElementBus> c_BusTextElements; ///< UI information for bus text elements
   std::vector<C_PuiSdNode> c_UINodes;                   ///< UI information for nodes (should always
   ///< stay synchronous with core arrays)
   std::vector<C_PuiSdBus> c_UIBuses; ///< UI information for buses (should always
   ///< stay synchronous with core arrays)
   std::vector<stw_opensyde_core::C_OSCNode> c_OSCNodes; ///< all nodes that are part of this system
   ///< definition
   std::vector<stw_opensyde_core::C_OSCSystemBus> c_OSCBuses; ///< all buses that are part of this system
   ///< definition
   std::vector<C_PuiSdCompleteBusConnectionData> c_BusConnections;

   virtual void Clear(void);
   virtual stw_types::uint32 Count(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
