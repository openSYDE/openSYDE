//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Update Package

   \class       stw::opensyde_core::C_OscSuServiceUpdatePackage

   Provides functions to create and unpack service update packages
   in openSYDE

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUSERVICEUPDATEPACKAGEH
#define C_OSCSUSERVICEUPDATEPACKAGEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscSuSequences.hpp"
#include "C_OscSystemDefinition.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------

class C_OscSuServiceUpdatePackage
{
public:
   static int32_t h_CreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                  const C_OscSystemDefinition & orc_SystemDefinition,
                                  const uint32_t ou32_ActiveBusIndex, const std::vector<uint8_t> & orc_ActiveNodes,
                                  const std::vector<uint32_t> & orc_NodesUpdateOrder,
                                  const std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                  stw::scl::C_SclStringList & orc_WarningMessages,
                                  stw::scl::C_SclString & orc_ErrorMessage, const bool oq_SaveInCompatibilityFormat,
                                  const bool oq_SaveAsFile, const stw::scl::C_SclString & orc_TemporaryDirectory = "");

   static int32_t h_ProcessPackage(const stw::scl::C_SclString & orc_PackagePath,
                                   const stw::scl::C_SclString & orc_TargetUnzipPath,
                                   C_OscSystemDefinition & orc_SystemDefinition, uint32_t & oru32_ActiveBusIndex,
                                   std::vector<uint8_t> & orc_ActiveNodes, std::vector<uint32_t> & orc_NodesUpdateOrder,
                                   std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                   stw::scl::C_SclStringList & orc_WarningMessages,
                                   stw::scl::C_SclString & orc_ErrorMessage, const bool oq_IsZip);

   static stw::scl::C_SclString h_GetPackageExtension();

protected:
   ///class for one node in service update package definition file
   /// is used for easy conversion of input parameters to target xml file
   class C_SupDefNodeContent
   {
   public:
      uint8_t u8_Active;
      uint32_t u32_Position;
      std::vector<stw::scl::C_SclString> c_ApplicationFileNames; // with relative path
      std::vector<stw::scl::C_SclString> c_NvmFileNames;         // with relative path
      stw::scl::C_SclString c_PemFile;

      /// Node configuration flags for security state
      bool q_SendSecurityEnabledState;
      bool q_SecurityEnabled;

      /// Node configuration flags for debugger state
      bool q_SendDebuggerEnabledState;
      bool q_DebuggerEnabled;
   };

   ///class to create service update package definition file
   /// is used for easy conversion of input parameters to target xml file
   class C_SupDefContent
   {
   public:
      std::vector<C_SupDefNodeContent> c_Nodes;
      uint32_t u32_ActiveBusIndex;
   };

   static stw::scl::C_SclStringList mhc_WarningMessages; // global warnings e.g. if update position of active node is
                                                         // not available
   static stw::scl::C_SclString mhc_ErrorMessage;        // description of error which caused the service update package
                                                         // to fail

   static int32_t mh_CheckSupFiles(const stw::scl::C_SclString & orc_PackagePath);

   static int32_t mh_CheckParamsToCreatePackage(const stw::scl::C_SclString & orc_PackagePath,
                                                const C_OscSystemDefinition & orc_SystemDefinition,
                                                const uint32_t ou32_ActiveBusIndex,
                                                const std::vector<uint8_t> & orc_ActiveNodes,
                                                const std::vector<uint32_t> & orc_NodesUpdateOrder,
                                                const std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite, const bool oq_SaveAsFile);

   static int32_t mh_CreateUpdatePackageDefFile(const stw::scl::C_SclString & orc_Path,
                                                const C_SupDefContent & orc_SupDefContent);

   static int32_t mh_CreateDeviceIniFile(const stw::scl::C_SclString & orc_Path,
                                         const std::set<stw::scl::C_SclString> & orc_DeviceDefinitionPaths);

   static int32_t mh_SupDefParamAdapter(const C_OscSystemDefinition & orc_SystemDefinition,
                                        const uint32_t ou32_ActiveBusIndex,
                                        const std::vector<uint8_t> & orc_ActiveNodes,
                                        const std::vector<uint32_t> & orc_NodesUpdateOrder,
                                        const std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                        C_OscSuServiceUpdatePackage::C_SupDefContent & orc_SupDefContent);

   static int32_t mh_GetUpdatePositionOfNode(const std::vector<uint32_t> & orc_NodesUpdateOrder,
                                             const uint32_t ou32_NodeForUpdate, uint32_t & oru32_UpdatePosition);

   static int32_t mh_SetNodesUpdateOrder(const std::map<uint32_t,
                                                        uint32_t> & orc_UpdateOrderByNodes,
                                         std::vector<uint32_t> & orc_NodesUpdateOrder);
   static void mh_LoadFilesSection(std::vector<stw::scl::C_SclString> & orc_Files, const uint32_t ou32_NodeCounter,
                                   const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                           uint32_t> & orc_PositionMap,
                                   const stw::scl::C_SclString & orc_TargetUnzipPath,
                                   C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BaseNodeName,
                                   const stw::scl::C_SclString & orc_ElementNodeName);
   static void mh_LoadPemConfigSection(C_OscSuSequences::C_DoFlash & orc_DoFlash, const uint32_t ou32_NodeCounter,
                                       const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                               uint32_t> & orc_PositionMap,
                                       const stw::scl::C_SclString & orc_TargetUnzipPath,
                                       C_OscXmlParserBase & orc_XmlParser);
   static void mh_SaveFiles(const std::vector<stw::scl::C_SclString> & orc_Files, C_OscXmlParserBase & orc_XmlParser,
                            const stw::scl::C_SclString & orc_BaseNodeName,
                            const stw::scl::C_SclString & orc_ElementNodeName);
   static void mh_SavePemConfig(const C_SupDefNodeContent & orc_CurrentNode, C_OscXmlParserBase & orc_XmlParser);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
