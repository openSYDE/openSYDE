//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node update package definition filer

   Node update package definition filer

   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_OscSupSignatureFiler.hpp"
#include "C_OscSupNodeDefinitionFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const uint16_t mu16_FILE_VERSION = 1U;
const uint8_t C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE = 1U;
// XML node names of service update package definition
static const stw::scl::C_SclString mc_ROOT_NAME = "opensyde-secure-update-collection-definition"; // xml root node
static const stw::scl::C_SclString mc_FILE_VERSION = "file-version";                              // xml node
static const stw::scl::C_SclString mc_FILES = "files";                                            // xml node
static const stw::scl::C_SclString mc_FILE = "file";                                              // xml node
static const stw::scl::C_SclString mc_PARAM_FILES = "param-files";                                // xml node
static const stw::scl::C_SclString mc_PARAM_FILE = "param-file";                                  // xml node
static const stw::scl::C_SclString mc_PEM_FILE_CONFIG = "pem-file-config";                        // xml node
static const stw::scl::C_SclString mc_PEM_FILE = "pem-file";                                      // xml node
static const stw::scl::C_SclString mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR = "security-enabled";         // xml node
                                                                                                  // attribute
static const stw::scl::C_SclString mc_PEM_FILE_CONFIG_SEC_SEND_ATTR = "security-send";            // xml node
                                                                                                  // attribute
static const stw::scl::C_SclString mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR = "debugger-enabled";         // xml node attribute
static const stw::scl::C_SclString mc_PEM_FILE_CONFIG_DEB_SEND_ATTR = "debugger-send";            // xml node attribute
static const stw::scl::C_SclString mc_FILE_NAME_ATTR = "name";                                    // xml node attribute
static const stw::scl::C_SclString mc_SIG_FILE = "secure-signature-file";                         // xml node
static const stw::scl::C_SclString mc_SIG_FILE_ATTR = "name";                                     // xml node
                                                                                                  // attribute

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save nodes

   \param[in]  orc_Files   Files
   \param[in]  orc_Nodes   Nodes

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_RD_WR    File(s) could not be created
   \retval   C_CONFIG   Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::h_SaveNodes(const std::vector<stw::scl::C_SclString> & orc_Files,
                                                 const std::vector<C_OscSupNodeDefinition> & orc_Nodes)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Nodes.size() == orc_Files.size())
   {
      for (uint32_t u32_Pos = 0; (u32_Pos < orc_Nodes.size()) && (s32_Retval == C_NO_ERR); u32_Pos++)
      {
         if (orc_Nodes[u32_Pos].u8_Active == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            s32_Retval = C_OscSupNodeDefinitionFiler::mh_SaveNode(orc_Files[u32_Pos], orc_Nodes[u32_Pos]);
         }
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load nodes

   \param[in]      orc_Files                 Files
   \param[in]      orc_NodeFoldersAbs        Target unzip path
   \param[in,out]  orc_ActiveNodes           Active nodes
   \param[in,out]  orc_ApplicationsToWrite   Applications to write
   \param[in,out]  orc_UpdateOrderByNodes    Update order by nodes
   \param[in]      orc_UpdatePosition        Update position
   \param[in,out]  orc_Signatures            Signatures

   \return
   STW error codes

   \retval   C_NO_ERR   success
   \retval   C_RD_WR    read/write error (see log file)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::h_LoadNodes(const std::vector<stw::scl::C_SclString> & orc_Files,
                                                 const std::vector<stw::scl::C_SclString> & orc_NodeFoldersAbs,
                                                 const std::vector<uint8_t> & orc_ActiveNodes,
                                                 std::vector<C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                                                 std::map<uint32_t, uint32_t> & orc_UpdateOrderByNodes,
                                                 const std::vector<uint32_t> & orc_UpdatePosition,
                                                 std::vector<stw::scl::C_SclString> & orc_Signatures)
{
   int32_t s32_Retval = C_NO_ERR;

   tgl_assert((orc_Files.size() == orc_NodeFoldersAbs.size()) &&
              ((orc_Files.size() == orc_ActiveNodes.size()) && (orc_Files.size() == orc_UpdatePosition.size())));
   if ((orc_Files.size() == orc_NodeFoldersAbs.size()) &&
       ((orc_Files.size() == orc_ActiveNodes.size()) && (orc_Files.size() == orc_UpdatePosition.size())))
   {
      // go through all nodes
      for (uint32_t u32_NodeCounter = 0; (u32_NodeCounter < orc_ActiveNodes.size()) && (s32_Retval == C_NO_ERR);
           ++u32_NodeCounter)
      {
         stw::scl::C_SclString c_Signature;
         C_OscSuSequences::C_DoFlash c_DoFlash;
         if (orc_ActiveNodes[u32_NodeCounter] == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            C_OscXmlParser c_XmlParser;
            s32_Retval = c_XmlParser.LoadFromFile(orc_Files[u32_NodeCounter]);
            if (s32_Retval == C_NO_ERR)
            {
               // get update position
               const uint32_t u32_UpdatePosition = orc_UpdatePosition[u32_NodeCounter];
               tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and
               // compatible update package
               C_OscSupNodeDefinitionFiler::mh_LoadFilesSection(c_DoFlash.c_FilesToFlash, u32_NodeCounter,
                                                                u32_UpdatePosition, orc_UpdateOrderByNodes,
                                                                orc_NodeFoldersAbs[u32_NodeCounter],
                                                                c_XmlParser, mc_FILES, mc_FILE);
               C_OscSupNodeDefinitionFiler::mh_LoadFilesSection(c_DoFlash.c_FilesToWriteToNvm, u32_NodeCounter,
                                                                u32_UpdatePosition, orc_UpdateOrderByNodes,
                                                                orc_NodeFoldersAbs[u32_NodeCounter],
                                                                c_XmlParser, mc_PARAM_FILES, mc_PARAM_FILE);
               C_OscSupNodeDefinitionFiler::mh_LoadPemConfigSection(c_DoFlash, u32_NodeCounter,
                                                                    u32_UpdatePosition, orc_UpdateOrderByNodes,
                                                                    orc_NodeFoldersAbs[u32_NodeCounter],
                                                                    c_XmlParser);
               s32_Retval = mh_LoadSignatureFile(orc_NodeFoldersAbs[u32_NodeCounter], c_Signature, c_XmlParser);
            }
         }
         orc_ApplicationsToWrite.push_back(c_DoFlash); // push back in any case even if we have no applications
                                                       // because of an inactive device
         orc_Signatures.push_back(c_Signature);        // same for signature
      }
   }
   if (s32_Retval != C_NO_ERR)
   {
      s32_Retval = C_RD_WR;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node

   \param[in]  orc_File    File
   \param[in]  orc_Node    Node

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_RD_WR    File(s) could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::mh_SaveNode(const stw::scl::C_SclString & orc_File,
                                                 const C_OscSupNodeDefinition & orc_Node)
{
   int32_t s32_Result;
   C_OscXmlParser c_XmlParser;

   //Root Node
   c_XmlParser.CreateAndSelectNodeChild(mc_ROOT_NAME);
   //File version
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_FILE_VERSION) == mc_FILE_VERSION);
   c_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mu16_FILE_VERSION));
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   mh_SaveFiles(orc_Node.c_ApplicationFileNames, c_XmlParser, mc_FILES, mc_FILE);
   mh_SaveFiles(orc_Node.c_NvmFileNames, c_XmlParser, mc_PARAM_FILES, mc_PARAM_FILE);
   mh_SavePemConfig(orc_Node, c_XmlParser);
   mh_SaveSignatureFile(orc_Node, c_XmlParser);

   // save update package definition file
   s32_Result = c_XmlParser.SaveToFile(orc_File);
   if (s32_Result != C_NO_ERR)
   {
      s32_Result = C_RD_WR;
   }
   return s32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load files section from service update package definition file

   \param[in,out]  orc_Files              Files vector to append to
   \param[in]      ou32_NodeCounter       Current node index
   \param[in]      ou32_UpdatePos         Current update position
   \param[in,out]  orc_PositionMap        Map for node indices and update positions
   \param[in]      orc_NodeFolderAbs      Path where all the files will be unzipped to
   \param[in,out]  orc_XmlParser          XMLParser for service update package definition file
   \param[in]      orc_BaseNodeName       XML node name to use on base level
   \param[in]      orc_ElementNodeName    XML node name to use on item level
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_LoadFilesSection(std::vector<stw::scl::C_SclString> & orc_Files,
                                                      const uint32_t ou32_NodeCounter, const uint32_t ou32_UpdatePos,
                                                      std::map<uint32_t, uint32_t> & orc_PositionMap,
                                                      const stw::scl::C_SclString & orc_NodeFolderAbs,
                                                      C_OscXmlParserBase & orc_XmlParser,
                                                      const stw::scl::C_SclString & orc_BaseNodeName,
                                                      const stw::scl::C_SclString & orc_ElementNodeName)
{
   if (orc_XmlParser.SelectNodeChild(orc_BaseNodeName) == orc_BaseNodeName)
   {
      stw::scl::C_SclString c_SelectedNode;
      // node has applications to update
      orc_PositionMap.insert(std::pair<uint32_t, uint32_t>(ou32_NodeCounter, ou32_UpdatePos));
      // get update application paths
      tgl_assert(orc_XmlParser.SelectNodeChild(orc_ElementNodeName) == orc_ElementNodeName);

      // go through all files
      do
      {
         // we have to take care of OS dependent path delimiters for windows '\\'
         const stw::scl::C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_FILE_NAME_ATTR);
         const stw::scl::C_SclString c_FilePath = TglFileIncludeTrailingDelimiter(
            orc_NodeFolderAbs) + c_XmlAttr;
         orc_Files.push_back(c_FilePath);
         c_SelectedNode = orc_XmlParser.SelectNodeNext(orc_ElementNodeName);
      }
      while (c_SelectedNode == orc_ElementNodeName);
      tgl_assert(orc_XmlParser.SelectNodeParent() == orc_BaseNodeName);
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load PEM configuration section from service update package definition file

   \param[in,out]  orc_DoFlash         Node configuration with PEM configuration
   \param[in]      ou32_NodeCounter    Current node index
   \param[in]      ou32_UpdatePos      Current update position
   \param[in,out]  orc_PositionMap     Map for node indices and update positions
   \param[in]      orc_NodeFolderAbs   Path where all the files will be unzipped to
   \param[in,out]  orc_XmlParser       XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_LoadPemConfigSection(C_OscSuSequences::C_DoFlash & orc_DoFlash,
                                                          const uint32_t ou32_NodeCounter,
                                                          const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                                                  uint32_t> & orc_PositionMap, const stw::scl::C_SclString & orc_NodeFolderAbs,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_XmlParser.SelectNodeChild(mc_PEM_FILE_CONFIG) == mc_PEM_FILE_CONFIG)
   {
      // get PEM file path
      tgl_assert(orc_XmlParser.SelectNodeChild(mc_PEM_FILE) == mc_PEM_FILE);

      // we have to take care of OS dependent path delimiters for windows '\\'
      const stw::scl::C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_FILE_NAME_ATTR);
      if (c_XmlAttr != "")
      {
         const stw::scl::C_SclString c_FilePath = TglFileIncludeTrailingDelimiter(
            orc_NodeFolderAbs) + c_XmlAttr;
         orc_DoFlash.c_PemFile = c_FilePath;

         // node has applications to update
         orc_PositionMap.insert(std::pair<uint32_t, uint32_t>(ou32_NodeCounter, ou32_UpdatePos));
      }

      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_PEM_FILE_CONFIG);

      if (orc_DoFlash.c_PemFile != "")
      {
         // Only in case of a PEM file, the states are relevant
         orc_DoFlash.q_SendSecurityEnabledState =
            orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_SEC_SEND_ATTR, false);
         orc_DoFlash.q_SecurityEnabled = orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR, false);

         orc_DoFlash.q_SendDebuggerEnabledState =
            orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_DEB_SEND_ATTR, false);
         orc_DoFlash.q_DebuggerEnabled = orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR, false);
      }

      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save files in XML

   \param[in,out]  orc_Files              Files to save
   \param[in,out]  orc_XmlParser          XMLParser for service update package definition file
   \param[in]      orc_BaseNodeName       XML node name to use on base level
   \param[in]      orc_ElementNodeName    XML node name to use on item level
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_SaveFiles(const std::vector<stw::scl::C_SclString> & orc_Files,
                                               C_OscXmlParserBase & orc_XmlParser,
                                               const stw::scl::C_SclString & orc_BaseNodeName,
                                               const stw::scl::C_SclString & orc_ElementNodeName)
{
   if (orc_Files.size() > 0)
   {
      //Files
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(orc_BaseNodeName) == orc_BaseNodeName);
      for (uint32_t u32_PosFile = 0; u32_PosFile < orc_Files.size(); u32_PosFile++)
      {
         //File
         tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(orc_ElementNodeName) == orc_ElementNodeName);
         orc_XmlParser.SetAttributeString(mc_FILE_NAME_ATTR, orc_Files[u32_PosFile]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == orc_BaseNodeName);
      }
      //Return for next node
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save PEM file configuration in XML

   \param[in,out]  orc_CurrentNode  Current node configuration with PEM file configuration
   \param[in,out]  orc_XmlParser    XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_SavePemConfig(const C_OscSupNodeDefinition & orc_CurrentNode,
                                                   C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_CurrentNode.c_PemFile != "")
   {
      //Files
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_PEM_FILE_CONFIG) == mc_PEM_FILE_CONFIG);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_SEC_SEND_ATTR, orc_CurrentNode.q_SendSecurityEnabledState);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR, orc_CurrentNode.q_SecurityEnabled);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_DEB_SEND_ATTR, orc_CurrentNode.q_SendDebuggerEnabledState);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR, orc_CurrentNode.q_DebuggerEnabled);

      //File
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_PEM_FILE) == mc_PEM_FILE);
      orc_XmlParser.SetAttributeString(mc_FILE_NAME_ATTR, orc_CurrentNode.c_PemFile);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_PEM_FILE_CONFIG);

      //Return for next node
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save signature file

   \param[in]      orc_Node         Node
   \param[in,out]  orc_XmlParser    Xml parser
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_SaveSignatureFile(const C_OscSupNodeDefinition & orc_Node,
                                                       C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_Node.u8_SignaturePresent == hu8_ACTIVE_NODE)
   {
      //File
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_SIG_FILE) == mc_SIG_FILE);
      orc_XmlParser.SetAttributeString(mc_SIG_FILE_ATTR, orc_Node.c_SignatureFile);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load signature file

   \param[in]      orc_NodeFolderAbs   Node folder abs
   \param[in,out]  orc_Signature       Signature
   \param[in,out]  orc_XmlParser       Xml parser

   \return
   STW error codes

   \retval   C_NO_ERR   File read
   \retval   C_RD_WR    File not read
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::mh_LoadSignatureFile(const stw::scl::C_SclString & orc_NodeFolderAbs,
                                                          stw::scl::C_SclString & orc_Signature,
                                                          C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild(mc_SIG_FILE) == mc_SIG_FILE)
   {
      const stw::scl::C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_SIG_FILE_ATTR);
      const stw::scl::C_SclString c_PackagePathTmp = TglFileIncludeTrailingDelimiter(orc_NodeFolderAbs) +
                                                     TglExtractFileName(c_XmlAttr);
      s32_Retval = C_OscSupSignatureFiler::h_LoadSignatureFile(c_PackagePathTmp, orc_Signature);

      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
   return s32_Retval;
}
