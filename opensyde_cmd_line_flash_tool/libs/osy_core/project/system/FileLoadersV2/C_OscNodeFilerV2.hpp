//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node reader/writer (V2)

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEFILERV2_HPP
#define C_OSCNODEFILERV2_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNode.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Node reader/writer
class C_OscNodeFilerV2
{
public:
   static int32_t h_LoadNode(const uint16_t ou16_XmlFormatVersion, C_OscNode & orc_Node,
                             C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNode(const C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComProtocols(std::vector<C_OscCanProtocol> & orc_NodeComProtocols,
                                         C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComProtocols(const std::vector<C_OscCanProtocol> & orc_NodeComProtocols,
                                      C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComProtocol(C_OscCanProtocol & orc_NodeComProtocol, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComProtocol(const C_OscCanProtocol & orc_NodeComProtocol, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComMessageContainers(std::vector<C_OscCanMessageContainer> & orc_NodeComMessageContainers,
                                                 C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessageContainers(
      const std::vector<C_OscCanMessageContainer> & orc_NodeComMessageContainers,
      C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComMessageContainer(C_OscCanMessageContainer & orc_NodeComMessageContainer,
                                                C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessageContainer(const C_OscCanMessageContainer & orc_NodeComMessageContainer,
                                             C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComMessages(std::vector<C_OscCanMessage> & orc_NodeComMessages,
                                        C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessages(const std::vector<C_OscCanMessage> & orc_NodeComMessages,
                                     C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComMessage(C_OscCanMessage & orc_NodeComMessage, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessage(const C_OscCanMessage & orc_NodeComMessage, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComSignals(std::vector<C_OscCanSignal> & orc_NodeComSignals,
                                       C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComSignals(const std::vector<C_OscCanSignal> & orc_NodeComSignals,
                                    C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeComSignal(C_OscCanSignal & orc_NodeComSignal, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComSignal(const C_OscCanSignal & orc_NodeComSignal, C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString h_CommunicationProtocolToString(
      const C_OscCanProtocol::E_Type & ore_CommunicationProtocol);
   static int32_t h_StringToCommunicationProtocol(const stw::scl::C_SclString & orc_String,
                                                  C_OscCanProtocol::E_Type & ore_Type);

private:
   static int32_t mh_LoadProperties(C_OscNodeProperties & orc_NodeProperties, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveProperties(const C_OscNodeProperties & orc_NodeProperties, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadStwFlashloaderOptions(C_OscNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                               C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveStwFlashloaderOptions(const C_OscNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                            C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadApplications(std::vector<C_OscNodeApplication> & orc_NodeApplications,
                                      C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveApplications(const std::vector<C_OscNodeApplication> & orc_NodeApplications,
                                   C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataPools(const uint16_t ou16_XmlFormatVersion, C_OscNode & orc_Node,
                                   C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveDataPools(const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString mh_DiagnosticServerToString(
      const C_OscNodeProperties::E_DiagnosticServerProtocol & ore_DiagnosticProtocol);
   static int32_t mh_StringToDiagnosticServer(const stw::scl::C_SclString & orc_String,
                                              C_OscNodeProperties::E_DiagnosticServerProtocol & ore_Type);
   static stw::scl::C_SclString mh_FlashLoaderToString(
      const C_OscNodeProperties::E_FlashLoaderProtocol & ore_FlashLoader);
   static int32_t mh_StringToFlashLoader(const stw::scl::C_SclString & orc_String,
                                         C_OscNodeProperties::E_FlashLoaderProtocol & ore_Type);
   static stw::scl::C_SclString mh_CommunicationByteOrderToString(
      const C_OscCanSignal::E_ByteOrderType & ore_CommunicationByteOrder);
   static int32_t mh_StringToCommunicationByteOrder(const stw::scl::C_SclString & orc_String,
                                                    C_OscCanSignal::E_ByteOrderType & ore_Type);
   static stw::scl::C_SclString mh_NodeComMessageTxMethodToString(
      const C_OscCanMessage::E_TxMethodType & ore_NodeComMessageTxMethod);
   static void mh_StringToNodeComMessageTxMethod(const stw::scl::C_SclString & orc_String,
                                                 C_OscCanMessage::E_TxMethodType & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
