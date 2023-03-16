//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef OSCUTILS_H
#define OSCUTILS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

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

private:
   static const float64_t mhf64_EPSILON;
   static stw::scl::C_SCLResourceStrings mhc_ResourceStrings;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
