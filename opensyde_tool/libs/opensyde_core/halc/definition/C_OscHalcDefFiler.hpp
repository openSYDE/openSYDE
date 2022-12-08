//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC definition
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFFILER_HPP
#define C_OSCHALCDEFFILER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcDef.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefFiler
{
public:
   static int32_t h_LoadFile(C_OscHalcDefBase & orc_IoData, const stw::scl::C_SclString & orc_Path);
   static int32_t h_SaveFile(const C_OscHalcDefBase & orc_IoData, const stw::scl::C_SclString & orc_Path);
   static int32_t h_LoadData(C_OscHalcDefBase & orc_IoData, C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_SaveData(const C_OscHalcDefBase & orc_IoData, C_OscXmlParserBase & orc_XmlParser);

   static int32_t h_LoadAvailability(const stw::scl::C_SclString & orc_AttributeName,
                                     std::vector<C_OscHalcDefChannelAvailability> & orc_Availability,
                                     const uint32_t ou32_NumChannels, const C_OscXmlParserBase & orc_XmlParser);
   static int32_t h_CheckUseCaseValue(const C_OscHalcDefDomain & orc_IoDataDomain);
   static int32_t h_CheckDomainDisplayNames(const C_OscHalcDefDomain & orc_IoDataDomain);

private:
   static const uint16_t mhu16_FILE_VERSION_1 = 1U;

   C_OscHalcDefFiler(void);

   static int32_t mh_LoadNvmData(C_OscHalcDefBase & orc_IoData, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadNvmAddressOffsetData(std::vector<uint32_t> & orc_Vector,
                                              const C_OscXmlParserBase & orc_XmlParser,
                                              const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode,
                                              const uint8_t ou8_NumConfigCopies, const bool oq_IsSafeVector);
   static int32_t mh_SaveIoDomain(const C_OscHalcDefDomain & orc_IoDataDomain, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadIoDataDomain(C_OscHalcDefDomain & orc_IoDataDomain, C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_CheckIoDataDomain(const C_OscHalcDefDomain & orc_IoDataDomain);
   static int32_t mh_CheckDefaultUseCase(const C_OscHalcDefDomain & orc_IoDataDomain);
   static int32_t mh_LoadChannels(std::vector<C_OscHalcDefChannelDef> & orc_Channels,
                                  C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_LoadChannelUseCases(std::vector<C_OscHalcDefChannelUseCase> & orc_ChannelUsecases,
                                         C_OscXmlParserBase & orc_XmlParser, const uint32_t ou32_NumChannels);
   static int32_t mh_SplitAvailabilityString(const stw::scl::C_SclString & orc_AvailabilityString,
                                             std::vector<stw::scl::C_SclString> & orc_SubElements,
                                             const C_OscXmlParserBase & orc_XmlParser,
                                             const stw::scl::C_SclString & orc_AttributeName);
   static int32_t mh_ParseAvailabilityStringSubElements(const std::vector<stw::scl::C_SclString> & orc_SubElements,
                                                        std::vector<C_OscHalcDefChannelAvailability> & orc_Availability,
                                                        const uint32_t ou32_NumChannels,
                                                        const C_OscXmlParserBase & orc_XmlParser,
                                                        const stw::scl::C_SclString & orc_AttributeName);
   static int32_t mh_CheckAvailability(const std::vector<C_OscHalcDefChannelAvailability> & orc_Availability,
                                       const C_OscXmlParserBase & orc_XmlParser);
   static int32_t mh_ConvertStringToNumber(const stw::scl::C_SclString & orc_Number, int32_t & ors32_Number,
                                           const C_OscXmlParserBase & orc_XmlParser,
                                           const stw::scl::C_SclString & orc_AttributeName);
   static int32_t mh_HandleNumberSection(stw::scl::C_SclString & orc_Number, std::vector<int32_t> & orc_FoundNumbers,
                                         bool & orq_LastNumDeclaredSection, const stw::scl::C_SclString & orc_Section,
                                         const C_OscXmlParserBase & orc_XmlParser,
                                         const stw::scl::C_SclString & orc_AttributeName);
   static int32_t mh_HandleNumberSectionEnd(const std::vector<int32_t> & orc_FoundNumbers, const bool oq_IsGroupSection,
                                            std::vector<C_OscHalcDefChannelAvailability> & orc_Availability,
                                            const uint32_t ou32_NumChannels, const C_OscXmlParserBase & orc_XmlParser,
                                            const stw::scl::C_SclString & orc_AttributeName);
   static stw::scl::C_SclString mh_GetAvailabilityString(
      const std::vector<C_OscHalcDefChannelAvailability> & orc_Availability, const bool oq_OnlySaveOnce);
   static int32_t mh_SaveUseCase(const C_OscHalcDefChannelUseCase & orc_UseCase, C_OscXmlParserBase & orc_XmlParser);
   static stw::scl::C_SclString mh_DomainCategoryEnumToString(const C_OscHalcDefDomain::E_Category oe_Category);
   static int32_t mh_DomainCategoryStringToEnum(const stw::scl::C_SclString & orc_Category,
                                                C_OscHalcDefDomain::E_Category & ore_Category);
   static void mh_GetAllNames(const std::vector<C_OscHalcDefStruct> & orc_Values,
                              std::vector<stw::scl::C_SclString> & orc_Names);
   static int32_t mh_CheckDuplicateNames(const stw::scl::C_SclString & orc_Section,
                                         const stw::scl::C_SclString & orc_DomainSingularName,
                                         std::vector<stw::scl::C_SclString> & orc_Names);
   static stw::scl::C_SclString mh_SafetyModeToString(
      const C_OscHalcDefBase::E_SafetyMode & ore_NodeDataPoolElementAccess);
   static int32_t mh_StringToSafetyMode(const stw::scl::C_SclString & orc_String,
                                        C_OscHalcDefBase::E_SafetyMode & ore_Type);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
