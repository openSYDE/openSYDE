//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for message trace
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETUTIL_HPP
#define C_CAMMETUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"
#include "C_CamMetTreeLoggerData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetUtil
{
public:
   C_CamMetUtil();

   static std::vector<int32_t> h_GetMultiplexerOrder(
      const std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals);
   static int32_t h_GetMultiplexerValue(
      const std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals);
   static int32_t h_GetRowForMultiplexerValue(
      const std::vector<stw::opensyde_core::C_OscComMessageLoggerDataSignal> & orc_Signals,
      const int32_t os32_MultiplexerValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
