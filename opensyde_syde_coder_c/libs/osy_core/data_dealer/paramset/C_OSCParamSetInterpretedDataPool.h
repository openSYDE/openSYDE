//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the interpreted entries for one datapool in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDDATAPOOL_H
#define C_OSCPARAMSETINTERPRETEDDATAPOOL_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OSCParamSetDataPoolInfo.h"
#include "C_OSCParamSetInterpretedList.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///container class for handling interpreted parameter set file content
class C_OSCParamSetInterpretedDataPool
{
public:
   C_OSCParamSetInterpretedDataPool(void);

   C_OSCParamSetDataPoolInfo c_DataPoolInfo;
   std::vector<C_OSCParamSetInterpretedList> c_Lists; ///< Interpreted lists of this datapool
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
