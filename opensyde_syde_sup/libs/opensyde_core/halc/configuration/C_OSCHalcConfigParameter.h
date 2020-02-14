//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGPARAMETER_H
#define C_OSCHALCCONFIGPARAMETER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCHalcDefContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcConfigParameter
{
public:
   C_OSCHalcConfigParameter();
   virtual ~C_OSCHalcConfigParameter();

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   C_OSCHalcDefContent c_Value;    ///< Value configured by user for this parameter
   stw_scl::C_SCLString c_Comment; ///< User comment for this parameter
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
