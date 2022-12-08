//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class to initially match all signals for one bus & then keep them sync for operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECANMESSAGESYNCMANAGER_H
#define C_PUISDNODECANMESSAGESYNCMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <QObject>
#include <QString>
#include "stwtypes.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdNodeDataPoolListElement.hpp"
#include "C_PuiSdNodeCanMessage.hpp"
#include "C_PuiSdNodeCanSignal.hpp"
#include "C_OscCanMessageUniqueId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanMessageSyncManager :
   public QObject
{
   Q_OBJECT

public:
   C_PuiSdNodeCanMessageSyncManager(QObject * const opc_Parent = NULL);

   void Init(const uint32_t & oru32_BusIndex, const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocol);
   void Init(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_InterfaceIndex,
             const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocol);
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> GetUniqueMessages(void) const;
   uint32_t GetUniqueMessageCount(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ComProtocol,
                                  uint32_t * const opu32_SignalCount = NULL) const;
   stw::opensyde_core::C_OscCanProtocol::E_Type GetCurrentComProtocol(void) const;
   static void h_GetConnectedAndActiveInterfaces(const uint32_t ou32_BusIndex,
                                                 const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocol,
                                                 std::vector<uint32_t> & orc_NodeIndexes,
                                                 std::vector<uint32_t> & orc_InterfaceIndexes,
                                                 std::vector<uint32_t> & orc_DatapoolIndexes);

   //Wrapped functions
   //Can protocol set
   int32_t SetCanMessageDirection(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                  const bool & orq_NewMessageIsTx);
   int32_t SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const stw::opensyde_core::C_OscCanMessage & orc_Message) const;
   int32_t SetCanMessageReceiveTimeout(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                       const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                       const uint32_t ou32_DatapoolIndex, const uint32_t ou32_ReceiveTimeout) const;
   int32_t SetCanMessageReceiveTimeoutAutoFlag(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex, const uint32_t ou32_DatapoolIndex,
      const C_PuiSdNodeCanMessage::E_RxTimeoutMode & ore_ReceiveTimeoutMode) const;
   int32_t SetCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const uint32_t & oru32_SignalIndex, const stw::opensyde_core::C_OscCanSignal & orc_OscSignal,
                        const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                        const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                        const C_PuiSdNodeCanSignal & orc_UiSignal) const;
   int32_t SetCanSignalMuxValue(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint32_t & oru32_SignalIndex, const uint16_t ou16_MultiplexValue) const;
   int32_t SetCanSignalPosition(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                const uint32_t & oru32_SignalIndex,
                                const stw::opensyde_core::C_OscCanSignal & orc_OscSignal,
                                const C_PuiSdNodeCanSignal & orc_UiSignal) const;

   //Can protocol add/delete/move
   int32_t AddCanMessage(const uint32_t & oru32_NodeIndex,
                         const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComType,
                         const uint32_t & oru32_InterfaceIndex, const uint32_t & oru32_DatapoolIndex,
                         const bool & orq_MessageIsTx, const stw::opensyde_core::C_OscCanMessage & orc_Message,
                         const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                         const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                         const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals, uint32_t & oru32_MessageIndex);
   int32_t InsertCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                            const stw::opensyde_core::C_OscCanMessage & orc_Message,
                            const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons,
                            const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons,
                            const C_PuiSdNodeCanMessage & orc_UiMessage);
   int32_t DeleteCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   int32_t ChangeCanMessageTx(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                              const uint32_t & oru32_NodeIndex, const uint32_t & oru32_InterfaceIndex,
                              const uint32_t ou32_DatapoolIndex);
   int32_t AddCanMessageRx(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                           const uint32_t ou32_DatapoolIndex);
   int32_t DeleteCanMessageRx(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                              const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                              const uint32_t ou32_DatapoolIndex);
   int32_t AddCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                        const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                        const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                        const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                        const C_PuiSdNodeCanSignal & orc_UiSignal) const;
   int32_t InsertCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint32_t & oru32_SignalIndex, const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                           const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_OscSignalCommon,
                           const C_PuiSdNodeDataPoolListElement & orc_UiSignalCommon,
                           const C_PuiSdNodeCanSignal & orc_UiSignal) const;
   int32_t DeleteCanSignal(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                           const uint32_t & oru32_SignalIndex) const;
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> GetMatchingMessageVector(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;

   void ReplaceMessageIdWithMatchingId(stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;

   //Error checks
   bool CheckCriticalMessageId(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void CheckErrorBus(bool * const opq_MessageNameInvalid, bool * const opq_MessageIdInvalid,
                      bool * const opq_MessagesHaveNoTx, bool * const opq_DelayTimeInvalid,
                      bool * const opq_TxMethodInvalid, bool * const opq_MessageSignalInvalid,
                      const uint32_t ou32_CanMessageValidSignalsDlcOffset, const bool oq_CanMessageSignalGapsValid,
                      const bool oq_ByteAlignmentRequired, const bool oq_SignalsRequired) const;
   void CheckMessageIdBus(const stw::opensyde_core::C_OscCanMessageUniqueId & orc_MessageId, bool & orq_Valid, const stw::opensyde_core::C_OscCanMessageIdentificationIndices * const opc_SkipMessage =
                             NULL, bool * const opq_EcosRangeError = NULL, bool * const opq_EcosEvenError = NULL,
                          bool * const opq_DuplicateDetected = NULL) const;
   void CheckMessageNameBus(const stw::scl::C_SclString & orc_MessageName, bool & orq_Valid, const stw::opensyde_core::C_OscCanMessageIdentificationIndices * const opc_SkipMessage =
                               NULL, bool * const opq_InvalidName = NULL,
                            bool * const opq_DuplicateName = NULL) const;
   void CheckMessageHasTx(bool & orq_Valid,
                          const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_Message) const;
   uint32_t GetNextValidMessageId(const bool & orq_Extended) const;
   bool RecheckCriticalMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                               stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_NewMessageId);
   uint64_t GetUniqueId(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   stw::opensyde_core::C_OscCanMessageIdentificationIndices GetMessageIdForUniqueId(const uint64_t ou64_UniqueId)
   const;
   int32_t GetMessageIdForMessageName(const QString & orc_MessageName,
                                      stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);

private:
   uint32_t m_GetMatchingMessageVectorIndex(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId) const;
   void m_RegisterIfNecessary(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   static bool mh_CheckIfAlreadyExisting(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                         const std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > & orc_Input);
   static void mh_RegisterIfNecessary(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                      std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > & orc_Output, std::vector<uint64_t> * const opc_OutputUniqueIds, std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   void m_RemoveAndUpdateIndices(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                 const bool oq_AllowDelete);
   static void mh_RemoveAndUpdateIndices(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                         std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageIds);
   static int32_t mh_GetNodeIndexesMatchingForMessage(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MatchingMessageIds,
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   int32_t m_DeleteCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                              const bool oq_AllowDelete);
   void m_UpdateIndicesToNewCanMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   static void mh_UpdateIndicesToNewCanMessage(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageIds);
   void m_UpdateIndicesToCanMessageDirectionChange(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   static void mh_UpdateIndicesToCanMessageDirectionChange(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageIds);
   static void mh_Init(const uint32_t & oru32_BusIndex,
                       const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocol,
                       std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > & orc_Output,
                       std::vector<uint64_t> * const opc_OutputUniqueIds,
                       std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   static void mh_Init(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_InterfaceIndex,
                       const stw::opensyde_core::C_OscCanProtocol::E_Type & ore_ComProtocol,
                       std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > & orc_Output,
                       std::vector<uint64_t> * const opc_OutputUniqueIds,
                       std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> m_GetAllUniqueMessages(void) const;
   static std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> mh_GetUniqueMessages(
      const std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > & orc_Input,
      const bool oq_CheckForMessageActiveFlag = false);
   static void mh_Append(const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_Input,
                         std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_Output);
   static void mh_HandleMessageComparison(
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageIdReference,
      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageIdNew,
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> * const opc_MatchingMessageIds,
      std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> * const opc_CriticalMessageMatches,
      bool * const opq_CriticalMatchFoundSecondTime, bool * const opq_CriticalMatchFound);
   bool m_RecheckCriticalMessage(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   uint64_t m_GetNewUniqueId(void) const;
   static uint64_t mh_GetNewUniqueId(const std::vector<uint64_t> & orc_ExistingUniqueIds);
   void m_ReportCanOpenUsage(const QString & orc_Function) const;
   static void mh_ReportCanOpenUsage(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_Type,
                                     const QString & orc_Function);

   bool mq_SingleNodeMode;
   uint32_t mu32_BusIndex; // In Bus mode
   std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > mc_MessageMatches;
   ///< Level 1: Different messages
   ///< Level 2: Matching message ids (Which IDs correlate to the same message)
   std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> mc_CriticalMessageMatches;
   ///<Problematic messages (Match but different Tx)
   std::vector<std::vector<std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> > >
   mc_MessageMatchesForOtherProtocols; ///< Level 1: Different Protocols
   ///< Level 2: Different messages
   ///< Level 3: Matching message ids (Which IDs correlate to the same message)
   std::vector<stw::opensyde_core::C_OscCanProtocol::E_Type> mc_OtherProtocols;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_Protocol; ///< Current protocol
   std::vector<uint64_t> mc_MessageMatchUniqueIds;           ///< Unique Ids for Matching message ids (current protocol)
   ///< Should always be in sync with mc_MessageMatches
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
