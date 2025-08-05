//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the interpreted entries for one datapool in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDDATAPOOL_HPP
#define C_OSCPARAMSETINTERPRETEDDATAPOOL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscParamSetDataPoolInfo.hpp"
#include "C_OscParamSetInterpretedList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///container class for handling interpreted parameter set file content
class C_OscParamSetInterpretedDataPool
{
public:
   C_OscParamSetInterpretedDataPool(void);

   C_OscParamSetDataPoolInfo c_DataPoolInfo;
   std::vector<C_OscParamSetInterpretedList> c_Lists; ///< Interpreted lists of this datapool
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
