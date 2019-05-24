//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Configuration of a openSYDE system definition for C_OSCComMessageLogger (implementation)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCComMessageLoggerOsySysDefConfig.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in]     orc_OsySysDef       openSYDE system definition
   \param[in]     ou32_BusIndex       Current bus index of system definition
*/
//-----------------------------------------------------------------------------
C_OSCComMessageLoggerOsySysDefConfig::C_OSCComMessageLoggerOsySysDefConfig(const C_OSCSystemDefinition & orc_OsySysDef,
                                                                           const stw_types::uint32 ou32_BusIndex) :
   c_OsySysDef(orc_OsySysDef),
   u32_BusIndex(ou32_BusIndex)
{
}
