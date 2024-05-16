//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for common node CAN related functionalities
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDNODECANUTIL_HPP
#define C_PUISDNODECANUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscCanSignal.hpp"
#include "C_PuiSdNodeCanSignal.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdNodeCanUtil
{
public:
   C_PuiSdNodeCanUtil();

   enum E_SignalType
   {
      eST_UNSIGNED = 0,
      eST_SIGNED = 1,
      eST_FLOAT32 = 2,
      eST_FLOAT64 = 3
   };

   static int32_t h_SetCanSignalPosition(const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId,
                                         const uint32_t & oru32_SignalIndex,
                                         const stw::opensyde_core::C_OscCanSignal & orc_OscSignal,
                                         const C_PuiSdNodeCanSignal & orc_UiSignal);
   static void h_AdaptValueToSignalLength(const uint16_t ou16_BitLength,
                                          stw::opensyde_core::C_OscNodeDataPoolContent & orc_Content);
   static stw::opensyde_core::C_OscNodeDataPoolContent::E_Type h_GetRequiredType(const uint16_t ou16_BitLength,
                                                                                 const E_SignalType oe_SignalType);
   static E_SignalType h_GetSignalType(const stw::opensyde_core::C_OscNodeDataPoolContent::E_Type oe_DpType);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
