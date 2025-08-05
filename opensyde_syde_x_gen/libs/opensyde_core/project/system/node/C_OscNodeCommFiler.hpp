//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for comm description files (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODECOMMFILER_HPP
#define C_OSCNODECOMMFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeCommFiler
{
public:
   static int32_t h_LoadNodeComProtocolFile(C_OscCanProtocol & orc_NodeComProtocol,
                                            const stw::scl::C_SclString & orc_FilePath,
                                            const std::vector<C_OscNodeDataPool> & orc_NodeDataPools);
   static int32_t h_LoadNodeComProtocol(C_OscCanProtocol & orc_NodeComProtocol, C_OscXmlParserBase & orc_XmlParser,
                                        const std::vector<C_OscNodeDataPool> & orc_NodeDataPools);
   static int32_t h_SaveNodeComProtocolFile(const C_OscCanProtocol & orc_NodeComProtocol,
                                            const stw::scl::C_SclString & orc_FilePath,
                                            const stw::scl::C_SclString & orc_DatapoolName);
   static void h_SaveNodeComProtocol(const C_OscCanProtocol & orc_NodeComProtocol, C_OscXmlParserBase & orc_XmlParser,
                                     const stw::scl::C_SclString & orc_DatapoolName);
   static int32_t h_LoadNodeComMessageContainers(std::vector<C_OscCanMessageContainer> & orc_NodeComMessageContainers,
                                                 C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessageContainers(
      const std::vector<C_OscCanMessageContainer> & orc_NodeComMessageContainers,
      C_OscXmlParserBase & orc_XmlParser, const C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadNodeComMessageContainer(C_OscCanMessageContainer & orc_NodeComMessageContainer,
                                                C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessageContainer(const C_OscCanMessageContainer & orc_NodeComMessageContainer,
                                             C_OscXmlParserBase & orc_XmlParser,
                                             const C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadNodeComMessages(std::vector<C_OscCanMessage> & orc_NodeComMessages,
                                        C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessages(const std::vector<C_OscCanMessage> & orc_NodeComMessages,
                                     C_OscXmlParserBase & orc_XmlParser,
                                     const C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadNodeComMessage(C_OscCanMessage & orc_NodeComMessage, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComMessage(const C_OscCanMessage & orc_NodeComMessage, C_OscXmlParserBase & orc_XmlParser,
                                    const C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadNodeComSignals(std::vector<C_OscCanSignal> & orc_NodeComSignals,
                                       C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeComSignals(const std::vector<C_OscCanSignal> & orc_NodeComSignals,
                                    C_OscXmlParserBase & orc_XmlParser, const C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadNodeComSignal(C_OscCanSignal & orc_NodeComSignal, C_OscXmlParserBase & orc_XmlParser,
                                      const bool oq_CanOpenOnly = false);
   static void h_SaveNodeComSignal(const C_OscCanSignal & orc_NodeComSignal, C_OscXmlParserBase & orc_XmlParser,
                                   const C_OscCanProtocol::E_Type oe_ProtocolType);
   static stw::scl::C_SclString h_CommunicationProtocolToString(
      const C_OscCanProtocol::E_Type & ore_CommunicationProtocol);
   static int32_t h_StringToCommunicationProtocol(const stw::scl::C_SclString & orc_String,
                                                  C_OscCanProtocol::E_Type & ore_Type);
   static stw::scl::C_SclString h_GetFileName(const stw::scl::C_SclString & orc_DatapoolName);
   static void h_SaveNodeOwnerIndex(const C_OscCanInterfaceId & orc_OwnerNodeIndex, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadNodeOwnerIndex(C_OscCanInterfaceId & orc_OwnerNodeIndex,
                                       const C_OscXmlParserBase & orc_XmlParser);

private:
   static void mh_SaveMessageCanOpenPart(const C_OscCanMessage & orc_NodeCommMessage,
                                         C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadMessageCanOpenPart(C_OscCanMessage & orc_NodeCommMessage, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeOwnerIndices(const C_OscCanInterfaceId & orc_OwnerNodeIndex,
                                       C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeOwnerIndices(C_OscCanInterfaceId & orc_OwnerNodeIndex, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveSignalCanOpenPart(const C_OscCanSignal & orc_NodeCommSignal, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadSignalCanOpenPart(C_OscCanSignal & orc_NodeCommSignal, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveSignalJ1939Part(const C_OscCanSignal & orc_NodeCommSignal, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadSignalJ1939Part(C_OscCanSignal & orc_NodeCommSignal, C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString mh_CommunicationByteOrderToString(
      const C_OscCanSignal::E_ByteOrderType & ore_CommunicationByteOrder);
   static int32_t mh_StringToCommunicationByteOrder(const stw::scl::C_SclString & orc_String,
                                                    C_OscCanSignal::E_ByteOrderType & ore_Type);
   static stw::scl::C_SclString mh_CommunicationMuxTypeToString(
      const C_OscCanSignal::E_MultiplexerType & ore_CommunicationByteOrder);
   static int32_t mh_StringToCommunicationMuxType(const stw::scl::C_SclString & orc_String,
                                                  C_OscCanSignal::E_MultiplexerType & ore_Type);
   static stw::scl::C_SclString mh_NodeComMessageTxMethodToString(
      const C_OscCanMessage::E_TxMethodType & ore_NodeComMessageTxMethod);
   static void mh_StringToNodeComMessageTxMethod(const stw::scl::C_SclString & orc_String,
                                                 C_OscCanMessage::E_TxMethodType & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
