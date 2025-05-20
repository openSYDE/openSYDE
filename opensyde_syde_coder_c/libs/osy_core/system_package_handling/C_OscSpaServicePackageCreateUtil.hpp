//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Package creation util
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSPASERVICEPACKAGECREATEUTIL_HPP
#define C_OSCSPASERVICEPACKAGECREATEUTIL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscSystemDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSpaServicePackageCreateUtil
{
public:
   static int32_t h_CheckPackagePathParam(const stw::scl::C_SclString & orc_PackagePath,
                                          const stw::scl::C_SclString & orc_UseCase,
                                          const stw::scl::C_SclString & orc_PackageExtension,
                                          const stw::scl::C_SclString & orc_PackageExtensionTmp,
                                          stw::scl::C_SclString & orc_ErrorMessage);
   static void h_GetTempFolderName(const stw::scl::C_SclString & orc_PackagePath,
                                   const stw::scl::C_SclString & orc_TemporaryDirectory,
                                   const stw::scl::C_SclString & orc_UseCase,
                                   const stw::scl::C_SclString & orc_PackageExtension,
                                   const stw::scl::C_SclString & orc_TemporaryPackageExtension,
                                   stw::scl::C_SclString & orc_UsedTempPath);
   static int32_t h_CreateTempFolderAndSubFolders(const stw::scl::C_SclString & orc_PackagePath,
                                                  const stw::scl::C_SclString & orc_TemporaryDirectory,
                                                  const stw::scl::C_SclString & orc_UseCase,
                                                  const stw::scl::C_SclString & orc_PackageExtension,
                                                  const stw::scl::C_SclString & orc_TemporaryPackageExtension,
                                                  const std::vector<stw::scl::C_SclString> & orc_AllStaticSubFolders,
                                                  stw::scl::C_SclString & orc_UsedTempPath,
                                                  stw::scl::C_SclString & orc_ErrorMessage);
   static int32_t h_SaveSystemDefinition(const C_OscSystemDefinition & orc_SystemDefinition,
                                         const stw::scl::C_SclString & orc_SystemDefinitionFileName,
                                         const stw::scl::C_SclString & orc_UseCase,
                                         const stw::scl::C_SclString & orc_UsedTempPath,
                                         const stw::scl::C_SclString & orc_OutFilePrefix,
                                         std::set<stw::scl::C_SclString> & orc_AllCreatedFiles,
                                         stw::scl::C_SclString & orc_ErrorMessage);
   static int32_t h_SaveDeviceDefinitionsAndIni(const C_OscSystemDefinition & orc_SystemDefinition,
                                                const stw::scl::C_SclString & orc_UseCase,
                                                const stw::scl::C_SclString & orc_UsedTempPath,
                                                const stw::scl::C_SclString & orc_OutFilePrefix,
                                                std::set<stw::scl::C_SclString> & orc_AllCreatedFiles,
                                                stw::scl::C_SclString & orc_ErrorMessage);
   static int32_t h_CreateZip(const stw::scl::C_SclString & orc_UseCase, const stw::scl::C_SclString & orc_UsedTempPath,
                              const stw::scl::C_SclString & orc_ZipFilePath,
                              const std::set<stw::scl::C_SclString> & orc_AllCreatedFiles,
                              stw::scl::C_SclString & orc_ErrorMessage);
   static void h_CleanUpTempFolder(const stw::scl::C_SclString & orc_UseCase,
                                   const stw::scl::C_SclString & orc_UsedTempPath, int32_t & ors32_ErrVal,
                                   stw::scl::C_SclString & orc_ErrorMessage);

private:
   static const stw::scl::C_SclString mhc_INI_DEV;

   static int32_t mh_CreateDeviceIniFile(const stw::scl::C_SclString & orc_Path,
                                         const std::set<stw::scl::C_SclString> & orc_DeviceDefinitionPaths,
                                         stw::scl::C_SclString & orc_ErrorMessage);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
