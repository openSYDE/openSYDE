//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for C_OscDataLoggerJob
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATALOGGERJOBFILER_HPP
#define C_OSCDATALOGGERJOBFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscDataLoggerJob.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDataLoggerJobFiler
{
public:
   static int32_t h_LoadFile(std::vector<C_OscDataLoggerJob> & orc_Config, const stw::scl::C_SclString & orc_Path);
   static int32_t h_SaveFile(const std::vector<C_OscDataLoggerJob> & orc_Config,
                             const stw::scl::C_SclString & orc_Path);
   static int32_t h_LoadData(std::vector<C_OscDataLoggerJob> & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveData(const std::vector<C_OscDataLoggerJob> & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataElementId(C_OscNodeDataPoolListElementId & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDataElementId(const C_OscNodeDataPoolListElementId & orc_Config,
                                   C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadDataElementOptArrayId(C_OscNodeDataPoolListElementOptArrayId & orc_Config,
                                              C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveDataElementOptArrayId(const C_OscNodeDataPoolListElementOptArrayId & orc_Config,
                                           C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_LoadCommInterfaceId(C_OscSystemBus::E_Type & ore_Type, uint8_t & oru8_InterfaceNumber,
                                        C_OscXmlParserBase & orc_XmlParser,
                                        const stw::scl::C_SclString & orc_ParentNodeName,
                                        const stw::scl::C_SclString & orc_UseCase);
   static void h_SaveCommInterfaceId(const C_OscSystemBus::E_Type oe_Type, const uint8_t ou8_InterfaceNumber,
                                     C_OscXmlParserBase & orc_XmlParser);

private:
   C_OscDataLoggerJobFiler();

   static const uint16_t mhu16_FILE_VERSION_1;

   static int32_t mh_LoadJobData(C_OscDataLoggerJob & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveJobData(const C_OscDataLoggerJob & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadJobProperties(C_OscDataLoggerJobProperties & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveJobProperties(const C_OscDataLoggerJobProperties & orc_Config,
                                    C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadConfiguredDataElements(std::vector<C_OscDataLoggerDataElementReference> & orc_Config,
                                                C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveConfiguredDataElements(const std::vector<C_OscDataLoggerDataElementReference> & orc_Config,
                                             C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadConfiguredDataElement(C_OscDataLoggerDataElementReference & orc_Config,
                                               C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveConfiguredDataElement(const C_OscDataLoggerDataElementReference & orc_Config,
                                            C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString mh_LogFileTypeTypeToString(
      const C_OscDataLoggerJobProperties::E_LogFileFormat & ore_Type);
   static int32_t mh_StringToLogFileType(const stw::scl::C_SclString & orc_String,
                                         C_OscDataLoggerJobProperties::E_LogFileFormat & ore_Type);
   static stw::scl::C_SclString mh_LocalLogTriggerTypeToString(
      const C_OscDataLoggerJobProperties::E_LocalLogTrigger & ore_Type);
   static int32_t mh_StringToLocalLogTriggerType(const stw::scl::C_SclString & orc_String,
                                                 C_OscDataLoggerJobProperties::E_LocalLogTrigger & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
