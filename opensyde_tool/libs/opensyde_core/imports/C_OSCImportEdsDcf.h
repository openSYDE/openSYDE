//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of file type .eds and .dcf (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIMPORTEDSDCF_H
#define C_OSCIMPORTEDSDCF_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCCanMessage.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCImportEdsDcf
{
public:
   static stw_types::sint32 h_Import(const stw_scl::C_SCLString & orc_FilePath, const stw_types::uint8 ou8_NodeId,
                                     std::vector<C_OSCCanMessage> & orc_OSCRxMessageData,
                                     std::vector<C_OSCNodeDataPoolListElement> & orc_OSCRxSignalData,
                                     std::vector<stw_types::uint8> & orc_RxSignalDefaultMinMaxValuesUsed,
                                     std::vector<C_OSCCanMessage> & orc_OSCTxMessageData,
                                     std::vector<C_OSCNodeDataPoolListElement> & orc_OSCTxSignalData,
                                     std::vector<stw_types::uint8> & orc_TxSignalDefaultMinMaxValuesUsed,
                                     std::vector<std::vector<stw_scl::C_SCLString> > & orc_ImportMessagesPerMessage,
                                     stw_scl::C_SCLString & orc_ParsingError, const bool oq_RestrictForCanOpenUsage,
                                     std::vector<C_OSCCanMessage> & orc_InvalidOSCRxMessageData,
                                     std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCRxSignalData,
                                     std::vector<stw_types::uint8> & orc_InvalidRxSignalDefaultMinMaxValuesUsed,
                                     std::vector<C_OSCCanMessage> & orc_InvalidOSCTxMessageData,
                                     std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCTxSignalData,
                                     std::vector<stw_types::uint8> & orc_InvalidTxSignalDefaultMinMaxValuesUsed,
                                     std::vector<std::vector<stw_scl::C_SCLString> > & orc_InvalidImportMessagesPerMessage);
   static stw_types::sint32 h_ParseSignalContent(const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                 const stw_types::uint32 ou32_COSignalObjectIndex,
                                                 const stw_types::uint32 ou32_COSignalObjectSubIndex,
                                                 const stw_types::uint32 ou32_StartBitCounter,
                                                 const bool oq_RestrictForCanOpenUsage, C_OSCCanSignal & orc_CurSignal,
                                                 C_OSCNodeDataPoolListElement & orc_CurDataPoolSignal,
                                                 bool & orq_DefaultMinMax);
   static stw_scl::C_SCLString h_GetObjectName(const C_OSCCanOpenObject & orc_COObject);

private:
   C_OSCImportEdsDcf(void);

   static const C_OSCCanOpenObject * mh_GetCOObject(const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                    const stw_types::uint32 ou32_Id,
                                                    const stw_types::sint32 os32_SubIndex);
   static stw_types::sint32 mh_ParseMessages(const stw_types::uint32 ou32_StartingId, const stw_types::uint8 ou8_NodeId,
                                             const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                             const std::vector<stw_types::uint32> & orc_Dummies,
                                             std::vector<C_OSCCanMessage> & orc_OSCMessageData,
                                             std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                             std::vector<stw_types::uint8> & orc_SignalDefaultMinMaxValuesUsed,
                                             const bool oq_IsEds,
                                             std::vector<std::vector<stw_scl::C_SCLString> > & orc_ImportMessages,
                                             const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                             std::vector<C_OSCCanMessage> & orc_InvalidOSCMessageData,
                                             std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCSignalData,
                                             std::vector<stw_types::uint8> & orc_InvalidSignalDefaultMinMaxValuesUsed,
                                             std::vector<std::vector<stw_scl::C_SCLString> > & orc_InvalidImportMessages);
   static stw_types::sint32 mh_ParseMessageContent(const stw_types::uint32 ou32_StartingId,
                                                   const stw_types::uint8 ou8_NodeId,
                                                   const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                   const std::vector<stw_types::uint32> & orc_Dummies,
                                                   std::vector<C_OSCCanMessage> & orc_OSCMessageData,
                                                   std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                                   std::vector<stw_types::uint8> & orc_SignalDefaultMinMaxValuesUsed,
                                                   const bool oq_IsEds,
                                                   std::vector<std::vector<stw_scl::C_SCLString> > & orc_ImportMessages,
                                                   const bool oq_IsTx, const bool oq_RestrictForCanOpenUsage,
                                                   const C_OSCCanOpenObject & orc_COMessageMainObject,
                                                   const stw_types::uint32 ou32_ItMessage,
                                                   const stw_types::uint32 ou32_CobId,
                                                   std::vector<C_OSCCanMessage> & orc_InvalidOSCMessageData,
                                                   std::vector<C_OSCNodeDataPoolListElement> & orc_InvalidOSCSignalData,
                                                   std::vector<stw_types::uint8> & orc_InvalidSignalDefaultMinMaxValuesUsed, std::vector<std::vector<stw_scl::C_SCLString> > & orc_InvalidImportMessages, const bool oq_CobIdIncludesNodeId);
   static stw_types::sint32 mh_LoadMessageTransmissionType(const stw_types::uint32 ou32_StartingId,
                                                           const stw_types::uint32 ou32_ItMessage,
                                                           const stw_types::uint8 ou8_NodeId,
                                                           const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects, const bool oq_IsEds, std::vector<stw_scl::C_SCLString> & orc_CurMessages, C_OSCCanMessage & orc_Message);
   static void mh_LoadMessageTransmissionTypeCanOpen(const stw_types::uint32 ou32_StartingId,
                                                     const stw_types::uint32 ou32_ItMessage,
                                                     const stw_types::uint8 ou8_NodeId,
                                                     const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                     const bool oq_IsEds, bool & orq_AddToSkippedMessages,
                                                     std::vector<stw_scl::C_SCLString> & orc_CurMessages,
                                                     C_OSCCanMessage & orc_Message);
   static void mh_LoadEventTimerSection(const stw_types::uint32 ou32_StartingId, const stw_types::uint32 ou32_ItMessage,
                                        const stw_types::uint8 ou8_NodeId,
                                        const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                        const bool oq_IsEds, const bool oq_IsTx,
                                        std::vector<stw_scl::C_SCLString> & orc_CurMessages,
                                        C_OSCCanMessage & orc_Message);
   static void mh_LoadEventTimerSectionCanOpen(const stw_types::uint32 ou32_StartingId,
                                               const stw_types::uint32 ou32_ItMessage,
                                               const stw_types::uint8 ou8_NodeId,
                                               const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                               const bool oq_IsEds, const bool oq_IsTx, C_OSCCanMessage & orc_Message);
   static void mh_LoadInhibitTimeSectionCanOpen(const stw_types::uint32 ou32_StartingId,
                                                const stw_types::uint32 ou32_ItMessage,
                                                const stw_types::uint8 ou8_NodeId,
                                                const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                const bool oq_IsEds,
                                                std::vector<stw_scl::C_SCLString> & orc_CurMessages,
                                                C_OSCCanMessage & orc_Message);
   static stw_types::sint32 mh_ParseSignals(const stw_types::uint32 ou32_COMessageId, const stw_types::uint8 ou8_NodeId,
                                            const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                            const std::vector<stw_types::uint32> & orc_Dummies,
                                            C_OSCCanMessage & orc_OSCMessageData,
                                            std::vector<C_OSCNodeDataPoolListElement> & orc_OSCSignalData,
                                            std::vector<stw_types::uint8> & orc_SignalDefaultMinMaxValuesUsed,
                                            const bool oq_IsEds, const bool oq_RestrictForCanOpenUsage,
                                            std::vector<stw_scl::C_SCLString> & orc_ImportMessages);
   static stw_scl::C_SCLString mh_GetCOObjectValue(const C_OSCCanOpenObject & orc_COObject, const bool oq_IsEds);
   static stw_types::sint32 mh_GetIntegerValue(const stw_scl::C_SCLString & orc_COValue,
                                               const stw_types::uint8 ou8_NodeId, stw_types::uint32 & oru32_Value,
                                               bool * const opq_IncludesNodeId = NULL);
   static stw_types::sint32 mh_GetUnsigned64IntegerValue(const stw_scl::C_SCLString & orc_COValue,
                                                         stw_types::sint32 & ors32_Value);
   static stw_types::sint32 mh_Get64IntegerValue(const stw_scl::C_SCLString & orc_COValue,
                                                 stw_types::sint64 & ors64_Value);
   static stw_types::sint32 mh_GetIntegerValueSimple(const stw_scl::C_SCLString & orc_COValue,
                                                     stw_types::uint32 & oru32_Value);
   static void mh_AddUserMessage(const stw_types::uint32 ou32_COObjectId,
                                 const stw_scl::C_SCLString & orc_COSectionName,
                                 const stw_scl::C_SCLString & orc_ErrorMessage,
                                 const stw_types::sint32 os32_COSubSectionId, const bool oq_IsError,
                                 std::vector<stw_scl::C_SCLString> * const opc_ImportMessages = NULL);
   static stw_scl::C_SCLString mh_GetNumberAsHex(const stw_types::uint32 ou32_Number);
   static stw_types::sint32 mh_CalcMinMaxInit(const C_OSCCanOpenObject * const opc_COSignalObject,
                                              C_OSCNodeDataPoolListElement & orc_Element,
                                              const stw_types::uint16 ou16_NumberBits, bool & orq_DefaultMinMax);
   static void mh_LoadDummies(const stw_scl::C_SCLString & orc_FilePath, std::vector<stw_types::uint32> & orc_Dummies);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
