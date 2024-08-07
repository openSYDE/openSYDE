//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System update package definition filer

   System update package definition filer

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscSupDefinitionFiler.hpp"
#include "C_OscSupNodeDefinitionFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw::scl::C_SclString C_OscSupDefinitionFiler::hc_PACKAGE_UPDATE_DEF = "service_update_package.syde_supdef";
static const uint16_t mu16_FILE_VERSION = 2U;
// XML node names of service update package definition
static const stw::scl::C_SclString mc_FILE_VERSION = "file-version";                // xml node
static const stw::scl::C_SclString mc_BUS_INDEX = "bus-index-client";               // xml node
static const stw::scl::C_SclString mc_ROOT_NAME = "opensyde-updatepack-definition"; // xml root node
static const stw::scl::C_SclString mc_NODES = "nodes";                              // xml node
static const stw::scl::C_SclString mc_NODE = "node";                                // xml node
static const stw::scl::C_SclString mc_NODE_ACTIVE_ATTR = "active";                  // xml node attribute
static const stw::scl::C_SclString mc_NODE_POSITION_ATTR = "position";              // xml node attribute
static const stw::scl::C_SclString mc_NODE_UPDATE = "update_package";               // xml node
static const stw::scl::C_SclString mc_NODE_FILE_ATTR = "file";                      // xml node attribute

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates update package definition file (internal function).

   Version 1.0

   Assumptions:
   * write permission of target folder

   \param[in]  orc_Path             destination path
   \param[in]  orc_SupDefContent    content to write in service update package xml file
   \param[in]  orc_Files            Files

   \return
   C_NO_ERR    success
   C_RD_WR     read/write error (see log file)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupDefinitionFiler::h_CreateUpdatePackageDefFile(const stw::scl::C_SclString & orc_Path,
                                                              const C_OscSupDefinition & orc_SupDefContent,
                                                              const std::vector<stw::scl::C_SclString> & orc_Files)
{
   const stw::scl::C_SclString c_FileName = TglFileIncludeTrailingDelimiter(orc_Path) + hc_PACKAGE_UPDATE_DEF;
   int32_t s32_Result;

   // fill update package definition
   C_OscXmlParser c_XmlParser;

   //Root Node
   c_XmlParser.CreateAndSelectNodeChild(mc_ROOT_NAME);

   //File version
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_FILE_VERSION) == mc_FILE_VERSION);
   c_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mu16_FILE_VERSION));
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   mh_SaveNodes(c_XmlParser, orc_SupDefContent.c_Nodes, orc_Files);

   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_BUS_INDEX) == mc_BUS_INDEX);
   c_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(orc_SupDefContent.u32_ActiveBusIndex));

   //Return
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   // save update package definition file
   s32_Result = c_XmlParser.SaveToFile(c_FileName);
   if (s32_Result != C_NO_ERR)
   {
      s32_Result = C_RD_WR;
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load update package definition file

   \param[in]      orc_TargetUnzipPath    Target unzip path
   \param[in]      oq_IsZip               Is zip
   \param[in]      orc_PackagePath        Package path
   \param[out]     oru32_FileVersion      File version
   \param[in,out]  orc_FilePackagePath    File package path
   \param[out]     oru32_ActiveBusIndex   Active bus index
   \param[in,out]  orc_ActiveNodes        Active nodes
   \param[in,out]  orc_UpdatePosition     Update position
   \param[in,out]  orc_PackageFiles       Package files

   \return
   STW error codes

   \retval   C_NO_ERR   success
   \retval   C_RD_WR    read/write error (see log file)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupDefinitionFiler::h_LoadUpdatePackageDefFile(const stw::scl::C_SclString & orc_TargetUnzipPath,
                                                            const bool oq_IsZip,
                                                            const stw::scl::C_SclString & orc_PackagePath,
                                                            uint32_t & oru32_FileVersion,
                                                            stw::scl::C_SclString &  orc_FilePackagePath,
                                                            uint32_t & oru32_ActiveBusIndex,
                                                            std::vector<uint8_t> & orc_ActiveNodes,
                                                            std::vector<uint32_t> & orc_UpdatePosition,
                                                            std::vector<stw::scl::C_SclString> & orc_PackageFiles)
{
   int32_t s32_Retval;
   C_OscXmlParser c_XmlParser;

   if (oq_IsZip)
   {
      s32_Retval = c_XmlParser.LoadFromFile(orc_TargetUnzipPath + C_OscSupDefinitionFiler::hc_PACKAGE_UPDATE_DEF);
   }
   else
   {
      orc_FilePackagePath = TglFileIncludeTrailingDelimiter(orc_PackagePath);
      s32_Retval = c_XmlParser.LoadFromFile(orc_FilePackagePath + C_OscSupDefinitionFiler::hc_PACKAGE_UPDATE_DEF);
   }

   if (s32_Retval == C_NO_ERR)
   {
      tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and
      // compatible update package

      // file version
      tgl_assert(c_XmlParser.SelectNodeChild(mc_FILE_VERSION) == mc_FILE_VERSION);
      const stw::scl::C_SclString c_FileVersion = c_XmlParser.GetNodeContent();
      oru32_FileVersion = static_cast<uint32_t>(c_FileVersion.ToInt());
      tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and
      // compatible update package

      if (oru32_FileVersion == mu16_FILE_VERSION)
      {
         // active bus index
         tgl_assert(c_XmlParser.SelectNodeChild(mc_BUS_INDEX) == mc_BUS_INDEX);
         const stw::scl::C_SclString c_BusIndex = c_XmlParser.GetNodeContent();
         oru32_ActiveBusIndex = static_cast<uint32_t>(c_BusIndex.ToInt());

         // get active nodes with update positions and files to flash

         tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and
         // compatible update package
         mh_LoadNodes(c_XmlParser, orc_ActiveNodes, orc_UpdatePosition, orc_PackageFiles);
      }
   }
   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_RD_WR;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save nodes

   \param[in,out]  orc_XmlParser    Xml parser
   \param[in]      orc_Nodes        Nodes
   \param[in]      orc_Files        Files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupDefinitionFiler::mh_SaveNodes(C_OscXmlParserBase & orc_XmlParser,
                                           const std::vector<C_OscSupNodeDefinition> & orc_Nodes,
                                           const std::vector<stw::scl::C_SclString> & orc_Files)
{
   //Nodes
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_NODES) == mc_NODES);

   //List nodes
   tgl_assert(orc_Nodes.size() == orc_Files.size());
   if (orc_Nodes.size() == orc_Files.size())
   {
      for (uint32_t u32_Pos = 0; u32_Pos < orc_Nodes.size(); u32_Pos++)
      {
         const C_OscSupNodeDefinition c_CurrentNode = orc_Nodes[u32_Pos];

         //Node
         tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_NODE) == mc_NODE);
         orc_XmlParser.SetAttributeUint32(mc_NODE_ACTIVE_ATTR, static_cast<uint32_t>(c_CurrentNode.u8_Active));

         // active node?
         if (c_CurrentNode.u8_Active == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            // if there are files to update for active node then list files
            if ((c_CurrentNode.c_ApplicationFileNames.size() > 0) ||
                (c_CurrentNode.c_NvmFileNames.size() > 0) ||
                (c_CurrentNode.c_PemFile != ""))
            {
               //Update Position
               orc_XmlParser.SetAttributeUint32(mc_NODE_POSITION_ATTR, c_CurrentNode.u32_Position);
            }
            tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_NODE_UPDATE) == mc_NODE_UPDATE);

            orc_XmlParser.SetAttributeString(mc_NODE_FILE_ATTR, orc_Files[u32_Pos]);

            tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODE);
            tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODES);
         }
         else
         {
            //Return for next node
            tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODES);
         }
      }
   }

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load nodes

   \param[in,out]  orc_XmlParser       Xml parser
   \param[in,out]  orc_ActiveNodes     Active nodes
   \param[in,out]  orc_UpdatePosition  Update position
   \param[in,out]  orc_PackageFiles    Package files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupDefinitionFiler::mh_LoadNodes(C_OscXmlParserBase & orc_XmlParser, std::vector<uint8_t> & orc_ActiveNodes,
                                           std::vector<uint32_t> & orc_UpdatePosition,
                                           std::vector<stw::scl::C_SclString> & orc_PackageFiles)
{
   tgl_assert(orc_XmlParser.SelectNodeChild(mc_NODES) == mc_NODES);

   tgl_assert(orc_XmlParser.SelectNodeChild(mc_NODE) == mc_NODE);

   // go through all nodes
   stw::scl::C_SclString c_SelectedNode;
   do
   {
      // get content of node
      stw::scl::C_SclString c_File;
      uint32_t u32_UpdatePosition = 0U;
      const uint8_t u8_NodeActive = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32(mc_NODE_ACTIVE_ATTR));
      orc_ActiveNodes.push_back(u8_NodeActive);
      if (u8_NodeActive == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
      {
         // get update position
         u32_UpdatePosition  = static_cast<uint8_t>(
            orc_XmlParser.GetAttributeUint32(mc_NODE_POSITION_ATTR));
         tgl_assert(orc_XmlParser.SelectNodeChild(mc_NODE_UPDATE) == mc_NODE_UPDATE);
         c_File = orc_XmlParser.GetAttributeString(mc_NODE_FILE_ATTR);
         tgl_assert(orc_XmlParser.SelectNodeParent() == mc_NODE);
      }
      orc_UpdatePosition.push_back(u32_UpdatePosition);
      orc_PackageFiles.push_back(c_File);
      //Next
      c_SelectedNode = orc_XmlParser.SelectNodeNext(mc_NODE);
   }
   while (c_SelectedNode == mc_NODE);
}
