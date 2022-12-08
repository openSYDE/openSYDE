//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for CAN communication message related, additional information (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANMESSAGE_H
#define C_OSCCANMESSAGE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscCanSignal.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_OscCanInterfaceId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanMessage
{
public:
   C_OscCanMessage(void);

   enum E_TxMethodType ///< Transmission trigger type
   {
      eTX_METHOD_CYCLIC,                 ///< Transmission triggered by timer
      eTX_METHOD_ON_CHANGE,              ///< Transmission triggered by data change (with min/max time window)
      eTX_METHOD_ON_EVENT,               ///< Transmission triggered by application event
      eTX_METHOD_CAN_OPEN_TYPE_0,        ///< Transmission ID 0 defined by CAN open standard
      eTX_METHOD_CAN_OPEN_TYPE_1_TO_240, ///< Transmission ID 1-240 defined by CAN open standard,
      ///< exact value identified by other variable
      eTX_METHOD_CAN_OPEN_TYPE_254, ///< Transmission ID 254 defined by CAN open standard
      eTX_METHOD_CAN_OPEN_TYPE_255  ///< Transmission ID 255 defined by CAN open standard
   };

   bool operator !=(const C_OscCanMessage & orc_Cmp) const;

   void CalcHash(uint32_t & oru32_HashValue, const bool oq_R20Compatible = false) const;
   bool CheckErrorSignal(const C_OscNodeDataPoolList * const opc_List, const uint32_t & oru32_SignalIndex,
                         const uint32_t ou32_CanMessageValidSignalsDlcOffset, const bool oq_CanMessageSignalGapsValid,
                         const bool oq_ByteAlignmentRequired) const;
   void CheckErrorSignalDetailed(const C_OscNodeDataPoolList * const opc_List, const uint32_t & oru32_SignalIndex,
                                 bool * const opq_LayoutConflict, bool * const opq_BorderConflict,
                                 bool * const opq_GapConflict, bool * const opq_ByteAlignmentLengthConflict,
                                 bool * const opq_ByteAlignmentStartbitConflict, bool * const opq_NameConflict,
                                 bool * const opq_NameInvalid, bool * const opq_MinOverMax,
                                 bool * const opq_ValueBelowMin, bool * const opq_ValueOverMax,
                                 bool * const opq_NoMultiplexerButMultiplexed,
                                 bool * const opq_MultiplexedValueOutOfRange,
                                 const uint32_t ou32_CanMessageValidSignalsDlcOffset,
                                 const bool oq_CanMessageSignalGapsValid, const bool oq_ByteAlignmentRequired) const;

   bool IsTransmissionTypeOfCyclicType(void) const;
   static bool h_IsTransmissionTypeOfCyclicType(const E_TxMethodType oe_Type);
   bool IsMultiplexed(uint32_t * const opu32_MultiplexerIndex = NULL) const;
   static bool h_ContainsMultiplexer(const std::vector<C_OscCanSignal> & orc_Signals,
                                     uint32_t * const opu32_MultiplexerIndex = NULL);
   void GetMultiplexerValues(std::set<uint16_t> & orc_Values) const;

   stw::scl::C_SclString c_Name;    ///< Message name
   stw::scl::C_SclString c_Comment; ///< Message comment
   uint32_t u32_CanId;              ///< CAN message identifier
   bool q_IsExtended;               ///< Flag if message id is using extended format
   uint16_t u16_Dlc;                ///< CAN message data length code
   ///< Reserved for other protocols
   E_TxMethodType e_TxMethod; ///< Message transmission trigger type
   uint32_t u32_CycleTimeMs;  ///< ONLY used if transmission trigger is eTX_METHOD_CYCLIC
   ///< or eTX_METHOD_ON_CHANGE (but no later than).
   ///< Format is in milli seconds.
   ///< Time between transmissions.
   ///< Probably the range is 1 to 50000 ms.
   uint16_t u16_DelayTimeMs; ///< Minimum time between transmission in ms. (aka not earlier than)
   ///< ONLY used if tx method is eTX_METHOD_ON_CHANGE
   uint32_t u32_TimeoutMs; ///< Maximum time between reception of two messages.
   ///< Format is in milli seconds. If value is set to 0, monitoring is disabled.
   std::vector<C_OscCanSignal> c_Signals; ///< Communication signals

   C_OscCanInterfaceId c_CanOpenManagerOwnerNodeIndex; ///< CANopen manager specific information about
   ///< owner node:
   ///< TX-PDO: sender
   ///< RX-PDO: receiver
   bool q_CanOpenManagerCobIdIncludesNodeId; ///< CANopen manager specific information about how to calculate the
   ///< COB-ID value
   uint32_t u32_CanOpenManagerCobIdOffset; ///< CANopen manager specific information about the COB-ID offset value
   bool q_CanOpenManagerMessageActive;     ///< CANopen manager specific information about if the message is considered
   ///< active
   uint16_t u16_CanOpenManagerPdoIndex; ///< CANopen manager specific information about which PDO index this
   /// message originated from
   uint8_t u8_CanOpenTxMethodAdditionalInfo; ///< CANopen manager specific information about which exact
   ///< transmission type is used, only used for detailed information
   ///< about type eTX_METHOD_CAN_OPEN_TYPE_1_TO_240

private:
   std::vector<uint32_t> m_GetSignalHashes(const C_OscNodeDataPoolList * const opc_List,
                                           const uint32_t & oru32_SignalIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
