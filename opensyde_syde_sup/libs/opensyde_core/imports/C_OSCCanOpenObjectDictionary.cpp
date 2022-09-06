//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen EDS / DCF file handler

   Provides functions to read information from CANopen EDS / DCF file.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h" //pre-compiled headers

#include <sstream>   //for std::istringstream
#include <algorithm> //for std::sort
#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLChecksums.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "CSCLString.h"
#include "CSCLIniFile.h"
#include "TGLFile.h"

//----------------------------------------------------------------------------------------------------------------------
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_TextAndSize
{
public:
   C_SCLString c_Text;
   uint8 u8_Size;

   bool q_IsInteger;
   bool q_IsUnsigned;
   bool q_IsFloat;
   bool q_IsString;
};

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
static const uint8 mu8_NUM_DATA_TYPES = 0x24U;

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
bool C_OSCCanOpenObject::operator <(const C_OSCCanOpenObject & orc_Object) const
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
sint32 C_OSCCanOpenObjectDictionary::LoadFromFile(const C_SCLString & orc_File)
{
   sint32 s32_Return;

   mc_LastError = "";
   c_Objects.SetLength(0);

   if (TGL_FileExists(orc_File) == false)
   {
      s32_Return = C_RANGE;
   }
   else
   {
      C_SCLIniFile c_IniFile(orc_File);

      s32_Return = m_AppendEDSBlock("MandatoryObjects", c_IniFile);

      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_AppendEDSBlock("OptionalObjects", c_IniFile);
      }
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = m_AppendEDSBlock("ManufacturerObjects", c_IniFile);
      }
      if (s32_Return != C_NO_ERR)
      {
         s32_Return = C_CONFIG;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      C_SCLString c_InfoError;
      s32_Return = this->c_InfoBlock.LoadFromFile(orc_File, c_InfoError);
      if (s32_Return != C_NO_ERR)
      {
         this->mc_LastError = c_InfoError;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      //sort object dictionary:
      if (c_Objects.GetLength() > 0)
      {
         std::sort(&c_Objects[0], &c_Objects[c_Objects.GetLength()]);
      }
   }
   else
   {
      c_Objects.SetLength(0);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get EDS/DCF data from one block

   Does not try to read invalid EDS files; this could result in some subsequent inconsistencies.
   If the function aborts with an error the data in "c_Objects" can become inconsistent.

   Sub-indexes are not necessarily linear. i.e. it might be that sub-index 1 and 3 exist, but 2 not.

   \param[in]     orc_File        File path

   \return
   C_NO_ERR    data read and appended to c_Objects
   C_CONFIG    inconsistency between object list in block and stated number of entries
               invalid file contents
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCCanOpenObjectDictionary::m_AppendEDSBlock(const C_SCLString & orc_Blockname, C_SCLIniFile & orc_IniFile)
{
   uint16 u16_NumEntries;
   sint32 s32_IndexInList;
   sint32 s32_Return = C_NO_ERR;

   s32_IndexInList = this->c_Objects.GetLength();
   u16_NumEntries = orc_IniFile.ReadUint16(orc_Blockname, "SupportedObjects", 0);
   this->c_Objects.IncLength(u16_NumEntries);
   for (sint32 s32_Loop = 0; s32_Loop < u16_NumEntries; s32_Loop++)
   {
      C_SCLString c_Section;
      C_SCLString c_Index;
      uint16 u16_Index = 0U;
      const C_SCLString c_Directive = C_SCLString::IntToStr(s32_Loop + 1);

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
            u16_Index = static_cast<uint16>(c_Index.ToInt());
            this->c_Objects[s32_IndexInList].u16_Index = u16_Index;
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
         C_SCLStringList c_List;
         //strap the "0x"
         c_Section = c_Index.SubString(3, c_Index.Length());
         orc_IniFile.ReadSectionValues(c_Section, &c_List, false);
         s32_Return = m_GetObjectDescription(u16_Index, 0, false, c_List, this->c_Objects[s32_IndexInList]);
      }

      if (s32_Return == C_NO_ERR)
      {
         uint8 u8_NumSubs = this->c_Objects[s32_IndexInList].u8_NumSubs;
         s32_IndexInList++;
         //are there sub-indexes ?
         if (u8_NumSubs > 0)
         {
            uint8 u8_HighestSupportedSubIndex;
            //sub-index 0 -> contains information about highest supported sub-index!
            C_SCLString c_SubSection = c_Section + "sub0";

            u8_HighestSupportedSubIndex = orc_IniFile.ReadUint8(c_SubSection, "DefaultValue", 0);
            if (u8_HighestSupportedSubIndex >= u8_NumSubs)
            {
               u8_NumSubs = static_cast<uint8>(u8_HighestSupportedSubIndex + 1);
            }

            for (uint16 u16_Sub = 0U; u16_Sub < u8_NumSubs; u16_Sub++)
            {
               c_SubSection = c_Section + "sub" + C_SCLString::IntToHex(u16_Sub, 1);
               //check if subsection exists
               if (orc_IniFile.SectionExists(c_SubSection) == true)
               {
                  C_SCLStringList c_List;
                  orc_IniFile.ReadSectionValues(c_SubSection, &c_List, false);

                  this->c_Objects.IncLength();
                  s32_Return = m_GetObjectDescription(u16_Index, static_cast<uint8>(u16_Sub), true, c_List,
                                                      this->c_Objects[s32_IndexInList]);
                  if (s32_Return != C_NO_ERR)
                  {
                     break;
                  }
                  s32_IndexInList++;
               }
               else
               {
                  //not an error: not all sub-indexes need to be present
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
sint32 C_OSCCanOpenObjectDictionary::m_GetObjectDescription(const uint16 ou16_Index, const uint8 ou8_SubIndex,
                                                            const bool oq_IsSubIndex,
                                                            const C_SCLStringList & orc_SectionValues,
                                                            C_OSCCanOpenObject & orc_Object)
{
   sint32 s32_Return = C_NO_ERR;

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
            orc_Object.u8_NumSubs = static_cast<uint8>(orc_SectionValues.Values("SubNumber").ToInt());
         }
         catch (...)
         {
            mc_LastError.PrintFormatted("File contains non-numeric SubNumber for object %04X.%02X !", ou16_Index,
                                        ou8_SubIndex);
            s32_Return = C_CONFIG;
         }
      }
      orc_Object.u8_SubIndex = 0;
   }

   if (s32_Return == C_NO_ERR)
   {
      if (orc_SectionValues.Values("DataType") == "")
      {
         orc_Object.u8_DataType = C_OSCCanOpenObject::hu8_DATA_TYPE_DOMAIN; //optional for "DOMAIN" objects
      }
      else
      {
         try
         {
            orc_Object.u8_DataType = static_cast<uint8>(orc_SectionValues.Values("DataType").ToInt());
         }
         catch (...)
         {
            mc_LastError.PrintFormatted("File contains non-numeric DataType for object %04X.%02X !", ou16_Index,
                                        ou8_SubIndex);
            s32_Return = C_CONFIG;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      uint8 u8_Size;
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
         orc_Object.q_IsMappableIntoPDO = false;
      }
      else
      {
         try
         {
            const sintn sn_Value = orc_SectionValues.Values("PDOMapping").ToInt();
            if (sn_Value == 0)
            {
               orc_Object.q_IsMappableIntoPDO = false;
            }
            else if (sn_Value == 1)
            {
               orc_Object.q_IsMappableIntoPDO = true;
            }
            else
            {
               orc_Object.q_IsMappableIntoPDO = false;
               mc_LastError.PrintFormatted(
                  "Invalid boolean value \"%s\" found in entry "
                  "\"PDOMapping\" for object %04X.%02X !", orc_SectionValues.Values(
                     "PDOMapping").c_str(), ou16_Index,
                  ou8_SubIndex);
               s32_Return = C_CONFIG;
            }
         }
         catch (...)
         {
            mc_LastError.PrintFormatted("Could not parse entry \"PDOMapping\" value \"%s\" for object %04X.%02X !", orc_SectionValues.Values(
                                           "PDOMapping").c_str(), ou16_Index,
                                        ou8_SubIndex);
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
sint32 C_OSCCanOpenObjectDictionary::m_IsSectionRo(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                                   const uint8 ou8_OdSubIndex, bool & orq_IsRo) const
{
   sint32 s32_Retval = C_NO_ERR;
   const uint16 u16_ObjectIndex = C_OSCCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(ou16_PdoIndex,
                                                                                                        oq_MessageIsTx);
   const C_OSCCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(u16_ObjectIndex, ou8_OdSubIndex);

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
bool C_OSCCanOpenObjectDictionary::m_DoesSectionExist(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                                      const uint8 ou8_OdSubIndex) const
{
   bool q_Retval;
   const uint16 u16_ObjectIndex = C_OSCCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(ou16_PdoIndex,
                                                                                                        oq_MessageIsTx);
   const C_OSCCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(u16_ObjectIndex, ou8_OdSubIndex);

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

uint16 C_OSCCanOpenObject::GetSize(void) const
{
   return this->mu16_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenObject::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u16_Index, sizeof(this->u16_Index),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_SubIndex, sizeof(this->u8_SubIndex),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_DataType, sizeof(this->u8_DataType),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Access.c_str(), this->c_Access.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->u8_NumSubs, sizeof(this->u8_NumSubs),
                                      oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_DefaultValue.c_str(), this->c_DefaultValue.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_ParameterValue.c_str(), this->c_ParameterValue.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_LowLimit.c_str(), this->c_LowLimit.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_HighLimit.c_str(), this->c_HighLimit.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Denotation.c_str(), this->c_Denotation.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->q_IsMappableIntoPDO, sizeof(this->q_IsMappableIntoPDO),
                                      oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------

void C_OSCCanOpenObject::SetSize(const uint16 ou16_Size)
{
   this->mu16_Size = ou16_Size;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name and size of object

   \param[out]  opc_Text    type of object in text form
   \param[out]  opu8_Size   size of object

   \return
   true:   object has access type RO, RW, RWW, RWR or CONST
   false:  object has none of those access types
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenObject::DataTypeToTextAndSize(C_SCLString * const opc_Text, uint8 * const opu8_Size) const
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
C_OSCCanOpenObject::C_OSCCanOpenObject() :
   mu16_Size(0),
   u16_Index(0),
   u8_SubIndex(0),
   u8_DataType(0),
   u8_NumSubs(0),
   q_IsMappableIntoPDO(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object may be read from

   \return
   true:   object has access type RO, RW, RWW, RWR or CONST
   false:  object has none of those access types
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanOpenObject::IsReadable(void) const
{
   const C_SCLString c_Help = c_Access.UpperCase();

   return ((c_Help  == "RO") || (c_Help == "RW") || (c_Help == "RWW") || (c_Help == "RWR") || (c_Help == "CONST"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object may be written to

   \return
   true:   object has access type RW, RWW, RWR or WO
   false:  object has none of those access types
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanOpenObject::IsWriteable(void) const
{
   const C_SCLString c_Help = c_Access.UpperCase();

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
bool C_OSCCanOpenObject::IsMappableIntoPDO() const
{
   return this->q_IsMappableIntoPDO;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check whether object is of signed integer type

   \return
   true:   object is of signed integer type
   false:  it is not
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanOpenObject::IsIntegerDataType(void) const
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
bool C_OSCCanOpenObject::IsUnsignedDataType(void) const
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
bool C_OSCCanOpenObject::IsFloatDataType(void) const
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
bool C_OSCCanOpenObject::IsStringDataType(void) const
{
   bool q_IsType = false;

   if ((u8_DataType < mu8_NUM_DATA_TYPES) && (u8_DataType != hu8_DATA_TYPE_INVALID))
   {
      q_IsType = mac_TextAndSizeTable[u8_DataType].q_IsString;
   }
   return q_IsType;
}

//----------------------------------------------------------------------------------------------------------------------

C_SCLString C_OSCCanOpenObjectDictionary::GetLastErrorText(void) const
{
   return mc_LastError;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with unit [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCCanOpenObjectDictionary::CalcHash(uint32 & oru32_HashValue) const
{
   this->c_InfoBlock.CalcHash(oru32_HashValue);
   for (sint32 s32_It = 0UL; s32_It < this->c_Objects.GetLength(); ++s32_It)
   {
      this->c_Objects[s32_It].CalcHash(oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is object heartbeat consumer supported

   \return
   0 : None supported
   Else : Number of supported heartbeat consumers
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_OSCCanOpenObjectDictionary::GetNumHeartbeatConsumers() const
{
   uint8 u8_Retval = 0;
   const C_OSCCanOpenObject * const pc_Object = this->GetCanOpenObject(hu16_OD_INDEX_HEARTBEAT_CONSUMER);

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
/*! \brief  Is heartbeat producer supported

   \return
   Flags

   \retval   True    Heartbeat producer object present
   \retval   False   Heartbeat producer object not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanOpenObjectDictionary::IsHeartbeatProducerSupported() const
{
   return CheckObjectPresentByIndex(hu16_OD_INDEX_HEARTBEAT_PRODUCER, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Does device support EMCY

  \return
  Flags

   \retval   True    EMCY object present
   \retval   False   EMCY object not present
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCCanOpenObjectDictionary::IsEMCYSupported() const
{
   return CheckObjectPresentByIndex(hu16_OD_INDEX_EMCY, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get granularity

   \return
   Granularity
*/
//----------------------------------------------------------------------------------------------------------------------
uint8 C_OSCCanOpenObjectDictionary::GetGranularity() const
{
   return this->c_InfoBlock.GetGranularity();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all available factory settings sub indices

   \return
   All available factory settings sub indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::set<uint8> C_OSCCanOpenObjectDictionary::GetAllAvailableFactorySettingsSubIndices() const
{
   const std::set<uint8> c_Retval = this->GetAllAvailableSubIndices(0x1011U);

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all available sub indices

   \param[in]  ou16_OdIndex  Index

   \return
   All available sub indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::set<uint8> C_OSCCanOpenObjectDictionary::GetAllAvailableSubIndices(const uint16 ou16_OdIndex) const
{
   std::set<uint8> c_Retval;

   for (sint32 s32_It = 0UL; s32_It < this->c_Objects.GetLength(); ++s32_It)
   {
      const C_OSCCanOpenObject & rc_Object = this->c_Objects[s32_It];
      //Matching index
      if (rc_Object.u16_Index == ou16_OdIndex)
      {
         if (rc_Object.u8_NumSubs == C_OSCCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
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
bool C_OSCCanOpenObjectDictionary::DoesInhibitTimeSectionExist(const uint16 ou16_PdoIndex,
                                                               const bool oq_MessageIsTx) const
{
   return this->m_DoesSectionExist(ou16_PdoIndex, oq_MessageIsTx,
                                   C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME);
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
bool C_OSCCanOpenObjectDictionary::DoesEventTimerSectionExist(const uint16 ou16_PdoIndex,
                                                              const bool oq_MessageIsTx) const
{
   return this->m_DoesSectionExist(ou16_PdoIndex, oq_MessageIsTx,
                                   C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER);
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
sint32 C_OSCCanOpenObjectDictionary::IsCobIdRo(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                               bool & orq_IsRo) const
{
   const sint32 s32_Retval = this->m_IsSectionRo(ou16_PdoIndex, oq_MessageIsTx,
                                                 C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_COB_ID,
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
sint32 C_OSCCanOpenObjectDictionary::IsInhibitTimeRo(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                                     bool & orq_IsRo) const
{
   const sint32 s32_Retval = this->m_IsSectionRo(
      ou16_PdoIndex, oq_MessageIsTx,
      C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_INHIBIT_TIME,
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
sint32 C_OSCCanOpenObjectDictionary::IsEventTimerRo(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                                    bool & orq_IsRo) const
{
   const sint32 s32_Retval = this->m_IsSectionRo(
      ou16_PdoIndex, oq_MessageIsTx,
      C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_EVENT_TIMER,
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
sint32 C_OSCCanOpenObjectDictionary::IsTransmissionTypeRo(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                                          bool & orq_IsRo) const
{
   const sint32 s32_Retval = this->m_IsSectionRo(ou16_PdoIndex, oq_MessageIsTx,
                                                 C_OSCCanOpenObjectDictionary::hu8_OD_SUB_INDEX_TRANSMISSION_TYPE,
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
sint32 C_OSCCanOpenObjectDictionary::IsPDOMappingRo(const uint16 ou16_PdoIndex, const bool oq_MessageIsTx,
                                                    bool & orq_IsRo) const
{
   sint32 s32_Retval = C_NO_ERR;
   const uint16 u16_ObjectIndex =
      C_OSCCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(ou16_PdoIndex, oq_MessageIsTx) +
      C_OSCCanOpenObjectDictionary::hu16_OD_PDO_MAPPING_OFFSET;
   const C_OSCCanOpenObject * const pc_Object = this->GetCanOpenSubIndexObject(u16_ObjectIndex, 0U);

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
   NULL CANopen object not found
   Else Valid CANopen object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCCanOpenObject * C_OSCCanOpenObjectDictionary::GetCanOpenObject(const uint16 ou16_OdIndex) const
{
   const C_OSCCanOpenObject * pc_Retval = NULL;

   for (sint32 s32_It = 0UL; s32_It < this->c_Objects.GetLength(); ++s32_It)
   {
      const C_OSCCanOpenObject & rc_Object = this->c_Objects[s32_It];
      //Matching index
      if (rc_Object.u16_Index == ou16_OdIndex)
      {
         if (rc_Object.u8_NumSubs != C_OSCCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
         {
            pc_Retval = &this->c_Objects[s32_It];
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
   NULL CANopen sub index object not found
   Else Valid CANopen sub index object
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OSCCanOpenObject * C_OSCCanOpenObjectDictionary::GetCanOpenSubIndexObject(const uint16 ou16_OdIndex,
                                                                                  const uint8 ou8_OdSubIndex) const
{
   const C_OSCCanOpenObject * pc_Retval = NULL;

   for (sint32 s32_It = 0UL; s32_It < this->c_Objects.GetLength(); ++s32_It)
   {
      const C_OSCCanOpenObject & rc_Object = this->c_Objects[s32_It];
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
            if (rc_Object.u8_NumSubs == C_OSCCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
            {
               pc_Retval = &this->c_Objects[s32_It];
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
bool C_OSCCanOpenObjectDictionary::CheckObjectPresentByIndex(const uint16 ou16_OdIndex,
                                                             const uint8 ou8_OdSubIndex) const
{
   bool q_Retval = false;

   for (sint32 s32_It = 0UL; s32_It < this->c_Objects.GetLength(); ++s32_It)
   {
      const C_OSCCanOpenObject & rc_Object = this->c_Objects[s32_It];
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
            if ((rc_Object.u8_NumSubs == C_OSCCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB) || (rc_Object.u8_NumSubs == 0))
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
void C_OSCCanOpenObjectDictionary::GetMappableObjects(std::map<uint32, std::vector<uint32> > & orc_SubIndices)
const
{
   for (sint32 s32_It = 0UL; s32_It < this->c_Objects.GetLength(); ++s32_It)
   {
      const C_OSCCanOpenObject & rc_Object = this->c_Objects[s32_It];
      //Matching index
      if (rc_Object.IsMappableIntoPDO())
      {
         //Matching sub index
         if (rc_Object.u8_NumSubs == C_OSCCanOpenObject::hu8_NUM_SUBS_WE_ARE_A_SUB)
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
uint16 C_OSCCanOpenObjectDictionary::h_GetCanOpenObjectDictionaryIndexForPdo(const uint16 ou16_PdoIndex,
                                                                             const bool oq_MessageIsTx)
{
   uint16 u16_Retval;

   if (oq_MessageIsTx)
   {
      u16_Retval = C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_TX_PDO + ou16_PdoIndex;
   }
   else
   {
      u16_Retval = C_OSCCanOpenObjectDictionary::hu16_OD_INDEX_FIRST_RX_PDO + ou16_PdoIndex;
   }
   return u16_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
