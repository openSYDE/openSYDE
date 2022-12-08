//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the interpreted entries for one list in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDLIST_H
#define C_OSCPARAMSETINTERPRETEDLIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_OscParamSetInterpretedElement.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Data class, containing the interpreted entries for one list in a parameter set file
class C_OscParamSetInterpretedList
{
public:
   C_OscParamSetInterpretedList(void);

   stw::scl::C_SclString c_Name;                            ///< List name
   std::vector<C_OscParamSetInterpretedElement> c_Elements; ///< Interpreted elements of this list
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
