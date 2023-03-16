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
#include "stwtypes.hpp"
#include "TglTime.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current date and time with milli seconds

   \param[out] orc_DateTime Current date and time value with milli seconds
*/
//----------------------------------------------------------------------------------------------------------------------
void TGL_PACKAGE stw::tgl::TglGetDateTimeNow(C_TglDateTime & orc_DateTime)
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
   orc_DateTime.mu16_Year  = static_cast<uint16_t>(pt_LocalTime->tm_year + 1900);
   orc_DateTime.mu8_Month  = static_cast<uint8_t>(pt_LocalTime->tm_mon + 1);
   orc_DateTime.mu8_Day    = static_cast<uint8_t>(pt_LocalTime->tm_mday);
   orc_DateTime.mu8_Hour   = static_cast<uint8_t>(pt_LocalTime->tm_hour);
   orc_DateTime.mu8_Minute = static_cast<uint8_t>(pt_LocalTime->tm_min);
   orc_DateTime.mu8_Second = static_cast<uint8_t>(pt_LocalTime->tm_sec);
   //Technically tm_sec could be > 59 in some rare cases due to leap seconds
   if (orc_DateTime.mu8_Second > 59U)
   {
      orc_DateTime.mu8_Second = 59U;
   }
   orc_DateTime.mu16_MilliSeconds = static_cast<uint16_t>(t_TimeSpec.tv_nsec / 1000000);
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
uint64_t TGL_PACKAGE stw::tgl::TglGetTickCountUs(void)
{
   int32_t s32_Error;
   uint64_t u64_Time_us = 0;
   struct timespec t_Time;

   s32_Error = clock_gettime(CLOCK_MONOTONIC, &t_Time);
   if (s32_Error == 0)
   {
      u64_Time_us = ((uint64_t)t_Time.tv_sec * 1000000) + ((t_Time.tv_nsec + 500) / 1000);
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
uint32_t TGL_PACKAGE stw::tgl::TglGetTickCount(void)
{
   return static_cast<uint32_t>(TglGetTickCountUs() / 1000U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delays for the given number of microseconds.

   \param[in]  ou32_NumberUs    Number of microseconds to wait
*/
//----------------------------------------------------------------------------------------------------------------------
void TGL_PACKAGE stw::tgl::TglDelayUs(const uint32_t ou32_NumberUs)
{
   const uint64_t u64_StopTime = TglGetTickCountUs() + ou32_NumberUs;

   while (TglGetTickCountUs() < u64_StopTime)
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sleep for a number of milliseconds

   Delay for a number of milliseconds. Thread control shall meanwhile be passed on.
   i.e.: no active, blocking waiting.

   \param[in]    ou32_NumberMs    number of milliseconds to delay
*/
//----------------------------------------------------------------------------------------------------------------------
void TGL_PACKAGE stw::tgl::TglSleep(const uint32_t ou32_NumberMs)
{
   usleep ((useconds_t)ou32_NumberMs * 1000);
}
