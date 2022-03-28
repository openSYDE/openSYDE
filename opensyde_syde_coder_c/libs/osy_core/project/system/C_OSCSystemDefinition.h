//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store system definition data

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMDEFINITIONH
#define C_OSCSYSTEMDEFINITIONH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_OSCNode.h"
#include "C_OSCNodeSquad.h"
#include "C_OSCSystemBus.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCDeviceManager.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSystemDefinition
{
public:
   C_OSCSystemDefinition(void);
   virtual ~C_OSCSystemDefinition(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   void AddBus(const C_OSCSystemBus & orc_Bus);
   stw_types::sint32 InsertBus(const stw_types::uint32 ou32_BusIndex, const C_OSCSystemBus & orc_Bus);
   stw_types::sint32 DeleteBus(const stw_types::uint32 ou32_BusIndex);
   stw_types::sint32 AddConnection(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_BusIndex,
                                   const stw_types::uint8 ou8_Interface);
   bool CheckInterfaceIsAvailable(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ComIndex,
                                  const stw_types::uint8 ou8_ComNodeID) const;
   bool CheckIpAddressIsValid(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_ComIndex,
                              const std::vector<stw_types::sint32> & orc_Ip) const;
   bool CheckBusIdAvailable(const stw_types::uint8 ou8_BusId,
                            const stw_types::uint32 * const opu32_BusIndexToSkip = NULL) const;
   stw_types::sint32 GetNextFreeBusId(stw_types::uint8 & oru8_BusId) const;
   stw_types::sint32 CheckErrorNode(const stw_types::uint32 ou32_NodeIndex, bool * const opq_NameConflict,
                                    bool * const opq_NameInvalid, bool * const opq_NodeIdInvalid,
                                    bool * const opq_IpInvalid, bool * const opq_DataPoolsInvalid,
                                    bool * const opq_ApplicationsInvalid, bool * const opq_DomainsInvalid,
                                    const bool & orq_AllowComDataPoolException,
                                    std::vector<stw_types::uint32> * const opc_InvalidInterfaceIndices,
                                    std::vector<stw_types::uint32> * const opc_InvalidDataPoolIndices,
                                    std::vector<stw_types::uint32> * const opc_InvalidApplicationIndices,
                                    std::vector<stw_types::uint32> * const opc_InvalidDomainIndices) const;
   stw_types::sint32 CheckErrorBus(const stw_types::uint32 ou32_BusIndex, bool * const opq_NameConflict,
                                   bool * const opq_NameInvalid, bool * const opq_IdInvalid,
                                   bool * const opq_DataPoolsInvalid) const;
   stw_types::sint32 CheckMessageIdBus(const stw_types::uint32 ou32_BusIndex,
                                       const C_OSCCanMessageUniqueId & orc_MessageId, bool & orq_Valid,
                                       const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage = NULL) const;
   stw_types::sint32 CheckMessageNameBus(const stw_types::uint32 ou32_BusIndex,
                                         const stw_scl::C_SCLString & orc_MessageName, bool & orq_Valid, const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage =
                                            NULL) const;
   stw_types::sint32 CheckMessageMatch(const C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                       const C_OSCCanMessageIdentificationIndices & orc_MessageId2, bool & orq_IsMatch,
                                       const bool oq_IgnoreMessageDirection = false) const;
   void GetNodeIndexesOfBus(const stw_types::uint32 ou32_BusIndex, std::vector<stw_types::uint32> & orc_NodeIndexes,
                            std::vector<stw_types::uint32> & orc_InterfaceIndexes) const;
   void GetNodeAndComDpIndexesOfBus(const stw_types::uint32 ou32_BusIndex,
                                    std::vector<stw_types::uint32> & orc_NodeIndexes,
                                    std::vector<stw_types::uint32> & orc_InterfaceIndexes,
                                    std::vector<stw_types::uint32> & orc_DatapoolIndexes) const;
   void GetNodeAndComDpIndexesOfBus(const stw_types::uint32 ou32_BusIndex,
                                    const C_OSCCanProtocol::E_Type & ore_ComProtocol,
                                    std::vector<stw_types::uint32> & orc_NodeIndexes,
                                    std::vector<stw_types::uint32> & orc_InterfaceIndexes,
                                    std::vector<stw_types::uint32> & orc_DatapoolIndexes) const;

   void AddNode(C_OSCNode & orc_Node, const stw_scl::C_SCLString & orc_SubDeviceName = "",
                const stw_scl::C_SCLString & orc_MainDeviceName = "");
   void AddNodeSquad(std::vector<C_OSCNode> & orc_Nodes, const std::vector<stw_scl::C_SCLString> & orc_SubDeviceNames,
                     const stw_scl::C_SCLString & orc_MainDeviceName);

   stw_types::sint32 DeleteNode(const stw_types::uint32 ou32_NodeIndex);

   stw_types::sint32 SetNodeName(const stw_types::uint32 ou32_NodeIndex, const stw_scl::C_SCLString & orc_NodeName);
   stw_types::sint32 GetNodeSquadIndexWithNodeIndex(const stw_types::uint32 ou32_NodeIndex,
                                                    stw_types::uint32 & oru32_NodeSquadIndex) const;

   static C_OSCDeviceManager hc_Devices;     ///< container of device types known in the system
   std::vector<C_OSCNode> c_Nodes;           ///< all nodes that are part of this system definition
   std::vector<C_OSCNodeSquad> c_NodeSquads; ///< all multi CPU based devices with sub nodes of this system definition
   std::vector<C_OSCSystemBus> c_Buses;      ///< all buses that are part of this system definition

private:
   stw_types::uint32 m_GetDataPoolHash(const stw_types::uint32 ou32_NodeIndex,
                                       const stw_types::uint32 ou32_DataPoolIndex) const;
   stw_types::uint32 m_GetRelatedProtocolHash(const stw_types::uint32 ou32_NodeIndex,
                                              const stw_types::uint32 ou32_DataPoolIndex) const;

   void m_GetNodeAndComDpIndexesOfBus(const stw_types::uint32 ou32_BusIndex,
                                      const C_OSCCanProtocol::E_Type * const ope_ComProtocol,
                                      std::vector<stw_types::uint32> & orc_NodeIndexes,
                                      std::vector<stw_types::uint32> & orc_InterfaceIndexes,
                                      std::vector<stw_types::uint32> * const opc_DatapoolIndexes) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
