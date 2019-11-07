//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter config data for IO config
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOCONFIGDATAPARAMETER_H
#define C_OSCIOCONFIGDATAPARAMETER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCIOFileDataContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOConfigDataParameter
{
public:
   C_OSCIOConfigDataParameter();
   virtual ~C_OSCIOConfigDataParameter();

   C_OSCIOFileDataContent c_Value; ///< Value configured by user for this parameter
   stw_scl::C_SCLString c_Comment; ///< User comment for this parameter
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
