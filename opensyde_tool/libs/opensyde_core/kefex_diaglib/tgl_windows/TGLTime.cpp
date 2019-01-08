//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       DiagLib Target Glue Layer: Time functions

   cf. header for details

   Here: Implementation for Windows.

   \implementation
   project     DiagLib
   copyright   STW (c) 1999-200x
   license     use only under terms of contract / confidential

   created     21.07.2009  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include <windows.h>
#include "stwtypes.h"
#include "TGLTime.h"
#include "TGLUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_tgl;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Get current date and time with milli seconds

   \param[out] orc_DateTime Current date and time value with milli seconds

   \created     14.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Actively wait for a period of time

   Actively blocks for the given number of microseconds.
   Depending on the underlying system very short delays might not be possible
     and are rounded up to the smallest possible delay.
   Thus for example 10 calls of TGL_DelayUs(1) can cause a longer delay than 1 call of TGL_DelayUs(10).

   \param[in]   ou32_NumberUs             number of microseconds to block

   \created     26.07.2000 STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void stw_tgl::TGL_DelayUs(const uint32 ou32_NumberUs)
{
   static bool hq_FirstCall = true;
   LARGE_INTEGER u_Counts;
   static float64 hf64_CountsPer1US = 0.0;
   float64 f64_EndTime;

   if (ou32_NumberUs == 0U)
   {
      return;
   }

   if (hq_FirstCall == true)
   {
      bool q_Test;
      hq_FirstCall = false;
      q_Test = (QueryPerformanceFrequency(&u_Counts) == 0) ? false : true;
      tgl_assert(q_Test == true);
      hf64_CountsPer1US = static_cast<float64>(u_Counts.QuadPart) / 1000000.0;
   }
   //lint -e{1960}  //comparison against 0.0 is well-defined; no problems ...
   if (hf64_CountsPer1US == 0.0)
   {
      return;
   }

   (void)QueryPerformanceCounter(&u_Counts);
   f64_EndTime = static_cast<float64>(u_Counts.QuadPart) + (static_cast<float64>(ou32_NumberUs) * hf64_CountsPer1US);
   for (;; )
   {
      (void)QueryPerformanceCounter(&u_Counts);
      if (static_cast<float64>(u_Counts.QuadPart) >= f64_EndTime)
      {
         break;
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get system time in microseconds

   Return elapsed time since system start in micro-seconds as precise as possible.

   \return
   System time in micro-seconds.

   \created     29.09.2008  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get system time in milliseconds

   Return elapsed time since system start in micro-seconds as precise as possible.
   As we return ms this will overflow after 47 days (just like the Win32 GetTickCount)

   \return
   System time in milliseconds.

   \created     05.03.2002  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 stw_tgl::TGL_GetTickCount(void)
{
   static sint64 hs64_Freq;
   static bool hq_FirstCall = true;
   LARGE_INTEGER u_Counts;

   if (hq_FirstCall == true)
   {
      bool q_Test;
      q_Test = (QueryPerformanceFrequency(&u_Counts) == 0) ? false : true;
      tgl_assert(q_Test == true);
      hs64_Freq = u_Counts.QuadPart / 1000U; //we want the result in ms not in seconds
      hq_FirstCall = false;
   }

   (void)QueryPerformanceCounter(&u_Counts);
   return static_cast<uint32>(u_Counts.QuadPart / hs64_Freq);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sleep for a number of milliseconds

   Delay for a number of milliseconds. Thread control shall meanwhile be passed on.
   i.e.: no active, blocking watiting.

   \param[in]    ou32_NumberMs    number of milliseconds to delay

   \created     21.09.2009  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void stw_tgl::TGL_Sleep(const uint32 ou32_NumberMs)
{
   Sleep(ou32_NumberMs);
}
