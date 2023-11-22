#include "precomp_headers.hpp" //pre-compiled headers

#include <cstring>
#include <cstdio>
#include <limits>

#include "DiagLib_config.hpp" //diaglib configuration

#include "stwtypes.hpp"
#include "CKFXVariableBase.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_SclChecksums.hpp"

//**************************************************************

using namespace stw::scl;
using namespace stw::diag_lib;

//**************************************************************

//using unions is not nice but more portable than reinterpret_casting
// (prevents problem on at least one target [MPC5200 GCC])
union U_Union32
{
   uint32_t u32_Value;
   float32_t f32_Value;
}; //lint !e1960  //cf. comment above

union U_Union64
{
   uint64_t u64_Value;
   float64_t f64_Value;
}; //lint !e1960  //cf. comment above

//**************************************************************

//must be the same sequence as the defines
const C_KFXVariableTypeDefinition stw::diag_lib::gatRAMParameterTypes[KFX_NUM_DATA_TYPES] =
{
   //name       array  number float  base_type
   {"",         false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_NOVAR
   {"UINT8",    false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_UINT8
   {"SINT8",    false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_SINT8
   {"SINT16",   false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_SINT16
   {"UINT16",   false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_UINT16
   {"SINT32",   false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_SINT32
   {"UINT32",   false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_UINT32
   {"STRING",   true,  false, false, KFX_DATA_TYPE_SINT8},   //KFX_DATA_TYPE_ASINT8
   {"",   false, true,  false, KFX_DATA_TYPE_NOVAR},         //KFX_DATA_TYPE_CRC
   {"AOBYTE",   true,  true,  false, KFX_DATA_TYPE_UINT8},   //KFX_DATA_TYPE_AUINT8
   {"FLOAT32",  false, true,  true,  KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_FLOAT32
   {"FLOAT64",  false, true,  true,  KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_FLOAT64
   {"ASINT16",  true,  true,  false, KFX_DATA_TYPE_SINT16},  //KFX_DATA_TYPE_ASINT16
   {"AUINT16",  true,  true,  false, KFX_DATA_TYPE_UINT16},  //KFX_DATA_TYPE_AUINT16
   {"ASINT32",  true,  true,  false, KFX_DATA_TYPE_SINT32},  //KFX_DATA_TYPE_ASINT32
   {"AUINT32",  true,  true,  false, KFX_DATA_TYPE_UINT32},  //KFX_DATA_TYPE_AUINT32
   {"AFLOAT32", true,  true,  true,  KFX_DATA_TYPE_FLOAT32}, //KFX_DATA_TYPE_AFLOAT32
   {"AFLOAT64", true,  true,  true,  KFX_DATA_TYPE_FLOAT64}, //KFX_DATA_TYPE_AFLOAT64
   {"SINT64",   false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_SINT64
   {"UINT64",   false, true,  false, KFX_DATA_TYPE_NOVAR},   //KFX_DATA_TYPE_UINT64
   {"ASINT64",  true,  true,  false, KFX_DATA_TYPE_SINT64},  //KFX_DATA_TYPE_ASINT64
   {"AUINT64",  true,  true,  false, KFX_DATA_TYPE_UINT64}  //KFX_DATA_TYPE_AUINT64
};

//-----------------------------------------------------------------------------
/*!
   \brief    Constructor

   Initialize class fields
*/
//-----------------------------------------------------------------------------
C_KFXVariableBase::C_KFXVariableBase(void)
{
   //strings will be automatically initialized to "" in their corresponding constructor !
   this->pu8_Value = NULL;
   this->mu32_Size = 0U;
   this->s32_ScalingFactor = KFX_FACTOR_VALUE_100_PERCENT;
   this->u8_ScalingDigits = 2U;

   this->u8_Type = KFX_DATA_TYPE_NOVAR;

   this->aau8_Defaults.SetLength(0);

   (void)std::memset(&au8_Access[0], KFX_DATA_ACCESS_INVALID, sizeof(au8_Access));

   this->u32_Address = 0U;
   this->q_LocationRAM = true;

   this->e_DefTransmissionType = eSRR;
   this->u16_DefInterval = 0U;
   this->u32_DefLowerHysteresis = 0U;
   this->u32_DefUpperHysteresis = 0U;

   this->u8_Class = 0U;

   //fill transient fields:
   this->q_ValueValid   = false;
   this->u32_TimeStamp  = 0U;
   this->q_TimeStamped  = false;
   this->q_ValueChanged = false;
   this->mu8_ActAccess = KFX_DATA_ACCESS_INVALID;
   this->q_CyclicTransmissionActive = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Destructor

   clean up
*/
//-----------------------------------------------------------------------------
C_KFXVariableBase::~C_KFXVariableBase()
{
   this->SetSize(0); //free dynamically allocated memory
}

//-----------------------------------------------------------------------------
/*!
   \brief    Update access permissions

   Update variable access permissions based on:
   - defined access rights (as aggregated in this class)
   - the groups the user belongs to (passed as parameter)

   For differing rights in different levels there must be a "ranking" of rights.
   This is defined as follows:  INV -> RO -> WO -> RW

   \param[in]   orq_AccessLevels   flags showing which groups the user belongs to
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::RecalcActAccess(const bool (&orq_AccessLevels)[KFX_NUM_ACCESS_GROUPS + 1])
{
   uint8_t u8_Index;

   this->mu8_ActAccess = KFX_DATA_ACCESS_INVISIBLE;
   for (u8_Index = 0; u8_Index < KFX_NUM_ACCESS_GROUPS; u8_Index++)
   {
      if (orq_AccessLevels[u8_Index] == true)
      {
         switch (au8_Access[u8_Index])
         {
         case KFX_DATA_ACCESS_RW:
            this->mu8_ActAccess = KFX_DATA_ACCESS_RW;
            break;
         case KFX_DATA_ACCESS_WO:
            this->mu8_ActAccess = KFX_DATA_ACCESS_WO;
            break;
         case KFX_DATA_ACCESS_RO:
            if (this->mu8_ActAccess != KFX_DATA_ACCESS_WO)
            {
               this->mu8_ActAccess = KFX_DATA_ACCESS_RO;
            }
            break;
         default:
            break;
         }
         if (this->mu8_ActAccess == KFX_DATA_ACCESS_RW)
         {
            //this is the max. possible right
            break;
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief    Check whether read permission is set

   Return whether reading the variable is permitted.

   \return
   true   -> variable may be read   \n
   false  -> variable may not be read
*/
//-----------------------------------------------------------------------------
bool C_KFXVariableBase::IsReadable(void) const
{
   return ((this->mu8_ActAccess == KFX_DATA_ACCESS_RO) || (this->mu8_ActAccess == KFX_DATA_ACCESS_RW));
}

//-----------------------------------------------------------------------------
/*!
   \brief    Check whether write permission is set

   Return whether writing to the variable is permitted.

   \return
   true   -> variable may be written     \n
   false  -> variable may not be written
*/
//-----------------------------------------------------------------------------
bool C_KFXVariableBase::IsWriteable(void) const
{
   return ((this->mu8_ActAccess == KFX_DATA_ACCESS_WO) || (this->mu8_ActAccess == KFX_DATA_ACCESS_RW));
}

//**************************************************************

uint8_t C_KFXVariableBase::GetCurrentAccessRights(void) const
{
   return this->mu8_ActAccess;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get size of variable

   Return size of variable in bytes

   \return
   size of variable in bytes
*/
//-----------------------------------------------------------------------------
uint32_t C_KFXVariableBase::GetSize(void) const
{
   return this->mu32_Size;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Set size of variable

   Define size of variable.

   Effects on:
   - value:   sets to 0
   - default: updates array lenghths (no change of data)
   - min/max: updates array lenghths (no change of data)

   \param[in]    ou32_Size   new variable size
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::SetSize(const uint32_t ou32_Size)
{
   int32_t s32_Index;

   try
   {
      if (ou32_Size == this->mu32_Size)
      {
         return; //no change
      }
      if (this->pu8_Value != NULL)
      {
         if (this->mu32_Size > 0U)
         {
            delete[] this->pu8_Value;
         }
         this->pu8_Value = NULL;
      }
      for (s32_Index = 0; s32_Index < this->aau8_Defaults.GetLength(); s32_Index++)
      {
         aau8_Defaults[s32_Index].SetLength(ou32_Size);
      }
      mc_MinValue.SetLength(ou32_Size);
      mc_MaxValue.SetLength(ou32_Size);

      if (ou32_Size > 0U)
      {
         this->pu8_Value = new uint8_t[ou32_Size];
         tgl_assert(this->pu8_Value != NULL);
      }
   }
   catch (...)
   {
      throw "C_KFXVariableBase::SetSize error";
   }
   this->mu32_Size = ou32_Size;
   this->ClearValue();
}

//**************************************************************
//Assumption: float endianness is the same as integer endianness
float64_t C_KFXVariableBase::mh_GetFloatData(const uint8_t ou8_Type, const uint8_t * const opu8_Data)
{
   U_Union32 u_Val32;
   U_Union64 u_Val64;

   if (ou8_Type == KFX_DATA_TYPE_FLOAT32)
   {
      u_Val32.u32_Value =              opu8_Data[0] +
                          (static_cast<uint32_t>(opu8_Data[1]) << 8U) +
                          (static_cast<uint32_t>(opu8_Data[2]) << 16U) +
                          (static_cast<uint32_t>(opu8_Data[3]) << 24U);
      return u_Val32.f32_Value;
   }
   else if (ou8_Type == KFX_DATA_TYPE_FLOAT64)
   {
      u_Val64.u64_Value =              opu8_Data[0] +
                          (static_cast<uint64_t>(opu8_Data[1]) << 8U) +
                          (static_cast<uint64_t>(opu8_Data[2]) << 16U) +
                          (static_cast<uint64_t>(opu8_Data[3]) << 24U) +
                          (static_cast<uint64_t>(opu8_Data[4]) << 32U) +
                          (static_cast<uint64_t>(opu8_Data[5]) << 40U) +
                          (static_cast<uint64_t>(opu8_Data[6]) << 48U) +
                          (static_cast<uint64_t>(opu8_Data[7]) << 56U);
      return u_Val64.f64_Value;
   }
   else
   {
      return 0.0;
   }
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatValue(void) const
{
   return C_KFXVariableBase::mh_GetFloatData(this->u8_Type, this->pu8_Value);
}

//**************************************************************

void C_KFXVariableBase::SetFloatValue(const float64_t of64_Value)
{
   C_KFXVariableBase::mh_SetFloatData(this->u8_Type, this->pu8_Value, of64_Value);
}

//**************************************************************

int64_t C_KFXVariableBase::mh_GetNumericData(const uint8_t ou8_Type, const uint8_t * const opu8_Data)
{
   int64_t s64_Value;

   switch (ou8_Type)
   {
   case KFX_DATA_TYPE_UINT8:
      s64_Value = (*opu8_Data);
      break;
   case KFX_DATA_TYPE_SINT8:
      s64_Value = static_cast<int8_t>(*opu8_Data);
      break;
   case KFX_DATA_TYPE_UINT16:
      s64_Value =                    opu8_Data[0] +
                  (static_cast<uint32_t>(opu8_Data[1]) << 8);
      break;
   case KFX_DATA_TYPE_SINT16:
      s64_Value = static_cast<int16_t>(
         opu8_Data[0] +
         (static_cast<uint32_t>(opu8_Data[1]) << 8));
      break;
   case KFX_DATA_TYPE_SINT32:
      s64_Value = static_cast<int32_t>(
         opu8_Data[0] +
         (static_cast<uint32_t>(opu8_Data[1]) << 8U) +
         (static_cast<uint32_t>(opu8_Data[2]) << 16U) +
         (static_cast<uint32_t>(opu8_Data[3]) << 24U));
      break;
   case KFX_DATA_TYPE_UINT32:
   case KFX_DATA_TYPE_FLOAT32: //if someone really requests data as float32_t we give him/her the best we can ...
      s64_Value =                     opu8_Data[0] +
                  (static_cast<uint32_t>(opu8_Data[1]) <<  8U) +
                  (static_cast<uint32_t>(opu8_Data[2]) << 16U) +
                  (static_cast<uint32_t>(opu8_Data[3]) << 24U);
      break;
   case KFX_DATA_TYPE_SINT64:
      s64_Value = static_cast<int64_t>(
         opu8_Data[0] +
         (static_cast<uint64_t>(opu8_Data[1]) <<  8U) +
         (static_cast<uint64_t>(opu8_Data[2]) << 16U) +
         (static_cast<uint64_t>(opu8_Data[3]) << 24U) +
         (static_cast<uint64_t>(opu8_Data[4]) << 32U) +
         (static_cast<uint64_t>(opu8_Data[5]) << 40U) +
         (static_cast<uint64_t>(opu8_Data[6]) << 48U) +
         (static_cast<uint64_t>(opu8_Data[7]) << 56U));
      break;
   case KFX_DATA_TYPE_UINT64:
   case KFX_DATA_TYPE_FLOAT64: //if someone really requests data as float64_t we give him/her the best we can ...
      //note: MSB might be cut off; application has to cast back
      s64_Value =                    opu8_Data[0] +
                  (static_cast<uint64_t>(opu8_Data[1]) <<  8U) +
                  (static_cast<uint64_t>(opu8_Data[2]) << 16U) +
                  (static_cast<uint64_t>(opu8_Data[3]) << 24U) +
                  (static_cast<uint64_t>(opu8_Data[4]) << 32U) +
                  (static_cast<uint64_t>(opu8_Data[5]) << 40U) +
                  (static_cast<uint64_t>(opu8_Data[6]) << 48U) +
                  (static_cast<uint64_t>(opu8_Data[7]) << 56U);
      break;
   default:
      s64_Value = 0;
      break;
   }
   return s64_Value;
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericValue(void) const
{
   return C_KFXVariableBase::mh_GetNumericData(this->u8_Type, this->pu8_Value);
}

//**************************************************************

void C_KFXVariableBase::SetNumericValue(const int64_t os64_Value)
{
   C_KFXVariableBase::mh_SetNumericData(this->mu32_Size, this->pu8_Value, os64_Value);
}

//**************************************************************

C_SclString C_KFXVariableBase::GetStringValue(void) const
{
   C_SclString c_Help;
   uint32_t u32_Len;

   //how many characters are used ?
   //do not use strlen as it causes problems with Codeguard if
   // the string is as long as the reserved memory
   for (u32_Len = 0U; u32_Len < this->mu32_Size; u32_Len++)
   {
      if (this->pu8_Value[u32_Len] == 0U)
      {
         break;
      }
   }
   char_t * const pcn_Buffer = new char_t[u32_Len + 1];
   (void)std::memcpy(pcn_Buffer, this->pu8_Value, u32_Len);
   pcn_Buffer[u32_Len] = '\0';
   c_Help = pcn_Buffer;
   delete[] pcn_Buffer;
   return c_Help;
}

//**************************************************************

void C_KFXVariableBase::SetStringValue(const C_SclString & orc_Value)
{
   if (orc_Value.Length() < this->mu32_Size) //consider the '\0' -> "<"
   {
      //preset to zero to leave remainder after \0 in a clean state:
      (void)std::memset(this->pu8_Value, 0, this->mu32_Size);
      std::sprintf(reinterpret_cast<char_t *>(this->pu8_Value), "%s", orc_Value.c_str());
   }
   else
   {
      (void)std::memcpy(this->pu8_Value, orc_Value.c_str(), this->mu32_Size);
   }
}

//**************************************************************

void C_KFXVariableBase::CopyDefaultToValue(const uint16_t ou16_DefaultIndex)
{
   if (ou16_DefaultIndex < static_cast<uint16_t>(aau8_Defaults.GetLength()))
   {
      (void)std::memcpy(this->pu8_Value, &this->aau8_Defaults[ou16_DefaultIndex][0], this->mu32_Size);
   }
}

//**************************************************************

void C_KFXVariableBase::CopyValueToDefault(const uint16_t ou16_DefaultIndex)
{
   if (ou16_DefaultIndex < static_cast<uint16_t>(aau8_Defaults.GetLength()))
   {
      (void)std::memcpy(&this->aau8_Defaults[ou16_DefaultIndex][0], this->pu8_Value, this->mu32_Size);
   }
}

//**************************************************************

bool C_KFXVariableBase::DefaultMatchesValue(const uint16_t ou16_DefaultIndex) const
{
   int32_t s32_Return;

   if (ou16_DefaultIndex < static_cast<uint16_t>(aau8_Defaults.GetLength()))
   {
      s32_Return = std::memcmp(this->pu8_Value, &this->aau8_Defaults[ou16_DefaultIndex][0], this->mu32_Size);
   }
   else
   {
      s32_Return = -1;
   }
   return (s32_Return == 0);
}

//**************************************************************

void C_KFXVariableBase::ClearAllDefaults(void)
{
   int32_t s32_Index;

   for (s32_Index = 0U; s32_Index < this->aau8_Defaults.GetLength(); s32_Index++)
   {
      this->ClearDefault(static_cast<uint16_t>(s32_Index));
   }
}

//**************************************************************

void C_KFXVariableBase::ClearDefault(const uint16_t ou16_DefaultIndex)
{
   if (ou16_DefaultIndex < static_cast<uint16_t>(aau8_Defaults.GetLength()))
   {
      (void)std::memset(&this->aau8_Defaults[ou16_DefaultIndex][0], 0, this->mu32_Size);
   }
}

//**************************************************************

void C_KFXVariableBase::ClearValue(void)
{
   (void)std::memset(this->pu8_Value, 0, this->mu32_Size);
}

//**************************************************************

bool C_KFXVariableBase::mh_IsValueInRange(const float64_t of64_Value, const float64_t of64_Min,
                                          const float64_t of64_Max)
{
   volatile float64_t f64_Compare; //make volatile for NaN check to be sure to work !

   //some libraries (tested with Borland C++ Builder) behave weird here and throw exception on INF or NAN
   try
   {
      f64_Compare = of64_Value;
      //portable NAN check:
      if (of64_Value != f64_Compare)
      {
         return false;
      }

      //portable infinity check:
      if ((std::numeric_limits<float64_t>::has_infinity == true) &&
          (of64_Value == std::numeric_limits<float64_t>::infinity()))
      {
         return false; //INF is always out of range
      }

      if ((of64_Value < of64_Min) || (of64_Value > of64_Max))
      {
         return false;
      }
   }
   catch (...)
   {
      return false;
   }
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Check whether variable value lies within min/max range

   Checks whether the current variable value lies within the define minimum and maximum values.
   Can be used for all available variable types.

   \return
   true      min <= value <= max
   false     value out of range of min/max (or: undefined variable type)
*/
//-----------------------------------------------------------------------------
bool C_KFXVariableBase::CheckMinMax(void) const
{
   int64_t s64_Value;
   uint32_t u32_NumEntries;
   uint32_t u32_Element;
   bool q_Return = false;

   if (gatRAMParameterTypes[this->u8_Type].q_IsArray == true)
   {
      u32_NumEntries = this->GetNumberOfArrayElements();
      for (u32_Element = 0U; u32_Element < u32_NumEntries; u32_Element++)
      {
         if (gatRAMParameterTypes[this->u8_Type].q_IsFloat == true)
         {
            q_Return = C_KFXVariableBase::mh_IsValueInRange(this->GetFloatValueFromArray(u32_Element),
                                                            this->GetFloatMinFromArray(u32_Element),
                                                            this->GetFloatMaxFromArray(u32_Element));
         }
         else
         {
            s64_Value = this->GetNumericValueFromArray(u32_Element);
            q_Return = ((s64_Value >= this->GetNumericMinFromArray(u32_Element)) &&
                        (s64_Value <= this->GetNumericMaxFromArray(u32_Element)));
         }
         if (q_Return == false)
         {
            break;
         }
      }
   }
   else if (gatRAMParameterTypes[this->u8_Type].q_IsFloat == true)
   {
      q_Return = C_KFXVariableBase::mh_IsValueInRange(this->GetFloatValue(), this->GetFloatMin(), this->GetFloatMax());
   }
   else
   {
      s64_Value = this->GetNumericValue();
      q_Return = ((s64_Value >= this->GetNumericMin()) && (s64_Value <= this->GetNumericMax()));
   }
   return q_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief    Set number of default values

   Set number of default values for this variable.

   \param[in]   ou16_NumDefaults    new number of default values
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::SetNumDefaults(const uint16_t ou16_NumDefaults)
{
   if (static_cast<uint16_t>(this->aau8_Defaults.GetLength()) == ou16_NumDefaults)
   {
      return;
   }

   if (static_cast<uint16_t>(this->aau8_Defaults.GetLength()) > ou16_NumDefaults)
   {
      this->aau8_Defaults.SetLength(ou16_NumDefaults);
   }
   else
   {
      int32_t s32_Index;
      //add new arrays:
      this->aau8_Defaults.SetLength(ou16_NumDefaults);
      if (this->mu32_Size > 0U)
      {
         for (s32_Index = 0; s32_Index < this->aau8_Defaults.GetLength(); s32_Index++)
         {
            if (this->aau8_Defaults[s32_Index].GetLength() == 0)
            {
               this->aau8_Defaults[s32_Index].SetLength(mu32_Size);
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get number of default values

   Get number of default values for this variable.

   \return
   number of default values
*/
//-----------------------------------------------------------------------------
uint16_t C_KFXVariableBase::GetNumDefaults(void) const
{
   return static_cast<uint16_t>(this->aau8_Defaults.GetLength());
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get numeric default value

   Get numeric default value for this variable for the specified default set.

   \param[in]   ou16_DefaultIndex   index of default set

   \return
   numeric default value for the specified set (zero if the set does not exist or the variable type is invalid)
*/
//-----------------------------------------------------------------------------
int64_t C_KFXVariableBase::GetNumericDefault(const uint16_t ou16_DefaultIndex) const
{
   if (ou16_DefaultIndex >= static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      return 0;
   }

   return C_KFXVariableBase::mh_GetNumericData(this->u8_Type, &this->aau8_Defaults[ou16_DefaultIndex][0]);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Set numeric default value

   Set numeric default value for this variable for the specified default set.
   If the specified default set does not exist the function does nothing.

   \param[in]   os64_Value          default value to set
   \param[in]   ou16_DefaultIndex   index of default set
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::SetNumericDefault(const int64_t os64_Value, const uint16_t ou16_DefaultIndex)
{
   if (ou16_DefaultIndex < static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      C_KFXVariableBase::mh_SetNumericData(this->mu32_Size, &this->aau8_Defaults[ou16_DefaultIndex][0], os64_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief    Get float default value

   Get float default value for this variable for the specified default set.

   \param[in]   ou16_DefaultIndex   index of default set

   \return
   numeric default value for the specified set (zero if the set does not exist or the variable type is invalid)
*/
//-----------------------------------------------------------------------------
float64_t C_KFXVariableBase::GetFloatDefault(const uint16_t ou16_DefaultIndex) const
{
   if (ou16_DefaultIndex >= static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      return 0.0;
   }

   return C_KFXVariableBase::mh_GetFloatData(this->u8_Type, &this->aau8_Defaults[ou16_DefaultIndex][0]);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Set float default value

   Set float default value for this variable for the specified default set.
   If the specified default set does not exist the function does nothing.

   \param[in]   of64_Value          default value to set
   \param[in]   ou16_DefaultIndex   index of default set
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::SetFloatDefault(const float64_t of64_Value, const uint16_t ou16_DefaultIndex)
{
   if (ou16_DefaultIndex < static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      C_KFXVariableBase::mh_SetFloatData(this->u8_Type, &this->aau8_Defaults[ou16_DefaultIndex][0], of64_Value);
   }
}

//**************************************************************

C_SclString C_KFXVariableBase::GetStringDefault(const uint16_t ou16_DefaultIndex) const
{
   C_SclString c_Help;
   uint32_t u32_Len;
   const uint8_t * pu8_Data;

   if (ou16_DefaultIndex >= static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      return "";
   }
   pu8_Data = &this->aau8_Defaults[ou16_DefaultIndex][0];

   //how many characters are used ?
   for (u32_Len = 0U; u32_Len < this->mu32_Size; u32_Len++)
   {
      if (pu8_Data[u32_Len] == 0U)
      {
         break;
      }
   }

   char_t * const pcn_Buffer = new char_t[u32_Len + 1];
   (void)std::memcpy(pcn_Buffer, pu8_Data, u32_Len);
   pcn_Buffer[u32_Len] = '\0';
   c_Help = pcn_Buffer;
   delete[] pcn_Buffer;
   return c_Help;
}

//**************************************************************

void C_KFXVariableBase::SetStringDefault(const C_SclString & orc_Value, const uint16_t ou16_DefaultIndex)
{
   uint8_t * pu8_Data;

   if (ou16_DefaultIndex < static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      pu8_Data = &this->aau8_Defaults[ou16_DefaultIndex][0];

      if (orc_Value.Length() < this->mu32_Size)
      {
         std::sprintf(reinterpret_cast<char_t *>(pu8_Data), "%s", orc_Value.c_str());
      }
      else
      {
         (void)std::memcpy(pu8_Data, orc_Value.c_str(), this->mu32_Size);
      }
   }
}

//**************************************************************

C_KFXVariableBase::C_KFXVariableBase(const C_KFXVariableBase & orc_Source)
{
   this->mu32_Size  = 0U;  //for initialization to work
   this->pu8_Value = NULL; //for initialization to work
   this->operator =(orc_Source);
}

//**************************************************************

C_KFXVariableBase & C_KFXVariableBase::operator =(const C_KFXVariableBase & orc_Source)
{
   uint8_t u8_Index;

   if (&orc_Source == this)
   {
      return (*this);
   }
   this->c_Name      = orc_Source.c_Name;
   this->SetSize(orc_Source.mu32_Size); //will allocate memory for value and default internally
   this->u8_Type     = orc_Source.u8_Type;
   this->mc_MinValue = orc_Source.mc_MinValue;
   this->mc_MaxValue = orc_Source.mc_MaxValue;
   for (u8_Index = 0; u8_Index < KFX_NUM_ACCESS_GROUPS; u8_Index++)
   {
      this->au8_Access[u8_Index] = orc_Source.au8_Access[u8_Index];
   }
   for (u8_Index = 0; u8_Index < KFX_DATA_MAX_NUM_LANGUAGES; u8_Index++)
   {
      this->ac_Comments[u8_Index] = orc_Source.ac_Comments[u8_Index];
   }
   this->c_Unit     = orc_Source.c_Unit;
   (void)std::memcpy(this->pu8_Value, orc_Source.pu8_Value, orc_Source.mu32_Size);
   this->aau8_Defaults          = orc_Source.aau8_Defaults;
   this->q_LocationRAM          = orc_Source.q_LocationRAM;
   this->e_DefTransmissionType   = orc_Source.e_DefTransmissionType;
   this->u16_DefInterval        = orc_Source.u16_DefInterval;
   this->u32_DefLowerHysteresis = orc_Source.u32_DefLowerHysteresis;
   this->u32_DefUpperHysteresis = orc_Source.u32_DefUpperHysteresis;
   this->u32_Address            = orc_Source.u32_Address;
   this->s32_ScalingFactor      = orc_Source.s32_ScalingFactor;
   this->u8_ScalingDigits       = orc_Source.u8_ScalingDigits;
   this->u8_Class               = orc_Source.u8_Class;

   //transient data:
   this->mu8_ActAccess   = orc_Source.mu8_ActAccess;
   this->q_ValueChanged  = orc_Source.q_ValueChanged;
   this->u32_TimeStamp   = orc_Source.u32_TimeStamp;
   this->q_TimeStamped   = orc_Source.q_TimeStamped;
   this->q_ValueValid    = orc_Source.q_ValueValid;
   this->q_CyclicTransmissionActive = orc_Source.q_CyclicTransmissionActive;

   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Calculate CRC over class data

   Calculate 16bit CRC over class member data.
   Can be used to detect changes in class data.
   Not endian-safe (do not use for serialization when sharing with other CPUs)

   \param[in,out]    oru16_CRC      CRC start value / calculated CRC
   \param[in]        oq_SkipValue   true -> do not include pu8_Value in CRC
                                    false -> include pu8_Value in CRC
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::CalcCRCOverEntry(uint16_t & oru16_CRC, const bool oq_SkipValue) const
{
   int32_t s32_Default;
   uint8_t u8_Index;

   C_SclChecksums::CalcCRC16STW(this->c_Name.c_str(),  this->c_Name.Length(), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u32_Address,  sizeof(this->u32_Address), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->mu32_Size,  sizeof(this->mu32_Size), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u8_Type,  sizeof(this->u8_Type), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->mc_MinValue[0], this->mu32_Size, oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->mc_MaxValue[0], this->mu32_Size, oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->au8_Access[0],  sizeof(this->au8_Access), oru16_CRC);
   for (s32_Default = 0; s32_Default < aau8_Defaults.GetLength(); s32_Default++)
   {
      C_SclChecksums::CalcCRC16STW(&this->aau8_Defaults[s32_Default][0], this->mu32_Size, oru16_CRC);
   }
   if (oq_SkipValue == false)
   {
      C_SclChecksums::CalcCRC16STW(this->pu8_Value, this->mu32_Size, oru16_CRC);
   }
   C_SclChecksums::CalcCRC16STW(&this->s32_ScalingFactor, sizeof(this->s32_ScalingFactor), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u8_ScalingDigits, sizeof(this->u8_ScalingDigits), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(this->c_Unit.c_str(), this->c_Unit.Length(), oru16_CRC);
   for (u8_Index = 0U; u8_Index < KFX_DATA_MAX_NUM_LANGUAGES; u8_Index++)
   {
      C_SclChecksums::CalcCRC16STW(this->ac_Comments[u8_Index].c_str(), this->ac_Comments[u8_Index].Length(),
                                   oru16_CRC);
   }
   C_SclChecksums::CalcCRC16STW(&this->q_LocationRAM, sizeof(this->q_LocationRAM), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->e_DefTransmissionType, sizeof(this->e_DefTransmissionType), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u16_DefInterval, sizeof(this->u16_DefInterval), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u32_DefLowerHysteresis, sizeof(this->u32_DefLowerHysteresis), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u32_DefUpperHysteresis, sizeof(this->u32_DefUpperHysteresis), oru16_CRC);
   C_SclChecksums::CalcCRC16STW(&this->u8_Class, sizeof(this->u8_Class), oru16_CRC);
}

//**************************************************************

bool C_KFXVariableBase::IsNumericValue(void) const
{
   if (this->u8_Type < KFX_NUM_DATA_TYPES)
   {
      return ((gatRAMParameterTypes[this->u8_Type].q_IsNumber) && (!gatRAMParameterTypes[this->u8_Type].q_IsArray));
   }
   return false;
}

//--------------------------------------------------------------

bool C_KFXVariableBase::IsArrayType(void) const
{
   if (this->u8_Type < KFX_NUM_DATA_TYPES)
   {
      return (gatRAMParameterTypes[this->u8_Type].q_IsArray);
   }
   return false;
}

//--------------------------------------------------------------

bool C_KFXVariableBase::IsFloatType(void) const
{
   if (this->u8_Type < KFX_NUM_DATA_TYPES)
   {
      return (gatRAMParameterTypes[this->u8_Type].q_IsFloat);
   }
   return false;
}

//--------------------------------------------------------------

bool C_KFXVariableBase::IsBinaryArrayType(void) const
{
   if (this->u8_Type < KFX_NUM_DATA_TYPES)
   {
      return ((gatRAMParameterTypes[this->u8_Type].q_IsArray) && (gatRAMParameterTypes[this->u8_Type].q_IsNumber));
   }
   return false;
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatMin(void) const
{
   return C_KFXVariableBase::mh_GetFloatData(this->u8_Type, &this->mc_MinValue[0]);
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatMax(void) const
{
   return C_KFXVariableBase::mh_GetFloatData(this->u8_Type, &this->mc_MaxValue[0]);
}

//**************************************************************

void C_KFXVariableBase::SetFloatMin(const float64_t of64_Min)
{
   C_KFXVariableBase::mh_SetFloatData(this->u8_Type, &this->mc_MinValue[0], of64_Min);
}

//**************************************************************

void C_KFXVariableBase::SetFloatMax(const float64_t of64_Max)
{
   C_KFXVariableBase::mh_SetFloatData(this->u8_Type, &this->mc_MaxValue[0], of64_Max);
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericMin(void) const
{
   return C_KFXVariableBase::mh_GetNumericData(this->u8_Type, &this->mc_MinValue[0]);
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericMax(void) const
{
   return C_KFXVariableBase::mh_GetNumericData(this->u8_Type, &this->mc_MaxValue[0]);
}

//**************************************************************

void C_KFXVariableBase::SetNumericMin(const int64_t os64_Min)
{
   C_KFXVariableBase::mh_SetNumericData(this->mu32_Size, &this->mc_MinValue[0], os64_Min);
}

//**************************************************************

void C_KFXVariableBase::SetNumericMax(const int64_t os64_Max)
{
   C_KFXVariableBase::mh_SetNumericData(this->mu32_Size, &this->mc_MaxValue[0], os64_Max);
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatMinFromArray(const uint32_t ou32_ArrayIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetFloatMinFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetFloatData(u8_ElementType, &mc_MinValue[u32_ByteOffset]);
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatMaxFromArray(const uint32_t ou32_ArrayIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetFloatMaxFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetFloatData(u8_ElementType, &mc_MaxValue[u32_ByteOffset]);
}

//**************************************************************

void C_KFXVariableBase::SetFloatMinInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Min)
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::SetFloatMinInArray: not a valid array type";
   }
   C_KFXVariableBase::mh_SetFloatData(u8_ElementType, &this->mc_MinValue[u32_ByteOffset], of64_Min);
}

//**************************************************************

void C_KFXVariableBase::SetFloatMaxInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Max)
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::SetFloatMaxInArray: not a valid array type";
   }
   C_KFXVariableBase::mh_SetFloatData(u8_ElementType, &this->mc_MaxValue[u32_ByteOffset], of64_Max);
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericMinFromArray(const uint32_t ou32_ArrayIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetNumericMinFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetNumericData(u8_ElementType, &mc_MinValue[u32_ByteOffset]);
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericMaxFromArray(const uint32_t ou32_ArrayIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetNumericMaxFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetNumericData(u8_ElementType, &mc_MaxValue[u32_ByteOffset]);
}

//**************************************************************

void C_KFXVariableBase::SetNumericMinInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Min)
{
   uint8_t u8_SizeOfOneEntry;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_SizeOfOneEntry = this->GetSizeOfArrayElement();
   if (u8_SizeOfOneEntry == 0U) //not really an array !
   {
      throw "C_KFXVariableBase::SetNumericMinInArray: not a valid array type";
   }
   C_KFXVariableBase::mh_SetNumericData(u8_SizeOfOneEntry, &this->mc_MinValue[u32_ByteOffset], os64_Min);
}

//**************************************************************

void C_KFXVariableBase::SetNumericMaxInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Max)
{
   uint8_t u8_SizeOfOneEntry;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_SizeOfOneEntry = this->GetSizeOfArrayElement();
   if (u8_SizeOfOneEntry == 0U) //not really an array !
   {
      throw "C_KFXVariableBase::SetNumericMaxInArray: not a valid array type";
   }
   C_KFXVariableBase::mh_SetNumericData(u8_SizeOfOneEntry, &this->mc_MaxValue[u32_ByteOffset], os64_Max);
}

//**************************************************************

uint8_t C_KFXVariableBase::m_GetEntryTypeByArrayType(void) const
{
   if (this->u8_Type < KFX_NUM_DATA_TYPES)
   {
      return gatRAMParameterTypes[this->u8_Type].u8_BaseType;
   }
   else
   {
      return KFX_DATA_TYPE_NOVAR;
   }
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericDefaultFromArray(const uint32_t ou32_ArrayIndex,
                                                      const uint16_t ou16_DefaultIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParamsDefault(ou16_DefaultIndex, ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetNumericDefaultFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetNumericData(u8_ElementType, &aau8_Defaults[ou16_DefaultIndex][u32_ByteOffset]);
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatDefaultFromArray(const uint32_t ou32_ArrayIndex,
                                                      const uint16_t ou16_DefaultIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParamsDefault(ou16_DefaultIndex, ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetFloatDefaultFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetFloatData(u8_ElementType, &aau8_Defaults[ou16_DefaultIndex][u32_ByteOffset]);
}

//**************************************************************

int64_t C_KFXVariableBase::GetNumericValueFromArray(const uint32_t ou32_ArrayIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetNumericValueFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetNumericData(u8_ElementType, &this->pu8_Value[u32_ByteOffset]);
}

//**************************************************************
//Assumption: float endianness is the same as integer endianness
void C_KFXVariableBase::mh_SetFloatData(const uint8_t ou8_Type, uint8_t * const opu8_Data, const float64_t of64_Value)
{
   U_Union32 u_Val32;
   U_Union64 u_Val64;

   switch (ou8_Type)
   {
   case KFX_DATA_TYPE_FLOAT32:
      u_Val32.f32_Value = static_cast<float32_t>(of64_Value);
      opu8_Data[0] = static_cast<uint8_t>(u_Val32.u32_Value);
      opu8_Data[1] = static_cast<uint8_t>(u_Val32.u32_Value >> 8U);
      opu8_Data[2] = static_cast<uint8_t>(u_Val32.u32_Value >> 16U);
      opu8_Data[3] = static_cast<uint8_t>(u_Val32.u32_Value >> 24U);
      break;
   case KFX_DATA_TYPE_FLOAT64:
      u_Val64.f64_Value = of64_Value;
      opu8_Data[0] = static_cast<uint8_t>(u_Val64.u64_Value);
      opu8_Data[1] = static_cast<uint8_t>(u_Val64.u64_Value >> 8U);
      opu8_Data[2] = static_cast<uint8_t>(u_Val64.u64_Value >> 16U);
      opu8_Data[3] = static_cast<uint8_t>(u_Val64.u64_Value >> 24U);
      opu8_Data[4] = static_cast<uint8_t>(u_Val64.u64_Value >> 32U);
      opu8_Data[5] = static_cast<uint8_t>(u_Val64.u64_Value >> 40U);
      opu8_Data[6] = static_cast<uint8_t>(u_Val64.u64_Value >> 48U);
      opu8_Data[7] = static_cast<uint8_t>(u_Val64.u64_Value >> 56U);
      break;
   default:
      //don't throw: this might cause some compatibility issues with existing applications
      break;
   }
}

//**************************************************************

void C_KFXVariableBase::mh_SetNumericData(const uint32_t ou32_Size, uint8_t * const opu8_Data, const int64_t os64_Value)
{
   switch (ou32_Size)
   {
   case 1:
      opu8_Data[0] = static_cast<uint8_t>(os64_Value);
      break;
   case 2:
      opu8_Data[0] = static_cast<uint8_t>(static_cast<uint16_t>(os64_Value));
      opu8_Data[1] = static_cast<uint8_t>((static_cast<uint16_t>(os64_Value)) >> 8U);
      break;
   case 4:
      opu8_Data[0] = static_cast<uint8_t>(static_cast<uint32_t>(os64_Value));
      opu8_Data[1] = static_cast<uint8_t>((static_cast<uint32_t>(os64_Value)) >>  8U);
      opu8_Data[2] = static_cast<uint8_t>((static_cast<uint32_t>(os64_Value)) >> 16U);
      opu8_Data[3] = static_cast<uint8_t>((static_cast<uint32_t>(os64_Value)) >> 24U);
      break;
   case 8:
      opu8_Data[0] = static_cast<uint8_t>(static_cast<uint64_t>(os64_Value));
      opu8_Data[1] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >>  8U);
      opu8_Data[2] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >> 16U);
      opu8_Data[3] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >> 24U);
      opu8_Data[4] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >> 32U);
      opu8_Data[5] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >> 40U);
      opu8_Data[6] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >> 48U);
      opu8_Data[7] = static_cast<uint8_t>((static_cast<uint64_t>(os64_Value)) >> 56U);
      break;
   default:
      //don't throw: this might cause some compatibility issues with existing applications
      break;
   }
}

//**************************************************************

void C_KFXVariableBase::SetNumericValueInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Value)
{
   uint8_t u8_SizeOfOneEntry;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_SizeOfOneEntry = this->GetSizeOfArrayElement();
   if (u8_SizeOfOneEntry == 0U) //not really an array !
   {
      throw "C_KFXVariableBase::SetNumericValueInArray: not a valid array type";
   }

   C_KFXVariableBase::mh_SetNumericData(u8_SizeOfOneEntry, &this->pu8_Value[u32_ByteOffset], os64_Value);
}

//**************************************************************

void C_KFXVariableBase::SetFloatValueInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Value)
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::SetFloatValueInArray: not a valid array type";
   }
   C_KFXVariableBase::mh_SetFloatData(u8_ElementType, &this->pu8_Value[u32_ByteOffset], of64_Value);
}

//**************************************************************

float64_t C_KFXVariableBase::GetFloatValueFromArray(const uint32_t ou32_ArrayIndex) const
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParams(ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::GetFloatValueFromArray: not a valid array type";
   }
   return C_KFXVariableBase::mh_GetFloatData(u8_ElementType, &this->pu8_Value[u32_ByteOffset]);
}

//**************************************************************

uint8_t C_KFXVariableBase::GetSizeOfArrayElement(void) const
{
   uint8_t u8_Result;

   switch (this->u8_Type)
   {
   case KFX_DATA_TYPE_ASINT8: //no break
   case KFX_DATA_TYPE_AUINT8:
      u8_Result = 1U;
      break;
   case KFX_DATA_TYPE_ASINT16: //no break
   case KFX_DATA_TYPE_AUINT16:
      u8_Result = 2U;
      break;
   case KFX_DATA_TYPE_ASINT32: //no break
   case KFX_DATA_TYPE_AUINT32: //no break
   case KFX_DATA_TYPE_AFLOAT32:
      u8_Result = 4U;
      break;
   case KFX_DATA_TYPE_AFLOAT64: //no break
   case KFX_DATA_TYPE_ASINT64:  //no break
   case KFX_DATA_TYPE_AUINT64:
      u8_Result = 8U;
      break;
   default:
      u8_Result = 0U;
      break;
   }
   return u8_Result;
}

//**************************************************************

uint32_t C_KFXVariableBase::GetNumberOfArrayElements(void) const
{
   uint8_t u8_SizeOfElement;

   if (this->IsArrayType() == false)
   {
      return 0U;
   }
   u8_SizeOfElement = this->GetSizeOfArrayElement();
   if (u8_SizeOfElement == 0U)
   {
      return 0U;
   }
   return (this->mu32_Size) / u8_SizeOfElement;
}

//**************************************************************

void C_KFXVariableBase::SetNumericDefaultInArray(const uint32_t ou32_ArrayIndex, const int64_t os64_Value,
                                                 const uint16_t ou16_DefaultIndex)
{
   uint8_t u8_SizeOfOneEntry;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParamsDefault(ou16_DefaultIndex, ou32_ArrayIndex, u32_ByteOffset);

   u8_SizeOfOneEntry = this->GetSizeOfArrayElement();
   if (u8_SizeOfOneEntry == 0U) //not really an array !
   {
      throw "C_KFXVariableBase::SetNumericDefaultInArray: not a valid array type";
   }

   C_KFXVariableBase::mh_SetNumericData(u8_SizeOfOneEntry, &this->aau8_Defaults[ou16_DefaultIndex][u32_ByteOffset],
                                        os64_Value);
}

//**************************************************************

void C_KFXVariableBase::SetFloatDefaultInArray(const uint32_t ou32_ArrayIndex, const float64_t of64_Value,
                                               const uint16_t ou16_DefaultIndex)
{
   uint8_t u8_ElementType;
   uint32_t u32_ByteOffset;

   this->m_CheckArrayAccessParamsDefault(ou16_DefaultIndex, ou32_ArrayIndex, u32_ByteOffset);

   u8_ElementType = this->m_GetEntryTypeByArrayType();
   if (u8_ElementType == KFX_DATA_TYPE_NOVAR)
   {
      throw "C_KFXVariableBase::SetFloatDefaultInArray: not a valid array type";
   }

   C_KFXVariableBase::mh_SetFloatData(u8_ElementType, &this->aau8_Defaults[ou16_DefaultIndex][u32_ByteOffset],
                                      of64_Value);
}

//**************************************************************
//will throw on invalid parameters
void C_KFXVariableBase::m_CheckArrayAccessParams(const uint32_t ou32_ArrayIndex, uint32_t & oru32_ByteOffset) const
{
   uint8_t u8_SizeOfOneEntry;

   u8_SizeOfOneEntry = this->GetSizeOfArrayElement();
   if (u8_SizeOfOneEntry == 0U) //not really an array !
   {
      throw "C_KFXVariableBase::m_CheckArrayAccessParams: not a valid array type";
   }

   oru32_ByteOffset = ou32_ArrayIndex * u8_SizeOfOneEntry;
   if ((oru32_ByteOffset + u8_SizeOfOneEntry) > mu32_Size)
   {
      throw "C_KFXVariableBase::m_CheckArrayAccessParams: overflow";
   }
}

//**************************************************************

void C_KFXVariableBase::m_CheckArrayAccessParamsDefault(const uint16_t ou16_DefaultIndex,
                                                        const uint32_t ou32_ArrayIndex,
                                                        uint32_t & oru32_ByteOffset) const
{
   if (ou16_DefaultIndex >= static_cast<uint16_t>(this->aau8_Defaults.GetLength()))
   {
      throw "C_KFXVariableBase: invalid default set index";
   }
   this->m_CheckArrayAccessParams(ou32_ArrayIndex, oru32_ByteOffset);
}

//-----------------------------------------------------------------------------
/*!
   \brief    Set min/max values to maximum ranges allowed by data type

   Set the minimum and maximum values to the extremes possible based on the defined variable type.
   For array variables this sets the individual elements of the min/max arrays to the min/max of the array base type.
*/
//-----------------------------------------------------------------------------
void C_KFXVariableBase::SetMinMaxToMaximum(void)
{
   uint32_t u32_Index;

   switch (this->u8_Type)
   {
   case KFX_DATA_TYPE_UINT8:
      this->SetNumericMin(static_cast<int64_t>(std::numeric_limits<uint8_t>::min()));
      this->SetNumericMax(static_cast<int64_t>(std::numeric_limits<uint8_t>::max()));
      break;
   case KFX_DATA_TYPE_SINT8:
      this->SetNumericMin(static_cast<int64_t>(std::numeric_limits<int8_t>::min()));
      this->SetNumericMax(static_cast<int64_t>(std::numeric_limits<int8_t>::max()));
      break;
   case KFX_DATA_TYPE_SINT16:
      this->SetNumericMin(static_cast<int64_t>(std::numeric_limits<int16_t>::min()));
      this->SetNumericMax(static_cast<int64_t>(std::numeric_limits<int16_t>::max()));
      break;
   case KFX_DATA_TYPE_UINT16:
      this->SetNumericMin(static_cast<int64_t>(std::numeric_limits<uint16_t>::min()));
      this->SetNumericMax(static_cast<int64_t>(std::numeric_limits<uint16_t>::max()));
      break;
   case KFX_DATA_TYPE_SINT32:
      this->SetNumericMin(static_cast<int64_t>(std::numeric_limits<int32_t>::min()));
      this->SetNumericMax(static_cast<int64_t>(std::numeric_limits<int32_t>::max()));
      break;
   case KFX_DATA_TYPE_UINT32:
      this->SetNumericMin(static_cast<int64_t>(std::numeric_limits<uint32_t>::min()));
      this->SetNumericMax(static_cast<int64_t>(std::numeric_limits<uint32_t>::max()));
      break;
   case KFX_DATA_TYPE_ASINT8:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<int8_t>::min()));
         this->SetNumericMaxInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<int8_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_AUINT8:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<uint8_t>::min()));
         this->SetNumericMaxInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<uint8_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_FLOAT32:
      this->SetFloatMin(static_cast<float64_t>(-std::numeric_limits<float32_t>::max()));
      this->SetFloatMax(static_cast<float64_t>(std::numeric_limits<float32_t>::max()));
      break;
   case KFX_DATA_TYPE_FLOAT64:
      this->SetFloatMin(-std::numeric_limits<float64_t>::max());
      this->SetFloatMax(std::numeric_limits<float64_t>::max());
      break;
   case KFX_DATA_TYPE_ASINT16:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<int16_t>::min()));
         this->SetNumericMaxInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<int16_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_AUINT16:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<uint16_t>::min()));
         this->SetNumericMaxInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<uint16_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_ASINT32:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<int32_t>::min()));
         this->SetNumericMaxInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<int32_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_AUINT32:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<uint32_t>::min()));
         this->SetNumericMaxInArray(u32_Index, static_cast<int64_t>(std::numeric_limits<uint32_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_AFLOAT32:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetFloatMinInArray(u32_Index, static_cast<float64_t>(-std::numeric_limits<float32_t>::max()));
         this->SetFloatMaxInArray(u32_Index, static_cast<float64_t>(std::numeric_limits<float32_t>::max()));
      }
      break;
   case KFX_DATA_TYPE_AFLOAT64:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetFloatMinInArray(u32_Index, -std::numeric_limits<float64_t>::max());
         this->SetFloatMaxInArray(u32_Index, std::numeric_limits<float64_t>::max());
      }
      break;
   case KFX_DATA_TYPE_SINT64:
      this->SetNumericMin(std::numeric_limits<int64_t>::min());
      this->SetNumericMax(std::numeric_limits<int64_t>::max());
      break;
   case KFX_DATA_TYPE_ASINT64:
      for (u32_Index = 0U; u32_Index < this->GetNumberOfArrayElements(); u32_Index++)
      {
         this->SetNumericMinInArray(u32_Index, std::numeric_limits<int64_t>::min());
         this->SetNumericMaxInArray(u32_Index, std::numeric_limits<int64_t>::max());
      }
      break;
   default:
      throw "Invalid type in C_KFXVariableBase::SetMinMaxToMaximum";
   }
}

//**************************************************************

const C_SclDynamicArray<uint8_t> & C_KFXVariableBase::GetMinReference(void) const
{
   return this->mc_MinValue;
}

//**************************************************************

const C_SclDynamicArray<uint8_t> & C_KFXVariableBase::GetMaxReference(void) const
{
   return this->mc_MaxValue;
}

//**************************************************************

C_SclDynamicArray<uint8_t> & C_KFXVariableBase::GetMinReference(void)
{
   return this->mc_MinValue;
}

//**************************************************************

C_SclDynamicArray<uint8_t> & C_KFXVariableBase::GetMaxReference(void)
{
   return this->mc_MaxValue;
}

//**************************************************************

C_SclString C_KFXVariableBase::GetTypeName(void) const
{
   if ((this->u8_Type != KFX_DATA_TYPE_NOVAR) && (this->u8_Type != KFX_DATA_TYPE_CRC) &&
       (this->u8_Type < KFX_NUM_DATA_TYPES))
   {
      return static_cast<C_SclString>(gatRAMParameterTypes[this->u8_Type].pcn_WrittenName);
   }
   return "INVALID";
}

//**************************************************************

C_SclString C_KFXVariableBase::GetTypeDependentValueString(const bool oq_Hex, const bool oq_LeadingZeroes) const
{
   C_SclString c_Help;
   int64_t s64_Temp;
   float64_t f64_Temp;

   switch (this->u8_Type)
   {
   case KFX_DATA_TYPE_UINT8:  //no break
   case KFX_DATA_TYPE_SINT8:  //no break
   case KFX_DATA_TYPE_SINT16: //no break
   case KFX_DATA_TYPE_UINT16: //no break
   case KFX_DATA_TYPE_SINT32: //no break
   case KFX_DATA_TYPE_UINT32: //no break
   case KFX_DATA_TYPE_SINT64: //no break
   case KFX_DATA_TYPE_UINT64: //todo: consider MSB
      s64_Temp = this->GetNumericValue();
      if (oq_Hex == true)
      {
         if (s64_Temp < 0)
         {
            s64_Temp *= -1;
            c_Help = "-0x" + C_SclString::IntToHex(s64_Temp, 0);
            if (oq_LeadingZeroes == true)
            {
               c_Help.Insert(C_SclString::StringOfChar('0', (3 + (this->GetSize() * 2)) - c_Help.Length()), 4);
            }
         }
         else
         {
            c_Help = "0x" + C_SclString::IntToHex(s64_Temp, 0);
            if (oq_LeadingZeroes == true)
            {
               c_Help.Insert(C_SclString::StringOfChar('0', (2 + (this->GetSize() * 2)) - c_Help.Length()), 3);
            }
         }
      }
      else
      {
         c_Help = C_SclString::IntToStr(s64_Temp);
      }
      break;
   case KFX_DATA_TYPE_FLOAT32:
   case KFX_DATA_TYPE_FLOAT64:
      f64_Temp = this->GetFloatValue();
      c_Help = C_SclString::FloatToStr(f64_Temp, u8_ScalingDigits);
      break;
   case KFX_DATA_TYPE_ASINT8:
      c_Help = this->C_KFXVariableBase::GetStringValue();
      break;
   case KFX_DATA_TYPE_AUINT8:   //no break
   case KFX_DATA_TYPE_ASINT16:  //no break
   case KFX_DATA_TYPE_AUINT16:  //no break
   case KFX_DATA_TYPE_ASINT32:  //no break
   case KFX_DATA_TYPE_AUINT32:  //no break
   case KFX_DATA_TYPE_AFLOAT32: //no break
   case KFX_DATA_TYPE_AFLOAT64: //no break
   case KFX_DATA_TYPE_ASINT64:  //no break
   case KFX_DATA_TYPE_AUINT64:
      c_Help = "N/A";
      break;
   default:
      tgl_assert(false);
      break;
   }
   return c_Help;
}

//**************************************************************

C_SclString C_KFXVariableBase::GetTypeDependentDefaultString(const bool oq_Hex, const bool oq_LeadingZeroes,
                                                             const uint16_t ou16_DefaultIndex) const
{
   C_SclString c_Help;
   int64_t s64_Temp;
   float64_t f64_Temp;

   if (ou16_DefaultIndex >= static_cast<uint16_t>(this->GetNumDefaults()))
   {
      return "";
   }

   switch (this->u8_Type)
   {
   case KFX_DATA_TYPE_UINT8:  //no break
   case KFX_DATA_TYPE_SINT8:  //no break
   case KFX_DATA_TYPE_SINT16: //no break
   case KFX_DATA_TYPE_UINT16: //no break
   case KFX_DATA_TYPE_SINT32: //no break
   case KFX_DATA_TYPE_UINT32: //no break
   case KFX_DATA_TYPE_SINT64: //no break
   case KFX_DATA_TYPE_UINT64: //todo: consider MSB
      s64_Temp = this->GetNumericDefault(ou16_DefaultIndex);
      if (oq_Hex == true)
      {
         if (s64_Temp < 0)
         {
            s64_Temp *= -1;
            c_Help = "-0x" + C_SclString::IntToHex(s64_Temp, 0);
            if (oq_LeadingZeroes == true)
            {
               c_Help.Insert(C_SclString::StringOfChar('0', (3 + (this->GetSize() * 2)) - c_Help.Length()), 4);
            }
         }
         else
         {
            c_Help = "0x" + C_SclString::IntToHex(s64_Temp, 0);
            if (oq_LeadingZeroes == true)
            {
               c_Help.Insert(C_SclString::StringOfChar('0', (2 + (this->GetSize() * 2)) - c_Help.Length()), 3);
            }
         }
      }
      else
      {
         c_Help = C_SclString::IntToStr(s64_Temp);
      }
      break;
   case KFX_DATA_TYPE_FLOAT32:
   case KFX_DATA_TYPE_FLOAT64:
      f64_Temp = this->GetFloatDefault(ou16_DefaultIndex);
      c_Help = C_SclString::FloatToStr(f64_Temp, u8_ScalingDigits);
      break;
   case KFX_DATA_TYPE_ASINT8:
      c_Help = this->C_KFXVariableBase::GetStringDefault(ou16_DefaultIndex);
      break;
   case KFX_DATA_TYPE_AUINT8:   //no break
   case KFX_DATA_TYPE_ASINT16:  //no break
   case KFX_DATA_TYPE_AUINT16:  //no break
   case KFX_DATA_TYPE_ASINT32:  //no break
   case KFX_DATA_TYPE_AUINT32:  //no break
   case KFX_DATA_TYPE_AFLOAT32: //no break
   case KFX_DATA_TYPE_AFLOAT64: //no break
   case KFX_DATA_TYPE_ASINT64:  //no break
   case KFX_DATA_TYPE_AUINT64:
      c_Help = "N/A";
      break;
   default:
      tgl_assert(false);
      break;
   }
   return c_Help;
}
