//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for an element in a list in a data pool (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTELEMENT_HPP
#define C_OSCNODEDATAPOOLLISTELEMENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_OscNodeDataPoolListElement
{
public:
   C_OscNodeDataPoolListElement(void);

   void CalcHash(uint32_t & oru32_HashValue) const;
   void CalcHashElement(uint32_t & oru32_HashValue, const uint32_t ou32_Index) const;
   void CalcHashStructure(uint32_t & oru32_HashValue) const;

   uint32_t GetSizeByte(void) const;

   ///access permissions to this element
   enum E_Access
   {
      eACCESS_RW, ///< Data pool list element access right read/write
      eACCESS_RO  ///< Data pool list element access right read only
   };

   stw::scl::C_SclString c_Name;                          ///< Element name
   stw::scl::C_SclString c_Comment;                       ///< Element comment
   C_OscNodeDataPoolContent c_MinValue;                   ///< Minimum value(s) for complete data element
   C_OscNodeDataPoolContent c_MaxValue;                   ///< Maximum value(s) for complete data element
   float64_t f64_Factor;                                  ///< Adaptation factor for display
   float64_t f64_Offset;                                  ///< Offset factor for display
   stw::scl::C_SclString c_Unit;                          ///< Variable unit
   E_Access e_Access;                                     ///< Defined access level
   bool q_InterpretAsString;                              ///< Flag if data type should be interpreted as string
   bool q_DiagEventCall;                                  ///< Flag if there is an event call
   std::vector<C_OscNodeDataPoolContent> c_DataSetValues; ///< Dataset value(s) for complete data element
   C_OscNodeDataPoolContent c_Value;                      ///< Current value(s) for complete data element
   C_OscNodeDataPoolContent c_NvmValue;                   ///< Current value(s) for complete data element in NVM
   bool q_NvmValueChanged;                                ///< Flag if the NvM element value was changed
   bool q_NvmValueIsValid;                                ///< Flag if the NvM element value was read from ECU
   uint32_t u32_NvmStartAddress;                          ///< NvM address of data element

   void SetType(const C_OscNodeDataPoolContent::E_Type & ore_Value);
   void SetArray(const bool oq_Array);
   void SetArraySize(const uint32_t & oru32_Size);
   C_OscNodeDataPoolContent::E_Type GetType(void) const;
   bool GetArray(void) const;
   uint32_t GetArraySize(void) const;
   int32_t CheckValueRange(void) const;
   int32_t CheckNvmValueRange(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
