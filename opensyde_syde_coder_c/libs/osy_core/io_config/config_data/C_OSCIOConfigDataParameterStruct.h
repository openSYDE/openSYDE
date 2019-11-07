//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain parameter struct config data for IO config
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOCONFIGDATAPARAMETERSTRUCT_H
#define C_OSCIOCONFIGDATAPARAMETERSTRUCT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCIOConfigDataParameter.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOConfigDataParameterStruct :
   public C_OSCIOConfigDataParameter
{
public:
   C_OSCIOConfigDataParameterStruct();

   std::vector<C_OSCIOConfigDataParameter> c_ParameterElements; ///< Parameter elements (synced with
                                                                // C_OSCIOFileDataBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
