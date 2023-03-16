//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Date and time class

   \class       stw::scl::C_SclDateTime
   \brief       Date and time class

   ANSI C++ date and time class.
   Aim: provide commonly used date and time functionality while only using plain ANSI C++.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CSCLDATETIMEHPP
#define CSCLDATETIMEHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace scl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
///Handling of date and time information
class C_SclDateTime
{
private:
public:
   C_SclDateTime(void);

   uint16_t mu16_Year;  ///< 1900 .. 20xx
   uint16_t mu16_Month; ///< 1 .. 12
   uint16_t mu16_Day;   ///< 1 .. 31

   uint16_t mu16_Hour;   ///< 0 .. 23
   uint16_t mu16_Minute; ///< 0 .. 59
   uint16_t mu16_Second; ///< 0 .. 59

   static C_SclDateTime Now(void);
   C_SclString TimeToString(void) const;     ///< format: "hh:mm:ss"
   C_SclString DateToString(void) const;     ///< format: "dd.mm.yyyy"
   C_SclString DateTimeToString(void) const; ///< format: "dd.mm.yyyy hh:mm:ss"
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
