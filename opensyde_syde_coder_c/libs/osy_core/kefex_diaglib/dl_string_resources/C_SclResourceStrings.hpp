//----------------------------------------------------------------------------------------------------------------------
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

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SCLRESOURCESTRINGSHPP
#define C_SCLRESOURCESTRINGSHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <string>
#include "C_SclDynamicArray.hpp"
#include "C_SclString.hpp"
#include "stwtypes.hpp"

namespace stw
{
namespace scl
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///resource string with index
class C_SclResourceString
{
public:
   uint16_t u16_Index;
   C_SclString c_Text;
};

///handler for resource style strings
class C_SCLResourceStrings
{
private:
   C_SclDynamicArray<C_SclResourceString> mac_Strings;
   bool mq_Dirty;

public:
   C_SCLResourceStrings(void);

   C_SclString LoadStr(const uint16_t ou16_Index);

   //add strings one by one ...
   //call ::SortByIndex after you are finished
   //Will be quite slow for bigger lists as the dynamic array needs to be resized all the time
   void AddString(const uint16_t ou16_Index, const C_SclString & orc_Text);
   void Clear(void);

   //will implicitly call SortByIndex
   void SetStringTable(const C_SclResourceString * const opc_Strings, const uint16_t ou16_NumStrings);

   void SortByIndex(void);
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
}
}

#endif
