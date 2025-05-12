//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen EDS / DCF file handler

   Provides functions to read information from CANopen EDS / DCF file.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include <sstream>   //for std::istringstream
#include <algorithm> //for std::sort
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscCanOpenObjectDictionary.hpp"
#include "C_SclString.hpp"
#include "C_SclIniFile.hpp"
#include "TglFile.hpp"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::opensyde_core;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_TextAndSize
{
public:
   C_SclString c_Text;
   uint8_t u8_Size;

   bool q_IsInteger;
   bool q_IsUnsigned;
   bool q_IsFloat;
   bool q_IsString;
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static const uint8_t mu8_NUM_DATA_TYPES = 0x24U;

static const C_TextAndSize mac_TextAndSizeTable[mu8_NUM_DATA_TYPES] =
{
   //name            size   sint,  uint,  float, string
   { "",                0,  false, false, false, false },            // 0
   { "BOOLEAN",         1,  false, true,  false, false },            // 1
   { "INTEGER8",        1,  true,  false, false, false },            // 2
   { "INTEGER16",       2,  true,  false, false, false },            // 3
   { "INTEGER32",       4,  true,  false, false, false },            // 4
   { "UNSIGNED8",       1,  false, true,  false, false },            // 5
   { "UNSIGNED16",      2,  false, true,  false, false },            // 6
   { "UNSIGNED32",      4,  false, true,  false, false },            // 7
   { "REAL32",          4,  false, false, true,  false },            // 8
   { "VISIBLE_STRING",  0,  false, false, false, true  },            // 9
   { "OCTET_STRING",    0,  false, false, false, false },            // a
   { "UNICODE_STRING",  0,  false, false, false, true  },            // b
   { "TIME_OF_DAY",     6,  false, false, false, false },            // c
   { "TIME_DIFFERENCE", 6,  false, false, false, false },            // d
   { "reserved",        0,  false, false, false, false },            // e
   { "DOMAIN",          0,  false, false, false, false },            // f
   { "INTEGER24",       3,  true,  false, false, false },            //10
   { "REAL64",          8,  false, false, true,  false },            //11
   { "INTEGER40",       5,  true,  false, false, false },            //12
   { "INTEGER48",       6,  true,  false, false, false },            //13
   { "INTEGER56",       6,  true,  false, false, false },            //14
   { "INTEGER64",       7,  true,  false, false, false },            //15
   { "UNSIGNED24",      3,  false, true,  false, false },            //16
   { "reserved",        0,  false, false, false, false },            //17
   { "UNSIGNED40",      5,  false, true,  false, false },            //18
   { "UNSIGNED48",      6,  false, true,  false, false },            //19
   { "UNSIGNED56",      7,  false, true,  false, false },            //1A
   { "UNSIGNED64",      8,  false, true,  false, false },            //1B
   { "reserved",        0,  false, false, false, false },            //1C
   { "reserved",        0,  false, false, false, false },            //1D
   { "reserved",        0,  false, false, false, false },            //1E
   { "reserved",        0,             false, false, false, false }, //1F
   { "PDO_COMMUNICATION_PARAMETER", 0, false, false, false, false }, //20
   { "PDO_MAPPING",     0,             false, false, false, false }, //21
   { "SDO_PARAMETER",   0,             false, false, false, false }, //22
   { "IDENTITY",        0,             false, false, false, false }  //23
};

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   compare two object dictionary entries

   Compare two object dictionary entries. The index is the sorting criterion.

   \param[in]     orc_Object          other object

   \return
   true   -> index of true is <  index of orc_Object
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::operator <(const C_OscCanOpenObject & orc_Object) const
{
   bool q_IsLess = false;

   if (u16_Index != orc_Object.u16_Index)
   {
      q_IsLess = (u16_Index < orc_Object.u16_Index);
   }
   else
   {
      //mu8_NumSubs != 0xFF: main index -> first in sorting !
      if (u8_NumSubs != 0xFF)
      {
         q_IsLess = true;
      }
      else
      {
         //same index -> check sub-index !
         q_IsLess = (u8_SubIndex < orc_Object.u8_SubIndex);
      }
   }
   return q_IsLess;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from CANopen EDS file

   Load CANopen EDS file and store contents in c_Objects.
   Objects in c_Objects will be sorted by index and sub-index.

   \param[in]     orc_File        File path

   \return
   C_NO_ERR    file loaded, content stored in c_Objects
   C_RANGE     file does not exist
   C_CONFIG    could not parse file (is it a valid EDS file ?)
                use GetLastErrorText() to get details
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::LoadFromFile(const C_SclString & orc_File)
{
   int32_t s32_Return;

   mc_LastError = "";
   c_Objects.resize(0);

   if (TglFileExists(orc_File) == false)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      C_SclIniFile c_IniFile(orc_File);

      //Get all "<index>sub<subindex>" sections from .ini to speed up parsing.
      C_SclStringList c_NamesOfAllSections;
      std::vector<uint32_t> c_ExistingEntrySections;

      //get all section names as strings:
      c_IniFile.ReadSections(&c_NamesOfAllSections);

      //we can have at most the same number of elements as we have sections:
      c_ExistingEntrySections.reserve(c_NamesOfAllSections.GetCount());
      for (uint32_t u32_Entry = 0; u32_Entry < c_NamesOfAllSections.GetCount(); u32_Entry++)
      {
         //We are only interested in the sections describing objects with subobjects.
         //Pattern: [<4 hex digits>sub<1 or 2 hex digits>, e.g. [12ABsubCD]
         //All other sections will be ignored.
         const C_SclString & rc_SectionName = c_NamesOfAllSections.Strings[u32_Entry];
         if ((rc_SectionName.Length() > (4 + 3)) && (rc_SectionName.Pos("sub") == 5))
         {
            try
            {
               const uint16_t u16_Index = static_cast<uint16_t>(("0x" + rc_SectionName.SubString(1, 4)).ToInt());
               //1 or 2 characters:
               const uint8_t u8_SubIndex = static_cast<uint8_t>(("0x" + rc_SectionName.SubString(8, 2)).ToInt());
               c_ExistingEntrySections.push_back((static_cast<uint32_t>(u16_Index) << 8U) + u8_SubIndex);
            }
            catch (...)
            {
               //Do not handle as an error. Most probably this is a malformed entry.
               //But there is a very low change this is a kind of valid section like e.g. "Foobsubs" which
               // does not contain an OD entry.
            }
         }
      }
      //Resize to actually used size (in correct EDS files not all sections contain OD entries matching the pattern)
      c_ExistingEntrySections.resize(c_ExistingEntrySections.size());
      //Sort vector by ID so we can search it later:
      std::sort(c_ExistingEntrySections.begin(), c_ExistingEntrySections.end());

      s32_Return = m_AppendEdsBlock("MandatoryObjects", c_ExistingEntrySections, c_IniFile);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_AppendEdsBlock("OptionalObjects", c_ExistingEntrySections, c_IniFile);
      }

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_AppendEdsBlock("ManufacturerObjects", c_ExistingEntrySections, c_IniFile);
      }

      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_CONFIG;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      C_SclString c_InfoError;
      s32_Return = this->c_InfoBlock.LoadFromFile(orc_File, c_InfoError);
      if (s32_Return != C_NO_ERR)
      {
         this->mc_LastError = c_InfoError;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //sort object dictionary:
      if (c_Objects.size() > 0)
      {
         std::sort(c_Objects.begin(), c_Objects.end());
      }
   }
   else
   {
      c_Objects.resize(0);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility for finding value in vector

   Use binary search to find specific entry in uint32_t vector.

   \param[in]     ou32_Value     value to find
   \param[in]     orc_AllValues  vector with sorted values to search in

   \retval  -1    value not found
            else  index of value within vector
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::mh_FindValue(const uint32_t ou32_Value,
                                                   const std::vector<uint32_t> & orc_AllValues)
{
   int32_t s32_Pos;
   int32_t s32_First;
   int32_t s32_Last;
   bool q_Found = false;

   s32_First = 0U;
   s32_Last  = static_cast<int32_t>(orc_AllValues.size());

   while (s32_First <= s32_Last)
   {
      s32_Pos = ((s32_First + s32_Last) / 2);
      if (ou32_Value > orc_AllValues[s32_Pos])
      {
         s32_First = s32_Pos + 1;
      }
      else if (ou32_Value < orc_AllValues[s32_Pos])
      {
         s32_Last = s32_Pos - 1;
      }
      else
      {
         //element found
         q_Found = true;
         break;
      }
   }
   return (q_Found == true) ? s32_Pos : -1;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get EDS/DCF data from one block

   Appends the information to "c_Objects".

   Does not try to read invalid EDS files; this could result in some subsequent inconsistencies.
   If the function aborts with an error the data in "c_Objects" can become inconsistent.

   Sub-indexes are not necessarily linear. i.e. it might be that sub-index 1 and 3 exist, but 2 not.

   \param[in]         orc_Blockname               Name of EDS/DCF block to read
   \param[in]         orc_ExistingEntrySections   List of all <index>sub<subindex> sections present in the ini file.
                                                  To compose: value = (index << 8) + subindex
   \param[in,out]     orc_IniFile                 Loaded ini file instance

   \return
   C_NO_ERR    data read and appended to c_Objects
   C_CONFIG    inconsistency between object list in block and stated number of entries
               invalid file contents
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::m_AppendEdsBlock(const C_SclString & orc_Blockname,
                                                       const std::vector<uint32_t> & orc_ExistingEntrySections,
                                                       C_SclIniFile & orc_IniFile)
{
   uint16_t u16_NumEntries;
   size_t x_IndexInList; //lint !e8080  //using type matching the used API
   int32_t s32_Return = C_NO_ERR;

   x_IndexInList = this->c_Objects.size();
   u16_NumEntries = orc_IniFile.ReadUint16(orc_Blockname, "SupportedObjects", 0);
   this->c_Objects.resize(this->c_Objects.size() + u16_NumEntries);
   for (int32_t s32_Loop = 0; s32_Loop < u16_NumEntries; s32_Loop++)
   {
      C_SclString c_Section;
      C_SclString c_Index;
      uint16_t u16_Index = 0U;
      const C_SclString c_Directive = C_SclString::IntToStr(s32_Loop + 1);

      c_Index = orc_IniFile.ReadString(orc_Blockname, c_Directive, "");
      if (c_Index == "")
      {
         mc_LastError = orc_Blockname + ": SupportedObjects inconsistent with object list !";
         s32_Return = C_CONFIG;
      }

      if (s32_Return == C_NO_ERR)
      {
         try
         {
            u16_Index = static_cast<uint16_t>(c_Index.ToInt());
            this->c_Objects[x_IndexInList].u16_Index = u16_Index;
         }
         catch (...)
         {
            mc_LastError = orc_Blockname + ": File contains non-numeric Index in SupportedObjects !";
            s32_Return = C_CONFIG;
         }
      }

      if (s32_Return == C_NO_ERR)
      {
         //now find all data associated with that index !
         C_SclStringList c_List;
         //strap the "0x"
         c_Section = c_Index.SubString(3, c_Index.Length());
         orc_IniFile.ReadSectionValues(c_Section, &c_List, false);
         s32_Return = m_GetObjectDescription(u16_Index, 0, false, c_List, this->c_Objects[x_IndexInList]);
      }

      if (s32_Return == C_NO_ERR)
      {
         uint16_t u16_NumSubs = static_cast<uint16_t>(this->c_Objects[x_IndexInList].u8_NumSubs);
         x_IndexInList++;
         //are there sub-indexes ?
         if (u16_NumSubs > 0)
         {
            //sub-index 0 -> contains information about highest supported sub-index!
            C_SclString c_SubSection = c_Section + "sub0";
            const uint8_t u8_HighestSupportedSubIndex = orc_IniFile.ReadUint8(c_SubSection, "DefaultValue", 0);
            if (u8_HighestSupportedSubIndex >= u16_NumSubs)
            {
               u16_NumSubs = static_cast<uint16_t>(u8_HighestSupportedSubIndex) + 1U;
            }

            for (uint16_t u16_Sub = 0U; u16_Sub < u16_NumSubs; u16_Sub++)
            {
               //compose uint32_t value to search for:
               const uint32_t u32_ComposedEntry = (static_cast<uint32_t>(u16_Index) << 8U) + u16_Sub;

               //Check if subsection exists by searching sorted list of sections:
               //Using this approach was prompted by a huge EDS file with many non-existing sections.
               //Brute force checking for all sections to exist, only to find out they don't, would waste a lot of time.
               //Binary search will come to that conclusion much faster.
               const int32_t s32_Pos = mh_FindValue(u32_ComposedEntry, orc_ExistingEntrySections);
               if (s32_Pos != -1)
               {
                  C_SclStringList c_List;
                  c_SubSection = c_Section + "sub" + C_SclString::IntToHex(u16_Sub, 1);
                  orc_IniFile.ReadSectionValues(c_SubSection, &c_List, false);

                  this->c_Objects.resize(this->c_Objects.size() + 1);
                  s32_Return = m_GetObjectDescription(u16_Index, static_cast<uint8_t>(u16_Sub), true, c_List,
                                                      this->c_Objects[x_IndexInList]);
                  if (s32_Return != C_NO_ERR)
                  {
                     break;
                  }
                  x_IndexInList++;
               }
               else
               {
                  //not an error: not all sub-indexes need to be present in an EDS file
               }
            }
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get information from one EDS/DCF section

   \param[in]     ou16_Index         index of object
   \param[in]     ou8_SubIndex       sub-index of object
   \param[in]     oq_IsSubIndex      true: object is a subindex object; false: object is the main thing
   \param[in]     orc_SectionValues  EDS/DCF section contents (in the form of "parameter=value" strings)
   \param[out]    orc_Object         read information

   \return
   C_NO_ERR    data read and placed into orc_Object
   C_CONFIG    invalid data (see mc_LastError for details)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::m_GetObjectDescription(const uint16_t ou16_Index, const uint8_t ou8_SubIndex,
                                                             const bool oq_IsSubIndex,
                                                             const C_SclStringList & orc_SectionValues,
                                                             C_OscCanOpenObject & orc_Object)
{
   int32_t s32_Return = C_NO_ERR;

   orc_Object.u16_Index   = ou16_Index;
   orc_Object.c_Name      = orc_SectionValues.Values("ParameterName").Trim();
   orc_Object.c_Access    = orc_SectionValues.Values("AccessType").UpperCase().Trim();
   if (oq_IsSubIndex == true)
   {
      orc_Object.u8_NumSubs  = 0xFFU; //0xFF: is_a_sub
      orc_Object.u8_SubIndex = ou8_SubIndex;
   }
   else
   {
      if (orc_SectionValues.Values("SubNumber") == "")
      {
         orc_Object.u8_NumSubs = 0U; //spec: may be empty if no sub-index exists
      }
      else
      {
         try
         {
            orc_Object.u8_NumSubs = static_cast<uint8_t>(orc_SectionValues.Values("SubNumber").ToInt());
         }
         catch (...)
         {
            mc_LastError.PrintFormatted("File contains non-numeric SubNumber for object %04X.%02X !",
                                        static_cast<uint32_t>(ou16_Index),
                                        static_cast<uint32_t>(ou8_SubIndex));
            s32_Return = C_CONFIG;
         }
      }
      orc_Object.u8_SubIndex = 0U;
   }

   if (s32_Return == C_NO_ERR)
   {
      if (orc_SectionValues.Values("DataType") == "")
      {
         orc_Object.u8_DataType = C_OscCanOpenObject::hu8_DATA_TYPE_DOMAIN; //optional for "DOMAIN" objects
      }
      else
      {
         try
         {
            orc_Object.u8_DataType = static_cast<uint8_t>(orc_SectionValues.Values("DataType").ToInt());
         }
         catch (...)
         {
            mc_LastError.PrintFormatted("File contains non-numeric DataType for object %04X.%02X !",
                                        static_cast<uint32_t>(ou16_Index),
                                        static_cast<uint32_t>(ou8_SubIndex));
            s32_Return = C_CONFIG;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      uint8_t u8_Size;
      orc_Object.DataTypeToTextAndSize(NULL, &u8_Size);
      orc_Object.SetSize(u8_Size);
      orc_Object.c_DefaultValue = orc_SectionValues.Values("DefaultValue").Trim();
      orc_Object.c_LowLimit = orc_SectionValues.Values("LowLimit").Trim();
      orc_Object.c_HighLimit = orc_SectionValues.Values("HighLimit").Trim();
      //stuff from DCF files:
      orc_Object.c_ParameterValue = orc_SectionValues.Values("ParameterValue").Trim();
      orc_Object.c_Denotation = orc_SectionValues.Values("Denotation").Trim();
      if (orc_SectionValues.Values("PDOMapping").IsEmpty())
      {
         orc_Object.q_IsMappableIntoPdo = false;
      }
      else
      {
         try
         {
            const int32_t s32_Value = orc_SectionValues.Values("PDOMapping").ToInt();
            if (s32_Value == 0)
            {
               orc_Object.q_IsMappableIntoPdo = false;
            }
            else if (s32_Value == 1)
            {
               orc_Object.q_IsMappableIntoPdo = true;
            }
            else
            {
               orc_Object.q_IsMappableIntoPdo = false;
               mc_LastError.PrintFormatted(
                  "Invalid boolean value \"%s\" found in entry \"PDOMapping\" for object %04X.%02X !",
                  orc_SectionValues.Values("PDOMapping").c_str(),
                  static_cast<uint32_t>(ou16_Index),
                  static_cast<uint32_t>(ou8_SubIndex));
               s32_Return = C_CONFIG;
            }
         }
         catch (...)
         {
            mc_LastError.PrintFormatted("Could not parse entry \"PDOMapping\" value \"%s\" for object %04X.%02X !",
                                        orc_SectionValues.Values("PDOMapping").c_str(),
                                        static_cast<uint32_t>(ou16_Index),
                                        static_cast<uint32_t>(ou8_SubIndex));
            s32_Return = C_CONFIG;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is section read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[in]   ou8_OdSubIndex   Object dictionary sub index
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::m_IsSectionRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                    const uint8_t ou8_OdSubIndex, bool & orq_IsRo) const
{
   int32_t s32_Retval = C_NO_ERR;
   const uint16_t u16_ObjectIndex =
      C_OscCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(ou16_PdoIndex, oq_MessageIsTx);
   const C_OscCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(u16_ObjectIndex, ou8_OdSubIndex);

   if (pc_Object != NULL)
   {
      orq_IsRo = pc_Object->IsWriteable() == false;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Does section exist

   \param[in]  ou16_PdoIndex     PDO index (0 = first PDO)
   \param[in]  oq_MessageIsTx    Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[in]  ou8_OdSubIndex    Object dictionary sub index

   \return
   Flags

   \retval   True    Section exists
   \retval   False   Section does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObjectDictionary::m_DoesSectionExist(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                      const uint8_t ou8_OdSubIndex) const
{
   bool q_Retval;
   const uint16_t u16_ObjectIndex =
      C_OscCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(ou16_PdoIndex, oq_MessageIsTx);
   const C_OscCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(u16_ObjectIndex, ou8_OdSubIndex);

   if (pc_Object != NULL)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------

uint16_t C_OscCanOpenObject::GetSize(void) const
{
   return this->mu16_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenObject::CalcHash(uint32_t & oru32_HashValue) const
{
   C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->u16_Index, sizeof(this->u16_Index), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->u8_SubIndex, sizeof(this->u8_SubIndex), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->u8_DataType, sizeof(this->u8_DataType), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_Access.c_str(), this->c_Access.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->u8_NumSubs, sizeof(this->u8_NumSubs), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_DefaultValue.c_str(), this->c_DefaultValue.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_ParameterValue.c_str(), this->c_ParameterValue.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_LowLimit.c_str(), this->c_LowLimit.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_HighLimit.c_str(), this->c_HighLimit.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(this->c_Denotation.c_str(), this->c_Denotation.Length(), oru32_HashValue);
   C_SclChecksums::CalcCRC32(&this->q_IsMappableIntoPdo, sizeof(this->q_IsMappableIntoPdo), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------

void C_OscCanOpenObject::SetSize(const uint16_t ou16_Size)
{
   this->mu16_Size = ou16_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name and size of object

   \param[out]  opc_Text    type of object in text form
   \param[out]  opu8_Size   size of object
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenObject::DataTypeToTextAndSize(C_SclString * const opc_Text, uint8_t * const opu8_Size) const
{
   if ((u8_DataType < mu8_NUM_DATA_TYPES) && (u8_DataType > hu8_DATA_TYPE_INVALID))
   {
      if (opc_Text != NULL)
      {
         *opc_Text = mac_TextAndSizeTable[u8_DataType].c_Text;
      }
      if (opu8_Size != NULL)
      {
         *opu8_Size = mac_TextAndSizeTable[u8_DataType].u8_Size;
      }
   }
   else
   {
      if (opc_Text != NULL)
      {
         *opc_Text  = "?\?\?";
      }
      if (opu8_Size != NULL)
      {
         *opu8_Size = 0;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanOpenObject::C_OscCanOpenObject() :
   mu16_Size(0),
   u16_Index(0),
   u8_SubIndex(0),
   u8_DataType(0),
   u8_NumSubs(0),
   q_IsMappableIntoPdo(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object may be read from

   \return
   true:   object has access type RO, RW, RWW, RWR or CONST
   false:  object has none of those access types
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsReadable(void) const
{
   const C_SclString c_Help = c_Access.UpperCase();

   return ((c_Help  == "RO") || (c_Help == "RW") || (c_Help == "RWW") || (c_Help == "RWR") || (c_Help == "CONST"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object may be written to

   \return
   true:   object has access type RW, RWW, RWR or WO
   false:  object has none of those access types
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsWriteable(void) const
{
   const C_SclString c_Help = c_Access.UpperCase();

   return ((c_Help == "RW") || (c_Help == "RWW") || (c_Help == "RWR") || (c_Help == "WO"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if object is mappable into a PDO

   \return
   Flags

   \retval   True    Object is mappable
   \retval   False   Object is not mappable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsMappableIntoPdo() const
{
   return this->q_IsMappableIntoPdo;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if object is mappable into a TPDO

   \return
   Flags

   \retval   True    Object is mappable
   \retval   False   Object is not mappable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsMappableIntoTxPdo() const
{
   const C_SclString c_Help = c_Access.UpperCase();

   return this->IsMappableIntoPdo() &&
          ((c_Help == "RO") || (c_Help == "RW") || (c_Help == "RWR") || (c_Help == "CONST"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if object is mappable into a RPDO

   \return
   Flags

   \retval   True    Object is mappable
   \retval   False   Object is not mappable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsMappableIntoRxPdo() const
{
   const C_SclString c_Help = c_Access.UpperCase();

   return this->IsMappableIntoPdo() && ((c_Help == "WO") || (c_Help == "RW") || (c_Help == "RWW"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object is of signed integer type

   \return
   true:   object is of signed integer type
   false:  it is not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsIntegerDataType(void) const
{
   bool q_IsType = false;

   if ((u8_DataType < mu8_NUM_DATA_TYPES) && (u8_DataType != hu8_DATA_TYPE_INVALID))
   {
      q_IsType = mac_TextAndSizeTable[u8_DataType].q_IsInteger;
   }
   return q_IsType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object is of unsigned type

   \return
   true:   object is of unsigned type
   false:  it is not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsUnsignedDataType(void) const
{
   bool q_IsType = false;

   if ((u8_DataType < mu8_NUM_DATA_TYPES) && (u8_DataType != hu8_DATA_TYPE_INVALID))
   {
      q_IsType = mac_TextAndSizeTable[u8_DataType].q_IsUnsigned;
   }
   return q_IsType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object is of REAL32 or REAL64 type

   \return
   true:   object is of REAL32 or REAL64 type
   false:  it is not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsFloatDataType(void) const
{
   bool q_IsType = false;

   if ((u8_DataType < mu8_NUM_DATA_TYPES) && (u8_DataType != hu8_DATA_TYPE_INVALID))
   {
      q_IsType = mac_TextAndSizeTable[u8_DataType].q_IsFloat;
   }
   return q_IsType;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object is of VISIBLE_STRING or UNICODE_STRING type

   \return
   true:   object is of VISIBLE_STRING or UNICODE_STRING type
   false:  it is not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObject::IsStringDataType(void) const
{
   bool q_IsType = false;

   if ((u8_DataType < mu8_NUM_DATA_TYPES) && (u8_DataType != hu8_DATA_TYPE_INVALID))
   {
      q_IsType = mac_TextAndSizeTable[u8_DataType].q_IsString;
   }
   return q_IsType;
}

//----------------------------------------------------------------------------------------------------------------------

C_SclString C_OscCanOpenObjectDictionary::GetLastErrorText(void) const
{
   return mc_LastError;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenObjectDictionary::CalcHash(uint32_t & oru32_HashValue) const
{
   this->c_InfoBlock.CalcHash(oru32_HashValue);
   for (uint32_t u32_It = 0UL; u32_It < this->c_Objects.size(); ++u32_It)
   {
      this->c_Objects[u32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is object heartbeat consumer supported

   \return
   0 : None supported
   Else : Number of supported heartbeat consumers
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscCanOpenObjectDictionary::GetNumHeartbeatConsumers() const
{
   uint8_t u8_Retval = 0;
   const C_OscCanOpenObject * const pc_Object = this->GetCanOpenObject(hu16_OD_INDEX_HEARTBEAT_CONSUMER);

   if (pc_Object != NULL)
   {
      u8_Retval = pc_Object->u8_NumSubs;
      if (u8_Retval > 0U)
      {
         //Don't count the first one (sub index counter)
         u8_Retval -= 1U;
      }
   }
   return u8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is heartbeat consumer read-only

   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsHeartbeatConsumerRo(bool & orq_IsRo) const
{
   int32_t s32_Retval = C_NO_ERR;
   // Checking the first consumer for read only because this is the entry which will be used for the user specified
   // value when it is not read-only
   const C_OscCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(hu16_OD_INDEX_HEARTBEAT_CONSUMER, 1U);

   if (pc_Object != NULL)
   {
      orq_IsRo = pc_Object->IsWriteable() == false;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is heartbeat producer supported

   \return
   Flags

   \retval   True    Heartbeat producer object present
   \retval   False   Heartbeat producer object not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObjectDictionary::IsHeartbeatProducerSupported() const
{
   return CheckObjectPresentByIndex(hu16_OD_INDEX_HEARTBEAT_PRODUCER, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is heartbeat producer read-only

   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsHeartbeatProducerRo(bool & orq_IsRo) const
{
   int32_t s32_Retval = C_NO_ERR;
   const C_OscCanOpenObject * const pc_Object = this->GetCanOpenObject(hu16_OD_INDEX_HEARTBEAT_PRODUCER);

   if (pc_Object != NULL)
   {
      orq_IsRo = pc_Object->IsWriteable() == false;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Does device support EMCY

  \return
  Flags

   \retval   True    EMCY object present
   \retval   False   EMCY object not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObjectDictionary::IsEmcySupported() const
{
   return CheckObjectPresentByIndex(hu16_OD_INDEX_EMCY, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get granularity

   \return
   Granularity
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscCanOpenObjectDictionary::GetGranularity() const
{
   return this->c_InfoBlock.GetGranularity();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all available factory settings sub indices

   \return
   All available factory settings sub indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::set<uint8_t> C_OscCanOpenObjectDictionary::GetAllAvailableFactorySettingsSubIndices() const
{
   const std::set<uint8_t> c_Retval = this->GetAllAvailableSubIndices(0x1011U);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all available sub indices

   \param[in]  ou16_OdIndex  Index

   \return
   All available sub indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::set<uint8_t> C_OscCanOpenObjectDictionary::GetAllAvailableSubIndices(const uint16_t ou16_OdIndex) const
{
   std::set<uint8_t> c_Retval;

   for (uint32_t u32_It = 0UL; u32_It < this->c_Objects.size(); ++u32_It)
   {
      const C_OscCanOpenObject & rc_Object = this->c_Objects[u32_It];
      //Matching index
      if (rc_Object.u16_Index == ou16_OdIndex)
      {
         if (rc_Object.u8_NumSubs == C_OscCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
         {
            c_Retval.insert(rc_Object.u8_SubIndex);
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Does inhibit time section exist

   \param[in]  ou16_PdoIndex     PDO index (0 = first PDO)
   \param[in]  oq_MessageIsTx    Flag if message is TX (for CANopen manager usage this flag is inverse)

   \return
   Flags

   \retval   True    Section exists
   \retval   False   Section does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObjectDictionary::DoesInhibitTimeSectionExist(const uint16_t ou16_PdoIndex,
                                                               const bool oq_MessageIsTx) const
{
   return this->m_DoesSectionExist(ou16_PdoIndex, oq_MessageIsTx,
                                   C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Does event timer section exist

   \param[in]  ou16_PdoIndex     PDO index (0 = first PDO)
   \param[in]  oq_MessageIsTx    Flag if message is TX (for CANopen manager usage this flag is inverse)

   \return
   Flags

   \retval   True    Section exists
   \retval   False   Section does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObjectDictionary::DoesEventTimerSectionExist(const uint16_t ou16_PdoIndex,
                                                              const bool oq_MessageIsTx) const
{
   return this->m_DoesSectionExist(ou16_PdoIndex, oq_MessageIsTx,
                                   C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is COB-ID read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsCobIdRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                bool & orq_IsRo) const
{
   const int32_t s32_Retval = this->m_IsSectionRo(ou16_PdoIndex, oq_MessageIsTx,
                                                  C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID,
                                                  orq_IsRo);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is inhibit time read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsInhibitTimeRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                      bool & orq_IsRo) const
{
   const int32_t s32_Retval = this->m_IsSectionRo(
      ou16_PdoIndex, oq_MessageIsTx,
      C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME,
      orq_IsRo);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is event timer read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsEventTimerRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                     bool & orq_IsRo) const
{
   const int32_t s32_Retval = this->m_IsSectionRo(
      ou16_PdoIndex, oq_MessageIsTx,
      C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER,
      orq_IsRo);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is transmission type read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsTransmissionTypeRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                           bool & orq_IsRo) const
{
   const int32_t s32_Retval = this->m_IsSectionRo(ou16_PdoIndex, oq_MessageIsTx,
                                                  C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE,
                                                  orq_IsRo);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is sync start value read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsSyncStartRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                    bool & orq_IsRo) const
{
   const int32_t s32_Retval = this->m_IsSectionRo(ou16_PdoIndex, oq_MessageIsTx,
                                                  C_OscCanOpenObjectDictionary::hu8_OD_SUB_INDEX_SYNC_START_VALUE,
                                                  orq_IsRo);

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is PDO mapping read-only

   \param[in]   ou16_PdoIndex    PDO index (0 = first PDO)
   \param[in]   oq_MessageIsTx   Flag if message is TX (for CANopen manager usage this flag is inverse)
   \param[out]  orq_IsRo         Is read-only

   \return
   STW error codes

   \retval   C_NO_ERR   Value found
   \retval   C_RANGE    Value missing
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscCanOpenObjectDictionary::IsPdoMappingRo(const uint16_t ou16_PdoIndex, const bool oq_MessageIsTx,
                                                     bool & orq_IsRo) const
{
   int32_t s32_Retval = C_NO_ERR;
   const uint16_t u16_ObjectIndex =
      C_OscCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(ou16_PdoIndex, oq_MessageIsTx) +
      C_OscCanOpenObjectDictionary::hu16_OD_PDO_MAPPING_OFFSET;
   const C_OscCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(u16_ObjectIndex, 0U);

   if (pc_Object != NULL)
   {
      orq_IsRo = pc_Object->IsWriteable() == false;
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen object

   Goes through the list of known object dictionary entries and returns a pointer to the object with the specified
    index.

   \param[in]  ou16_OdIndex   Object dictionary index

   \return
   NULL  CANopen object not found
   Else  Valid CANopen object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenObject * C_OscCanOpenObjectDictionary::GetCanOpenObject(const uint16_t ou16_OdIndex) const
{
   const C_OscCanOpenObject * pc_Retval = NULL;

   for (uint32_t u32_It = 0UL; u32_It < this->c_Objects.size(); ++u32_It)
   {
      const C_OscCanOpenObject & rc_Object = this->c_Objects[u32_It];
      //Matching index
      if (rc_Object.u16_Index == ou16_OdIndex)
      {
         if (rc_Object.u8_NumSubs != C_OscCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
         {
            pc_Retval = &this->c_Objects[u32_It];
            break;
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen sub index object

   \param[in]  ou16_OdIndex      Object dictionary index
   \param[in]  ou8_OdSubIndex    Object dictionary sub index

   \return
   NULL  CANopen sub index object not found
   Else  Valid CANopen sub index object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenObject * C_OscCanOpenObjectDictionary::GetCanOpenSubIndexObject(const uint16_t ou16_OdIndex,
                                                                                  const uint8_t ou8_OdSubIndex) const
{
   const C_OscCanOpenObject * pc_Retval = NULL;

   for (uint32_t u32_It = 0UL; u32_It < this->c_Objects.size(); ++u32_It)
   {
      const C_OscCanOpenObject & rc_Object = this->c_Objects[u32_It];
      //Matching index
      if (rc_Object.u16_Index == ou16_OdIndex)
      {
         //Matching sub index
         if (rc_Object.u8_SubIndex == ou8_OdSubIndex)
         {
            // 2 cases:
            // 1. search for sub index directly, if sub index entry in file
            // or
            // 2. search for object entry directly, if no sub index entries in file
            if (rc_Object.u8_NumSubs == C_OscCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
            {
               pc_Retval = &this->c_Objects[u32_It];
               break;
            }
         }
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check object present by index

   \param[in]  ou16_OdIndex      Object dictionary index
   \param[in]  ou8_OdSubIndex    Object dictionary sub index (0 means search for object directly)

   \return
   Flags

   \retval   True    Object present
   \retval   False   Object not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscCanOpenObjectDictionary::CheckObjectPresentByIndex(const uint16_t ou16_OdIndex,
                                                             const uint8_t ou8_OdSubIndex) const
{
   bool q_Retval = false;

   for (uint32_t u32_It = 0UL; u32_It < this->c_Objects.size(); ++u32_It)
   {
      const C_OscCanOpenObject & rc_Object = this->c_Objects[u32_It];
      //Matching index
      if (rc_Object.u16_Index == ou16_OdIndex)
      {
         //Matching sub index
         if (rc_Object.u8_SubIndex == ou8_OdSubIndex)
         {
            // 2 cases:
            // 1. search for sub index directly, if sub index entry in file
            // or
            // 2. search for object entry directly, if no sub index entries in file
            if ((rc_Object.u8_NumSubs == C_OscCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB) || (rc_Object.u8_NumSubs == 0))
            {
               q_Retval = true;
               break;
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get mappable objects

   \param[in,out]  orc_SubIndices   Sub indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscCanOpenObjectDictionary::GetMappableObjects(std::map<uint32_t, std::vector<uint32_t> > & orc_SubIndices)
const
{
   for (uint32_t u32_It = 0UL; u32_It < this->c_Objects.size(); ++u32_It)
   {
      const C_OscCanOpenObject & rc_Object = this->c_Objects[u32_It];
      //Matching index
      if (rc_Object.IsMappableIntoPdo())
      {
         //Matching sub index
         if (rc_Object.u8_NumSubs == C_OscCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
         {
            orc_SubIndices[rc_Object.u16_Index].push_back(rc_Object.u8_SubIndex);
         }
         else
         {
            orc_SubIndices[rc_Object.u16_Index].push_back(0U);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get CANopen object dictionary index for PDO

   \param[in]  ou16_PdoIndex     PDO index (0 = first PDO)
   \param[in]  oq_MessageIsTx    Flag if message is TX (for CANopen manager usage this flag is inverse)

   \return
   Object dictionary index
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(const uint16_t ou16_PdoIndex,
                                                                               const bool oq_MessageIsTx)
{
   uint16_t u16_Retval;

   if (oq_MessageIsTx)
   {
      u16_Retval = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO + ou16_PdoIndex;
   }
   else
   {
      u16_Retval = C_OscCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO + ou16_PdoIndex;
   }
   return u16_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
