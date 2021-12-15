//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Time functions

   cf. header for details
   Here: Implementation for Windows.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <unistd.h>
#include <time.h>
#include "stwtypes.h"
#include "TGLTime.h"

//----------------------------------------------------------------------------------------------------------------------

using namespace stw_types;
using namespace stw_tgl;

/* -- Defines ------------------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */


//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current date and time with milli seconds

   \param[out]  orc_DateTime     Current date and time value with milli seconds
*/
//----------------------------------------------------------------------------------------------------------------------
void stw_tgl::TGL_GetDateTimeNow(C_TGLDateTime & orc_DateTime)
{
   struct timespec t_TimeSpec;
   struct tm * pt_LocalTime;
   time_t t_UnixTime;

   clock_gettime(CLOCK_REALTIME, &t_TimeSpec);

   // Make sure nsec is <1sec
   t_TimeSpec.tv_sec += (t_TimeSpec.tv_nsec / 1000000000);
   t_TimeSpec.tv_nsec = (t_TimeSpec.tv_nsec % 1000000000);

   t_UnixTime = t_TimeSpec.tv_sec;
   pt_LocalTime = localtime(&t_UnixTime);

   //Convert from tm format to TGL format
   orc_DateTime.mu16_Year  = static_cast<uint16>(pt_LocalTime->tm_year + 1900);
   orc_DateTime.mu8_Month  = static_cast<uint8>(pt_LocalTime->tm_mon + 1);
   orc_DateTime.mu8_Day    = static_cast<uint8>(pt_LocalTime->tm_mday);
   orc_DateTime.mu8_Hour   = static_cast<uint8>(pt_LocalTime->tm_hour);
   orc_DateTime.mu8_Minute = static_cast<uint8>(pt_LocalTime->tm_min);
   orc_DateTime.mu8_Second = static_cast<uint8>(pt_LocalTime->tm_sec);
   //Technically tm_sec could be > 59 in some rare cases due to leap seconds
   if (orc_DateTime.mu8_Second > 59U)
   {
      orc_DateTime.mu8_Second = 59U;
   }
   orc_DateTime.mu16_MilliSeconds = static_cast<uint16>(t_TimeSpec.tv_nsec / 1000000);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delays for the given number of microseconds.

   \param[in]  ou32_NumberUs    Number of microseconds to wait
*/
//----------------------------------------------------------------------------------------------------------------------
void stw_tgl::TGL_DelayUs(const uint32 ou32_NumberUs)
{
   uint64 u64_StopTime = TGL_GetTickCountUS() + ou32_NumberUs;

   while (TGL_GetTickCountUS() < u64_StopTime)
   {

   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get system time in microseconds

   Return elapsed time since system start in micro-seconds as precise as possible.

   Hint:
   The staring point of CLOCK_MONOTONIC is not specified in the POSIX standard.
   However, for Linux the starting point is at boot time and its the same for all
   processes. Thus it can be used to get the system-up time.

   \return
   System time in microseconds.
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 stw_tgl::TGL_GetTickCountUS(void)
{
   sint32 s32_Error;
   uint64 u64_Time_us = 0;
   struct timespec t_Time;

   s32_Error = clock_gettime(CLOCK_MONOTONIC, &t_Time);
   if (s32_Error == 0)
   {
      u64_Time_us = ((uint64)t_Time.tv_sec * 1000000) + ((t_Time.tv_nsec + 500) / 1000);
   }
   return u64_Time_us;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return elapsed time since system start in ms.

   Return elapsed time since system start in milliseconds.

   Hint:
   The staring point of CLOCK_MONOTONIC is not specified in the POSIX standard.
   However, for Linux the starting point is at boot time and its the same for all
   processes. Thus it can be used to get the system-up time.

   \return
   System time in milliseconds
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 stw_tgl::TGL_GetTickCount(void)
{
   uint32 u32_ret = (uint32)(TGL_GetTickCountUS() / 1000);
   return u32_ret;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sleep for a number of milliseconds

   Delay for a number of milliseconds. Thread control shall meanwhile be passed on.
   i.e.: no active, blocking watiting.

   \param[in]    ou32_NumberMs    number of milliseconds to delay
*/
//----------------------------------------------------------------------------------------------------------------------
void stw_tgl::TGL_Sleep(const uint32 ou32_NumberMs)
{
   usleep ((useconds_t)ou32_NumberMs * 1000);
}
