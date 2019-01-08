//-----------------------------------------------------------------------------
/*!
   \file
   \brief       resource string handling class

   ANSI C++ resource string handling class.
   Provides a platform independent mechanism for resource string handling.
   Individual strings are identified by their index.
   Most application will instance a singleton of the C_SCLResourceStrings class.

   The class uses some RAM as the whole string table is copied there and is sorted by the string's indexes
   to improve look-up performance.

   Loading string tables from file is not implemented yet but can easily be added.

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-200x
   license     use only under terms of contract / confidential

   created     22.07.2009  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef CSCLResourceStringsH
#define CSCLResourceStringsH

/* -- Includes ------------------------------------------------------------ */

#include <string>
#include "SCLDynamicArray.h"
#include "CSCLString.h"
#include "stwtypes.h"

namespace stw_scl
{

/* -- Defines ------------------------------------------------------------- */
//maybe this will be a part of a Borland library:
#ifndef SCL_PACKAGE
#ifdef __BORLANDC__
#define SCL_PACKAGE __declspec(package)
#else
#define SCL_PACKAGE
#endif
#endif

/* -- Types --------------------------------------------------------------- */

///resource string with index
class SCL_PACKAGE C_SCLResourceString
{
public:
   stw_types::uint16 u16_Index;
   C_SCLString c_Text;
};

///handler for ressource style strings
class SCL_PACKAGE C_SCLResourceStrings
{
private:
   SCLDynamicArray<C_SCLResourceString> mac_Strings;
   bool mq_Dirty;

public:
   C_SCLResourceStrings(void);

   C_SCLString LoadStr(const stw_types::uint16 ou16_Index);

   //add strings one by one ...
   //call ::SortByIndex after you are finished
   //Will be quite slow for bigger lists as the dynamic array needs to be resized all the time
   void AddString(const stw_types::uint16 ou16_Index, const C_SCLString & orc_Text);
   void Clear(void);

   //will implicitely call SortByIndex
   void SetStringTable(const C_SCLResourceString * const opc_Strings, const stw_types::uint16 ou16_NumStrings);

   void SortByIndex(void);
};

/* -- Global Variables ---------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------- */

/* -- Implementation ------------------------------------------------------ */


}

#endif


