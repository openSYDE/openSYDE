//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system definition (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDUTIL_HPP
#define C_PUISDUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QString>
#include "C_SclString.hpp"
#include "stwtypes.hpp"
#include "C_OscNodeSquad.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_PuiSdNodeConnectionId.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_PuiSdNodeInterfaceAutomaticProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdUtil
{
public:
   static bool h_CheckNameAvailable(const std::vector<const QString *> & orc_ExistingStrings,
                                    const QString & orc_Proposal, const uint32_t * const opu32_SkipIndex);
   static QString h_ConvertBusTypeToStringUppercase(const stw::opensyde_core::C_OscSystemBus::E_Type & ore_Type);
   static QString h_ConvertProtocolTypeToString(const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_Type);
   static QString h_ConvertProtocolTypeToDatapoolNameString(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Type);
   static QString h_ConvertDataPoolTypeToString(const stw::opensyde_core::C_OscNodeDataPool::E_Type & ore_Type);
   static QString h_GetInterfaceName(const stw::opensyde_core::C_OscSystemBus::E_Type oe_Type,
                                     const uint8_t ou8_InterfaceNumber);
   static stw::opensyde_core::C_OscCanProtocol::E_Type h_GetRelatedCanProtocolType(const uint32_t ou32_NodeIndex,
                                                                                   const uint32_t ou32_DatapoolIndex);
   static int32_t h_ConvertIndex(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ElementId,
                                 stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                 uint32_t & oru32_SignalIndex);
   static int32_t h_ConvertFromSignalIndex(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t ou32_SignalIndex, stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_ElementId);
   static uint32_t h_GetDefaultMessageCycleTime(void);
   static uint32_t h_GetMessageAutoTimeoutTime(const uint32_t ou32_CycleTime);
   static QString h_GetNamespace(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id);
   static QString h_GetSignalNamespace(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id);
   static QString h_GetHalcNamespace(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_Id);
   static QString h_GetSubNodeDeviceName(const uint32_t ou32_NodeIndex);
   static QString h_GetNodeBaseNameOrName(const uint32_t ou32_NodeIndex);
   static uint32_t h_GetIndexOfFirstNodeInGroup(const uint32_t ou32_NodeIndex);
   static bool h_CheckIsFirstInAnyGroupOrNotInAny(const uint32_t ou32_NodeIndex,
                                                  const std::vector<stw::opensyde_core::C_OscNodeSquad> & orc_AvailableGroups);
   static void h_GetInterfaceDataForNode(const uint32_t ou32_NodeIndex,
                                         const C_PuiSdNodeConnectionId & orc_ConnectionId,
                                         std::vector<C_PuiSdNodeInterfaceAutomaticProperties> & orc_Properties);

private:
   C_PuiSdUtil(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
