//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Time functions

   cf. header for details

   Here: Implementation for Windows.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers

#include <windows.h>
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
   SYSTEMTIME c_LocalTimeNow;

   //Get now
   GetLocalTime(&c_LocalTimeNow);

   //Convert to readable format
   orc_DateTime.mu16_Year  = c_LocalTimeNow.wYear;
   orc_DateTime.mu8_Month  = static_cast<uint8_t>(c_LocalTimeNow.wMonth);
   orc_DateTime.mu8_Day    = static_cast<uint8_t>(c_LocalTimeNow.wDay);
   orc_DateTime.mu8_Hour   = static_cast<uint8_t>(c_LocalTimeNow.wHour);
   orc_DateTime.mu8_Minute = static_cast<uint8_t>(c_LocalTimeNow.wMinute);
   orc_DateTime.mu8_Second = static_cast<uint8_t>(c_LocalTimeNow.wSecond);
   orc_DateTime.mu16_MilliSeconds = c_LocalTimeNow.wMilliseconds;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Actively wait for a period of time

   Actively blocks for the given number of microseconds.
   Depending on the underlying system very short delays might not be possible
     and are rounded up to the smallest possible delay.
   Thus for example 10 calls of TGL_DelayUs(1) can cause a longer delay than 1 call of TGL_DelayUs(10).

   \param[in]   ou32_NumberUs             number of microseconds to block
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
   Sleep(ou32_NumberMs);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system time in microseconds

   Return elapsed time since system start in micro-seconds as precise as possible.

   \return
   System time in microseconds.
*/
//----------------------------------------------------------------------------------------------------------------------
uint64_t stw::tgl::TglGetTickCountUs(void)
{
   static bool hq_FirstCall = true;
   static float64_t hf64_CountsPer1US;
   LARGE_INTEGER u_Counts;

   if (hq_FirstCall == true)
   {
      bool q_Test;
      q_Test = (QueryPerformanceFrequency(&u_Counts) == 0) ? false : true;
      tgl_assert(q_Test == true);
      hf64_CountsPer1US = static_cast<float64_t>(u_Counts.QuadPart) / 1000000.0; //we want the result in us not in
                                                                                 // seconds
      hq_FirstCall = false;
   }

   (void)QueryPerformanceCounter(&u_Counts);
   return static_cast<uint64_t>(static_cast<float64_t>(u_Counts.QuadPart) / hf64_CountsPer1US); //lint !e1960
   //dropping the decimals is precise enough
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system time in milliseconds

   Return elapsed time since system start in milliseconds as precise as possible.
   As we return an uint32 milliseconds value it will overflow after 49.7 days (just like the Win32 GetTickCount)

   \return
   System time in milliseconds
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t stw::tgl::TglGetTickCount(void)
{
   return static_cast<uint32_t>(TglGetTickCountUs() / 1000U);
}

