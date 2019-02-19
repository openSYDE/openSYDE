//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Storage container for all CAN message types (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.04.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCCANMESSAGECONTAINER_H
#define C_OSCCANMESSAGECONTAINER_H

/* -- Includes ------------------------------------------------------------- */
#include <vector>
#include "C_OSCCanMessage.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCCanMessageContainer
{
public:
   C_OSCCanMessageContainer(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   void ReCalcDataElementIndices(void);
   stw_types::uint32 GetMessageSignalDataStartIndex(const bool & orq_IsTx,
                                                    const stw_types::uint32 & oru32_MessageIndex) const;
   const std::vector<C_OSCCanMessage> & GetMessagesConst(const bool & orq_IsTx) const;
   std::vector<C_OSCCanMessage> & GetMessages(const bool & orq_IsTx);
   void CheckMessageLocalError(const C_OSCNodeDataPoolList * const opc_List,
                               const stw_types::uint32 & oru32_MessageIndex, const bool & orq_IsTx,
                               bool * const opq_NameConflict, bool * const opq_NameInvalid,
                               bool * const opq_DelayTimeInvalid, bool * const opq_IdConflict,
                               bool * const opq_IdInvalid, bool * const opq_SignalInvalid,
                               const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset) const;
   bool CheckLocalError(const C_OSCNodeDataPoolList & orc_ListTx, const C_OSCNodeDataPoolList & orc_ListRx,
                        const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset,
                        std::vector<stw_types::uint32> * const opc_InvalidTxMessages = NULL,
                        std::vector<stw_types::uint32> * const opc_InvalidRxMessages = NULL) const;

   bool ContainsAtLeastOneMessage(void) const;

   std::vector<C_OSCCanMessage> c_TxMessages; ///< Transmitted message types
   std::vector<C_OSCCanMessage> c_RxMessages; ///< Received message types
   bool q_IsComProtocolUsedByInterface;       ///< Flag whether com protocol is used on the corresponding CAN
   ///< interface.
   ///< Default is false

private:
   std::vector<stw_types::uint32> m_GetHashes(const C_OSCNodeDataPoolList & orc_ListTx,
                                              const C_OSCNodeDataPoolList & orc_ListRx) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
