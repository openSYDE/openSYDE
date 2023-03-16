//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       string list class

   ANSI C++ string list class.
   For details cf. documentation in .h file.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__            //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <cstdio>
#include <cctype> //for toupper
#include <climits>
#include <cstring>

#include "C_SclStringList.hpp"
#include "stwtypes.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append one string

   Append one string at the end of the list

   \param[in]  orc_String   string to append

   \return
   index of new string
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SclStringList::Add(const C_SclString & orc_String)
{
   Strings.IncLength();
   Strings[Strings.GetHigh()] = orc_String;
   return static_cast<uint32_t>(Strings.GetHigh());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append one string

   Append one string at the end of the list.
   Functionally identical to C_SclStringList::Add but without the return value.

   \param[in]  orc_String   string to append
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::Append(const C_SclString & orc_String)
{
   this->Add(orc_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear all strings

   Empty the string list.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::Clear(void)
{
   Strings.SetLength(0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete one string

   Delete one string from the string list.
   Will throw if the index is invalid.

   \param[in]  ou32_Index   index of string to remove (0 = first string)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::Delete(const uint32_t ou32_Index)
{
   Strings.Delete(static_cast<int32_t>(ou32_Index));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Swap the contents of two strings

   Swap the tests of two strings.
   Will throw if the index is invalid.
   If one of the indexes is out of range the function has undefined bahavior.

   \param[in]  ou32_Index1   index of first string to swap (0 = first string in list)
   \param[in]  ou32_Index2   index of second string to swap (0 = first string in list)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::Exchange(const uint32_t ou32_Index1, const uint32_t ou32_Index2)
{
   C_SclString c_Temp;

   c_Temp = Strings[static_cast<int32_t>(ou32_Index2)];
   Strings[static_cast<int32_t>(ou32_Index2)] = Strings[static_cast<int32_t>(ou32_Index1)];
   Strings[static_cast<int32_t>(ou32_Index1)] = c_Temp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert string into list

   Insert one string into the string list at a defined position.

   \param[in]  ou32_Index   index before which to insert the new string (0 = first string in list)
   \param[in]  orc_String   string to insert
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::Insert(const uint32_t ou32_Index, const C_SclString & orc_String)
{
   Strings.Insert(static_cast<int32_t>(ou32_Index), orc_String);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find index of string within list

   Returns the index of a specified string within the string list.
   The comparison ignores upper/lower case.

   \param[in]  orc_String   string to find

   \return  ou32_Index   index of string (0 = first string in list; -1 = string not found)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SclStringList::IndexOf(const C_SclString & orc_String)
{
   int32_t s32_Index;

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      if (Strings[s32_Index].AnsiCompareIc(orc_String) == 0)
      {
         return s32_Index;
      }
   }
   return -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return all strings as one concatenated string

   Adds all strings to one single string.
   "\r\n" is inserted after each string (also after the last string).

   \return  concatenated string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclStringList::GetText(void) const
{
   C_SclString c_Text;
   int32_t s32_Index;

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      c_Text += (Strings[s32_Index] + "\r\n");
   }
   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get number of string in list

   Return number of strings contained in string list.

   \return  number of strings
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SclStringList::GetCount(void) const
{
   return static_cast<uint32_t>(Strings.GetLength());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load strings from file

   Load strings from an ASCII file.
   Will replace any existing strings.

   Will throw if:
   - the file does not exist
   - the memory to load the file cannot be allocated
   - the file cannot be read from

   \param[in]     orc_FileName     path to file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::LoadFromFile(const C_SclString & orc_FileName)
{
   std::FILE * pc_File;
   long x_FileSize;
   char_t * pcn_Buffer;
   size_t x_SizeRead;
   long x_Index;
   uint32_t u32_NumStrings;
   uint32_t u32_Line;
   int32_t s32_Len; //2GB file size limit is acceptable

   Strings.SetLength(0);
   pc_File = std::fopen(orc_FileName.c_str(), "rb");
   if (pc_File == NULL)
   {
      throw ("C_SclStringList::LoadFromFile: file not found");
   }

   //read the whole thing into one array:
   (void)std::fseek(pc_File, 0, SEEK_END);
   x_FileSize = std::ftell(pc_File);
   (void)std::fseek(pc_File, 0, SEEK_SET);

   try
   {
      pcn_Buffer = new char_t[x_FileSize + 1U]; //+ 1: put a terminating \0
   }
   catch (...)
   {
      throw ("C_SclStringList::LoadFromFile: could not allocate buffer for file");
   }
   pcn_Buffer[x_FileSize] = '\0';

   x_SizeRead = std::fread(pcn_Buffer, 1U, x_FileSize, pc_File);
   if (x_SizeRead != static_cast<size_t>(x_FileSize))
   {
      delete[] pcn_Buffer;
      throw ("C_SclStringList::LoadFromFile: could not read file");
   }

   (void)std::fclose(pc_File);

   //first detect number of lines then set StringList length
   //improves speed significantly over using ::Add for each line directly
   u32_NumStrings = 0U;

   x_Index = 0U;
   while (x_Index < x_FileSize)
   {
      if ((pcn_Buffer[x_Index] == '\r') || (pcn_Buffer[x_Index] == '\n'))
      {
         pcn_Buffer[x_Index] = '\0';
         //maybe there is a subsequent \n
         if (pcn_Buffer[x_Index + 1U] == '\n')
         {
            x_Index++;
            pcn_Buffer[x_Index] = '\0';
         }
         u32_NumStrings++;
      }
      x_Index++;
   }
   //maybe the last line is not terminated ?
   if ((x_Index > 0) && (pcn_Buffer[x_Index - 1U] != '\0'))
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
      throw ("C_SclStringList::LoadFromFile: could not allocate buffer for file");
   }
   x_Index = 0U;

   for (u32_Line = 0U; u32_Line < u32_NumStrings; u32_Line++)
   {
      //using the std::string directly improves performance significantly
      s32_Len = strlen(&pcn_Buffer[x_Index]);
      Strings[u32_Line].AsStdString()->assign(&pcn_Buffer[x_Index], s32_Len); //assign with setting length is quite
                                                                              // fast
      x_Index += (s32_Len + 1);

      //maybe there is a subsequent \n
      if ((x_Index < x_FileSize) && (pcn_Buffer[x_Index] == '\0'))
      {
         x_Index++;
      }
   }

   delete[] pcn_Buffer;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save strings to file

   Save the contents of the string list to a file.
   An existing file will be overwritten.
   "\r\n" will be appended after each line (also after the last line).

   Will throw if:
   - the file cannot be created
   - the file cannot be written to

   \param[in]     orc_FileName     path to file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::SaveToFile(const C_SclString & orc_FileName)
{
   int32_t s32_Line;
   uint32_t u32_NumWritten;

   std::FILE * pc_File;

   pc_File = std::fopen(orc_FileName.c_str(), "wb");
   if (pc_File == NULL)
   {
      throw ("C_SclStringList::SaveToFile: could not create file");
   }

   for (s32_Line = 0; s32_Line < Strings.GetLength(); s32_Line++)
   {
      u32_NumWritten = std::fwrite(Strings[s32_Line].c_str(), 1U, Strings[s32_Line].Length(), pc_File);
      if (u32_NumWritten != Strings[s32_Line].Length())
      {
         (void)std::fclose(pc_File);
         throw ("C_SclStringList::SaveToFile: could not write to file");
      }
      u32_NumWritten = std::fwrite("\r\n", 1U, 2U, pc_File);
      if (u32_NumWritten != 2U)
      {
         (void)std::fclose(pc_File);
         throw ("C_SclStringList::SaveToFile: could not write to file");
      }
   }
   (void)std::fclose(pc_File);
}

//----------------------------------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclStringList::Values(const C_SclString & orc_Key) const
{
   int32_t s32_Index;
   C_SclString c_Help = "";
   const C_SclString c_Search = orc_Key.UpperCase() + "=";

   for (s32_Index = 0; s32_Index < Strings.GetLength(); s32_Index++)
   {
      //performance boost: first check only first character:
      if (static_cast<char_t>(std::toupper(Strings[s32_Index].c_str()[0])) == (c_Search.c_str()[0]))
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Finds the first line containg a name + "="

   Returns the index of the first line containing a specified name with an added equals character.
   The comparison is done independent of upper/lower case.
   The "=" may be preceeded by blanks.

   \param[in]     orc_Name    name to search foe

   \return
   if "name=" is found:  index of string (0 = first string in list)
   else:                 -1
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SclStringList::IndexOfName(const C_SclString & orc_Name) const
{
   bool q_Found = false;
   int32_t s32_Index;
   const C_SclString c_Search = orc_Name.UpperCase();
   C_SclString c_Remainder;
   uint32_t u32_Pos;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the part of a string after a "="

   Returns the part of one string after the equels ("=") chraracter.

   \param[in]     ou32_Index   index of string (0 = first string in list)

   \return
   if the string contains "=":  part of string after "="
   else:                        empty string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclStringList::ValueFromIndex(const uint32_t ou32_Index) const
{
   uint32_t u32_Pos;
   C_SclString c_Help;

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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add multiple strings to string list

   Adds then content of another string list to this string list.

   \param[in]     opc_Strings   string list to add (pointer is not checked for validity)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::AddStrings(const C_SclStringList * const opc_Strings)
{
   int32_t s32_Index;
   int32_t s32_OldIndex;

   s32_OldIndex = Strings.GetLength();
   Strings.IncLength(opc_Strings->Strings.GetLength()); //first increase length (faster than adding one by one)
   for (s32_Index = 0; s32_Index < opc_Strings->Strings.GetLength(); s32_Index++)
   {
      Strings[s32_Index + s32_OldIndex] = opc_Strings->Strings[s32_Index];
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sort strings in list

   Sorts by ASCII values of the individual characters.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclStringList::Sort(void)
{
   int32_t s32_Index;
   int32_t s32_Position;
   C_SclString t_Key;

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

//----------------------------------------------------------------------------------------------------------------------
