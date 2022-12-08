//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       String class

   \class       stw::scl::C_SclString
   \brief       String class

   ANSI C++ string handling class.
   Aim: provide most of the functionality that Borland's AnsiString does
   while only using plain ANSI C++.

   What is NOT implemented (compared to Borland VCL AnsiString):
   - loading strings from ressource files (Windows specific)
   - Format
   - currency functions (not commonly used, VCL specific)
   - "wchar_t" Unicode functions
   - "MBCS" functions
   - "WideString" functions
   - AnsiCompareIC
   - FloatToStrF
   - AnsiPos
   - AnsiLastChar
   - vprintf and cat_vprintf are private as they have not been explicitely tested yet

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CSCLSTRINGH
#define CSCLSTRINGH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <string>
#include <cstdarg>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "stwtypes.hpp"
#include "C_SclDynamicArray.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace scl
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */
//maybe this will be a part of a Borland library:
#ifndef SCL_PACKAGE
#ifdef __BORLANDC__
#define SCL_PACKAGE __declspec(package)
#else
#define SCL_PACKAGE
#endif
#endif

//mark functions as deprecated if possible
#if (!defined(SCL_DEPRECATED_PRE)) && (!defined(SCL_DEPRECATED_POST))
#if __cplusplus >= 201402L //C++14 ?
#define SCL_DEPRECATED_PRE  [[deprecated]]
#define SCL_DEPRECATED_POST
#elif defined(__GNUC__)
#define SCL_DEPRECATED_PRE
#define SCL_DEPRECATED_POST __attribute__((deprecated))
#elif defined(_MSC_VER)
#define SCL_DEPRECATED_PRE __declspec(deprecated)
#define SCL_DEPRECATED_POST
#elif __BORLANDC__ >= 0x630 && (!defined(__clang__)) //>= XE but not Clang-based ?
#define SCL_DEPRECATED_PRE
#define SCL_DEPRECATED_POST [[deprecated]]
#else
//just informative anyway ...
#define SCL_DEPRECATED_PRE
#define SCL_DEPRECATED_POST
#endif
#endif

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///String wrapper class
class SCL_PACKAGE C_SclString
{
private:
   void m_ThrowIfOutOfRange(const int32_t os32_Index) const;
   static int64_t mh_StrTos64(const char_t * const opcn_String, const bool oq_Hex);
   static int32_t mh_StrTos32(const char_t * const opcn_String, const bool oq_Hex);

   std::string c_String; ///< actual string data wrapped by C_SclString

   static int32_t mh_GetRequiredPrintfSize(const char_t * const opcn_Format, va_list opv_Args);
   void m_SnPrintf(const int32_t os32_Size, const char_t * const opcn_Format, va_list opv_Args);
   void m_CatSnPrintf(const int32_t os32_Size, const char_t * const opcn_Format, va_list opv_Args);

public:
   static C_SclString StringOfChar(const char_t ocn_Char, const uint32_t ou32_Count);

   //deprecated function names for compatibility:
   //lint -e{1960,1916}  naming and signature to stay compatible with AnsiString;
   SCL_DEPRECATED_PRE C_SclString & sprintf(const char_t * const opcn_Format, ...) SCL_DEPRECATED_POST;
   //lint -e{1960,1916}  naming and signature to stay compatible with AnsiString;
   SCL_DEPRECATED_PRE int32_t printf(const char_t * const opcn_Format, ...) SCL_DEPRECATED_POST;

   //It is not a good idea to use printf/sprintf as function names as this can cause conflicts with some libraries.
   //the following functions are simply clean alternative names for printf/sprintf that should be used:
   //lint -e{1960,1916}  in this case variable number of arguments is what we want ...
   C_SclString & StringPrintFormatted(const char_t * const opcn_Format, ...);
   //lint -e{1960,1916}  in this case variable number of arguments is what we want ...
   int32_t PrintFormatted(const char_t * const opcn_Format, ...);

   //lint -e{1960,1916}  signature to stay compatible with AnsiString;
   C_SclString & cat_sprintf(const char_t * const opcn_Format, ...);
   //lint -e{1960,1916}  signature to stay compatible with AnsiString;
   int32_t cat_printf(const char_t * const opcn_Format, ...);

   //constructors:
   C_SclString(void);
   C_SclString(const C_SclString & orc_InitValue);
   C_SclString(const char_t * const opcn_InitValue, const uint32_t oun_Length);
   C_SclString(const int8_t os8_InitValue);
   C_SclString(const uint8_t ou8_InitValue);
   C_SclString(const float64_t of64_InitValue);
   C_SclString(const wchar_t * const opwcn_InitValue);
   C_SclString(wchar_t * const opwcn_InitValue);
   //PC-lint 9 reports 1036 when calling the constructor with a "charn" which is handled by the template constructor
   // So there is no ambiguity. False Positive.
   //lint -estring(1036, "*stw::scl::C_SclString::C_SclString(double)*")
   template <typename T> C_SclString(const T orc_Value);

   virtual ~C_SclString();

   //overloaded operators
   C_SclString & operator =(const C_SclString & orc_Source);
   C_SclString & operator +=(const C_SclString & orc_Source);

   char_t operator [](const uint32_t ou32_Index) const;
   char_t & operator [](const uint32_t ou32_Index);

   int32_t AnsiCompare(const C_SclString & orc_Source) const;
   int32_t AnsiCompareIc(const C_SclString & orc_Source) const; //ignore case

   //some functions are implemented here for potentially better performance
   //difference to AnsiString: returns a const pointer !
   //so the string can not directly be modified by the application !
   //Use operator [] instead !
   const char_t * c_str(void) const;
   const void * data(void) const;
   uint32_t Length(void) const;
   bool IsEmpty(void) const;

   // Modify string
   C_SclString & Insert(const C_SclString & orc_Source, const uint32_t ou32_Index);
   C_SclString & Delete(const uint32_t ou32_Index, const uint32_t ou32_Count);
   C_SclString & SetLength(const uint32_t ou32_NewLength);

   uint32_t Pos(const C_SclString & orc_SubString) const;
   uint32_t LastPos(const C_SclString & orc_SubString) const; //last position of string
   uint32_t LastDelimiter(const C_SclString & orc_Delimiters) const;

   C_SclString LowerCase(void) const;
   C_SclString UpperCase(void) const;
   C_SclString Trim(void) const;
   C_SclString TrimLeft(void) const;
   C_SclString TrimRight(void) const;
   C_SclString SubString(const uint32_t ou32_Index, const uint32_t ou32_Count) const;

   int32_t ToInt(void) const; //difference to AnsiString: throws, but not an EConvertError
   int32_t ToIntDef(const int32_t osn_Default) const;
   int64_t ToInt64(void) const;
   float64_t ToDouble(void) const; //difference to AnsiString: throws, but not an EConvertError
   //accepts "." and "," (is a little slow however)

   //get pointer to internally used std::string (to easily interface with classes based on it):
   const std::string * AsStdString(void) const;
   std::string * AsStdString(void);

   static C_SclString IntToStr(const char_t ocn_Value);
   template <typename T> static C_SclString IntToStr(const T orc_Value);
   static C_SclString IntToHex(const uint8_t ou8_Value, const uint32_t ou32_Digits);
   static C_SclString IntToHex(const int8_t os8_Value, const uint32_t ou32_Digits);
   template <typename T> static C_SclString IntToHex(const T orc_Value, const uint32_t ou32_Digits);

   static C_SclString FloatToStr(const float32_t of32_Value);
   static C_SclString FloatToStr(const float64_t of64_Value);
   static C_SclString FloatToStr(const float64_t of64_Value, const int32_t os32_Digits);

   void Tokenize(const C_SclString & orc_Delimiters, C_SclDynamicArray<C_SclString> & orc_TokenizedData) const;
};

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Constructor

   Covers all integer types.

   \param[in]  orc_Value    value to convert
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T> C_SclString::C_SclString(const T orc_Value)
{
   std::stringstream c_Stream;
   c_Stream << orc_Value;
   c_String = c_Stream.str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Convert number to string

   Can be used for integer types.

   \param[in]  orc_Value    value to convert

   \return
   Resulting string
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T> C_SclString C_SclString::IntToStr(const T orc_Value)
{
   const C_SclString c_Text(orc_Value);

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Convert number to hexadecimal string

   Compose string from integer value data.
   Does not insert a "0x" prefix before the data.

   Example:
   "IntToHex(0x123, 4)" will return 0123.

   \param[in]  orc_Value    value to convert
   \param[in]  ou32_Digits  number of digits to return (zeroes will be filled in from the left)

   \return
   Resulting string
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T> C_SclString C_SclString::IntToHex(const T orc_Value, const uint32_t ou32_Digits)
{
   C_SclString c_Text;

   std::stringstream c_Stream;
   c_Stream << &std::hex << std::setw(ou32_Digits) << std::setfill('0') << orc_Value;
   c_Text.c_String = c_Stream.str();

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------

extern bool SCL_PACKAGE operator == (const C_SclString & orc_Par1, const C_SclString & orc_Par2);
extern bool SCL_PACKAGE operator != (const C_SclString & orc_Par1, const C_SclString & orc_Par2);
extern bool SCL_PACKAGE operator < (const C_SclString & orc_Par1, const C_SclString & orc_Par2);
extern bool SCL_PACKAGE operator > (const C_SclString & orc_Par1, const C_SclString & orc_Par2);
extern bool SCL_PACKAGE operator <= (const C_SclString & orc_Par1, const C_SclString & orc_Par2);
extern bool SCL_PACKAGE operator >= (const C_SclString & orc_Par1, const C_SclString & orc_Par2);
extern C_SclString SCL_PACKAGE operator + (const C_SclString & orc_Par1, const C_SclString & orc_Par2);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}
#endif
