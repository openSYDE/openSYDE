//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Update Package base

   Provides functions to create and unpack service update packages
   in openSYDE

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUPSERVICEUPDATEPACKAGEBASE_H
#define C_OSCSUPSERVICEUPDATEPACKAGEBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscSecurityEcdsa.hpp"
#include "C_OscSupDefinition.hpp"
#include "C_OscSystemDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------

class C_OscSupServiceUpdatePackageBase
{
public:
   static stw::scl::C_SclString h_GetPackageExtension();

protected:
   static const stw::scl::C_SclString mhc_PACKAGE_EXT;
   static const stw::scl::C_SclString mhc_PACKAGE_EXT_TMP; // intermediate directory before creating zip archive
   static const stw::scl::C_SclString mhc_SUP_SYSDEF;
   static const stw::scl::C_SclString mhc_INI_DEV;

   static stw::scl::C_SclStringList mhc_WarningMessages; // global warnings e.g. if update position of active node is
                                                         // not available
   static stw::scl::C_SclString mhc_ErrorMessage;        // description of error which caused the service update package
                                                         // to fail

   static int32_t mh_CheckCommonSecurityParameters(const std::vector<uint8_t> & orc_EncryptNodes,
                                                   const std::vector<stw::scl::C_SclString> & orc_EncryptNodesPassword,
                                                   const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys,
                                                   const uint32_t ou32_NumNodes, const stw::scl::C_SclString & orc_Mode,
                                                   const stw::scl::C_SclString & orc_Function);

   static void mh_GetSydeSecureDefFileNames(const C_OscSystemDefinition & orc_SystemDefinition,
                                            const stw::scl::C_SclString & orc_TargetPath,
                                            std::vector<stw::scl::C_SclString> & orc_AbsPath,
                                            std::vector<stw::scl::C_SclString> & orc_RelPath);
   static void mh_GetNodeFolderNames(const C_OscSystemDefinition & orc_SystemDefinition,
                                     const stw::scl::C_SclString & orc_TargetPath,
                                     std::vector<stw::scl::C_SclString> & orc_AbsPath,
                                     std::vector<stw::scl::C_SclString> & orc_RelPath);
   static void mh_AdaptEncryptionParameters(const std::vector<uint8_t> & orc_InEncryptNodes,
                                            const std::vector<stw::scl::C_SclString> & orc_InEncryptNodesPassword,
                                            const uint32_t ou32_NodeCount, std::vector<uint8_t> & orc_OutEncryptNodes,
                                            std::vector<stw::scl::C_SclString> & orc_OutEncryptNodesPassword);
   static void mh_AdaptCommonSignatureParameters(const std::vector<std::vector<uint8_t> > & orc_InNodeSignatureKeys,
                                                 const uint32_t ou32_NodeCount,
                                                 std::vector<std::vector<uint8_t> > & orc_OutNodeSignatureKeys);
   static int32_t mh_CalcDigest(const stw::scl::C_SclString & orc_SourcePath,
                                const std::set<stw::scl::C_SclString> &orc_SupFiles,
                                uint8_t(&orau8_Digest)[C_OscSecurityEcdsa::hu32_SHA256_FINAL_LENGTH],
                                const bool oq_PathsAreAbsolute);
   static int32_t mh_AddFileToDigest(const stw::scl::C_SclString & orc_FilePath, C_OscSecurityEcdsa & orc_Signature);
   static int32_t mh_AddFileSectionToDigest(std::ifstream & orc_File, C_OscSecurityEcdsa & orc_Signature,
                                            const uint32_t ou32_SectionLength);
   static void mh_Init(void);
   static void mh_GetWarningsAndErrors(stw::scl::C_SclStringList & orc_WarningMessages,
                                       stw::scl::C_SclString & orc_ErrorMessage);
   static void mh_DigestToString(const uint8_t (&orau8_DigestBin)[C_OscSecurityEcdsa::hu32_SHA256_FINAL_LENGTH],
                                 stw::scl::C_SclString & orc_Digest);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
