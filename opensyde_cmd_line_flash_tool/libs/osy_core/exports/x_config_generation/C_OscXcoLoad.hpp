//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Config Package loading
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCOLOAD_HPP
#define C_OSCXCOLOAD_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscXcoBase.hpp"
#include "C_OscXcoManifest.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXcoLoad :
   public C_OscXcoBase
{
public:
   static int32_t h_ProcessPackage(const stw::scl::C_SclString & orc_PackagePath,
                                   const stw::scl::C_SclString & orc_TargetUnzipPath, C_OscXcoManifest & orc_Manifest,
                                   stw::scl::C_SclString & orc_SystemDefinitionPath,
                                   stw::scl::C_SclString & orc_DeviceDefinitionPath,
                                   stw::scl::C_SclStringList & orc_WarningMessages,
                                   stw::scl::C_SclString & orc_ErrorMessage);

private:
   static const stw::scl::C_SclString mhc_USE_CASE;

   static int32_t mh_CheckParamsToProcessPackage(const stw::scl::C_SclString & orc_PackagePath,
                                                 const stw::scl::C_SclString & orc_TargetUnzipPath);
   static int32_t mh_CheckXcfgFiles(const stw::scl::C_SclString & orc_PackagePath);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
