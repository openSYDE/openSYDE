//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for static functions for usage in system update and basic update.
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCUPDATEUTIL_HPP
#define C_OSCUPDATEUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscUpdateUtil
{
public:
   static uint32_t h_GetAdaptedTransferDataTimeout(const uint32_t ou32_DeviceTransferDataTimeout,
                                                   const uint32_t ou32_MaxBlockLength, const uint32_t ou32_Bitrate);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
