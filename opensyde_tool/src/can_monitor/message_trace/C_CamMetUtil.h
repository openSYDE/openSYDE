//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility functions for message trace
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETUTIL_H
#define C_CAMMETUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"
#include "C_CamMetTreeLoggerData.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetUtil
{
public:
   C_CamMetUtil();

   static std::vector<stw_types::sintn> h_GetMultiplexerOrder(
      const std::vector<stw_opensyde_core::C_OSCComMessageLoggerDataSignal> & orc_Signals);
   static stw_types::sint32 mh_GetMultiplexerValue(
      const std::vector<stw_opensyde_core::C_OSCComMessageLoggerDataSignal> & orc_Signals);
   static stw_types::sint32 mh_GetRowForMultiplexerValue(
      const std::vector<stw_opensyde_core::C_OSCComMessageLoggerDataSignal> & orc_Signals,
      const stw_types::sint32 os32_MultiplexerValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
