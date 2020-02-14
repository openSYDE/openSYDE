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
#include "C_OSCNode.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Node reader/writer
class C_OSCNodeFiler
{
public:
   static stw_types::sint32 h_LoadNodeFile(C_OSCNode & orc_Node, const stw_scl::C_SCLString & orc_FilePath);
   static stw_types::sint32 h_LoadNode(C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                       const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveNodeFile(const C_OSCNode & orc_Node, const stw_scl::C_SCLString & orc_FilePath,
                                           std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 h_SaveNode(const C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                       const stw_scl::C_SCLString & orc_BasePath,
                                       std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 h_LoadNodeComProtocols(std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                                   const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 h_SaveNodeComProtocols(const std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                                   const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_scl::C_SCLString & orc_BasePath,
                                                   std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_scl::C_SCLString h_GetFolderName(const stw_scl::C_SCLString & orc_NodeName);
   static stw_scl::C_SCLString h_GetFileName(void);

private:
   static stw_types::sint32 mh_LoadProperties(C_OSCNodeProperties & orc_NodeProperties,
                                              C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveProperties(const C_OSCNodeProperties & orc_NodeProperties, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadStwFlashloaderOptions(C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                         C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveStwFlashloaderOptions(const C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                            C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadApplications(std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static void mh_SaveApplications(const std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                   C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadDataPools(C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                             const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_SaveDataPools(const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                             C_OSCXMLParserBase & orc_XMLParser,
                                             const stw_scl::C_SCLString & orc_BasePath,
                                             std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_types::sint32 mh_LoadHALC(C_OSCHalcConfig & orc_Config, C_OSCXMLParserBase & orc_XMLParser,
                                        const stw_scl::C_SCLString & orc_BasePath);
   static stw_types::sint32 mh_SaveHALC(const C_OSCHalcConfig & orc_Config, C_OSCXMLParserBase & orc_XMLParser,
                                        const stw_scl::C_SCLString & orc_BasePath,
                                        std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles);
   static stw_scl::C_SCLString mh_DiagnosticServerToString(
      const C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_DiagnosticProtocol);
   static stw_types::sint32 mh_StringToDiagnosticServer(const stw_scl::C_SCLString & orc_String,
                                                        C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_Type);
   static stw_scl::C_SCLString mh_FlashLoaderToString(
      const C_OSCNodeProperties::E_FlashLoaderProtocol & ore_FlashLoader);
   static stw_types::sint32 mh_StringToFlashLoader(const stw_scl::C_SCLString & orc_String,
                                                   C_OSCNodeProperties::E_FlashLoaderProtocol & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
