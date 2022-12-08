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
#include "C_OscNode.hpp"
#include "C_OscNodeSquad.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscDeviceManager.hpp"
#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemDefinition
{
public:
   C_OscSystemDefinition(void);
   virtual ~C_OscSystemDefinition(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   void AddBus(const C_OscSystemBus & orc_Bus);
   int32_t InsertBus(const uint32_t ou32_BusIndex, const C_OscSystemBus & orc_Bus);
   int32_t DeleteBus(const uint32_t ou32_BusIndex);
   int32_t AddConnection(const uint32_t ou32_NodeIndex, const uint32_t ou32_BusIndex, const uint8_t ou8_Interface);
   bool CheckInterfaceIsAvailable(const uint32_t ou32_NodeIndex, const uint32_t ou32_ComIndex,
                                  const uint8_t ou8_ComNodeId) const;
   bool CheckIpAddressIsValid(const uint32_t ou32_NodeIndex, const uint32_t ou32_ComIndex,
                              const std::vector<int32_t> & orc_Ip) const;
   bool CheckBusIdAvailable(const uint8_t ou8_BusId, const uint32_t * const opu32_BusIndexToSkip = NULL) const;
   int32_t GetNextFreeBusId(uint8_t & oru8_BusId) const;
   int32_t CheckErrorNode(const uint32_t ou32_NodeIndex, bool * const opq_NameConflict, bool * const opq_NameInvalid,
                          bool * const opq_NodeIdInvalid, bool * const opq_IpInvalid, bool * const opq_DataPoolsInvalid,
                          bool * const opq_ApplicationsInvalid, bool * const opq_DomainsInvalid,
                          bool * const opq_CommSignalCountInvalid, bool * const opq_CoPdoCountInvalid,
                          bool * const opq_CoNodeIdInvalid, bool * const opq_CoHearbeatTimeInvalid,
                          const bool & orq_AllowComDataPoolException,
                          std::vector<uint32_t> * const opc_InvalidInterfaceIndices,
                          std::vector<uint32_t> * const opc_InvalidDataPoolIndices,
                          std::vector<uint32_t> * const opc_InvalidApplicationIndices,
                          std::vector<uint32_t> * const opc_InvalidDomainIndices,
                          std::vector<C_OscCanProtocol::E_Type> * const opc_InvalidProtocolTypes) const;
   int32_t CheckErrorBus(const uint32_t ou32_BusIndex, bool * const opq_NameConflict, bool * const opq_NameInvalid,
                         bool * const opq_IdInvalid, bool * const opq_DataPoolsInvalid) const;
   int32_t CheckMessageIdBus(const uint32_t ou32_BusIndex, const C_OscCanMessageUniqueId & orc_MessageId,
                             bool & orq_Valid,
                             const C_OscCanMessageIdentificationIndices * const opc_SkipMessage = NULL) const;
   int32_t CheckMessageNameBus(const uint32_t ou32_BusIndex, const stw::scl::C_SclString & orc_MessageName,
                               bool & orq_Valid, const C_OscCanMessageIdentificationIndices * const opc_SkipMessage =
                                  NULL) const;
   int32_t CheckMessageMatch(const C_OscCanMessageIdentificationIndices & orc_MessageId1,
                             const C_OscCanMessageIdentificationIndices & orc_MessageId2, bool & orq_IsMatch,
                             const bool oq_IgnoreMessageDirection = false) const;
   void GetNodeIndexesOfBus(const uint32_t ou32_BusIndex, std::vector<uint32_t> & orc_NodeIndexes,
                            std::vector<uint32_t> & orc_InterfaceIndexes) const;
   void GetNodeAndComDpIndexesOfBus(const uint32_t ou32_BusIndex, std::vector<uint32_t> & orc_NodeIndexes,
                                    std::vector<uint32_t> & orc_InterfaceIndexes,
                                    std::vector<uint32_t> & orc_DatapoolIndexes) const;
   void GetNodeAndComDpIndexesOfBus(const uint32_t ou32_BusIndex, const C_OscCanProtocol::E_Type & ore_ComProtocol,
                                    std::vector<uint32_t> & orc_NodeIndexes,
                                    std::vector<uint32_t> & orc_InterfaceIndexes,
                                    std::vector<uint32_t> & orc_DatapoolIndexes) const;

   void AddNode(C_OscNode & orc_Node, const stw::scl::C_SclString & orc_SubDeviceName = "",
                const stw::scl::C_SclString & orc_MainDeviceName = "");
   void AddNodeSquad(std::vector<C_OscNode> & orc_Nodes, const std::vector<stw::scl::C_SclString> & orc_SubDeviceNames,
                     const stw::scl::C_SclString & orc_MainDeviceName);

   int32_t DeleteNode(const uint32_t ou32_NodeIndex);

   int32_t SetNodeName(const uint32_t ou32_NodeIndex, const stw::scl::C_SclString & orc_NodeName);
   int32_t GetNodeSquadIndexWithNodeIndex(const uint32_t ou32_NodeIndex, uint32_t & oru32_NodeSquadIndex) const;

   static C_OscDeviceManager hc_Devices;     ///< container of device types known in the system
   std::vector<C_OscNode> c_Nodes;           ///< all nodes that are part of this system definition
   std::vector<C_OscNodeSquad> c_NodeSquads; ///< all multi CPU based devices with sub nodes of this system definition
   std::vector<C_OscSystemBus> c_Buses;      ///< all buses that are part of this system definition

private:
   uint32_t m_GetDataPoolHash(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) const;
   uint32_t m_GetRelatedProtocolHash(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) const;

   void m_GetNodeAndComDpIndexesOfBus(const uint32_t ou32_BusIndex,
                                      const C_OscCanProtocol::E_Type * const ope_ComProtocol,
                                      std::vector<uint32_t> & orc_NodeIndexes,
                                      std::vector<uint32_t> & orc_InterfaceIndexes,
                                      std::vector<uint32_t> * const opc_DatapoolIndexes) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
