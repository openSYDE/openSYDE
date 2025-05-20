//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for static functions for usage in system update and basic update.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscUpdateUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns an adapted device transfer data timeout time for compensating a potential higher bus load

   The device specific time does only cover the needed time of the device itself, but not the real communication
   time. In case of a high bus load this time can vary a lot.

   Information for the calculation:
   * Adding the result of the calculation as offset to the original time
   * Assuming we have 7 bytes per CAN frame
   * The lowest supported bitrate is 100kB/s
   * -> 2ms for each CAN message roughly results in the number of messages for each block * 2ms
   * Assuming a high "Alien" busload of 80% we need to multiply by 5
   -> (((ou32_MaxBlockLength / 7) * 2ms) * 5) + original device timeout time

   \param[in]       ou32_DeviceTransferDataTimeout     Device specific timeout time as base
   \param[in]       ou32_MaxBlockLength                Maximum number of bytes of each block
   \param[in]       ou32_Bitrate                       Bus bitrate in kBit/s

   \return
   Calculated device transfer data timeout
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_OscUpdateUtil::h_GetAdaptedTransferDataTimeout(const uint32_t ou32_DeviceTransferDataTimeout,
                                                          const uint32_t ou32_MaxBlockLength,
                                                          const uint32_t ou32_Bitrate)
{
   uint32_t u32_AdaptedTime = ou32_DeviceTransferDataTimeout;
   uint32_t u32_Bitrate = ou32_Bitrate;
   const uint32_t u32_Offset = (ou32_MaxBlockLength * 10U) / 7U;

   if (u32_Bitrate == 0U)
   {
      //this should not happen ... assume the slowest we support (100kBit/s)
      u32_Bitrate = 100U;
   }

   // Scale the offset to the expected bitrate on the bus in relation to the slowest bitrate of 100 kBit/s
   u32_AdaptedTime += (u32_Offset * 100U) / u32_Bitrate;

   osc_write_log_info("Update Node",
                      "Used \"transferdatatimeout\" by adding an offset for compensating a "
                      "potential high bus load: " +
                      C_SclString::IntToStr(u32_AdaptedTime) + "ms");

   return u32_AdaptedTime;
}
