//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Time functions

   DiagLib Target Glue Layer module containing time functions:
   - system timestamps
   - active delays

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef TGLTIMEHPP
#define TGLTIMEHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#ifndef TGL_PACKAGE
#ifdef __BORLANDC__
#define TGL_PACKAGE __declspec(package)
#else
#define TGL_PACKAGE
#endif
#endif

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace tgl
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_TglDateTime
{
public:
   uint16_t mu16_Year; ///< absolute year number, e.g. 2017
   uint8_t mu8_Month;  ///< 1 .. 12
   uint8_t mu8_Day;    ///< 1 .. 31

   uint8_t mu8_Hour;           ///< 0 .. 23
   uint8_t mu8_Minute;         ///< 0 .. 59
   uint8_t mu8_Second;         ///< 0 .. 59
   uint16_t mu16_MilliSeconds; ///< 0 .. 999
};

void TGL_PACKAGE TglGetDateTimeNow(C_TglDateTime & orc_DateTime);
void TGL_PACKAGE TglDelayUs(const uint32_t ou32_NumberUs);
void TGL_PACKAGE TglSleep(const uint32_t ou32_NumberMs);
uint32_t TGL_PACKAGE TglGetTickCount(void);
uint64_t TGL_PACKAGE TglGetTickCountUs(void);

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
