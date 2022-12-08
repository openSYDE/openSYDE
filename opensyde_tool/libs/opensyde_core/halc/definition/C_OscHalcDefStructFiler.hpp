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
#include "C_OscHalcDefStruct.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscHalcDefChannelUseCase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefStructFiler
{
public:
   static int32_t h_LoadStructs(std::vector<C_OscHalcDefStruct> & orc_Structs, C_OscXmlParserBase & orc_XmlParser,
                                const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                const stw::scl::C_SclString & orc_CurrentNodeName,
                                const stw::scl::C_SclString & orc_SectionNodeName,
                                const stw::scl::C_SclString & orc_GroupNodeName,
                                const stw::scl::C_SclString & orc_SingleNodeName, const bool oq_RequireId,
                                const bool oq_RequireSection, const uint32_t ou32_DomainNameLength);
   static int32_t h_SaveStructs(const std::vector<C_OscHalcDefStruct> & orc_Structs,
                                const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_CurrentNodeName,
                                const stw::scl::C_SclString & orc_SectionNodeName,
                                const stw::scl::C_SclString & orc_GroupNodeName,
                                const stw::scl::C_SclString & orc_SingleNodeName);
   static int32_t h_ParseSimplestTypeValue(const stw::scl::C_SclString & orc_TypeStr,
                                           C_OscNodeDataPoolContent & orc_Content,
                                           const C_OscXmlParserBase & orc_XmlParser,
                                           const stw::scl::C_SclString & orc_AttributeName);
   static int32_t h_GetTypeForSimplestTypeString(const stw::scl::C_SclString & orc_TypeStr,
                                                 C_OscNodeDataPoolContent::E_Type & ore_Type,
                                                 const C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString h_GetTypeString(const C_OscNodeDataPoolContent::E_Type oe_Type);
   static int32_t h_SaveSimpleValueAsAttribute(const stw::scl::C_SclString & orc_Node,
                                               C_OscXmlParserBase & orc_XmlParser,
                                               const C_OscNodeDataPoolContent & orc_Content);
   static int32_t h_SetType(C_OscXmlParserBase & orc_XmlParser, C_OscHalcDefContent & orc_Content,
                            stw::scl::C_SclString & orc_TypeStr, stw::scl::C_SclString & orc_BaseTypeStr,
                            const stw::scl::C_SclString & orc_CurrentNodeName);
   static int32_t h_ParseAttributeIntoContent(C_OscHalcDefContent & orc_Content,
                                              const C_OscXmlParserBase & orc_XmlParser,
                                              const stw::scl::C_SclString & orc_AttributeName,
                                              const stw::scl::C_SclString & orc_Type,
                                              const stw::scl::C_SclString & orc_BaseType,
                                              const bool oq_RequireAttribute);

   static const uint32_t hu32_MAX_ALLOWED_COMBINED_VARIABLE_LENGTH;

private:
   static const stw::scl::C_SclString mhc_FALSE;
   static const stw::scl::C_SclString mhc_TRUE;

   C_OscHalcDefStructFiler(void);

   static int32_t mh_LoadStruct(C_OscHalcDefStruct & orc_Struct, C_OscXmlParserBase & orc_XmlParser,
                                const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                const stw::scl::C_SclString & orc_GroupNodeName,
                                const stw::scl::C_SclString & orc_SingleNodeName, const bool oq_RequireId,
                                const uint32_t ou32_DomainNameLength);
   static int32_t mh_SaveStruct(const C_OscHalcDefStruct & orc_Struct,
                                const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_GroupNodeName,
                                const stw::scl::C_SclString & orc_SingleNodeName);
   static int32_t mh_LoadDataElement(C_OscHalcDefElement & orc_Element, C_OscXmlParserBase & orc_XmlParser,
                                     const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                     const std::vector<uint32_t> & orc_BaseAvailability,
                                     const stw::scl::C_SclString & orc_SingleNodeName, const bool oq_RequireId,
                                     const uint32_t ou32_DomainNameLength);
   static int32_t mh_SaveDataElement(const C_OscHalcDefElement & orc_Element,
                                     const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                     C_OscXmlParserBase & orc_XmlParser,
                                     const stw::scl::C_SclString & orc_SingleNodeName);
   static int32_t mh_ParseAttributeAvailability(std::vector<uint32_t> & orc_Availability,
                                                const stw::scl::C_SclString & orc_AttributeContent,
                                                const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                                const C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_GetAvailabilityString(const std::vector<uint32_t> & orc_Availability,
                                           const std::vector<C_OscHalcDefChannelUseCase> & orc_UseCases,
                                           stw::scl::C_SclString & orc_Output);
   static void mh_SetMaxValForType(const stw::scl::C_SclString & orc_TypeStr, C_OscNodeDataPoolContent & orc_Content);
   static void mh_SetMinValForType(const stw::scl::C_SclString & orc_TypeStr, C_OscNodeDataPoolContent & orc_Content);
   static bool mh_CheckInitialBitmaskContentValid(const C_OscHalcDefContent & orc_Content,
                                                  const C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString mh_ConvertToHex(const uint64_t ou64_Value);
   static void mh_HandleEnumMinMax(C_OscHalcDefElement & orc_Element);
   static int32_t mh_CheckValidUint(const stw::scl::C_SclString & orc_Item);
   static int32_t mh_CheckValidSint(const stw::scl::C_SclString & orc_Item);
   static int32_t mh_CheckValidDouble(const stw::scl::C_SclString & orc_Item);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
