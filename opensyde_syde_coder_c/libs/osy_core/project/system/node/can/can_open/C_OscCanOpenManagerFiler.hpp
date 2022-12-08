//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for CANopen data classes
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENMANAGERFILER_H
#define C_OSCCANOPENMANAGERFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscCanOpenManagerInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanOpenManagerFiler
{
public:
   C_OscCanOpenManagerFiler();

   static int32_t h_LoadFile(std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                             const stw::scl::C_SclString & orc_Path, const stw::scl::C_SclString & orc_BasePath);
   static int32_t h_SaveFile(const std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                             const stw::scl::C_SclString & orc_Path, const stw::scl::C_SclString & orc_BasePath,
                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles, const std::map<uint32_t,
                                                                                                         stw::scl::C_SclString> & orc_NodeIndicesToNameMap);
   static int32_t h_LoadData(std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                             C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath);
   static int32_t h_SaveData(const std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                             C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath,
                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles, const std::map<uint32_t,
                                                                                                         stw::scl::C_SclString> & orc_NodeIndicesToNameMap);

private:
   static const uint16_t mhu16_FILE_VERSION_1;

   static int32_t mh_LoadManagerData(C_OscCanOpenManagerInfo & orc_Config, C_OscXmlParserBase & orc_XmlParser,
                                     const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveManagerData(const C_OscCanOpenManagerInfo & orc_Config, C_OscXmlParserBase & orc_XmlParser,
                                     const stw::scl::C_SclString & orc_BasePath,
                                     std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                     const std::map<uint32_t,
                                                    stw::scl::C_SclString> & orc_NodeIndicesToNameMap);
   static int32_t mh_LoadManagerProperties(C_OscCanOpenManagerInfo & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveManagerProperties(const C_OscCanOpenManagerInfo & orc_Config, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadManagerSyncProperties(C_OscCanOpenManagerInfo & orc_Config,
                                               C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveManagerSyncProperties(const C_OscCanOpenManagerInfo & orc_Config,
                                            C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadManagerSubDevices(std::map<C_OscCanInterfaceId,
                                                    C_OscCanOpenManagerDeviceInfo> & orc_Config,
                                           C_OscXmlParserBase & orc_XmlParser,
                                           const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveManagerSubDevices(const std::map<C_OscCanInterfaceId,
                                                          C_OscCanOpenManagerDeviceInfo> & orc_Config,
                                           C_OscXmlParserBase & orc_XmlParser,
                                           const stw::scl::C_SclString & orc_BasePath,
                                           std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                           const std::map<uint32_t,
                                                          stw::scl::C_SclString> & orc_NodeIndicesToNameMap);
   static int32_t mh_LoadManagerSubDevice(C_OscCanOpenManagerDeviceInfo & orc_Config,
                                          C_OscXmlParserBase & orc_XmlParser,
                                          const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveManagerSubDevice(const C_OscCanOpenManagerDeviceInfo & orc_Config,
                                          C_OscXmlParserBase & orc_XmlParser,
                                          const stw::scl::C_SclString & orc_BasePath,
                                          std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                          const stw::scl::C_SclString & orc_NodeName,
                                          const uint8_t ou8_InterfaceNumber);
   static int32_t mh_LoadManagerSubDeviceEdsPart(C_OscCanOpenManagerDeviceInfo & orc_Config,
                                                 C_OscXmlParserBase & orc_XmlParser,
                                                 const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveManagerSubDeviceEdsPart(const C_OscCanOpenManagerDeviceInfo & orc_Config,
                                                 C_OscXmlParserBase & orc_XmlParser,
                                                 const stw::scl::C_SclString & orc_BasePath,
                                                 std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                                 const stw::scl::C_SclString & orc_NodeName,
                                                 const uint8_t ou8_InterfaceNumber);
   static int32_t mh_LoadManagerMappedSignals(std::vector< C_OscCanOpenManagerMappableSignal > & orc_Config,
                                              C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveManagerMappedSignals(const std::vector<C_OscCanOpenManagerMappableSignal> & orc_Config,
                                           C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadManagerMappedSignal(C_OscCanOpenManagerMappableSignal & orc_Config,
                                             C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveManagerMappedSignal(const C_OscCanOpenManagerMappableSignal & orc_Config,
                                          C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString mh_CanOpenManagerInfoTypeToString(
      const C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType & ore_Type);
   static int32_t mh_StringToCanOpenManagerInfoType(const stw::scl::C_SclString & orc_String,
                                                    C_OscCanOpenManagerInfo::E_NmtErrorBehaviourType & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
