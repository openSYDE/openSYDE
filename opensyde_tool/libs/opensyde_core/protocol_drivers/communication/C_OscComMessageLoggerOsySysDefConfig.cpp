//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Configuration of an openSYDE system definition for C_OscComMessageLogger (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscComMessageLoggerOsySysDefConfig.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     orc_OsySysDef       openSYDE system definition
   \param[in]     ou32_BusIndex       Current bus index of system definition
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscComMessageLoggerOsySysDefConfig::C_OscComMessageLoggerOsySysDefConfig(const C_OscSystemDefinition & orc_OsySysDef,
                                                                           const uint32_t ou32_BusIndex) :
   c_OsySysDef(orc_OsySysDef),
   u32_BusIndex(ou32_BusIndex)
{
}
