//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data conversion structure

   The structure matches the information provided by a DBC file.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIECONVERTERH
#define C_CIECONVERTERH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "stwtypes.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscNode.hpp"

#include "DBC.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieConverter
{
private:
public:
   ///information about the current bus
   class C_CieSystemBus
   {
   public:
      stw::scl::C_SclString c_Name;    ///< unique name of bus
      stw::scl::C_SclString c_Comment; ///< user comment
   };

   ///information about a node
   class C_CieNodeProperties
   {
   public:
      stw::scl::C_SclString c_Name;    ///< Unique name
      stw::scl::C_SclString c_Comment; ///< User comment
   };

   class C_CieDataPoolElement
   {
   public:
      stw::scl::C_SclString c_Name;                                              ///< Element name
      stw::scl::C_SclString c_Comment;                                           ///< Element comment
      stw::opensyde_core::C_OscNodeDataPoolContent c_MinValue;                   ///< Minimum value(s) for complete data
                                                                                 // element
      stw::opensyde_core::C_OscNodeDataPoolContent c_MaxValue;                   ///< Maximum value(s) for complete data
                                                                                 // element
      float64_t f64_Factor;                                                      ///< Adaptation factor for display
      float64_t f64_Offset;                                                      ///< Offset factor for display
      stw::scl::C_SclString c_Unit;                                              ///< Variable unit
      std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> c_DataSetValues; ///< Only one element which is initial
                                                                                 // value
   };

   ///information about messages
   class C_CieCanSignal
   {
   public:
      uint32_t u32_J1939Spn;                                              ///< j1939 SPN
      stw::opensyde_core::C_OscCanSignal::E_ByteOrderType e_ComByteOrder; ///< Communication value byte order
      uint16_t u16_ComBitLength;                                          ///< Communication value bit length
      uint16_t u16_ComBitStart;                                           ///< Communication value bit start
      // position
      C_CieDataPoolElement c_Element;                              ///< Communication data pool element
      std::map<int64_t, stw::scl::C_SclString> c_ValueDescription; ///< Optional communication value
      // descriptions
      stw::opensyde_core::C_OscCanSignal::E_MultiplexerType e_MultiplexerType; ///< Signal multiplexer type
      uint16_t u16_MultiplexValue;                                             ///< Only used if eMULTIPLEXED_SIGNAL
      ///< Multiplexer value if this signal should be present and represents the associated multiplex group
   };

   class C_CieCanMessage
   {
   public:
      stw::scl::C_SclString c_Name;    ///< Message name
      stw::scl::C_SclString c_Comment; ///< Message comment
      uint32_t u32_CanId;              ///< CAN message identifier
      bool q_IsExtended;               ///< Flag if message id is using extended format
      uint16_t u16_Dlc;                ///< CAN message data length code
      ///< Reserved for other protocols
      stw::opensyde_core::C_OscCanMessage::E_TxMethodType e_TxMethod; ///< Message transmission trigger type
      uint32_t u32_CycleTimeMs;                                       ///< ONLY used if transmission trigger is
      // eTX_METHOD_CYCLIC
      std::vector<C_CieCanSignal> c_Signals; ///< Communication signals
   };

   /// Messages and appropriate warnings
   class C_CieNodeMessage
   {
   public:
      bool operator <(const C_CieNodeMessage & orc_Cmp) const;
      bool operator ==(const C_CieNodeMessage & orc_Cmp) const;

      C_CieCanMessage c_CanMessage;         ///< Transmitted message type
      stw::scl::C_SclStringList c_Warnings; ///< Appropriate warnings: if empty, there is no warning
   };

   ///all information about a single node
   class C_CieNode
   {
   public:
      C_CieNodeProperties c_Properties;           ///< General node properties
      std::vector<C_CieNodeMessage> c_TxMessages; ///< Transmitted messages with optional warnings
      std::vector<C_CieNodeMessage> c_RxMessages; ///< Received messages with optional warnings
   };

   ///COMM data structure for import/export from/to DBC file
   ///with information about bus and all found nodes
   class C_CieCommDefinition
   {
   public:
      C_CieSystemBus c_Bus;           ///< the all node are connected to
      std::vector<C_CieNode> c_Nodes; ///< all nodes for this bus
      std::vector<C_CieNodeMessage> c_UnmappedMessages;
   };
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
