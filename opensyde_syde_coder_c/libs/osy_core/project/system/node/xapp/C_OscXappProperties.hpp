//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       X-app properties
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXAPPPROPERTIES_HPP
#define C_OSCXAPPPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXappProperties
{
public:
   C_OscXappProperties();

   void Initialize();

   void CalcHash(uint32_t & oru32_HashValue) const;

   uint32_t u32_PollingIntervalMs;                  ///< Data polling interval in milliseconds
   uint32_t u32_DataRequestIntervalMs;              ///< Data request interval in milliseconds
   C_OscSystemBus::E_Type e_ConnectedInterfaceType; ///< e.g. CAN, ethernet
   uint8_t u8_ConnectedInterfaceNumber;             ///< 0 = first interface for this type
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
