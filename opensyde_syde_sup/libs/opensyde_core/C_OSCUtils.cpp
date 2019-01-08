//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       OpenSYDE utility functions (implementation)

   OpenSYDE utility functions

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <cmath>
#include <cctype>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "TGLFile.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;

/* -- Module Global Constants ---------------------------------------------- */
const stw_types::float64 C_OSCUtils::mhf64_Epsilon = 1e-5;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   CheckValidCName

   Function adapted from KEFEX to openSYDE (KFXCheckValidCName)
   Check if tName follows C naming conventions:
   -> must not be empty
   -> only alphanumeric characters + "_"
   -> should not be longer than "ou16_MaxLength" characters


   \param[in]     orc_Name         symbol name to check
   \param[in]     ou16_MaxLength   permitted maximum identifier length

   \return
   true  -> OK
   false -> violation of rules

   \created     23.01.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
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
      if ((isalnum(cn_Char) == 0) && (cn_Char != '_')) //ANSI compliant check
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

//-----------------------------------------------------------------------------
/*!
   \brief   Compare to float64s for near equality

   \param[in] orf64_Float1 Float 1 to compare
   \param[in] orf64_Float2 Float 2 to compare

   \return
   true  Equal
   false Not equal

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCUtils::h_IsFloat64NearlyEqual(const float64 & orf64_Float1, const float64 & orf64_Float2)
{
   //From Marshall Cline's C++ FAQ Lite document
   return std::abs(orf64_Float1 - orf64_Float2) <= (C_OSCUtils::mhf64_Epsilon * std::abs(orf64_Float1));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Compare to float32s for near equality

   \param[in] orf32_Float1 Float 1 to compare
   \param[in] orf32_Float2 Float 2 to compare

   \return
   true  Equal
   false Not equal

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCUtils::h_IsFloat32NearlyEqual(const float32 & orf32_Float1, const float32 & orf32_Float2)
{
   //From Marshall Cline's C++ FAQ Lite document
   return std::abs(orf32_Float1 - orf32_Float2) <=
          (static_cast<float32>(C_OSCUtils::mhf64_Epsilon) * std::abs(orf32_Float1));
}

//-----------------------------------------------------------------------------
/*!
   \brief   create target folder (from bottom-up if required)

   Nice and simple logic grabbed from
    https://stackoverflow.com/questions/1530760/how-do-i-recursively-create-a-folder-in-win32

   \param[in]     orc_Folder         Path to create

   \return
   C_NO_ERR  folder created
   C_NOACT   could not create folder

   \created     11.04.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCUtils::h_CreateFolderRecursively(const C_SCLString & orc_Folder)
{
   size_t un_CharIndex = 0U;
   sint32 s32_Return = C_NO_ERR;

   std::string c_Path = orc_Folder.c_str();
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

//-----------------------------------------------------------------------------
/*!
   \brief   Replace special characters in string

   Aims:
   * convert into strings that can be used for file system folder names
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
   * . (unusual but permitted)
   * <space> (unusual but permitted)

   See https://en.wikipedia.org/wiki/Filename for a list of limitations.

   Special handling will also be applied to strings only containing "." or "..".
   These will be replaced by "dot" resp. "doubledot".

   As a result the length of the string might change.

   \param[in]     orc_String         Original string

   \return
   Niceified string

   \created     11.04.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_SCLString C_OSCUtils::h_NiceifyStringForFileName(const C_SCLString & orc_String)
{
   C_SCLString c_Result;

   //check fringe cases; "." and ".." have special meaning in most file systems
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
             (cn_Character != ' '))
         {
            c_Result += C_SCLString::IntToStr(cn_Character);
         }
         else
         {
            c_Result += cn_Character;
         }
      }
   }
   return c_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Replace non-C characters in string

   Aims:
   * convert a text string so that it can be used as a comment in C code
   * also prevent star-backslash combinations in order to not accidentally terminate the comment

   Strategy:
   * all non-printable C characters are replaced by "_" (this also will remove nl,cr,tab)
   * "*" is replaced by "_" if immediately followed by "/"
   * "\" is replaced by "_" if at the end of the string

   \param[in]     orc_String         Original string

   \return
   Niceified string

   \created     30.08.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
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
      else if ((cn_Character == '*') && (u32_Index < orc_String.Length()) &&
               (c_Result[u32_NextIndex] == '/'))
      {
         //prevent adding end of C comment
         c_Result[u32_Index] = '_';
      }
      else if ((cn_Character == '\\') && (u32_Index == orc_String.Length()))
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

//-----------------------------------------------------------------------------
/*!
   \brief   Check if scaling active

   \param[in] of64_Factor Factor
   \param[in] of64_Offset Offset

   \return
   True  Scaling active
   False Scaling inactive

   \created     15.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_OSCUtils::h_IsScalingActive(const float64 of64_Factor, const float64 of64_Offset)
{
   return ((C_OSCUtils::h_IsFloat64NearlyEqual(of64_Factor, 1.0) == false) ||
           (C_OSCUtils::h_IsFloat64NearlyEqual(of64_Offset, 0.0) == false));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Scales a value

   returns ((value * factor) + offset)

   \param[in]     of64_Value      Original unscaled value
   \param[in]     of64_Factor     Scaling factor
   \param[in]     of64_Offset     Scaling offset

   \return
   Scaled value

   \created     27.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
float64 C_OSCUtils::h_GetValueScaled(const float64 of64_Value, const float64 of64_Factor, const float64 of64_Offset)
{
   float64 f64_Result;

   f64_Result = of64_Value * of64_Factor;
   f64_Result += of64_Offset;

   return f64_Result;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Calculates a scaled value back to the unscaled value

   returns ((value - of64_Offset) - offset)

   \param[in]     of64_Value      Scaled value
   \param[in]     of64_Factor     Scaling factor
   \param[in]     of64_Offset     Scaling offset

   \return
   Origin value

   \created     29.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
float64 C_OSCUtils::h_GetValueUnscaled(const float64 of64_Value, const float64 of64_Factor, const float64 of64_Offset)
{
   float64 f64_Result;

   f64_Result = of64_Value - of64_Offset;
   f64_Result /= of64_Factor;

   return f64_Result;
}
