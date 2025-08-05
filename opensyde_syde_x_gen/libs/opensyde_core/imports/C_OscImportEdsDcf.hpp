//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of file type .eds and .dcf (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIMPORTEDSDCF_HPP
#define C_OSCIMPORTEDSDCF_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscCanMessage.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscCanOpenObjectDictionary.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_OscEdsDcfImportMessageGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscImportEdsDcf
{
public:
   static int32_t h_Import(const stw::scl::C_SclString & orc_FilePath, const uint8_t ou8_NodeId,
                           C_OscEdsDcfImportMessageGroup & orc_AllRxMessageData,
                           C_OscEdsDcfImportMessageGroup & orc_AllTxMessageData,
                           std::vector<std::vector<stw::scl::C_SclString> > & orc_ImportMessagesPerMessage,
                           stw::scl::C_SclString & orc_ParsingError,
                           const C_OscCanProtocol::E_Type oe_ImportForProtocol,
                           C_OscEdsDcfImportMessageGroup & orc_AllInvalidRxMessageData,
                           C_OscEdsDcfImportMessageGroup & orc_AllInvalidTxMessageData,
                           std::vector<std::vector<stw::scl::C_SclString> > & orc_InvalidImportMessagesPerMessage);
   static int32_t h_ParseSignalContent(const std::map<uint16_t,
                                                      C_OscCanOpenObject> & orc_CoObjects,
                                       const uint32_t ou32_CoSignalObjectIndex,
                                       const uint32_t ou32_CoSignalObjectSubIndex, const uint32_t ou32_StartBitCounter,
                                       const bool oq_RestrictForCanOpenUsage, const bool oq_IsEds,
                                       C_OscCanSignal & orc_CurSignal,
                                       C_OscNodeDataPoolListElement & orc_CurDataPoolSignal, bool & orq_DefaultMinMax);
   static stw::scl::C_SclString h_GetObjectName(const C_OscCanOpenObjectData & orc_CoObject);
   static stw::scl::C_SclString h_GetCoObjectValue(const C_OscCanOpenObjectData & orc_CoObject, const bool oq_IsEds);

private:
   C_OscImportEdsDcf(void);

   static const C_OscCanOpenObjectData * mh_GetCoObject(const std::map<uint16_t, C_OscCanOpenObject> & orc_CoObjects,
                                                        const uint32_t ou32_Id, const int32_t os32_SubIndex);
   static int32_t mh_ParseMessages(const uint32_t ou32_StartingId, const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                                            C_OscCanOpenObject> & orc_CoObjects, const std::vector<uint32_t> & orc_Dummies, C_OscEdsDcfImportMessageGroup & orc_AllMessageData, const bool oq_IsEds, std::vector<std::vector<stw::scl::C_SclString> > & orc_ImportMessages, const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage, const bool oq_ImportSrdoUseCase, C_OscEdsDcfImportMessageGroup & orc_AllInvalidMessageData, std::vector<std::vector<stw::scl::C_SclString> > & orc_InvalidImportMessages);
   static int32_t mh_DoesInformationDirectionMatchToParsedMessages(const uint32_t ou32_StartingId,
                                                                   const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                                            C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, const bool oq_IsTx, const uint32_t ou32_MessageIndex, bool & orq_Matches);
   static int32_t mh_ParseMessageContent(const uint32_t ou32_StartingId, const uint8_t ou8_NodeId,
                                         const std::map<uint16_t, C_OscCanOpenObject> & orc_CoObjects,
                                         const std::vector<uint32_t> & orc_Dummies,
                                         C_OscEdsDcfImportMessageGroup & orc_AllMessageData, const bool oq_IsEds,
                                         std::vector<std::vector<stw::scl::C_SclString> > & orc_ImportMessages,
                                         const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                         const bool oq_ImportSrdoUseCase,
                                         const C_OscCanOpenObjectData & orc_CoMessageMainObject,
                                         const uint32_t ou32_ItMessage, const uint32_t ou32_CobId,
                                         C_OscEdsDcfImportMessageGroup & orc_AllInvalidMessageData,
                                         std::vector<std::vector<stw::scl::C_SclString> > & orc_InvalidImportMessages,
                                         const bool oq_CobIdIncludesNodeId);
   static int32_t mh_LoadMessageTransmissionType(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                 const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                          C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, const bool oq_ImportSrdoUseCase, std::vector<stw::scl::C_SclString> & orc_CurMessages, C_OscCanMessage & orc_Message);
   static void mh_LoadMessageTransmissionTypeCanOpen(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                     const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                              C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, bool & orq_AddToSkippedMessages, std::vector<stw::scl::C_SclString> & orc_CurMessages, C_OscCanMessage & orc_Message);
   static void mh_LoadEventTimerSection(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                        const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                 C_OscCanOpenObject> & orc_CoObjects,
                                        const bool oq_IsEds, const bool oq_IsTx,
                                        std::vector<stw::scl::C_SclString> & orc_CurMessages,
                                        C_OscCanMessage & orc_Message);
   static void mh_LoadSrdoCyclicSection(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                        const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                 C_OscCanOpenObject> & orc_CoObjects,
                                        const bool oq_IsEds, std::vector<stw::scl::C_SclString> & orc_CurMessages,
                                        C_OscCanMessage & orc_Message);
   static void mh_LoadEventTimerSectionCanOpen(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                               const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                        C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, const bool oq_IsTx, C_OscCanMessage & orc_Message);
   static void mh_LoadInhibitTimeSectionCanOpen(const uint32_t ou32_StartingId, const uint32_t ou32_ItMessage,
                                                const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                                         C_OscCanOpenObject> & orc_CoObjects, const bool oq_IsEds, std::vector<stw::scl::C_SclString> & orc_CurMessages, C_OscCanMessage & orc_Message);
   static int32_t mh_ParseSignals(const uint32_t ou32_CoMessageId, const uint16_t ou16_MappingOffset,
                                  const uint8_t ou8_NodeId, const std::map<uint16_t,
                                                                           C_OscCanOpenObject> & orc_CoObjects,
                                  const std::vector<uint32_t> & orc_Dummies, C_OscCanMessage & orc_OscMessageData,
                                  std::vector<C_OscNodeDataPoolListElement> & orc_OscSignalData,
                                  std::vector<uint8_t> & orc_SignalDefaultMinMaxValuesUsed, const bool oq_IsEds,
                                  const bool oq_RestrictForCanOpenUsage, const bool oq_ImportSrdoUseCase,
                                  std::vector<stw::scl::C_SclString> & orc_ImportMessages);
   static int32_t mh_GetIntegerValue(const stw::scl::C_SclString & orc_CoValue, const uint8_t ou8_NodeId,
                                     uint32_t & oru32_Value, bool * const opq_IncludesNodeId = NULL);
   static int32_t mh_GetUnsigned64IntegerValue(const stw::scl::C_SclString & orc_CoValue, int32_t & ors32_Value);
   static int32_t mh_Get64IntegerValue(const stw::scl::C_SclString & orc_CoValue, int64_t & ors64_Value);
   static int32_t mh_GetIntegerValueSimple(const stw::scl::C_SclString & orc_CoValue, uint32_t & oru32_Value);
   static void mh_AddUserMessage(const uint32_t ou32_CoObjectId, const stw::scl::C_SclString & orc_CoSectionName,
                                 const stw::scl::C_SclString & orc_ErrorMessage, const int32_t os32_CoSubSectionId,
                                 const bool oq_IsError,
                                 std::vector<stw::scl::C_SclString> * const opc_ImportMessages = NULL);
   static stw::scl::C_SclString mh_GetNumberAsHex(const uint32_t ou32_Number);
   static int32_t mh_CalcMinMaxInit(const C_OscCanOpenObjectData * const opc_CoSignalObject,
                                    C_OscNodeDataPoolListElement & orc_Element, const uint16_t ou16_NumberBits,
                                    const bool oq_IsEds, bool & orq_DefaultMinMax);
   static void mh_LoadDummies(const stw::scl::C_SclString & orc_FilePath, std::vector<uint32_t> & orc_Dummies);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
