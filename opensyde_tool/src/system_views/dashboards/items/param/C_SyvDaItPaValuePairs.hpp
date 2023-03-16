//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to model value pairs for parametrization write process (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAVALUEPAIRS_H
#define C_SYVDAITPAVALUEPAIRS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaValuePairs
{
public:
   C_SyvDaItPaValuePairs(void);
   C_SyvDaItPaValuePairs(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Expected, const bool oq_Changed);

   bool q_Changed;
   stw::opensyde_core::C_OscNodeDataPoolContent c_Expected;
   stw::opensyde_core::C_OscNodeDataPoolContent c_Actual;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
