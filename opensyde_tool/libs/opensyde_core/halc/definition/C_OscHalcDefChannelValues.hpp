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
#include "C_OscHalcDefStruct.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefChannelValues
{
public:
   C_OscHalcDefChannelValues(void);
   virtual ~C_OscHalcDefChannelValues();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   std::vector<C_OscHalcDefStruct> c_Parameters;   ///< All available parameters for this domain
   std::vector<C_OscHalcDefStruct> c_InputValues;  ///< All available inputs for this domain
   std::vector<C_OscHalcDefStruct> c_OutputValues; ///< All available outputs for this domain
   std::vector<C_OscHalcDefStruct> c_StatusValues; ///< All available status values for this domain
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
