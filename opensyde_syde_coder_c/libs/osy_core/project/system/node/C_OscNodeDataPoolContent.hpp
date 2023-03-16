//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for any supported data type (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLCONTENT_HPP
#define C_OSCNODEDATAPOOLCONTENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <string>
#include "stwtypes.hpp"

#include "TglTasks.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class for any supported data pool data type
class C_OscNodeDataPoolContent
{
public:
   C_OscNodeDataPoolContent(void);
   C_OscNodeDataPoolContent(const C_OscNodeDataPoolContent & orc_Source);
   //lint -esym(1539,stw::opensyde_core::C_OscNodeDataPoolContent::mc_CriticalSection)
   //The lock of the copy is intended to remain unchanged.
   C_OscNodeDataPoolContent & operator = (const C_OscNodeDataPoolContent & orc_Source);
   virtual ~C_OscNodeDataPoolContent(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;
   virtual void CalcHashElement(uint32_t & oru32_HashValue, const uint32_t ou32_Index) const;
   virtual void CalcHashStructure(uint32_t & oru32_HashValue) const;

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

   void SetValueU8(const uint8_t ou8_Value);
   uint8_t GetValueU8(void) const;
   void SetValueU16(const uint16_t ou16_Value);
   uint16_t GetValueU16(void) const;
   void SetValueU32(const uint32_t ou32_Value);
   uint32_t GetValueU32(void) const;
   void SetValueU64(const uint64_t ou64_Value);
   uint64_t GetValueU64(void) const;
   void SetValueS8(const int8_t os8_Value);
   int8_t GetValueS8(void) const;
   void SetValueS16(const int16_t os16_Value);
   int16_t GetValueS16(void) const;
   void SetValueS32(const int32_t os32_Value);
   int32_t GetValueS32(void) const;
   void SetValueS64(const int64_t os64_Value);
   int64_t GetValueS64(void) const;
   void SetValueF32(const float32_t of32_Value);
   float32_t GetValueF32(void) const;
   void SetValueF64(const float64_t of64_Value);
   float64_t GetValueF64(void) const;

   void SetValueArrU8(const std::vector<uint8_t> & orc_Value);
   void SetValueArrU8Element(const uint8_t ou8_Value, const uint32_t ou32_Index);
   const std::vector<uint8_t> GetValueArrU8(void) const;
   uint8_t GetValueArrU8Element(const uint32_t ou32_Index) const;

   void SetValueArrU16(const std::vector<uint16_t> & orc_Value);
   void SetValueArrU16Element(const uint16_t ou16_Value, const uint32_t ou32_Index);
   const std::vector<uint16_t> GetValueArrU16(void) const;
   uint16_t GetValueArrU16Element(const uint32_t ou32_Index) const;

   void SetValueArrU32(const std::vector<uint32_t> & orc_Value);
   void SetValueArrU32Element(const uint32_t ou32_Value, const uint32_t ou32_Index);
   const std::vector<uint32_t> GetValueArrU32(void) const;
   uint32_t GetValueArrU32Element(const uint32_t ou32_Index) const;

   void SetValueArrU64(const std::vector<uint64_t> & orc_Value);
   void SetValueArrU64Element(const uint64_t ou64_Value, const uint32_t ou32_Index);
   const std::vector<uint64_t> GetValueArrU64(void) const;
   uint64_t GetValueArrU64Element(const uint32_t ou32_Index) const;

   void SetValueArrS8(const std::vector<int8_t> & orc_Value);
   void SetValueArrS8Element(const int8_t os8_Value, const uint32_t ou32_Index);
   const std::vector<int8_t> GetValueArrS8(void) const;
   int8_t GetValueArrS8Element(const uint32_t ou32_Index) const;

   void SetValueArrS16(const std::vector<int16_t> & orc_Value);
   void SetValueArrS16Element(const int16_t os16_Value, const uint32_t ou32_Index);
   const std::vector<int16_t> GetValueArrS16(void) const;
   int16_t GetValueArrS16Element(const uint32_t ou32_Index) const;

   void SetValueArrS32(const std::vector<int32_t> & orc_Value);
   void SetValueArrS32Element(const int32_t os32_Value, const uint32_t ou32_Index);
   const std::vector<int32_t> GetValueArrS32(void) const;
   int32_t GetValueArrS32Element(const uint32_t ou32_Index) const;

   void SetValueArrS64(const std::vector<int64_t> & orc_Value);
   void SetValueArrS64Element(const int64_t os64_Value, const uint32_t ou32_Index);
   const std::vector<int64_t> GetValueArrS64(void) const;
   int64_t GetValueArrS64Element(const uint32_t ou32_Index) const;

   void SetValueArrF32(const std::vector<float32_t> & orc_Value);
   void SetValueArrF32Element(const float32_t of32_Value, const uint32_t ou32_Index);
   const std::vector<float32_t> GetValueArrF32(void) const;
   float32_t GetValueArrF32Element(const uint32_t ou32_Index) const;

   void SetValueArrF64(const std::vector<float64_t> & orc_Value);
   void SetValueArrF64Element(const float64_t of64_Value, const uint32_t ou32_Index);
   const std::vector<float64_t> GetValueArrF64(void) const;
   float64_t GetValueArrF64Element(const uint32_t ou32_Index) const;

   int32_t SetValueFromBigEndianBlob(const std::vector<uint8_t> & orc_Data);
   int32_t SetValueFromLittleEndianBlob(const std::vector<uint8_t> & orc_Data);
   void GetValueAsBigEndianBlob(std::vector<uint8_t> & orc_Data) const;
   void GetValueAsLittleEndianBlob(std::vector<uint8_t> & orc_Data) const;

   void SetArraySize(const uint32_t & oru32_Size);
   uint32_t GetArraySize(void) const;

   void SetType(const E_Type & ore_Value);
   E_Type GetType(void) const;

   void SetArray(const bool oq_Value);
   bool GetArray(void) const;

   uint32_t GetSizeByte(void) const;

   bool CheckInsideRange(const C_OscNodeDataPoolContent::E_Type & ore_Type) const;
   bool operator <=(const C_OscNodeDataPoolContent & orc_Cmp) const;
   bool operator >=(const C_OscNodeDataPoolContent & orc_Cmp) const;
   bool operator >(const C_OscNodeDataPoolContent & orc_Cmp) const;
   bool operator <(const C_OscNodeDataPoolContent & orc_Cmp) const;
   bool operator ==(const C_OscNodeDataPoolContent & orc_Cmp) const;
   bool operator !=(const C_OscNodeDataPoolContent & orc_Cmp) const;

   bool CompareArrayGreaterOrEqual(const C_OscNodeDataPoolContent & orc_Cmp, const uint32_t ou32_Index) const;
   bool CompareArrayGreater(const C_OscNodeDataPoolContent & orc_Cmp, const uint32_t ou32_Index) const;

   void GetValueAsScaledString(const float64_t of64_Factor, const float64_t of64_Offset, std::string & orc_Output,
                               const uint32_t ou32_Index, const bool oq_AllowRangeAdaptation = true,
                               const bool oq_AllowSpecialHandling = false) const;
   void GetAnyValueAsFloat64(float64_t & orf64_Output, const uint32_t ou32_Index) const;

private:
   E_Type me_Type;               ///< Currently active type
   bool mq_Array;                ///< Flag for array (true) or single element type (false)
   std::vector<uint8_t> mc_Data; ///< Contained value

   template <typename T> void m_SetValue(const T & orc_Value, const E_Type oe_Type);
   template <typename T> void m_GetValue(const E_Type oe_Type, T & orc_Value) const;

   template <typename T> void m_SetValueArray(const T & orc_Value, const E_Type oe_Type);
   //lint -e{8058,8080} //template parameter names are not properly handled by naming convention check
   template <typename T> void m_SetValueArrayElement(const T & orc_Value, const uint32_t ou32_Index,
                                                     const E_Type oe_Type);
   template <typename T> void m_GetValueArray(const E_Type oe_Type, T & orc_Result) const;
   //lint -e{8058,8080} //template parameter names are not properly handled by naming convention check
   template <typename T> void m_GetValueArrayElement(const E_Type oe_Type, const uint32_t ou32_Index,
                                                     T & orc_Result) const;

   static uint64_t mh_BinaryToUnsignedInt64Big(const uint8_t * const opu8_Data);
   static uint32_t mh_BinaryToUnsignedInt32Big(const uint8_t * const opu8_Data);
   static uint64_t mh_BinaryToUnsignedInt64Little(const uint8_t * const opu8_Data);
   static uint32_t mh_BinaryToUnsignedInt32Little(const uint8_t * const opu8_Data);
   static void mh_UnsignedInt64ToBinaryBig(const uint64_t ou64_Data, uint8_t * const opu8_Data);
   static void mh_UnsignedInt32ToBinaryBig(const uint32_t ou32_Data, uint8_t * const opu8_Data);
   static void mh_UnsignedInt64ToBinaryLittle(const uint64_t ou64_Data, uint8_t * const opu8_Data);
   static void mh_UnsignedInt32ToBinaryLittle(const uint32_t ou32_Data, uint8_t * const opu8_Data);

   bool m_CompareArrayEqual(const C_OscNodeDataPoolContent & orc_Cmp, const uint32_t ou32_Index) const;
   bool m_CompareArrayNotEqual(const C_OscNodeDataPoolContent & orc_Cmp, const uint32_t ou32_Index) const;

   // It is mutable because of the constness of the getter functions. Without the keyword mutable the getter functions
   // must be non const and that is not wanted.
   mutable stw::tgl::C_TglCriticalSection mc_CriticalSection;

   void m_GetBaseType(bool & orq_IsUintBase, bool & orq_IsSintBase, bool & orq_IsFloatBase, uint64_t & oru64_UintValue,
                      int64_t & ors64_SintValue, float64_t & orf64_FloatValue) const;
   void m_GetBaseTypeArray(const uint32_t & oru32_Index, bool & orq_IsUintBase, bool & orq_IsSintBase,
                           bool & orq_IsFloatBase, uint64_t & oru64_UintValue, int64_t & ors64_SintValue,
                           float64_t & orf64_FloatValue) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
