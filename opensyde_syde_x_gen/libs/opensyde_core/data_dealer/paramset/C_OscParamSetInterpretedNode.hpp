//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing all interpreted entries for one node in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDNODE_HPP
#define C_OSCPARAMSETINTERPRETEDNODE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscParamSetInterpretedDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class, containing all interpreted entries for one node in a parameter set file
class C_OscParamSetInterpretedNode
{
public:
   C_OscParamSetInterpretedNode(void);

   stw::scl::C_SclString c_Name;                              ///< Node name
   std::vector<C_OscParamSetInterpretedDataPool> c_DataPools; ///< Interpreted data pools, used by this parameter set
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
