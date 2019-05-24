//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for comm description files (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODECOMMFILER_H
#define C_OSCNODECOMMFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCXMLParser.h"
#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeCommFiler
{
public:
   static stw_types::sint32 h_LoadNodeComProtocolFile(C_OSCCanProtocol & orc_NodeComProtocol,
                                                      const stw_scl::C_SCLString & orc_FilePath,
                                                      const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools);
   static stw_types::sint32 h_LoadNodeComProtocol(C_OSCCanProtocol & orc_NodeComProtocol,
                                                  C_OSCXMLParserBase & orc_XMLParser,
                                                  const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools);
   static stw_types::sint32 h_SaveNodeComProtocolFile(const C_OSCCanProtocol & orc_NodeComProtocol,
                                                      const stw_scl::C_SCLString & orc_FilePath,
                                                      const stw_scl::C_SCLString & orc_DatapoolName);
   static void h_SaveNodeComProtocol(const C_OSCCanProtocol & orc_NodeComProtocol, C_OSCXMLParserBase & orc_XMLParser,
                                     const stw_scl::C_SCLString & orc_DatapoolName);
   static stw_types::sint32 h_LoadNodeComMessageContainers(
      std::vector<C_OSCCanMessageContainer> & orc_NodeComMessageContainers, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComMessageContainers(
      const std::vector<C_OSCCanMessageContainer> & orc_NodeComMessageContainers,
      C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComMessageContainer(C_OSCCanMessageContainer & orc_NodeComMessageContainer,
                                                          C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComMessageContainer(const C_OSCCanMessageContainer & orc_NodeComMessageContainer,
                                             C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComMessages(std::vector<C_OSCCanMessage> & orc_NodeComMessages,
                                                  C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComMessages(const std::vector<C_OSCCanMessage> & orc_NodeComMessages,
                                     C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComMessage(C_OSCCanMessage & orc_NodeComMessage,
                                                 C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComMessage(const C_OSCCanMessage & orc_NodeComMessage, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComSignals(std::vector<C_OSCCanSignal> & orc_NodeComSignals,
                                                 C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComSignals(const std::vector<C_OSCCanSignal> & orc_NodeComSignals,
                                    C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComSignal(C_OSCCanSignal & orc_NodeComSignal, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComSignal(const C_OSCCanSignal & orc_NodeComSignal, C_OSCXMLParserBase & orc_XMLParser);
   static stw_scl::C_SCLString h_CommunicationProtocolToString(
      const C_OSCCanProtocol::E_Type & ore_CommunicationProtocol);
   static stw_types::sint32 h_StringToCommunicationProtocol(const stw_scl::C_SCLString & orc_String,
                                                            C_OSCCanProtocol::E_Type & ore_Type);
   static stw_scl::C_SCLString h_GetFileName(const stw_scl::C_SCLString & orc_DatapoolName);

private:
   static stw_scl::C_SCLString mh_CommunicationByteOrderToString(
      const C_OSCCanSignal::E_ByteOrderType & ore_CommunicationByteOrder);
   static stw_types::sint32 mh_StringToCommunicationByteOrder(const stw_scl::C_SCLString & orc_String,
                                                              C_OSCCanSignal::E_ByteOrderType & ore_Type);
   static stw_scl::C_SCLString mh_NodeComMessageTxMethodToString(
      const C_OSCCanMessage::E_TxMethodType & ore_NodeComMessageTxMethod);
   static stw_types::sint32 mh_StringToNodeComMessageTxMethod(const stw_scl::C_SCLString & orc_String,
                                                              C_OSCCanMessage::E_TxMethodType & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
