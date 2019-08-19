//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing all interpreted entries for one node in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDNODE_H
#define C_OSCPARAMSETINTERPRETEDNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OSCParamSetInterpretedDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class, containing all interpreted entries for one node in a parameter set file
class C_OSCParamSetInterpretedNode
{
public:
   C_OSCParamSetInterpretedNode(void);

   stw_scl::C_SCLString c_Name;                               ///< Node name
   std::vector<C_OSCParamSetInterpretedDataPool> c_DataPools; ///< Interpreted data pools, used by this parameter set
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
