//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Util class for traffic encryption status functions
   \copyright   Copyright 2026 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDATRAFFICENCRYPTIONSTATUSHELPER_HPP
#define C_SYVDATRAFFICENCRYPTIONSTATUSHELPER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>

#include "stwtypes.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaTrafficEncryptionStatusHelper
{
public:
   static QMap<uint32_t, bool> h_GetMappingNodeToTrafficEncryptionStatus(const uint32_t ou32_ViewIndex,
                                                                         const bool oq_ConnectionActiveStatus);
   static bool h_GetViewNodeEncrypted(
      const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId,
      const bool oq_ConnectionActiveStatus, const uint32_t ou32_ViewIndex, const QMap<uint32_t,
                                                                                      bool> & orc_MappingNodeToTrafficEncryptionStatus);

private:
   C_SyvDaTrafficEncryptionStatusHelper();

   static void mh_GetTrafficEncryptionStatusForNodeUsingBus(QMap<uint32_t,
                                                                 bool> & orc_MappingNodeToTrafficEncryptionStatus,
                                                            const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_BusIndex);
   static void mh_GetTrafficEncryptionStatusForNodeUsingBusAndNodeId(QMap<uint32_t,
                                                                          bool> & orc_MappingNodeToTrafficEncryptionStatus, const uint32_t ou32_NodeIndex, const uint32_t ou32_BusIndex, const uint8_t ou8_NodeId);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
