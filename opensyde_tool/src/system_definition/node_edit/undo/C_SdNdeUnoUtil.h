//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for undo command utility functions (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEUNOUTIL_H
#define C_SDNDEUNOUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeUnoUtil
{
public:
   static void h_AdaptTargetToDeletedSource(const std::vector<stw_types::uint32> & orc_Source,
                                            std::vector<stw_types::uint32> & orc_Target);

private:
   C_SdNdeUnoUtil();
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
