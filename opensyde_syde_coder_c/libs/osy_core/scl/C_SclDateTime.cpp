//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Date and time class

   ANSI C++ date and time class.
   For details cf. documentation in .h file.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers
#ifdef __BORLANDC__          //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

//lint -estring(829,*ctime*)   //this module is specifically for Windows targets; no trouble with unspecified
// behavior expected
#include <ctime>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclDateTime.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Constructor

   Init with default values:
    1.1.1900  00:00:00
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclDateTime::C_SclDateTime(void) :
   mu16_Year(1900),
   mu16_Month(0),
   mu16_Day(1),
   mu16_Hour(0),
   mu16_Minute(0),
   mu16_Second(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current time and date

   Return the current time and date

   \return
   current time and data
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclDateTime C_SclDateTime::Now(void)
{
   std::time_t s32_Time;
   C_SclDateTime t_DateTime;
   struct std::tm * pt_Time;
   s32_Time = std::time(NULL);

   pt_Time = std::localtime(&s32_Time);
   t_DateTime.mu16_Year   = static_cast<uint16_t>(pt_Time->tm_year + 1900);
   t_DateTime.mu16_Month  = static_cast<uint16_t>(pt_Time->tm_mon + 1);
   t_DateTime.mu16_Day    = static_cast<uint16_t>(pt_Time->tm_mday);
   t_DateTime.mu16_Hour   = static_cast<uint16_t>(pt_Time->tm_hour);
   t_DateTime.mu16_Minute = static_cast<uint16_t>(pt_Time->tm_min);
   t_DateTime.mu16_Second = static_cast<uint16_t>(pt_Time->tm_sec);

   return t_DateTime;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return object data time as string

   Returns the time in the format "hh:mm:ss".

   \return
   time as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclDateTime::TimeToString(void) const
{
   C_SclString c_String;

   c_String.PrintFormatted("%02d:%02d:%02d", mu16_Hour, mu16_Minute, mu16_Second);
   return c_String;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return object data date as string

   Returns the date in the format "dd.mm.yyyy".

   \return
   date as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclDateTime::DateToString(void) const
{
   C_SclString c_String;

   c_String.PrintFormatted("%02d.%02d.%04d", mu16_Day, mu16_Month, mu16_Year);
   return c_String;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return object data date and time as string

   Returns the date and time in the format "dd.mm.yyyy hh:mm:ss".

   \return
   date as string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_SclDateTime::DateTimeToString(void) const
{
   C_SclString c_String;

   c_String.PrintFormatted("%02d.%02d.%04d %02d:%02d:%02d", mu16_Day,  mu16_Month,  mu16_Year,
                           mu16_Hour, mu16_Minute, mu16_Second);
   return c_String;
}

//----------------------------------------------------------------------------------------------------------------------
