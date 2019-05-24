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

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCCanSignal.h"
#include "C_OSCNodeDataPoolList.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCCanMessage
{
public:
   C_OSCCanMessage(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   bool CheckErrorSignal(const C_OSCNodeDataPoolList * const opc_List, const stw_types::uint32 & oru32_SignalIndex,
                         const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset) const;
   void CheckErrorSignalDetailed(const C_OSCNodeDataPoolList * const opc_List,
                                 const stw_types::uint32 & oru32_SignalIndex, bool * const opq_LayoutConflict,
                                 bool * const opq_BorderConflict, bool * const opq_NameConflict,
                                 bool * const opq_NameInvalid, bool * const opq_MinOverMax,
                                 bool * const opq_ValueBelowMin, bool * const opq_ValueOverMax,
                                 const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset) const;

   enum E_TxMethodType ///< Transmission trigger type
   {
      eTX_METHOD_CYCLIC,    ///< Transmission triggered by timer
      eTX_METHOD_ON_CHANGE, ///< Transmission triggered by data change (with min/max time window)
      eTX_METHOD_ON_EVENT   ///< Transmission triggered by application event
   };

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
   ///< Format is in milli seconds
   ///< ONLY used if transmission trigger is eTX_METHOD_CYCLIC
   ///< or eTX_METHOD_ON_CHANGE.
   std::vector<C_OSCCanSignal> c_Signals; ///< Communication signals

private:
   std::vector<stw_types::uint32> m_GetSignalHashes(const C_OSCNodeDataPoolList * const opc_List,
                                                    const stw_types::uint32 & oru32_SignalIndex) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
