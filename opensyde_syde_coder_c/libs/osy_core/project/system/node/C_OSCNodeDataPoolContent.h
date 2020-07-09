//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for any supported data type (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLCONTENT_H
#define C_OSCNODEDATAPOOLCONTENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <string>
#include "stwtypes.h"

#include "TGLTasks.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class for any supported data pool data type
class C_OSCNodeDataPoolContent
{
public:
   C_OSCNodeDataPoolContent(void);
   C_OSCNodeDataPoolContent(const C_OSCNodeDataPoolContent & orc_Source);
   C_OSCNodeDataPoolContent & operator = (const C_OSCNodeDataPoolContent & orc_Source);
   virtual ~C_OSCNodeDataPoolContent(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   enum E_Type ///< Data type
   {
      eUINT8,   ///< Data type unsigned 8 bit integer
      eUINT16,  ///< Data type unsigned 16 bit integer
      eUINT32,  ///< Data type unsigned 32 bit integer
      eUINT64,  ///< Data type unsigned 64 bit integer
      eSINT8,   ///< Data type signed 8 bit integer
      eSINT16,  ///< Data type signed 16 bit integer
      eSINT32,  ///< Data type signed 32 bit integer
      eSINT64,  ///< Data type signed 64 bit integer
      eFLOAT32, ///< Data type 32 bit floating point
      eFLOAT64  ///< Data type 64 bit floating point
   };

   void SetValueU8(const stw_types::uint8 ou8_Value);
   stw_types::uint8 GetValueU8(void) const;
   void SetValueU16(const stw_types::uint16 ou16_Value);
   stw_types::uint16 GetValueU16(void) const;
   void SetValueU32(const stw_types::uint32 ou32_Value);
   stw_types::uint32 GetValueU32(void) const;
   void SetValueU64(const stw_types::uint64 ou64_Value);
   stw_types::uint64 GetValueU64(void) const;
   void SetValueS8(const stw_types::sint8 os8_Value);
   stw_types::sint8 GetValueS8(void) const;
   void SetValueS16(const stw_types::sint16 os16_Value);
   stw_types::sint16 GetValueS16(void) const;
   void SetValueS32(const stw_types::sint32 os32_Value);
   stw_types::sint32 GetValueS32(void) const;
   void SetValueS64(const stw_types::sint64 os64_Value);
   stw_types::sint64 GetValueS64(void) const;
   void SetValueF32(const stw_types::float32 of32_Value);
   stw_types::float32 GetValueF32(void) const;
   void SetValueF64(const stw_types::float64 of64_Value);
   stw_types::float64 GetValueF64(void) const;

   void SetValueAU8(const std::vector<stw_types::uint8> & orc_Value);
   void SetValueAU8Element(const stw_types::uint8 ou8_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::uint8> GetValueAU8(void) const;
   stw_types::uint8 GetValueAU8Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAU16(const std::vector<stw_types::uint16> & orc_Value);
   void SetValueAU16Element(const stw_types::uint16 ou16_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::uint16> GetValueAU16(void) const;
   stw_types::uint16 GetValueAU16Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAU32(const std::vector<stw_types::uint32> & orc_Value);
   void SetValueAU32Element(const stw_types::uint32 ou32_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::uint32> GetValueAU32(void) const;
   stw_types::uint32 GetValueAU32Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAU64(const std::vector<stw_types::uint64> & orc_Value);
   void SetValueAU64Element(const stw_types::uint64 ou64_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::uint64> GetValueAU64(void) const;
   stw_types::uint64 GetValueAU64Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAS8(const std::vector<stw_types::sint8> & orc_Value);
   void SetValueAS8Element(const stw_types::sint8 os8_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::sint8> GetValueAS8(void) const;
   stw_types::sint8 GetValueAS8Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAS16(const std::vector<stw_types::sint16> & orc_Value);
   void SetValueAS16Element(const stw_types::sint16 os16_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::sint16> GetValueAS16(void) const;
   stw_types::sint16 GetValueAS16Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAS32(const std::vector<stw_types::sint32> & orc_Value);
   void SetValueAS32Element(const stw_types::sint32 os32_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::sint32> GetValueAS32(void) const;
   stw_types::sint32 GetValueAS32Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAS64(const std::vector<stw_types::sint64> & orc_Value);
   void SetValueAS64Element(const stw_types::sint64 os64_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::sint64> GetValueAS64(void) const;
   stw_types::sint64 GetValueAS64Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAF32(const std::vector<stw_types::float32> & orc_Value);
   void SetValueAF32Element(const stw_types::float32 of32_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::float32> GetValueAF32(void) const;
   stw_types::float32 GetValueAF32Element(const stw_types::uint32 ou32_Index) const;

   void SetValueAF64(const std::vector<stw_types::float64> & orc_Value);
   void SetValueAF64Element(const stw_types::float64 of64_Value, const stw_types::uint32 ou32_Index);
   const std::vector<stw_types::float64> GetValueAF64(void) const;
   stw_types::float64 GetValueAF64Element(const stw_types::uint32 ou32_Index) const;

   stw_types::sint32 SetValueFromBigEndianBlob(const std::vector<stw_types::uint8> & orc_Data);
   stw_types::sint32 SetValueFromLittleEndianBlob(const std::vector<stw_types::uint8> & orc_Data);
   void GetValueAsBigEndianBlob(std::vector<stw_types::uint8> & orc_Data) const;
   void GetValueAsLittleEndianBlob(std::vector<stw_types::uint8> & orc_Data) const;

   void SetArraySize(const stw_types::uint32 & oru32_Size);
   stw_types::uint32 GetArraySize(void) const;

   void SetType(const E_Type & ore_Value);
   E_Type GetType(void) const;

   bool GetArray(void) const;
   void SetArray(const bool oq_Value);

   stw_types::uint32 GetSizeByte(void) const;

   bool CheckInsideRange(const C_OSCNodeDataPoolContent::E_Type & ore_Type) const;
   bool operator <=(const C_OSCNodeDataPoolContent & orc_Cmp) const;
   bool operator >=(const C_OSCNodeDataPoolContent & orc_Cmp) const;
   bool operator >(const C_OSCNodeDataPoolContent & orc_Cmp) const;
   bool operator <(const C_OSCNodeDataPoolContent & orc_Cmp) const;
   bool operator ==(const C_OSCNodeDataPoolContent & orc_Cmp) const;
   bool operator !=(const C_OSCNodeDataPoolContent & orc_Cmp) const;

   bool CompareArrayGreaterOrEqual(const C_OSCNodeDataPoolContent & orc_Cmp, const stw_types::uint32 ou32_Index) const;
   bool CompareArrayGreater(const C_OSCNodeDataPoolContent & orc_Cmp, const stw_types::uint32 ou32_Index) const;

   void GetValueAsScaledString(const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                               std::string & orc_Output, const stw_types::uint32 ou32_Index) const;
   void GetAnyValueAsFloat64(stw_types::float64 & orf64_Output, const stw_types::uint32 ou32_Index) const;

private:
   E_Type me_Type;                                   ///< Currently active type
   bool mq_Array;                                    ///< Flag for array (true) or single element type (false)
   std::vector<stw_types::uint8> mc_Data;            ///< Contained value
   static const stw_types::sint32 mhs32_TypeError;   ///< Exception value for type mismatch
   static const stw_types::sint32 mhs32_AccessError; ///< Exception value for out of bounds array access

   template <typename T> void m_SetValue(const T & orc_Value, const E_Type oe_Type);
   template <typename T> void m_GetValue(const E_Type oe_Type, T & orc_Value) const;

   template <typename T> void m_SetValueArray(const T & orc_Value, const E_Type oe_Type);
   template <typename T> void m_SetValueArrayElement(const T & orc_Value, const stw_types::uint32 ou32_Index,
                                                     const E_Type oe_Type);
   template <typename T> void m_GetValueArray(const E_Type oe_Type, T & orc_Result) const;
   template <typename T> void m_GetValueArrayElement(const E_Type oe_Type, const stw_types::uint32 ou32_Index,
                                                     T & orc_Result) const;

   static stw_types::uint64 mh_BinaryToUInt64Big(const stw_types::uint8 * const opu8_Data);
   static stw_types::uint32 mh_BinaryToUInt32Big(const stw_types::uint8 * const opu8_Data);
   static stw_types::uint64 mh_BinaryToUInt64Little(const stw_types::uint8 * const opu8_Data);
   static stw_types::uint32 mh_BinaryToUInt32Little(const stw_types::uint8 * const opu8_Data);
   static void mh_UInt64ToBinaryBig(const stw_types::uint64 ou64_Data, stw_types::uint8 * const opu8_Data);
   static void mh_UInt32ToBinaryBig(const stw_types::uint32 ou32_Data, stw_types::uint8 * const opu8_Data);
   static void mh_UInt64ToBinaryLittle(const stw_types::uint64 ou64_Data, stw_types::uint8 * const opu8_Data);
   static void mh_UInt32ToBinaryLittle(const stw_types::uint32 ou32_Data, stw_types::uint8 * const opu8_Data);

   bool m_CompareArrayEqual(const C_OSCNodeDataPoolContent & orc_Cmp, const stw_types::uint32 ou32_Index) const;
   bool m_CompareArrayNotEqual(const C_OSCNodeDataPoolContent & orc_Cmp, const stw_types::uint32 ou32_Index) const;

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw_tgl::C_TGLCriticalSection mc_CriticalSection;

   void m_GetBaseType(bool & orq_IsUintBase, bool & orq_IsSintBase, bool & orq_IsFloatBase,
                      stw_types::uint64 & oru64_UintValue, stw_types::sint64 & ors64_SintValue,
                      stw_types::float64 & orf64_FloatValue) const;
   void m_GetBaseTypeArray(const stw_types::uint32 & oru32_Index, bool & orq_IsUintBase, bool & orq_IsSintBase,
                           bool & orq_IsFloatBase, stw_types::uint64 & oru64_UintValue,
                           stw_types::sint64 & ors64_SintValue, stw_types::float64 & orf64_FloatValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
