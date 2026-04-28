//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef OSCUTILS_HPP
#define OSCUTILS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclResourceStrings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///openSYDE utility functions
class C_OscUtils
{
public:
   static bool h_CheckValidCeName(const stw::scl::C_SclString & orc_Name,
                                  const bool oq_AutomaticCeStringAdaptation = false,
                                  const uint16_t ou16_MaxLength = 31U);
   static bool h_IsFloat64NearlyEqual(const float64_t & orf64_Float1, const float64_t & orf64_Float2);
   static bool h_IsFloat32NearlyEqual(const float32_t & orf32_Float1, const float32_t & orf32_Float2);
   static int32_t h_CreateFolderRecursively(const stw::scl::C_SclString & orc_Folder);
   static stw::scl::C_SclString h_NiceifyStringForFileName(const stw::scl::C_SclString & orc_String);
   static stw::scl::C_SclString h_NiceifyStringForCeComment(const stw::scl::C_SclString & orc_String);
   static bool h_CheckValidFileName(const stw::scl::C_SclString & orc_String);
   static bool h_CheckValidFilePath(const stw::scl::C_SclString & orc_String);
   static bool h_IsScalingActive(const float64_t of64_Factor, const float64_t of64_Offset);
   static float64_t h_GetValueScaled(const float64_t of64_Value, const float64_t of64_Factor,
                                     const float64_t of64_Offset, const bool oq_AllowRangeAdaptation = true);
   static float64_t h_GetValueUnscaled(const float64_t of64_Value, const float64_t of64_Factor,
                                       const float64_t of64_Offset);
   static stw::scl::C_SclString h_PosSerialNumberToString(const uint8_t * const opu8_SerialNumber);
   static stw::scl::C_SclString h_FsnSerialNumberToString(const uint8_t ou8_ManufacturerFormat,
                                                          const stw::scl::C_SclString & orc_RawSerialNumber);
   static void h_FileToString(const stw::scl::C_SclString & orc_FilePath, stw::scl::C_SclString & orc_OutputString);
   static void h_RangeCheckFloat(float64_t & orf64_Value);
   static stw::scl::C_SclString h_LoadString(const uint16_t ou16_StringIndex);
   static int32_t h_CopyFile(const stw::scl::C_SclString & orc_SourceFile, const stw::scl::C_SclString & orc_TargetFile,
                             stw::scl::C_SclString * const opc_ErrorPath = NULL,
                             stw::scl::C_SclString * const opc_ErrorMessage = NULL);
   static stw::scl::C_SclString h_GetCommandLineAsString(const int32_t os32_Argc, char_t * const * const oppcn_Argv);

   //Utilities for path place holder parsing
   static stw::scl::C_SclString h_MakeIndependentOfDbProjectPath(const stw::scl::C_SclString & orc_DbProjectPath,
                                                                 const stw::scl::C_SclString & orc_OsydeProjectPath,
                                                                 const stw::scl::C_SclString & orc_Path);
   static stw::scl::C_SclString h_ResolvePlaceholderVariables(const stw::scl::C_SclString & orc_Path,
                                                              const stw::scl::C_SclString & orc_OsydeProjectPath,
                                                              const stw::scl::C_SclString & orc_DbProjectPath = "");
   static stw::scl::C_SclString h_ResolveProjIndependentPlaceholderVariables(const stw::scl::C_SclString & orc_Path);
   static stw::scl::C_SclString h_ConcatPathIfNecessary(const stw::scl::C_SclString & orc_BaseDir,
                                                        const stw::scl::C_SclString & orc_RelativeOrAbsolutePath);

   //Unique string
   static stw::scl::C_SclString h_GetUniqueName(const std::map<stw::scl::C_SclString, bool> & orc_ExistingStrings,
                                                const stw::scl::C_SclString & orc_ProposedName,
                                                const uint32_t ou32_MaxCharLimit,
                                                const stw::scl::C_SclString & orc_SkipName = "");
   static void h_GetNumberAtStringEnd(const stw::scl::C_SclString & orc_ProposedName,
                                      stw::scl::C_SclString & orc_CutString, int32_t & ors32_Number);

   // Path variables
   static const stw::scl::C_SclString hc_PATH_VARIABLE_OPENSYDE_BIN;
   static const stw::scl::C_SclString hc_PATH_VARIABLE_OPENSYDE_PROJ;
   static const stw::scl::C_SclString hc_PATH_VARIABLE_DATABLOCK_PROJ;
   static const stw::scl::C_SclString hc_PATH_VARIABLE_USER_NAME;
   static const stw::scl::C_SclString hc_PATH_VARIABLE_COMPUTER_NAME;

private:
   static const float64_t mhf64_EPSILON;
   static stw::scl::C_SCLResourceStrings mhc_ResourceStrings;

   static void mh_GetBaseNameAndCurrentConflictNumberFromString(const stw::scl::C_SclString & orc_ConflictingValue,
                                                                const stw::scl::C_SclString & orc_SkipName,
                                                                stw::scl::C_SclString & orc_CutString,
                                                                int32_t & ors32_Number);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
