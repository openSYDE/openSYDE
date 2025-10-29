//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Header file with constants specific for SYDEflash

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef FLA_CONSTANTS_HPP
#define FLA_CONSTANTS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

// Paths (different from paths in constants.h!)
const QString mc_DLL_PATH_PEAK_FLA = "..\\..\\tool\\STW_dlls\\stwpeak2\\stwpeak2_64.dll";
const QString mc_DLL_PATH_VECTOR_FLA = "..\\..\\tool\\STW_dlls\\stwvec\\stwvec64.dll";

// Path variables for can monitor only
const QString mc_PATH_VARIABLE_SYDE_FLASH_BIN = "%{SYDE_FLASH_BINARY}";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // FLA_CONSTANTS_HPP
