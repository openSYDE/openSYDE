//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for manifest data
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCEMANIFESTFILER_HPP
#define C_OSCXCEMANIFESTFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscXceManifest.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXceManifestFiler
{
public:
   static int32_t h_LoadFile(C_OscXceManifest & orc_Config, const stw::scl::C_SclString & orc_Path);
   static int32_t h_SaveFile(const C_OscXceManifest & orc_Config, const stw::scl::C_SclString & orc_Path);
   static int32_t h_LoadData(C_OscXceManifest & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveData(const C_OscXceManifest & orc_Config, C_OscXmlParserBase & orc_XmlParser);

   static const stw::scl::C_SclString hc_FILE_NAME;

private:
   static const uint16_t mhu16_FILE_VERSION_1;
   static const uint16_t mhu16_PACKAGE_VERSION_1;

   C_OscXceManifestFiler();
   static int32_t mh_LoadUpdatePackageParameters(std::vector<C_OscXceUpdatePackageParameters> & orc_Config,
                                                 C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveUpdatePackageParameters(const std::vector<C_OscXceUpdatePackageParameters> & orc_Config,
                                              C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadUpdatePackageParameter(C_OscXceUpdatePackageParameters & orc_Config,
                                                const C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveUpdatePackageParameter(const C_OscXceUpdatePackageParameters & orc_Config,
                                             C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
