#ifndef CKFXVARIABLEBASEHPP
#define CKFXVARIABLEBASEHPP

#include "stwtypes.hpp"
#include "C_SclDynamicArray.hpp"
#include "C_SclString.hpp"
#include "DiagLib_config.hpp"

//---------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//---------------------------------------------------------------------------
///type of variable transmission
enum E_TransmissionType
{
   eSRR,   //single shot
   eTCRR,  //cyclic
   eECRR,  //event triggered
   eTCRRTS //cyclic with timestamp
};

//---------------------------------------------------------------------------

class C_KFXVariableTypeDefinition
{
public:
   const char_t * pcn_WrittenName; //name as string
   bool q_IsArray;
   bool q_IsNumber; //false = string
   bool q_IsFloat;
   uint8_t u8_BaseType; //underlying type (for arrays)
};

//---------------------------------------------------------------------------

//constants
const uint8_t KFX_NUM_ACCESS_GROUPS = 10U;

//RAM/EEPROM constants
const uint8_t KFX_DATA_TYPE_NOVAR    =  0x00U; //not a valid variable
const uint8_t KFX_DATA_TYPE_UINT8    =  0x01U;
const uint8_t KFX_DATA_TYPE_SINT8    =  0x02U;
const uint8_t KFX_DATA_TYPE_SINT16   =  0x03U;
const uint8_t KFX_DATA_TYPE_UINT16   =  0x04U;
const uint8_t KFX_DATA_TYPE_SINT32   =  0x05U;
const uint8_t KFX_DATA_TYPE_UINT32   =  0x06U;
const uint8_t KFX_DATA_TYPE_ASINT8   =  0x07U;
const uint8_t KFX_DATA_TYPE_CRC      =  0x08U;
const uint8_t KFX_DATA_TYPE_AUINT8   =  0x09U;
const uint8_t KFX_DATA_TYPE_FLOAT32  =  0x0AU;
const uint8_t KFX_DATA_TYPE_FLOAT64  =  0x0BU;
const uint8_t KFX_DATA_TYPE_ASINT16  =  0x0CU;
const uint8_t KFX_DATA_TYPE_AUINT16  =  0x0DU;
const uint8_t KFX_DATA_TYPE_ASINT32  =  0x0EU;
const uint8_t KFX_DATA_TYPE_AUINT32  =  0x0FU;
const uint8_t KFX_DATA_TYPE_AFLOAT32 =  0x10U;
const uint8_t KFX_DATA_TYPE_AFLOAT64 =  0x11U;
const uint8_t KFX_DATA_TYPE_SINT64   =  0x12U;
const uint8_t KFX_DATA_TYPE_UINT64   =  0x13U;
const uint8_t KFX_DATA_TYPE_ASINT64  =  0x14U;
const uint8_t KFX_DATA_TYPE_AUINT64  =  0x15U;
const uint8_t KFX_NUM_DATA_TYPES     =  0x16U;
//old names for compatibility:
const uint8_t KFX_DATA_TYPE_BYTE     =  KFX_DATA_TYPE_UINT8;
const uint8_t KFX_DATA_TYPE_CHAR     =  KFX_DATA_TYPE_SINT8;
const uint8_t KFX_DATA_TYPE_INT      =  KFX_DATA_TYPE_SINT16;
const uint8_t KFX_DATA_TYPE_WORD     =  KFX_DATA_TYPE_UINT16;
const uint8_t KFX_DATA_TYPE_LONG     =  KFX_DATA_TYPE_SINT32;
const uint8_t KFX_DATA_TYPE_DWORD    =  KFX_DATA_TYPE_UINT32;
const uint8_t KFX_DATA_TYPE_STRING   =  KFX_DATA_TYPE_ASINT8;
const uint8_t KFX_DATA_TYPE_AOBYTE   =  KFX_DATA_TYPE_AUINT8;
const uint8_t KFX_DATA_TYPE_FLOAT    =  KFX_DATA_TYPE_FLOAT32;

const uint8_t KFX_DATA_TYPE_INVALID  =  0xFFU;

const uint8_t KFX_DATA_ACCESS_RW        =   0U;
const uint8_t KFX_DATA_ACCESS_RO        =   1U;
const uint8_t KFX_DATA_ACCESS_WO        =   2U;
const uint8_t KFX_DATA_ACCESS_INVISIBLE =   3U;
const uint8_t KFX_DATA_ACCESS_INVALID   = 255U;

const uint8_t KFX_DATA_MAX_NUM_LANGUAGES = 5U;

//matlab/simulink specific parameters:
const uint8_t KFX_DATA_CLASS_SIGNAL     = 0U;
const uint8_t KFX_DATA_CLASS_PARAMETER  = 1U;

extern const C_KFXVariableTypeDefinition gatRAMParameterTypes[KFX_NUM_DATA_TYPES];

const int32_t KFX_FACTOR_VALUE_100_PERCENT = 10000;

//---------------------------------------------------------------------------

///KEFEX variable description base class
class C_KFXVariableBase
{
private:
   uint8_t mu8_ActAccess; //actual access rights based on the current user group(s)
   uint32_t mu32_Size;

   static bool mh_IsValueInRange(const float64_t of64_Value, const float64_t of64_Min, const float64_t of64_Max);

   static float64_t mh_GetFloatData(const uint8_t ou8_Type, const uint8_t * const opu8_Data);
   static int64_t mh_GetNumericData(const uint8_t ou8_Type, const uint8_t * const opu8_Data);
   static void mh_SetNumericData(const uint32_t ou32_Size, uint8_t * const opu8_Data, const int64_t os64_Value);
   static void mh_SetFloatData(const uint8_t ou8_Type, uint8_t * const opu8_Data, const float64_t of64_Value);

   void m_CheckArrayAccessParams(const uint32_t ou32_ArrayIndex, uint32_t & oru32_ByteOffset) const;
   void m_CheckArrayAccessParamsDefault(const uint16_t ou16_DefaultIndex, const uint32_t ou32_ArrayIndex,
                                        uint32_t & oru32_ByteOffset) const;

   uint8_t m_GetEntryTypeByArrayType(void) const;

   stw::scl::C_SclDynamicArray<uint8_t> mc_MinValue;
   stw::scl::C_SclDynamicArray<uint8_t> mc_MaxValue;

public:
   C_KFXVariableBase(void);
   C_KFXVariableBase(const C_KFXVariableBase & orc_Source);
   virtual ~C_KFXVariableBase();

   C_KFXVariableBase & operator =(const C_KFXVariableBase & orc_Source);

   //Simple fields:
   stw::scl::C_SclString c_Name;
   stw::scl::C_SclString c_Unit;
   stw::scl::C_SclString ac_Comments[KFX_DATA_MAX_NUM_LANGUAGES];

   uint8_t u8_Class;     ///< matlab/simulink variable class; 0 = signal; 1 = parameter
   uint32_t u32_Address; ///< address (in EEPROM or ROM)

   uint8_t * pu8_Value;
   uint8_t u8_Type;

   //min/max handling
   void SetMinMaxToMaximum(void); //set min/max values to maximum ranges allowed by data type
   float64_t GetFloatMin(void) const;
   float64_t GetFloatMax(void) const;
   void SetFloatMin(const float64_t of64_Min);
   void SetFloatMax(const float64_t of64_Max);
   int64_t GetNumericMin(void) const;
   //lint -sem(stw::diag_lib::C_KFXVariableBase::GetNumericMax,pure)
   int64_t GetNumericMax(void) const;
   void SetNumericMin(const int64_t os64_Min);
   void SetNumericMax(const int64_t os64_Max);
   float64_t GetFloatMinFromArray(const uint32_t ou32_ArrayIndex) const;
   float64_t GetFloatMaxFromArray(const uint32_t ou32_ArrayIndex) const;
   void SetFloatMinInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Min);
   void SetFloatMaxInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Max);
   int64_t GetNumericMinFromArray(const uint32_t ou32_ArrayIndex) const;
   int64_t GetNumericMaxFromArray(const uint32_t ou32_ArrayIndex) const;
   void SetNumericMinInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Min);
   void SetNumericMaxInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Max);
   //direct access to min/max array:
   const stw::scl::C_SclDynamicArray<uint8_t> & GetMinReference(void) const;
   const stw::scl::C_SclDynamicArray<uint8_t> & GetMaxReference(void) const;
   stw::scl::C_SclDynamicArray<uint8_t> & GetMinReference(void);
   stw::scl::C_SclDynamicArray<uint8_t> & GetMaxReference(void);

   stw::scl::C_SclDynamicArray<stw::scl::C_SclDynamicArray<uint8_t> > aau8_Defaults;

   uint8_t u8_ScalingDigits;
   int32_t s32_ScalingFactor;

   uint8_t au8_Access[KFX_NUM_ACCESS_GROUPS]; ///< access rights for all groups

   bool q_LocationRAM; //true  -> a real RAM variable
                       //false -> function call or result

   E_TransmissionType e_DefTransmissionType;
   uint16_t u16_DefInterval;
   uint32_t u32_DefLowerHysteresis;
   uint32_t u32_DefUpperHysteresis;

   //transient fields holding the actual state of the variable:
   bool q_ValueChanged;
   uint32_t u32_TimeStamp;
   bool q_TimeStamped;
   bool q_ValueValid;
   bool q_CyclicTransmissionActive;

   //access right handling
   uint8_t GetCurrentAccessRights(void) const;
   bool IsReadable(void) const;
   bool IsWriteable(void) const;
   void RecalcActAccess(const bool (&orq_AccessLevels)[KFX_NUM_ACCESS_GROUPS + 1]);

   //variable information access functions
   void SetSize(const uint32_t ou32_Size);
   //lint -sem(stw::diag_lib::C_KFXVariableBase::GetSize,pure)
   uint32_t GetSize(void) const;

   //handling of Value
   int64_t GetNumericValue(void) const;
   void SetNumericValue(const int64_t os64_Value);
   float64_t GetFloatValue(void) const;
   void SetFloatValue(const float64_t of64_Value);
   void ClearValue(void);
   stw::scl::C_SclString GetStringValue(void) const;
   void SetStringValue(const stw::scl::C_SclString & orc_Value);
   //array values:
   int64_t GetNumericValueFromArray(const uint32_t ou32_ArrayIndex) const;
   float64_t GetFloatValueFromArray(const uint32_t ou32_ArrayIndex) const;
   void SetNumericValueInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Value);
   void SetFloatValueInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Value);

   //handling of Defaults
   uint16_t GetNumDefaults(void) const;
   void SetNumDefaults(const uint16_t ou16_NumDefaults);

   void CopyDefaultToValue(const uint16_t ou16_DefaultIndex = 0U);
   void CopyValueToDefault(const uint16_t ou16_DefaultIndex = 0U);
   bool DefaultMatchesValue(const uint16_t ou16_DefaultIndex = 0U) const;
   int64_t GetNumericDefault(const uint16_t ou16_DefaultIndex = 0U) const;
   void SetNumericDefault(const int64_t os64_Value, const uint16_t ou16_DefaultIndex = 0U);
   float64_t GetFloatDefault(const uint16_t ou16_DefaultIndex = 0U) const;
   void SetFloatDefault(const float64_t of64_Value, const uint16_t ou16_DefaultIndex = 0U);
   void ClearDefault(const uint16_t ou16_DefaultIndex = 0U);
   stw::scl::C_SclString GetStringDefault(const uint16_t ou16_DefaultIndex = 0U) const;
   void SetStringDefault(const stw::scl::C_SclString & orc_Value, const uint16_t ou16_DefaultIndex = 0U);
   //array defaults:
   int64_t GetNumericDefaultFromArray(const uint32_t ou32_ArrayIndex, const uint16_t ou16_DefaultIndex = 0U) const;
   float64_t GetFloatDefaultFromArray(const uint32_t ou32_ArrayIndex, const uint16_t ou16_DefaultIndex = 0U) const;
   void SetNumericDefaultInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Value,
                                 const uint16_t ou16_DefaultIndex = 0U);
   void SetFloatDefaultInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Value,
                               const uint16_t ou16_DefaultIndex = 0U);

   //functions supporting multiple sets of defaults:
   void ClearAllDefaults(void);

   //size of one array element:
   uint8_t GetSizeOfArrayElement(void) const;
   //number of elements in array:
   uint32_t GetNumberOfArrayElements(void) const;

   void CalcCRCOverEntry(uint16_t & oru16_CRC, const bool oq_SkipValue) const;

   //utility functions:
   bool CheckMinMax(void) const;
   //lint -sem(stw::diag_lib::C_KFXVariableBase::IsNumericValue,pure)
   bool IsNumericValue(void) const;

   bool IsArrayType(void) const;
   bool IsBinaryArrayType(void) const;
   //lint -sem(stw::diag_lib::C_KFXVariableBase::IsFloatType,pure)
   bool IsFloatType(void) const;

   // functions not for public use (may be dropped):
   stw::scl::C_SclString GetTypeName(void) const;
   stw::scl::C_SclString GetTypeDependentValueString(const bool oq_Hex, const bool oq_LeadingZeroes = false) const;
   stw::scl::C_SclString GetTypeDependentDefaultString(const bool oq_Hex, const bool oq_LeadingZeroes = false,
                                                       const uint16_t ou16_DefaultIndex = 0U) const;
};

//---------------------------------------------------------------------------
}
}

#endif
