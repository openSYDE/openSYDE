//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Configuration of an openSYDE system definition for C_OscComMessageLogger (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCOMMESSAGELOGGEROSYSYSDEFCONFIG_HPP
#define C_OSCCOMMESSAGELOGGEROSYSYSDEFCONFIG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_OscSystemDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscComMessageLoggerOsySysDefConfig
{
public:
   C_OscComMessageLoggerOsySysDefConfig(const C_OscSystemDefinition & orc_OsySysDef, const uint32_t ou32_BusIndex);

   C_OscSystemDefinition c_OsySysDef;
   uint32_t u32_BusIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
