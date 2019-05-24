//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the interpreted entries for one element in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDELEMENT_H
#define C_OSCPARAMSETINTERPRETEDELEMENT_H

/* -- Includes ------------------------------------------------------------- */
#include "CSCLString.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

///Data class, containing the interpreted entries for one element in a parameter set file
class C_OSCParamSetInterpretedElement
{
public:
   C_OSCParamSetInterpretedElement(void);

   stw_scl::C_SCLString c_Name;         ///< Element name
   C_OSCNodeDataPoolContent c_NvmValue; ///< Element parameter value
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
