//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       String class

   \class       stw_scl::C_SCLString
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
#include "stwtypes.h"
#include "SCLDynamicArray.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_scl
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
#elif __BORLANDC__ >= 0x630 //>= XE ?
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
class SCL_PACKAGE C_SCLString
{
private:
   void m_ThrowIfOutOfRange(const stw_types::sint32 os32_Index) const;
   static stw_types::sint64 m_StrTos64(const stw_types::charn * const opcn_String, const bool oq_Hex);
   static stw_types::sint32 m_StrTos32(const stw_types::charn * const opcn_String, const bool oq_Hex);

   std::string c_String; ///< actual string data wrapped by C_SCLString

   static stw_types::sintn mh_GetRequiredPrintfSize(const stw_types::charn * const opcn_Format, va_list opv_Args);
   void m_SNPrintf(const stw_types::sintn osn_Size, const stw_types::charn * const opcn_Format, va_list opv_Args);
   void m_CatSNPrintf(const stw_types::sintn osn_Size, const stw_types::charn * const opcn_Format, va_list opv_Args);

public:
   static C_SCLString StringOfChar(const stw_types::charn ocn_Char, const stw_types::uint32 ou32_Count);

   //deprecated function names for compatibility:
   //lint -e{1960,1916}  naming and signature to stay compatible with AnsiString;
   SCL_DEPRECATED_PRE C_SCLString & sprintf(const stw_types::charn * const opcn_Format, ...) SCL_DEPRECATED_POST;
   //lint -e{1960,1916}  naming and signature to stay compatible with AnsiString;
   SCL_DEPRECATED_PRE stw_types::sintn printf(const stw_types::charn * const opcn_Format, ...) SCL_DEPRECATED_POST;

   //It is not a good idea to use printf/sprintf as function names as this can cause conflicts with some libraries.
   //the following functions are simply clean alternative names for printf/sprintf that should be used:
   //lint -e{1960,1916}  in this case variable number of arguments is what we want ...
   C_SCLString & StringPrintFormatted(const stw_types::charn * const opcn_Format, ...);
   //lint -e{1960,1916}  in this case variable number of arguments is what we want ...
   stw_types::sintn PrintFormatted(const stw_types::charn * const opcn_Format, ...);

   //lint -e{1960,1916}  signature to stay compatible with AnsiString;
   C_SCLString & cat_sprintf(const stw_types::charn * const opcn_Format, ...);
   //lint -e{1960,1916}  signature to stay compatible with AnsiString;
   stw_types::sintn cat_printf(const stw_types::charn * const opcn_Format, ...);

   static C_SCLString IntToHex(const stw_types::sintn osn_Value, const stw_types::uint32 ou32_Digits);
   static C_SCLString IntToHex(const stw_types::sint64 os64_Value, const stw_types::uint32 ou32_Digits);

   C_SCLString(void);
   C_SCLString(const stw_types::charn * const opcn_InitValue);
   C_SCLString(const C_SCLString & orc_InitValue);
   C_SCLString(const stw_types::charn * const opcn_InitValue, const stw_types::uintn oun_Length);
   C_SCLString(const stw_types::charn ocn_InitValue);
   C_SCLString(const stw_types::sint16 os16_InitValue);
   C_SCLString(const stw_types::uint16 ou16_InitValue);
   C_SCLString(const stw_types::sintn osn_InitValue);
   C_SCLString(const stw_types::uintn oun_InitValue);
   C_SCLString(const stw_types::sint32 os32_InitValue);
   C_SCLString(const stw_types::uint32 ou32_InitValue);
   C_SCLString(const stw_types::sint64 os64_InitValue);
   C_SCLString(const stw_types::uint64 ou64_InitValue);
   C_SCLString(const stw_types::float64 of64_InitValue);

   //There's no defined stw_types typedef for wchar_t yet, so use the plain type here:
   //Also the prefix is just a guess.
   C_SCLString(const wchar_t * const opwcn_InitValue);
   virtual ~C_SCLString();

   //overloaded operators
   C_SCLString & operator =(const C_SCLString & orc_Source);
   C_SCLString & operator +=(const C_SCLString & orc_Source);

   stw_types::charn operator [](const stw_types::sintn osn_Index) const;
   stw_types::charn & operator [](const stw_types::sintn osn_Index);

   stw_types::sintn AnsiCompare(const C_SCLString & orc_Source) const;
   stw_types::sintn AnsiCompareIC(const C_SCLString & orc_Source) const; //ignore case

   //some functions are implemented here for potentially better performance
   //difference to AnsiString: returns a const pointer !
   //so the string can not directly be modified by the application !
   //Use operator [] instead !
   const stw_types::charn * c_str(void) const;
   const void * data(void) const;
   stw_types::uint32 Length(void) const;
   bool IsEmpty(void) const;

   // Modify string
   C_SCLString & Insert(const C_SCLString & orc_Source, const stw_types::uint32 ou32_Index);
   C_SCLString & Delete(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_Count);
   C_SCLString & SetLength(const stw_types::uint32 ou32_NewLength);

   stw_types::uint32 Pos(const C_SCLString & orc_SubString) const;
   stw_types::uint32 LastPos(const C_SCLString & orc_SubString) const; //last position of string
   stw_types::uint32 LastDelimiter(const C_SCLString & orc_Delimiters) const;

   C_SCLString LowerCase(void) const;
   C_SCLString UpperCase(void) const;
   C_SCLString Trim(void) const;
   C_SCLString TrimLeft(void) const;
   C_SCLString TrimRight(void) const;
   C_SCLString SubString(const stw_types::uint32 ou32_Index, const stw_types::uint32 ou32_Count) const;

   stw_types::sintn  ToInt(void) const; //difference to AnsiString: throws, but not an EConvertError
   stw_types::sintn  ToIntDef(const stw_types::sintn osn_Default) const;
   stw_types::sint64 ToInt64(void) const;
   stw_types::float64 ToDouble(void) const; //difference to AnsiString: throws, but not an EConvertError
                                            //accepts "." and "," (is a little slow however)

   //get pointer to internally used std::string (to easily interface with classes based on it):
   const std::string * AsStdString(void) const;
   std::string * AsStdString(void);

   //some methods to replace global VCL utility functions:
   static C_SCLString IntToStr(const stw_types::sint64 os64_Value);
   static C_SCLString IntToStr(const stw_types::uint64 ou64_Value);
   static C_SCLString IntToStr(const stw_types::sint32 os32_Value);
   static C_SCLString IntToStr(const stw_types::uint32 ou32_Value);
   static C_SCLString IntToStr(const stw_types::sintn osn_Value);
   static C_SCLString IntToStr(const stw_types::uintn oun_Value);
   static C_SCLString FloatToStr(const stw_types::float32 of32_Value);
   static C_SCLString FloatToStr(const stw_types::float64 of64_Value);
   static C_SCLString FloatToStr(const stw_types::float64 of64_Value, const stw_types::sint32 os32_Digits);

   void Tokenize(const C_SCLString & orc_Delimiters, SCLDynamicArray<C_SCLString> & orc_TokenizedData) const;
};

extern bool SCL_PACKAGE operator == (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);
extern bool SCL_PACKAGE operator != (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);
extern bool SCL_PACKAGE operator < (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);
extern bool SCL_PACKAGE operator > (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);
extern bool SCL_PACKAGE operator <= (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);
extern bool SCL_PACKAGE operator >= (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);
extern C_SCLString SCL_PACKAGE operator + (const C_SCLString & orc_Par1, const C_SCLString & orc_Par2);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
