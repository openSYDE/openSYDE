//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for system filers (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMFILERUTIL_HPP
#define C_OSCSYSTEMFILERUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXmlParser.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscNodeCodeExportSettings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemFilerUtil
{
public:
   static stw::scl::C_SclString h_BusTypeEnumToString(const C_OscSystemBus::E_Type oe_Type);
   static int32_t h_BusTypeStringToEnum(const stw::scl::C_SclString & orc_Type, C_OscSystemBus::E_Type & ore_Type);
   static int32_t h_GetParserForExistingFile(C_OscXmlParser & orc_FileXmlParser, const stw::scl::C_SclString & orc_Path,
                                             const stw::scl::C_SclString & orc_RootNode);
   static int32_t h_GetParserForNewFile(C_OscXmlParser & orc_FileXmlParser, const stw::scl::C_SclString & orc_Path,
                                        const stw::scl::C_SclString & orc_RootNode);
   static int32_t h_CreateFolder(const stw::scl::C_SclString & orc_Path);
   static stw::scl::C_SclString h_PrepareItemNameForFileName(const stw::scl::C_SclString & orc_ItemName);
   static stw::scl::C_SclString h_CombinePaths(const stw::scl::C_SclString & orc_BasePathName,
                                               const stw::scl::C_SclString & orc_SubFolderFileName);
   static int32_t h_SaveStringToFile(const stw::scl::C_SclString & orc_CompleteFileAsString,
                                     const stw::scl::C_SclString & orc_CompleteFilePath,
                                     const stw::scl::C_SclString & orc_LogHeading);
   static void h_AdaptProjectPathToSystemDefinition(const stw::scl::C_SclString & orc_ProjectPath,
                                                    stw::scl::C_SclString & orc_SystemDefintionPath);
   static void h_AdaptProjectPathToSystemViews(const stw::scl::C_SclString & orc_ProjectPath,
                                               stw::scl::C_SclString & orc_SystemViewsPath);
   static stw::scl::C_SclString h_CodeExportScalingTypeToString(const C_OscNodeCodeExportSettings::E_Scaling &
                                                                ore_Scaling);
   static int32_t h_StringToCodeExportScalingType(const stw::scl::C_SclString & orc_String,
                                                  C_OscNodeCodeExportSettings::E_Scaling & ore_Scaling);
   static int32_t h_CheckVersion(C_OscXmlParserBase & orc_XmlParser, const uint16_t ou16_ExpectedFileVersion,
                                 const stw::scl::C_SclString & orc_TagName, const stw::scl::C_SclString & orc_UseCase);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
