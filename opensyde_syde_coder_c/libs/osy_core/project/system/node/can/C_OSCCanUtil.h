//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for CAN specific operations (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCANUTIL_H
#define C_OSCCANUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCNodeDataPoolContent.h"
#include "C_OSCCanSignal.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCCanUtil
{
public:
   C_OSCCanUtil(void);

   static bool h_IsSignalInMessage(const stw_types::uint8 ou8_Dlc,
                                   const stw_opensyde_core::C_OSCCanSignal & orc_Signal);

   static void h_GetSignalValue(const stw_types::uint8(&orau8_CanDb)[8],
                                const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                std::vector<stw_types::uint8> & orc_DataPoolData,
                                const C_OSCNodeDataPoolContent::E_Type oe_ContentType);

   static void h_GetSignalValue(const stw_types::uint8(&orau8_CanDb)[8],
                                const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                C_OSCNodeDataPoolContent & orc_Value);

   static void h_SetSignalValue(stw_types::uint8(&orau8_CanDb)[8],
                                const stw_opensyde_core::C_OSCCanSignal & orc_Signal,
                                const C_OSCNodeDataPoolContent &orc_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
