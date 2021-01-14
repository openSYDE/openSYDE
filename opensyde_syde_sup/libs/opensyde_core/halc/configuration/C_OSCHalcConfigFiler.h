//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC configuration data
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGFILER_H
#define C_OSCHALCCONFIGFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCXMLParser.h"
#include "C_OSCHalcConfig.h"
#include "C_OSCHalcDefStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigFiler
{
public:
   static stw_types::sint32 h_LoadFile(C_OSCHalcConfig & orc_IOData, const stw_scl::C_SCLString & orc_Path,
                                       const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveFile(const C_OSCHalcConfig & orc_IOData, const stw_scl::C_SCLString & orc_Path,
                                       const stw_scl::C_SCLString & orc_BasePath,
                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 h_LoadData(C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                       const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveData(const C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                       const stw_scl::C_SCLString & orc_BasePath,
                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);

   static stw_types::sint32 h_PrepareForFile(const stw_scl::C_SCLString & orc_Path);

   static stw_types::sint32 h_SaveIODomain(const C_OSCHalcConfigDomain & orc_IODomain,
                                           C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_LoadIODomain(C_OSCHalcConfigDomain & orc_IODomain, C_OSCXMLParserBase & orc_XMLParser);

private:
   static const stw_types::uint16 mhu16_FILE_VERSION_1 = 1U;

   C_OSCHalcConfigFiler();

   static stw_types::sint32 mh_SaveIODataBase(const C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_BasePath,
                                              std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 mh_SaveIODomains(const C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOChannels(const std::vector<C_OSCHalcConfigChannel> & orc_Channels,
                                              C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOChannel(const C_OSCHalcConfigChannel & orc_Channel,
                                             C_OSCXMLParserBase & orc_XMLParser,
                                             const stw_scl::C_SCLString & orc_NodeName,
                                             const stw_scl::C_SCLString & orc_NodeParentName);
   static stw_types::sint32 mh_SaveIOParameterStructs(
      const std::vector<C_OSCHalcConfigParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser,
      const stw_scl::C_SCLString & orc_NodeName);
   static stw_types::sint32 mh_SaveIOParameterStruct(const C_OSCHalcConfigParameterStruct & orc_ParameterStruct,
                                                     C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOParameters(const std::vector<C_OSCHalcConfigParameter> & orc_Parameters,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOParameter(const C_OSCHalcConfigParameter & orc_Parameter,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BaseNode);
   static stw_types::sint32 mh_LoadIODataBase(C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_LoadIODomains(C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOChannel(C_OSCHalcConfigChannel & orc_IOChannel, C_OSCXMLParserBase & orc_XMLParser,
                                             const stw_scl::C_SCLString & orc_NodeName);
   static stw_types::sint32 mh_LoadIOParameterStructs(
      std::vector<C_OSCHalcConfigParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser,
      const stw_scl::C_SCLString & orc_NodeName);
   static stw_types::sint32 mh_LoadIOParameterStruct(C_OSCHalcConfigParameterStruct & orc_ParameterStruct,
                                                     C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOParameters(std::vector<C_OSCHalcConfigParameter> & orc_Parameters,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOParameter(C_OSCHalcConfigParameter & orc_Parameter,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BaseName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
