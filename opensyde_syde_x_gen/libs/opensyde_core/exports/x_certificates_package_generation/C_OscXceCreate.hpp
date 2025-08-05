//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: X-Certificates Package creation
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCXCECREATE_HPP
#define C_OSCXCECREATE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <set>

#include "stwtypes.hpp"
#include "C_OscXceBase.hpp"
#include "C_OscXceManifest.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscXceCreate :
   public C_OscXceBase
{
public:
   static int32_t h_CreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                  const std::vector<stw::scl::C_SclString> & orc_CertificatesPath,
                                  const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters,
                                  stw::scl::C_SclStringList & orc_WarningMessages,
                                  stw::scl::C_SclString & orc_ErrorMessage,
                                  const stw::scl::C_SclString & orc_TemporaryDirectory = "");

private:
   static const stw::scl::C_SclString mhc_USE_CASE;

   static int32_t mh_CheckParamsToCreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                                const std::vector<stw::scl::C_SclString> & orc_CertificatesPath,
                                                const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters);
   static int32_t mh_CheckFileExists(const stw::scl::C_SclString & orc_Path);
   static C_OscXceManifest mh_CreateManifest(
      const std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters);
   static int32_t mh_PrepareCertFiles(const stw::scl::C_SclString & orc_TmpPath,
                                      const std::vector<stw::scl::C_SclString> & orc_CertificatesPath,
                                      std::vector<C_OscXceUpdatePackageParameters> & orc_UpdatePackageParameters,
                                      std::set<stw::scl::C_SclString> & orc_XcertFiles);
   static stw::scl::C_SclString mh_GenOutFilePathPart(const stw::scl::C_SclString & orc_InPath,
                                                      const stw::scl::C_SclString & orc_TargetFolder);
   static int32_t mh_CopyFile(const stw::scl::C_SclString & orc_InPath, const stw::scl::C_SclString & orc_OutBasePath,
                              const stw::scl::C_SclString & orc_OutFolder, std::map<scl::C_SclString,
                                                                                    bool> & orc_ExistingFiles,
                              std::set<stw::scl::C_SclString> & orc_XcertFiles,
                              stw::scl::C_SclString * const opc_OutFilePath = NULL);
   static stw::scl::C_SclString mh_GetUniqueFileName(const stw::scl::C_SclString & orc_InPath,
                                                     const stw::scl::C_SclString & orc_OutFolder,
                                                     std::map<scl::C_SclString,
                                                              bool> & orc_ExistingFiles);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
