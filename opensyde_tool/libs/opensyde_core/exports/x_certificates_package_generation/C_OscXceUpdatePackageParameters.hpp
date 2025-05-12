//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Xapp update package parameters data
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCEUPDATEPACKAGEPARAMETERS_HPP
#define C_OSCXCEUPDATEPACKAGEPARAMETERS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXceUpdatePackageParameters
{
public:
   C_OscXceUpdatePackageParameters();

   stw::scl::C_SclString c_Password;
   stw::scl::C_SclString c_AuthenticationKeyPath;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
