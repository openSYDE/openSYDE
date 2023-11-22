//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Update package configuration file reader/writer (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACCONFIGFILER_HPP
#define C_SYVUPPACCONFIGFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_SyvUpPacConfig.hpp"

#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacConfigFiler
{
public:
   static int32_t h_LoadConfig(const QString & orc_FilePath, C_SyvUpPacConfig & orc_Config);
   static int32_t h_SaveConfig(const QString & orc_FilePath, const C_SyvUpPacConfig & orc_Config);

   ///known file versions
   static const uint16_t hu16_FILE_VERSION_1 = 1U;
   static const uint16_t hu16_FILE_VERSION_LATEST = hu16_FILE_VERSION_1;

private:
   C_SyvUpPacConfigFiler(void);

   static bool mh_SaveNode(stw::opensyde_core::C_OscXmlParser & orc_XmlParser,
                           const C_SyvUpPacConfigNode & orc_NodeConfig);
   static bool mh_SaveApp(stw::opensyde_core::C_OscXmlParser & orc_XmlParser,
                          const C_SyvUpPacConfigNodeApp & orc_AppConfig);
   static bool mh_SaveParamSet(stw::opensyde_core::C_OscXmlParser & orc_XmlParser, const QString & orc_Path);
   static bool mh_SaveFile(stw::opensyde_core::C_OscXmlParser & orc_XmlParser, const QString & orc_Path);
   static void mh_SaveNodeUpdateInformationPem(const C_SyvUpPacConfigNode & orc_NodeConfig,
                                               stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);

   static bool mh_LoadNodes(stw::opensyde_core::C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfig & orc_Config);
   static bool mh_LoadApps(stw::opensyde_core::C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfigNode & orc_NodeConfig);
   static bool mh_LoadParamSets(stw::opensyde_core::C_OscXmlParser & orc_XmlParser,
                                C_SyvUpPacConfigNode & orc_NodeConfig);
   static bool mh_LoadFiles(stw::opensyde_core::C_OscXmlParser & orc_XmlParser, C_SyvUpPacConfigNode & orc_NodeConfig);
   static int32_t mh_LoadNodeUpdateInformationPem(C_SyvUpPacConfigNode & orc_NodeConfig,
                                                  stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformationPemStates(C_SyvUpPacConfigNode & orc_NodeConfig,
                                                        stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
