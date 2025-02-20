//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for any supported data type (implementation)

   Data class for any supported data type

   All SetValue* and GetValue* functions are synchronized and thread safe.
   All other functions (SetArray for example) are not thread safe and shall not be used
   if more than one thread are accessing to the instance.

   The data is held in a std::vector of uint8.
   The getter and setter functions place the data into that vector in native endianness.
   This is not a problem as the RAM data is not shared with other systems.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>
#include <sstream>
#include <limits>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_OscNodeDataPoolContent.hpp"
#include "C_SclChecksums.hpp"
#include "C_OscUtils.hpp"
#include "TglUtils.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::tgl;
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::C_OscNodeDataPoolContent(void) :
   me_Type(eUINT8),
   mq_Array(false),
   mc_CriticalSection()
{
   mc_Data.resize(1, 0U); //matching uint8 type
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy constructor

   \param[in]  orc_Source  Reference to source
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::C_OscNodeDataPoolContent(const C_OscNodeDataPoolContent & orc_Source) :
   me_Type(orc_Source.me_Type),
   mq_Array(orc_Source.mq_Array),
   mc_Data(orc_Source.mc_Data),
   mc_CriticalSection()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy operator

   \param[in]  orc_Source  Reference to source

   \return
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent & C_OscNodeDataPoolContent::operator =(const C_OscNodeDataPoolContent & orc_Source)
{
   if (this != &orc_Source)
   {
      this->me_Type = orc_Source.me_Type;
      this->mq_Array = orc_Source.mq_Array;
      this->mc_Data = orc_Source.mc_Data;
   }

   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default Destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::~C_OscNodeDataPoolContent(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->me_Type, sizeof(this->me_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_Array, sizeof(this->mq_Array), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mc_Data[0], static_cast<uint32_t>(this->mc_Data.size()),
                                       oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over one element

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
   \param[in]      ou32_Index       Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::CalcHashElement(uint32_t & oru32_HashValue, const uint32_t ou32_Index) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->me_Type, sizeof(this->me_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_Array, sizeof(this->mq_Array), oru32_HashValue);
   if ((this->mq_Array == true) && (ou32_Index < this->GetArraySize()))
   {
      uint64_t u64_Val = 0ULL;
      int64_t s64_Val = 0LL;
      float64_t f64_Val = 0.0;
      switch (this->me_Type)
      {
      case C_OscNodeDataPoolContent::eUINT8:
         u64_Val = static_cast<uint64_t>(this->GetValueArrU8Element(ou32_Index));
         stw::scl::C_SclChecksums::CalcCRC32(&u64_Val, sizeof(u64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         u64_Val = static_cast<uint64_t>(this->GetValueArrU16Element(ou32_Index));
         stw::scl::C_SclChecksums::CalcCRC32(&u64_Val, sizeof(u64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         u64_Val = static_cast<uint64_t>(this->GetValueArrU32Element(ou32_Index));
         stw::scl::C_SclChecksums::CalcCRC32(&u64_Val, sizeof(u64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         u64_Val = this->GetValueArrU64Element(ou32_Index);
         stw::scl::C_SclChecksums::CalcCRC32(&u64_Val, sizeof(u64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         s64_Val = static_cast<int64_t>(this->GetValueArrS8Element(ou32_Index));
         stw::scl::C_SclChecksums::CalcCRC32(&s64_Val, sizeof(s64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         s64_Val = static_cast<int64_t>(this->GetValueArrS16Element(ou32_Index));
         stw::scl::C_SclChecksums::CalcCRC32(&s64_Val, sizeof(s64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         s64_Val = static_cast<int64_t>(this->GetValueArrS32Element(ou32_Index));
         stw::scl::C_SclChecksums::CalcCRC32(&s64_Val, sizeof(s64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         s64_Val = this->GetValueArrS64Element(ou32_Index);
         stw::scl::C_SclChecksums::CalcCRC32(&s64_Val, sizeof(s64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         f64_Val = static_cast<float64_t>(this->GetValueArrF32Element(ou32_Index));
         //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
         stw::scl::C_SclChecksums::CalcCRC32(&f64_Val, sizeof(f64_Val), oru32_HashValue);
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         f64_Val = this->GetValueArrF64Element(ou32_Index);
         //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
         stw::scl::C_SclChecksums::CalcCRC32(&f64_Val, sizeof(f64_Val), oru32_HashValue);
         break;
      default:
         break;
      }
   }
   else
   {
      //Just one element
      stw::scl::C_SclChecksums::CalcCRC32(&this->mc_Data[0], static_cast<uint32_t>(this->mc_Data.size()),
                                          oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over structure only

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::CalcHashStructure(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->me_Type, sizeof(this->me_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->mq_Array, sizeof(this->mq_Array), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: set current value for simply type of any type

   \param[in]  orc_Value   New value
   \param[in]  oe_Type     supposed type held in this class

   \return
   No direct return. If we do not hold a plain type exception C_CONFIG is thrown as sint32
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8058,8080} //template parameter names are not properly handled by naming convention check
template <typename T> void C_OscNodeDataPoolContent::m_SetValue(const T & orc_Value, const E_Type oe_Type)
{
   if (this->mq_Array == false)
   {
      if (this->me_Type == oe_Type)
      {
         this->mc_CriticalSection.Acquire();
         //lint -e{9110} //we do not really use the bit representation; mc_Data is just our "BLOB" storage
         (void)std::memcpy(&this->mc_Data[0], &orc_Value, sizeof(orc_Value));
         this->mc_CriticalSection.Release();
      }
      else
      {
         throw std::invalid_argument("Element type mismatch");
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: set current value for simply type of any type

   \param[in]  oe_Type     supposed type held in this class
   \param[in]  orc_Value   New value

   \return
   No direct return. If we do not hold a plain type exception C_CONFIG is thrown as sint32
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8058,8080} //template parameter names are not properly handled by naming convention check
template <typename T> void C_OscNodeDataPoolContent::m_GetValue(const E_Type oe_Type, T & orc_Value) const
{
   if (this->mq_Array == false)
   {
      if (this->me_Type == oe_Type)
      {
         this->mc_CriticalSection.Acquire();
         //lint -e{9110} //we do not really use the bit representation; mc_Data is just our "BLOB" storage
         (void)std::memcpy(&orc_Value, &this->mc_Data[0], sizeof(orc_Value));
         this->mc_CriticalSection.Release();
      }
      else
      {
         throw std::invalid_argument("Element type mismatch");
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for uint8 type

   \param[in]  ou8_Value   New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueU8(const uint8_t ou8_Value)
{
   m_SetValue(ou8_Value, C_OscNodeDataPoolContent::eUINT8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for uint8 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscNodeDataPoolContent::GetValueU8(void) const
{
   uint8_t u8_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eUINT8, u8_Retval);
   return u8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for uint16 type

   \param[in]  ou16_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueU16(const uint16_t ou16_Value)
{
   m_SetValue(ou16_Value, C_OscNodeDataPoolContent::eUINT16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for uint16 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscNodeDataPoolContent::GetValueU16(void) const
{
   uint16_t u16_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eUINT16, u16_Retval);
   return u16_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for uint32 type

   \param[in]  ou32_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueU32(const uint32_t ou32_Value)
{
   m_SetValue(ou32_Value, C_OscNodeDataPoolContent::eUINT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for uint32 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolContent::GetValueU32(void) const
{
   uint32_t u32_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eUINT32, u32_Retval);
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for uint64 type

   \param[in]  ou64_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueU64(const uint64_t ou64_Value)
{
   m_SetValue(ou64_Value, C_OscNodeDataPoolContent::eUINT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for uint64 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_OscNodeDataPoolContent::GetValueU64(void) const
{
   uint64_t u64_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eUINT64, u64_Retval);
   return u64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for sint8 type

   \param[in]  os8_Value   New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueS8(const int8_t os8_Value)
{
   m_SetValue(os8_Value, C_OscNodeDataPoolContent::eSINT8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for sint8 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int8_t C_OscNodeDataPoolContent::GetValueS8(void) const
{
   int8_t s8_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eSINT8, s8_Retval);
   return s8_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for sint16 type

   \param[in]  os16_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueS16(const int16_t os16_Value)
{
   m_SetValue(os16_Value, C_OscNodeDataPoolContent::eSINT16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for sint16 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int16_t C_OscNodeDataPoolContent::GetValueS16(void) const
{
   int16_t s16_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eSINT16, s16_Retval);
   return s16_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for sint32 type

   \param[in]  os32_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueS32(const int32_t os32_Value)
{
   m_SetValue(os32_Value, C_OscNodeDataPoolContent::eSINT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for sint32 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolContent::GetValueS32(void) const
{
   int32_t s32_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eSINT32, s32_Retval);
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for sint64 type

   \param[in]  os64_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueS64(const int64_t os64_Value)
{
   m_SetValue(os64_Value, C_OscNodeDataPoolContent::eSINT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for sint64 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int64_t C_OscNodeDataPoolContent::GetValueS64(void) const
{
   int64_t s64_Retval = 0;

   m_GetValue(C_OscNodeDataPoolContent::eSINT64, s64_Retval);
   return s64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for float32 type

   \param[in]  of32_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueF32(const float32_t of32_Value)
{
   m_SetValue(of32_Value, C_OscNodeDataPoolContent::eFLOAT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for float32 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_OscNodeDataPoolContent::GetValueF32(void) const
{
   float32_t f32_Retval = 0.0F;

   m_GetValue(C_OscNodeDataPoolContent::eFLOAT32, f32_Retval);
   return f32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for float64 type

   \param[in]  of64_Value  New assignment value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueF64(const float64_t of64_Value)
{
   m_SetValue(of64_Value, C_OscNodeDataPoolContent::eFLOAT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for float64 type

   \return
   Type match: currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_OscNodeDataPoolContent::GetValueF64(void) const
{
   float64_t f64_Retval = 0.0;

   m_GetValue(C_OscNodeDataPoolContent::eFLOAT64, f64_Retval);
   return f64_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of uint8 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU8(const std::vector<uint8_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eUINT8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of uint8 type

   \param[in]  ou8_Value   New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU8Element(const uint8_t ou8_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(ou8_Value, ou32_Index, C_OscNodeDataPoolContent::eUINT8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of uint8 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> C_OscNodeDataPoolContent::GetValueArrU8(void) const
{
   std::vector<uint8_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eUINT8, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of uint8 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint8_t C_OscNodeDataPoolContent::GetValueArrU8Element(const uint32_t ou32_Index) const
{
   uint8_t u8_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eUINT8, ou32_Index, u8_Value);
   return u8_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of uint16 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU16(const std::vector<uint16_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eUINT16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of uint16 type

   \param[in]  ou16_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU16Element(const uint16_t ou16_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(ou16_Value, ou32_Index, C_OscNodeDataPoolContent::eUINT16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of uint16 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint16_t> C_OscNodeDataPoolContent::GetValueArrU16(void) const
{
   std::vector<uint16_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eUINT16, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of uint16 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint16_t C_OscNodeDataPoolContent::GetValueArrU16Element(const uint32_t ou32_Index) const
{
   uint16_t u16_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eUINT16, ou32_Index, u16_Value);
   return u16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: set current value for array of any type

   \param[in]  orc_Value   New values
   \param[in]  oe_Type     supposed type held in this class

   \return
   No direct return. If we do not hold an array type exception C_CONFIG is thrown as sint32
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T> void C_OscNodeDataPoolContent::m_SetValueArray(const T & orc_Value, const E_Type oe_Type)
{
   if (this->mq_Array == true)
   {
      if (this->me_Type == oe_Type)
      {
         this->mc_CriticalSection.Acquire();
         mc_Data.resize(orc_Value.size() * sizeof(orc_Value[0]));
         //lint -e{9110} //we do not really use the bit representation; mc_Data is just our "BLOB" storage
         (void)std::memcpy(&this->mc_Data[0], &orc_Value[0], this->mc_Data.size());
         this->mc_CriticalSection.Release();
      }
      else
      {
         throw std::invalid_argument("Element type mismatch");
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: set new value for array element of any type

   \param[in]  orc_Value   New value
   \param[in]  ou32_Index  Index to access
   \param[in]  oe_Type     supposed type held in this class

   \return
   No direct return.
   * If we do not hold an array type exception C_CONFIG is thrown as sint32
   * On invalid index exception C_RANGE is thrown as sint32
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8058,8080} //template parameter names are not properly handled by naming convention check
template <typename T> void C_OscNodeDataPoolContent::m_SetValueArrayElement(const T & orc_Value,
                                                                            const uint32_t ou32_Index,
                                                                            const E_Type oe_Type)
{
   if (this->mq_Array == true)
   {
      if (this->me_Type == oe_Type)
      {
         if (this->GetArraySize() > ou32_Index)
         {
            this->mc_CriticalSection.Acquire();
            //lint -e{9110} //we do not really use the bit representation; mc_Data is just our "BLOB" storage
            //lint -e{9114} //range of parameter is safe for sizeof result to fit in
            (void)std::memcpy(&this->mc_Data[ou32_Index * (sizeof(orc_Value))], &orc_Value, sizeof(orc_Value));
            this->mc_CriticalSection.Release();
         }
         else
         {
            throw std::out_of_range("Array element access out of range");
         }
      }
      else
      {
         throw std::invalid_argument("Element type mismatch");
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get current value of array

   \param[in]   oe_Type       supposed type held in this class
   \param[out]  orc_Result    value of array content

   \return
   No direct return.If we do not hold an array type exception C_CONFIG is thrown as sint32
*/
//----------------------------------------------------------------------------------------------------------------------
template <typename T> void C_OscNodeDataPoolContent::m_GetValueArray(const E_Type oe_Type, T & orc_Result) const
{
   if (this->mq_Array == true)
   {
      if (this->me_Type == oe_Type)
      {
         this->mc_CriticalSection.Acquire();
         orc_Result.resize(this->mc_Data.size() / sizeof(orc_Result[0]));
         //lint -e{9110} //we do not really use the bit representation; mc_Data is just our "BLOB" storage
         (void)std::memcpy(&orc_Result[0], &this->mc_Data[0], this->mc_Data.size());
         this->mc_CriticalSection.Release();
      }
      else
      {
         throw std::invalid_argument("Element type mismatch");
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get current value of array element

   \param[in]   oe_Type       supposed type held in this class
   \param[in]   ou32_Index    index within array
   \param[out]  orc_Result    value of array element

   \return
   No direct return.
   * If we do not hold an array type exception C_CONFIG is thrown as sint32
   * On invalid index exception C_RANGE is thrown as sint32
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{8058,8080} //template parameter names are not properly handled by naming convention check
template <typename T> void C_OscNodeDataPoolContent::m_GetValueArrayElement(const E_Type oe_Type,
                                                                            const uint32_t ou32_Index,
                                                                            T & orc_Result) const
{
   if (this->mq_Array == true)
   {
      if (this->me_Type == oe_Type)
      {
         if (this->GetArraySize() > ou32_Index)
         {
            this->mc_CriticalSection.Acquire();
            //lint -e{9110} //we do not really use the bit representation; mc_Data is just our "BLOB" storage
            //lint -e{9114} //range of parameter is safe for sizeof result to fit in
            (void)std::memcpy(&orc_Result, &this->mc_Data[ou32_Index * sizeof(orc_Result)], sizeof(orc_Result));
            this->mc_CriticalSection.Release();
         }
         else
         {
            throw std::out_of_range("Array element access out of range");
         }
      }
      else
      {
         throw std::invalid_argument("Element type mismatch");
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of uint32 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU32(const std::vector<uint32_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eUINT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of uint32 type

   \param[in]  ou32_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU32Element(const uint32_t ou32_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(ou32_Value, ou32_Index, C_OscNodeDataPoolContent::eUINT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of uint32 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint32_t> C_OscNodeDataPoolContent::GetValueArrU32(void) const
{
   std::vector<uint32_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eUINT32, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of uint32 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolContent::GetValueArrU32Element(const uint32_t ou32_Index) const
{
   uint32_t u32_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eUINT32, ou32_Index, u32_Value);
   return u32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of uint64 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU64(const std::vector<uint64_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eUINT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of uint64 type

   \param[in]  ou64_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrU64Element(const uint64_t ou64_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(ou64_Value, ou32_Index, C_OscNodeDataPoolContent::eUINT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of uint64 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint64_t> C_OscNodeDataPoolContent::GetValueArrU64(void) const
{
   std::vector<uint64_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eUINT64, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of uint64 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_OscNodeDataPoolContent::GetValueArrU64Element(const uint32_t ou32_Index) const
{
   uint64_t u64_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eUINT64, ou32_Index, u64_Value);
   return u64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of sint8 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS8(const std::vector<int8_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eSINT8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of sint8 type

   \param[in]  os8_Value   New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS8Element(const int8_t os8_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(os8_Value, ou32_Index, C_OscNodeDataPoolContent::eSINT8);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of sint8 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int8_t> C_OscNodeDataPoolContent::GetValueArrS8(void) const
{
   std::vector<int8_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eSINT8, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of sint8 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int8_t C_OscNodeDataPoolContent::GetValueArrS8Element(const uint32_t ou32_Index) const
{
   int8_t s8_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eSINT8, ou32_Index, s8_Value);
   return s8_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of sint16 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS16(const std::vector<int16_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eSINT16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of sint16 type

   \param[in]  os16_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS16Element(const int16_t os16_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(os16_Value, ou32_Index, C_OscNodeDataPoolContent::eSINT16);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of sint16 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int16_t> C_OscNodeDataPoolContent::GetValueArrS16(void) const
{
   std::vector<int16_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eSINT16, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of sint16 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int16_t C_OscNodeDataPoolContent::GetValueArrS16Element(const uint32_t ou32_Index) const
{
   int16_t s16_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eSINT16, ou32_Index, s16_Value);
   return s16_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of sint32 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS32(const std::vector<int32_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eSINT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of sint32 type

   \param[in]  os32_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS32Element(const int32_t os32_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(os32_Value, ou32_Index, C_OscNodeDataPoolContent::eSINT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of sint32 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int32_t> C_OscNodeDataPoolContent::GetValueArrS32(void) const
{
   std::vector<int32_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eSINT32, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of sint32 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolContent::GetValueArrS32Element(const uint32_t ou32_Index) const
{
   int32_t s32_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eSINT32, ou32_Index, s32_Value);
   return s32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of sint64 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS64(const std::vector<int64_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eSINT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of sint64 type

   \param[in]  os64_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrS64Element(const int64_t os64_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(os64_Value, ou32_Index, C_OscNodeDataPoolContent::eSINT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of sint64 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<int64_t> C_OscNodeDataPoolContent::GetValueArrS64(void) const
{
   std::vector<int64_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eSINT64, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of sint64 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
int64_t C_OscNodeDataPoolContent::GetValueArrS64Element(const uint32_t ou32_Index) const
{
   int64_t s64_Value = 0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eSINT64, ou32_Index, s64_Value);
   return s64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of float32 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrF32(const std::vector<float32_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eFLOAT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of float32 type

   \param[in]  of32_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrF32Element(const float32_t of32_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(of32_Value, ou32_Index, C_OscNodeDataPoolContent::eFLOAT32);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of float32 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<float32_t> C_OscNodeDataPoolContent::GetValueArrF32(void) const
{
   std::vector<float32_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eFLOAT32, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of float32 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
float32_t C_OscNodeDataPoolContent::GetValueArrF32Element(const uint32_t ou32_Index) const
{
   float32_t f32_Value = 0.0F;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eFLOAT32, ou32_Index, f32_Value);
   return f32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current value for array of float64 type

   \param[in]  orc_Value   New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrF64(const std::vector<float64_t> & orc_Value)
{
   m_SetValueArray(orc_Value, C_OscNodeDataPoolContent::eFLOAT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value at specific index for array of float64 type

   \param[in]  of64_Value  New value
   \param[in]  ou32_Index  Index to access
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetValueArrF64Element(const float64_t of64_Value, const uint32_t ou32_Index)
{
   m_SetValueArrayElement(of64_Value, ou32_Index, C_OscNodeDataPoolContent::eFLOAT64);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for array of float64 type

   \return
   Type match:    currently set values
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<float64_t> C_OscNodeDataPoolContent::GetValueArrF64(void) const
{
   std::vector<float64_t> c_RetVal;

   m_GetValueArray(C_OscNodeDataPoolContent::eFLOAT64, c_RetVal);
   return c_RetVal;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current value for specific index for array of float64 type

   \param[in]  ou32_Index  Index to access

   \return
   Type match:    currently set value
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_OscNodeDataPoolContent::GetValueArrF64Element(const uint32_t ou32_Index) const
{
   float64_t f64_Value = 0.0;

   m_GetValueArrayElement(C_OscNodeDataPoolContent::eFLOAT64, ou32_Index, f64_Value);
   return f64_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resize current array

   \param[in]  oru32_Size  New size value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetArraySize(const uint32_t & oru32_Size)
{
   if (this->mq_Array == true)
   {
      switch (this->me_Type)
      {
      case eUINT8:
      case eSINT8:
         this->mc_Data.resize(oru32_Size, 0U);
         break;
      case eUINT16:
      case eSINT16:
         this->mc_Data.resize(static_cast<size_t>(oru32_Size) * 2, 0U);
         break;
      case eUINT32:
      case eSINT32:
      case eFLOAT32:
         this->mc_Data.resize(static_cast<size_t>(oru32_Size) * 4, 0U);
         break;
      case eUINT64:
      case eSINT64:
      case eFLOAT64:
         this->mc_Data.resize(static_cast<size_t>(oru32_Size) * 8, 0U);
         break;
      default:
         throw std::invalid_argument("Type mismatch (incorrect data type)");
         break;
      }
   }
   else
   {
      throw std::invalid_argument("Element type mismatch");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current array size

   \return
   Type match:    current size
   Type mismatch: Exception C_CONFIG
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolContent::GetArraySize(void) const
{
   uint32_t u32_Retval = 1;

   if (this->mq_Array == true)
   {
      switch (this->me_Type)
      {
      case eUINT8:
      case eSINT8:
         u32_Retval = static_cast<uint32_t>(this->mc_Data.size());
         break;
      case eUINT16:
      case eSINT16:
         u32_Retval = static_cast<uint32_t>(this->mc_Data.size() / 2);
         break;
      case eUINT32:
      case eSINT32:
      case eFLOAT32:
         u32_Retval = static_cast<uint32_t>(this->mc_Data.size() / 4);
         break;
      case eUINT64:
      case eSINT64:
      case eFLOAT64:
         u32_Retval = static_cast<uint32_t>(this->mc_Data.size() / 8);
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }
   }
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change type

   Attempt conversion of previous value(s) to new type

   \param[in]  ore_Value   New Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetType(const E_Type & ore_Value)
{
   if (this->mq_Array == false)
   {
      bool q_UnsignedPreviousBase = false;
      bool q_SignedPreviousBase = false;
      bool q_FloatPreviousBase = false;
      bool q_UnsignedNewBase = false;
      bool q_SignedNewBase = false;
      bool q_FloatNewBase = false;
      uint64_t u64_PreviousValue = 0;
      int64_t s64_PreviousValue = 0;
      float64_t f64_PreviousValue = 0.0;
      uint64_t u64_NewValue = 0;
      int64_t s64_NewValue = 0;
      float64_t f64_NewValue = 0.0;

      //Get previous value
      m_GetBaseType(q_UnsignedPreviousBase, q_SignedPreviousBase, q_FloatPreviousBase, u64_PreviousValue,
                    s64_PreviousValue,
                    f64_PreviousValue);

      //Apply new type
      this->me_Type = ore_Value;

      //set new size
      switch (this->me_Type)
      {
      case eUINT8:
      case eSINT8:
         this->mc_Data.resize(1);
         break;
      case eUINT16:
      case eSINT16:
         this->mc_Data.resize(2);
         break;
      case eUINT32:
      case eSINT32:
      case eFLOAT32:
         this->mc_Data.resize(4);
         break;
      case eUINT64:
      case eSINT64:
      case eFLOAT64:
         this->mc_Data.resize(8);
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }

      //Get new value type
      m_GetBaseType(q_UnsignedNewBase, q_SignedNewBase, q_FloatNewBase, u64_NewValue, s64_NewValue, f64_NewValue);

      //Convert value
      if (q_UnsignedNewBase == true)
      {
         if (q_UnsignedPreviousBase == true)
         {
            u64_NewValue = u64_PreviousValue;
         }
         else if (q_SignedPreviousBase == true)
         {
            u64_NewValue = static_cast<uint64_t>(s64_PreviousValue);
         }
         else if (q_FloatPreviousBase == true)
         {
            u64_NewValue = static_cast<uint64_t>(f64_PreviousValue);
         }
         else
         {
            //Type unknown
         }
      }
      else if (q_SignedNewBase == true)
      {
         if (q_UnsignedPreviousBase == true)
         {
            s64_NewValue = static_cast<int64_t>(u64_PreviousValue);
         }
         else if (q_SignedPreviousBase == true)
         {
            s64_NewValue = s64_PreviousValue;
         }
         else if (q_FloatPreviousBase == true)
         {
            s64_NewValue = static_cast<int64_t>(f64_PreviousValue);
         }
         else
         {
            //Type unknown
         }
      }
      else if (q_FloatNewBase == true)
      {
         if (q_UnsignedPreviousBase == true)
         {
            f64_NewValue = static_cast<float64_t>(u64_PreviousValue);
         }
         else if (q_SignedPreviousBase == true)
         {
            f64_NewValue = static_cast<float64_t>(s64_PreviousValue);
         }
         else if (q_FloatPreviousBase == true)
         {
            f64_NewValue = f64_PreviousValue;
         }
         else
         {
            //Type unknown
         }
      }
      else
      {
         //Type unknown
      }

      //Set new value
      switch (this->me_Type)
      {
      case eUINT8:
         this->SetValueU8(static_cast<uint8_t>(u64_NewValue));
         break;
      case eUINT16:
         this->SetValueU16(static_cast<uint16_t>(u64_NewValue));
         break;
      case eUINT32:
         this->SetValueU32(static_cast<uint32_t>(u64_NewValue));
         break;
      case eUINT64:
         this->SetValueU64(u64_NewValue);
         break;
      case eSINT8:
         this->SetValueS8(static_cast<int8_t>(s64_NewValue));
         break;
      case eSINT16:
         this->SetValueS16(static_cast<int16_t>(s64_NewValue));
         break;
      case eSINT32:
         this->SetValueS32(static_cast<int32_t>(s64_NewValue));
         break;
      case eSINT64:
         this->SetValueS64(s64_NewValue);
         break;
      case eFLOAT32:
         this->SetValueF32(static_cast<float32_t>(f64_NewValue));
         break;
      case eFLOAT64:
         this->SetValueF64(f64_NewValue);
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }
   }
   else
   {
      const uint32_t u32_Size = this->GetArraySize();

      //get copy of existing data:
      const C_OscNodeDataPoolContent c_OldData = (*this);

      //Apply new type
      this->me_Type = ore_Value;
      //size new data:
      switch (ore_Value)
      {
      case eUINT8:
      case eSINT8:
         this->mc_Data.resize(u32_Size);
         break;
      case eUINT16:
      case eSINT16:
         this->mc_Data.resize(static_cast<size_t>(u32_Size) * 2);
         break;
      case eUINT32:
      case eSINT32:
      case eFLOAT32:
         this->mc_Data.resize(static_cast<size_t>(u32_Size) * 4);
         break;
      case eUINT64:
      case eSINT64:
      case eFLOAT64:
         this->mc_Data.resize(static_cast<size_t>(u32_Size) * 8);
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }
      //Copy data (if any)
      for (uint32_t u32_ItArray = 0; u32_ItArray < u32_Size; ++u32_ItArray)
      {
         uint64_t u64_PreviousValue = 0;
         int64_t s64_PreviousValue = 0;
         float64_t f64_PreviousValue = 0.0;
         uint64_t u64_NewValue = 0;
         int64_t s64_NewValue = 0;
         float64_t f64_NewValue = 0.0;
         bool q_UnsignedPreviousBase = false;
         bool q_SignedPreviousBase = false;
         bool q_FloatPreviousBase = false;
         bool q_UnsignedNewBase = false;
         bool q_SignedNewBase = false;
         bool q_FloatNewBase = false;

         //Get previous value
         c_OldData.m_GetBaseTypeArray(u32_ItArray, q_UnsignedPreviousBase, q_SignedPreviousBase, q_FloatPreviousBase,
                                      u64_PreviousValue, s64_PreviousValue, f64_PreviousValue);
         //Get new value type
         this->m_GetBaseTypeArray(u32_ItArray, q_UnsignedNewBase, q_SignedNewBase, q_FloatNewBase, u64_NewValue,
                                  s64_NewValue, f64_NewValue);

         //Convert value
         if (q_UnsignedNewBase == true)
         {
            if (q_UnsignedPreviousBase == true)
            {
               u64_NewValue = u64_PreviousValue;
            }
            else if (q_SignedPreviousBase == true)
            {
               u64_NewValue = static_cast<uint64_t>(s64_PreviousValue);
            }
            else if (q_FloatPreviousBase == true)
            {
               u64_NewValue = static_cast<uint64_t>(f64_PreviousValue);
            }
            else
            {
               //Type unknown
            }
         }
         else if (q_SignedNewBase == true)
         {
            if (q_UnsignedPreviousBase == true)
            {
               s64_NewValue = static_cast<int64_t>(u64_PreviousValue);
            }
            else if (q_SignedPreviousBase == true)
            {
               s64_NewValue = s64_PreviousValue;
            }
            else if (q_FloatPreviousBase == true)
            {
               s64_NewValue = static_cast<int64_t>(f64_PreviousValue);
            }
            else
            {
               //Type unknown
            }
         }
         else if (q_FloatNewBase == true)
         {
            if (q_UnsignedPreviousBase == true)
            {
               f64_NewValue = static_cast<float64_t>(u64_PreviousValue);
            }
            else if (q_SignedPreviousBase == true)
            {
               f64_NewValue = static_cast<float64_t>(s64_PreviousValue);
            }
            else if (q_FloatPreviousBase == true)
            {
               f64_NewValue = f64_PreviousValue;
            }
            else
            {
               //Type unknown
            }
         }
         else
         {
            //Type unknown
         }

         //Set new value
         switch (this->me_Type)
         {
         case eUINT8:
            this->SetValueArrU8Element(static_cast<uint8_t>(u64_NewValue), u32_ItArray);
            break;
         case eUINT16:
            this->SetValueArrU16Element(static_cast<uint16_t>(u64_NewValue), u32_ItArray);
            break;
         case eUINT32:
            this->SetValueArrU32Element(static_cast<uint32_t>(u64_NewValue), u32_ItArray);
            break;
         case eUINT64:
            this->SetValueArrU64Element(u64_NewValue, u32_ItArray);
            break;
         case eSINT8:
            this->SetValueArrS8Element(static_cast<int8_t>(s64_NewValue), u32_ItArray);
            break;
         case eSINT16:
            this->SetValueArrS16Element(static_cast<int16_t>(s64_NewValue), u32_ItArray);
            break;
         case eSINT32:
            this->SetValueArrS32Element(static_cast<int32_t>(s64_NewValue), u32_ItArray);
            break;
         case eSINT64:
            this->SetValueArrS64Element(s64_NewValue, u32_ItArray);
            break;
         case eFLOAT32:
            this->SetValueArrF32Element(static_cast<float32_t>(f64_NewValue), u32_ItArray);
            break;
         case eFLOAT64:
            this->SetValueArrF64Element(f64_NewValue, u32_ItArray);
            break;
         default:
            throw std::invalid_argument("Element type mismatch");
            break;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get currently set type

   \return
   Currently set type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_OscNodeDataPoolContent::GetType(void) const
{
   return me_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current array status

   \return
   true:  is array
   false: single element
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::GetArray(void) const
{
   return mq_Array;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change array status

   \param[in]  oq_Value    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::SetArray(const bool oq_Value)
{
   if (this->mq_Array != oq_Value)
   {
      this->mq_Array = oq_Value;
      //if it's an array start with one element; if not we only have one element ...

      //Resize data
      switch (this->me_Type)
      {
      case eUINT8:
      case eSINT8:
         this->mc_Data.resize(1);
         break;
      case eUINT16:
      case eSINT16:
         this->mc_Data.resize(2);
         break;
      case eUINT32:
      case eSINT32:
      case eFLOAT32:
         this->mc_Data.resize(4);
         break;
      case eUINT64:
      case eSINT64:
      case eFLOAT64:
         this->mc_Data.resize(8);
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get size in bytes of currently set data

   \return
   size of contained element in bytes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolContent::GetSizeByte(void) const
{
   return static_cast<uint32_t>(mc_Data.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current content value(s) inside range of new type

   \param[in]  ore_Type    Type to test for

   \return
   true  Conversion possible
   false Adaptation necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::CheckInsideRange(const C_OscNodeDataPoolContent::E_Type & ore_Type) const
{
   bool q_Retval = true;
   bool q_ThisBaseIsUnsigned = false;
   bool q_ThisBaseIsSigned = false;
   bool q_ThisBaseIsFloatingPoint = false;
   uint64_t u64_Value = 0;
   int64_t s64_Value = 0;
   float64_t f64_Value = 0.0;

   for (uint32_t u32_ItElement = 0; u32_ItElement < this->GetArraySize(); ++u32_ItElement)
   {
      if (this->mq_Array == false)
      {
         m_GetBaseType(q_ThisBaseIsUnsigned, q_ThisBaseIsSigned, q_ThisBaseIsFloatingPoint, u64_Value, s64_Value,
                       f64_Value);
      }
      else
      {
         m_GetBaseTypeArray(u32_ItElement, q_ThisBaseIsUnsigned, q_ThisBaseIsSigned, q_ThisBaseIsFloatingPoint,
                            u64_Value, s64_Value, f64_Value);
      }
      switch (ore_Type)
      {
      case C_OscNodeDataPoolContent::eUINT8:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<uint8_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if ((static_cast<int64_t>(std::numeric_limits<uint8_t>::min()) <= s64_Value) &&
                (s64_Value <= static_cast<int64_t>(std::numeric_limits<uint8_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<uint8_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<uint8_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if ((static_cast<int64_t>(std::numeric_limits<uint16_t>::min()) <= s64_Value) &&
                (s64_Value <= static_cast<int64_t>(std::numeric_limits<uint16_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<uint16_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<uint16_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if ((static_cast<int64_t>(std::numeric_limits<uint32_t>::min()) <= s64_Value) &&
                (s64_Value <= static_cast<int64_t>(std::numeric_limits<uint32_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<uint32_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<uint32_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         if (q_ThisBaseIsUnsigned == true)
         {
            q_Retval = true;
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if (static_cast<int64_t>(std::numeric_limits<uint64_t>::min()) <= s64_Value)
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<uint64_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<uint64_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<int8_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if ((static_cast<int64_t>(std::numeric_limits<int8_t>::min()) <= s64_Value) &&
                (s64_Value <= static_cast<int64_t>(std::numeric_limits<int8_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<int8_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<int8_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<int16_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if ((static_cast<int64_t>(std::numeric_limits<int16_t>::min()) <= s64_Value) &&
                (s64_Value <= static_cast<int64_t>(std::numeric_limits<int16_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<int16_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<int16_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<int32_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if ((static_cast<int64_t>(std::numeric_limits<int32_t>::min()) <= s64_Value) &&
                (s64_Value <= static_cast<int64_t>(std::numeric_limits<int32_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<int32_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<int32_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            q_Retval = true;
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((static_cast<float64_t>(std::numeric_limits<int64_t>::min()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<int64_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<float32_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if (((-std::numeric_limits<float32_t>::max()) <= static_cast<float32_t>(s64_Value)) &&
                (static_cast<float32_t>(s64_Value) <= std::numeric_limits<float32_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            if ((-static_cast<float64_t>(std::numeric_limits<float32_t>::max()) <= f64_Value) &&
                (f64_Value <= static_cast<float64_t>(std::numeric_limits<float32_t>::max())))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else
         {
            //Unknown type
         }
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         if (q_ThisBaseIsUnsigned == true)
         {
            if (u64_Value <= static_cast<uint64_t>(std::numeric_limits<float64_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsSigned == true)
         {
            if (((-std::numeric_limits<float64_t>::max()) <= static_cast<float64_t>(s64_Value)) &&
                (static_cast<float64_t>(s64_Value) <= std::numeric_limits<float64_t>::max()))
            {
               q_Retval = true;
            }
            else
            {
               q_Retval = false;
            }
         }
         else if (q_ThisBaseIsFloatingPoint == true)
         {
            q_Retval = true;
         }
         else
         {
            //Unknown type
         }
         break;
      default:
         //Unknown min/max
         break;
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller or equal than orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current smaller or equal than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::operator <=(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() <= orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() <= orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval =  (this->GetValueU32() <= orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() <= orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() <= orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() <= orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() <= orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() <= orc_Cmp.GetValueS64());
            break;
         case eFLOAT32:
            q_Retval = (this->GetValueF32() <= orc_Cmp.GetValueF32());
            break;
         case eFLOAT64:
            q_Retval = (this->GetValueF64() <= orc_Cmp.GetValueF64());
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            q_Retval = true;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               if (this->CompareArrayGreater(orc_Cmp, u32_ItArray) == true)
               {
                  q_Retval = false;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current greater or equal than orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current greater or equal than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::operator >=(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() >= orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() >= orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval = (this->GetValueU32() >= orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() >= orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() >= orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() >= orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() >= orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() >= orc_Cmp.GetValueS64());
            break;
         case eFLOAT32:
            q_Retval = (this->GetValueF32() >= orc_Cmp.GetValueF32());
            break;
         case eFLOAT64:
            q_Retval = (this->GetValueF64() >= orc_Cmp.GetValueF64());
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            q_Retval = true;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               if (this->CompareArrayGreaterOrEqual(orc_Cmp, u32_ItArray) == false)
               {
                  q_Retval = false;
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current greater than orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current greater than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::operator >(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() > orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() > orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval = (this->GetValueU32() > orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() > orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() > orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() > orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() > orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() > orc_Cmp.GetValueS64());
            break;
         case eFLOAT32:
            q_Retval = (this->GetValueF32() > orc_Cmp.GetValueF32());
            break;
         case eFLOAT64:
            q_Retval = (this->GetValueF64() > orc_Cmp.GetValueF64());
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            q_Retval = true;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               if (this->CompareArrayGreater(orc_Cmp, u32_ItArray) == false)
               {
                  q_Retval = false;
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current smaller than orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current smaller than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::operator <(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() < orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() < orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval = (this->GetValueU32() < orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() < orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() < orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() < orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() < orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() < orc_Cmp.GetValueS64());
            break;
         case eFLOAT32:
            q_Retval = (this->GetValueF32() < orc_Cmp.GetValueF32());
            break;
         case eFLOAT64:
            q_Retval = (this->GetValueF64() < orc_Cmp.GetValueF64());
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            q_Retval = true;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               if (this->CompareArrayGreaterOrEqual(orc_Cmp, u32_ItArray) == true)
               {
                  q_Retval = false;
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current equal to orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::operator ==(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() == orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() == orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval = (this->GetValueU32() == orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() == orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() == orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() == orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() == orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() == orc_Cmp.GetValueS64());
            break;
         case eFLOAT32:
            q_Retval = (C_OscUtils::h_IsFloat32NearlyEqual(this->GetValueF32(), orc_Cmp.GetValueF32()));
            break;
         case eFLOAT64:
            q_Retval = (C_OscUtils::h_IsFloat64NearlyEqual(this->GetValueF64(), orc_Cmp.GetValueF64()));
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            q_Retval = true;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               if (this->m_CompareArrayNotEqual(orc_Cmp, u32_ItArray) == true)
               {
                  q_Retval = false;
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if current not equal to orc_Cmp

   Warning: Due to array comparison no comparison function may be combined.

   \param[in]  orc_Cmp  Compared instance

   \return
   Current not equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::operator !=(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() != orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() != orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval = (this->GetValueU32() != orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() != orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() != orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() != orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() != orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() != orc_Cmp.GetValueS64());
            break;
         case eFLOAT32:
            q_Retval = (C_OscUtils::h_IsFloat32NearlyEqual(this->GetValueF32(), orc_Cmp.GetValueF32()) == false);
            break;
         case eFLOAT64:
            q_Retval = (C_OscUtils::h_IsFloat64NearlyEqual(this->GetValueF64(), orc_Cmp.GetValueF64()) == false);
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            //If empty, it is equal
            q_Retval = false;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               //If one is not equal
               if (this->CompareArrayEqual(orc_Cmp, u32_ItArray) == false)
               {
                  //Not equal
                  q_Retval = true;
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare content strict

   \param[in]  orc_Cmp  Compared instance

   \return
   Flags

   \retval   True    Equal to orc_Cmp
   \retval   False   Not equal to orc_Cmp
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::CompareContentStrict(const C_OscNodeDataPoolContent & orc_Cmp) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if (this->mq_Array == false)
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueU8() == orc_Cmp.GetValueU8());
            break;
         case eUINT16:
            q_Retval = (this->GetValueU16() == orc_Cmp.GetValueU16());
            break;
         case eUINT32:
            q_Retval = (this->GetValueU32() == orc_Cmp.GetValueU32());
            break;
         case eUINT64:
            q_Retval = (this->GetValueU64() == orc_Cmp.GetValueU64());
            break;
         case eSINT8:
            q_Retval = (this->GetValueS8() == orc_Cmp.GetValueS8());
            break;
         case eSINT16:
            q_Retval = (this->GetValueS16() == orc_Cmp.GetValueS16());
            break;
         case eSINT32:
            q_Retval = (this->GetValueS32() == orc_Cmp.GetValueS32());
            break;
         case eSINT64:
            q_Retval = (this->GetValueS64() == orc_Cmp.GetValueS64());
            break;
         //lint -e{777} Required float equal check
         case eFLOAT32:
            q_Retval = this->GetValueF32() == orc_Cmp.GetValueF32();
            break;
         //lint -e{777} Required float equal check
         case eFLOAT64:
            q_Retval = this->GetValueF64() == orc_Cmp.GetValueF64();
            break;
         default:
            break;
         }
      }
      else
      {
         if (this->GetArraySize() == orc_Cmp.GetArraySize())
         {
            q_Retval = true;
            for (uint32_t u32_ItArray = 0; u32_ItArray < this->GetArraySize(); ++u32_ItArray)
            {
               if (this->m_CompareArrayNotEqual(orc_Cmp, u32_ItArray) == true)
               {
                  q_Retval = false;
                  break;
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if value at array index greater or equal to orc_Cmp

   \param[in]  orc_Cmp     Compared instance
   \param[in]  ou32_Index  Array index instance

   \return
   Current greater or equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::CompareArrayGreaterOrEqual(const C_OscNodeDataPoolContent & orc_Cmp,
                                                          const uint32_t ou32_Index) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if ((this->GetArraySize() == orc_Cmp.GetArraySize()) && (ou32_Index < this->GetArraySize()))
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueArrU8Element(ou32_Index) >= orc_Cmp.GetValueArrU8Element(ou32_Index));
            break;
         case eUINT16:
            q_Retval = (this->GetValueArrU16Element(ou32_Index) >= orc_Cmp.GetValueArrU16Element(ou32_Index));
            break;
         case eUINT32:
            q_Retval = (this->GetValueArrU32Element(ou32_Index) >= orc_Cmp.GetValueArrU32Element(ou32_Index));
            break;
         case eUINT64:
            q_Retval = (this->GetValueArrU64Element(ou32_Index) >= orc_Cmp.GetValueArrU64Element(ou32_Index));
            break;
         case eSINT8:
            q_Retval = (this->GetValueArrS8Element(ou32_Index) >= orc_Cmp.GetValueArrS8Element(ou32_Index));
            break;
         case eSINT16:
            q_Retval = (this->GetValueArrS16Element(ou32_Index) >= orc_Cmp.GetValueArrS16Element(ou32_Index));
            break;
         case eSINT32:
            q_Retval = (this->GetValueArrS32Element(ou32_Index) >= orc_Cmp.GetValueArrS32Element(ou32_Index));
            break;
         case eSINT64:
            q_Retval = (this->GetValueArrS64Element(ou32_Index) >= orc_Cmp.GetValueArrS64Element(ou32_Index));
            break;
         case eFLOAT32:
            q_Retval = (this->GetValueArrF32Element(ou32_Index) >= orc_Cmp.GetValueArrF32Element(ou32_Index));
            break;
         case eFLOAT64:
            q_Retval = (this->GetValueArrF64Element(ou32_Index) >= orc_Cmp.GetValueArrF64Element(ou32_Index));
            break;
         default:
            break;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if value at array index greater than orc_Cmp

   \param[in]  orc_Cmp     Compared instance
   \param[in]  ou32_Index  Array index instance

   \return
   Current greater than orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::CompareArrayGreater(const C_OscNodeDataPoolContent & orc_Cmp,
                                                   const uint32_t ou32_Index) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if ((this->GetArraySize() == orc_Cmp.GetArraySize()) && (ou32_Index < this->GetArraySize()))
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueArrU8Element(ou32_Index) > orc_Cmp.GetValueArrU8Element(ou32_Index));
            break;
         case eUINT16:
            q_Retval = (this->GetValueArrU16Element(ou32_Index) > orc_Cmp.GetValueArrU16Element(ou32_Index));
            break;
         case eUINT32:
            q_Retval = (this->GetValueArrU32Element(ou32_Index) > orc_Cmp.GetValueArrU32Element(ou32_Index));
            break;
         case eUINT64:
            q_Retval = (this->GetValueArrU64Element(ou32_Index) > orc_Cmp.GetValueArrU64Element(ou32_Index));
            break;
         case eSINT8:
            q_Retval = (this->GetValueArrS8Element(ou32_Index) > orc_Cmp.GetValueArrS8Element(ou32_Index));
            break;
         case eSINT16:
            q_Retval = (this->GetValueArrS16Element(ou32_Index) > orc_Cmp.GetValueArrS16Element(ou32_Index));
            break;
         case eSINT32:
            q_Retval = (this->GetValueArrS32Element(ou32_Index) > orc_Cmp.GetValueArrS32Element(ou32_Index));
            break;
         case eSINT64:
            q_Retval = (this->GetValueArrS64Element(ou32_Index) > orc_Cmp.GetValueArrS64Element(ou32_Index));
            break;
         case eFLOAT32:
            q_Retval = (this->GetValueArrF32Element(ou32_Index) > orc_Cmp.GetValueArrF32Element(ou32_Index));
            break;
         case eFLOAT64:
            q_Retval = (this->GetValueArrF64Element(ou32_Index) > orc_Cmp.GetValueArrF64Element(ou32_Index));
            break;
         default:
            break;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all value as scaled string

   Info: - floats doubles and any scaled value will be displayed in exponential format with 6 digits precision,
           e.g. 2.45823e+06
         - The postfix e+01 will be skipped (if the number has less than 6 digits)
         - The last digit will get rounded if necessary

   \param[in]   of64_Factor               Scaling factor
   \param[in]   of64_Offset               Scaling offset
   \param[out]  orc_Output                Scaled string
   \param[in]   ou32_Index                Index to use in case of array
   \param[in]   oq_AllowRangeAdaptation   Allow range adaptation
   \param[in]   oq_AllowSpecialHandling   Allow special handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::GetValueAsScaledString(const float64_t of64_Factor, const float64_t of64_Offset,
                                                      std::string & orc_Output, const uint32_t ou32_Index,
                                                      const bool oq_AllowRangeAdaptation,
                                                      const bool oq_AllowSpecialHandling) const
{
   std::stringstream c_Stream;

   if (C_OscUtils::h_IsScalingActive(of64_Factor, of64_Offset) == true)
   {
      float64_t f64_Tmp;
      this->GetAnyValueAsFloat64(f64_Tmp, ou32_Index);
      f64_Tmp = C_OscUtils::h_GetValueScaled(f64_Tmp, of64_Factor, of64_Offset, oq_AllowRangeAdaptation);
      c_Stream << f64_Tmp;
   }
   else
   {
      if (this->GetArray() == false)
      {
         switch (this->GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            c_Stream << static_cast<uint32_t>(this->GetValueU8());
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            c_Stream << static_cast<uint32_t>(this->GetValueU16());
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            c_Stream << this->GetValueU32();
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            c_Stream << this->GetValueU64();
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            c_Stream << static_cast<int32_t>(this->GetValueS8());
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            c_Stream << static_cast<int32_t>(this->GetValueS16());
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            c_Stream << this->GetValueS32();
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            c_Stream << this->GetValueS64();
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            if (oq_AllowSpecialHandling == true)
            {
               C_SclString c_Precison;
               c_Precison.PrintFormatted("%.9g", this->GetValueF32());
               c_Stream << c_Precison.c_str();
            }
            else
            {
               c_Stream << this->GetValueF32();
            }
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            if (oq_AllowSpecialHandling == true)
            {
               C_SclString c_Precison;
               c_Precison.PrintFormatted("%.17g", this->GetValueF64());
               c_Stream << c_Precison.c_str();
            }
            else
            {
               c_Stream << this->GetValueF64();
            }
            break;
         default:
            break;
         }
      }
      else
      {
         switch (this->GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            c_Stream << static_cast<uint32_t>(this->GetValueArrU8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            c_Stream << static_cast<uint32_t>(this->GetValueArrU16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            c_Stream << this->GetValueArrU32Element(ou32_Index);
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            c_Stream << this->GetValueArrU64Element(ou32_Index);
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            c_Stream << static_cast<int32_t>(this->GetValueArrS8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            c_Stream << static_cast<int32_t>(this->GetValueArrS16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            c_Stream << this->GetValueArrS32Element(ou32_Index);
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            c_Stream << this->GetValueArrS64Element(ou32_Index);
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            c_Stream << this->GetValueArrF32Element(ou32_Index);
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            c_Stream << this->GetValueArrF64Element(ou32_Index);
            break;
         default:
            break;
         }
      }
   }
   orc_Output = c_Stream.str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert value to float64

   \param[in]  orf64_Output   Output value
   \param[in]  ou32_Index     Optional fallback index if array
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::GetAnyValueAsFloat64(float64_t & orf64_Output, const uint32_t ou32_Index) const
{
   orf64_Output = 0.0;

   if (this->GetArray() == true)
   {
      if (ou32_Index < this->GetArraySize())
      {
         switch (this->GetType())
         {
         case C_OscNodeDataPoolContent::eUINT8:
            orf64_Output = static_cast<float64_t>(this->GetValueArrU8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT16:
            orf64_Output = static_cast<float64_t>(this->GetValueArrU16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT32:
            orf64_Output = static_cast<float64_t>(this->GetValueArrU32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eUINT64:
            orf64_Output = static_cast<float64_t>(this->GetValueArrU64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT8:
            orf64_Output = static_cast<float64_t>(this->GetValueArrS8Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT16:
            orf64_Output = static_cast<float64_t>(this->GetValueArrS16Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT32:
            orf64_Output = static_cast<float64_t>(this->GetValueArrS32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eSINT64:
            orf64_Output = static_cast<float64_t>(this->GetValueArrS64Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT32:
            orf64_Output = static_cast<float64_t>(this->GetValueArrF32Element(ou32_Index));
            break;
         case C_OscNodeDataPoolContent::eFLOAT64:
            orf64_Output = static_cast<float64_t>(this->GetValueArrF64Element(ou32_Index));
            break;
         default:
            break;
         }
      }
   }
   else
   {
      switch (this->GetType())
      {
      case C_OscNodeDataPoolContent::eUINT8:
         orf64_Output = static_cast<float64_t>(this->GetValueU8());
         break;
      case C_OscNodeDataPoolContent::eUINT16:
         orf64_Output = static_cast<float64_t>(this->GetValueU16());
         break;
      case C_OscNodeDataPoolContent::eUINT32:
         orf64_Output = static_cast<float64_t>(this->GetValueU32());
         break;
      case C_OscNodeDataPoolContent::eUINT64:
         orf64_Output = static_cast<float64_t>(this->GetValueU64());
         break;
      case C_OscNodeDataPoolContent::eSINT8:
         orf64_Output = static_cast<float64_t>(this->GetValueS8());
         break;
      case C_OscNodeDataPoolContent::eSINT16:
         orf64_Output = static_cast<float64_t>(this->GetValueS16());
         break;
      case C_OscNodeDataPoolContent::eSINT32:
         orf64_Output = static_cast<float64_t>(this->GetValueS32());
         break;
      case C_OscNodeDataPoolContent::eSINT64:
         orf64_Output = static_cast<float64_t>(this->GetValueS64());
         break;
      case C_OscNodeDataPoolContent::eFLOAT32:
         orf64_Output = static_cast<float64_t>(this->GetValueF32());
         break;
      case C_OscNodeDataPoolContent::eFLOAT64:
         orf64_Output = static_cast<float64_t>(this->GetValueF64());
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns pointer to the used const memory of the content

   The usage of the associated critical section must be considered when accessing the memory

   \param[out]      oppc_CriticalSection   Optional pointer to the associated critical section

   \return
   Pointer to the vector with the const value data of the content
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<uint8_t> * stw::opensyde_core::C_OscNodeDataPoolContent::GetDataAccessConst(
   stw::tgl::C_TglCriticalSection ** const oppc_CriticalSection) const
{
   if (oppc_CriticalSection != NULL)
   {
      *oppc_CriticalSection = &this->mc_CriticalSection;
   }

   return &this->mc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns pointer to the used memory of the content

   The usage of the associated critical section must be considered when accessing the memory

   \param[out]      oppc_CriticalSection   Optional pointer to the associated critical section

   \return
   Pointer to the vector with the value data of the content
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint8_t> * stw::opensyde_core::C_OscNodeDataPoolContent::GetDataAccess(
   stw::tgl::C_TglCriticalSection ** const oppc_CriticalSection)
{
   if (oppc_CriticalSection != NULL)
   {
      *oppc_CriticalSection = &this->mc_CriticalSection;
   }

   //lint -e{1536} it is intended to expose a private member to increase performance when data transfers without
   // type knowledge is possible
   return &this->mc_Data;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if value at array index equal to orc_Cmp

   \param[in]  orc_Cmp     Compared instance
   \param[in]  ou32_Index  Array index instance

   \return
   Current equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::CompareArrayEqual(const C_OscNodeDataPoolContent & orc_Cmp,
                                                 const uint32_t ou32_Index) const
{
   return (m_CompareArrayNotEqual(orc_Cmp, ou32_Index) == false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if value at array index not equal to orc_Cmp

   \param[in]  orc_Cmp     Compared instance
   \param[in]  ou32_Index  Array index instance

   \return
   Current not equal to orc_Cmp
   Else false
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolContent::m_CompareArrayNotEqual(const C_OscNodeDataPoolContent & orc_Cmp,
                                                      const uint32_t ou32_Index) const
{
   bool q_Retval = false;

   if ((this->mq_Array == orc_Cmp.GetArray()) && (this->me_Type == orc_Cmp.GetType()))
   {
      if ((this->GetArraySize() == orc_Cmp.GetArraySize()) && (ou32_Index < this->GetArraySize()))
      {
         switch (this->me_Type)
         {
         case eUINT8:
            q_Retval = (this->GetValueArrU8Element(ou32_Index) != orc_Cmp.GetValueArrU8Element(ou32_Index));
            break;
         case eUINT16:
            q_Retval = (this->GetValueArrU16Element(ou32_Index) != orc_Cmp.GetValueArrU16Element(ou32_Index));
            break;
         case eUINT32:
            q_Retval = (this->GetValueArrU32Element(ou32_Index) != orc_Cmp.GetValueArrU32Element(ou32_Index));
            break;
         case eUINT64:
            q_Retval = (this->GetValueArrU64Element(ou32_Index) != orc_Cmp.GetValueArrU64Element(ou32_Index));
            break;
         case eSINT8:
            q_Retval = (this->GetValueArrS8Element(ou32_Index) != orc_Cmp.GetValueArrS8Element(ou32_Index));
            break;
         case eSINT16:
            q_Retval = (this->GetValueArrS16Element(ou32_Index) != orc_Cmp.GetValueArrS16Element(ou32_Index));
            break;
         case eSINT32:
            q_Retval = (this->GetValueArrS32Element(ou32_Index) != orc_Cmp.GetValueArrS32Element(ou32_Index));
            break;
         case eSINT64:
            q_Retval = (this->GetValueArrS64Element(ou32_Index) != orc_Cmp.GetValueArrS64Element(ou32_Index));
            break;
         case eFLOAT32:
            q_Retval = (C_OscUtils::h_IsFloat32NearlyEqual(this->GetValueArrF32Element(ou32_Index),
                                                           orc_Cmp.GetValueArrF32Element(ou32_Index)) == false);
            break;
         case eFLOAT64:
            q_Retval = (C_OscUtils::h_IsFloat64NearlyEqual(this->GetValueArrF64Element(ou32_Index),
                                                           orc_Cmp.GetValueArrF64Element(ou32_Index)) == false);
            break;
         default:
            break;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert current type and value to basic type to reduce overhead

   Warning: should only be used for non array type

   \param[out]  orq_IsUintBase      Indicator if current type is any unsigned integer type
   \param[out]  orq_IsSintBase      Indicator if current type is any signed integer type
   \param[out]  orq_IsFloatBase     Indicator if current type is any floating point type
   \param[out]  oru64_UintValue     Current value, if any unsigned integer type
   \param[out]  ors64_SintValue     Current value, if any signed integer type
   \param[out]  orf64_FloatValue    Current value, if any floating point type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::m_GetBaseType(bool & orq_IsUintBase, bool & orq_IsSintBase, bool & orq_IsFloatBase,
                                             uint64_t & oru64_UintValue, int64_t & ors64_SintValue,
                                             float64_t & orf64_FloatValue) const
{
   orq_IsUintBase = false;
   orq_IsSintBase = false;
   orq_IsFloatBase = false;
   oru64_UintValue = 0;
   ors64_SintValue = 0;
   orf64_FloatValue = 0.0;
   if (this->mq_Array == false)
   {
      switch (this->me_Type)
      {
      case eUINT8:
         oru64_UintValue = this->GetValueU8();
         orq_IsUintBase = true;
         break;
      case eUINT16:
         oru64_UintValue = this->GetValueU16();
         orq_IsUintBase = true;
         break;
      case eUINT32:
         oru64_UintValue = this->GetValueU32();
         orq_IsUintBase = true;
         break;
      case eUINT64:
         oru64_UintValue = this->GetValueU64();
         orq_IsUintBase = true;
         break;
      case eSINT8:
         ors64_SintValue = this->GetValueS8();
         orq_IsSintBase = true;
         break;
      case eSINT16:
         ors64_SintValue = this->GetValueS16();
         orq_IsSintBase = true;
         break;
      case eSINT32:
         ors64_SintValue = this->GetValueS32();
         orq_IsSintBase = true;
         break;
      case eSINT64:
         ors64_SintValue = this->GetValueS64();
         orq_IsSintBase = true;
         break;
      case eFLOAT32:
         orf64_FloatValue = this->GetValueF32();
         orq_IsFloatBase = true;
         break;
      case eFLOAT64:
         orf64_FloatValue = this->GetValueF64();
         orq_IsFloatBase = true;
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert current type and value to basic type to reduce overhead

   Warning: should only be used for array type

   \param[in]   oru32_Index         Array index
   \param[out]  orq_IsUintBase      Indicator if current type is any unsigned integer type
   \param[out]  orq_IsSintBase      Indicator if current type is any signed integer type
   \param[out]  orq_IsFloatBase     Indicator if current type is any floating point type
   \param[out]  oru64_UintValue     Current value, if any unsigned integer type
   \param[out]  ors64_SintValue     Current value, if any signed integer type
   \param[out]  orf64_FloatValue    Current value, if any floating point type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::m_GetBaseTypeArray(const uint32_t & oru32_Index, bool & orq_IsUintBase,
                                                  bool & orq_IsSintBase, bool & orq_IsFloatBase,
                                                  uint64_t & oru64_UintValue, int64_t & ors64_SintValue,
                                                  float64_t & orf64_FloatValue) const
{
   orq_IsUintBase = false;
   orq_IsSintBase = false;
   orq_IsFloatBase = false;
   oru64_UintValue = 0;
   ors64_SintValue = 0;
   orf64_FloatValue = 0.0;
   if (this->mq_Array == true)
   {
      switch (this->me_Type)
      {
      case eUINT8:
         oru64_UintValue = this->GetValueArrU8Element(oru32_Index);
         orq_IsUintBase = true;
         break;
      case eUINT16:
         oru64_UintValue = this->GetValueArrU16Element(oru32_Index);
         orq_IsUintBase = true;
         break;
      case eUINT32:
         oru64_UintValue = this->GetValueArrU32Element(oru32_Index);
         orq_IsUintBase = true;
         break;
      case eUINT64:
         oru64_UintValue = this->GetValueArrU64Element(oru32_Index);
         orq_IsUintBase = true;
         break;
      case eSINT8:
         ors64_SintValue = this->GetValueArrS8Element(oru32_Index);
         orq_IsSintBase = true;
         break;
      case eSINT16:
         ors64_SintValue = this->GetValueArrS16Element(oru32_Index);
         orq_IsSintBase = true;
         break;
      case eSINT32:
         ors64_SintValue = this->GetValueArrS32Element(oru32_Index);
         orq_IsSintBase = true;
         break;
      case eSINT64:
         ors64_SintValue = this->GetValueArrS64Element(oru32_Index);
         orq_IsSintBase = true;
         break;
      case eFLOAT32:
         orf64_FloatValue = this->GetValueArrF32Element(oru32_Index);
         orq_IsFloatBase = true;
         break;
      case eFLOAT64:
         orf64_FloatValue = this->GetValueArrF64Element(oru32_Index);
         orq_IsFloatBase = true;
         break;
      default:
         throw std::invalid_argument("Element type mismatch");
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value from big endian BLOB

   Convert BLOB to values.
   Considers configured element type.

   \param[in]  orc_Data    data to set

   \return
   C_NO_ERR   value set
   C_CONFIG   size of orc_Data does not match our size
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolContent::SetValueFromBigEndianBlob(const std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return = C_NO_ERR;
   //using unions is not nice but more portable than reinterpret_casting
   //lint -e{9018}  //cf. comment above
   union U_Union32
   {
      uint32_t u32_Value;
      float32_t f32_Value;
   };

   //lint -e{9018}  //cf. comment above
   union U_Union64
   {
      uint64_t u64_Value;
      float64_t f64_Value;
   };

   //is size correct ?
   if (this->GetSizeByte() != orc_Data.size())
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      if (this->GetArray() == false)
      {
         //plain type
         switch (this->me_Type)
         {
         case eUINT8: ///< Data type unsigned 8 bit integer
            this->SetValueU8(orc_Data[0]);
            break;
         case eUINT16: ///< Data type unsigned 16 bit integer
            this->SetValueU16(static_cast<uint16_t>((static_cast<uint16_t>(orc_Data[0]) << 8U)) + orc_Data[1]);
            break;
         case eUINT32: ///< Data type unsigned 32 bit integer
            this->SetValueU32(mh_BinaryToUnsignedInt32Big(&orc_Data[0]));
            break;
         case eUINT64: ///< Data type unsigned 64 bit integer
            this->SetValueU64(mh_BinaryToUnsignedInt64Big(&orc_Data[0]));
            break;
         case eSINT8: ///< Data type signed 8 bit integer
            this->SetValueS8(orc_Data[0]);
            break;
         case eSINT16: ///< Data type signed 16 bit integer
            this->SetValueS16(
               static_cast<int16_t>(static_cast<uint16_t>((static_cast<uint16_t>(orc_Data[0]) << 8U))) + orc_Data[1]);
            break;
         case eSINT32: ///< Data type signed 32 bit integer
            this->SetValueS32(static_cast<int32_t>(mh_BinaryToUnsignedInt32Big(&orc_Data[0])));
            break;
         case eSINT64: ///< Data type signed 64 bit integer
            this->SetValueS64(static_cast<int64_t>(mh_BinaryToUnsignedInt64Big(&orc_Data[0])));
            break;
         case eFLOAT32: ///< Data type 32 bit floating point
            U_Union32 u_Value32;
            u_Value32.u32_Value = mh_BinaryToUnsignedInt32Big(&orc_Data[0]);
            this->SetValueF32(u_Value32.f32_Value);
            break;
         case eFLOAT64: ///< Data type 64 bit floating point
            U_Union64 u_Value64;
            u_Value64.u64_Value = mh_BinaryToUnsignedInt64Big(&orc_Data[0]);
            this->SetValueF64(u_Value64.f64_Value);
            break;
         default:
            break;
         }
      }
      else
      {
         //array type
         //we set up the array here and assign in one go to ensure data consistency
         uint32_t u32_ArrayIndex;
         std::vector<uint16_t> c_Data16;
         std::vector<uint32_t> c_Data32;
         std::vector<uint64_t> c_Data64;
         std::vector<int8_t> c_DataS8;
         std::vector<int16_t> c_DataS16;
         std::vector<int32_t> c_DataS32;
         std::vector<int64_t> c_DataS64;
         std::vector<float32_t> c_DataF32;
         std::vector<float64_t> c_DataF64;
         switch (this->me_Type)
         {
         case eUINT8:
            this->SetValueArrU8(orc_Data);
            break;
         case eUINT16:
            c_Data16.resize(orc_Data.size() / 2);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_Data16[u32_ArrayIndex] =
                  static_cast<uint16_t>((static_cast<uint16_t>(orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U]) <<
                                         8U)) +
                  orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) + 1U];
            }
            this->SetValueArrU16(c_Data16);
            break;
         case eUINT32: ///< Data type unsigned 32 bit integer
            c_Data32.resize(orc_Data.size() / 4);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_Data32[u32_ArrayIndex] = mh_BinaryToUnsignedInt32Big(
                  &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
            }
            this->SetValueArrU32(c_Data32);
            break;
         case eUINT64: ///< Data type unsigned 64 bit integer
            c_Data64.resize(orc_Data.size() / 8);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_Data64[u32_ArrayIndex] = mh_BinaryToUnsignedInt64Big(
                  &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
            }
            this->SetValueArrU64(c_Data64);
            break;
         case eSINT8: ///< Data type signed 8 bit integer
            c_DataS8.resize(orc_Data.size());
            (void)std::memcpy(&c_DataS8[0], &orc_Data[0], orc_Data.size());
            this->SetValueArrS8(c_DataS8);
            break;
         case eSINT16: ///< Data type signed 16 bit integer
            c_DataS16.resize(orc_Data.size() / 2);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_DataS16[u32_ArrayIndex] =
                  static_cast<int16_t>(
                     static_cast<uint16_t>((static_cast<uint16_t>(orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U]) <<
                                            8U)) +
                     orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) + 1U]);
            }
            this->SetValueArrS16(c_DataS16);
            break;
         case eSINT32: ///< Data type unsigned 32 bit integer
            c_DataS32.resize(orc_Data.size() / 4);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_DataS32[u32_ArrayIndex] =
                  static_cast<int32_t>(mh_BinaryToUnsignedInt32Big(&orc_Data[static_cast<size_t>(u32_ArrayIndex) *
                                                                             4U]));
            }
            this->SetValueArrS32(c_DataS32);
            break;
         case eSINT64: ///< Data type unsigned 64 bit integer
            c_DataS64.resize(orc_Data.size() / 8);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_DataS64[u32_ArrayIndex] =
                  static_cast<int64_t>(mh_BinaryToUnsignedInt64Big(&orc_Data[static_cast<size_t>(u32_ArrayIndex) *
                                                                             8U]));
            }
            this->SetValueArrS64(c_DataS64);
            break;
         case eFLOAT32: ///< Data type 32 bit floating point
            U_Union32 u_Value32;
            c_DataF32.resize(orc_Data.size() / 4);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               u_Value32.u32_Value = mh_BinaryToUnsignedInt32Big(&orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
               c_DataF32[u32_ArrayIndex] = u_Value32.f32_Value;
            }
            this->SetValueArrF32(c_DataF32);
            break;
         case eFLOAT64: ///< Data type 64 bit floating point
            U_Union64 u_Value64;
            c_DataF64.resize(orc_Data.size() / 8);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               u_Value64.u64_Value = mh_BinaryToUnsignedInt64Big(&orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
               c_DataF64[u32_ArrayIndex] = u_Value64.f64_Value;
            }
            this->SetValueArrF64(c_DataF64);
            break;
         default:
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value from little endian BLOB

   Convert BLOB to values.
   Considers configured element type.

   \param[in]  orc_Data    data to set

   \return
   C_NO_ERR   value set
   C_CONFIG   size of orc_Data does not match our size
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolContent::SetValueFromLittleEndianBlob(const std::vector<uint8_t> & orc_Data)
{
   int32_t s32_Return = C_NO_ERR;

   //using unions is not nice but more portable than reinterpret_casting
   //lint -e{9018}  //cf. comment above
   union U_Union32
   {
      uint32_t u32_Value;
      float32_t f32_Value;
   };

   //lint -e{9018}  //cf. comment above
   union U_Union64
   {
      uint64_t u64_Value;
      float64_t f64_Value;
   };

   //is size correct ?
   if (this->GetSizeByte() != orc_Data.size())
   {
      s32_Return = C_CONFIG;
   }
   else
   {
      if (this->GetArray() == false)
      {
         //plain type
         switch (this->me_Type)
         {
         case eUINT8: ///< Data type unsigned 8 bit integer
            this->SetValueU8(orc_Data[0]);
            break;
         case eUINT16: ///< Data type unsigned 16 bit integer
            this->SetValueU16(static_cast<uint16_t>((static_cast<uint16_t>(orc_Data[1]) << 8U)) + orc_Data[0]);
            break;
         case eUINT32: ///< Data type unsigned 32 bit integer
            this->SetValueU32(mh_BinaryToUnsignedInt32Little(&orc_Data[0]));
            break;
         case eUINT64: ///< Data type unsigned 64 bit integer
            this->SetValueU64(mh_BinaryToUnsignedInt64Little(&orc_Data[0]));
            break;
         case eSINT8: ///< Data type signed 8 bit integer
            this->SetValueS8(orc_Data[0]);
            break;
         case eSINT16: ///< Data type signed 16 bit integer
            this->SetValueS16(static_cast<int16_t>(static_cast<uint16_t>((static_cast<uint16_t>(orc_Data[1]) << 8U)) +
                                                   orc_Data[0]));
            break;
         case eSINT32: ///< Data type signed 32 bit integer
            this->SetValueS32(static_cast<int32_t>(mh_BinaryToUnsignedInt32Little(&orc_Data[0])));
            break;
         case eSINT64: ///< Data type signed 64 bit integer
            this->SetValueS64(static_cast<int64_t>(mh_BinaryToUnsignedInt64Little(&orc_Data[0])));
            break;
         case eFLOAT32: ///< Data type 32 bit floating point
            U_Union32 u_Value32;
            u_Value32.u32_Value = mh_BinaryToUnsignedInt32Little(&orc_Data[0]);
            this->SetValueF32(u_Value32.f32_Value);
            break;
         case eFLOAT64: ///< Data type 64 bit floating point
            U_Union64 u_Value64;
            u_Value64.u64_Value = mh_BinaryToUnsignedInt64Little(&orc_Data[0]);
            this->SetValueF64(u_Value64.f64_Value);
            break;
         default:
            break;
         }
      }
      else
      {
         //array type
         //we set up the array here and assign in one go to ensure data consistency
         uint32_t u32_ArrayIndex;
         std::vector<uint16_t> c_Data16;
         std::vector<uint32_t> c_Data32;
         std::vector<uint64_t> c_Data64;
         std::vector<int8_t> c_DataS8;
         std::vector<int16_t> c_DataS16;
         std::vector<int32_t> c_DataS32;
         std::vector<int64_t> c_DataS64;
         std::vector<float32_t> c_DataF32;
         std::vector<float64_t> c_DataF64;
         switch (this->me_Type)
         {
         case eUINT8:
            this->SetValueArrU8(orc_Data);
            break;
         case eUINT16:
            c_Data16.resize(orc_Data.size() / 2);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_Data16[u32_ArrayIndex] =
                  static_cast<uint16_t>(static_cast<uint16_t>(orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) +
                                                                       1U]) << 8U) +
                  orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U];
            }
            this->SetValueArrU16(c_Data16);
            break;
         case eUINT32: ///< Data type unsigned 32 bit integer
            c_Data32.resize(orc_Data.size() / 4);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_Data32[u32_ArrayIndex] =
                  mh_BinaryToUnsignedInt32Little(&orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
            }
            this->SetValueArrU32(c_Data32);
            break;
         case eUINT64: ///< Data type unsigned 64 bit integer
            c_Data64.resize(orc_Data.size() / 8);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_Data64[u32_ArrayIndex] =
                  mh_BinaryToUnsignedInt64Little(&orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
            }
            this->SetValueArrU64(c_Data64);
            break;
         case eSINT8: ///< Data type signed 8 bit integer
            c_DataS8.resize(orc_Data.size());
            (void)std::memcpy(&c_DataS8[0], &orc_Data[0], orc_Data.size());
            this->SetValueArrS8(c_DataS8);
            break;
         case eSINT16: ///< Data type signed 16 bit integer
            c_DataS16.resize(orc_Data.size() / 2);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_DataS16[u32_ArrayIndex] =
                  static_cast<int16_t>(
                     static_cast<uint16_t>(static_cast<uint16_t>(orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) +
                                                                          1U]) <<
                                           8U) +
                     orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U]);
            }
            this->SetValueArrS16(c_DataS16);
            break;
         case eSINT32: ///< Data type unsigned 32 bit integer
            c_DataS32.resize(orc_Data.size() / 4);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_DataS32[u32_ArrayIndex] =
                  static_cast<int32_t>(mh_BinaryToUnsignedInt32Little(&orc_Data[static_cast<size_t>(u32_ArrayIndex) *
                                                                                4U]));
            }
            this->SetValueArrS32(c_DataS32);
            break;
         case eSINT64: ///< Data type unsigned 64 bit integer
            c_DataS64.resize(orc_Data.size() / 8);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               c_DataS64[u32_ArrayIndex] =
                  static_cast<int64_t>(mh_BinaryToUnsignedInt64Little(&orc_Data[static_cast<size_t>(u32_ArrayIndex) *
                                                                                8U]));
            }
            this->SetValueArrS64(c_DataS64);
            break;
         case eFLOAT32: ///< Data type 32 bit floating point
            U_Union32 u_Value32;
            c_DataF32.resize(orc_Data.size() / 4);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               u_Value32.u32_Value =
                  mh_BinaryToUnsignedInt32Little(&orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
               c_DataF32[u32_ArrayIndex] = u_Value32.f32_Value;
            }
            this->SetValueArrF32(c_DataF32);
            break;
         case eFLOAT64: ///< Data type 64 bit floating point
            U_Union64 u_Value64;
            c_DataF64.resize(orc_Data.size() / 8);
            for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
            {
               u_Value64.u64_Value =
                  mh_BinaryToUnsignedInt64Little(&orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
               c_DataF64[u32_ArrayIndex] = u_Value64.f64_Value;
            }
            this->SetValueArrF64(c_DataF64);
            break;
         default:
            break;
         }
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get values as big endian BLOB

   Convert content value to BLOB.
   Considers configured element type.

   \param[out]  orc_Data   data to set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::GetValueAsBigEndianBlob(std::vector<uint8_t> & orc_Data) const
{
   //using unions is not nice but more portable than reinterpret_casting
   //lint -e{9018}  //cf. comment above
   union U_Union32
   {
      uint32_t u32_Value;
      float32_t f32_Value;
   };

   //lint -e{9018}  //cf. comment above
   union U_Union64
   {
      uint64_t u64_Value;
      float64_t f64_Value;
   };

   //set size ...
   orc_Data.resize(this->GetSizeByte());

   if (this->GetArray() == false)
   {
      uint16_t u16_Value;
      U_Union32 u_Value32;
      U_Union64 u_Value64;
      //plain type
      switch (this->me_Type)
      {
      case eUINT8: ///< Data type unsigned 8 bit integer
         orc_Data[0] = this->GetValueU8();
         break;
      case eUINT16: ///< Data type unsigned 16 bit integer
         u16_Value = this->GetValueU16();
         orc_Data[0] = static_cast<uint8_t>(u16_Value >> 8U);
         orc_Data[1] = static_cast<uint8_t>(u16_Value);
         break;
      case eUINT32: ///< Data type unsigned 32 bit integer
         mh_UnsignedInt32ToBinaryBig(this->GetValueU32(), &orc_Data[0]);
         break;
      case eUINT64: ///< Data type unsigned 64 bit integer
         mh_UnsignedInt64ToBinaryBig(this->GetValueU64(), &orc_Data[0]);
         break;
      case eSINT8: ///< Data type signed 8 bit integer
         orc_Data[0] = this->GetValueS8();
         break;
      case eSINT16: ///< Data type signed 16 bit integer
         u16_Value = static_cast<uint16_t>(this->GetValueS16());
         orc_Data[0] = static_cast<uint8_t>(u16_Value >> 8U);
         orc_Data[1] = static_cast<uint8_t>(u16_Value);
         break;
      case eSINT32: ///< Data type signed 32 bit integer
         mh_UnsignedInt32ToBinaryBig(static_cast<uint32_t>(this->GetValueS32()), &orc_Data[0]);
         break;
      case eSINT64: ///< Data type signed 64 bit integer
         mh_UnsignedInt64ToBinaryBig(static_cast<uint64_t>(this->GetValueS64()), &orc_Data[0]);
         break;
      case eFLOAT32: ///< Data type 32 bit floating point
         u_Value32.f32_Value = this->GetValueF32();
         mh_UnsignedInt32ToBinaryBig(u_Value32.u32_Value, &orc_Data[0]);
         break;
      case eFLOAT64: ///< Data type 64 bit floating point
         u_Value64.f64_Value = this->GetValueF64();
         mh_UnsignedInt64ToBinaryBig(u_Value64.u64_Value, &orc_Data[0]);
         break;
      default:
         break;
      }
   }
   else
   {
      //array type
      //we set up the array here and assign in one go to ensure data consistency
      uint32_t u32_ArrayIndex;
      std::vector<uint16_t> c_Data16;
      std::vector<uint32_t> c_Data32;
      std::vector<uint64_t> c_Data64;
      std::vector<int8_t> c_DataS8;
      std::vector<int16_t> c_DataS16;
      std::vector<int32_t> c_DataS32;
      std::vector<int64_t> c_DataS64;
      std::vector<float32_t> c_DataF32;
      std::vector<float64_t> c_DataF64;
      switch (this->me_Type)
      {
      case eUINT8:
         orc_Data = this->GetValueArrU8();
         break;
      case eUINT16:
         c_Data16 = this->GetValueArrU16();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U] = static_cast<uint8_t>(c_Data16[u32_ArrayIndex] >> 8U);
            orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) + 1] = static_cast<uint8_t>(c_Data16[u32_ArrayIndex]);
         }
         break;
      case eUINT32: ///< Data type unsigned 32 bit integer
         c_Data32 = this->GetValueArrU32();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt32ToBinaryBig(c_Data32[u32_ArrayIndex], &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
         }
         break;
      case eUINT64: ///< Data type unsigned 64 bit integer
         c_Data64 = this->GetValueArrU64();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt64ToBinaryBig(c_Data64[u32_ArrayIndex], &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
         }
         break;
      case eSINT8: ///< Data type signed 8 bit integer
         c_DataS8 = this->GetValueArrS8();
         (void)std::memcpy(&orc_Data[0], &c_DataS8[0], orc_Data.size());
         break;
      case eSINT16: ///< Data type signed 16 bit integer
         c_DataS16 = this->GetValueArrS16();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U] =
               static_cast<uint8_t>(static_cast<uint16_t>(c_DataS16[u32_ArrayIndex]) >> 8U);
            orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) + 1] =
               static_cast<uint8_t>(static_cast<uint16_t>(c_DataS16[u32_ArrayIndex]));
         }
         break;
      case eSINT32: ///< Data type unsigned 32 bit integer
         c_DataS32 = this->GetValueArrS32();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt32ToBinaryBig(static_cast<uint32_t>(c_DataS32[u32_ArrayIndex]),
                                        &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
         }
         break;
      case eSINT64: ///< Data type unsigned 64 bit integer
         c_DataS64 = this->GetValueArrS64();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt64ToBinaryBig(static_cast<uint64_t>(c_DataS64[u32_ArrayIndex]),
                                        &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
         }
         break;
      case eFLOAT32: ///< Data type 32 bit floating point
         U_Union32 u_Value32;
         c_DataF32 = this->GetValueArrF32();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            u_Value32.f32_Value = c_DataF32[u32_ArrayIndex];
            mh_UnsignedInt32ToBinaryBig(u_Value32.u32_Value, &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
         }
         break;
      case eFLOAT64: ///< Data type 64 bit floating point
         U_Union64 u_Value64;
         c_DataF64 = this->GetValueArrF64();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            u_Value64.f64_Value = c_DataF64[u32_ArrayIndex];
            mh_UnsignedInt64ToBinaryBig(u_Value64.u64_Value, &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
         }
         break;
      default:
         break;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: Get uint64 from binary array in big endian order

   \param[in]  opu8_Data   resulting data (caller is responsible to pass 8 bytes buffer)

   \return  converted data
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_OscNodeDataPoolContent::mh_BinaryToUnsignedInt64Big(const uint8_t * const opu8_Data)
{
   return (static_cast<uint64_t>(opu8_Data[0]) << 56U) +
          (static_cast<uint64_t>(opu8_Data[1]) << 48U) +
          (static_cast<uint64_t>(opu8_Data[2]) << 40U) +
          (static_cast<uint64_t>(opu8_Data[3]) << 32U) +
          (static_cast<uint64_t>(opu8_Data[4]) << 24U) +
          (static_cast<uint64_t>(opu8_Data[5]) << 16U) +
          (static_cast<uint64_t>(opu8_Data[6]) << 8U) +
          opu8_Data[7];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: Get uint32 from binary array in big endian order

   \param[in]  opu8_Data   resulting data (caller is responsible to pass 4 bytes buffer)

   \return  converted data
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolContent::mh_BinaryToUnsignedInt32Big(const uint8_t * const opu8_Data)
{
   return (static_cast<uint32_t>(opu8_Data[0]) << 24U) +
          (static_cast<uint32_t>(opu8_Data[1]) << 16U) +
          (static_cast<uint32_t>(opu8_Data[2]) << 8U) +
          opu8_Data[3];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: Get uint64 from binary array in little endian order

   \param[in]  opu8_Data   resulting data (caller is responsible to pass 8 bytes buffer)

   \return  converted data
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t C_OscNodeDataPoolContent::mh_BinaryToUnsignedInt64Little(const uint8_t * const opu8_Data)
{
   return (static_cast<uint64_t>(opu8_Data[7]) << 56U) +
          (static_cast<uint64_t>(opu8_Data[6]) << 48U) +
          (static_cast<uint64_t>(opu8_Data[5]) << 40U) +
          (static_cast<uint64_t>(opu8_Data[4]) << 32U) +
          (static_cast<uint64_t>(opu8_Data[3]) << 24U) +
          (static_cast<uint64_t>(opu8_Data[2]) << 16U) +
          (static_cast<uint64_t>(opu8_Data[1]) << 8U) +
          opu8_Data[0];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: Get uint64 from binary array in little endian order

   \param[in]  opu8_Data   resulting data (caller is responsible to pass 4 bytes buffer)

   \return  converted data
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolContent::mh_BinaryToUnsignedInt32Little(const uint8_t * const opu8_Data)
{
   return (static_cast<uint32_t>(opu8_Data[3]) << 24U) +
          (static_cast<uint32_t>(opu8_Data[2]) << 16U) +
          (static_cast<uint32_t>(opu8_Data[1]) << 8U) +
          opu8_Data[0];
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: fill uint64 value into binary data

   Copy uint64 value to byte array in big endian order

   \param[in]   ou64_Data  data to fill in
   \param[out]  opu8_Data  resulting data (caller is responsible to pass 8 bytes buffer)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::mh_UnsignedInt64ToBinaryBig(const uint64_t ou64_Data, uint8_t * const opu8_Data)
{
   opu8_Data[0] = static_cast<uint8_t>(ou64_Data >> 56U);
   opu8_Data[1] = static_cast<uint8_t>(ou64_Data >> 48U);
   opu8_Data[2] = static_cast<uint8_t>(ou64_Data >> 40U);
   opu8_Data[3] = static_cast<uint8_t>(ou64_Data >> 32U);
   opu8_Data[4] = static_cast<uint8_t>(ou64_Data >> 24U);
   opu8_Data[5] = static_cast<uint8_t>(ou64_Data >> 16U);
   opu8_Data[6] = static_cast<uint8_t>(ou64_Data >> 8U);
   opu8_Data[7] = static_cast<uint8_t>(ou64_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: fill uint32 value into binary data

   Copy uint32 value to byte array in big endian order

   \param[in]   ou32_Data  data to fill in
   \param[out]  opu8_Data  resulting data (caller is responsible to pass 4 bytes buffer)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::mh_UnsignedInt32ToBinaryBig(const uint32_t ou32_Data, uint8_t * const opu8_Data)
{
   opu8_Data[0] = static_cast<uint8_t>(ou32_Data >> 24U);
   opu8_Data[1] = static_cast<uint8_t>(ou32_Data >> 16U);
   opu8_Data[2] = static_cast<uint8_t>(ou32_Data >> 8U);
   opu8_Data[3] = static_cast<uint8_t>(ou32_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: fill uint64 value into binary data

   Copy uint64 value to byte array in little endian order

   \param[in]   ou64_Data  data to fill in
   \param[out]  opu8_Data  resulting data (caller is responsible to pass 8 bytes buffer)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::mh_UnsignedInt64ToBinaryLittle(const uint64_t ou64_Data, uint8_t * const opu8_Data)
{
   opu8_Data[7] = static_cast<uint8_t>(ou64_Data >> 56U);
   opu8_Data[6] = static_cast<uint8_t>(ou64_Data >> 48U);
   opu8_Data[5] = static_cast<uint8_t>(ou64_Data >> 40U);
   opu8_Data[4] = static_cast<uint8_t>(ou64_Data >> 32U);
   opu8_Data[3] = static_cast<uint8_t>(ou64_Data >> 24U);
   opu8_Data[2] = static_cast<uint8_t>(ou64_Data >> 16U);
   opu8_Data[1] = static_cast<uint8_t>(ou64_Data >> 8U);
   opu8_Data[0] = static_cast<uint8_t>(ou64_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: fill uint32 value into binary data

   Copy uint32 value to byte array in little endian order

   \param[in]   ou32_Data  data to fill in
   \param[out]  opu8_Data  resulting data (caller is responsible to pass 4 bytes buffer)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::mh_UnsignedInt32ToBinaryLittle(const uint32_t ou32_Data, uint8_t * const opu8_Data)
{
   opu8_Data[3] = static_cast<uint8_t>(ou32_Data >> 24U);
   opu8_Data[2] = static_cast<uint8_t>(ou32_Data >> 16U);
   opu8_Data[1] = static_cast<uint8_t>(ou32_Data >> 8U);
   opu8_Data[0] = static_cast<uint8_t>(ou32_Data);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get values as little endian BLOB

   Convert content value to BLOB.
   Considers configured element type.

   \param[out]  orc_Data   data to set
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolContent::GetValueAsLittleEndianBlob(std::vector<uint8_t> & orc_Data) const
{
   //using unions is not nice but more portable than reinterpret_casting
   //lint -e{9018}  //cf. comment above
   union U_Union32
   {
      uint32_t u32_Value;
      float32_t f32_Value;
   };

   //lint -e{9018}  //cf. comment above
   union U_Union64
   {
      uint64_t u64_Value;
      float64_t f64_Value;
   };

   //set size ...
   orc_Data.resize(this->GetSizeByte());

   if (this->GetArray() == false)
   {
      uint16_t u16_Value;
      U_Union32 u_Value32;
      U_Union64 u_Value64;
      //plain type
      switch (this->me_Type)
      {
      case eUINT8: ///< Data type unsigned 8 bit integer
         orc_Data[0] = this->GetValueU8();
         break;
      case eUINT16: ///< Data type unsigned 16 bit integer
         u16_Value = this->GetValueU16();
         orc_Data[1] = static_cast<uint8_t>(u16_Value >> 8U);
         orc_Data[0] = static_cast<uint8_t>(u16_Value);
         break;
      case eUINT32: ///< Data type unsigned 32 bit integer
         mh_UnsignedInt32ToBinaryLittle(this->GetValueU32(), &orc_Data[0]);
         break;
      case eUINT64: ///< Data type unsigned 64 bit integer
         mh_UnsignedInt64ToBinaryLittle(this->GetValueU64(), &orc_Data[0]);
         break;
      case eSINT8: ///< Data type signed 8 bit integer
         orc_Data[0] = this->GetValueS8();
         break;
      case eSINT16: ///< Data type signed 16 bit integer
         u16_Value = static_cast<uint16_t>(this->GetValueS16());
         orc_Data[1] = static_cast<uint8_t>(u16_Value >> 8U);
         orc_Data[0] = static_cast<uint8_t>(u16_Value);
         break;
      case eSINT32: ///< Data type signed 32 bit integer
         mh_UnsignedInt32ToBinaryLittle(static_cast<uint32_t>(this->GetValueS32()), &orc_Data[0]);
         break;
      case eSINT64: ///< Data type signed 64 bit integer
         mh_UnsignedInt64ToBinaryLittle(static_cast<uint64_t>(this->GetValueS64()), &orc_Data[0]);
         break;
      case eFLOAT32: ///< Data type 32 bit floating point
         u_Value32.f32_Value = this->GetValueF32();
         mh_UnsignedInt32ToBinaryLittle(u_Value32.u32_Value, &orc_Data[0]);
         break;
      case eFLOAT64: ///< Data type 64 bit floating point
         u_Value64.f64_Value = this->GetValueF64();
         mh_UnsignedInt64ToBinaryLittle(u_Value64.u64_Value, &orc_Data[0]);
         break;
      default:
         break;
      }
   }
   else
   {
      //array type
      //we set up the array here and assign in one go to ensure data consistency
      uint32_t u32_ArrayIndex;
      std::vector<uint16_t> c_Data16;
      std::vector<uint32_t> c_Data32;
      std::vector<uint64_t> c_Data64;
      std::vector<int8_t> c_DataS8;
      std::vector<int16_t> c_DataS16;
      std::vector<int32_t> c_DataS32;
      std::vector<int64_t> c_DataS64;
      std::vector<float32_t> c_DataF32;
      std::vector<float64_t> c_DataF64;
      switch (this->me_Type)
      {
      case eUINT8:
         orc_Data = this->GetValueArrU8();
         break;
      case eUINT16:
         c_Data16 = this->GetValueArrU16();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) +
                     1] = static_cast<uint8_t>(c_Data16[u32_ArrayIndex] >> 8U);
            orc_Data[static_cast<size_t>(u32_ArrayIndex) * 2U] = static_cast<uint8_t>(c_Data16[u32_ArrayIndex]);
         }
         break;
      case eUINT32: ///< Data type unsigned 32 bit integer
         c_Data32 = this->GetValueArrU32();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt32ToBinaryLittle(c_Data32[u32_ArrayIndex],
                                           &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
         }
         break;
      case eUINT64: ///< Data type unsigned 64 bit integer
         c_Data64 = this->GetValueArrU64();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt64ToBinaryLittle(c_Data64[u32_ArrayIndex],
                                           &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
         }
         break;
      case eSINT8: ///< Data type signed 8 bit integer
         c_DataS8 = this->GetValueArrS8();
         (void)std::memcpy(&orc_Data[0], &c_DataS8[0], orc_Data.size());
         break;
      case eSINT16: ///< Data type signed 16 bit integer
         c_DataS16 = this->GetValueArrS16();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            orc_Data[(static_cast<size_t>(u32_ArrayIndex) * 2U) +
                     1] = static_cast<uint8_t>(static_cast<uint16_t>(c_DataS16[u32_ArrayIndex]) >> 8U);
            orc_Data[static_cast<size_t>(u32_ArrayIndex) *
                     2U] = static_cast<uint8_t>(static_cast<uint16_t>(c_DataS16[u32_ArrayIndex]));
         }
         break;
      case eSINT32: ///< Data type signed 32 bit integer
         c_DataS32 = this->GetValueArrS32();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt32ToBinaryLittle(static_cast<uint32_t>(c_DataS32[u32_ArrayIndex]),
                                           &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
         }
         break;
      case eSINT64: ///< Data type signed 64 bit integer
         c_DataS64 = this->GetValueArrS64();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            mh_UnsignedInt64ToBinaryLittle(static_cast<uint64_t>(c_DataS64[u32_ArrayIndex]),
                                           &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
         }
         break;
      case eFLOAT32: ///< Data type 32 bit floating point
         U_Union32 u_Value32;
         c_DataF32 = this->GetValueArrF32();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            u_Value32.f32_Value = c_DataF32[u32_ArrayIndex];
            mh_UnsignedInt32ToBinaryLittle(u_Value32.u32_Value, &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 4U]);
         }
         break;
      case eFLOAT64: ///< Data type 64 bit floating point
         U_Union64 u_Value64;
         c_DataF64 = this->GetValueArrF64();
         for (u32_ArrayIndex = 0U; u32_ArrayIndex < this->GetArraySize(); u32_ArrayIndex++)
         {
            u_Value64.f64_Value = c_DataF64[u32_ArrayIndex];
            mh_UnsignedInt64ToBinaryLittle(u_Value64.u64_Value, &orc_Data[static_cast<size_t>(u32_ArrayIndex) * 8U]);
         }
         break;
      default:
         break;
      }
   }
}
