//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC configuration data
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGFILER_HPP
#define C_OSCHALCCONFIGFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscHalcConfig.hpp"
#include "C_OscHalcDefStruct.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigFiler
{
public:
   static int32_t h_LoadFile(C_OscHalcConfig & orc_IoData, const stw::scl::C_SclString & orc_Path,
                             const stw::scl::C_SclString & orc_BasePath);
   static int32_t h_SaveFile(const C_OscHalcConfig & orc_IoData, const stw::scl::C_SclString & orc_Path,
                             const stw::scl::C_SclString & orc_BasePath,
                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);
   static int32_t h_LoadData(C_OscHalcConfig & orc_IoData, C_OscXmlParserBase & orc_XmlParser,
                             const stw::scl::C_SclString & orc_BasePath);
   static int32_t h_SaveData(const C_OscHalcConfig & orc_IoData, C_OscXmlParserBase & orc_XmlParser,
                             const stw::scl::C_SclString & orc_BasePath,
                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);

   static int32_t h_PrepareForFile(const stw::scl::C_SclString & orc_Path);

   static int32_t h_SaveIoDomain(const C_OscHalcConfigDomain & orc_IoDomain, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadIoDomain(C_OscHalcConfigDomain & orc_IoDomain, C_OscXmlParserBase & orc_XmlParser);

private:
   static const uint16_t mhu16_FILE_VERSION_1 = 1U;

   C_OscHalcConfigFiler();

   static int32_t mh_SaveIoDataBase(const C_OscHalcDefBase & orc_IoData, C_OscXmlParserBase & orc_XmlParser,
                                    const stw::scl::C_SclString & orc_BasePath,
                                    std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);
   static int32_t mh_SaveIoDomains(const C_OscHalcConfig & orc_IoData, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_SaveIoChannels(const std::vector<C_OscHalcConfigChannel> & orc_Channels,
                                    C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_SaveIoChannel(const C_OscHalcConfigChannel & orc_Channel, C_OscXmlParserBase & orc_XmlParser,
                                   const stw::scl::C_SclString & orc_NodeName,
                                   const stw::scl::C_SclString & orc_NodeParentName);
   static int32_t mh_SaveIoParameterStructs(const std::vector<C_OscHalcConfigParameterStruct> & orc_ParameterStructs,
                                            C_OscXmlParserBase & orc_XmlParser,
                                            const stw::scl::C_SclString & orc_NodeName);
   static int32_t mh_SaveIoParameterStruct(const C_OscHalcConfigParameterStruct & orc_ParameterStruct,
                                           C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_SaveIoParameters(const std::vector<C_OscHalcConfigParameter> & orc_Parameters,
                                      C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_SaveIoParameter(const C_OscHalcConfigParameter & orc_Parameter, C_OscXmlParserBase & orc_XmlParser,
                                     const stw::scl::C_SclString & orc_BaseNode);
   static int32_t mh_LoadIoDataBase(C_OscHalcDefBase & orc_IoData, C_OscXmlParserBase & orc_XmlParser,
                                    const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_LoadIoDomains(C_OscHalcConfig & orc_IoData, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadIoChannel(C_OscHalcConfigChannel & orc_IoChannel, C_OscXmlParserBase & orc_XmlParser,
                                   const stw::scl::C_SclString & orc_NodeName);
   static int32_t mh_LoadIoParameterStructs(std::vector<C_OscHalcConfigParameterStruct> & orc_ParameterStructs,
                                            C_OscXmlParserBase & orc_XmlParser,
                                            const stw::scl::C_SclString & orc_NodeName);
   static int32_t mh_LoadIoParameterStruct(C_OscHalcConfigParameterStruct & orc_ParameterStruct,
                                           C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadIoParameters(std::vector<C_OscHalcConfigParameter> & orc_Parameters,
                                      C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadIoParameter(C_OscHalcConfigParameter & orc_Parameter, C_OscXmlParserBase & orc_XmlParser,
                                     const stw::scl::C_SclString & orc_BaseName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
