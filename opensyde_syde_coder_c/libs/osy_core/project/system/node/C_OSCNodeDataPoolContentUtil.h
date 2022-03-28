//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool content utility functions
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLCONTENTUTIL_H
#define C_OSCNODEDATAPOOLCONTENTUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeDataPoolContentUtil
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

   static void h_ZeroContent(stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content);
   static void h_SetValueInContent(const stw_types::float64 of64_Value,
                                   stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Content,
                                   const stw_types::uint32 ou32_ArrayIndex = 0U);
   // set a value in min and max range
   static stw_types::sint32 h_SetValueInMinMaxRange(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                                    const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                                    stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value,
                                                    C_OSCNodeDataPoolContentUtil::E_ValueChangedTo & ore_ValueChangedTo, const C_OSCNodeDataPoolContentUtil::E_SetValue oe_SetValue =
                                                       C_OSCNodeDataPoolContentUtil::eLEAVE_VALUE);
   static stw_types::uint32 h_GetDataTypeSizeInByte(const C_OSCNodeDataPoolContent::E_Type oe_Type);

private:
   C_OSCNodeDataPoolContentUtil(void);

   static const stw_types::float64 mhf64_EPSILON;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
