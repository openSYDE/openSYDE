//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Package loading util
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSPASERVICEPACKAGELOADUTIL_HPP
#define C_OSCSPASERVICEPACKAGELOADUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSpaServicePackageLoadUtil
{
public:
   static stw::scl::C_SclString h_GetUnzipPath(const stw::scl::C_SclString & orc_TargetUnzipPath);
   static int32_t h_CheckParamsToProcessZipPackage(const stw::scl::C_SclString & orc_PackagePath,
                                                   const stw::scl::C_SclString & orc_TargetUnzipPath,
                                                   const stw::scl::C_SclString & orc_UseCase,
                                                   stw::scl::C_SclString & orc_ErrorMessage);
   static int32_t h_SearchFilesInPath(const stw::scl::C_SclString & orc_PackagePath,
                                      const std::vector<stw::scl::C_SclString> & orc_NecessaryFiles);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
