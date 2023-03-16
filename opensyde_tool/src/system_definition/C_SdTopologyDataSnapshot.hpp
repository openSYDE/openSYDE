//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Snapshot of system definition items (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYDATASNAPSHOT_HPP
#define C_SDTOPOLOGYDATASNAPSHOT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"
#include "C_PuiSdNode.hpp"
#include "C_OscNodeSquad.hpp"
#include "C_PuiSdBus.hpp"
#include "C_PuiBsElements.hpp"
#include "C_PuiSdTextElementBus.hpp"
#include "C_PuiSdCompleteBusConnectionData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyDataSnapshot :
   public C_PuiBsElements
{
public:
   C_SdTopologyDataSnapshot(void);
   ~C_SdTopologyDataSnapshot(void) override;

   std::vector<C_PuiSdTextElementBus> c_BusTextElements; ///< UI information for bus text elements
   std::vector<C_PuiSdNode> c_UiNodes;                   ///< UI information for nodes (should always
   ///< stay synchronous with core arrays)
   std::vector<C_PuiSdBus> c_UiBuses; ///< UI information for buses (should always
   ///< stay synchronous with core arrays)
   std::vector<stw::opensyde_core::C_OscNode> c_OscNodes;           ///< all nodes that are part of this system
   std::vector<stw::opensyde_core::C_OscNodeSquad> c_OscNodeGroups; ///< all node groups that are part of this system
   ///< definition
   std::vector<stw::opensyde_core::C_OscSystemBus> c_OscBuses; ///< all buses that are part of this system
   ///< definition
   std::vector<C_PuiSdCompleteBusConnectionData> c_BusConnections;

   void Clear(void) override;
   uint32_t Count(void) const override;

   void SetDataPositionOffset(const QPointF & orc_NewPos) override;
   void SetDataZetOffset(const float64_t of64_HighestUsedZetValue) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
