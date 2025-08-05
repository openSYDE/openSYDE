//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Config Package creation
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCOCREATE_HPP
#define C_OSCXCOCREATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscXcoBase.hpp"
#include "C_OscXcoManifest.hpp"
#include "C_OscSystemDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXcoCreate :
   public C_OscXcoBase
{
public:
   static int32_t h_CreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                  const C_OscSystemDefinition & orc_SystemDefinition,
                                  const C_OscXcoManifest & orc_Manifest,
                                  stw::scl::C_SclStringList & orc_WarningMessages,
                                  stw::scl::C_SclString & orc_ErrorMessage,
                                  const stw::scl::C_SclString & orc_TemporaryDirectory = "");

private:
   static const stw::scl::C_SclString mhc_USE_CASE;

   static int32_t mh_CheckParamsToCreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                                const C_OscSystemDefinition & orc_SystemDefinition,
                                                const C_OscXcoManifest & orc_Manifest);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
