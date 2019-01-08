//-----------------------------------------------------------------------------
/*!
   \file
   \brief       string list class

   \class       stw_scl::C_SCLStringList
   \brief       string list class
   
   ANSI C++ string list class.
   Aim: provide most of the functionality that Borland's TStringList does
    while only using ANSI C++.
   So cf. the documentation of the VCL TStringList for details on most API functions.

   The C_SCLString is used as a base (-> no unicode).

   What is NOT implemented (compared to Borland VCL TStringList):
   - OnChange and OnChanging callbacks
   - implicit sorting (explicit function available)
   - Everything associated with TObject
   - stream-based functions

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-200x
   license     use only under terms of contract / confidential

   created     18.06.2009  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef CSCLSTRINGLISTH
#define CSCLSTRINGLISTH

/* -- Includes ------------------------------------------------------------- */
#include "SCLDynamicArray.h"
#include "CSCLString.h"
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_scl
{
//maybe this will be a part of a Borland library:
#ifndef SCL_PACKAGE
#ifdef __BORLANDC__
#define SCL_PACKAGE __declspec(package)
#else
#define SCL_PACKAGE
#endif
#endif

/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
///Container class for lists of strings
class SCL_PACKAGE C_SCLStringList
{
private:
public:
   //from VCL:
   stw_types::uint32 Add(const C_SCLString & orc_String);
   void Append(const C_SCLString & orc_String);

   void Clear(void);
   void Delete(const stw_types::uint32 ou32_Index);
   void Exchange(const stw_types::uint32 ou32_Index1, const stw_types::uint32 ou32_Index2);
   void Insert(const stw_types::uint32 ou32_Index, const C_SCLString & orc_String);
   stw_types::sint32 IndexOf(const C_SCLString & orc_String);

   C_SCLString GetText(void) const;
   stw_types::uint32 GetCount(void) const;

   void LoadFromFile(const C_SCLString & orc_FileName);
   void SaveToFile(const C_SCLString & orc_FileName);

   stw_types::sint32 IndexOfName(const C_SCLString & orc_Name) const;
   C_SCLString ValueFromIndex(const stw_types::uint32 ou32_Index) const;
   C_SCLString Values(const C_SCLString & orc_Key) const;
   void AddStrings(const C_SCLStringList * const opc_Strings);

   void Sort(void);

   SCLDynamicArray<C_SCLString> Strings; ///< actual strings
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
