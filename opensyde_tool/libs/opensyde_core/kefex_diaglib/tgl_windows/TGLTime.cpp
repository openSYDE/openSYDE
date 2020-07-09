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
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <windows.h>
#include "stwtypes.h"
#include "TGLTime.h"
#include "TGLUtils.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_types;
using namespace stw_tgl;

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
void stw_tgl::TGL_GetDateTimeNow(C_TGLDateTime & orc_DateTime)
{
   SYSTEMTIME c_LocalTimeNow;

   //Get now
   GetLocalTime(&c_LocalTimeNow);

   //Convert to readable format
   orc_DateTime.mu16_Year  = c_LocalTimeNow.wYear;
   orc_DateTime.mu8_Month  = static_cast<uint8>(c_LocalTimeNow.wMonth);
   orc_DateTime.mu8_Day    = static_cast<uint8>(c_LocalTimeNow.wDay);
   orc_DateTime.mu8_Hour   = static_cast<uint8>(c_LocalTimeNow.wHour);
   orc_DateTime.mu8_Minute = static_cast<uint8>(c_LocalTimeNow.wMinute);
   orc_DateTime.mu8_Second = static_cast<uint8>(c_LocalTimeNow.wSecond);
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
void stw_tgl::TGL_DelayUs(const uint32 ou32_NumberUs)
{
   const uint64 u64_StopTime = TGL_GetTickCountUS() + ou32_NumberUs;

   while (TGL_GetTickCountUS() < u64_StopTime)
   {
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system time in microseconds

   Return elapsed time since system start in micro-seconds as precise as possible.

   \return
   System time in micro-seconds.
*/
//----------------------------------------------------------------------------------------------------------------------
uint64 stw_tgl::TGL_GetTickCountUS(void)
{
   static bool hq_FirstCall = true;
   static float64 hf64_CountsPer1US;
   LARGE_INTEGER u_Counts;

   if (hq_FirstCall == true)
   {
      bool q_Test;
      q_Test = (QueryPerformanceFrequency(&u_Counts) == 0) ? false : true;
      tgl_assert(q_Test == true);
      hf64_CountsPer1US = static_cast<float64>(u_Counts.QuadPart) / 1000000.0; //we want the result in us not in seconds
      hq_FirstCall = false;
   }

   (void)QueryPerformanceCounter(&u_Counts);
   return static_cast<uint64>(static_cast<float64>(u_Counts.QuadPart) / hf64_CountsPer1US); //lint !e1960
   //dropping the decimals is precise enough
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get system time in milliseconds

   Return elapsed time since system start in micro-seconds as precise as possible.
   As we return an uint32 milliseconds value it will overflow after 49.7 days (just like the Win32 GetTickCount)

   \return
   System time in milliseconds.
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 stw_tgl::TGL_GetTickCount(void)
{
   return static_cast<uint32>(TGL_GetTickCountUS() / 1000U);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sleep for a number of milliseconds

   Delay for a number of milliseconds. Thread control shall meanwhile be passed on.
   i.e.: no active, blocking waiting.

   \param[in]    ou32_NumberMs    number of milliseconds to delay
*/
//----------------------------------------------------------------------------------------------------------------------
void stw_tgl::TGL_Sleep(const uint32 ou32_NumberMs)
{
   Sleep(ou32_NumberMs);
}
