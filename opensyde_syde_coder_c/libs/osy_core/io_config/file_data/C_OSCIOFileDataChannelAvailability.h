//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description domain channel use-case availability
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATACHANNELAVAILABILITY_H
#define C_OSCIOFILEDATACHANNELAVAILABILITY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataChannelAvailability
{
public:
   C_OSCIOFileDataChannelAvailability(void);

   stw_types::uint32 u32_ValueIndex;                 ///< Channel index corresponding to this availability
   std::vector<stw_types::uint32> c_DependentValues; ///< Optional other channel indices which are also linked to this
                                                     // setting
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
