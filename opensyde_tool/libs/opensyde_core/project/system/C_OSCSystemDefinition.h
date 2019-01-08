//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Store system definition data

   (See .cpp file for full description)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     02.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCSYSTEMDEFINITIONH
#define C_OSCSYSTEMDEFINITIONH

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "C_OSCNode.h"
#include "C_OSCSystemBus.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCDeviceManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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
   bool CheckBusIdAvailable(const stw_types::uint8 ou8_BusId,
                            const stw_types::uint32 * const opu32_BusIndexToSkip = NULL) const;
   stw_types::sint32 GetNextFreeBusId(stw_types::uint8 & oru8_BusId) const;
   stw_types::sint32 CheckErrorNode(const stw_types::uint32 ou32_NodeIndex, bool * const opq_NameConflict,
                                    bool * const opq_NameInvalid, bool * const opq_NodeIdInvalid,
                                    bool * const opq_DataPoolsInvalid, bool * const opq_ApplicationsInvalid,
                                    const bool & orq_AllowComDataPoolException,
                                    std::vector<stw_types::uint32> * const opc_InvalidInterfaceIndices,
                                    std::vector<stw_types::uint32> * const opc_InvalidDataPoolIndices,
                                    std::vector<stw_types::uint32> * const opc_InvalidApplicationIndices) const;
   stw_types::sint32 CheckErrorBus(const stw_types::uint32 ou32_BusIndex, bool * const opq_NameConflict,
                                   bool * const opq_NameInvalid, bool * const opq_IdInvalid,
                                   bool * const opq_DataPoolsInvalid) const;
   stw_types::sint32 CheckMessageIdBus(const stw_types::uint32 ou32_BusIndex, const stw_types::uint32 ou32_MessageId,
                                       bool & orq_Valid,
                                       const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage = NULL) const;
   stw_types::sint32 CheckMessageNameBus(const stw_types::uint32 ou32_BusIndex,
                                         const stw_scl::C_SCLString & orc_MessageName, bool & orq_Valid, const C_OSCCanMessageIdentificationIndices * const opc_SkipMessage =
                                            NULL) const;
   stw_types::sint32 CheckMessageMatch(const C_OSCCanMessageIdentificationIndices & orc_MessageId1,
                                       const C_OSCCanMessageIdentificationIndices & orc_MessageId2, bool & orq_IsMatch,
                                       const bool oq_IgnoreMessageDirection = false) const;
   void GetNodeIndexesOfBus(const stw_types::uint32 ou32_BusIndex, std::vector<stw_types::uint32> & orc_NodeIndexes,
                            std::vector<stw_types::uint32> & orc_InterfaceIndexes) const;
   void GetMessageReceiversIndices(const stw_types::uint32 ou32_BusIndex, const stw_scl::C_SCLString & orc_MessageName,
                                   std::vector<stw_types::uint32> & orc_NodeIndices,
                                   std::vector<stw_types::uint32> & orc_InterfaceIndices) const;
   void GetMessageTransmittersIndices(const stw_types::uint32 ou32_BusIndex,
                                      const stw_scl::C_SCLString & orc_MessageName,
                                      std::vector<stw_types::uint32> & orc_NodeIndices,
                                      std::vector<stw_types::uint32> & orc_InterfaceIndices) const;

   void AddNode(C_OSCNode & orc_Node);

   static C_OSCDeviceManager hc_Devices; ///< container of device types known in the system
   std::vector<C_OSCNode> c_Nodes;       ///< all nodes that are part of this system definition
   std::vector<C_OSCSystemBus> c_Buses;  ///< all buses that are part of this system definition
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
