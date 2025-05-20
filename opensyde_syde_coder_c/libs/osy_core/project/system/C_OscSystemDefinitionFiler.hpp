//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load/save system definition data from/to file (V3)

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMDEFINITIONFILER_HPP
#define C_OSCSYSTEMDEFINITIONFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscNode.hpp"
#include "C_OscNodeFiler.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscSystemBusFiler.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemDefinitionFiler
{
public:
   static int32_t h_LoadSystemDefinitionFile(C_OscSystemDefinition & orc_SystemDefinition,
                                             const stw::scl::C_SclString & orc_PathSystemDefinition,
                                             const stw::scl::C_SclString & orc_PathDeviceDefinitions,
                                             const bool oq_UseDeviceDefinitions = true,
                                             uint16_t * const opu16_ReadFileVersion = NULL,
                                             const std::vector<uint8_t> * const opc_NodesToLoad = NULL,
                                             const bool oq_SkipContent = false,
                                             const stw::scl::C_SclString * const opc_ExpectedNodeName = NULL);
   static int32_t h_SaveSystemDefinitionFile(const C_OscSystemDefinition & orc_SystemDefinition,
                                             const stw::scl::C_SclString & orc_Path,
                                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles = NULL);
   static int32_t h_LoadNodes(std::vector<C_OscNode> & orc_Nodes, C_OscXmlParserBase & orc_XmlParser,
                              const C_OscDeviceManager & orc_DeviceDefinitions,
                              const stw::scl::C_SclString & orc_BasePath, const bool oq_UseDeviceDefinitions = true,
                              const bool oq_UseFileInterface = true,
                              const std::vector<uint8_t> * const opc_NodesToLoad = NULL,
                              const bool oq_SkipContent = false,
                              const stw::scl::C_SclString * const opc_ExpectedNodeName = NULL);
   static int32_t h_LoadBuses(std::vector<C_OscSystemBus> & orc_Buses, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_SaveNodes(const std::vector<C_OscNode> & orc_Nodes, C_OscXmlParserBase & orc_XmlParser,
                              const stw::scl::C_SclString & orc_BasePath,
                              std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);
   static void h_SaveBuses(const std::vector<C_OscSystemBus> & orc_Buses, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadSystemDefinition(C_OscSystemDefinition & orc_SystemDefinition,
                                         C_OscXmlParserBase & orc_XmlParser,
                                         const stw::scl::C_SclString & orc_PathDeviceDefinitions,
                                         const stw::scl::C_SclString & orc_BasePath,
                                         const bool oq_UseDeviceDefinitions = true,
                                         uint16_t * const opu16_ReadFileVersion = NULL,
                                         const std::vector<uint8_t> * const opc_NodesToLoad = NULL,
                                         const bool oq_SkipContent = false,
                                         const stw::scl::C_SclString * const opc_ExpectedNodeName = NULL);
   static int32_t h_SaveSystemDefinition(const C_OscSystemDefinition & orc_SystemDefinition,
                                         C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath,
                                         std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);

   static void h_SplitDeviceType(const stw::scl::C_SclString & orc_CompleteType, stw::scl::C_SclString & orc_MainType,
                                 stw::scl::C_SclString & orc_SubType);

   ///known file versions
   static const uint16_t hu16_FILE_VERSION_1 = 1U;
   static const uint16_t hu16_FILE_VERSION_2 = 2U;
   static const uint16_t hu16_FILE_VERSION_3 = 3U;
   static const uint16_t hu16_FILE_VERSION_LATEST = hu16_FILE_VERSION_3;

private:
   static std::map<uint32_t, stw::scl::C_SclString> mh_MapNodeIndicesToName(const std::vector<C_OscNode> & orc_Nodes);
   static int32_t mh_LoadSystemDefinitionProperties(C_OscSystemDefinition & orc_SystemDefinition,
                                                    C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveSystemDefinitionProperties(const C_OscSystemDefinition & orc_SystemDefinition,
                                                 C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
