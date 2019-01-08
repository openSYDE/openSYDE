//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data conversion structure

   The structure matches the information provided by a DBC file.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.10.2017  STW/U.Roesch
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CIECONVERTERH
#define C_CIECONVERTERH

/* -- Includes ------------------------------------------------------------- */
#include "CSCLString.h"
#include "CSCLStringList.h"
#include "stwtypes.h"
#include "C_OSCSystemBus.h"
#include "C_OSCNode.h"

#include "DBC.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CieConverter
{
private:
public:
   ///information about the current bus
   struct C_CIESystemBus
   {
      stw_scl::C_SCLString c_Name;    ///< unique name of bus
      stw_scl::C_SCLString c_Comment; ///< user comment
   };

   ///information about a node
   struct C_CIENodeProperties
   {
      stw_scl::C_SCLString c_Name;    ///< Unique name
      stw_scl::C_SCLString c_Comment; ///< User comment
   };

   struct C_CIEDataPoolElement
   {
      stw_scl::C_SCLString c_Name;                                              ///< Element name
      stw_scl::C_SCLString c_Comment;                                           ///< Element comment
      stw_opensyde_core::C_OSCNodeDataPoolContent c_MinValue;                   ///< Minimum value(s) for complete data
                                                                                // element
      stw_opensyde_core::C_OSCNodeDataPoolContent c_MaxValue;                   ///< Maximum value(s) for complete data
                                                                                // element
      stw_types::float64 f64_Factor;                                            ///< Adaptation factor for display
      stw_types::float64 f64_Offset;                                            ///< Offset factor for display
      stw_scl::C_SCLString c_Unit;                                              ///< Variable unit
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> c_DataSetValues; ///< Only one element which is initial
                                                                                // value
   };

   ///information about messages
   struct C_CIECanSignal
   {
      stw_opensyde_core::C_OSCCanSignal::E_ByteOrderType e_ComByteOrder;    ///< Communication value byte order
      stw_types::uint16 u16_ComBitLength;                                   ///< Communication value bit length
      stw_types::uint16 u16_ComBitStart;                                    ///< Communication value bit start position
      C_CIEDataPoolElement c_Element;                                       ///< Communication data pool element
      std::map<stw_types::uint32, stw_scl::C_SCLString> c_ValueDescription; ///< Optional communication value
                                                                            // descriptions
   };

   struct C_CIECanMessage
   {
      stw_scl::C_SCLString c_Name;    ///< Message name
      stw_scl::C_SCLString c_Comment; ///< Message comment
      stw_types::uint32 u32_CanId;    ///< CAN message identifier
      bool q_IsExtended;              ///< Flag if message id is using extended format
      stw_types::uint16 u16_Dlc;      ///< CAN message data length code
      ///< Reserved for other protocols
      stw_opensyde_core::C_OSCCanMessage::E_TxMethodType e_TxMethod; ///< Message transmission trigger type
      stw_types::uint32 u32_CycleTimeMs;                             ///< ONLY used if transmission trigger is
                                                                     // eTX_METHOD_CYCLIC
      ///< or eTX_METHOD_CYCLIC_ON_CHANGE.
      ///< Format is in milli seconds.
      ///< Time between transmissions.
      ///< Probably the range is 1 to 50000 ms.
      //      stw_types::uint16 u16_DelayTimeMs;     ///< Minimum time between transmission in ms.
      //                                             ///< ONLY used if tx method is eTX_METHOD_ON_CHANGE
      //                                             ///< or eTX_METHOD_CYCLIC_ON_CHANGE. ->UR: unused
      std::vector<C_CIECanSignal> c_Signals; ///< Communication signals
   };

   //   struct C_CIEDataPoolList
   //   {
   //      std::vector<C_CIEDataPoolListElement> c_Elements;  ///< List variables
   //      std::vector<C_OSCNodeDataPoolDataSet> c_DataSets;  ///< Data sets

   //   };

   /// Messages and appropriate warnings
   struct C_CIENodeMessage
   {
      C_CIECanMessage c_CanMessage;        ///< Transmitted message type
      stw_scl::C_SCLStringList c_Warnings; ///< Appropriate warnings: if empty, there is no warning
   };

   ///all information about a single node
   struct C_CIENode
   {
      C_CIENodeProperties c_Properties;           ///< General node properties
      std::vector<C_CIENodeMessage> c_TxMessages; ///< Transmitted messages with optional warnings
      std::vector<C_CIENodeMessage> c_RxMessages; ///< Received messages with optional warnings
   };

   ///COMM data structure for import/export from/to DBC file
   ///with information about bus and all found nodes
   struct C_CIECommDefinition
   {
      C_CIESystemBus c_Bus;           ///< the all node are connected to
      std::vector<C_CIENode> c_Nodes; ///< all nodes for this bus
   };
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
