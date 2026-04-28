//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View filer (core parts)
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCVIEWFILER_HPP
#define C_OSCVIEWFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscNode.hpp"
#include "C_OscViewData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscViewFiler
{
public:
   C_OscViewFiler(void);

   // Load
   static int32_t h_LoadSystemViewsFile(std::vector<C_OscViewData> & orc_Views,
                                        const stw::scl::C_SclString & orc_PathSystemViews,
                                        const std::vector<C_OscNode> & orc_OscNodes);
   static int32_t h_LoadViewsOsc(std::vector<C_OscViewData> & orc_Views, const std::vector<C_OscNode> & orc_OscNodes,
                                 C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath);
   static int32_t h_LoadViewOsc(C_OscViewData & orc_View, stw::opensyde_core::C_OscXmlParserBase & orc_XmlParser,
                                const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes);

   // Save
   static void h_SaveNodeActiveFlags(const std::vector<uint8_t> & orc_NodeActiveFlags,
                                     C_OscXmlParserBase & orc_XmlParser);
   static void h_SaveNodeUpdateInformation(const std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation,
                                           C_OscXmlParserBase & orc_XmlParser);
   static void h_SavePc(const opensyde_core::C_OscViewPc & orc_OscPc, C_OscXmlParserBase & orc_XmlParser);

   // Utilities
   static int32_t h_StringToSecurityOptionDebugger(const stw::scl::C_SclString & orc_String,
                                                   C_OscViewNodeUpdate::E_StateDebugger & ore_State);
   static int32_t h_StringToSecurityOptionAuthentication(const stw::scl::C_SclString & orc_String,
                                                         C_OscViewNodeUpdate::E_StateSecureAuthentication & ore_State);
   static int32_t h_StringToSecurityOptionEncryption(const stw::scl::C_SclString & orc_String,
                                                     C_OscViewNodeUpdate::E_StateTrafficEncryption & ore_State);
   static stw::scl::C_SclString h_SecurityOptionDebuggerToString(const C_OscViewNodeUpdate::E_StateDebugger oe_State);
   static stw::scl::C_SclString h_SecurityOptionAuthenticationToString(
      const C_OscViewNodeUpdate::E_StateSecureAuthentication oe_State);
   static stw::scl::C_SclString h_SecurityOptionEncryptionToString(
      const C_OscViewNodeUpdate::E_StateTrafficEncryption oe_State);

private:
   // Load
   static int32_t mh_LoadNodeActiveFlags(std::vector<uint8_t> & orc_NodeActiveFlags,
                                         C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformation(std::vector<C_OscViewNodeUpdate> & orc_NodeUpdateInformation,
                                               C_OscXmlParserBase & orc_XmlParser,
                                               const std::vector<C_OscNode> & orc_OscNodes);
   static void mh_LoadPc(opensyde_core::C_OscViewPc & orc_OscPc, const C_OscXmlParserBase & orc_XmlParser);
   static void mh_LoadNodeUpdateInformationPaths(std::vector<stw::scl::C_SclString> & orc_Paths,
                                                 const stw::scl::C_SclString & orc_XmlTagBaseName,
                                                 C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadOneNodeUpdateInformation(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                  C_OscXmlParserBase & orc_XmlParser, const C_OscNode & orc_Node);
   static void mh_LoadNodeUpdateInformationParam(std::vector<C_OscViewNodeUpdateParamInfo> & orc_Info,
                                                 C_OscXmlParserBase & orc_XmlParser);
   static void mh_LoadNodeUpdateInformationSkipUpdateOfFiles(std::vector<bool> & orc_Flags,
                                                             C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformationSecurity(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                       C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNodeUpdateInformationSecurityStates(C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                             C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadViewFileOsc(C_OscViewData & orc_View, const stw::scl::C_SclString & orc_FilePath,
                                     const std::vector<stw::opensyde_core::C_OscNode> & orc_OscNodes);

   // Save
   static void mh_SaveNodeUpdateInformationPaths(const std::vector<stw::scl::C_SclString> & orc_Paths,
                                                 const stw::scl::C_SclString & orc_XmlTagBaseName,
                                                 C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationParamInfo(const std::vector<C_OscViewNodeUpdateParamInfo> & orc_Info,
                                                     C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationSkipUpdateOfFiles(const std::vector<bool> & orc_Flags,
                                                             C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveNodeUpdateInformationSecurity(const C_OscViewNodeUpdate & orc_NodeUpdateInformation,
                                                    C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
