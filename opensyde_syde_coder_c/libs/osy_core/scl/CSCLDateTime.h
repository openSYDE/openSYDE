//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Date and time class

   \class       stw_scl::C_SCLDateTime
   \brief       Date and time class
   
   ANSI C++ date and time class.
   Aim: provide commonly used date and time functionality while only using plain ANSI C++.

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CSCLDATETIMEH
#define CSCLDATETIMEH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_scl
{
//maybe this will be a part of a Borland library:
#ifndef SCL_PACKAGE
#ifdef __BORLANDC__
#define SCL_PACKAGE __declspec(package)
#else
#define SCL_PACKAGE
#endif
#endif

/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
///Handling of date and time information
class SCL_PACKAGE C_SCLDateTime
{
private:
public:
   C_SCLDateTime(void);

   stw_types::uint16 mu16_Year;  ///< 1900 .. 20xx
   stw_types::uint16 mu16_Month; ///< 1 .. 12
   stw_types::uint16 mu16_Day;   ///< 1 .. 31

   stw_types::uint16 mu16_Hour;   ///< 0 .. 23
   stw_types::uint16 mu16_Minute; ///< 0 .. 59
   stw_types::uint16 mu16_Second; ///< 0 .. 59

   static C_SCLDateTime Now(void);
   C_SCLString TimeToString(void) const;     ///< format: "hh:mm:ss"
   C_SCLString DateToString(void) const;     ///< format: "dd.mm.yyyy"
   C_SCLString DateTimeToString(void) const; ///< format: "dd.mm.yyyy hh:mm:ss"
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
