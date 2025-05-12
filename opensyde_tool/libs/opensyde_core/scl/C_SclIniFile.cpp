//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       INI file class

   ANSI C++ INI file class.
   For details cf. documentation in .h file.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include <cstdio>
#include <climits>
#include <cstring>

#include "C_SclIniFile.hpp"
#include "C_SclString.hpp"
#include "stwtypes.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
///This constant contains the character that is checked for to determine if a
///line is a comment or not.
static const char_t mcn_CommentIndicator = ';';

///This constant contains the character that is checked against to determine if
///a line contains an assignment ( key = value )
static const char_t mcn_EqualIndicator = '=';

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

C_SclIniSection::C_SclIniSection() :
   ms32_PreviousKeyIndex(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   If it can load the file, it will do so and populate
   the section list with the values from the file.

   \param[in]     orc_FileName    path to ini file
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclIniFile::C_SclIniFile(const C_SclString & orc_FileName) :
   mq_Dirty(false),
   ms32_PreviousSectionIndex(0),
   FileName(orc_FileName)
{
   if (orc_FileName == "")
   {
      //empty in-memory file
      mc_Sections.SetLength(0);
   }
   else
   {
      (void)m_Load(FileName);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Destructor

   Saves the file if any values have changed since the last save.

   Throws if
   - the filename is empty
   - writing to the file fails (e.g. write protected)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclIniFile::~C_SclIniFile() SCL_WILL_THROW
{
   if ((mq_Dirty == true) && (FileName != ""))
   {
      UpdateFile();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: file loader

   Attempts to load in the text file. If successful it will populate the
   Section list with the key/value pairs found in the file. Note that comments
    are saved so that they can be rewritten to the file later.
   Saves the file if any values have changed since the last save.

   \param[in]  orc_FileName  path to existing file

   \return
   true   file loaded
   false  file not loaded
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SclIniFile::m_Load(const C_SclString & orc_FileName)
{
   C_SclDynamicArray<uint16_t> c_Items;
   C_SclString c_Line;
   C_SclString c_Comment;

   int32_t s32_Index;
   uint16_t u16_NumSections;
   uint16_t u16_NumKeysAdded = 0U;

   C_SclStringList c_List;
   try
   {
      c_List.LoadFromFile(orc_FileName);
   }
   catch (...)
   {
      //ignore: not a problem, file does probably not exist (yet) !
      mc_Sections.SetLength(0);
      return false;
   }

   //first pass: parse to know how to set the dimensions of our dynamic array:
   //(performance increase)
   for (s32_Index = 0; s32_Index < c_List.Strings.GetLength(); s32_Index++)
   {
      c_Line = c_List.Strings[s32_Index].Trim(); //Trim to be defensive against whitespaces

      if (c_Line.Pos(mcn_CommentIndicator) == 1U)
      {
      }
      else if (c_Line.Pos("[") == 1U) // new section
      {
         c_Items.IncLength();
      }
      else if (c_Line.Pos("=") != 0U) // we have a key
      {
         if (c_Items.GetLength() != 0)
         {
            c_Items[c_Items.GetHigh()] += 1U;
         }
      }
      else
      {
         //nothing to do ...
      }
   }

   try
   {
      mc_Sections.SetLength(c_Items.GetLength());
   }
   catch (...)
   {
      return false;
   }

   for (s32_Index = 0; s32_Index < mc_Sections.GetLength(); s32_Index++)
   {
      try
      {
         mc_Sections[s32_Index].c_Keys.SetLength(c_Items[s32_Index]);
      }
      catch (...)
      {
         return false;
      }
   }

   u16_NumSections = 0U;

   //2nd pass: enter data
   for (s32_Index = 0; s32_Index < c_List.Strings.GetLength(); s32_Index++)
   {
      c_Line = c_List.Strings[s32_Index].Trim();

      if (c_Line.Pos(mcn_CommentIndicator) == 1U)
      {
         c_Comment += (c_Line + "\n");
      }
      else if (c_Line.Pos("[") == 1U) // new section
      {
         (void)c_Line.Delete(1U, 1U);
         (void)c_Line.Delete(c_Line.Pos("]"), 1U);

         //we already dimensioned the array -> simply copy data over
         mc_Sections[u16_NumSections].c_Name    = c_Line;
         mc_Sections[u16_NumSections].c_Comment = c_Comment;

         u16_NumSections++;
         c_Comment = "";
         u16_NumKeysAdded = 0U;
      }
      else if (c_Line.Length() > 0U) // we have a key, add this key/value pair
      {
         C_SclString c_Key;
         C_SclString c_Value;
         mh_GetNextPair(c_Line, c_Key, c_Value);

         if (c_Key.Length() > 0U)
         {
            if (u16_NumSections > 0U)
            {
               //we already dimensioned the array -> simply copy data over
               mc_Sections[static_cast<int32_t>(u16_NumSections) - 1].c_Keys[u16_NumKeysAdded].c_Key     = c_Key;
               mc_Sections[static_cast<int32_t>(u16_NumSections) - 1].c_Keys[u16_NumKeysAdded].c_Value   = c_Value;
               mc_Sections[static_cast<int32_t>(u16_NumSections) - 1].c_Keys[u16_NumKeysAdded].c_Comment = c_Comment;
               u16_NumKeysAdded++;
               c_Comment = "";
            }
         }
      }
      else
      {
         //probably an empty line
      }
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write data to ini file

   Attempts to save the Section list and keys to the file.
   Throws if
   - the filename is empty
   - writing to the file fails (e.g. write protected)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::UpdateFile(void)
{
   C_SclStringList c_Strings;

   if (FileName.Length() == 0U)
   {
      throw "C_SclIniFile::UpdateFile failed !";
   }

   this->GetFileAsStringList(c_Strings);
   try
   {
      c_Strings.SaveToFile(FileName);
   }
   catch (...)
   {
      throw "C_SclIniFile::UpdateFile: could not write to file !";
   }

   mq_Dirty = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: set value of key

   Given a key, a value and a section, this function will attempt to locate the
    Key within the given section, and if it finds it, change the keys value to
    the new value. If it does not locate the key, it will create a new key with
    the proper value and place it in the section requested.
   If oq_ForceAppend is true a new key will be appended to the end of the section
    without checking if this key already exists. Caution: This could produce a
    corrupted INI file but makes a big performance improvement when generating a
    new file.

   \param[in]  orc_Section      INI section to write to
   \param[in]  orc_Key          INI key to write to
   \param[in]  orc_Value        value to write
   \param[in]  oq_ForceAppend   see description above

   \return
   false   error (section or key empty)
   true    value written
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SclIniFile::m_SetValue(const C_SclString & orc_Section, const C_SclString & orc_Key,
                              const C_SclString & orc_Value, const bool oq_ForceAppend)
{
   C_SclIniKey * pc_Key = NULL;
   C_SclIniSection * pc_Section;

   if ((orc_Section == "") || (orc_Key == ""))
   {
      return false;
   }

   //does the section already exist ?
   pc_Section = m_GetSection(orc_Section);
   if (pc_Section != NULL)
   {
      if (oq_ForceAppend == false)
      {
         pc_Key = pc_Section->GetKey(orc_Key);
      }
   }
   else
   {
      //no: create new section
      pc_Section = m_CreateSection(orc_Section);
   }

   mq_Dirty = true;

   // if the key does not exist in that section, then add the new key.
   if (pc_Key == NULL)
   {
      pc_Section->c_Keys.IncLength();
      pc_Section->c_Keys[pc_Section->c_Keys.GetHigh()].c_Key     = orc_Key;
      pc_Section->c_Keys[pc_Section->c_Keys.GetHigh()].c_Value   = orc_Value;
      pc_Section->c_Keys[pc_Section->c_Keys.GetHigh()].c_Comment = "";
   }
   else
   {
      //existing key:
      pc_Key->c_Value = orc_Value;
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write string value to ini file

   Writes a string value.

   If oq_ForceAppend==true a new key will be appended to the end of the section
   without checking if this key already exists. Caution: This could produce a
   corrupted INI file but makes a big performance improvement when generating a
   new file.

   Throws if orc_Section or orc_Key are empty strings.

   \param[in]  orc_Section     section to write to
   \param[in]  orc_Key         key to write to
   \param[in]  orc_Value       value to write
   \param[in]  oq_ForceAppend  append at the end without checking whether key already exists
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::WriteString(const C_SclString & orc_Section, const C_SclString & orc_Key,
                               const C_SclString & orc_Value, const bool oq_ForceAppend)
{
   bool q_Return;

   q_Return = m_SetValue(orc_Section, orc_Key, orc_Value.Trim(), oq_ForceAppend);
   if (q_Return == false)
   {
      throw "C_SclIniFile::WriteString failed !";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write int32_t value to ini file

   Writes an int32_t value.

   If oq_ForceAppend==true a new key will be appended to the end of the section
   without checking if this key already exists. Caution: This could produce a
   corrupted INI file but makes a big performance improvement when generating a
   new file.

   Throws if orc_Section or orc_Key are empty strings.

   \param[in]  orc_Section     section to write to
   \param[in]  orc_Key         key to write to
   \param[in]  os32_Value      value to write
   \param[in]  oq_ForceAppend  append at the end without checking whether key already exists
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::WriteInteger(const C_SclString & orc_Section, const C_SclString & orc_Key, const int32_t os32_Value,
                                const bool oq_ForceAppend)
{
   bool q_Return;
   const C_SclString c_Value(os32_Value);

   q_Return = m_SetValue(orc_Section, orc_Key, c_Value, oq_ForceAppend);
   if (q_Return == false)
   {
      throw "C_SclIniFile::WriteInteger failed !";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write bool value to ini file

   Writes a bool value.

   If oq_ForceAppend==true a new key will be appended to the end of the section
   without checking if this key already exists. Caution: This could produce a
   corrupted INI file but makes a big performance improvement when generating a
   new file.

   Throws if orc_Section or orc_Key are empty strings.

   \param[in]  orc_Section     section to write to
   \param[in]  orc_Key         key to write to
   \param[in]  oq_Value        value to write
   \param[in]  oq_ForceAppend  append at the end without checking whether key already exists
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::WriteBool(const C_SclString & orc_Section, const C_SclString & orc_Key, const bool oq_Value,
                             const bool oq_ForceAppend)
{
   bool q_Return;
   const C_SclString c_Value(static_cast<int32_t>(oq_Value));

   q_Return = m_SetValue(orc_Section, orc_Key, c_Value, oq_ForceAppend);
   if (q_Return == false)
   {
      throw "C_SclIniFile::WriteBool failed !";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Write float64 value to ini file

   Writes a float64 value.

   If oq_ForceAppend==true a new key will be appended to the end of the section
   without checking if this key already exists. Caution: This could produce a
   corrupted INI file but makes a big performance improvement when generating a
   new file.

   Throws if orc_Section or orc_Key are empty strings.

   \param[in]  orc_Section     section to write to
   \param[in]  orc_Key         key to write to
   \param[in]  of64_Value      value to write
   \param[in]  oq_ForceAppend  append at the end without checking whether key already exists
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::WriteFloat(const C_SclString & orc_Section, const C_SclString & orc_Key, const float64_t of64_Value,
                              const bool oq_ForceAppend)
{
   bool q_Return;
   const C_SclString c_Value(of64_Value);

   q_Return = m_SetValue(orc_Section, orc_Key, c_Value, oq_ForceAppend);
   if (q_Return == false)
   {
      throw "C_SclIniFile::WriteFloat failed !";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get value of key

   Returns the key value as a C_SclString object. A return value of ""
    indicates that the key could not be found or an empty value.
    only used internally, so we return a reference to the data.

   \param[in]  orc_Key         INI key to read from
   \param[in]  orc_Section     INI section to read from

   \return
   read value ("" if key or section does not exist)
*/
//----------------------------------------------------------------------------------------------------------------------
const C_SclString & C_SclIniFile::m_GetValue(const C_SclString & orc_Key, const C_SclString & orc_Section)
{
   static const C_SclString hc_Empty = "";

   const C_SclIniKey * const pc_Key = m_GetKey(orc_Key, orc_Section);

   return (pc_Key == NULL) ? hc_Empty : pc_Key->c_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read a C_SclString value

   Read a C_SclString value from the ini file.
   In the following cases the default will be used:
   - section not present
   - key not present
   - value is empty

   \param[in]     orc_Section    ini section
   \param[in]     orc_Key        ini key
   \param[in]     orc_Default    default value

   \return
   C_SclString value from ini (or default if error)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclIniFile::ReadString(const C_SclString & orc_Section, const C_SclString & orc_Key,
                                     const C_SclString & orc_Default)
{
   C_SclString c_String;

   c_String = m_GetValue(orc_Key, orc_Section);
   if (c_String == "") //not found or empty
   {
      c_String = orc_Default;
   }
   return c_String;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read a uint8 value

   Read a uint8 value from the ini file.
   In the following cases the default will be used:
   - section not present
   - key not present
   - value in ini is not a number
   - value in ini is < 0 or > 0xFF

   \param[in]     orc_Section    ini section
   \param[in]     orc_Key        ini key
   \param[in]     ou8_Default    default value

   \return
   uint8 value from ini (or default if error)
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_SclIniFile::ReadUint8(const C_SclString & orc_Section, const C_SclString & orc_Key, const uint8_t ou8_Default)
{
   int32_t s32_Return;

   s32_Return = ReadInteger(orc_Section, orc_Key, ou8_Default);
   if ((s32_Return > 0xFF) || (s32_Return < 0))
   {
      s32_Return = ou8_Default;
   }
   return static_cast<uint8_t>(s32_Return);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read a uint16 value

   Read a uint16 value from the ini file.
   In the following cases the default will be used:
   - section not present
   - key not present
   - value in ini is not a number
   - value in ini is < 0 or > 0xFFFF

   \param[in]     orc_Section    ini section
   \param[in]     orc_Key        ini key
   \param[in]     ou16_Default   default value

   \return
   uint16 value from ini (or default if error)
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_SclIniFile::ReadUint16(const C_SclString & orc_Section, const C_SclString & orc_Key,
                                  const uint16_t ou16_Default)
{
   int32_t s32_Return;

   s32_Return = ReadInteger(orc_Section, orc_Key, ou16_Default);
   if ((s32_Return < 0) || (s32_Return > 0xFFFF))
   {
      s32_Return = ou16_Default;
   }
   return static_cast<uint16_t>(s32_Return);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read an int32_t value

   Read an int32_t value from the ini file.
   In the following cases the default will be used:
   - section not present
   - key not present
   - value in ini is not a number (or not "TRUE" or "FALSE")

   A value of "TRUE" (ignoring casing) is returned as the value 1.
   A value of "FALSE" (ignoring casing) is returned as the value 0.

   \param[in]     orc_Section    ini section
   \param[in]     orc_Key        ini key
   \param[in]     os32_Default   default value

   \return
   int32_t value from ini (or default if error)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SclIniFile::ReadInteger(const C_SclString & orc_Section, const C_SclString & orc_Key,
                                  const int32_t os32_Default)
{
   C_SclString c_String;
   int32_t s32_Return;

   c_String = m_GetValue(orc_Key, orc_Section);
   if (c_String == "") //not found or empty
   {
      s32_Return = os32_Default;
   }
   else
   {
      try
      {
         s32_Return = c_String.ToInt();
      }
      catch (...)
      {
         if (c_String.UpperCase() == "TRUE")
         {
            s32_Return = 1;
         }
         else if (c_String.UpperCase() == "FALSE")
         {
            s32_Return = 0;
         }
         else
         {
            s32_Return = os32_Default;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read a bool value

   Read a bool value from the ini file.
   In the following cases the default will be used:
   - section not present
   - key not present
   - value in ini is not a number or not "TRUE" or "FALSE"

   A value of "1" or "TRUE" (ignoring casing) is returned as the value 1.
   A value of "0" or "FALSE" (ignoring casing) is returned as the value 0.

   \param[in]     orc_Section    ini section
   \param[in]     orc_Key        ini key
   \param[in]     oq_Default     default value

   \return
   bool value from ini (or default if error)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SclIniFile::ReadBool(const C_SclString & orc_Section, const C_SclString & orc_Key, const bool oq_Default)
{
   return ((ReadInteger(orc_Section, orc_Key, static_cast<int32_t>(oq_Default)) == 0) ? false : true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   read a float64 value

   Read a float64 value from the ini file.
   In the following cases the default will be used:
   - section not present
   - key not present
   - value in ini is not convertible to float64

   Will accept "." or "," as decimal separator.

   \param[in]     orc_Section    ini section
   \param[in]     orc_Key        ini key
   \param[in]     of64_Default   default value

   \return
   float64 value from ini (or default if error)
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_SclIniFile::ReadFloat(const C_SclString & orc_Section, const C_SclString & orc_Key,
                                  const float64_t of64_Default)
{
   C_SclString c_String;
   float64_t f64_Return;

   c_String = m_GetValue(orc_Key, orc_Section);
   if (c_String == "") //not found or empty
   {
      f64_Return = of64_Default;
   }
   else
   {
      f64_Return = c_String.ToDouble();
   }
   return f64_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Erase whole section

   Erase one section from the ini file.
   Will throw if the section is not found.

   \param[in]     orc_Section    ini section to erase
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::EraseSection(const C_SclString & orc_Section)
{
   int32_t s32_Index;

   for (s32_Index = 0; s32_Index < mc_Sections.GetLength(); s32_Index++)
   {
      if (mc_Sections[s32_Index].c_Name.AnsiCompareIc(orc_Section) == 0)
      {
         mc_Sections.Delete(s32_Index);
         mq_Dirty = true;
         ms32_PreviousSectionIndex = 0;
         return;
      }
   }

   throw "C_SclIniFile::EraseSection failed !";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete one key

   Delete a specific key and its value in a specific section.
   If the section or the key is not found the function will do nothing.

   \param[in]     orc_Section    ini section containing key to erase
   \param[in]     orc_Key        key to erase
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::DeleteKey(const C_SclString & orc_Section, const C_SclString & orc_Key)
{
   int32_t s32_Index;
   C_SclIniSection * const pc_Section = m_GetSection(orc_Section);

   if (pc_Section == NULL)
   {
      return;
   }

   for (s32_Index = 0; s32_Index < pc_Section->c_Keys.GetLength(); s32_Index++)
   {
      if (pc_Section->c_Keys[s32_Index].c_Key.AnsiCompareIc(orc_Key) == 0)
      {
         pc_Section->c_Keys.Delete(s32_Index);
         mq_Dirty = true;
         return;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create a new .ini section

   Given a section name, this function creates the new section and assigns
    an empty comment.

   \param[in]     orc_Section    section to create

   \return
   Pointer to new section
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclIniSection * C_SclIniFile::m_CreateSection(const C_SclString & orc_Section)
{
   mc_Sections.IncLength();
   mc_Sections[mc_Sections.GetHigh()].c_Name    = orc_Section;
   mc_Sections[mc_Sections.GetHigh()].c_Comment = "";
   mq_Dirty = true;
   ms32_PreviousSectionIndex = 0;

   return &mc_Sections[mc_Sections.GetHigh()];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Detect whether a section exists.

   Checks whether a specified section exists in the ini file.

   \param[in]     orc_Section    section to check

   \return
   true    section exists
   false   section does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SclIniFile::SectionExists(const C_SclString & orc_Section)
{
   const C_SclIniSection * const pt_Section = m_GetSection(orc_Section);

   return (pt_Section != NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Detect whether a key exists.

   Checks whether a specified key exists within a specified section in the ini file.

   \param[in]     orc_Section    section to check
   \param[in]     orc_Key        key to check

   \return
   true    key exists in section
   false   key does not exist in section
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SclIniFile::ValueExists(const C_SclString & orc_Section, const C_SclString & orc_Key)
{
   const C_SclIniKey * const pt_Key = m_GetKey(orc_Key, orc_Section);

   return (pt_Key != NULL);
}

//----------------------------------------------------------------------------------------------------------------------
// Given a key and section name, looks up the key and if found, returns a
// pointer to that key, otherwise returns NULL.
C_SclIniKey * C_SclIniFile::m_GetKey(const C_SclString & orc_Key, const C_SclString & orc_Section)
{
   C_SclIniSection * const pt_Section = m_GetSection(orc_Section);

   if (pt_Section == NULL)
   {
      return NULL;
   }

   return pt_Section->GetKey(orc_Key);
}

//----------------------------------------------------------------------------------------------------------------------

C_SclIniKey * C_SclIniSection::GetKey(const C_SclString & orc_Key)
{
   int32_t s32_Index;
   int32_t s32_LastIndex;

   s32_LastIndex = ms32_PreviousKeyIndex;

   //Search from last successful point to the end
   for (s32_Index = s32_LastIndex; s32_Index < this->c_Keys.GetLength(); s32_Index++)
   {
      if (this->c_Keys[s32_Index].c_Key.AnsiCompareIc(orc_Key) == 0)
      {
         ms32_PreviousKeyIndex = s32_Index;
         return &this->c_Keys[s32_Index];
      }
   }

   //Not found yet -> Search from beginning to last successful point
   for (s32_Index = 0; s32_Index < s32_LastIndex; s32_Index++)
   {
      if (this->c_Keys[s32_Index].c_Key.AnsiCompareIc(orc_Key) == 0)
      {
         ms32_PreviousKeyIndex = s32_Index;
         return &this->c_Keys[s32_Index];
      }
   }

   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_SclIniSection::GetValue(const C_SclString & orc_Key)
{
   C_SclString c_Result;
   C_SclIniKey * const pc_Key = this->GetKey(orc_Key);

   if (pc_Key != NULL)
   {
      c_Result = pc_Key->c_Value;
   }
   return c_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Find section

   Given a section name, locates that section in the list and returns a pointer to it.

   We do not simply scan from the first index each time.
   Instead we try to improve performance for subsequent calls by assuming the following:
   In many cases the caller will parse the file in a ways that
   - usually the same section is used for many keys
   - sections parsed after the first typically reside after the previously parsed section
   So this approach can help to massively speed up access.

   \param[in]     orc_Section    ini section to find

   \return
   NULL                section not found
   else                address of found section
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclIniSection * C_SclIniFile::m_GetSection(const C_SclString & orc_Section)
{
   int32_t s32_Index;
   int32_t s32_LastIndex;

   s32_LastIndex = ms32_PreviousSectionIndex;

   //Search from last successful point to the end
   for (s32_Index = s32_LastIndex; s32_Index < mc_Sections.GetLength(); s32_Index++)
   {
      if (mc_Sections[s32_Index].c_Name.AnsiCompareIc(orc_Section) == 0)
      {
         ms32_PreviousSectionIndex = s32_Index;
         return &mc_Sections[s32_Index];
      }
   }

   //Not found yet -> Search from beginning to last successful point
   for (s32_Index = 0; s32_Index < s32_LastIndex; s32_Index++)
   {
      if (mc_Sections[s32_Index].c_Name.AnsiCompareIc(orc_Section) == 0)
      {
         ms32_PreviousSectionIndex = s32_Index;
         return &mc_Sections[s32_Index];
      }
   }

   return NULL;
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_SclIniFile::mh_CommentStr(const C_SclString & orc_Comment)
{
   C_SclString c_NewStr;
   C_SclString c_Comment;

   c_Comment = orc_Comment.Trim(); //remove potentially trailing "\n"

   if (c_Comment.Length() == 0U)
   {
      return c_Comment;
   }

   if (c_Comment.Pos(mcn_CommentIndicator) > 1U)
   {
      c_NewStr = static_cast<C_SclString>(mcn_CommentIndicator) + " ";
   }

   c_NewStr += c_Comment;

   return c_NewStr;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read names of keys of section

   Places the names of all keys of one section into a string list.

   \param[in]  orc_Section     section to read from
   \param[out] opc_Strings     target string list (is not checked for validity)
   \param[in]  oq_Append       true: append to string list
                               false: clear string list before adding keys
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::ReadSection(const C_SclString & orc_Section, C_SclStringList * const opc_Strings,
                               const bool oq_Append)
{
   int32_t s32_Loop;
   uint32_t u32_OldLength;
   const C_SclIniSection * const pc_Section = m_GetSection(orc_Section);

   if (oq_Append == true)
   {
      u32_OldLength = opc_Strings->GetCount();
   }
   else
   {
      u32_OldLength = 0U;
   }

   if (pc_Section != NULL)
   {
      opc_Strings->Strings.SetLength(static_cast<int32_t>(u32_OldLength) + pc_Section->c_Keys.GetLength());
      for (s32_Loop = 0; s32_Loop < pc_Section->c_Keys.GetLength(); s32_Loop++)
      {
         opc_Strings->Strings[static_cast<int32_t>(u32_OldLength) + s32_Loop] = pc_Section->c_Keys[s32_Loop].c_Key;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read keys and values of one section

   Places the keys and values of one section into a string list.
   The format for each entry is "key=value".

   \param[in]  orc_Section     section to read from
   \param[out] opc_Strings     target string list (is not checked for validity)
   \param[in]  oq_Append       true: append to string list
                               false: clear string list before adding keys
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::ReadSectionValues(const C_SclString & orc_Section, C_SclStringList * const opc_Strings,
                                     const bool oq_Append)
{
   int32_t s32_Loop;
   uint32_t u32_OldLength;
   const C_SclIniSection * const pc_Section = m_GetSection(orc_Section);

   if (oq_Append == true)
   {
      u32_OldLength = opc_Strings->GetCount();
   }
   else
   {
      u32_OldLength = 0U;
   }

   if (pc_Section != NULL)
   {
      opc_Strings->Strings.SetLength(static_cast<int32_t>(u32_OldLength) + pc_Section->c_Keys.GetLength());
      for (s32_Loop = 0; s32_Loop < pc_Section->c_Keys.GetLength(); s32_Loop++)
      {
         opc_Strings->Strings[static_cast<int32_t>(u32_OldLength) + s32_Loop] =
            pc_Section->c_Keys[s32_Loop].c_Key + "=" + pc_Section->c_Keys[s32_Loop].c_Value;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Read names of all sections

   Places the names of all sections into a string list.

   \param[out] opc_Strings     target string list (is not checked for validity)
   \param[in]  oq_Append       true: append to string list
                               false: clear string list before adding keys
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::ReadSections(C_SclStringList * const opc_Strings, const bool oq_Append) const
{
   int32_t s32_Loop;
   uint32_t u32_OldLength;

   if (oq_Append == true)
   {
      u32_OldLength = opc_Strings->GetCount();
   }
   else
   {
      u32_OldLength = 0U;
   }

   opc_Strings->Strings.SetLength(static_cast<int32_t>(u32_OldLength) + mc_Sections.GetLength());
   for (s32_Loop = 0; s32_Loop < mc_Sections.GetLength(); s32_Loop++)
   {
      opc_Strings->Strings[static_cast<int32_t>(u32_OldLength) + s32_Loop] = mc_Sections[s32_Loop].c_Name;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function for parsing file

   Given a key + delimiter + value string, pulls the key name and value from the string.

   \param[in]   orc_CommandLine    string in "<key>=<value>" format
   \param[out]  orc_Key            extracted key (empty string if input format is not correct)
   \param[out]  orc_Value          extracted value (empty string if input format is not correct)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::mh_GetNextPair(const C_SclString & orc_CommandLine, C_SclString & orc_Key, C_SclString & orc_Value)
{
   const uint32_t u32_Pos = orc_CommandLine.Pos(mcn_EqualIndicator);

   if (u32_Pos > 0U)
   {
      orc_Key   = orc_CommandLine.SubString(1U, u32_Pos - 1U).Trim();
      orc_Value = orc_CommandLine.SubString(u32_Pos + 1U, INT_MAX).Trim();
   }
   else
   {
      orc_Key   = "";
      orc_Value = "";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   place INI file content into a string list

   Will write the current ini file content to the specified string list.
   The format is identical to the one used for writing the actual INI file.

   \param[out]     orc_Strings    content of INI file (will be cleared before adding INI file strings)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SclIniFile::GetFileAsStringList(C_SclStringList & orc_Strings) const
{
   int32_t s32_Section;
   int32_t s32_Key;
   int32_t s32_NumKeys;
   const C_SclIniSection * pc_Section;
   const C_SclIniKey * pc_Key;

   orc_Strings.Clear();

   for (s32_Section = 0; s32_Section < mc_Sections.GetLength(); s32_Section++)
   {
      pc_Section = &mc_Sections[s32_Section];

      if (pc_Section->c_Comment.Length() > 0U)
      {
         orc_Strings.Add(mh_CommentStr(pc_Section->c_Comment));
      }

      if (pc_Section->c_Name.Length() > 0U)
      {
         orc_Strings.Add("[" + pc_Section->c_Name + "]");
      }

      s32_NumKeys = pc_Section->c_Keys.GetLength();
      for (s32_Key = 0; s32_Key < s32_NumKeys; s32_Key++)
      {
         pc_Key = &pc_Section->c_Keys[s32_Key];
         if (pc_Key->c_Key.Length() > 0U)
         {
            if (pc_Key->c_Comment.Length() > 0U)
            {
               orc_Strings.Add(mh_CommentStr(pc_Key->c_Comment));
            }

            orc_Strings.Add(pc_Key->c_Key + mcn_EqualIndicator + pc_Key->c_Value);
         }
      }

      //add one blank line after every section (increases readability; required by some parsers)
      if (s32_Section < (mc_Sections.GetLength() - 1))
      {
         orc_Strings.Add("");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
