//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       INI file class

   \class       stw_scl::C_SCLIniFile
   \brief       ANSI C++ INI file class.
   
   Aim: provide most of the functionality that Borland's TIniFile does
    while only using ANSI C++ (with STL in this case).
   So cf. the documentation of the VCL TIniFile for details on most API functions.

   Instead of AnsiString/UnicodeString the C_SCLString class is used (which is not Unicode-capable).
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
#ifndef CSCLINIFILEH
#define CSCLINIFILEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <string>
#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLStringList.h"
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

//unfortunately we need to mark throwing constructors and destructors specially in C++11
#ifndef SCL_WILL_THROW
#if __cplusplus >= 201103L //C++11 ?
#define SCL_WILL_THROW  noexcept(false)
#else
//not required (will be expected to throw by default)
#define SCL_WILL_THROW
#endif
#endif

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///This structure stores the definition of a key.
/// A key is a named identifier that is associated with a value. It may or may not have a comment.
/// All comments must PRECEDE the key on the line in the config file.
class SCL_PACKAGE C_SCLIniKey
{
public:
   C_SCLString c_Key;     ///< key (text before the "=")
   C_SCLString c_Value;   ///< value (text after the "=")
   C_SCLString c_Comment; ///< comment preceeding the key
};

//----------------------------------------------------------------------------------------------------------------------
///This structure stores the definition of a section.
/// A section contains any number of keys, and may or may not have a comment.
/// Like keys, all comments must precede the section.
class SCL_PACKAGE C_SCLIniSection
{
public:
   C_SCLString c_Name;                  ///< name (text within "[""]")
   C_SCLString c_Comment;               ///< comment preceeding the section
   SCLDynamicArray<C_SCLIniKey> c_Keys; ///< key/value pairs contains in this section
};

//----------------------------------------------------------------------------------------------------------------------
///INI file handling
class SCL_PACKAGE C_SCLIniFile
{
private:
   // Utility Methods
   static void mh_GetNextPair(const C_SCLString & orc_CommandLine, C_SCLString & orc_Key, C_SCLString & orc_Value);

   // Returns the requested key (if found) from the requested Section. Returns NULL otherwise.
   C_SCLIniKey * m_GetKey(const C_SCLString & orc_Key, const C_SCLString & orc_Section);

   //shortcut if we already have a valid section
   C_SCLIniKey * m_GetKey(const C_SCLString & orc_Key, C_SCLIniSection * const opc_Section);

   // Returns the requested section (if found), NULL otherwise.
   C_SCLIniSection * m_GetSection(const C_SCLString & orc_Section);

   // Our default access method. Returns a const reference to the raw C_SCLString value.
   // Note that this returns keys specific to the given section only.
   const C_SCLString & m_GetValue(const C_SCLString & orc_Key, const C_SCLString & orc_Section);

   // Sets the value of a given key. Will create the key if it is not found.
   // If oq_ForceAppend==true a new key will be appended to the end of the section
   // without checking if this key already exists. Caution: This could produce a
   // corrupted INI file but makes a big performance improvement when generating a
   // new file.
   bool m_SetValue(const C_SCLString & orc_Section, const C_SCLString & orc_Key, const C_SCLString & orc_Value,
                   const bool oq_ForceAppend);

   // Parses a string into a proper comment token/comment.
   static C_SCLString mh_CommentStr(const C_SCLString & orc_Comment);

   // File handling methods
   bool m_Load(const C_SCLString & orc_FileName);

   // Key/Section handling methods
   // CreateSection: Creates the new section if it does not already
   // exist. Section is created with no keys.
   C_SCLIniSection * m_CreateSection(const C_SCLString & orc_Section);

   SCLDynamicArray<C_SCLIniSection> mc_Sections; ///< Our list of sections
   bool mq_Dirty;                                ///< Tracks whether or not data has changed.
   stw_types::sint32 ms32_PreviousSectionIndex;  ///< for speeding up searching for a section
   stw_types::sint32 ms32_PreviousKeyIndex;      ///< for speeding up searching for a key

   // Methods

public:
   // Constructors & Destructors
   C_SCLIniFile(const C_SCLString & orc_FileName);
   virtual ~C_SCLIniFile() SCL_WILL_THROW;

   C_SCLString        ReadString (const C_SCLString & orc_Section, const C_SCLString & orc_Key,
                                  const C_SCLString & orc_Default);
   stw_types::sintn   ReadInteger(const C_SCLString & orc_Section, const C_SCLString & orc_Key,
                                  const stw_types::sintn osn_Default);
   bool               ReadBool   (const C_SCLString & orc_Section, const C_SCLString & orc_Key, const bool oq_Default);
   stw_types::float64 ReadFloat  (const C_SCLString & orc_Section, const C_SCLString & orc_Key,
                                  const stw_types::float64 of64_Default);

   //convenience shortcuts for commonly used stw_types:
   stw_types::uint8   ReadUint8 (const C_SCLString & orc_Section, const C_SCLString & orc_Key,
                                 const stw_types::uint8 ou8_Default);
   stw_types::uint16  ReadUint16(const C_SCLString & orc_Section, const C_SCLString & orc_Key,
                                 const stw_types::uint16 ou16_Default);

   void WriteString (const C_SCLString & orc_Section, const C_SCLString & orc_Key, const C_SCLString & orc_Value,
                     const bool oq_ForceAppend = false);
   void WriteInteger(const C_SCLString & orc_Section, const C_SCLString & orc_Key, const stw_types::sintn osn_Value,
                     const bool oq_ForceAppend = false);
   void WriteBool   (const C_SCLString & orc_Section, const C_SCLString & orc_Key, const bool oq_Value,
                     const bool oq_ForceAppend = false);
   void WriteFloat  (const C_SCLString & orc_Section, const C_SCLString & orc_Key, const stw_types::float64 of64_Value,
                     const bool oq_ForceAppend = false);

   void EraseSection(const C_SCLString & orc_Section);
   void DeleteKey(const C_SCLString & orc_Section, const C_SCLString & orc_Key);

   void UpdateFile(void);

   bool SectionExists(const C_SCLString & orc_Section);
   bool ValueExists(const C_SCLString & orc_Section, const C_SCLString & orc_Key);

   void ReadSection(const C_SCLString & orc_Section, C_SCLStringList * const opc_Strings, const bool oq_Append = false);
   void ReadSectionValues(const C_SCLString & orc_Section, C_SCLStringList * const opc_Strings,
                          const bool oq_Append = false);
   void ReadSections(C_SCLStringList * const opc_Strings, const bool oq_Append = false) const;

   void GetFileAsStringList(C_SCLStringList & orc_Strings) const;

   C_SCLString FileName; ///< path to ini file; can be used after creation to store data in another ini file
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
