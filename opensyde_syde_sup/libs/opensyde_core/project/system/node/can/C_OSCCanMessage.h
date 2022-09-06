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

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCCanSignal.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_OSCCanInterfaceId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanMessage
{
public:
   C_OSCCanMessage(void);

   enum E_TxMethodType ///< Transmission trigger type
   {
      eTX_METHOD_CYCLIC,            ///< Transmission triggered by timer
      eTX_METHOD_ON_CHANGE,         ///< Transmission triggered by data change (with min/max time window)
      eTX_METHOD_ON_EVENT,          ///< Transmission triggered by application event
      eTX_METHOD_CAN_OPEN_TYPE_254, ///< Transmission ID 254 defined by CAN open standard
      eTX_METHOD_CAN_OPEN_TYPE_255  ///< Transmission ID 255 defined by CAN open standard
   };

   bool operator !=(const C_OSCCanMessage & orc_Cmp) const;

   void CalcHash(stw_types::uint32 & oru32_HashValue, const bool oq_R20Compatible = false) const;
   bool CheckErrorSignal(const C_OSCNodeDataPoolList * const opc_List, const stw_types::uint32 & oru32_SignalIndex,
                         const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset,
                         const bool oq_CANMessageSignalGapsValid, const bool oq_ByteAlignmentRequired) const;
   void CheckErrorSignalDetailed(const C_OSCNodeDataPoolList * const opc_List,
                                 const stw_types::uint32 & oru32_SignalIndex, bool * const opq_LayoutConflict,
                                 bool * const opq_BorderConflict, bool * const opq_GapConflict,
                                 bool * const opq_ByteAlignmentLengthConflict,
                                 bool * const opq_ByteAlignmentStartbitConflict, bool * const opq_NameConflict,
                                 bool * const opq_NameInvalid, bool * const opq_MinOverMax,
                                 bool * const opq_ValueBelowMin, bool * const opq_ValueOverMax,
                                 bool * const opq_NoMultiplexerButMultiplexed,
                                 bool * const opq_MultiplexedValueOutOfRange,
                                 const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset,
                                 const bool oq_CANMessageSignalGapsValid, const bool oq_ByteAlignmentRequired) const;

   bool IsTransmissionTypeACyclicType(void) const;
   static bool h_IsTransmissionTypeACyclicType(const E_TxMethodType oe_Type);
   bool IsMultiplexed(stw_types::uint32 * const opu32_MultiplexerIndex = NULL) const;
   static bool h_ContainsMultiplexer(const std::vector<C_OSCCanSignal> & orc_Signals,
                                     stw_types::uint32 * const opu32_MultiplexerIndex = NULL);
   void GetMultiplexerValues(std::set<stw_types::uint16> & orc_Values) const;

   stw_scl::C_SCLString c_Name;    ///< Message name
   stw_scl::C_SCLString c_Comment; ///< Message comment
   stw_types::uint32 u32_CanId;    ///< CAN message identifier
   bool q_IsExtended;              ///< Flag if message id is using extended format
   stw_types::uint16 u16_Dlc;      ///< CAN message data length code
   ///< Reserved for other protocols
   E_TxMethodType e_TxMethod;         ///< Message transmission trigger type
   stw_types::uint32 u32_CycleTimeMs; ///< ONLY used if transmission trigger is eTX_METHOD_CYCLIC
   ///< or eTX_METHOD_ON_CHANGE (but no later than).
   ///< Format is in milli seconds.
   ///< Time between transmissions.
   ///< Probably the range is 1 to 50000 ms.
   stw_types::uint16 u16_DelayTimeMs; ///< Minimum time between transmission in ms. (aka not earlier than)
   ///< ONLY used if tx method is eTX_METHOD_ON_CHANGE
   stw_types::uint32 u32_TimeoutMs; ///< Maximum time between reception of two messages.
   ///< Format is in milli seconds. If value is set to 0, monitoring is disabled.
   std::vector<C_OSCCanSignal> c_Signals; ///< Communication signals

   C_OSCCanInterfaceId c_CanOpenManagerOwnerNodeIndex; ///< CANopen manager specific information about
   ///< owner node:
   ///< TX-PDO: sender
   ///< RX-PDO: receiver
   bool q_CanOpenManagerCobIdIncludesNodeID; ///< CANopen manager specific information about how to calculate the
   ///< COB-ID value
   stw_types::uint32 u32_CanOpenManagerCobIdOffset; ///< CANopen manager specific information about the COB-ID offset
   ///< value
   bool q_CanOpenManagerMessageActive; ///< CANopen manager specific information about if the message is considered
   ///< active
   stw_types::uint16 u16_CanOpenManagerPdoIndex; ///< CANopen manager specific information about which PDO index this
   /// message originated from

private:
   std::vector<stw_types::uint32> m_GetSignalHashes(const C_OSCNodeDataPoolList * const opc_List,
                                                    const stw_types::uint32 & oru32_SignalIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
