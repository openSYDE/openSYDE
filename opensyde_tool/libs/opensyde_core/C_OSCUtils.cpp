//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE utility functions (implementation)

   openSYDE utility functions

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cctype>
#include <fstream>
#include <iterator>
#include <cmath>
#include <limits>
#include <fstream>
#include <algorithm>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "TGLFile.h"
#include "CSCLResourceStrings.h"
#define STR_TABLE_INCLUDE //we really want the symbols from the DLStrings.h header
#include "DLStrings.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::float64 C_OSCUtils::mhf64_Epsilon = 1e-5;
stw_scl::C_SCLResourceStrings C_OSCUtils::mhc_ResourceStrings;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  CheckValidCName

   Function adapted from KEFEX to openSYDE (KFXCheckValidCName)
   Check if tName follows C naming conventions:
   -> must not be empty
   -> only alphanumeric characters + "_"
   -> should not be longer than "ou16_MaxLength" characters

   \param[in]  orc_Name          symbol name to check
   \param[in]  ou16_MaxLength    permitted maximum identifier length

   \return
   true  -> OK
   false -> violation of rules
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCUtils::h_CheckValidCName(const stw_scl::C_SCLString & orc_Name, const stw_types::uint16 ou16_MaxLength)
{
   uint32 u32_Index;
   charn cn_Char;

   if (orc_Name.Length() == 0)
   {
      return false;
   }

   // -> only alphanumeric characters + "_"
   for (u32_Index = 0; u32_Index < orc_Name.Length(); u32_Index++)
   {
      cn_Char = orc_Name.c_str()[u32_Index];
      if ((std::isalnum(cn_Char) == 0) && (cn_Char != '_')) //ANSI compliant check
      {
         return false;
      }
   }

   // -> should not be longer than ou16_MaxLength characters
   if (orc_Name.Length() > static_cast<uint32>(ou16_MaxLength))
   {
      return false;
   }

   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare to float64s for near equality

   \param[in]  orf64_Float1   Float 1 to compare
   \param[in]  orf64_Float2   Float 2 to compare

   \return
   true  Equal
   false Not equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCUtils::h_IsFloat64NearlyEqual(const float64 & orf64_Float1, const float64 & orf64_Float2)
{
   //From Marshall Cline's C++ FAQ Lite document
   return std::abs(orf64_Float1 - orf64_Float2) <= (C_OSCUtils::mhf64_Epsilon * std::abs(orf64_Float1));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare to float32s for near equality

   \param[in]  orf32_Float1   Float 1 to compare
   \param[in]  orf32_Float2   Float 2 to compare

   \return
   true  Equal
   false Not equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCUtils::h_IsFloat32NearlyEqual(const float32 & orf32_Float1, const float32 & orf32_Float2)
{
   //From Marshall Cline's C++ FAQ Lite document
   return std::abs(orf32_Float1 - orf32_Float2) <=
          (static_cast<float32>(C_OSCUtils::mhf64_Epsilon) * std::abs(orf32_Float1));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  create target folder (from bottom-up if required)

   Nice and simple logic grabbed from
    https://stackoverflow.com/questions/1530760/how-do-i-recursively-create-a-folder-in-win32

   \param[in]  orc_Folder  Path to create

   \return
   C_NO_ERR  folder created
   C_NOACT   could not create folder
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCUtils::h_CreateFolderRecursively(const C_SCLString & orc_Folder)
{
   //lint -e{8080} //using type expected by the library for compatibility
   size_t un_CharIndex = 0U;
   sint32 s32_Return = C_NO_ERR;

   const std::string c_Path = orc_Folder.c_str();

   do
   {
      std::string c_PartialPath;
      un_CharIndex = c_Path.find_first_of("\\/", un_CharIndex + 1);

      c_PartialPath = c_Path.substr(0, un_CharIndex);
      s32_Return = TGL_CreateDirectory(c_PartialPath.c_str());
      if (s32_Return != 0)
      {
         s32_Return = C_NOACT;
      }
   }
   while ((un_CharIndex != std::string::npos) && (s32_Return == C_NO_ERR));
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Replace special characters in string

   Aims:
   * convert into strings that can be used for file system folder names or file names
   * prevent accidentally rendering unique names identical
     (e.g.: "ITEM!§" and "ITEM%&" should not result in the same string)

   As different file system have different valid characters this function only keeps commonly permitted characters.

   All characters but the following will be replaced by their ASCII codes:
   * a..z
   * A..Z
   * 0..9
   * _
   * -
   * (
   * )
   * {
   * }
   * $
   * %
   * &
   * !
   * #
   * +
   * ,
   * ;
   * =
   * @
   * [
   * ]
   * ^
   * '
   * ~
   * . (unusual but permitted)
   * <space> (unusual but permitted)

   See https://en.wikipedia.org/wiki/Filename for a list of limitations.

   Special handling will also be applied to the strings "." and "..":
   These will be replaced by "dot" resp. "doubledot".
   Furthermore empty or blank strings (e.g. "", " ", "  ") will be replaced by "blank".

   As a result the length of the string might change.

   \param[in]  orc_String  Original string

   \return
   Niceified string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCUtils::h_NiceifyStringForFileName(const C_SCLString & orc_String)
{
   C_SCLString c_Result;
   bool q_Blank = true;

   //check fringe cases; "." and ".." have special meaning in most file systems and are no valid directory names
   if (orc_String == ".")
   {
      c_Result = "dot";
   }
   else if (orc_String == "..")
   {
      c_Result = "doubledot";
   }
   else
   {
      for (uint32 u32_Index = 0U; u32_Index < orc_String.Length(); u32_Index++)
      {
         const charn cn_Character = orc_String.c_str()[u32_Index];
         if ((std::isalnum(cn_Character) == 0) &&
             (cn_Character != '_') && (cn_Character != '-') && (cn_Character != '(') && (cn_Character != ')') &&
             (cn_Character != '{') && (cn_Character != '}') && (cn_Character != '$') && (cn_Character != '.') &&
             (cn_Character != ' ') && (cn_Character != '%') && (cn_Character != '&') && (cn_Character != '!') &&
             (cn_Character != '#') && (cn_Character != '+') && (cn_Character != ',') && (cn_Character != ';') &&
             (cn_Character != '=') && (cn_Character != '@') && (cn_Character != '[') && (cn_Character != ']') &&
             (cn_Character != '^') && (cn_Character != '\'') && (cn_Character != '~'))
         {
            c_Result += C_SCLString::IntToStr(cn_Character);
         }
         else
         {
            c_Result += cn_Character;

            if (cn_Character != ' ')
            {
               q_Blank = false;
            }
         }
      }
   }

   if (q_Blank == true)
   {
      c_Result = "blank";
   }

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Replace non-C characters in string

   Aims:
   * convert a text string so that it can be used as a comment in C code
   * also prevent star-backslash combinations in order to not accidentally terminate the comment

   Strategy:
   * all non-printable C characters are replaced by "_" (this also will remove nl,cr,tab)
   * "*" is replaced by "_" if immediately followed by "/"
   * "\" is replaced by "_" if at the end of the string

   \param[in]  orc_String  Original string

   \return
   Niceified string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCUtils::h_NiceifyStringForCComment(const C_SCLString & orc_String)
{
   C_SCLString c_Result = orc_String;

   for (uint32 u32_Index = 1U; u32_Index <= orc_String.Length(); u32_Index++)
   {
      const charn cn_Character = c_Result[u32_Index];
      const uint32 u32_NextIndex = u32_Index + 1U;

      if ((std::isprint(cn_Character) == 0) || (cn_Character == '@') || (cn_Character == '`'))
      {
         c_Result[u32_Index] = '_';
      }
      else if ((u32_Index < orc_String.Length()) && (cn_Character == '*') &&
               (c_Result[u32_NextIndex] == '/'))
      {
         //prevent adding end of C comment
         c_Result[u32_Index] = '_';
      }
      else if ((u32_Index == orc_String.Length()) && (cn_Character == '\\'))
      {
         //prevent continuing C++ comment
         c_Result[u32_Index] = '_';
      }
      else
      {
         // Nothing to do
      }
   }
   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if string is valid for a file name by comparing it with result of h_NiceifyStringForFileName.

   See description of h_NiceifyStringForFileName. If the string has at least one character which would be replaced
   by h_NiceifyStringForFileName, the string is invalid and therefore the function returns false.

   For checking whole paths use h_CheckValidFileName, because here slashes and colons are not allowed.

   \param[in]  orc_String  File path name

   \retval   true    The string is valid
   \retval   false   The string is not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCUtils::h_CheckValidFileName(const C_SCLString & orc_String)
{
   bool q_Return = true;
   const C_SCLString c_Temp = h_NiceifyStringForFileName(orc_String);

   if (c_Temp != orc_String)
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if string is valid for file or directory paths.

   Splits the provided string at '/' and '\'and uses h_CheckValidFileName for these sub-strings.
   If the sub-string is a double dot ".." it is skipped because of its special meaning (directory up) in path context
   If the provided string starts with a drive name (C:) or with a single dot "."
   these sub-strings are skipped from file name check as they are valid in their special context.

   Empty paths are handled invalid because an empty file path is not valid for file saving.
   Same rule applies if the path only contains (back-)slashes.
   Multiple slashes in direct sequence are handled as valid if there exists at least one further character.

   \param[in]  orc_String  File path string

   \retval   true    The string is valid for file or directory path
   \retval   false   The string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCUtils::h_CheckValidFilePath(const C_SCLString & orc_String)
{
   bool q_Return = true;

   if (orc_String.IsEmpty() == true)
   {
      q_Return = false;
   }
   else
   {
      bool q_AtLeastOneOtherChar = false;
      SCLDynamicArray<C_SCLString> c_SplitStrings;
      orc_String.Tokenize("\\/", c_SplitStrings);

      for (sint32 s32_Index = 0U; (s32_Index < c_SplitStrings.GetLength()) && (q_Return == true); s32_Index++)
      {
         const C_SCLString & rc_Substring = c_SplitStrings[s32_Index];

         // empty sub-strings are okay here, because they result from two consecutive (back-)slashes or from
         // trailing/leading(back-)slash
         if (rc_Substring.IsEmpty() == false)
         {
            q_AtLeastOneOtherChar = true;

            // skip ".." as it has special meaning in paths but is not allowed for names
            if (rc_Substring != "..")
            {
               // skip if complete string starts with drive name or "."
               if ((s32_Index > 0) || (rc_Substring != ".")) /* . at beginning is allowed */
               {
                  if ((rc_Substring.Length() != 2U) || (rc_Substring.SubString(2, 1) != ':')) /* drive names e.g. C: */
                  {
                     q_Return = h_CheckValidFileName(rc_Substring);
                  }
               }
            }
         }
      }

      // only (back-)slashes found
      if (q_AtLeastOneOtherChar == false)
      {
         q_Return = false;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if scaling active

   \param[in]  of64_Factor    Factor
   \param[in]  of64_Offset    Offset

   \return
   True  Scaling active
   False Scaling inactive
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCUtils::h_IsScalingActive(const float64 of64_Factor, const float64 of64_Offset)
{
   bool q_Return = (C_OSCUtils::h_IsFloat64NearlyEqual(of64_Factor, 1.0) == false);

   q_Return = (q_Return || (C_OSCUtils::h_IsFloat64NearlyEqual(of64_Offset, 0.0) == false));

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Scales a value

   returns ((value * factor) + offset)

   \param[in]  of64_Value                 Original unscaled value
   \param[in]  of64_Factor                Scaling factor
   \param[in]  of64_Offset                Scaling offset
   \param[in]  oq_AllowRangeAdaptation    Allow range adaptation

   \return
   Scaled value
*/
//----------------------------------------------------------------------------------------------------------------------
float64 C_OSCUtils::h_GetValueScaled(const float64 of64_Value, const float64 of64_Factor, const float64 of64_Offset,
                                     const bool oq_AllowRangeAdaptation)
{
   float64 f64_Result;

   f64_Result = of64_Value * of64_Factor;
   f64_Result += of64_Offset;

   if (oq_AllowRangeAdaptation == true)
   {
      h_RangeCheckFloat(f64_Result);
   }

   return f64_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates a scaled value back to the unscaled value

   returns ((value - offset) / factor)

   \param[in]  of64_Value     Scaled value
   \param[in]  of64_Factor    Scaling factor
   \param[in]  of64_Offset    Scaling offset

   \return
   Origin value
*/
//----------------------------------------------------------------------------------------------------------------------
float64 C_OSCUtils::h_GetValueUnscaled(const float64 of64_Value, const float64 of64_Factor, const float64 of64_Offset)
{
   float64 f64_Result;

   f64_Result = of64_Value - of64_Offset;
   f64_Result /= of64_Factor;

   return f64_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert POS serial number array to string

   Converting the POS (Plain Old Serial) STW format in form of 6 BCD coded bytes to a string.

   Support of two serial number variants of the formats:
      1: format up to and including 2019. E.g: 05.123456.1001
      2: format from 2020. E.g: 200012345678

   \param[in]  opu8_SerialNumber    Pointer to first of six serial number array elements

   \return
   serial number string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCUtils::h_PosSerialNumberToString(const uint8 * const opu8_SerialNumber)
{
   C_SCLString c_Result;

   if (opu8_SerialNumber != NULL)
   {
      if (opu8_SerialNumber[0] < static_cast<uint8>(0x20))
      {
         //format up to and including 2019. E.g: 05.123456.1001
         c_Result.PrintFormatted("%02X.%02X%02X%02X.%02X%02X",
                                 opu8_SerialNumber[0], opu8_SerialNumber[1], opu8_SerialNumber[2], opu8_SerialNumber[3],
                                 opu8_SerialNumber[4], opu8_SerialNumber[5]);
      }
      else
      {
         //format from 2020. E.g: 200012345678
         c_Result.PrintFormatted("%02X%02X%02X%02X%02X%02X",
                                 opu8_SerialNumber[0], opu8_SerialNumber[1], opu8_SerialNumber[2], opu8_SerialNumber[3],
                                 opu8_SerialNumber[4], opu8_SerialNumber[5]);
      }
   }

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert FSN serial number raw string to a formatted string

   Converting the FSN (Flexible Serial Number) format in form of the defined ou8_ManufacturerFormat
   orc_RawSerialNumber must have at least one byte and the maximum length is 29 byte

   \param[in]  ou8_ManufacturerFormat    Manufacturer format which defines the type of serial number:
                                         0:      STW POS format
                                         1..255: reserved
   \param[in]  orc_RawSerialNumber       Unedited raw serial number

   \retval   string        formatted serial number string
   \retval   empty string  if length of orc_RawSerialNumber does not match the expectations
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCUtils::h_FsnSerialNumberToString(const stw_types::uint8 ou8_ManufacturerFormat,
                                                  const stw_scl::C_SCLString & orc_RawSerialNumber)
{
   C_SCLString c_Result;

   if ((orc_RawSerialNumber.Length() > 0) &&
       (orc_RawSerialNumber.Length() <= 29))
   {
      if (ou8_ManufacturerFormat == 0U)
      {
         // Must match exactly
         if (orc_RawSerialNumber.Length() == 6)
         {
            // STW POS format
            //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?&
            // anyway
            c_Result = C_OSCUtils::h_PosSerialNumberToString(
               reinterpret_cast<const uint8 *>(orc_RawSerialNumber.c_str()));
         }
      }
      else
      {
         // No concrete formats defined yet
         c_Result = orc_RawSerialNumber;
      }
   }

   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy file to string

   \param[in]   orc_FilePath        File to copy
   \param[out]  orc_OutputString    File content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCUtils::h_FileToString(const C_SCLString & orc_FilePath, C_SCLString & orc_OutputString)
{
   std::string c_Input;
   {
      //Read
      std::ifstream c_File;

      c_File.open(orc_FilePath.c_str(), std::ifstream::in);
      if (c_File.is_open())
      {
         c_File.seekg(0LL, std::ios::end);
         c_Input.reserve(static_cast<stw_types::uint32>(c_File.tellg()));
         c_File.seekg(0LL, std::ios::beg);

         c_Input.assign(static_cast<std::istreambuf_iterator<stw_types::charn> >(c_File),
                        std::istreambuf_iterator<stw_types::charn>());
         c_File.close();
      }
   }
   //Copy to output
   orc_OutputString = c_Input.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Range check float

   \param[out]  orf64_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCUtils::h_RangeCheckFloat(float64 & orf64_Value)
{
   orf64_Value = std::min(orf64_Value, std::numeric_limits<float64>::max());
   orf64_Value = std::max(orf64_Value, -std::numeric_limits<float64>::max());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   load resource string

   Load resource strings from resource table.
   We use a string table using the C_SCLResourceStrings class
    and fill a singleton of it with the application strings.

   Note: this is only useful for application using an numeric-index-based localization approach.
   This is for example not compatible with string-index-based approaches like gettext.

   \param[in]    ou16_StringIndex     Index of string

   \return
   string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCUtils::h_LoadString(const uint16 ou16_StringIndex)
{
   static bool hq_Initialized = false;

   if (hq_Initialized == false)
   {
      mhc_ResourceStrings.SetStringTable(gac_DIAG_LIB_RESOURCE_STRINGS, gu16_DIAGLIB_NR_RES_STRNGS);
      hq_Initialized = true;
   }

   return mhc_ResourceStrings.LoadStr(ou16_StringIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copies a file from source to target folder (internal function).

   Assumptions:
   * read permission of source folder
   * write permission of target folder

   \param[in]  orc_SourceFile            source file (full path required)
   \param[in]  orc_TargetFile            target file (        -"-       )
   \param[out] opc_ErrorPath             if != NULL and the function fails:
                                          file path (source or target) that caused the problem
   \param[out] opc_ErrorMessage          if != NULL and the function fails:
                                          error message that caused the problem

   \return
   C_NO_ERR    success
   C_RD_WR     read/write error (see log file for details)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCUtils::h_CopyFile(const C_SCLString & orc_SourceFile, const C_SCLString & orc_TargetFile,
                              C_SCLString * const opc_ErrorPath, C_SCLString * const opc_ErrorMessage)
{
   sint32 s32_Return = C_NO_ERR;
   C_SCLString c_ErrorMessage = "";

   std::fstream c_Input(orc_SourceFile.c_str(), std::fstream::in | std::fstream::binary);
   if (c_Input.fail() == true)
   {
      c_ErrorMessage = "Could not read \"" + orc_SourceFile + "\".";
      osc_write_log_error("Copying file", c_ErrorMessage);
      s32_Return = C_RD_WR;
      if (opc_ErrorPath != NULL)
      {
         *opc_ErrorPath = orc_SourceFile;
      }
   }
   else
   {
      c_Input << &std::noskipws;

      const std::istream_iterator<uint8> c_Begin(c_Input);
      const std::istream_iterator<uint8> c_End;

      std::fstream c_Output(orc_TargetFile.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
      if (c_Output.fail() == true)
      {
         c_ErrorMessage = "Could not write \"" + orc_TargetFile + "\".";
         osc_write_log_error("Copying file", c_ErrorMessage);
         s32_Return = C_RD_WR;
         if (opc_ErrorPath != NULL)
         {
            *opc_ErrorPath = orc_TargetFile;
         }
      }
      else
      {
         const std::ostream_iterator<uint8> c_Begin2(c_Output);
         try
         {
            std::copy(c_Begin, c_End, c_Begin2);
         }
         catch (...)
         {
            c_ErrorMessage = "Could not write stream of \"" + orc_TargetFile + "\".";
            osc_write_log_error("Copying file", c_ErrorMessage);
            s32_Return = C_RD_WR;
            if (opc_ErrorPath != NULL)
            {
               *opc_ErrorPath = orc_TargetFile;
            }
         }
      }
   }

   if ((s32_Return != C_NO_ERR) &&
       (opc_ErrorMessage != NULL))
   {
      *opc_ErrorMessage = c_ErrorMessage;
   }

   return s32_Return;
}
