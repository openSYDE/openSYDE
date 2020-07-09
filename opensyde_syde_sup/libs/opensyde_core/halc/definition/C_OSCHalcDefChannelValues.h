//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group channel values
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFCHANNELVALUES_H
#define C_OSCHALCDEFCHANNELVALUES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCHalcDefStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefChannelValues
{
public:
   C_OSCHalcDefChannelValues(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   std::vector<C_OSCHalcDefStruct> c_Parameters;   ///< All available parameters for this domain
   std::vector<C_OSCHalcDefStruct> c_InputValues;  ///< All available inputs for this domain
   std::vector<C_OSCHalcDefStruct> c_OutputValues; ///< All available outputs for this domain
   std::vector<C_OSCHalcDefStruct> c_StatusValues; ///< All available status values for this domain
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
