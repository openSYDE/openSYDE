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
#include "C_OscXmlParserLog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscSupSignatureFiler.hpp"
#include "C_OscSupNodeDefinitionFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const uint16_t mu16_FILE_VERSION_1 = 1U;
static const stw::scl::C_SclString mc_FILE_VERSION_1_1 = "0x000101";
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
static const stw::scl::C_SclString mc_SECURITY = "security-config";                               // xml node
static const stw::scl::C_SclString mc_SECURITY_AUTHENTICATION = "secure-authentication";          // xml node
static const stw::scl::C_SclString mc_SECURITY_TRAFFIC_ENCRYPTION = "traffic-encryption-config";  // xml node
static const stw::scl::C_SclString mc_SECURITY_DEBUGGER_CONFIG = "debugger-config";               // xml node
static const stw::scl::C_SclString mc_SECURITY_ENAB_ATTR = "enabled";                             // xml node attribute
static const stw::scl::C_SclString mc_SECURITY_SEND_ATTR = "send";                                // xml node attribute
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

   \param[in]  orc_Files            Files
   \param[in]  orc_Nodes            Nodes
   \param[in]  oq_UseMinorVersion1  Use minor version 1

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_RD_WR    File(s) could not be created
   \retval   C_CONFIG   Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::h_SaveNodes(const std::vector<stw::scl::C_SclString> & orc_Files,
                                                 const std::vector<C_OscSupNodeDefinition> & orc_Nodes,
                                                 const bool oq_UseMinorVersion1)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Nodes.size() == orc_Files.size())
   {
      for (uint32_t u32_Pos = 0; (u32_Pos < orc_Nodes.size()) && (s32_Retval == C_NO_ERR); u32_Pos++)
      {
         if (orc_Nodes[u32_Pos].u8_Active == C_OscSupNodeDefinitionFiler::hu8_ACTIVE_NODE)
         {
            s32_Retval = C_OscSupNodeDefinitionFiler::mh_SaveNode(orc_Files[u32_Pos], orc_Nodes[u32_Pos],
                                                                  oq_UseMinorVersion1);
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
   \param[in]      oq_UseMinorVersion1       Use minor version 1

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
                                                 std::vector<stw::scl::C_SclString> & orc_Signatures,
                                                 const bool oq_UseMinorVersion1)
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
            C_OscXmlParserLog c_XmlParser;
            c_XmlParser.SetLogHeading("Loading secure update collection definition");
            s32_Retval = c_XmlParser.LoadFromFile(orc_Files[u32_NodeCounter]);
            if (s32_Retval == C_NO_ERR)
            {
               // get update position
               const uint32_t u32_UpdatePosition = orc_UpdatePosition[u32_NodeCounter];
               tgl_assert(c_XmlParser.SelectRoot() == mc_ROOT_NAME); // we shall have a valid and
               s32_Retval = mh_CheckFileVersion(oq_UseMinorVersion1, c_XmlParser);
               if (s32_Retval == C_NO_ERR)
               {
                  // compatible update package
                  C_OscSupNodeDefinitionFiler::mh_LoadFilesSection(c_DoFlash.c_FilesToFlash, u32_NodeCounter,
                                                                   u32_UpdatePosition, orc_UpdateOrderByNodes,
                                                                   orc_NodeFoldersAbs[u32_NodeCounter],
                                                                   c_XmlParser, mc_FILES, mc_FILE);
                  C_OscSupNodeDefinitionFiler::mh_LoadFilesSection(c_DoFlash.c_FilesToWriteToNvm, u32_NodeCounter,
                                                                   u32_UpdatePosition, orc_UpdateOrderByNodes,
                                                                   orc_NodeFoldersAbs[u32_NodeCounter],
                                                                   c_XmlParser, mc_PARAM_FILES, mc_PARAM_FILE);
                  if (oq_UseMinorVersion1)
                  {
                     s32_Retval = C_OscSupNodeDefinitionFiler::mh_LoadPemConfigSectionVersion1Minor1(c_DoFlash,
                                                                                                     u32_NodeCounter,
                                                                                                     u32_UpdatePosition,
                                                                                                     orc_UpdateOrderByNodes,
                                                                                                     orc_NodeFoldersAbs[
                                                                                                        u32_NodeCounter],
                                                                                                     c_XmlParser);
                  }
                  else
                  {
                     C_OscSupNodeDefinitionFiler::mh_LoadPemConfigSectionVersion1(c_DoFlash, u32_NodeCounter,
                                                                                  u32_UpdatePosition,
                                                                                  orc_UpdateOrderByNodes,
                                                                                  orc_NodeFoldersAbs[u32_NodeCounter],
                                                                                  c_XmlParser);
                  }
                  if (s32_Retval == C_NO_ERR)
                  {
                     s32_Retval = mh_LoadSignatureFile(orc_NodeFoldersAbs[u32_NodeCounter], c_Signature, c_XmlParser);
                  }
               }
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

   \param[in]  orc_File             File
   \param[in]  orc_Node             Node
   \param[in]  oq_UseMinorVersion1  Use minor version 1

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_RD_WR    File(s) could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::mh_SaveNode(const stw::scl::C_SclString & orc_File,
                                                 const C_OscSupNodeDefinition & orc_Node,
                                                 const bool oq_UseMinorVersion1)
{
   int32_t s32_Result;
   C_OscXmlParser c_XmlParser;

   //Root Node
   c_XmlParser.CreateAndSelectNodeChild(mc_ROOT_NAME);
   //File version
   tgl_assert(c_XmlParser.CreateAndSelectNodeChild(mc_FILE_VERSION) == mc_FILE_VERSION);
   if (oq_UseMinorVersion1)
   {
      c_XmlParser.SetNodeContent(mc_FILE_VERSION_1_1);
   }
   else
   {
      c_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(mu16_FILE_VERSION_1));
   }
   tgl_assert(c_XmlParser.SelectNodeParent() == mc_ROOT_NAME);

   mh_SaveFiles(orc_Node.c_ApplicationFileNames, c_XmlParser, mc_FILES, mc_FILE);
   mh_SaveFiles(orc_Node.c_NvmFileNames, c_XmlParser, mc_PARAM_FILES, mc_PARAM_FILE);
   if (oq_UseMinorVersion1)
   {
      mh_SavePemConfigVersion1Minor1(orc_Node, c_XmlParser);
   }
   else
   {
      mh_SavePemConfigVersion1(orc_Node, c_XmlParser);
   }
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
/*! \brief  Load PEM configuration section from service update package definition file version 1

   \param[in,out]  orc_DoFlash         Node configuration with PEM configuration
   \param[in]      ou32_NodeCounter    Current node index
   \param[in]      ou32_UpdatePos      Current update position
   \param[in,out]  orc_PositionMap     Map for node indices and update positions
   \param[in]      orc_NodeFolderAbs   Path where all the files will be unzipped to
   \param[in,out]  orc_XmlParser       XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_LoadPemConfigSectionVersion1(C_OscSuSequences::C_DoFlash & orc_DoFlash,
                                                                  const uint32_t ou32_NodeCounter,
                                                                  const uint32_t ou32_UpdatePos, std::map<uint32_t,
                                                                                                          uint32_t> & orc_PositionMap, const stw::scl::C_SclString & orc_NodeFolderAbs,
                                                                  C_OscXmlParserBase & orc_XmlParser)
{
   if (orc_XmlParser.SelectNodeChild(mc_PEM_FILE_CONFIG) == mc_PEM_FILE_CONFIG)
   {
      // get PEM file path
      if (orc_XmlParser.SelectNodeChild(mc_PEM_FILE) == mc_PEM_FILE)
      {
         // we have to take care of OS dependent path delimiters for windows '\\'
         const stw::scl::C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_FILE_NAME_ATTR);
         if (c_XmlAttr != "")
         {
            const stw::scl::C_SclString c_FilePath = TglFileIncludeTrailingDelimiter(
               orc_NodeFolderAbs) + c_XmlAttr;
            orc_DoFlash.c_PemFile = c_FilePath;
         }

         tgl_assert(orc_XmlParser.SelectNodeParent() == mc_PEM_FILE_CONFIG);
      }
      orc_DoFlash.q_SendSecureAuthenticationEnabledState =
         orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_SEC_SEND_ATTR, false);
      orc_DoFlash.q_SecureAuthenticationEnabled = orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR,
                                                                                 false);

      orc_DoFlash.q_SendDebuggerEnabledState =
         orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_DEB_SEND_ATTR, false);
      orc_DoFlash.q_DebuggerEnabled = orc_XmlParser.GetAttributeBool(mc_PEM_FILE_CONFIG_DEB_ENAB_ATTR, false);

      mh_DecideNodeRequiresFlashForSecurity(orc_DoFlash, ou32_NodeCounter, ou32_UpdatePos, orc_PositionMap);

      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load PEM configuration section from service update package definition file version 1 minor 1

   \param[in,out]  orc_DoFlash         Node configuration with PEM configuration
   \param[in]      ou32_NodeCounter    Current node index
   \param[in]      ou32_UpdatePos      Current update position
   \param[in,out]  orc_PositionMap     Map for node indices and update positions
   \param[in]      orc_NodeFolderAbs   Path where all the files will be unzipped to
   \param[in,out]  orc_XmlParser       XMLParser for service update package definition file

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_CONFIG   Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::mh_LoadPemConfigSectionVersion1Minor1(C_OscSuSequences::C_DoFlash & orc_DoFlash,
                                                                           const uint32_t ou32_NodeCounter,
                                                                           const uint32_t ou32_UpdatePos,
                                                                           std::map<uint32_t,
                                                                                    uint32_t> & orc_PositionMap,
                                                                           const stw::scl::C_SclString & orc_NodeFolderAbs,
                                                                           C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError(mc_SECURITY);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadPemConfigOption(mc_SECURITY_AUTHENTICATION,
                                          orc_DoFlash.q_SendSecureAuthenticationEnabledState,
                                          orc_DoFlash.q_SecureAuthenticationEnabled, true, orc_XmlParser,
                                          orc_NodeFolderAbs,
                                          &orc_DoFlash.c_PemFile);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadPemConfigOption(mc_SECURITY_TRAFFIC_ENCRYPTION,
                                          orc_DoFlash.q_SendTrafficEncryptionEnabledState,
                                          orc_DoFlash.q_TrafficEncryptionEnabled, false, orc_XmlParser,
                                          orc_NodeFolderAbs);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadPemConfigOption(mc_SECURITY_DEBUGGER_CONFIG, orc_DoFlash.q_SendDebuggerEnabledState,
                                          orc_DoFlash.q_DebuggerEnabled, false, orc_XmlParser, orc_NodeFolderAbs);
   }
   if (s32_Retval == C_NO_ERR)
   {
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
   }
   if (s32_Retval == C_NO_ERR)
   {
      mh_DecideNodeRequiresFlashForSecurity(orc_DoFlash, ou32_NodeCounter, ou32_UpdatePos, orc_PositionMap);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Decide node requires flash for security

   \param[in]      orc_DoFlash         Node configuration with PEM configuration
   \param[in]      ou32_NodeCounter    Current node index
   \param[in]      ou32_UpdatePos      Current update position
   \param[in,out]  orc_PositionMap     Map for node indices and update positions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_DecideNodeRequiresFlashForSecurity(const C_OscSuSequences::C_DoFlash & orc_DoFlash,
                                                                        const uint32_t ou32_NodeCounter,
                                                                        const uint32_t ou32_UpdatePos,
                                                                        std::map<uint32_t, uint32_t> & orc_PositionMap)
{
   if ((orc_DoFlash.c_PemFile != "") || (orc_DoFlash.q_SendSecureAuthenticationEnabledState) ||
       (orc_DoFlash.q_SendTrafficEncryptionEnabledState) || (orc_DoFlash.q_SendDebuggerEnabledState))
   {
      // node has applications to update
      orc_PositionMap.insert(std::pair<uint32_t, uint32_t>(ou32_NodeCounter, ou32_UpdatePos));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load pem config option

   \param[in]      orc_NodeName        Node name
   \param[out]     orq_SendOption      Send option
   \param[out]     orq_EnabledOption   Enabled option
   \param[in]      oq_AddFileOption    Add file option
   \param[in,out]  orc_XmlParser       Xml parser
   \param[in]      orc_NodeFolderAbs   Node folder abs
   \param[in,out]  opc_FileName        File name

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_CONFIG   Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::mh_LoadPemConfigOption(const stw::scl::C_SclString & orc_NodeName,
                                                            bool & orq_SendOption, bool & orq_EnabledOption,
                                                            const bool oq_AddFileOption,
                                                            C_OscXmlParserBase & orc_XmlParser,
                                                            const stw::scl::C_SclString & orc_NodeFolderAbs,
                                                            stw::scl::C_SclString * const opc_FileName)
{
   int32_t s32_Retval = orc_XmlParser.SelectNodeChildError(orc_NodeName);

   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeBoolError(mc_SECURITY_SEND_ATTR, orq_SendOption);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = orc_XmlParser.GetAttributeBoolError(mc_SECURITY_ENAB_ATTR, orq_EnabledOption);
   }
   if (oq_AddFileOption)
   {
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = orc_XmlParser.SelectNodeChildError(mc_PEM_FILE);
      }
      if (s32_Retval == C_NO_ERR)
      {
         tgl_assert(opc_FileName != NULL);
         if (opc_FileName != NULL)
         {
            // we have to take care of OS dependent path delimiters for windows '\\'
            const stw::scl::C_SclString c_XmlAttr = orc_XmlParser.GetAttributeString(mc_FILE_NAME_ATTR);
            if (c_XmlAttr != "")
            {
               const stw::scl::C_SclString c_FilePath = TglFileIncludeTrailingDelimiter(orc_NodeFolderAbs) + c_XmlAttr;
               *opc_FileName = c_FilePath;
            }
            else
            {
               *opc_FileName = "";
            }
         }
         tgl_assert(orc_XmlParser.SelectNodeParent() == orc_NodeName);
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      tgl_assert(orc_XmlParser.SelectNodeParent() == mc_SECURITY);
   }
   return s32_Retval;
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
/*! \brief  Save PEM file configuration in XML version 1

   \param[in]      orc_CurrentNode  Current node configuration with PEM file configuration
   \param[in,out]  orc_XmlParser    XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_SavePemConfigVersion1(const C_OscSupNodeDefinition & orc_CurrentNode,
                                                           C_OscXmlParserBase & orc_XmlParser)
{
   if ((orc_CurrentNode.c_PemFile != "") || (orc_CurrentNode.q_SendSecureAuthenticationEnabledState) ||
       (orc_CurrentNode.q_SendDebuggerEnabledState))
   {
      //Files
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_PEM_FILE_CONFIG) == mc_PEM_FILE_CONFIG);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_SEC_SEND_ATTR,
                                     orc_CurrentNode.q_SendSecureAuthenticationEnabledState);
      orc_XmlParser.SetAttributeBool(mc_PEM_FILE_CONFIG_SEC_ENAB_ATTR, orc_CurrentNode.q_SecureAuthenticationEnabled);
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
/*! \brief  Save PEM file configuration in XML version 1 minor 1

   \param[in]      orc_CurrentNode  Current node configuration with PEM file configuration
   \param[in,out]  orc_XmlParser    XMLParser for service update package definition file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_SavePemConfigVersion1Minor1(const C_OscSupNodeDefinition & orc_CurrentNode,
                                                                 C_OscXmlParserBase & orc_XmlParser)
{
   //Files
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_SECURITY) == mc_SECURITY);
   mh_SavePemConfigOption(mc_SECURITY_AUTHENTICATION, orc_CurrentNode.q_SendSecureAuthenticationEnabledState,
                          orc_CurrentNode.q_SecureAuthenticationEnabled, true, orc_XmlParser,
                          orc_CurrentNode.c_PemFile);
   mh_SavePemConfigOption(mc_SECURITY_TRAFFIC_ENCRYPTION, orc_CurrentNode.q_SendTrafficEncryptionEnabledState,
                          orc_CurrentNode.q_TrafficEncryptionEnabled, false, orc_XmlParser);
   mh_SavePemConfigOption(mc_SECURITY_DEBUGGER_CONFIG, orc_CurrentNode.q_SendDebuggerEnabledState,
                          orc_CurrentNode.q_DebuggerEnabled, false, orc_XmlParser);

   //Return for next node
   tgl_assert(orc_XmlParser.SelectNodeParent() == mc_ROOT_NAME);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save PEM file configuration option

   \param[in]      orc_NodeName        Node name
   \param[in]      oq_SendOption       Send option
   \param[in]      oq_EnabledOption    Enabled option
   \param[in]      oq_AddFileOption    Add file option
   \param[in,out]  orc_XmlParser       Xml parser
   \param[in]      oc_FileName         File name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscSupNodeDefinitionFiler::mh_SavePemConfigOption(const stw::scl::C_SclString & orc_NodeName,
                                                         const bool oq_SendOption, const bool oq_EnabledOption,
                                                         const bool oq_AddFileOption,
                                                         C_OscXmlParserBase & orc_XmlParser,
                                                         const stw::scl::C_SclString oc_FileName)
{
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(orc_NodeName) == orc_NodeName);
   orc_XmlParser.SetAttributeBool(mc_SECURITY_SEND_ATTR, oq_SendOption);
   orc_XmlParser.SetAttributeBool(mc_SECURITY_ENAB_ATTR, oq_EnabledOption);
   if (oq_AddFileOption)
   {
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild(mc_PEM_FILE) == mc_PEM_FILE);
      orc_XmlParser.SetAttributeString(mc_FILE_NAME_ATTR, oc_FileName);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == orc_NodeName);
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == mc_SECURITY);
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check file version

   \param[in]      oq_UseMinorVersion1    Use minor version 1
   \param[in,out]  orc_XmlParser          Xml parser

   \return
   STW error codes

   \retval   C_NO_ERR   No error
   \retval   C_CONFIG   Input invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscSupNodeDefinitionFiler::mh_CheckFileVersion(const bool oq_UseMinorVersion1,
                                                         C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(orc_XmlParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading secure update collection definition",
                             "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading secure update collection definition", "Value of \"file-version\": " +
                            stw::scl::C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (((oq_UseMinorVersion1 == false) && (u16_FileVersion != 1U)) ||
             ((oq_UseMinorVersion1 == true) && (u16_FileVersion != 0x101U)))
         {
            osc_write_log_error("Loading secure update collection definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      orc_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading secure update collection definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}
