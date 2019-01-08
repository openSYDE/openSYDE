//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing the interpreted entries for one element in a parameter set file (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     12.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
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
