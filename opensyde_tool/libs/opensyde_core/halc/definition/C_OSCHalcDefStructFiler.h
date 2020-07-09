//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC definition struct section
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFSTRUCTFILER_H
#define C_OSCHALCDEFSTRUCTFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcDefStruct.h"
#include "C_OSCXMLParser.h"
#include "C_OSCHalcDefChannelUseCase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefStructFiler
{
public:
   static stw_types::sint32 h_LoadStructs(std::vector<C_OSCHalcDefStruct> & orc_Structs,
                                          C_OSCXMLParserBase & orc_XMLParser,
                                          const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                          const stw_scl::C_SCLString & orc_CurrentNodeName,
                                          const stw_scl::C_SCLString & orc_SectionNodeName,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId,
                                          const bool oq_RequireSection, const stw_types::uint32 ou32_DomainNameLength);
   static stw_types::sint32 h_SaveStructs(const std::vector<C_OSCHalcDefStruct> & orc_Structs,
                                          const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                          C_OSCXMLParserBase & orc_XMLParser,
                                          const stw_scl::C_SCLString & orc_CurrentNodeName,
                                          const stw_scl::C_SCLString & orc_SectionNodeName,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName);
   static void h_ParseSimplestTypeValue(const stw_scl::C_SCLString & orc_TypeStr,
                                        C_OSCNodeDataPoolContent & orc_Content,
                                        const C_OSCXMLParserBase & orc_XMLParser,
                                        const stw_scl::C_SCLString & orc_AttributeName);
   static stw_types::sint32 h_GetTypeForSimplestTypeString(const stw_scl::C_SCLString & orc_TypeStr,
                                                           C_OSCNodeDataPoolContent::E_Type & ore_Type);
   static stw_scl::C_SCLString h_GetTypeString(const C_OSCNodeDataPoolContent::E_Type oe_Type);
   static stw_types::sint32 h_SaveSimpleValueAsAttribute(const stw_scl::C_SCLString & orc_Node,
                                                         C_OSCXMLParserBase & orc_XMLParser,
                                                         const C_OSCNodeDataPoolContent & orc_Content);
   static stw_types::sint32 h_SetType(C_OSCXMLParserBase & orc_XMLParser, C_OSCHalcDefContent & orc_Content,
                                      stw_scl::C_SCLString & orc_TypeStr, stw_scl::C_SCLString & orc_BaseTypeStr,
                                      const stw_scl::C_SCLString & orc_CurrentNodeName,
                                      const stw_scl::C_SCLString & orc_UseCaseForLog);
   static stw_types::sint32 h_ParseAttributeIntoContent(C_OSCHalcDefContent & orc_Content,
                                                        const C_OSCXMLParserBase & orc_XMLParser,
                                                        const stw_scl::C_SCLString & orc_AttributeName,
                                                        const stw_scl::C_SCLString & orc_Type,
                                                        const stw_scl::C_SCLString & orc_BaseType,
                                                        const bool oq_RequireAttribute,
                                                        const stw_scl::C_SCLString & orc_CurrentNodeName);

private:
   static const stw_scl::C_SCLString mhc_False;
   static const stw_scl::C_SCLString mhc_True;
   static const stw_types::uint32 hu32_MAX_ALLOWED_COMBINED_VARIABLE_LENGTH = 32UL;

   C_OSCHalcDefStructFiler(void);

   static stw_types::sint32 mh_LoadStruct(C_OSCHalcDefStruct & orc_Struct, C_OSCXMLParserBase & orc_XMLParser,
                                          const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId,
                                          const stw_types::uint32 ou32_DomainNameLength);
   static stw_types::sint32 mh_SaveStruct(const C_OSCHalcDefStruct & orc_Struct,
                                          const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                          C_OSCXMLParserBase & orc_XMLParser,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName);
   static stw_types::sint32 mh_LoadDataElement(C_OSCHalcDefElement & orc_Element, C_OSCXMLParserBase & orc_XMLParser,
                                               const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                               const std::vector<stw_types::uint32> & orc_BaseAvailability,
                                               const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId,
                                               const stw_types::uint32 ou32_DomainNameLength);
   static stw_types::sint32 mh_SaveDataElement(const C_OSCHalcDefElement & orc_Element,
                                               const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_SingleNodeName);
   static stw_types::sint32 mh_ParseAttributeAvailability(std::vector<stw_types::uint32> & orc_Availability,
                                                          const stw_scl::C_SCLString & orc_AttributeContent,
                                                          const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases);
   static stw_types::sint32 mh_GetAvailabilityString(const std::vector<stw_types::uint32> & orc_Availability,
                                                     const std::vector<C_OSCHalcDefChannelUseCase> & orc_UseCases,
                                                     stw_scl::C_SCLString & orc_Output);
   static void mh_SetMaxValForType(const stw_scl::C_SCLString & orc_TypeStr, C_OSCNodeDataPoolContent & orc_Content);
   static void mh_SetMinValForType(const stw_scl::C_SCLString & orc_TypeStr, C_OSCNodeDataPoolContent & orc_Content);
   static bool mh_CheckInitialBitmaskContentValid(const C_OSCHalcDefContent & orc_Content);
   static stw_scl::C_SCLString mh_ConvertToHex(const stw_types::uint64 ou64_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
