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

#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///openSYDE utility functions
class C_OSCUtils
{
public:
   static bool h_CheckValidCName(const stw_scl::C_SCLString & orc_Name, const stw_types::uint16 ou16_MaxLength = 31U);
   static bool h_IsFloat64NearlyEqual(const stw_types::float64 & orf64_Float1, const stw_types::float64 & orf64_Float2);
   static bool h_IsFloat32NearlyEqual(const stw_types::float32 & orf32_Float1, const stw_types::float32 & orf32_Float2);
   static stw_types::sint32 h_CreateFolderRecursively(const stw_scl::C_SCLString & orc_Folder);
   static stw_scl::C_SCLString h_NiceifyStringForFileName(const stw_scl::C_SCLString & orc_String);
   static stw_scl::C_SCLString h_NiceifyStringForCComment(const stw_scl::C_SCLString & orc_String);
   static bool h_CheckValidFileName(const stw_scl::C_SCLString & orc_String);
   static bool h_CheckValidFilePath(const stw_scl::C_SCLString & orc_String);

   static bool h_IsScalingActive(const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset);
   static stw_types::float64 h_GetValueScaled(const stw_types::float64 of64_Value, const stw_types::float64 of64_Factor,
                                              const stw_types::float64 of64_Offset);
   static stw_types::float64 h_GetValueUnscaled(const stw_types::float64 of64_Value,
                                                const stw_types::float64 of64_Factor,
                                                const stw_types::float64 of64_Offset);
   static stw_scl::C_SCLString h_SerialNumberToString(const stw_types::uint8 * const opu8_SerialNumber);
   static void h_FileToString(const stw_scl::C_SCLString & orc_FilePath, stw_scl::C_SCLString & orc_OutputString);

private:
   static const stw_types::float64 mhf64_Epsilon;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
