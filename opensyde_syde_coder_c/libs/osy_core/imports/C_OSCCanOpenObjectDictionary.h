//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CANopen EDS / DCF file handler

   see .cpp file for details

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANOPENEDSDCFH
#define C_OSCCANOPENEDSDCFH

/* -- Includes ------------------------------------------------------------ */

#include "stwtypes.h"
#include "CSCLString.h"
#include "CSCLIniFile.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///container for on object dictionary entry
class C_OSCCanOpenObject
{
private:
   stw_types::uint16 mu16_Size; //size in bytes

public:
   //data types according to DS302 V4.02
   static const stw_types::uint8 hu8_DATA_TYPE_INVALID          = 0x00U;
   static const stw_types::uint8 hu8_DATA_TYPE_BOOLEAN          = 0x01U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER8         = 0x02U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER16        = 0x03U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER32        = 0x04U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED8        = 0x05U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED16       = 0x06U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED32       = 0x07U;
   static const stw_types::uint8 hu8_DATA_TYPE_REAL32           = 0x08U;
   static const stw_types::uint8 hu8_DATA_TYPE_VISIBLE_STRING   = 0x09U;
   static const stw_types::uint8 hu8_DATA_TYPE_OCTET_STRING     = 0x0AU;
   static const stw_types::uint8 hu8_DATA_TYPE_UNICODE_STRING   = 0x0BU;
   static const stw_types::uint8 hu8_DATA_TYPE_TIME_OF_DAY      = 0x0CU;
   static const stw_types::uint8 hu8_DATA_TYPE_TIME_DIFFERENCE  = 0x0DU;
   static const stw_types::uint8 hu8_DATA_TYPE_DOMAIN           = 0x0FU;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER24        = 0x10U;
   static const stw_types::uint8 hu8_DATA_TYPE_REAL64           = 0x11U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER40        = 0x12U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER48        = 0x13U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER56        = 0x14U;
   static const stw_types::uint8 hu8_DATA_TYPE_INTEGER64        = 0x15U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED24       = 0x16U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED40       = 0x18U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED48       = 0x19U;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED56       = 0x1AU;
   static const stw_types::uint8 hu8_DATA_TYPE_UNSIGNED64       = 0x1BU;
   static const stw_types::uint8 hu8_DATA_TYPE_PDO_COMMUNICATION_PARAMETER  = 0x20U;
   static const stw_types::uint8 hu8_DATA_TYPE_PDO_MAPPING      = 0x21U;
   static const stw_types::uint8 hu8_DATA_TYPE_SDO_PARAMETER    = 0x22U;
   static const stw_types::uint8 hu8_DATA_TYPE_IDENTITY         = 0x23U;

   stw_scl::C_SCLString c_Name;   //object name
   stw_types::uint16 u16_Index;   //object index
   stw_types::uint8 u8_SubIndex;  //object subindex
   stw_types::uint8 u8_DataType;  //CANopen data type
   stw_scl::C_SCLString c_Access; //access permission ("RW", "RO", "RWW", "CONST", "WO")
   stw_types::uint8 u8_NumSubs;   //number of sub-indexes (0xFF: we are a sub)
   stw_scl::C_SCLString c_DefaultValue;
   stw_scl::C_SCLString c_ParameterValue;
   stw_scl::C_SCLString c_Denotation;

   bool IsReadable(void) const;  //do the access rights permit reading ?
   bool IsWriteable(void) const; //do the access rights permit writing ?

   bool IsIntegerDataType(void) const;  //INTEGER (signed)
   bool IsUnsignedDataType(void) const; //BOOLEAN, UNSIGNED
   bool IsFloatDataType(void) const;    //REAL32 or REAL64
   bool IsStringDataType(void) const;   //VISIBLE_STRING, UNICODE_STRING

   void DataTypeToTextAndSize(stw_scl::C_SCLString * const opc_Text, stw_types::uint8 * const opu8_Size) const;

   void SetSize(const stw_types::uint16 ou16_Size);
   stw_types::uint16 GetSize(void) const;

   //less than operator for sorting:
   bool operator < (const C_OSCCanOpenObject & orc_Object) const;
};

//-----------------------------------------------------------------------------

///container for a CANopen object dictionary
class C_OSCCanOpenObjectDictionary
{
private:
   stw_types::sint32 m_AppendEDSBlock(const stw_scl::C_SCLString & orc_Blockname, stw_scl::C_SCLIniFile & orc_IniFile);
   stw_types::sint32 m_GetObjectDescription(const stw_types::uint16 ou16_Index, const stw_types::uint8 ou8_SubIndex,
                                            const bool oq_IsSubIndex,
                                            const stw_scl::C_SCLStringList & orc_SectionValues,
                                            C_OSCCanOpenObject & orc_Object);

   stw_scl::C_SCLString mc_LastError;

public:
   stw_scl::SCLDynamicArray<C_OSCCanOpenObject> c_Objects;

   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_File);
   stw_scl::C_SCLString GetLastErrorText(void) const;
};

/* -- Global Variables ---------------------------------------------------- */

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
