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
#include "C_OSCXMLParser.h"
#include "C_OSCCanOpenManagerInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanOpenManagerFiler
{
public:
   C_OSCCanOpenManagerFiler();

   static stw_types::sint32 h_LoadFile(std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> & orc_Config,
                                       const stw_scl::C_SCLString & orc_Path,
                                       const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveFile(const std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> & orc_Config,
                                       const stw_scl::C_SCLString & orc_Path, const stw_scl::C_SCLString & orc_BasePath,
                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                       const std::map<stw_types::uint32,
                                                      stw_scl::C_SCLString> & orc_NodeIndicesToNameMap);
   static stw_types::sint32 h_LoadData(std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> & orc_Config,
                                       C_OSCXMLParserBase & orc_XMLParser, const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveData(const std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> & orc_Config,
                                       C_OSCXMLParserBase & orc_XMLParser, const stw_scl::C_SCLString & orc_BasePath,
                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                       const std::map<stw_types::uint32,
                                                      stw_scl::C_SCLString> & orc_NodeIndicesToNameMap);

private:
   static const stw_types::uint16 mhu16_FILE_VERSION_1;

   static stw_types::sint32 mh_LoadManagerData(C_OSCCanOpenManagerInfo & orc_Config, C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_SaveManagerData(const C_OSCCanOpenManagerInfo & orc_Config,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BasePath,
                                               std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                               const std::map<stw_types::uint32,
                                                              stw_scl::C_SCLString> & orc_NodeIndicesToNameMap);
   static stw_types::sint32 mh_LoadManagerProperties(C_OSCCanOpenManagerInfo & orc_Config,
                                                     C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveManagerProperties(const C_OSCCanOpenManagerInfo & orc_Config, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadManagerSubDevices(std::map<C_OSCCanInterfaceId,
                                                              C_OSCCanOpenManagerDeviceInfo> & orc_Config,
                                                     C_OSCXMLParserBase & orc_XMLParser,
                                                     const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_SaveManagerSubDevices(const std::map<C_OSCCanInterfaceId,
                                                                    C_OSCCanOpenManagerDeviceInfo> & orc_Config,
                                                     C_OSCXMLParserBase & orc_XMLParser,
                                                     const stw_scl::C_SCLString & orc_BasePath,
                                                     std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                     const std::map<stw_types::uint32,
                                                                    stw_scl::C_SCLString> & orc_NodeIndicesToNameMap);
   static stw_types::sint32 mh_LoadManagerSubDevice(C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                    C_OSCXMLParserBase & orc_XMLParser,
                                                    const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_SaveManagerSubDevice(const C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                    C_OSCXMLParserBase & orc_XMLParser,
                                                    const stw_scl::C_SCLString & orc_BasePath,
                                                    std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                    const stw_scl::C_SCLString & orc_NodeName,
                                                    const stw_types::uint8 ou8_InterfaceNumber);
   static stw_types::sint32 mh_LoadManagerSubDeviceEDSPart(C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                           C_OSCXMLParserBase & orc_XMLParser,
                                                           const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_SaveManagerSubDeviceEDSPart(const C_OSCCanOpenManagerDeviceInfo & orc_Config,
                                                           C_OSCXMLParserBase & orc_XMLParser,
                                                           const stw_scl::C_SCLString & orc_BasePath,
                                                           std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles,
                                                           const stw_scl::C_SCLString & orc_NodeName,
                                                           const stw_types::uint8 ou8_InterfaceNumber);
   static stw_types::sint32 mh_LoadManagerMappedSignals(std::vector< C_OSCCanOpenManagerMappableSignal > & orc_Config,
                                                        C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveManagerMappedSignals(const std::vector<C_OSCCanOpenManagerMappableSignal> & orc_Config,
                                           C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadManagerMappedSignal(C_OSCCanOpenManagerMappableSignal & orc_Config,
                                                       C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveManagerMappedSignal(const C_OSCCanOpenManagerMappableSignal & orc_Config,
                                          C_OSCXMLParserBase & orc_XMLParser);
   static stw_scl::C_SCLString mh_CanOpenManagerInfoTypeToString(
      const C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType & ore_Type);
   static stw_types::sint32 mh_StringToCanOpenManagerInfoType(const stw_scl::C_SCLString & orc_String,
                                                              C_OSCCanOpenManagerInfo::E_NMTErrorBehaviourType & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
