//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load IO config data
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOCONFIGDATAFILER_H
#define C_OSCIOCONFIGDATAFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCXMLParser.h"
#include "C_OSCIOConfigData.h"
#include "C_OSCIOFileDataStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOConfigDataFiler
{
public:
   static stw_types::sint32 h_LoadIODataFile(C_OSCIOConfigData & orc_IOData, const stw_scl::C_SCLString & orc_Path,
                                             const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveIODataFile(const C_OSCIOConfigData & orc_IOData,
                                             const stw_scl::C_SCLString & orc_Path,
                                             const stw_scl::C_SCLString & orc_BasePath,
                                             std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);

private:
   static const stw_types::uint16 hu16_FILE_VERSION_1 = 1U;

   C_OSCIOConfigDataFiler();

   static stw_types::sint32 mh_SaveIOData(const C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                          const stw_scl::C_SCLString & orc_BasePath,
                                          std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 mh_SaveIODataBase(const C_OSCIOFileDataBase & orc_IOData,
                                              C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_BasePath,
                                              std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 mh_SaveIODomains(const C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOChannels(const std::vector<C_OSCIOConfigDataChannel> & orc_Channels,
                                              C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOChannel(const C_OSCIOConfigDataChannel & orc_Channel,
                                             C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOParameterStructs(
      const std::vector<C_OSCIOConfigDataParameterStruct> & orc_ParameterStructs,
      C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOParameterStruct(const C_OSCIOConfigDataParameterStruct & orc_ParameterStruct,
                                                     C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOParameters(const std::vector<C_OSCIOConfigDataParameter> & orc_Parameters,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_SaveIOParameter(const C_OSCIOConfigDataParameter & orc_Parameter,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BaseNode);
   static stw_scl::C_SCLString mh_GetStringForDbConfig(const C_OSCIOConfigDataChannel::E_DataBlockConfig oe_Config);
   static stw_types::sint32 mh_GetDbConfigForString(const stw_scl::C_SCLString & orc_Str,
                                                    C_OSCIOConfigDataChannel::E_DataBlockConfig & ore_Config);
   static stw_types::sint32 mh_LoadIOData(C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                          const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_LoadIODataBase(C_OSCIOFileDataBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_LoadIODomains(C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIODomain(C_OSCIOConfigDataDomain & orc_IODomain, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOChannel(C_OSCIOConfigDataChannel & orc_IOChannel,
                                             C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIODatablockConfigs(
      std::vector<C_OSCIOConfigDataChannel::E_DataBlockConfig> & orc_DatablockConfigs,
      C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOParameterStructs(
      std::vector<C_OSCIOConfigDataParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOParameterStruct(C_OSCIOConfigDataParameterStruct & orc_ParameterStruct,
                                                     C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOParameters(std::vector<C_OSCIOConfigDataParameter> & orc_Parameters,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIOParameter(C_OSCIOConfigDataParameter & orc_Parameter,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BaseName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
