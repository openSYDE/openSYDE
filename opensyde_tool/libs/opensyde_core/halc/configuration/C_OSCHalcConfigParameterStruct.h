//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter struct config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGPARAMETERSTRUCT_H
#define C_OSCHALCCONFIGPARAMETERSTRUCT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcConfigParameter.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigParameterStruct :
   public C_OSCHalcConfigParameter
{
public:
   C_OSCHalcConfigParameterStruct();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   std::vector<C_OSCHalcConfigParameter> c_ParameterElements; ///< Parameter elements (synced with C_OSCHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
