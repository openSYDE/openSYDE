//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition domain channel use-case availability
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFCHANNELAVAILABILITY_HPP
#define C_OSCHALCDEFCHANNELAVAILABILITY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefChannelAvailability
{
public:
   C_OscHalcDefChannelAvailability(void);
   virtual ~C_OscHalcDefChannelAvailability();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   uint32_t u32_ValueIndex;                 ///< Channel index corresponding to this availability
   std::vector<uint32_t> c_DependentValues; ///< Optional other channel indices which are also linked to this
   // setting
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
