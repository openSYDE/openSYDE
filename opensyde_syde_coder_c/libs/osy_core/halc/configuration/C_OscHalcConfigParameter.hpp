//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGPARAMETER_HPP
#define C_OSCHALCCONFIGPARAMETER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscHalcDefContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigParameter
{
public:
   C_OscHalcConfigParameter();
   virtual ~C_OscHalcConfigParameter();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   C_OscHalcDefContent c_Value;     ///< Value configured by user for this parameter
   stw::scl::C_SclString c_Comment; ///< User comment for this parameter
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
