//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Update Package base

   For details cf. documentation in .h file.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <fstream>
#include <iterator>
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "C_OscSupServiceUpdatePackageV1.hpp"
#include "C_OscSupServiceUpdatePackageBase.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_OscSystemDefinitionFiler.hpp"
#include "C_OscSystemDefinitionFilerV2.hpp"
#include "C_OscDeviceDefinition.hpp"
#include "C_OscDeviceDefinitionFiler.hpp"
#include "C_OscSuSequences.hpp"
#include "TglFile.hpp"
#include "C_SclIniFile.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscUtils.hpp"
#include "C_OscZipFile.hpp"
#include "C_OscAesFile.hpp"
#include "C_OscSupSignatureFiler.hpp"
#include "C_OscSupDefinitionFiler.hpp"
#include "C_OscSecurityPemSecUpdate.hpp"
#include "C_OscSupNodeDefinitionFiler.hpp"
#include "C_OscSecurityEcdsa.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;
using namespace stw::tgl;
using namespace stw::diag_lib;
using namespace std;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const C_SclString C_OscSupServiceUpdatePackageBase::mhc_PACKAGE_EXT = ".syde_sup";
const C_SclString C_OscSupServiceUpdatePackageBase::mhc_PACKAGE_EXT_TMP = ".syde_sup_tmp"; // intermediate directory
                                                                                           // before creating zip
                                                                                           // archive
const C_SclString C_OscSupServiceUpdatePackageBase::mhc_SUP_SYSDEF = "sup_system_definition.syde_sysdef";
const C_SclString C_OscSupServiceUpdatePackageBase::mhc_INI_DEV = "devices.ini";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
stw::scl::C_SclStringList C_OscSupServiceUpdatePackageBase::mhc_WarningMessages; // global warnings e.g. if update
                                                                                 // position
                                                                                 // of
                                                                                 // active node is not available
stw::scl::C_SclString C_OscSupServiceUpdatePackageBase::mhc_ErrorMessage;        // description of error which caused
                                                                                 // the
                                                                                 // service update package to fail

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns fix defined service update package extension.

   \return
   service update package extension
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscSupServiceUpdatePackageBase::h_GetPackageExtension()
{
   return mhc_PACKAGE_EXT;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check common security parameters

   \param[in]  orc_EncryptNodes           Encrypt nodes
   \param[in]  orc_EncryptNodesPassword   Encrypt nodes password
   \param[in]  orc_NodeSignatureKeys      Node signature keys
   \param[in]  ou32_NumNodes              Num nodes
   \param[in]  orc_Mode                   Mode
   \param[in]  orc_Function               Function

   \return
   C_NO_ERR    success
   C_CHECKSUM  size of orc_EncryptNodes does not match system definition
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageBase::mh_CheckCommonSecurityParameters(
   const std::vector<uint8_t> & orc_EncryptNodes, const std::vector<C_SclString> & orc_EncryptNodesPassword,
   const std::vector<std::vector<uint8_t> > & orc_NodeSignatureKeys, const uint32_t ou32_NumNodes,
   const C_SclString & orc_Mode, const stw::scl::C_SclString & orc_Function)
{
   int32_t s32_Return = C_NO_ERR;

   if (orc_EncryptNodes.size() != orc_EncryptNodesPassword.size())
   {
      mhc_ErrorMessage = "The container of " + orc_Mode + "ed nodes and " + orc_Mode +
                         "ed passwords have not the same size.";
      osc_write_log_error(orc_Function, mhc_ErrorMessage);
      s32_Return = C_CHECKSUM;
   }

   if (s32_Return == C_NO_ERR)
   {
      if (((orc_EncryptNodes.size() != 0UL) && (orc_EncryptNodes.size() != 1UL)) &&
          (orc_EncryptNodes.size() != ou32_NumNodes))
      {
         mhc_ErrorMessage = "The container of " + orc_Mode + "ed nodes and nodes have not the same size.";
         osc_write_log_error(orc_Function, mhc_ErrorMessage);
         s32_Return = C_CHECKSUM;
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      if (((orc_NodeSignatureKeys.size() != 0UL) && (orc_NodeSignatureKeys.size() != 1UL)) &&
          (orc_NodeSignatureKeys.size() != ou32_NumNodes))
      {
         mhc_ErrorMessage = "The container of signature keys and nodes have not the same size.";
         osc_write_log_error(orc_Function, mhc_ErrorMessage);
         s32_Return = C_CHECKSUM;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get sydeSUP secure def file names

   \param[in]      orc_SystemDefinition   System definition
   \param[in]      orc_TargetPath         Target path
   \param[in,out]  orc_AbsPath            Abs path
   \param[in,out]  orc_RelPath            Rel path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_GetSydeSecureDefFileNames(const C_OscSystemDefinition & orc_SystemDefinition,
                                                                    const C_SclString & orc_TargetPath,
                                                                    std::vector<C_SclString> & orc_AbsPath,
                                                                    std::vector<C_SclString> & orc_RelPath)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < orc_SystemDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      const C_OscNode & rc_Node = orc_SystemDefinition.c_Nodes[u32_ItNode];
      const C_SclString c_Folder = TglFileIncludeTrailingDelimiter(C_OscUtils::h_NiceifyStringForFileName(
                                                                      rc_Node.c_Properties.c_Name));
      const C_SclString c_File = "secure_update_collection.syde_sucdef";
      const C_SclString c_RelPath = c_File;
      const C_SclString c_AbsPath = orc_TargetPath + c_Folder + c_RelPath;
      orc_AbsPath.push_back(c_AbsPath);
      orc_RelPath.push_back(c_RelPath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get node folder names

   \param[in]      orc_SystemDefinition   System definition
   \param[in]      orc_TargetPath         Target path
   \param[in,out]  orc_AbsPath            Abs path
   \param[in,out]  orc_RelPath            Rel path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_GetNodeFolderNames(const C_OscSystemDefinition & orc_SystemDefinition,
                                                             const C_SclString & orc_TargetPath,
                                                             std::vector<C_SclString> & orc_AbsPath,
                                                             std::vector<C_SclString> & orc_RelPath)
{
   for (uint32_t u32_ItNode = 0UL; u32_ItNode < orc_SystemDefinition.c_Nodes.size(); ++u32_ItNode)
   {
      const C_OscNode & rc_Node = orc_SystemDefinition.c_Nodes[u32_ItNode];
      const C_SclString c_RelFile = C_OscUtils::h_NiceifyStringForFileName(rc_Node.c_Properties.c_Name);
      const C_SclString c_AbsFile = TglFileIncludeTrailingDelimiter(orc_TargetPath + c_RelFile);
      orc_AbsPath.push_back(c_AbsFile);
      orc_RelPath.push_back(c_RelFile);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt encryption parameters

   \param[in]      orc_InEncryptNodes           In encrypt nodes
   \param[in]      orc_InEncryptNodesPassword   In encrypt nodes password
   \param[in]      ou32_NodeCount               Node count
   \param[in,out]  orc_OutEncryptNodes          Out encrypt nodes
   \param[in,out]  orc_OutEncryptNodesPassword  Out encrypt nodes password
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_AdaptEncryptionParameters(const std::vector<uint8_t> & orc_InEncryptNodes,
                                                                    const std::vector<C_SclString> & orc_InEncryptNodesPassword, const uint32_t ou32_NodeCount, std::vector<uint8_t> & orc_OutEncryptNodes,
                                                                    std::vector<C_SclString> & orc_OutEncryptNodesPassword)
{
   orc_OutEncryptNodes = orc_InEncryptNodes;
   orc_OutEncryptNodesPassword = orc_InEncryptNodesPassword;
   if (orc_OutEncryptNodes.size() == 0UL)
   {
      //Default
      orc_OutEncryptNodes.resize(ou32_NodeCount, 0U);
      orc_OutEncryptNodesPassword.resize(ou32_NodeCount, "");
   }
   else if (orc_OutEncryptNodes.size() == 1UL)
   {
      //All same
      orc_OutEncryptNodes.resize(ou32_NodeCount, orc_InEncryptNodes[0U]);
      orc_OutEncryptNodesPassword.resize(ou32_NodeCount, orc_InEncryptNodesPassword[0U]);
   }
   else
   {
      //No change necessary
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt common signature parameters

   \param[in]      orc_InNodeSignatureKeys   In node signature keys
   \param[in]      ou32_NodeCount            Node count
   \param[in,out]  orc_OutNodeSignatureKeys  Out node signature keys
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_AdaptCommonSignatureParameters(
   const std::vector<std::vector<uint8_t> > & orc_InNodeSignatureKeys, const uint32_t ou32_NodeCount,
   std::vector<std::vector<uint8_t> > & orc_OutNodeSignatureKeys)
{
   orc_OutNodeSignatureKeys = orc_InNodeSignatureKeys;
   if (orc_OutNodeSignatureKeys.size() == 0UL)
   {
      //Default
      orc_OutNodeSignatureKeys.resize(ou32_NodeCount, std::vector<uint8_t>());
   }
   else if (orc_OutNodeSignatureKeys.size() == 1UL)
   {
      //All same
      orc_OutNodeSignatureKeys.resize(ou32_NodeCount, orc_OutNodeSignatureKeys[0U]);
   }
   else
   {
      //No change necessary
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calc digest

   \param[in]   orc_SourcePath         Source path
   \param[in]   orc_SupFiles           Sup files
   \param[out]  orau8_Digest           Digest
   \param[in]   oq_PathsAreAbsolute    Paths are absolute

   \return
   STW error codes

   \retval   C_NO_ERR   No err
   \retval   C_RD_WR    File not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageBase::mh_CalcDigest(const C_SclString & orc_SourcePath,
                                                        const std::set<C_SclString> & orc_SupFiles,
                                                        uint8_t (&orau8_Digest)[C_OscSecurityEcdsa::hu32_SHA256_FINAL_LENGTH],
                                                        const bool oq_PathsAreAbsolute)
{
   C_OscSecurityEcdsa c_Signature;

   int32_t s32_Retval = c_Signature.Sha256Init();

   if (s32_Retval == C_NO_ERR)
   {
      for (std::set<C_SclString>::const_iterator c_ItFile = orc_SupFiles.begin();
           (c_ItFile != orc_SupFiles.end()) && (s32_Retval == C_NO_ERR);
           ++c_ItFile)
      {
         const C_SclString c_CompleteFilePath = oq_PathsAreAbsolute ? *c_ItFile : orc_SourcePath + *c_ItFile;
         s32_Retval = mh_AddFileToDigest(c_CompleteFilePath, c_Signature);
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = c_Signature.Sha256GetDigest(orau8_Digest);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add file to digest

   \param[in]      orc_FilePath     File path
   \param[in,out]  orc_Signature    Signature

   \return
   STW error codes

   \retval   C_NO_ERR   No err
   \retval   C_RD_WR    File not found
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageBase::mh_AddFileToDigest(const C_SclString & orc_FilePath,
                                                             C_OscSecurityEcdsa & orc_Signature)
{
   int32_t s32_Retval = C_NO_ERR;
   const uint32_t u32_SECTION_SIZE = 256;

   std::ifstream c_InputFileStream;
   const uint32_t u32_InputFileSize = static_cast<uint32_t>(TglFileSize(orc_FilePath));

   c_InputFileStream.open(orc_FilePath.c_str(), std::ifstream::binary);

   if (c_InputFileStream.is_open() == false)
   {
      s32_Retval = C_RD_WR;
   }
   else
   {
      uint32_t u32_RemainingFileCount = u32_InputFileSize;
      while ((u32_RemainingFileCount / u32_SECTION_SIZE) >= 1UL)
      {
         s32_Retval = mh_AddFileSectionToDigest(c_InputFileStream, orc_Signature, u32_SECTION_SIZE);
         u32_RemainingFileCount = u32_RemainingFileCount - u32_SECTION_SIZE;
      }
      if (s32_Retval == C_NO_ERR)
      {
         if (u32_RemainingFileCount > 0UL)
         {
            s32_Retval = mh_AddFileSectionToDigest(c_InputFileStream, orc_Signature, u32_RemainingFileCount);
         }
      }
      //close file
      c_InputFileStream.close();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add file section to digest

   \param[in,out]  orc_File            File
   \param[in,out]  orc_Signature       Signature
   \param[in]      ou32_SectionLength  Section length

   \return
   STW error codes

   \retval   C_NO_ERR   Everything read
   \retval   C_RD_WR    File could not be read
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupServiceUpdatePackageBase::mh_AddFileSectionToDigest(ifstream & orc_File,
                                                                    C_OscSecurityEcdsa & orc_Signature,
                                                                    const uint32_t ou32_SectionLength)
{
   int32_t s32_Retval = C_NO_ERR;
   //read file content
   bool q_HasFailed;

   std::vector<uint8_t> c_InputData;
   c_InputData.resize(static_cast<size_t>(ou32_SectionLength));
   //lint -e{9176} //no problems as long as charn has the same size as uint8; if not we'd be in deep !"=?& anyway
   orc_File.read(reinterpret_cast<char_t *>(&c_InputData[0]), c_InputData.size());
   //check for error
   q_HasFailed = orc_File.fail();
   if (q_HasFailed == true)
   {
      s32_Retval = C_RD_WR;
   }
   else
   {
      s32_Retval = orc_Signature.Sha256Update(&c_InputData[0], static_cast<uint32_t>(c_InputData.size()));
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_Init()
{
   mhc_WarningMessages.Clear(); // clear old warning messages
   mhc_ErrorMessage = "";       // clear old error message
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get warnings and errors

   \param[in,out]  orc_WarningMessages    Warning messages
   \param[in,out]  orc_ErrorMessage       Error message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_GetWarningsAndErrors(C_SclStringList & orc_WarningMessages,
                                                               C_SclString & orc_ErrorMessage)
{
   orc_WarningMessages = mhc_WarningMessages; // set warning messages for caller
   orc_ErrorMessage = mhc_ErrorMessage;       // set error message for caller
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Digest to string

   \param[in]      orau8_DigestBin  Digest bin
   \param[in,out]  orc_Digest       Digest
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupServiceUpdatePackageBase::mh_DigestToString(
   const uint8_t (&orau8_DigestBin)[C_OscSecurityEcdsa::hu32_SHA256_FINAL_LENGTH], C_SclString & orc_Digest)
{
   orc_Digest = "";
   for (uint32_t u32_ItByte = 0UL; u32_ItByte < C_OscSecurityEcdsa::hu32_SHA256_FINAL_LENGTH; ++u32_ItByte)
   {
      std::stringstream c_Stream;
      c_Stream << std::setw(2) << std::setfill('0') << std::hex << std::uppercase <<
         static_cast<int32_t>(orau8_DigestBin[u32_ItByte]);
      orc_Digest += c_Stream.str();
   }
}
