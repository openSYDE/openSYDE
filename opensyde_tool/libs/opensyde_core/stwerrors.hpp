//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       STW standard error code collection

   A collection of error codes uses as standardized return values from functions.
   To be used for C++ applications.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef STWERRORSHPP
#define STWERRORSHPP

#ifndef __cplusplus
//use sterrors.h for C sources
extern T_stwerrors_hpp_undefined_type gt_stwerrors_hpp_FailBuild;
#endif

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Global Constants ---------------------------------------------------------------------------------------------- */

//C++ style constants:
namespace stw
{
namespace errors
{
const int32_t C_NO_ERR      = static_cast<int32_t>(0);   ///< everything OK
const int32_t C_UNKNOWN_ERR = static_cast<int32_t>(-1);  ///< unexpected problem (e.g. exception)
const int32_t C_WARN        = static_cast<int32_t>(-2);  ///< function executed, but with minor problems
const int32_t C_DEFAULT     = static_cast<int32_t>(-3);  ///<
const int32_t C_BUSY        = static_cast<int32_t>(-4);  ///< operation not possible (e.g. resource already in use)
const int32_t C_RANGE       = static_cast<int32_t>(-5);  ///< function parameter(s) out of range
const int32_t C_OVERFLOW    = static_cast<int32_t>(-6);  ///< overflow (e.g. communication buffer)
const int32_t C_RD_WR       = static_cast<int32_t>(-7);  ///< error reading or writing
const int32_t C_NOACT       = static_cast<int32_t>(-8);  ///< function "does not work" (e.g. no response from
                                                         // communication partner)
const int32_t C_COM         = static_cast<int32_t>(-9);  ///< communication problem
const int32_t C_CONFIG      = static_cast<int32_t>(-10); ///< configuration problem (e.g. inconsistent parameters or
                                                         // state)
const int32_t C_CHECKSUM    = static_cast<int32_t>(-11); ///< checksum error
const int32_t C_TIMEOUT     = static_cast<int32_t>(-12); ///< timeout error

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
