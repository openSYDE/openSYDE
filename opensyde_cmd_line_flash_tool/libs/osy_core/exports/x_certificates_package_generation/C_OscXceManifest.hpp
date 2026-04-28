//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Xapp certificates package data
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCEMANIFEST_HPP
#define C_OSCXCEMANIFEST_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "C_SclString.hpp"
#include "C_OscXceUpdatePackageParameters.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXceManifest
{
public:
   C_OscXceManifest();

   stw::scl::C_SclString c_CertificatesPath;
   std::vector<C_OscXceUpdatePackageParameters> c_UpdatePackageParameters;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
