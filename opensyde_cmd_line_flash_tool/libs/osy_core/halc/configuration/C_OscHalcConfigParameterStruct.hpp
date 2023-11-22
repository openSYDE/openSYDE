//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter struct config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGPARAMETERSTRUCT_HPP
#define C_OSCHALCCONFIGPARAMETERSTRUCT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscHalcConfigParameter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigParameterStruct :
   public C_OscHalcConfigParameter
{
public:
   C_OscHalcConfigParameterStruct();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   std::vector<C_OscHalcConfigParameter> c_ParameterElements; ///< Parameter elements (synced with C_OscHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
