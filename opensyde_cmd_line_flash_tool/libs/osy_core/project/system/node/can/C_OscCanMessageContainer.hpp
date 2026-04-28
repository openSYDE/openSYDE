//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage container for all CAN message types (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANMESSAGECONTAINER_HPP
#define C_OSCCANMESSAGECONTAINER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_OscCanMessage.hpp"
#include "C_OscSystemNameMaxCharLimitChangeReportItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanMessageContainer
{
public:
   C_OscCanMessageContainer(void);

   void CalcHash(uint32_t & oru32_HashValue) const;
   void ReCalcDataElementIndices(void);
   uint32_t GetMessageSignalDataStartIndex(const bool & orq_IsTx, const uint32_t & oru32_MessageIndex) const;
   const std::vector<C_OscCanMessage> & GetMessagesConst(const bool & orq_IsTx) const;
   std::vector<C_OscCanMessage> & GetMessages(const bool & orq_IsTx);
   void CheckMessageLocalError(const C_OscNodeDataPoolList * const opc_List, const uint32_t & oru32_MessageIndex,
                               const bool & orq_IsTx, bool * const opq_NameConflict, bool * const opq_NameInvalid,
                               bool * const opq_DelayTimeInvalid, bool * const opq_IdConflict,
                               bool * const opq_IdInvalid, bool * const opq_SignalInvalid,
                               bool * const opq_NoSignalsInvalid, bool * const opq_TxMethodInvalid,
                               const uint32_t ou32_CanMessageValidSignalsDlcOffset,
                               const bool oq_CanMessageSignalGapsValid, const bool oq_ByteAlignmentRequired,
                               const bool oq_SignalsRequired, const bool oq_CanOpenPdoSyncValid) const;
   bool CheckLocalError(const C_OscNodeDataPoolList & orc_ListTx, const C_OscNodeDataPoolList & orc_ListRx,
                        const uint32_t ou32_CanMessageValidSignalsDlcOffset, const bool oq_CanMessageSignalGapsValid,
                        const bool oq_ByteAlignmentRequired, const bool oq_SignalsRequired,
                        const bool oq_CanOpenPdoSyncValid, std::vector<uint32_t> * const opc_InvalidTxMessages = NULL,
                        std::vector<uint32_t> * const opc_InvalidRxMessages = NULL) const;

   bool ContainsAtLeastOneMessage(void) const;
   bool ContainsAtLeastOneActiveMessage(void) const;
   bool CheckMinSignalError(void) const;
   void HandleNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit,
                               std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems);

   std::vector<C_OscCanMessage> c_TxMessages; ///< Transmitted message types
   std::vector<C_OscCanMessage> c_RxMessages; ///< Received message types
   bool q_IsComProtocolUsedByInterface;       ///< Flag whether com protocol is used on the corresponding CAN
   ///< interface.
   ///< Default is false

private:
   std::vector<uint32_t> m_GetHashes(const C_OscNodeDataPoolList & orc_ListTx,
                                     const C_OscNodeDataPoolList & orc_ListRx) const;

   static bool mh_CheckMinSignalErrorPerVector(const std::vector<C_OscCanMessage> & orc_Messages);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
