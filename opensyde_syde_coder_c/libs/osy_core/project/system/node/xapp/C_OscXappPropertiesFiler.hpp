//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       X-app properties filer
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXAPPPROPERTIESFILER_HPP
#define C_OSCXAPPPROPERTIESFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscXappProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXappPropertiesFiler
{
public:
   static int32_t h_LoadXappPropertiesFile(C_OscXappProperties & orc_XappProperties,
                                           const stw::scl::C_SclString & orc_FilePath);
   static int32_t h_LoadXappProperties(C_OscXappProperties & orc_XappProperties, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_SaveXappPropertiesFile(const C_OscXappProperties & orc_XappProperties,
                                           const stw::scl::C_SclString & orc_FilePath);
   static void h_SaveXappProperties(const C_OscXappProperties & orc_XappProperties, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadCommInterfaceId(C_OscSystemBus::E_Type & ore_Type, uint8_t & oru8_InterfaceNumber,
                                        C_OscXmlParserBase & orc_XmlParser,
                                        const stw::scl::C_SclString & orc_ParentNodeName,
                                        const stw::scl::C_SclString & orc_UseCase);
   static void h_SaveCommInterfaceId(const C_OscSystemBus::E_Type oe_Type, const uint8_t ou8_InterfaceNumber,
                                     C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString h_GetFileName(void);

private:
   C_OscXappPropertiesFiler();

   static const uint16_t mhu16_FILE_VERSION_1;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
