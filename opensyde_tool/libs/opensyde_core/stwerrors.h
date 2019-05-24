//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       STW standard error code collection

   A collection of error codes uses as standardized return values from functions.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWERRORSH
#define STWERRORSH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#ifdef __cplusplus

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

//C++ style constants:
namespace stw_errors
{
const stw_types::sint32 C_NO_ERR      = static_cast<stw_types::sint32>(0);   ///< everything OK
const stw_types::sint32 C_UNKNOWN_ERR = static_cast<stw_types::sint32>(-1);  ///< unexpected problem (e.g. exception)
const stw_types::sint32 C_WARN        = static_cast<stw_types::sint32>(-2);  ///< function executed, but with minor
                                                                             // problems
const stw_types::sint32 C_DEFAULT     = static_cast<stw_types::sint32>(-3);  ///<
const stw_types::sint32 C_BUSY        = static_cast<stw_types::sint32>(-4);  ///< operation not possible (e.g. resource
                                                                             // already in use)
const stw_types::sint32 C_RANGE       = static_cast<stw_types::sint32>(-5);  ///< function parameter(s) out of range
const stw_types::sint32 C_OVERFLOW    = static_cast<stw_types::sint32>(-6);  ///< overflow (e.g. communication buffer)
const stw_types::sint32 C_RD_WR       = static_cast<stw_types::sint32>(-7);  ///< error reading or writing
const stw_types::sint32 C_NOACT       = static_cast<stw_types::sint32>(-8);  ///< function "does not work" (e.g. no
                                                                             // response from communication partner)
const stw_types::sint32 C_COM         = static_cast<stw_types::sint32>(-9);  ///< communication problem
const stw_types::sint32 C_CONFIG      = static_cast<stw_types::sint32>(-10); ///< configuration problem (e.g.
                                                                             // inconsistent parameters or state)
const stw_types::sint32 C_CHECKSUM    = static_cast<stw_types::sint32>(-11); ///< checksum error
const stw_types::sint32 C_TIMEOUT     = static_cast<stw_types::sint32>(-12); ///< timeout error

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
#else
//C style defines:
#define C_NO_ERR      (0)
#define C_UNKNOWN_ERR (-1)
#define C_WARN        (-2)
#define C_DEFAULT     (-3)
#define C_BUSY        (-4)
#define C_RANGE       (-5)
#define C_OVERFLOW    (-6)
#define C_RD_WR       (-7)
#define C_NOACT       (-8)
#define C_COM         (-9)
#define C_CONFIG      (-10)
#define C_CHECKSUM    (-11)
#define C_TIMEOUT     (-12)
#endif

#endif
