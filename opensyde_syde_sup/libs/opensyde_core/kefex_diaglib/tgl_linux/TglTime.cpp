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
#include <ctime>
#include "stwtypes.hpp"
#include "TglTime.hpp"
#include "TglUtils.hpp"

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
void stw::tgl::TglGetDateTimeNow(C_TglDateTime & orc_DateTime)
{
   struct timespec c_TimeSpec;
   struct std::tm * pc_LocalTime;
   struct std::tm c_Time;

   time_t x_UnixTime; //lint !e8080 //using type to match library API

   clock_gettime(CLOCK_REALTIME, &c_TimeSpec);

   // Make sure nsec is <1sec
   c_TimeSpec.tv_sec += (c_TimeSpec.tv_nsec / 1000000000);
   c_TimeSpec.tv_nsec = (c_TimeSpec.tv_nsec % 1000000000);

   x_UnixTime = c_TimeSpec.tv_sec;
   pc_LocalTime = localtime_r(&x_UnixTime, &c_Time);

   tgl_assert(pc_LocalTime != NULL);
   if (pc_LocalTime != NULL)
   {
      //Convert from tm format to TGL format
      orc_DateTime.mu16_Year  = static_cast<uint16_t>(pc_LocalTime->tm_year + 1900);
      orc_DateTime.mu8_Month  = static_cast<uint8_t>(pc_LocalTime->tm_mon + 1);
      orc_DateTime.mu8_Day    = static_cast<uint8_t>(pc_LocalTime->tm_mday);
      orc_DateTime.mu8_Hour   = static_cast<uint8_t>(pc_LocalTime->tm_hour);
      orc_DateTime.mu8_Minute = static_cast<uint8_t>(pc_LocalTime->tm_min);
      orc_DateTime.mu8_Second = static_cast<uint8_t>(pc_LocalTime->tm_sec);
   }

   //Technically tm_sec could be > 59 in some rare cases due to leap seconds
   if (orc_DateTime.mu8_Second > 59U)
   {
      orc_DateTime.mu8_Second = 59U;
   }
   orc_DateTime.mu16_MilliSeconds = static_cast<uint16_t>(c_TimeSpec.tv_nsec / 1000000);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delays for the given number of microseconds.

   \param[in]  ou32_NumberUs    Number of microseconds to wait
*/
//----------------------------------------------------------------------------------------------------------------------
void stw::tgl::TglDelayUs(const uint32_t ou32_NumberUs)
{
   const uint64_t u64_StopTime = TglGetTickCountUs() + ou32_NumberUs;

   while (TglGetTickCountUs() < u64_StopTime)
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sleep for a number of milliseconds

   Delay for a number of milliseconds. Thread control shall meanwhile be passed on.
   i.e.: no active, blocking waiting.

   \param[in]    ou32_NumberMs    number of milliseconds to delay
*/
//----------------------------------------------------------------------------------------------------------------------
void stw::tgl::TglSleep(const uint32_t ou32_NumberMs)
{
   usleep(static_cast<useconds_t>(ou32_NumberMs) * 1000);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get system time in microseconds

   Return elapsed time since system start in micro-seconds as precise as possible.

   Hint:
   The starting point of CLOCK_MONOTONIC is not specified in the POSIX standard.
   However, for Linux the starting point is at boot time and its the same for all
   processes. Thus it can be used to get the system-up time.

   \return
   System time in micro-seconds.
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t stw::tgl::TglGetTickCountUs(void)
{
   int32_t s32_Error;
   uint64_t u64_TimeUs = 0;
   struct timespec c_Time;

   s32_Error = clock_gettime(CLOCK_MONOTONIC, &c_Time);
   if (s32_Error == 0)
   {
      u64_TimeUs = (static_cast<uint64_t>(c_Time.tv_sec) * 1000000U) +
                   ((static_cast<uint64_t>(c_Time.tv_nsec) + 500U) / 1000U);
   }
   return u64_TimeUs;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return elapsed time since system start in ms.

   Return elapsed time since system start in milliseconds.

   Hint:
   The starting point of CLOCK_MONOTONIC is not specified in the POSIX standard.
   However, for Linux the starting point is at boot time and its the same for all
   processes. Thus it can be used to get the system-up time.

   \return
   System time in milliseconds
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t stw::tgl::TglGetTickCount(void)
{
   return static_cast<uint32_t>(TglGetTickCountUs() / 1000U);
}
