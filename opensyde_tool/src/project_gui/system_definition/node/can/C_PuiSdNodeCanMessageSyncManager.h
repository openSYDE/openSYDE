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
#include "stwtypes.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_OSCCanProtocol.h"
#include "C_PuiSdNodeDataPoolListElement.h"
#include "C_PuiSdNodeCanSignal.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanMessageSyncManager :
   public QObject
{
   Q_OBJECT

public:
   C_PuiSdNodeCanMessageSyncManager(QObject * const opc_Parent = NULL);

   void Init(const stw_types::uint32 & oru32_BusIndex,
             const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocol);
   void Init(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_InterfaceIndex,
             const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocol);
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> GetUniqueMessages(void) const;
   static void h_GetConnectedAndActiveInterfaces(const stw_types::uint32 ou32_BusIndex,
                                                 const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocol,
                                                 std::vector<stw_types::uint32> & orc_NodeIndexes,
                                                 std::vector<stw_types::uint32> & orc_InterfaceIndexes);

   //Wrapped functions
   //Can protocol set
   stw_types::sint32 SetCanMessageDirection(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const bool & orq_NewMessageIsTx);
   stw_types::sint32 SetCanMessagePropertiesWithoutDirectionChangeAndWithoutTimeoutChange(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_opensyde_core::C_OSCCanMessage & orc_Message) const;
   stw_types::sint32 SetCanMessageReceiveTimeout(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
      const stw_types::uint32 ou32_ReceiveTimeout) const;
   stw_types::sint32 SetCanMessageReceiveTimeoutAutoFlag(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
      const bool oq_UseAutoReceiveTimeout) const;
   stw_types::sint32 SetCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                  const stw_types::uint32 & oru32_SignalIndex,
                                  const stw_opensyde_core::C_OSCCanSignal & orc_OSCSignal,
                                  const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                  const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                  const C_PuiSdNodeCanSignal & orc_UISignal) const;
   stw_types::sint32 SetCanSignalCommon(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const stw_types::uint32 & oru32_SignalIndex,
                                        const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                        const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon) const;
   stw_types::sint32 SetCanSignalPosition(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                          const stw_types::uint32 & oru32_SignalIndex,
                                          const stw_opensyde_core::C_OSCCanSignal & orc_OSCSignal,
                                          const C_PuiSdNodeCanSignal & orc_UISignal) const;

   //Can protocol add/delete/move
   stw_types::sint32 AddCanMessage(const stw_types::uint32 & oru32_NodeIndex,
                                   const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComType,
                                   const stw_types::uint32 & oru32_InterfaceIndex, const bool & orq_MessageIsTx,
                                   const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals, stw_types::uint32 & oru32_MessageIndex);
   stw_types::sint32 InsertCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                      const stw_opensyde_core::C_OSCCanMessage & orc_Message,
                                      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals);
   stw_types::sint32 DeleteCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   stw_types::sint32 ChangeCanMessageTx(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_InterfaceIndex);
   stw_types::sint32 AddCanMessageRx(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_InterfaceIndex);
   stw_types::sint32 DeleteCanMessageRx(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const stw_types::uint32 & oru32_NodeIndex,
                                        const stw_types::uint32 & oru32_InterfaceIndex);
   stw_types::sint32 AddCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                  const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                  const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                  const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                  const C_PuiSdNodeCanSignal & orc_UISignal) const;
   stw_types::sint32 InsertCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     const stw_types::uint32 & oru32_SignalIndex,
                                     const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                     const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_OSCSignalCommon,
                                     const C_PuiSdNodeDataPoolListElement & orc_UISignalCommon,
                                     const C_PuiSdNodeCanSignal & orc_UISignal) const;
   stw_types::sint32 DeleteCanSignal(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                     const stw_types::uint32 & oru32_SignalIndex) const;
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> GetMatchingMessageVector(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;

   //Error checks
   bool CheckCriticalMessageId(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void CheckErrorBus(bool * const opq_MessageNameInvalid, bool * const opq_MessageIdInvalid,
                      bool * const opq_MessagesHaveNoTx, bool * const opq_DelayTimeInvalid,
                      bool * const opq_MessageSignalInvalid,
                      const stw_types::uint32 ou32_CANMessageValidSignalsDLCOffset) const;
   void CheckMessageIdBus(const stw_types::uint32 & oru32_MessageId, bool & orq_Valid, const stw_opensyde_core::C_OSCCanMessageIdentificationIndices * const opc_SkipMessage =
                             NULL, bool * const opq_EcosRangeError = NULL, bool * const opq_EcosEvenError = NULL,
                          bool * const opq_DuplicateDetected = NULL) const;
   void CheckMessageNameBus(const stw_scl::C_SCLString & orc_MessageName, bool & orq_Valid, const stw_opensyde_core::C_OSCCanMessageIdentificationIndices * const opc_SkipMessage =
                               NULL, bool * const opq_InvalidName = NULL,
                            bool * const opq_DuplicateName = NULL) const;
   void CheckMessageHasTx(bool & orq_Valid,
                          const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_Message) const;
   stw_types::uint32 GetNextValidMessageId(const bool & orq_Extended) const;
   bool RecheckCriticalMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                               stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_NewMessageId);
   stw_types::uint64 GetUniqueId(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   stw_opensyde_core::C_OSCCanMessageIdentificationIndices GetMessageIdForUniqueId(
      const stw_types::uint64 ou64_UniqueId)
   const;
   stw_types::sint32 GetMessageIdForMessageName(const QString & orc_MessageName,
                                                stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);

private:
   stw_types::uint32 m_GetMatchingMessageVectorIndex(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   bool m_CheckIfAlreadyExisting(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId) const;
   void m_RegisterIfNecessary(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   static bool mh_CheckIfAlreadyExisting(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                         const std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > & orc_Input);
   static void mh_RegisterIfNecessary(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                      std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > & orc_Output, std::vector<stw_types::uint64> * const opc_OutputUniqueIds, std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   void m_RemoveAndUpdateIndices(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                 const bool oq_AllowDelete);
   static void mh_RemoveAndUpdateIndices(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                         std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageIds);
   static stw_types::sint32 mh_GetNodeIndexesMatchingForMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MatchingMessageIds,
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   stw_types::sint32 m_DeleteCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                        const bool oq_AllowDelete);
   void m_UpdateIndicesToNewCanMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   static void mh_UpdateIndicesToNewCanMessage(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageIds);
   void m_UpdateIndicesToCanMessageDirectionChange(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   static void mh_UpdateIndicesToCanMessageDirectionChange(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId,
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageIds);
   static void mh_Init(const stw_types::uint32 & oru32_BusIndex,
                       const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocol,
                       std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > & orc_Output,
                       std::vector<stw_types::uint64> * const opc_OutputUniqueIds,
                       std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   static void mh_Init(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_InterfaceIndex,
                       const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_ComProtocol,
                       std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > & orc_Output,
                       std::vector<stw_types::uint64> * const opc_OutputUniqueIds,
                       std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches);
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> m_GetAllUniqueMessages(void) const;
   static std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mh_GetUniqueMessages(
      const std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > & orc_Input);
   static void mh_Append(const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_Input,
                         std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_Output);
   static void mh_HandleMessageComparison(
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageIdReference,
      const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageIdNew,
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> * const opc_MatchingMessageIds,
      std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> * const opc_CriticalMessageMatches,
      bool * const opq_CriticalMatchFoundASecondTime, bool * const opq_CriticalMatchFound);
   bool m_RecheckCriticalMessage(const stw_opensyde_core::C_OSCCanMessageIdentificationIndices & orc_MessageId);
   stw_types::uint64 m_GetNewUniqueId(void) const;
   static stw_types::uint64 mh_GetNewUniqueId(const std::vector<stw_types::uint64> & orc_ExistingUniqueIds);

   bool mq_SingleNodeMode;
   std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > mc_MessageMatches;
   ///< Level 1: Different messages
   ///< Level 2: Matching message ids (Which IDs correlate to the same message)
   std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_CriticalMessageMatches;
   ///<Problematic messages (Match but different TX)
   std::vector<std::vector<std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> > >
   mc_MessageMatchesForOtherProtocols; ///< Level 1: Different Protocols
   ///< Level 2: Different messages
   ///< Level 3: Matching message ids (Which IDs correlate to the same message)
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_Protocol; ///< Current protocol
   std::vector<stw_types::uint64> mc_MessageMatchUniqueIds; ///< Unique Ids for Matching message ids (current protocol)
   ///< Should always be in sync with mc_MessageMatches
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
