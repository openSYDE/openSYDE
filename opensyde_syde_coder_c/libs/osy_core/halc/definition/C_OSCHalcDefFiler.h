//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFFILER_H
#define C_OSCHALCDEFFILER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcDef.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefFiler
{
public:
   static stw_types::sint32 h_LoadFile(C_OSCHalcDefBase & orc_IOData, const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_SaveFile(const C_OSCHalcDefBase & orc_IOData, const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 h_LoadData(C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 h_SaveData(const C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser);

   static stw_types::sint32 h_LoadAvailability(const stw_scl::C_SCLString & orc_AvailabilityString,
                                               std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability,
                                               const stw_types::uint32 ou32_NumChannels);
   static stw_types::sint32 h_CheckUseCaseValue(const C_OSCHalcDefDomain & orc_IODataDomain);
   static stw_types::sint32 h_CheckDomainDisplayNames(const C_OSCHalcDefDomain & orc_IODataDomain);

private:
   static const stw_types::uint16 mhu16_FILE_VERSION_1 = 1U;

   C_OSCHalcDefFiler(void);

   static stw_types::sint32 mh_SaveFile(const C_OSCHalcDefBase & orc_IOData, const stw_scl::C_SCLString & orc_Path);
   static stw_types::sint32 mh_SaveIODomain(const C_OSCHalcDefDomain & orc_IODataDomain,
                                            C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadIODataDomain(C_OSCHalcDefDomain & orc_IODataDomain,
                                                C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_CheckIODataDomain(const C_OSCHalcDefDomain & orc_IODataDomain);
   static stw_types::sint32 mh_CheckDefaultUseCase(const C_OSCHalcDefDomain & orc_IODataDomain);
   static stw_types::sint32 mh_LoadChannels(std::vector<C_OSCHalcDefChannelDef> & orc_Channels,
                                            C_OSCXMLParserBase & orc_XMLParser);
   static stw_types::sint32 mh_LoadChannelUseCases(std::vector<C_OSCHalcDefChannelUseCase> & orc_ChannelUsecases,
                                                   C_OSCXMLParserBase & orc_XMLParser,
                                                   const stw_types::uint32 ou32_NumChannels);
   static stw_types::sint32 mh_SplitAvailabilityString(const stw_scl::C_SCLString & orc_AvailabilityString,
                                                       std::vector<stw_scl::C_SCLString> & orc_SubElements);
   static stw_types::sint32 mh_ParseAvailabilityStringSubElements(
      const std::vector<stw_scl::C_SCLString> & orc_SubElements,
      std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability, const stw_types::uint32 ou32_NumChannels);
   static stw_types::sint32 mh_CheckAvailability(const std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability);
   static stw_types::sint32 mh_ConvertStringToNumber(const stw_scl::C_SCLString & orc_Number,
                                                     stw_types::sintn & orsn_Number);
   static stw_types::sint32 mh_HandleNumberSection(stw_scl::C_SCLString & orc_Number,
                                                   std::vector<stw_types::sintn> & orc_FoundNumbers,
                                                   bool & orq_LastNumDeclaredSection,
                                                   const stw_scl::C_SCLString & orc_Section);
   static stw_types::sint32 mh_HandleNumberSectionEnd(const std::vector<stw_types::sintn> & orc_FoundNumbers,
                                                      const bool oq_IsGroupSection,
                                                      std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability,
                                                      const stw_types::uint32 ou32_NumChannels);
   static stw_scl::C_SCLString mh_GetAvailabilityString(
      const std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability);
   static stw_types::sint32 mh_SaveUseCase(const C_OSCHalcDefChannelUseCase & orc_UseCase,
                                           C_OSCXMLParserBase & orc_XMLParser);
   static stw_scl::C_SCLString mh_DomainCategoryEnumToString(const C_OSCHalcDefDomain::E_Category oe_Category);
   static stw_types::sint32 mh_DomainCategoryStringToEnum(const stw_scl::C_SCLString & orc_Category,
                                                          C_OSCHalcDefDomain::E_Category & ore_Category);
   static void mh_GetAllNames(const std::vector<C_OSCHalcDefStruct> & orc_Values,
                              std::vector<stw_scl::C_SCLString> & orc_Names);
   static stw_types::sint32 mh_CheckDuplicateNames(const stw_scl::C_SCLString & orc_Section,
                                                   const stw_scl::C_SCLString & orc_DomainSingularName,
                                                   std::vector<stw_scl::C_SCLString> & orc_Names);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
