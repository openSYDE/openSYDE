//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for an element in a list in a data pool (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLLISTELEMENTH
#define C_OSCNODEDATAPOOLLISTELEMENTH

/* -- Includes ------------------------------------------------------------- */

#include <vector>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_OSCNodeDataPoolListElement
{
public:
   C_OSCNodeDataPoolListElement(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::uint32 GetSizeByte(void) const;

   ///access permissions to this element
   enum E_Access
   {
      eACCESS_RW, ///< Data pool list element access right read/write
      eACCESS_RO  ///< Data pool list element access right read only
   };

   stw_scl::C_SCLString c_Name;                           ///< Element name
   stw_scl::C_SCLString c_Comment;                        ///< Element comment
   C_OSCNodeDataPoolContent c_MinValue;                   ///< Minimum value(s) for complete data element
   C_OSCNodeDataPoolContent c_MaxValue;                   ///< Maximum value(s) for complete data element
   stw_types::float64 f64_Factor;                         ///< Adaptation factor for display
   stw_types::float64 f64_Offset;                         ///< Offset factor for display
   stw_scl::C_SCLString c_Unit;                           ///< Variable unit
   E_Access e_Access;                                     ///< Defined access level
   bool q_DiagEventCall;                                  ///< Flag if there is an event call
   std::vector<C_OSCNodeDataPoolContent> c_DataSetValues; ///< Dataset value(s) for complete data element
   C_OSCNodeDataPoolContent c_Value;                      ///< Current value(s) for complete data element
   C_OSCNodeDataPoolContent c_NvmValue;                   ///< Current value(s) for complete data element in NVM
   bool q_NvMValueChanged;                                ///< Flag if the NvM element value was changed
   bool q_IsValid;                                        ///< Flag if the NvM element value was read from ECU
   stw_types::uint32 u32_NvMStartAddress;                 ///< NvM address of data element

   void SetType(const C_OSCNodeDataPoolContent::E_Type & ore_Value);
   void SetArray(const bool oq_Array);
   void SetArraySize(const stw_types::uint32 & oru32_Size);
   C_OSCNodeDataPoolContent::E_Type GetType(void) const;
   bool GetArray(void) const;
   stw_types::uint32 GetArraySize(void) const;
   stw_types::sint32 CheckValueRange(void) const;
   stw_types::sint32 CheckNvmValueRange(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
