//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       INI file class

   \class       stw::scl::C_SclIniFile
   \brief       ANSI C++ INI file class.

   Aim: provide most of the functionality that Borland's TIniFile does
    while only using ANSI C++ (with STL in this case).
   So cf. the documentation of the VCL TIniFile for details on most API functions.

   Instead of AnsiString/UnicodeString the C_SclString class is used (which is not Unicode-capable).
   Loosely Based on CDataFile class by Gary McNickle <gary#sunstorm.net>.

   Additionally the following is supported:
   - INI files > 64kB
   - using file path "" in the constructor can be used to create a INI file structure in memory only
   - GetFileAsStringList can be used to save INI file content to a string list in the same format used for the file

   Contrary to "ftp://ftp.ihnet.it/zyxel/public/software/Windows/win31ini.txt"
    comments (starting with ";") are only permitted at the beginning of a line.

   What is NOT implemented (compared to Borland VCL TIniFile):
   - ReadDate
   - ReadDateTime
   - ReadTime
   - WriteDate
   - WriteDateTime
   - WriteTime
   - stream-based functions

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SCLINIFILEHPP
#define C_SCLINIFILEHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <string>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "C_SclDynamicArray.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace scl
{
/* -- Defines ------------------------------------------------------------------------------------------------------- */

//unfortunately we need to mark throwing constructors and destructors specially in C++11
#ifndef SCL_WILL_THROW
#if __cplusplus >= 201103L //C++11 ?
#define SCL_WILL_THROW  noexcept (false)
#else
//not required (will be expected to throw by default)
#define SCL_WILL_THROW
#endif
#endif

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Stores the definition of a key.
/// A key is a named identifier that is associated with a value. It may or may not have a comment.
/// All comments must PRECEDE the key on the line in the config file.
class C_SclIniKey
{
public:
   C_SclString c_Key;     ///< key (text before the "=")
   C_SclString c_Value;   ///< value (text after the "=")
   C_SclString c_Comment; ///< comment preceding the key
};

//----------------------------------------------------------------------------------------------------------------------
///Stores the definition of a section.
/// A section contains any number of keys, and may or may not have a comment.
/// Like keys, all comments must precede the section.
class C_SclIniSection
{
protected:
   int32_t ms32_PreviousKeyIndex; ///< for speeding up searching for a key

public:
   C_SclIniSection();

   //Returns the requested key (if found) from the Section. Returns NULL otherwise.
   C_SclIniKey * GetKey(const C_SclString & orc_Key);
   //Return value of specified key (or empty string if not found)
   C_SclString GetValue(const C_SclString & orc_Key);

   C_SclString c_Name;                    ///< name (text within "[""]")
   C_SclString c_Comment;                 ///< comment preceding the section
   C_SclDynamicArray<C_SclIniKey> c_Keys; ///< key/value pairs contained in this section
};

//----------------------------------------------------------------------------------------------------------------------
///INI file handling
class C_SclIniFile
{
protected:
   // Utility Methods
   static void mh_GetNextPair(const C_SclString & orc_CommandLine, C_SclString & orc_Key, C_SclString & orc_Value);

   // Returns the requested key (if found) from the requested Section. Returns NULL otherwise.
   C_SclIniKey * m_GetKey(const C_SclString & orc_Key, const C_SclString & orc_Section);

   // Returns the requested section (if found), NULL otherwise.
   C_SclIniSection * m_GetSection(const C_SclString & orc_Section);

   // Our default access method. Returns a const reference to the raw C_SclString value.
   // Note that this returns keys specific to the given section only.
   const C_SclString & m_GetValue(const C_SclString & orc_Key, const C_SclString & orc_Section);

   // Sets the value of a given key. Will create the key if it is not found.
   // If oq_ForceAppend==true a new key will be appended to the end of the section
   // without checking if this key already exists. Caution: This could produce a
   // corrupted INI file but makes a big performance improvement when generating a
   // new file.
   bool m_SetValue(const C_SclString & orc_Section, const C_SclString & orc_Key, const C_SclString & orc_Value,
                   const bool oq_ForceAppend);

   // Parses a string into a proper comment token/comment.
   static C_SclString mh_CommentStr(const C_SclString & orc_Comment);

   // File handling methods
   bool m_Load(const C_SclString & orc_FileName);

   // Key/Section handling methods
   // CreateSection: Creates the new section if it does not already
   // exist. Section is created with no keys.
   C_SclIniSection * m_CreateSection(const C_SclString & orc_Section);

   C_SclDynamicArray<C_SclIniSection> mc_Sections; ///< Our list of sections
   bool mq_Dirty;                                  ///< Tracks whether or not data has changed.
   int32_t ms32_PreviousSectionIndex;              ///< for speeding up searching for a section

public:
   // Constructors & Destructors
   C_SclIniFile(const C_SclString & orc_FileName);
   virtual ~C_SclIniFile() SCL_WILL_THROW;

   C_SclString ReadString (const C_SclString & orc_Section, const C_SclString & orc_Key,
                           const C_SclString & orc_Default);
   int32_t   ReadInteger(const C_SclString & orc_Section, const C_SclString & orc_Key, const int32_t os32_Default);
   bool      ReadBool   (const C_SclString & orc_Section, const C_SclString & orc_Key, const bool oq_Default);
   float64_t ReadFloat  (const C_SclString & orc_Section, const C_SclString & orc_Key, const float64_t of64_Default);

   //convenience shortcuts for commonly used stw_types:
   uint8_t   ReadUint8 (const C_SclString & orc_Section, const C_SclString & orc_Key, const uint8_t ou8_Default);
   uint16_t  ReadUint16(const C_SclString & orc_Section, const C_SclString & orc_Key, const uint16_t ou16_Default);

   void WriteString (const C_SclString & orc_Section, const C_SclString & orc_Key, const C_SclString & orc_Value,
                     const bool oq_ForceAppend = false);
   void WriteInteger(const C_SclString & orc_Section, const C_SclString & orc_Key, const int32_t os32_Value,
                     const bool oq_ForceAppend = false);
   void WriteBool   (const C_SclString & orc_Section, const C_SclString & orc_Key, const bool oq_Value,
                     const bool oq_ForceAppend = false);
   void WriteFloat  (const C_SclString & orc_Section, const C_SclString & orc_Key, const float64_t of64_Value,
                     const bool oq_ForceAppend = false);

   void EraseSection(const C_SclString & orc_Section);
   void DeleteKey(const C_SclString & orc_Section, const C_SclString & orc_Key);

   void UpdateFile(void);

   bool SectionExists(const C_SclString & orc_Section);
   bool ValueExists(const C_SclString & orc_Section, const C_SclString & orc_Key);

   void ReadSection(const C_SclString & orc_Section, C_SclStringList * const opc_Strings, const bool oq_Append = false);
   void ReadSectionValues(const C_SclString & orc_Section, C_SclStringList * const opc_Strings,
                          const bool oq_Append = false);
   void ReadSections(C_SclStringList * const opc_Strings, const bool oq_Append = false) const;

   void GetFileAsStringList(C_SclStringList & orc_Strings) const;

   C_SclString FileName; ///< path to ini file; can be used after creation to store data in another ini file
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}
#endif
