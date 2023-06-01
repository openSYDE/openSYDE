//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       DiagLib configuration.

   Contains potentially target specific adaptations for the DiagLib library

   \copyright   Copyright 2009 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CDIAGLLIB_CONFIG_HPP
#define CDIAGLLIB_CONFIG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscUtils.hpp"

/* -- Defines ------------------------------------------------------------------------------------------------------- */

//disable logging functionality of KEFEX protocol driver
//so the dependency from the CCMONProtocol classes is resolved:
#define DIAGLIB_KEFEX_PROTOCOL_NO_LOGGING

//DiagLib needs TGL_LoadStr; we have our own implementation:
#define TGL_LoadStr stw::opensyde_core::C_OscUtils::h_LoadString

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Function Prototypes ------------------------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

#endif
