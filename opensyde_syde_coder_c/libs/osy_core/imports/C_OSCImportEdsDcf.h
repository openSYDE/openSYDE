//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of file type .eds and .dcf (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.04.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCIMPORTEDSDCF_H
#define C_OSCIMPORTEDSDCF_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCCanMessage.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCImportEdsDcf
{
public:
   static stw_types::sint32 h_Import(const stw_scl::C_SCLString & orc_FilePath, const stw_types::uint8 ou8_NodeId,
                                     std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCRxMessageData,
                                     std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCRxSignalData,
                                     std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCTxMessageData,
                                     std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCTxSignalData,
                                     std::vector<std::vector<stw_scl::C_SCLString> > & orc_ImportMessagesPerMessage,
                                     stw_scl::C_SCLString & orc_ParsingError);

private:
   C_OSCImportEdsDcf(void);

   static const C_OSCCanOpenObject * mh_GetCOObject(const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                                    const stw_types::uint32 ou32_Id,
                                                    const stw_types::sint32 os32_SubIndex);
   static stw_types::sint32 mh_ParseMessages(const stw_types::uint32 ou32_StartingId, const stw_types::uint8 ou8_NodeId,
                                             const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                             const std::vector<stw_types::uint32> & orc_Dummies,
                                             std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_OSCMessageData,
                                             std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalData, const bool oq_IsEds, std::vector<std::vector<stw_scl::C_SCLString> > & orc_ImportMessages);
   static stw_types::sint32 mh_ParseSignals(const stw_types::uint32 ou32_COMessageId, const stw_types::uint8 ou8_NodeId,
                                            const stw_scl::SCLDynamicArray<C_OSCCanOpenObject> & orc_COObjects,
                                            const std::vector<stw_types::uint32> & orc_Dummies,
                                            stw_opensyde_core::C_OSCCanMessage & orc_OSCMessageData,
                                            std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalData, const bool oq_IsEds);
   static stw_scl::C_SCLString mh_GetCOObjectValue(const C_OSCCanOpenObject & orc_COObject, const bool oq_IsEds);
   static stw_types::sint32 mh_GetIntegerValue(const stw_scl::C_SCLString & orc_COValue,
                                               const stw_types::uint8 ou8_NodeId, stw_types::uint32 & oru32_Value);
   static stw_types::sint32 mh_GetIntegerValueSimple(const stw_scl::C_SCLString & orc_COValue,
                                                     stw_types::uint32 & oru32_Value);
   static void mh_AddUserMessage(const stw_types::uint32 ou32_COObjectId,
                                 const stw_scl::C_SCLString & orc_COSectionName,
                                 const stw_scl::C_SCLString & orc_ErrorMessage,
                                 const stw_types::sint32 os32_COSubSectionId, const bool oq_IsError,
                                 std::vector<stw_scl::C_SCLString> * const opc_ImportMessages = NULL);
   static stw_types::sint32 mh_CalcMinMax(C_OSCNodeDataPoolListElement & orc_Element,
                                          const stw_types::uint16 ou16_NumberBits);
   static void mh_LoadDummies(const stw_scl::C_SCLString & orc_FilePath, std::vector<stw_types::uint32> & orc_Dummies);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
