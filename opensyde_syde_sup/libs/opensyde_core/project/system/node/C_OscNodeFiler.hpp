//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node reader/writer (V3)

   (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEFILERV3_H
#define C_OSCNODEFILERV3_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNode.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Node reader/writer
class C_OscNodeFiler
{
public:
   static int32_t h_LoadNodeFile(C_OscNode & orc_Node, const stw::scl::C_SclString & orc_FilePath,
                                 const bool oq_SkipContent = false);
   static int32_t h_LoadNode(C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                             const stw::scl::C_SclString & orc_BasePath, const bool oq_SkipContent = false);
   static int32_t h_SaveNodeFile(const C_OscNode & orc_Node, const stw::scl::C_SclString & orc_FilePath,
                                 std::vector<stw::scl::C_SclString> * const opc_CreatedFiles, const std::map<uint32_t,
                                                                                                             stw::scl::C_SclString> & orc_NodeIndicesToNameMap);
   static int32_t h_SaveNode(const C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                             const stw::scl::C_SclString & orc_BasePath,
                             std::vector<stw::scl::C_SclString> * const opc_CreatedFiles, const std::map<uint32_t,
                                                                                                         stw::scl::C_SclString> & orc_NodeIndicesToNameMap);
   static int32_t h_LoadNodeComProtocols(std::vector<C_OscCanProtocol> & orc_NodeComProtocols,
                                         const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                         C_OscXmlParserBase & orc_XmlParser,
                                         const stw::scl::C_SclString & orc_BasePath);
   static int32_t h_SaveNodeComProtocols(const std::vector<C_OscCanProtocol> & orc_NodeComProtocols,
                                         const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                         C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath,
                                         std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);
   static stw::scl::C_SclString h_GetFolderName(const stw::scl::C_SclString & orc_NodeName);
   static stw::scl::C_SclString h_GetFileName(void);

private:
   static int32_t mh_LoadProperties(C_OscNodeProperties & orc_NodeProperties, C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveProperties(const C_OscNodeProperties & orc_NodeProperties, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadStwFlashloaderOptions(C_OscNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                               C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveStwFlashloaderOptions(const C_OscNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                            C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadComInterface(std::vector<C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
                                      C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveComInterface(const std::vector<C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
                                   C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadApplications(std::vector<C_OscNodeApplication> & orc_NodeApplications,
                                      C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveApplications(const std::vector<C_OscNodeApplication> & orc_NodeApplications,
                                   C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadDataPools(C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                   const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveDataPools(const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                   C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath,
                                   std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);
   static int32_t mh_LoadHalc(C_OscHalcConfig & orc_Config, C_OscXmlParserBase & orc_XmlParser,
                              const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveHalc(const C_OscHalcConfig & orc_Config, C_OscXmlParserBase & orc_XmlParser,
                              const stw::scl::C_SclString & orc_BasePath,
                              std::vector<stw::scl::C_SclString> * const opc_CreatedFiles);
   static int32_t mh_LoadCanOpenManagers(std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                                         C_OscXmlParserBase & orc_XmlParser,
                                         const stw::scl::C_SclString & orc_BasePath);
   static int32_t mh_SaveCanOpenManagers(const std::map<uint8_t,
                                                        C_OscCanOpenManagerInfo> & orc_Config,
                                         C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath,
                                         std::vector<stw::scl::C_SclString> * const opc_CreatedFiles,
                                         const std::map<uint32_t,
                                                        stw::scl::C_SclString> & orc_NodeIndicesToNameMap);
   static stw::scl::C_SclString mh_DiagnosticServerToString(const C_OscNodeProperties::E_DiagnosticServerProtocol &
                                                            ore_DiagnosticProtocol);
   static int32_t mh_StringToDiagnosticServer(const stw::scl::C_SclString & orc_String,
                                              C_OscNodeProperties::E_DiagnosticServerProtocol & ore_Type);
   static stw::scl::C_SclString mh_FlashLoaderToString(const C_OscNodeProperties::E_FlashLoaderProtocol &
                                                       ore_FlashLoader);
   static int32_t mh_StringToFlashLoader(const stw::scl::C_SclString & orc_String,
                                         C_OscNodeProperties::E_FlashLoaderProtocol & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
