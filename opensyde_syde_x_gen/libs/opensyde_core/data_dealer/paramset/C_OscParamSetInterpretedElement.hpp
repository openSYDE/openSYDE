//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the interpreted entries for one element in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDELEMENT_HPP
#define C_OSCPARAMSETINTERPRETEDELEMENT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class, containing the interpreted entries for one element in a parameter set file
class C_OscParamSetInterpretedElement
{
public:
   C_OscParamSetInterpretedElement(void);

   stw::scl::C_SclString c_Name;        ///< Element name
   C_OscNodeDataPoolContent c_NvmValue; ///< Element parameter value
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
