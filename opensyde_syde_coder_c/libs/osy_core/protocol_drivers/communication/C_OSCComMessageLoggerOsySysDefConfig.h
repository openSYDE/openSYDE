//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Configuration of a openSYDE system definition for C_OSCComMessageLogger (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGEROSYSYSDEFCONFIG_H
#define C_OSCCOMMESSAGELOGGEROSYSYSDEFCONFIG_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"

#include "C_OSCSystemDefinition.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCComMessageLoggerOsySysDefConfig
{
public:
   C_OSCComMessageLoggerOsySysDefConfig(const C_OSCSystemDefinition & orc_OsySysDef,
                                        const stw_types::uint32 ou32_BusIndex);

   C_OSCSystemDefinition c_OsySysDef;
   stw_types::uint32 u32_BusIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
