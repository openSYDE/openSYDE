//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load/save system definition data from/to file (V2)

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMDEFINITIONFILERV2H
#define C_OSCSYSTEMDEFINITIONFILERV2H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscNode.hpp"
#include "C_OscNodeFilerV2.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscSystemBusFilerV2.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemDefinitionFilerV2
{
public:
   static int32_t h_LoadSystemDefinitionFile(C_OscSystemDefinition & orc_SystemDefinition,
                                             const stw::scl::C_SclString & orc_PathSystemDefinition,
                                             const stw::scl::C_SclString & orc_PathDeviceDefinitions,
                                             const bool oq_UseDeviceDefinitions = true);
   static int32_t h_SaveSystemDefinitionFile(const C_OscSystemDefinition & orc_SystemDefinition,
                                             const stw::scl::C_SclString & orc_Path);
   static int32_t h_LoadSystemDefinitionString(C_OscSystemDefinition & orc_SystemDefinition,
                                               const stw::scl::C_SclString & orc_Content,
                                               const stw::scl::C_SclString & orc_PathDeviceDefinitions,
                                               const bool oq_UseDeviceDefinitions = true);
   static void h_SaveSystemDefinitionString(const C_OscSystemDefinition & orc_SystemDefinition,
                                            stw::scl::C_SclString & orc_Content);
   static int32_t h_LoadNodes(const uint16_t ou16_XmlFormatVersion, std::vector<C_OscNode> & orc_Nodes,
                              C_OscXmlParserBase & orc_XmlParser, const C_OscDeviceManager & orc_DeviceDefinitions,
                              const bool oq_UseDeviceDefinitions = true);
   static int32_t h_LoadBuses(std::vector<C_OscSystemBus> & orc_Buses, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodes(const std::vector<C_OscNode> & orc_Nodes, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveBuses(const std::vector<C_OscSystemBus> & orc_Buses, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadSystemDefinition(C_OscSystemDefinition & orc_SystemDefinition,
                                         C_OscXmlParserBase & orc_XmlParser,
                                         const stw::scl::C_SclString & orc_PathDeviceDefinitions,
                                         const bool oq_UseDeviceDefinitions = true);
   static void h_SaveSystemDefinition(const C_OscSystemDefinition & orc_SystemDefinition,
                                      C_OscXmlParserBase & orc_XmlParser);

   ///known file versions
   static const uint16_t hu16_FILE_VERSION_1 = 1U;
   static const uint16_t hu16_FILE_VERSION_2 = 2U;
   static const uint16_t hu16_FILE_VERSION_LATEST = hu16_FILE_VERSION_2;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
