//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update package definition filer
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUPNODEDEFINITIONFILER_HPP
#define C_OSCSUPNODEDEFINITIONFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscSupNodeDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSupNodeDefinitionFiler
{
public:
   static int32_t h_SaveNodes(const std::vector<stw::scl::C_SclString> & orc_Files,
                              const std::vector<C_OscSupNodeDefinition> & orc_Nodes);
   static int32_t h_LoadNodes(const std::vector<stw::scl::C_SclString> & orc_Files,
                              const std::vector<stw::scl::C_SclString> & orc_NodeFoldersAbs,
                              const std::vector<uint8_t> & orc_ActiveNodes,
                              std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite, std::map<uint32_t,
                                                                                                           uint32_t> & orc_UpdateOrderByNodes, const std::vector<uint32_t> & orc_UpdatePosition, std::vector<stw::scl::C_SclString> & orc_Signatures);

   static const uint8_t hu8_ACTIVE_NODE;

private:
   static int32_t mh_SaveNode(const stw::scl::C_SclString & orc_File, const C_OscSupNodeDefinition & orc_Node);
   static void mh_LoadFilesSection(std::vector<stw::scl::C_SclString> & orc_Files, const uint32_t ou32_NodeCounter,
                                   const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                           uint32_t> & orc_PositionMap,
                                   const stw::scl::C_SclString & orc_NodeFolderAbs, C_OscXmlParserBase & orc_XmlParser,
                                   const stw::scl::C_SclString & orc_BaseNodeName,
                                   const stw::scl::C_SclString & orc_ElementNodeName);
   static void mh_LoadPemConfigSection(C_OscSuSequences::C_DoFlash & orc_DoFlash, const uint32_t ou32_NodeCounter,
                                       const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                               uint32_t> & orc_PositionMap,
                                       const stw::scl::C_SclString & orc_NodeFolderAbs,
                                       C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveFiles(const std::vector<stw::scl::C_SclString> & orc_Files, C_OscXmlParserBase & orc_XmlParser,
                            const stw::scl::C_SclString & orc_BaseNodeName,
                            const stw::scl::C_SclString & orc_ElementNodeName);
   static void mh_SavePemConfig(const C_OscSupNodeDefinition & orc_CurrentNode, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveSignatureFile(const C_OscSupNodeDefinition & orc_Node, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadSignatureFile(const stw::scl::C_SclString & orc_NodeFolderAbs,
                                       stw::scl::C_SclString & orc_Signature, C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
