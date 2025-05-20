//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for manifest data
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCOMANIFESTFILER_HPP
#define C_OSCXCOMANIFESTFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscXcoManifest.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXcoManifestFiler
{
public:
   static int32_t h_LoadFile(C_OscXcoManifest & orc_Config, const stw::scl::C_SclString & orc_Path);
   static int32_t h_SaveFile(const C_OscXcoManifest & orc_Config, const stw::scl::C_SclString & orc_Path);
   static int32_t h_LoadData(C_OscXcoManifest & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveData(const C_OscXcoManifest & orc_Config, C_OscXmlParserBase & orc_XmlParser);

   static const stw::scl::C_SclString hc_FILE_NAME;

private:
   C_OscXcoManifestFiler();

   static const uint16_t mhu16_FILE_VERSION_1;
   static const uint16_t mhu16_PACKAGE_VERSION_1;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
