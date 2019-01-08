//-----------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib Target Glue Layer: Time functions

   DiagLib Target Glue Layer module containing time functions:
   - system timestamps
   - active delays

   \implementation
   project     KEFEX
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.07.2009  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef TGLTIMEH
#define TGLTIMEH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#ifndef TGL_PACKAGE
#ifdef __BORLANDC__
#define TGL_PACKAGE __declspec(package)
#else
#define TGL_PACKAGE
#endif
#endif

/* -- Namespace ------------------------------------------------------------ */
namespace stw_tgl
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_TGLDateTime
{
public:
   stw_types::uint16 mu16_Year; ///< absolute year number, e.g. 2017
   stw_types::uint8 mu8_Month;  ///< 1 .. 12
   stw_types::uint8 mu8_Day;    ///< 1 .. 31

   stw_types::uint8 mu8_Hour;           ///< 0 .. 23
   stw_types::uint8 mu8_Minute;         ///< 0 .. 59
   stw_types::uint8 mu8_Second;         ///< 0 .. 59
   stw_types::uint16 mu16_MilliSeconds; ///< 0 .. 999
};

void TGL_PACKAGE TGL_GetDateTimeNow(C_TGLDateTime & orc_DateTime);
void TGL_PACKAGE TGL_DelayUs(const stw_types::uint32 ou32_NumberUs);
void TGL_PACKAGE TGL_Sleep(const stw_types::uint32 ou32_NumberMs);
stw_types::uint32 TGL_PACKAGE TGL_GetTickCount(void);
stw_types::uint64 TGL_PACKAGE TGL_GetTickCountUS(void);

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
