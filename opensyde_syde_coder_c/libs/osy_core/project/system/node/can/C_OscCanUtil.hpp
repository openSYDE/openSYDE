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
#include "C_OscNodeDataPoolContent.hpp"
#include "C_OscCanSignal.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscCanUtil
{
public:
   C_OscCanUtil(void);

   static bool h_IsSignalInMessage(const uint8_t ou8_Dlc, const stw::opensyde_core::C_OscCanSignal & orc_Signal);

   static void h_GetSignalValue(const uint8_t(&orau8_CanDb)[8], const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                std::vector<uint8_t> & orc_DataPoolData,
                                const C_OscNodeDataPoolContent::E_Type oe_ContentType);

   static void h_GetSignalValue(const uint8_t(&orau8_CanDb)[8], const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                C_OscNodeDataPoolContent & orc_Value);

   static void h_SetSignalValue(uint8_t(&orau8_CanDb)[8],
                                const stw::opensyde_core::C_OscCanSignal & orc_Signal,
                                const C_OscNodeDataPoolContent &orc_Value);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
