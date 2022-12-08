//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for an element in a list in a data pool (implementation)

   Data class for an element in a list in a data pool

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "C_OscNodeDataPoolListElement.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
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
C_OscNodeDataPoolListElement::C_OscNodeDataPoolListElement(void) :
   c_Name("NewDataElement"),
   c_Comment(""),
   c_MinValue(),
   c_MaxValue(),
   f64_Factor(1.0),
   f64_Offset(0.0),
   c_Unit(""),
   e_Access(eACCESS_RW),
   q_DiagEventCall(false),
   c_DataSetValues(),
   c_Value(),
   c_NvmValue(),
   q_NvmValueChanged(false),
   q_NvmValueIsValid(false),
   u32_NvmStartAddress(0)
{
   this->SetType(C_OscNodeDataPoolContent::eUINT8);
   this->SetArray(false);
   this->c_MinValue.SetValueU8(0U);
   this->c_MaxValue.SetValueU8(255U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElement::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   this->c_MinValue.CalcHash(oru32_HashValue);
   this->c_MaxValue.CalcHash(oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Factor, sizeof(this->f64_Factor), oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Offset, sizeof(this->f64_Offset), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Unit.c_str(), this->c_Unit.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Access, sizeof(this->e_Access), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_DiagEventCall, sizeof(this->q_DiagEventCall), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_DataSetValues.size(); ++u32_Counter)
   {
      this->c_DataSetValues[u32_Counter].CalcHash(oru32_HashValue);
   }
   // Do not calculate the value to the CRC
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmStartAddress, sizeof(this->u32_NvmStartAddress), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over one element

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
   \param[in]      ou32_Index       Index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElement::CalcHashElement(uint32_t & oru32_HashValue, const uint32_t ou32_Index) const
{
   uint32_t u32_Counter;

   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   this->c_MinValue.CalcHashElement(oru32_HashValue, ou32_Index);
   this->c_MaxValue.CalcHashElement(oru32_HashValue, ou32_Index);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Factor, sizeof(this->f64_Factor), oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Offset, sizeof(this->f64_Offset), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Unit.c_str(), this->c_Unit.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Access, sizeof(this->e_Access), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_DiagEventCall, sizeof(this->q_DiagEventCall), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_DataSetValues.size(); ++u32_Counter)
   {
      this->c_DataSetValues[u32_Counter].CalcHashElement(oru32_HashValue, ou32_Index);
   }
   // Do not calculate the value to the CRC
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmStartAddress, sizeof(this->u32_NvmStartAddress), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calculates the hash value over structure only

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElement::CalcHashStructure(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   this->c_MinValue.CalcHashStructure(oru32_HashValue);
   this->c_MaxValue.CalcHashStructure(oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Factor, sizeof(this->f64_Factor), oru32_HashValue);
   //lint -e{9110} //we do not really use the bit representation; we just assume it is "stable" for this type
   stw::scl::C_SclChecksums::CalcCRC32(&this->f64_Offset, sizeof(this->f64_Offset), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Unit.c_str(), this->c_Unit.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Access, sizeof(this->e_Access), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_DiagEventCall, sizeof(this->q_DiagEventCall), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_DataSetValues.size(); ++u32_Counter)
   {
      this->c_DataSetValues[u32_Counter].CalcHashStructure(oru32_HashValue);
   }
   // Do not calculate the value to the CRC
   stw::scl::C_SclChecksums::CalcCRC32(&this->u32_NvmStartAddress, sizeof(this->u32_NvmStartAddress), oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get size in bytes

   \return
   size of element in bytes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolListElement::GetSizeByte(void) const
{
   return this->c_Value.GetSizeByte();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change internal type

   Change type of all variables which are of variable type

   \param[in]  ore_Value   New type value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElement::SetType(const C_OscNodeDataPoolContent::E_Type & ore_Value)
{
   c_MinValue.SetType(ore_Value);
   c_MaxValue.SetType(ore_Value);
   c_Value.SetType(ore_Value);
   c_NvmValue.SetType(ore_Value);
   for (uint32_t u32_It = 0; u32_It < c_DataSetValues.size(); ++u32_It)
   {
      c_DataSetValues[u32_It].SetType(ore_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change array status

   \param[in]  oq_Array    New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElement::SetArray(const bool oq_Array)
{
   c_MinValue.SetArray(oq_Array);
   c_MaxValue.SetArray(oq_Array);
   c_Value.SetArray(oq_Array);
   c_NvmValue.SetArray(oq_Array);
   for (uint32_t u32_It = 0; u32_It < c_DataSetValues.size(); ++u32_It)
   {
      c_DataSetValues[u32_It].SetArray(oq_Array);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Resize current array

   \param[in]  oru32_Size  New size value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeDataPoolListElement::SetArraySize(const uint32_t & oru32_Size)
{
   c_MinValue.SetArraySize(oru32_Size);
   c_MaxValue.SetArraySize(oru32_Size);
   c_Value.SetArraySize(oru32_Size);
   c_NvmValue.SetArraySize(oru32_Size);
   for (uint32_t u32_It = 0; u32_It < c_DataSetValues.size(); ++u32_It)
   {
      c_DataSetValues[u32_It].SetArraySize(oru32_Size);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get internal type

   \return
   Internal type
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent::E_Type C_OscNodeDataPoolListElement::GetType(void) const
{
   return this->c_Value.GetType();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array status

   \return
   Array status
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscNodeDataPoolListElement::GetArray(void) const
{
   return this->c_Value.GetArray();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current array size

   \return
   Current array size
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscNodeDataPoolListElement::GetArraySize(void) const
{
   return this->c_Value.GetArraySize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the current value for its range

   The function checks c_Value against the minimum (c_MinValue) and maximum (c_MaxValue)

   \return
   C_NO_ERR   Current value is valid
   C_RANGE    Current value is invalid
   C_CONFIG   Wrong types are set in minimum or maximum
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolListElement::CheckValueRange(void) const
{
   int32_t s32_Return = C_RANGE;

   try
   {
      if ((this->c_Value.GetArray() != this->c_MaxValue.GetArray()) ||
          (this->c_Value.GetArray() != this->c_MinValue.GetArray()) ||
          (this->c_Value.GetType() != this->c_MaxValue.GetType()) ||
          (this->c_Value.GetType() != this->c_MinValue.GetType()))
      {
         s32_Return = C_CONFIG;
      }
      else
      {
         const bool q_Valid = (((this->c_Value >= this->c_MinValue) &&
                                (this->c_Value <= this->c_MaxValue)) ? true : false);

         if (q_Valid == true)
         {
            s32_Return = C_NO_ERR;
         }
      }
   }
   catch (...)
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks the current NVM value for its range

   The function checks c_NvmValue against the minimum (c_MinValue) and maximum (c_MaxValue)

   \return
   C_NO_ERR   Current value is valid
   C_RANGE    Current value is invalid
   C_CONFIG   Wrong types are set in minimum or maximum
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeDataPoolListElement::CheckNvmValueRange(void) const
{
   int32_t s32_Return = C_RANGE;

   try
   {
      if ((this->c_NvmValue.GetArray() != this->c_MaxValue.GetArray()) ||
          (this->c_NvmValue.GetArray() != this->c_MinValue.GetArray()) ||
          (this->c_NvmValue.GetType() != this->c_MaxValue.GetType()) ||
          (this->c_NvmValue.GetType() != this->c_MinValue.GetType()))
      {
         s32_Return = C_CONFIG;
      }
      else
      {
         const bool q_Valid = (((this->c_NvmValue >= this->c_MinValue) &&
                                (this->c_NvmValue <= this->c_MaxValue)) ? true : false);

         if (q_Valid == true)
         {
            s32_Return = C_NO_ERR;
         }
      }
   }
   catch (...)
   {
      s32_Return = C_CONFIG;
   }

   return s32_Return;
}
