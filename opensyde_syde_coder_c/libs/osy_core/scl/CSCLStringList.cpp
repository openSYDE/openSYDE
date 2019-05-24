//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       string list class

   ANSI C++ string list class.
   For details cf. documentation in .h file.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstdio>
#include <cctype> //for toupper
#include <climits>
#include <cstring>

#include "CSCLStringList.h"
#include "stwtypes.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_scl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Append one string

   Append one string at the end of the list

   \param[in]  orc_String   string to append

   \return
   index of new string
*/
//-----------------------------------------------------------------------------
uint32 C_SCLStringList::Add(const C_SCLString & orc_String)
{
   Strings.IncLength();
   Strings[Strings.GetHigh()] = orc_String;
   return static_cast<uint32>(Strings.GetHigh());
}

//-----------------------------------------------------------------------------
/*! \brief   Append one string

   Append one string at the end of the list.
   Functionally identical to C_SCLStringList::Add but without the return value.

   \param[in]  orc_String   string to append
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::Append(const C_SCLString & orc_String)
{
   this->Add(orc_String);
}

//-----------------------------------------------------------------------------
/*! \brief   Clear all strings

   Empty the string list.
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::Clear(void)
{
   Strings.SetLength(0);
}

//-----------------------------------------------------------------------------
/*! \brief   Delete one string

   Delete one string from the string list.
   Will throw if the index is invalid.

   \param[in]  ou32_Index   index of string to remove (0 = first string)
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::Delete(const uint32 ou32_Index)
{
   Strings.Delete(static_cast<sint32>(ou32_Index));
}

//-----------------------------------------------------------------------------
/*! \brief   Swap the contents of two strings

   Swap the tests of two strings.
   Will throw if the index is invalid.
   If one of the indexes is out of range the function has undefined bahavior.

   \param[in]  ou32_Index1   index of first string to swap (0 = first string in list)
   \param[in]  ou32_Index2   index of second string to swap (0 = first string in list)
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::Exchange(const uint32 ou32_Index1, const uint32 ou32_Index2)
{
   C_SCLString c_Temp;

   c_Temp = Strings[static_cast<sint32>(ou32_Index2)];
   Strings[static_cast<sint32>(ou32_Index2)] = Strings[static_cast<sint32>(ou32_Index1)];
   Strings[static_cast<sint32>(ou32_Index1)] = c_Temp;
}

//-----------------------------------------------------------------------------
/*! \brief   Insert string into list

   Insert one string into the string list at a defined position.

   \param[in]  ou32_Index   index before which to insert the new string (0 = first string in list)
   \param[in]  orc_String   string to insert
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::Insert(const uint32 ou32_Index, const C_SCLString & orc_String)
{
   Strings.Insert(static_cast<sint32>(ou32_Index), orc_String);
}

//-----------------------------------------------------------------------------
/*! \brief   Find index of string within list

   Returns the index of a specified string within the string list.
   The comparison ignores upper/lower case.

   \param[in]  orc_String   string to find

   \return  ou32_Index   index of string (0 = first string in list; -1 = string not found)
*/
//-----------------------------------------------------------------------------
sint32 C_SCLStringList::IndexOf(const C_SCLString & orc_String)
{
   sint32 s32_Index;

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      if (Strings[s32_Index].AnsiCompareIC(orc_String) == 0)
      {
         return s32_Index;
      }
   }
   return -1;
}

//-----------------------------------------------------------------------------
/*! \brief   Return all strings as one concatenated string

   Adds all strings to one single string.
   "\r\n" is inserted after each string (also after the last string).

   \return  concatenated string
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLStringList::GetText(void) const
{
   C_SCLString c_Text;
   sint32 s32_Index;

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      c_Text += (Strings[s32_Index] + "\r\n");
   }
   return c_Text;
}

//-----------------------------------------------------------------------------
/*! \brief   Get number of string in list

   Return number of strings contained in string list.

   \return  number of strings
*/
//-----------------------------------------------------------------------------
uint32 C_SCLStringList::GetCount(void) const
{
   return static_cast<uint32>(Strings.GetLength());
}

//-----------------------------------------------------------------------------
/*! \brief   Load strings from file

   Load strings from an ASCII file.
   Will replace any existing strings.

   Will throw if:
   - the file does not exist
   - the memory to load the file cannot be allocated
   - the file cannot be read from

   \param[in]     orc_FileName     path to file
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::LoadFromFile(const C_SCLString & orc_FileName)
{
   std::FILE * pt_File;
   uintn un_FileSize;
   charn * pcn_Buffer;
   size_t un_Size;
   uintn un_Index;
   uint32 u32_NumStrings;
   uint32 u32_Line;
   sintn sn_Len;

   Strings.SetLength(0);
   pt_File = std::fopen(orc_FileName.c_str(), "rb");
   if (pt_File == NULL)
   {
      throw ("C_SCLStringList::LoadFromFile: file not found");
   }

   //read the whole thing into one array:
   (void)std::fseek(pt_File, 0, SEEK_END);
   un_FileSize = std::ftell(pt_File);
   (void)std::fseek(pt_File, 0, SEEK_SET);

   try
   {
      pcn_Buffer = new charn[un_FileSize + 1U]; //+ 1: put a terminating \0
   }
   catch (...)
   {
      throw ("C_SCLStringList::LoadFromFile: could not allocate buffer for file");
   }
   pcn_Buffer[un_FileSize] = '\0';

   un_Size = std::fread(pcn_Buffer, 1U, un_FileSize, pt_File);
   if (un_Size != un_FileSize)
   {
      delete[] pcn_Buffer;
      throw ("C_SCLStringList::LoadFromFile: could not read file");
   }

   (void)std::fclose(pt_File);

   //first detect number of lines then set StringList length
   //improves speed significantly over using ::Add for each line directly
   u32_NumStrings = 0U;

   un_Index = 0U;
   while (un_Index < un_FileSize)
   {
      if ((pcn_Buffer[un_Index] == '\r') || (pcn_Buffer[un_Index] == '\n'))
      {
         pcn_Buffer[un_Index] = '\0';
         //maybe there is a subsequent \n
         if (pcn_Buffer[un_Index + 1U] == '\n')
         {
            un_Index++;
            pcn_Buffer[un_Index] = '\0';
         }
         u32_NumStrings++;
      }
      un_Index++;
   }
   //maybe the last line is not terminated ?
   if ((un_Index > 0U) && (pcn_Buffer[un_Index - 1U] != '\0'))
   {
      u32_NumStrings++;
   }

   //split into strings:
   try
   {
      Strings.SetLength(u32_NumStrings);
   }
   catch (...)
   {
      delete[] pcn_Buffer;
      throw ("C_SCLStringList::LoadFromFile: could not allocate buffer for file");
   }
   un_Index = 0U;

   for (u32_Line = 0U; u32_Line < u32_NumStrings; u32_Line++)
   {
      //using the std::string directly improves performance significantly
      sn_Len = strlen(&pcn_Buffer[un_Index]);
      Strings[u32_Line].AsStdString()->assign(&pcn_Buffer[un_Index], sn_Len); //assign with setting length is quite fast
      un_Index += static_cast<uintn>(sn_Len + 1);

      //maybe there is a subsequent \n
      if ((un_Index < un_FileSize) && (pcn_Buffer[un_Index] == '\0'))
      {
         un_Index++;
      }
   }

   delete[] pcn_Buffer;
}

//-----------------------------------------------------------------------------
/*! \brief   Save strings to file

   Save the contents of the string list to a file.
   An existing file will be overwritten.
   "\r\n" will be appended after each line (also after the last line).

   Will throw if:
   - the file cannot be created
   - the file cannot be written to

   \param[in]     orc_FileName     path to file
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::SaveToFile(const C_SCLString & orc_FileName)
{
   sint32 s32_Line;
   uint32 u32_NumWritten;

   std::FILE * pt_File;

   pt_File = std::fopen(orc_FileName.c_str(), "wb");
   if (pt_File == NULL)
   {
      throw ("C_SCLStringList::SaveToFile: could not create file");
   }

   for (s32_Line = 0; s32_Line < Strings.GetLength(); s32_Line++)
   {
      u32_NumWritten = std::fwrite(Strings[s32_Line].c_str(), 1U, Strings[s32_Line].Length(), pt_File);
      if (u32_NumWritten != Strings[s32_Line].Length())
      {
         (void)std::fclose(pt_File);
         throw ("C_SCLStringList::SaveToFile: could not write to file");
      }
      u32_NumWritten = std::fwrite("\r\n", 1U, 2U, pt_File);
      if (u32_NumWritten != 2U)
      {
         (void)std::fclose(pt_File);
         throw ("C_SCLStringList::SaveToFile: could not write to file");
      }
   }
   (void)std::fclose(pt_File);
}

//-----------------------------------------------------------------------------
/*! \brief   Returns the value of one key=value pair

   Tries to find a specified "key=" at the beginning of a string contained in the list.
   Then returns the rest of the line after the "=".
   If "key=" is contained more than once the first occurrence will be detected.
   The comparison is done independent of upper/lower case.

   Example:
   Assuming a line contains the text "foo=bar" a call to "Values("foo");" will return "bar".

   \param[in]     orc_Key    key to search for

   \return
   if "key=" is found:  "value" after the "="
   else:                empty string
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLStringList::Values(const C_SCLString & orc_Key) const
{
   sint32 s32_Index;
   C_SCLString c_Help = "";
   C_SCLString c_Search = orc_Key.UpperCase() + "=";

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      //performance boost: first check only first character:
      if (static_cast<charn>(std::toupper(Strings[s32_Index].c_str()[0])) == (c_Search.c_str()[0]))
      {
         if (Strings[s32_Index].UpperCase().Pos(c_Search) == 1U) //key + "="
         {
            c_Help = Strings[s32_Index].SubString(c_Search.Length() + 1, INT_MAX);
            break;
         }
      }
   }
   return c_Help;
}

//-----------------------------------------------------------------------------
/*! \brief   Finds the first line containg a name + "="

   Returns the index of the first line containing a specified name with an added equals character.
   The comparison is done independent of upper/lower case.
   The "=" may be preceeded by blanks.

   \param[in]     orc_Name    name to search foe

   \return
   if "name=" is found:  index of string (0 = first string in list)
   else:                 -1
*/
//-----------------------------------------------------------------------------
sint32 C_SCLStringList::IndexOfName(const C_SCLString & orc_Name) const
{
   bool q_Found = false;
   sint32 s32_Index;
   C_SCLString c_Search = orc_Name.UpperCase();
   C_SCLString c_Remainder;
   uint32 u32_Pos;

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      u32_Pos = Strings[s32_Index].UpperCase().Pos(c_Search);
      if (u32_Pos == 1U)
      {
         //there must be a subsequent "=" (may be preceeded by blanks)
         c_Remainder = Strings[s32_Index].SubString(orc_Name.Length() + 1, Strings[s32_Index].Length()).TrimLeft();
         if (c_Remainder.c_str()[0] == '=')
         {
            q_Found = true;
            break;
         }
      }
   }
   if (q_Found == false)
   {
      s32_Index = -1;
   }

   return s32_Index;
}

//-----------------------------------------------------------------------------
/*! \brief   Returns the part of a string after a "="

   Returns the part of one string after the equels ("=") chraracter.

   \param[in]     ou32_Index   index of string (0 = first string in list)

   \return
   if the string contains "=":  part of string after "="
   else:                        empty string
*/
//-----------------------------------------------------------------------------
C_SCLString C_SCLStringList::ValueFromIndex(const uint32 ou32_Index) const
{
   uint32 u32_Pos;
   C_SCLString c_Help;

   u32_Pos = Strings[ou32_Index].Pos("=");
   if (u32_Pos != 0U)
   {
      c_Help = Strings[ou32_Index].SubString(u32_Pos + 1U, INT_MAX);
   }
   else
   {
      c_Help = "";
   }
   return c_Help;
}

//-----------------------------------------------------------------------------
/*! \brief   Add multiple strings to string list

   Adds then content of another string list to this string list.

   \param[in]     opc_Strings   string list to add (pointer is not checked for validity)
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::AddStrings(const C_SCLStringList * const opc_Strings)
{
   sint32 s32_Index;
   sint32 s32_OldIndex;

   s32_OldIndex = Strings.GetLength();
   Strings.IncLength(opc_Strings->Strings.GetLength()); //first increase length (faster than adding one by one)
   for (s32_Index = 0; s32_Index < opc_Strings->Strings.GetLength(); s32_Index++)
   {
      Strings[s32_Index + s32_OldIndex] = opc_Strings->Strings[s32_Index];
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Sort strings in list

   Sorts by ASCII values of the individual characters.
*/
//-----------------------------------------------------------------------------
void C_SCLStringList::Sort(void)
{
   sint32 s32_Index;
   sint32 s32_Position;
   C_SCLString t_Key;

   for (s32_Index = 1; s32_Index < Strings.GetLength(); s32_Index++)
   {
      t_Key = Strings[s32_Index];
      s32_Position = s32_Index - 1;
      while ((s32_Position >= 0) && (Strings[s32_Position] > t_Key))
      {
         Strings[s32_Position + 1] = Strings[s32_Position];
         s32_Position--;
      }
      Strings[s32_Position + 1] = t_Key;
   }
}

//-----------------------------------------------------------------------------
