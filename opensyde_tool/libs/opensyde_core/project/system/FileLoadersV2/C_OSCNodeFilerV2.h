//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node reader/writer (V2)

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEFILERV2H
#define C_OSCNODEFILERV2H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNode.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Node reader/writer
class C_OSCNodeFilerV2
{
public:
   static stw_types::sint32 h_LoadNode(const stw_types::uint16 ou16_XmlFormatVersion, C_OSCNode & orc_Node,
                                       C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNode(const C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComProtocols(std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                                   C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComProtocols(const std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                      C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadNodeComProtocol(C_OSCCanProtocol & orc_NodeComProtocol,
                                                  C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodeComProtocol(const C_OSCCanProtocol & orc_NodeComProtocol, C_OSCXMLParserBase & orc_XMLParser);
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

private:
   static stw_types::sint32 mh_LoadProperties(C_OSCNodeProperties & orc_NodeProperties,
                                              C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveProperties(const C_OSCNodeProperties & orc_NodeProperties, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadStwFlashloaderOptions(C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                         C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveStwFlashloaderOptions(const C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                            C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadApplications(std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveApplications(const std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                   C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadDataPools(const stw_types::uint16 ou16_XmlFormatVersion, C_OSCNode & orc_Node,
                                             C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveDataPools(const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_scl::C_SCLString mh_DiagnosticServerToString(
      const C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_DiagnosticProtocol);
   static stw_types::sint32 mh_StringToDiagnosticServer(const stw_scl::C_SCLString & orc_String,
                                                        C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_Type);
   static stw_scl::C_SCLString mh_FlashLoaderToString(
      const C_OSCNodeProperties::E_FlashLoaderProtocol & ore_FlashLoader);
   static stw_types::sint32 mh_StringToFlashLoader(const stw_scl::C_SCLString & orc_String,
                                                   C_OSCNodeProperties::E_FlashLoaderProtocol & ore_Type);
   static stw_scl::C_SCLString mh_CommunicationByteOrderToString(
      const C_OSCCanSignal::E_ByteOrderType & ore_CommunicationByteOrder);
   static stw_types::sint32 mh_StringToCommunicationByteOrder(const stw_scl::C_SCLString & orc_String,
                                                              C_OSCCanSignal::E_ByteOrderType & ore_Type);
   static stw_scl::C_SCLString mh_NodeComMessageTxMethodToString(
      const C_OSCCanMessage::E_TxMethodType & ore_NodeComMessageTxMethod);
   static void mh_StringToNodeComMessageTxMethod(const stw_scl::C_SCLString & orc_String,
                                                 C_OSCCanMessage::E_TxMethodType & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
