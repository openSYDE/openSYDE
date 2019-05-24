//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE: Service Update Package

   \class       stw_opensyde_core::C_OSCSuServiceUpdatePackage

   Provides functions to create and unpack service update packages
   in openSYDE

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUSERVICEUPDATEPACKAGEH
#define C_OSCSUSERVICEUPDATEPACKAGEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"
#include "C_OSCSuSequences.h"
#include "C_OSCSystemDefinition.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------

class C_OSCSuServiceUpdatePackage
{
public:
   C_OSCSuServiceUpdatePackage(void);
   virtual ~C_OSCSuServiceUpdatePackage(void);

   static stw_types::sint32 h_CreatePackage(const stw_scl::C_SCLString & orc_PackagePath,
                                            const C_OSCSystemDefinition & orc_SystemDefinition,
                                            const stw_types::uint32 ou32_ActiveBusIndex,
                                            const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                            const std::vector<stw_types::uint32> & orc_NodesUpdateOrder,
                                            const std::vector<C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                            stw_scl::C_SCLStringList & orc_WarningMessages,
                                            stw_scl::C_SCLString & orc_ErrorMessage,
                                            const bool oq_SaveInCompatibilityFormat = false);

   static stw_types::sint32 h_UnpackPackage(const stw_scl::C_SCLString & orc_PackagePath,
                                            const stw_scl::C_SCLString & orc_TargetUnzipPath,
                                            C_OSCSystemDefinition & orc_SystemDefinition,
                                            stw_types::uint32 & oru32_ActiveBusIndex,
                                            std::vector<stw_types::uint8> & orc_ActiveNodes,
                                            std::vector<stw_types::uint32> & orc_NodesUpdateOrder,
                                            std::vector<C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                            stw_scl::C_SCLStringList & orc_WarningMessages,
                                            stw_scl::C_SCLString & orc_ErrorMessage);

   static stw_scl::C_SCLString h_GetPackageExtension();

protected:
   ///class for one node in service update package definition file
   /// is used for easy conversion of input parameters to target xml file
   class C_SupDefNodeContent
   {
   public:
      stw_types::uint8 u8_Active;
      stw_types::uint32 u32_Position;
      std::vector<stw_scl::C_SCLString> c_ApplicationFileNames; // with relative path
      std::vector<stw_scl::C_SCLString> c_NVMFileNames;         // with relative path
   };

   ///class to create service update package definition file
   /// is used for easy conversion of input parameters to target xml file
   class C_SupDefContent
   {
   public:
      std::vector<C_SupDefNodeContent> c_Nodes;
      stw_types::uint32 u32_ActiveBusIndex;
   };

   static stw_scl::C_SCLStringList mhc_WarningMessages; // global warnings e.g. if update position of active node is
                                                        // not available
   static stw_scl::C_SCLString mhc_ErrorMessage;        // description of error which caused the service update package
                                                        // to fail

   static stw_types::sint32 h_CheckParamsToCreatePackage(const stw_scl::C_SCLString & orc_PackagePath,
                                                         const C_OSCSystemDefinition & orc_SystemDefinition,
                                                         const stw_types::uint32 ou32_ActiveBusIndex,
                                                         const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                                         const std::vector<stw_types::uint32> & orc_NodesUpdateOrder,
                                                         const std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite);

   static stw_types::sint32 h_CopyFile(const stw_scl::C_SCLString & orc_SourceFile,
                                       const stw_scl::C_SCLString & orc_TargetFile);

   static void h_CreateUpdatePackageDefFile(const stw_scl::C_SCLString & orc_Path,
                                            const C_SupDefContent & orc_SupDefContent);

   static stw_types::sint32 h_CreateDeviceIniFile(const stw_scl::C_SCLString & orc_Path,
                                                  const std::set<stw_scl::C_SCLString> & orc_DeviceDefinitionPaths);

   static stw_types::sint32 h_StoreDeviceDefFiles(const stw_scl::C_SCLString & orc_Path,
                                                  const std::set<stw_scl::C_SCLString> & orc_DeviceDefinitionPaths);

   static stw_types::sint32 h_SupDefParamAdapter(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                 const stw_types::uint32 ou32_ActiveBusIndex,
                                                 const std::vector<stw_types::uint8> & orc_ActiveNodes,
                                                 const std::vector<stw_types::uint32> & orc_NodesUpdateOrder,
                                                 const std::vector<C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite, C_OSCSuServiceUpdatePackage::C_SupDefContent & orc_SupDefContent);

   static stw_types::sint32 h_GetUpdatePositionOfNode(const std::vector<stw_types::uint32> & orc_NodesUpdateOrder,
                                                      const stw_types::uint32 ou32_NodeForUpdate,
                                                      stw_types::uint32 & oru32_UpdatePosition);

   static stw_types::sint32 h_SetNodesUpdateOrder(const std::map<stw_types::uint32,
                                                                 stw_types::uint32> & orc_UpdateOrderByNodes,
                                                  std::vector<stw_types::uint32> & orc_NodesUpdateOrder);
   static void h_AppendFilesRelative(std::set<stw_scl::C_SCLString> & orc_Set,
                                     const std::vector<stw_scl::C_SCLString> & orc_Files,
                                     const stw_scl::C_SCLString & orc_BasePath);
   static void h_LoadFilesSection(std::vector<stw_scl::C_SCLString> & orc_Files,
                                  const stw_types::uint32 ou32_NodeCounter, const stw_types::uint32 ou32_UpdatePos,
                                  std::map<stw_types::uint32,
                                           stw_types::uint32> & orc_PositionMap,
                                  const stw_scl::C_SCLString & orc_TargetUnzipPath, C_OSCXMLParserBase & orc_XMLParser,
                                  const stw_scl::C_SCLString & orc_BaseNodeName,
                                  const stw_scl::C_SCLString & orc_ElementNodeName);
   static void h_SaveFiles(const std::vector<stw_scl::C_SCLString> & orc_Files, C_OSCXMLParserBase & orc_XMLParser,
                           const stw_scl::C_SCLString & orc_BaseNodeName,
                           const stw_scl::C_SCLString & orc_ElementNodeName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
