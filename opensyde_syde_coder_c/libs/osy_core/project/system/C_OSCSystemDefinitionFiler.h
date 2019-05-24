//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load/save system definition data from/to file (V3)

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMDEFINITIONFILER_H
#define C_OSCSYSTEMDEFINITIONFILER_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNode.h"
#include "C_OSCNodeFiler.h"
#include "C_OSCSystemBus.h"
#include "C_OSCSystemBusFiler.h"
#include "C_OSCSystemDefinition.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCSystemDefinitionFiler
{
public:
   static stw_types::sint32 h_LoadSystemDefinitionFile(C_OSCSystemDefinition & orc_SystemDefinition,
                                                       const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                                       const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                       const bool oq_UseDeviceDefinitions = true,
                                                       stw_types::uint16 * const opu16_ReadFileVersion = NULL);
   static stw_types::sint32 h_SaveSystemDefinitionFile(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                       const stw_scl::C_SCLString & orc_Path,
                                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles = NULL);
   static stw_types::sint32 h_LoadNodes(std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser,
                                        const C_OSCDeviceManager & orc_DeviceDefinitions,
                                        const stw_scl::C_SCLString & orc_BasePath,
                                        const bool oq_UseDeviceDefinitions = true,
                                        const bool oq_UseFileInterface = true);
   static stw_types::sint32 h_LoadBuses(std::vector<C_OSCSystemBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_SaveNodes(const std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser,
                                        const stw_scl::C_SCLString & orc_BasePath,
                                        std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static void h_SaveBuses(const std::vector<C_OSCSystemBus> & orc_Buses, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadSystemDefinition(C_OSCSystemDefinition & orc_SystemDefinition,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                   const stw_scl::C_SCLString & orc_BasePath,
                                                   const bool oq_UseDeviceDefinitions = true,
                                                   stw_types::uint16 * const opu16_ReadFileVersion = NULL);
   static stw_types::sint32 h_SaveSystemDefinition(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_scl::C_SCLString & orc_BasePath,
                                                   std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);

   ///known file versions
   static const stw_types::uint16 hu16_FILE_VERSION_1 = 1U;
   static const stw_types::uint16 hu16_FILE_VERSION_2 = 2U;
   static const stw_types::uint16 hu16_FILE_VERSION_3 = 3U;
   static const stw_types::uint16 hu16_FILE_VERSION_LATEST = hu16_FILE_VERSION_3;
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
