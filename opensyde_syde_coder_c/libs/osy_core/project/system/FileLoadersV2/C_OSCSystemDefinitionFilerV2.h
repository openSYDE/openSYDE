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
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNode.h"
#include "C_OSCNodeFilerV2.h"
#include "C_OSCSystemBus.h"
#include "C_OSCSystemBusFilerV2.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCSystemDefinitionFilerV2
{
public:
   static stw_types::sint32 h_LoadSystemDefinitionFile(C_OSCSystemDefinition & orc_SystemDefinition,
                                                       const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                                       const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                       const bool oq_UseDeviceDefinitions = true);
   static stw_types::sint32 h_SaveSystemDefinitionFile(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                       const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_LoadSystemDefinitionString(C_OSCSystemDefinition & orc_SystemDefinition,
                                                         const stw_scl::C_SCLString & orc_Content,
                                                         const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                         const bool oq_UseDeviceDefinitions = true);
   static void h_SaveSystemDefinitionString(const C_OSCSystemDefinition & orc_SystemDefinition,
                                            stw_scl::C_SCLString & orc_Content);
   static stw_types::sint32 h_LoadNodes(const stw_types::uint16 ou16_XmlFormatVersion,
                                        std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser,
                                        const C_OSCDeviceManager & orc_DeviceDefinitions,
                                        const bool oq_UseDeviceDefinitions = true);
   static stw_types::sint32 h_LoadBuses(std::vector<C_OSCSystemBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveNodes(const std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser);
   static void h_SaveBuses(const std::vector<C_OSCSystemBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadSystemDefinition(C_OSCSystemDefinition & orc_SystemDefinition,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                   const bool oq_UseDeviceDefinitions = true);
   static void h_SaveSystemDefinition(const C_OSCSystemDefinition & orc_SystemDefinition,
                                      C_OSCXMLParserBase & orc_XMLParser);

   ///known file versions
   static const stw_types::uint16 hu16_FILE_VERSION_1 = 1U;
   static const stw_types::uint16 hu16_FILE_VERSION_2 = 2U;
   static const stw_types::uint16 hu16_FILE_VERSION_LATEST = hu16_FILE_VERSION_2;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
