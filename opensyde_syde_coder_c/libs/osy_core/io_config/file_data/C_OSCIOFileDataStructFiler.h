//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load IO description struct section
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATASTRUCTFILER_H
#define C_OSCIOFILEDATASTRUCTFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCIOFileDataStruct.h"
#include "C_OSCXMLParser.h"
#include "C_OSCIOFileDataChannelUseCase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataStructFiler
{
public:
   static stw_types::sint32 h_LoadStructs(std::vector<C_OSCIOFileDataStruct> & orc_Structs,
                                          C_OSCXMLParserBase & orc_XMLParser,
                                          const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                          const stw_scl::C_SCLString & orc_CurrentNodeName,
                                          const stw_scl::C_SCLString & orc_SectionNodeName,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId,
                                          const bool oq_RequireSection);
   static stw_types::sint32 h_SaveStructs(const std::vector<C_OSCIOFileDataStruct> & orc_Structs,
                                          const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                          C_OSCXMLParserBase & orc_XMLParser,
                                          const stw_scl::C_SCLString & orc_CurrentNodeName,
                                          const stw_scl::C_SCLString & orc_SectionNodeName,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName);
   static void h_ParseSimplestTypeValue(const stw_scl::C_SCLString & orc_Type_Str,
                                        C_OSCNodeDataPoolContent & orc_Content,
                                        const C_OSCXMLParserBase & orc_XMLParser,
                                        const stw_scl::C_SCLString & orc_AttributeName);
   static stw_types::sint32 h_GetTypeForSimplestTypeString(const stw_scl::C_SCLString & orc_Type_Str,
                                                           C_OSCNodeDataPoolContent::E_Type & ore_Type);
   static stw_scl::C_SCLString h_GetTypeString(const C_OSCNodeDataPoolContent::E_Type oe_Type);
   static stw_types::sint32 h_SaveSimpleValueAsAttribute(const stw_scl::C_SCLString & orc_Node,
                                                         C_OSCXMLParserBase & orc_XMLParser,
                                                         const C_OSCNodeDataPoolContent & orc_Content);
   static stw_types::sint32 h_SetType(C_OSCXMLParserBase & orc_XMLParser, C_OSCIOFileDataContent & orc_Content,
                                      stw_scl::C_SCLString & orc_TypeStr, stw_scl::C_SCLString & orc_BaseTypeStr,
                                      const stw_scl::C_SCLString & orc_CurrentNodeName,
                                      const stw_scl::C_SCLString & orc_UseCaseForLog);
   static stw_types::sint32 h_ParseAttributeIntoContent(C_OSCIOFileDataContent & orc_Content,
                                                         const C_OSCXMLParserBase & orc_XMLParser,
                                                         const stw_scl::C_SCLString & orc_AttributeName,
                                                         const stw_scl::C_SCLString & orc_Type,
                                                         const stw_scl::C_SCLString & orc_BaseType,
                                                         const bool oq_RequireAttribute,
                                                         const stw_scl::C_SCLString & orc_CurrentNodeName);

private:
   static const stw_scl::C_SCLString mhc_False;
   static const stw_scl::C_SCLString mhc_True;

   C_OSCIOFileDataStructFiler(void);

   static stw_types::sint32 mh_LoadStruct(C_OSCIOFileDataStruct & orc_Struct, C_OSCXMLParserBase & orc_XMLParser,
                                          const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName, const bool oq_RequireId);
   static stw_types::sint32 mh_SaveStruct(const C_OSCIOFileDataStruct & orc_Struct,
                                          const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                          C_OSCXMLParserBase & orc_XMLParser,
                                          const stw_scl::C_SCLString & orc_GroupNodeName,
                                          const stw_scl::C_SCLString & orc_SingleNodeName);
   static stw_types::sint32 mh_LoadDataElement(C_OSCIOFileDataElement & orc_Element, C_OSCXMLParserBase & orc_XMLParser,
                                               const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                               const std::vector<stw_types::uint32> & orc_BaseAvailability,
                                               const stw_scl::C_SCLString & orc_SingleNodeName,
                                               const bool oq_RequireId);
   static stw_types::sint32 mh_SaveDataElement(const C_OSCIOFileDataElement & orc_Element,
                                               const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_SingleNodeName);
   static stw_types::sint32 mh_ParseAttributeAvailability(std::vector<stw_types::uint32> & orc_Availability,
                                                          const stw_scl::C_SCLString & orc_AttributeContent,
                                                          const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases);
   static stw_types::sint32 mh_GetAvailabilityString(const std::vector<stw_types::uint32> & orc_Availability,
                                                     const std::vector<C_OSCIOFileDataChannelUseCase> & orc_UseCases,
                                                     stw_scl::C_SCLString & orc_Output);
   static void mh_SetMaxValForType(const stw_scl::C_SCLString & orc_Type_Str, C_OSCNodeDataPoolContent & orc_Content);
   static void mh_SetMinValForType(const stw_scl::C_SCLString & orc_Type_Str, C_OSCNodeDataPoolContent & orc_Content);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
