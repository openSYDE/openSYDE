//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool content utility functions
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLCONTENTUTIL_HPP
#define C_OSCNODEDATAPOOLCONTENTUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolContentUtil
{
public:
   // enum indicates where a specific value should be set to
   enum E_SetValue
   {
      eLEAVE_VALUE, // if in range
      eTO_ZERO      //    -"-
   };

   // enum indicates where a specific value was set to
   enum E_ValueChangedTo
   {
      eNO_CHANGE,
      eZERO,
      eMIN,
      eMAX
   };

   static void h_ZeroContent(stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   static void h_SetValueInContent(const float64_t of64_Value,
                                   stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content,
                                   const uint32_t ou32_ArrayIndex = 0U);
   // set a value in min and max range
   static int32_t h_SetValueInMinMaxRange(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                          const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                          stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value,
                                          C_OscNodeDataPoolContentUtil::E_ValueChangedTo & ore_ValueChangedTo, const C_OscNodeDataPoolContentUtil::E_SetValue oe_SetValue =
                                             C_OscNodeDataPoolContentUtil::eLEAVE_VALUE);
   static uint32_t h_GetDataTypeSizeInByte(const C_OscNodeDataPoolContent::E_Type oe_Type);

private:
   C_OscNodeDataPoolContentUtil(void);

   static const float64_t mhf64_EPSILON;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
