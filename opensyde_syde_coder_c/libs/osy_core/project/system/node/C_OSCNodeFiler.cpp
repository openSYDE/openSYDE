//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node reader/writer (V3)

   Load / save node data from / to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "TGLUtils.h"
#include "C_OSCNodeFiler.h"
#include "C_OSCNodeCommFiler.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCNodeDataPoolFiler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCHalcConfigFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node file

   \param[out]  orc_Node      Data storage
   \param[in]   orc_FilePath  File path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::h_LoadNodeFile(C_OSCNode & orc_Node, const C_SCLString & orc_FilePath)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForExistingFile(c_XMLParser, orc_FilePath,
                                                                        "opensyde-node-core-definition");

   //File version
   if (c_XMLParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16 u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16>(c_XMLParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading node definition", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading node definition", "Value of \"file-version\": " +
                            C_SCLString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading node definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (c_XMLParser.SelectNodeChild("node") == "node")
      {
         s32_Retval = C_OSCNodeFiler::h_LoadNode(orc_Node, c_XMLParser, orc_FilePath);
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"node\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_Node         data storage
   \param[in,out]  orc_XMLParser    XML with node active
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::h_LoadNode(C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                  const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval;
   bool q_AutoNvmStartAddressHere;

   orc_Node.Initialize();

   //Check optional auto nvm start address
   q_AutoNvmStartAddressHere = orc_XMLParser.AttributeExists("datapool-auto-nvm-start-address");
   if (q_AutoNvmStartAddressHere == true)
   {
      orc_Node.q_DatapoolAutoNvMStartAddress = orc_XMLParser.GetAttributeBool("datapool-auto-nvm-start-address");
   }

   //Type
   if (orc_XMLParser.SelectNodeChild("type") == "type")
   {
      orc_Node.c_DeviceType = orc_XMLParser.GetNodeContent();
      //Return
      orc_XMLParser.SelectNodeParent(); //back up to node
   }

   s32_Retval = mh_LoadProperties(orc_Node.c_Properties, orc_XMLParser);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadApplications(orc_Node.c_Applications, orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadDataPools(orc_Node, orc_XMLParser, orc_BasePath);

         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_LoadHALC(orc_Node.c_HALCConfig, orc_XMLParser, orc_BasePath);
            //AFTER loading the datapools
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("com-protocols") == "com-protocols"))
            {
               s32_Retval = h_LoadNodeComProtocols(orc_Node.c_ComProtocols, orc_Node.c_DataPools, orc_XMLParser,
                                                   orc_BasePath);
               if (s32_Retval == C_NO_ERR)
               {
                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "node"); //back up to node
               }
            }
            else
            {
               osc_write_log_error("Loading node definition", "Could not find \"com-protocols\" node.");
               s32_Retval = C_CONFIG;
            }
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node

   Save node to XML file

   \param[in]      orc_Node            data storage
   \param[in,out]  orc_FilePath        File path for xml
   \param[in,out]  opc_CreatedFiles    Optional storage for history of all created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::h_SaveNodeFile(const C_OSCNode & orc_Node, const C_SCLString & orc_FilePath,
                                      std::vector<C_SCLString> * const opc_CreatedFiles)
{
   C_OSCXMLParser c_XMLParser;
   sint32 s32_Retval = C_OSCSystemFilerUtil::h_GetParserForNewFile(c_XMLParser, orc_FilePath,
                                                                   "opensyde-node-core-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XMLParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XMLParser.CreateAndSelectNodeChild("node") == "node");
      //node
      s32_Retval = C_OSCNodeFiler::h_SaveNode(orc_Node, c_XMLParser, orc_FilePath, opc_CreatedFiles);
      //Don't forget to save!
      if (c_XMLParser.SaveToFile(orc_FilePath) != C_NO_ERR)
      {
         osc_write_log_error("Saving node definition", "Could not create file for node.");
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]      orc_Node            data storage
   \param[in,out]  orc_XMLParser       XML with node active
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Optional storage for history of all created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::h_SaveNode(const C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                  const stw_scl::C_SCLString & orc_BasePath,
                                  std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval;

   orc_XMLParser.SetAttributeBool("datapool-auto-nvm-start-address", orc_Node.q_DatapoolAutoNvMStartAddress);
   //Type
   orc_XMLParser.CreateNodeChild("type", orc_Node.c_DeviceType);
   mh_SaveProperties(orc_Node.c_Properties, orc_XMLParser);
   mh_SaveApplications(orc_Node.c_Applications, orc_XMLParser);
   orc_XMLParser.CreateAndSelectNodeChild("com-protocols");
   s32_Retval = h_SaveNodeComProtocols(orc_Node.c_ComProtocols, orc_Node.c_DataPools, orc_XMLParser, orc_BasePath,
                                       opc_CreatedFiles);
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
      s32_Retval = mh_SaveDataPools(orc_Node.c_DataPools, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_SaveHALC(orc_Node.c_HALCConfig, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node com protocols

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "com-protocols"
   post-condition: the passed XML parser has the active node set to the same "com-protocols"

   \param[out]     orc_NodeComProtocols   data storage
   \param[in]      orc_NodeDataPools      Loaded datapools
   \param[in,out]  orc_XMLParser          XML with list active
   \param[in]      orc_BasePath           Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::h_LoadNodeComProtocols(std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                              const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                              C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_CurNodeComProtocol;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_NodeComProtocols.reserve(u32_ExpectedSize);
   }

   c_CurNodeComProtocol = orc_XMLParser.SelectNodeChild("com-protocol");

   //Clear
   orc_NodeComProtocols.clear();
   if (c_CurNodeComProtocol == "com-protocol")
   {
      do
      {
         C_OSCCanProtocol c_CurComProtocol;
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_BasePath.IsEmpty())
            {
               s32_Retval = C_OSCNodeCommFiler::h_LoadNodeComProtocol(c_CurComProtocol, orc_XMLParser,
                                                                      orc_NodeDataPools);
            }
            else
            {
               s32_Retval = C_OSCNodeCommFiler::h_LoadNodeComProtocolFile(c_CurComProtocol,
                                                                          C_OSCSystemFilerUtil::h_CombinePaths(
                                                                             orc_BasePath,
                                                                             orc_XMLParser.GetNodeContent()),
                                                                          orc_NodeDataPools);
            }
         }

         //Append
         orc_NodeComProtocols.push_back(c_CurComProtocol);

         //Next
         c_CurNodeComProtocol = orc_XMLParser.SelectNodeNext("com-protocol");
      }
      while (c_CurNodeComProtocol == "com-protocol");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeComProtocols.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected comm protocol count, expected: %i, got %i", u32_ExpectedSize,
                              orc_NodeComProtocols.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node com protocols

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "com-protocols"
   post-condition: the passed XML parser has the active node set to the same "com-protocols"

   \param[in]      orc_NodeComProtocols   data storage
   \param[in]      orc_NodeDataPools      data pools for file name generation
   \param[in,out]  orc_XMLParser          XML with list active
   \param[in]      orc_BasePath           Base path
   \param[in,out]  opc_CreatedFiles       Optional storage for history of all created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::h_SaveNodeComProtocols(const std::vector<C_OSCCanProtocol> & orc_NodeComProtocols,
                                              const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                              C_OSCXMLParserBase & orc_XMLParser,
                                              const stw_scl::C_SCLString & orc_BasePath,
                                              std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_XMLParser.SetAttributeUint32("length", orc_NodeComProtocols.size());
   for (uint32 u32_ItComProtocol = 0; (u32_ItComProtocol < orc_NodeComProtocols.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItComProtocol)
   {
      const C_OSCCanProtocol & rc_CurProtocol = orc_NodeComProtocols[u32_ItComProtocol];
      orc_XMLParser.CreateAndSelectNodeChild("com-protocol");
      //Find matching datapool
      if (rc_CurProtocol.u32_DataPoolIndex < orc_NodeDataPools.size())
      {
         const C_OSCNodeDataPool & rc_CurDatapool = orc_NodeDataPools[rc_CurProtocol.u32_DataPoolIndex];
         if (orc_BasePath.IsEmpty())
         {
            //To string
            C_OSCNodeCommFiler::h_SaveNodeComProtocol(rc_CurProtocol, orc_XMLParser, rc_CurDatapool.c_Name);
         }
         else
         {
            const C_SCLString c_FileName = C_OSCNodeCommFiler::h_GetFileName(rc_CurDatapool.c_Name);
            const C_SCLString c_CombinedFileName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
            //Save comm definition file
            s32_Retval = C_OSCNodeCommFiler::h_SaveNodeComProtocolFile(orc_NodeComProtocols[u32_ItComProtocol],
                                                                       c_CombinedFileName,
                                                                       rc_CurDatapool.c_Name);
            //Set file reference
            orc_XMLParser.SetNodeContent(c_FileName);
            //Store if necessary
            if (opc_CreatedFiles != NULL)
            {
               opc_CreatedFiles->push_back(c_FileName);
            }
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Saving system definition",
                             "Invalid index " + C_SCLString::IntToStr(
                                rc_CurProtocol.u32_DataPoolIndex) + " for comm protocol");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "com-protocols");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get automatically generated folder name

   \param[in]  orc_NodeName   Node name

   \return
   Automatically generated folder name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFiler::h_GetFolderName(const C_SCLString & orc_NodeName)
{
   return "node_" + C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(orc_NodeName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get automatically generated file name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFiler::h_GetFileName(void)
{
   return "node_core.xml";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node properties

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_NodeProperties  data storage
   \param[in,out]  orc_XMLParser       XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_LoadProperties(C_OSCNodeProperties & orc_NodeProperties, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("properties") == "properties")
   {
      //Name
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_NodeProperties.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"name\" node.");
         s32_Retval = C_CONFIG;
      }
      //Comment
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_NodeProperties.c_Comment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      //Diagnostic server
      if ((orc_XMLParser.SelectNodeChild("diagnostic-server") == "diagnostic-server") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval =
            mh_StringToDiagnosticServer(orc_XMLParser.GetNodeContent(), orc_NodeProperties.e_DiagnosticServer);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"diagnostic-server\" node.");
         s32_Retval = C_CONFIG;
      }
      //Flash loader
      if ((orc_XMLParser.SelectNodeChild("flash-loader") == "flash-loader") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = mh_StringToFlashLoader(orc_XMLParser.GetNodeContent(), orc_NodeProperties.e_FlashLoader);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"flash-loader\" node.");
         s32_Retval = C_CONFIG;
      }

      //Communication interfaces
      if ((orc_XMLParser.SelectNodeChild("communication-interfaces") == "communication-interfaces") &&
          (s32_Retval == C_NO_ERR))
      {
         s32_Retval = mh_LoadComInterface(orc_NodeProperties.c_ComInterfaces, orc_XMLParser);
      }

      //openSYDE server settings
      if (orc_XMLParser.SelectNodeChild("open-syde-server-settings") == "open-syde-server-settings")
      {
         if (orc_XMLParser.AttributeExists("max-clients") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxClients =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("max-clients"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxClients = 1;
         }
         if (orc_XMLParser.AttributeExists("max-parallel-transmissions") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("max-parallel-transmissions"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions = 64;
         }
         if (orc_XMLParser.AttributeExists("application-index") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex =
               static_cast<sint16>(orc_XMLParser.GetAttributeSint32("application-index"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex = -1;
         }
         if (orc_XMLParser.AttributeExists("max-tx-message-buffer") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx =
               static_cast<uint16>(orc_XMLParser.GetAttributeUint32("max-tx-message-buffer"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx = 585U;
         }
         if (orc_XMLParser.AttributeExists("max-rx-routing-message-buffer") == true)
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx =
               static_cast<uint16>(orc_XMLParser.GetAttributeUint32("max-rx-routing-message-buffer"));
         }
         else
         {
            orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx = 585U;
         }

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         orc_NodeProperties.c_OpenSYDEServerSettings.Initialize();
      }

      //Flashloader settings
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadStwFlashloaderOptions(orc_NodeProperties.c_STWFlashloaderSettings, orc_XMLParser);
      }

      //Code export settings
      if ((orc_XMLParser.SelectNodeChild("code-export-settings") == "code-export-settings") && (s32_Retval == C_NO_ERR))
      {
         if (orc_XMLParser.SelectNodeChild("scaling-support") == "scaling-support")
         {
            s32_Retval = C_OSCSystemFilerUtil::h_StringToCodeExportScalingType(
               orc_XMLParser.GetNodeContent(), orc_NodeProperties.c_CodeExportSettings.e_ScalingSupport);
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "code-export-settings");
         }
         else
         {
            orc_NodeProperties.c_CodeExportSettings.e_ScalingSupport = C_OSCNodeCodeExportSettings::eFLOAT32;
         }

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
      }
      else
      {
         orc_NodeProperties.c_CodeExportSettings.Initialize();
      }

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"properties\" node.");
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node properties

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]      orc_NodeProperties  data storage
   \param[in,out]  orc_XMLParser       XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFiler::mh_SaveProperties(const C_OSCNodeProperties & orc_NodeProperties,
                                       C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("properties");
   orc_XMLParser.CreateNodeChild("name", orc_NodeProperties.c_Name);
   orc_XMLParser.CreateNodeChild("comment", orc_NodeProperties.c_Comment);
   orc_XMLParser.CreateNodeChild("diagnostic-server",
                                 mh_DiagnosticServerToString(orc_NodeProperties.e_DiagnosticServer));
   orc_XMLParser.CreateNodeChild("flash-loader", mh_FlashLoaderToString(orc_NodeProperties.e_FlashLoader));

   //Com interfaces
   mh_SaveComInterface(orc_NodeProperties.c_ComInterfaces, orc_XMLParser);

   //openSYDE server settings
   orc_XMLParser.CreateAndSelectNodeChild("open-syde-server-settings");
   orc_XMLParser.SetAttributeUint32("max-clients", orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxClients);
   orc_XMLParser.SetAttributeUint32("max-parallel-transmissions",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions);
   orc_XMLParser.SetAttributeSint32("application-index",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex);
   orc_XMLParser.SetAttributeUint32("max-tx-message-buffer",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx);
   orc_XMLParser.SetAttributeUint32("max-rx-routing-message-buffer",
                                    orc_NodeProperties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");

   //Flashloader options
   mh_SaveStwFlashloaderOptions(orc_NodeProperties.c_STWFlashloaderSettings, orc_XMLParser);

   //Code export settings
   orc_XMLParser.CreateAndSelectNodeChild("code-export-settings");
   orc_XMLParser.CreateNodeChild("scaling-support",
                                 C_OSCSystemFilerUtil::h_CodeExportScalingTypeToString(orc_NodeProperties.
                                                                                       c_CodeExportSettings.
                                                                                       e_ScalingSupport));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");

   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node STW flashloader settings

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[out]     orc_StwFlashloaderSettings   data storage
   \param[in,out]  orc_XMLParser                XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_LoadStwFlashloaderOptions(C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                    C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("stw-flashloader-settings") == "stw-flashloader-settings")
   {
      if (orc_XMLParser.SelectNodeChild("reset-message") == "reset-message")
      {
         if (orc_XMLParser.AttributeExists("active") == true)
         {
            orc_StwFlashloaderSettings.q_ResetMessageActive = orc_XMLParser.GetAttributeBool("active");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"active\" attribute in \"reset-message\" node.");
            s32_Retval = C_CONFIG;
         }
         if (orc_XMLParser.AttributeExists("extended") == true)
         {
            orc_StwFlashloaderSettings.q_ResetMessageExtendedId = orc_XMLParser.GetAttributeBool("extended");
         }
         else
         {
            orc_StwFlashloaderSettings.q_ResetMessageExtendedId = false;
         }
         if (orc_XMLParser.AttributeExists("id") == true)
         {
            orc_StwFlashloaderSettings.u32_ResetMessageId = orc_XMLParser.GetAttributeUint32("id");
         }
         else
         {
            orc_StwFlashloaderSettings.u32_ResetMessageId = 0;
         }
         if (orc_XMLParser.AttributeExists("dlc") == true)
         {
            orc_StwFlashloaderSettings.u8_ResetMessageDlc = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("dlc"));
         }
         else
         {
            orc_StwFlashloaderSettings.u8_ResetMessageDlc = 8;
         }
         orc_StwFlashloaderSettings.c_Data.clear();
         if (orc_XMLParser.SelectNodeChild("data-bytes") == "data-bytes")
         {
            C_SCLString c_CurNode = orc_XMLParser.SelectNodeChild("data-byte");
            if (c_CurNode == "data-byte")
            {
               do
               {
                  if (orc_XMLParser.AttributeExists("value") == true)
                  {
                     const uint8 u8_DataByte = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("value"));
                     orc_StwFlashloaderSettings.c_Data.push_back(u8_DataByte);
                  }
                  else
                  {
                     osc_write_log_error("Loading node definition",
                                         "Could not find \"value\" attribute in \"data-byte\" node.");
                     s32_Retval = C_CONFIG;
                  }
                  c_CurNode = orc_XMLParser.SelectNodeNext("data-byte");
               }
               while ((c_CurNode == "data-byte") && (s32_Retval == C_NO_ERR));
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "data-bytes");
            }
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "reset-message");
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "stw-flashloader-settings");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"reset-message\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"stw-flashloader-settings\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node STW flashloader settings

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[in]      orc_StwFlashloaderSettings   data storage
   \param[in,out]  orc_XMLParser                XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFiler::mh_SaveStwFlashloaderOptions(const C_OSCNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                  C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("stw-flashloader-settings");
   orc_XMLParser.CreateAndSelectNodeChild("reset-message");
   orc_XMLParser.SetAttributeBool("active", orc_StwFlashloaderSettings.q_ResetMessageActive);
   orc_XMLParser.SetAttributeBool("extended", orc_StwFlashloaderSettings.q_ResetMessageExtendedId);
   orc_XMLParser.SetAttributeUint32("id", orc_StwFlashloaderSettings.u32_ResetMessageId);
   orc_XMLParser.SetAttributeUint32("dlc", static_cast<uint32>(orc_StwFlashloaderSettings.u8_ResetMessageDlc));
   orc_XMLParser.CreateAndSelectNodeChild("data-bytes");
   for (uint32 u32_ItDataByte = 0; u32_ItDataByte < orc_StwFlashloaderSettings.c_Data.size(); ++u32_ItDataByte)
   {
      orc_XMLParser.CreateAndSelectNodeChild("data-byte");
      orc_XMLParser.SetAttributeUint32("value", static_cast<uint32>(orc_StwFlashloaderSettings.c_Data[u32_ItDataByte]));
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-bytes");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "reset-message");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "stw-flashloader-settings");
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node communication interface settings

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[out]     orc_ComInterfaces      data storage
   \param[in,out]  orc_XMLParser          XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_LoadComInterface(std::vector<C_OSCNodeComInterfaceSettings> & orc_ComInterfaces,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_CurNode = orc_XMLParser.SelectNodeChild("communication-interface");

   if (c_CurNode == "communication-interface")
   {
      do
      {
         C_OSCNodeComInterfaceSettings c_ComInterface;

         c_ComInterface.u8_InterfaceNumber =
            static_cast<uint8>(orc_XMLParser.GetAttributeUint32("interface-number"));
         c_ComInterface.u8_NodeID = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("node-id"));
         c_ComInterface.q_IsUpdateEnabled = orc_XMLParser.GetAttributeBool("update-available");
         c_ComInterface.q_IsRoutingEnabled = orc_XMLParser.GetAttributeBool("routing-available");
         c_ComInterface.q_IsDiagnosisEnabled = orc_XMLParser.GetAttributeBool("diagnosis-available");
         //Type
         if ((orc_XMLParser.SelectNodeChild("type") == "type") && (s32_Retval == C_NO_ERR))
         {
            s32_Retval = C_OSCSystemFilerUtil::h_BusTypeStringToEnum(
               orc_XMLParser.GetNodeContent(), c_ComInterface.e_InterfaceType);
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"communication-interface\".\"type\" node.");
            s32_Retval = C_CONFIG;
         }

         //IP address
         if ((orc_XMLParser.SelectNodeChild("ip-address") == "ip-address") && (s32_Retval == C_NO_ERR))
         {
            c_ComInterface.c_Ip.au8_IpAddress[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte1"));
            c_ComInterface.c_Ip.au8_IpAddress[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte2"));
            c_ComInterface.c_Ip.au8_IpAddress[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte3"));
            c_ComInterface.c_Ip.au8_IpAddress[3] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte4"));
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            c_ComInterface.c_Ip.au8_IpAddress[0] = 0U;
            c_ComInterface.c_Ip.au8_IpAddress[1] = 0U;
            c_ComInterface.c_Ip.au8_IpAddress[2] = 0U;
            c_ComInterface.c_Ip.au8_IpAddress[3] = 0U;
         }

         //net mask
         if ((orc_XMLParser.SelectNodeChild("net-mask") == "net-mask") && (s32_Retval == C_NO_ERR))
         {
            c_ComInterface.c_Ip.au8_NetMask[0] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte1"));
            c_ComInterface.c_Ip.au8_NetMask[1] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte2"));
            c_ComInterface.c_Ip.au8_NetMask[2] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte3"));
            c_ComInterface.c_Ip.au8_NetMask[3] = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte4"));
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            c_ComInterface.c_Ip.au8_NetMask[0] = 0U;
            c_ComInterface.c_Ip.au8_NetMask[1] = 0U;
            c_ComInterface.c_Ip.au8_NetMask[2] = 0U;
            c_ComInterface.c_Ip.au8_NetMask[3] = 0U;
         }

         //default gateway
         if ((orc_XMLParser.SelectNodeChild("default-gateway") == "default-gateway") && (s32_Retval == C_NO_ERR))
         {
            c_ComInterface.c_Ip.au8_DefaultGateway[0] =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte1"));
            c_ComInterface.c_Ip.au8_DefaultGateway[1] =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte2"));
            c_ComInterface.c_Ip.au8_DefaultGateway[2] =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte3"));
            c_ComInterface.c_Ip.au8_DefaultGateway[3] =
               static_cast<uint8>(orc_XMLParser.GetAttributeUint32("byte4"));
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            c_ComInterface.c_Ip.au8_DefaultGateway[0] = 0U;
            c_ComInterface.c_Ip.au8_DefaultGateway[1] = 0U;
            c_ComInterface.c_Ip.au8_DefaultGateway[2] = 0U;
            c_ComInterface.c_Ip.au8_DefaultGateway[3] = 0U;
         }
         //Bus
         if (orc_XMLParser.SelectNodeChild("bus") == "bus")
         {
            c_ComInterface.q_IsBusConnected = orc_XMLParser.GetAttributeBool("connected");
            c_ComInterface.u32_BusIndex = orc_XMLParser.GetAttributeUint32("bus-index");
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"communication-interface\".\"bus\" node.");
            s32_Retval = C_CONFIG;
         }
         orc_ComInterfaces.push_back(c_ComInterface);
         c_CurNode = orc_XMLParser.SelectNodeNext("communication-interface");
      }
      while (c_CurNode == "communication-interface");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interfaces");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node communication interface settings

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[in]      orc_ComInterfaces     data storage
   \param[in,out]  orc_XMLParser         XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFiler::mh_SaveComInterface(const std::vector<C_OSCNodeComInterfaceSettings> & orc_ComInterfaces,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("communication-interfaces");
   for (uint32 u32_ItComInterface = 0; u32_ItComInterface < orc_ComInterfaces.size();
        ++u32_ItComInterface)
   {
      const C_OSCNodeComInterfaceSettings & rc_CurComInterface = orc_ComInterfaces[u32_ItComInterface];

      orc_XMLParser.CreateAndSelectNodeChild("communication-interface");
      orc_XMLParser.SetAttributeUint32("interface-number", static_cast<uint32>(rc_CurComInterface.u8_InterfaceNumber));
      orc_XMLParser.SetAttributeUint32("node-id", static_cast<uint32>(rc_CurComInterface.u8_NodeID));
      orc_XMLParser.SetAttributeBool("update-available", rc_CurComInterface.q_IsUpdateEnabled);
      orc_XMLParser.SetAttributeBool("routing-available", rc_CurComInterface.q_IsRoutingEnabled);
      orc_XMLParser.SetAttributeBool("diagnosis-available", rc_CurComInterface.q_IsDiagnosisEnabled);
      orc_XMLParser.CreateNodeChild("type",
                                    C_OSCSystemFilerUtil::h_BusTypeEnumToString(rc_CurComInterface.e_InterfaceType));
      if (rc_CurComInterface.e_InterfaceType == C_OSCSystemBus::eETHERNET)
      {
         orc_XMLParser.CreateAndSelectNodeChild("ip-address");
         orc_XMLParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_IpAddress[0]);
         orc_XMLParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_IpAddress[1]);
         orc_XMLParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_IpAddress[2]);
         orc_XMLParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_IpAddress[3]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         orc_XMLParser.CreateAndSelectNodeChild("net-mask");
         orc_XMLParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_NetMask[0]);
         orc_XMLParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_NetMask[1]);
         orc_XMLParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_NetMask[2]);
         orc_XMLParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_NetMask[3]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
         orc_XMLParser.CreateAndSelectNodeChild("default-gateway");
         orc_XMLParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_DefaultGateway[0]);
         orc_XMLParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_DefaultGateway[1]);
         orc_XMLParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_DefaultGateway[2]);
         orc_XMLParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_DefaultGateway[3]);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
      }
      //Create this section for compatibility reasons (had content and was required by previous openSYDE versions)
      orc_XMLParser.CreateAndSelectNodeChild("communication-protocol");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
      orc_XMLParser.CreateAndSelectNodeChild("bus");
      orc_XMLParser.SetAttributeBool("connected", rc_CurComInterface.q_IsBusConnected);
      orc_XMLParser.SetAttributeUint32("bus-index", rc_CurComInterface.u32_BusIndex);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interface");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "communication-interfaces");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node applications

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_NodeApplications   data storage
   \param[in,out]  orc_XMLParser          XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_LoadApplications(std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                           C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("applications") == "applications")
   {
      C_SCLString c_CurNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_NodeApplications.reserve(u32_ExpectedSize);
      }

      c_CurNode = orc_XMLParser.SelectNodeChild("application");
      if (c_CurNode == "application")
      {
         do
         {
            C_OSCNodeApplication c_CurApplication;

            if (orc_XMLParser.AttributeExists("active") == true)
            {
               c_CurApplication.q_Active = orc_XMLParser.GetAttributeBool("active");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if (orc_XMLParser.AttributeExists("process-id") == true)
            {
               c_CurApplication.u8_ProcessId = static_cast<uint8>(orc_XMLParser.GetAttributeUint32("process-id"));
            }
            else
            {
               c_CurApplication.u8_ProcessId = 0U;
            }

            if (orc_XMLParser.AttributeExists("generated-code-version"))
            {
               c_CurApplication.u16_GenCodeVersion =
                  static_cast<uint16>(orc_XMLParser.GetAttributeUint32("generated-code-version"));
            }
            else
            {
               // probably deprecated project -> default to first version
               c_CurApplication.u16_GenCodeVersion = 1U;
            }

            //Type
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("type") == "type"))
            {
               C_OSCNodeApplication::h_StringToApplication(orc_XMLParser.GetNodeContent(), c_CurApplication.e_Type);
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Name
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("name") == "name"))
            {
               c_CurApplication.c_Name = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            //Comment
            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("comment") == "comment"))
            {
               c_CurApplication.c_Comment = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("project-path") == "project-path"))
            {
               c_CurApplication.c_ProjectPath = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_ProjectPath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("ide-call") == "ide-call"))
            {
               c_CurApplication.c_IDECall = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_IDECall = "";
            }

            if ((s32_Retval == C_NO_ERR) &&
                (orc_XMLParser.SelectNodeChild("code-generator-path") == "code-generator-path"))
            {
               c_CurApplication.c_CodeGeneratorPath = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_CodeGeneratorPath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("generate-path") == "generate-path"))
            {
               c_CurApplication.c_GeneratePath = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_GeneratePath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("result-path") == "result-path"))
            {
               c_CurApplication.c_ResultPaths.resize(1);
               c_CurApplication.c_ResultPaths[0] = orc_XMLParser.GetNodeContent();
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
            }
            else if ((s32_Retval == C_NO_ERR) && (orc_XMLParser.SelectNodeChild("result") == "result"))
            {
               uint32 u32_ExpectedOutputfileNumber = 0UL;
               const bool q_ExpectedSizeForOutputFiles = orc_XMLParser.AttributeExists("length");
               c_CurApplication.c_ResultPaths.clear();

               //Check optional length
               if (q_ExpectedSizeForOutputFiles == true)
               {
                  u32_ExpectedOutputfileNumber = orc_XMLParser.GetAttributeUint32("length");
                  c_CurApplication.c_ResultPaths.reserve(u32_ExpectedOutputfileNumber);
               }

               c_CurNode = orc_XMLParser.SelectNodeChild("output-file");
               if (c_CurNode == "output-file")
               {
                  do
                  {
                     c_CurApplication.c_ResultPaths.push_back(orc_XMLParser.GetNodeContent());
                     c_CurNode = orc_XMLParser.SelectNodeNext("output-file");
                  }
                  while (c_CurNode == "output-file");

                  //Compare length
                  if (q_ExpectedSizeForOutputFiles == true)
                  {
                     if (u32_ExpectedOutputfileNumber != c_CurApplication.c_ResultPaths.size())
                     {
                        C_SCLString c_Tmp;
                        c_Tmp.PrintFormatted("Unexpected output file count, expected: %i, got %i",
                                             u32_ExpectedOutputfileNumber, c_CurApplication.c_ResultPaths.size());
                        osc_write_log_warning("Load file", c_Tmp.c_str());
                     }
                  }

                  //Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "result");
                  // Return
                  tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
               }
               else
               {
                  // At least one output file must be provided in list
                  s32_Retval = C_CONFIG;
               }
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_ResultPaths.resize(1);
               c_CurApplication.c_ResultPaths[0] = "";
            }

            orc_NodeApplications.push_back(c_CurApplication);
            c_CurNode = orc_XMLParser.SelectNodeNext("application");
         }
         while ((c_CurNode == "application") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "applications");
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_NodeApplications.size())
         {
            C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected application count, expected: %i, got %i", u32_ExpectedSize,
                                 orc_NodeApplications.size());
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node applications

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]      orc_NodeApplications   data storage
   \param[in,out]  orc_XMLParser          XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCNodeFiler::mh_SaveApplications(const std::vector<C_OSCNodeApplication> & orc_NodeApplications,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateAndSelectNodeChild("applications");
   orc_XMLParser.SetAttributeUint32("length", orc_NodeApplications.size());
   for (uint32 u32_ItApplication = 0; u32_ItApplication < orc_NodeApplications.size(); ++u32_ItApplication)
   {
      const C_OSCNodeApplication & rc_CurApplication = orc_NodeApplications[u32_ItApplication];
      orc_XMLParser.CreateAndSelectNodeChild("application");
      orc_XMLParser.SetAttributeBool("active", rc_CurApplication.q_Active);
      orc_XMLParser.SetAttributeUint32("process-id", static_cast<uint32>(rc_CurApplication.u8_ProcessId));
      orc_XMLParser.SetAttributeUint32("generated-code-version",
                                       static_cast<uint32>(rc_CurApplication.u16_GenCodeVersion));
      orc_XMLParser.CreateNodeChild("type", C_OSCNodeApplication::h_ApplicationToString(rc_CurApplication.e_Type));
      orc_XMLParser.CreateNodeChild("name", rc_CurApplication.c_Name);
      orc_XMLParser.CreateNodeChild("comment", rc_CurApplication.c_Comment);
      orc_XMLParser.CreateNodeChild("project-path", rc_CurApplication.c_ProjectPath);
      orc_XMLParser.CreateNodeChild("ide-call", rc_CurApplication.c_IDECall);
      orc_XMLParser.CreateNodeChild("code-generator-path", rc_CurApplication.c_CodeGeneratorPath);
      orc_XMLParser.CreateNodeChild("generate-path", rc_CurApplication.c_GeneratePath);
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("result") == "result");
      orc_XMLParser.SetAttributeUint32("length", rc_CurApplication.c_ResultPaths.size());
      for (uint32 u32_ItOutFile = 0UL; u32_ItOutFile < rc_CurApplication.c_ResultPaths.size(); ++u32_ItOutFile)
      {
         orc_XMLParser.CreateNodeChild("output-file", rc_CurApplication.c_ResultPaths[u32_ItOutFile]);
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "application");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "applications");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pools

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_Node         data storage
   \param[in,out]  orc_XMLParser    XML with core active
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_LoadDataPools(C_OSCNode & orc_Node, C_OSCXMLParserBase & orc_XMLParser,
                                        const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear
   orc_Node.c_DataPools.clear();
   if (orc_XMLParser.SelectNodeChild("data-pools") == "data-pools")
   {
      C_SCLString c_CurNode;
      uint32 u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
         orc_Node.c_DataPools.reserve(u32_ExpectedSize);
      }

      c_CurNode = orc_XMLParser.SelectNodeChild("data-pool");
      if (c_CurNode == "data-pool")
      {
         do
         {
            C_OSCNodeDataPool c_CurDataPool;
            if (orc_BasePath.IsEmpty())
            {
               s32_Retval = C_OSCNodeDataPoolFiler::h_LoadDataPool(c_CurDataPool, orc_XMLParser);
            }
            else
            {
               s32_Retval =
                  C_OSCNodeDataPoolFiler::h_LoadDataPoolFile(c_CurDataPool,
                                                             C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                                  orc_XMLParser.
                                                                                                  GetNodeContent()));
            }
            if (s32_Retval != C_NO_ERR)
            {
               break;
            }
            orc_Node.c_DataPools.push_back(c_CurDataPool);
            c_CurNode = orc_XMLParser.SelectNodeNext("data-pool");
         }
         while (c_CurNode == "data-pool");

         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         if (q_ExpectedSizeHere == true)
         {
            if (u32_ExpectedSize != orc_Node.c_DataPools.size())
            {
               C_SCLString c_Tmp;
               c_Tmp.PrintFormatted("Unexpected Datapool count, expected: %i, got %i", u32_ExpectedSize,
                                    orc_Node.c_DataPools.size());
               osc_write_log_warning("Load file", c_Tmp.c_str());
            }
         }
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
      }
   }
   else
   {
      osc_write_log_error("Loading Datapools", "Could not find \"data-pools\" node.");
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node data pools

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[in]      orc_NodeDataPools   data storage
   \param[in,out]  orc_XMLParser       XML with core active
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Optional storage for history of all created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_SaveDataPools(const std::vector<C_OSCNodeDataPool> & orc_NodeDataPools,
                                        C_OSCXMLParserBase & orc_XMLParser, const stw_scl::C_SCLString & orc_BasePath,
                                        std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_XMLParser.CreateAndSelectNodeChild("data-pools");
   orc_XMLParser.SetAttributeUint32("length", orc_NodeDataPools.size());
   for (uint32 u32_ItDataPool = 0; (u32_ItDataPool < orc_NodeDataPools.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItDataPool)
   {
      const C_OSCNodeDataPool & rc_CurDatapool = orc_NodeDataPools[u32_ItDataPool];
      orc_XMLParser.CreateAndSelectNodeChild("data-pool");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         C_OSCNodeDataPoolFiler::h_SaveDataPool(rc_CurDatapool, orc_XMLParser);
      }
      else
      {
         const C_SCLString c_FileName = C_OSCNodeDataPoolFiler::h_GetFileName(rc_CurDatapool.c_Name);
         const C_SCLString c_CombinedFileName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Save datapool file
         s32_Retval = C_OSCNodeDataPoolFiler::h_SaveDataPoolFile(rc_CurDatapool, c_CombinedFileName);
         //Set file reference
         orc_XMLParser.SetNodeContent(c_FileName);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "data-pools");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load HALC

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_LoadHALC(C_OSCHalcConfig & orc_Config, C_OSCXMLParserBase & orc_XMLParser,
                                   const C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   //Clear
   orc_Config.Clear();
   if (orc_XMLParser.SelectNodeChild("halc-file") == "halc-file")
   {
      if (orc_BasePath.IsEmpty())
      {
         //From string
         s32_Retval = C_OSCHalcConfigFiler::h_LoadData(orc_Config, orc_XMLParser, orc_BasePath);
      }
      else
      {
         s32_Retval =
            C_OSCHalcConfigFiler::h_LoadFile(orc_Config,
                                             C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                  orc_XMLParser.
                                                                                  GetNodeContent()), orc_BasePath);
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   else
   {
      //Ignore
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save HALC

   \param[in]      orc_Config          Config
   \param[in,out]  orc_XMLParser       XML parser
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_SaveHALC(const C_OSCHalcConfig & orc_Config, C_OSCXMLParserBase & orc_XMLParser,
                                   const C_SCLString & orc_BasePath, std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval = C_NO_ERR;

   if (!orc_Config.c_FileString.IsEmpty())
   {
      orc_XMLParser.CreateAndSelectNodeChild("halc-file");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         s32_Retval = C_OSCHalcConfigFiler::h_SaveData(orc_Config, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
      }
      else
      {
         // const C_SCLString c_FileName = C_OSCNodeDataPoolFiler::h_GetFileName(rc_CurDatapool.c_Name);
         //Fix
         const C_SCLString c_FileName = "halc.xml";
         const C_SCLString c_CombinedFileName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Save datapool file
         s32_Retval = C_OSCHalcConfigFiler::h_SaveFile(orc_Config, c_CombinedFileName, orc_BasePath, opc_CreatedFiles);
         //Set file reference
         orc_XMLParser.SetNodeContent(c_FileName);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
         }
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "node");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform diagnostic server type to string

   \param[in]  ore_DiagnosticProtocol  Diagnostic protocol type

   \return
   Stringified diagnostic server type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFiler::mh_DiagnosticServerToString(
   const C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_DiagnosticProtocol)
{
   C_SCLString c_Retval;

   switch (ore_DiagnosticProtocol)
   {
   case C_OSCNodeProperties::eDS_OPEN_SYDE:
      c_Retval = "open-syde";
      break;
   case C_OSCNodeProperties::eDS_KEFEX:
      c_Retval = "kefex";
      break;
   case C_OSCNodeProperties::eDS_NONE:
      c_Retval = "none";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to diagnostic server type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Diagnostic server type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_StringToDiagnosticServer(const C_SCLString & orc_String,
                                                   C_OSCNodeProperties::E_DiagnosticServerProtocol & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "kefex")
   {
      ore_Type = C_OSCNodeProperties::eDS_KEFEX;
   }
   else if (orc_String == "open-syde")
   {
      ore_Type = C_OSCNodeProperties::eDS_OPEN_SYDE;
   }
   else if (orc_String == "none")
   {
      ore_Type = C_OSCNodeProperties::eDS_NONE;
   }
   else
   {
      osc_write_log_error("Loading node definition", "Invalid value for \"properties\".\"diagnostic-server\": " +
                          orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform flash loader type to string

   \param[in]  ore_FlashLoader   Flash loader type

   \return
   Stringified flash loader type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCNodeFiler::mh_FlashLoaderToString(const C_OSCNodeProperties::E_FlashLoaderProtocol & ore_FlashLoader)
{
   C_SCLString c_Retval;

   switch (ore_FlashLoader)
   {
   case C_OSCNodeProperties::eFL_OPEN_SYDE:
      c_Retval = "open-syde";
      break;
   case C_OSCNodeProperties::eFL_STW:
      c_Retval = "stw";
      break;
   case C_OSCNodeProperties::eFL_NONE:
      c_Retval = "none";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transform string to flash loader type

   \param[in]   orc_String    String to interpret
   \param[out]  ore_Type      Flash loader type

   \return
   C_NO_ERR   no error
   C_RANGE    String unknown
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCNodeFiler::mh_StringToFlashLoader(const C_SCLString & orc_String,
                                              C_OSCNodeProperties::E_FlashLoaderProtocol & ore_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_String == "stw")
   {
      ore_Type = C_OSCNodeProperties::eFL_STW;
   }
   else if (orc_String == "open-syde")
   {
      ore_Type = C_OSCNodeProperties::eFL_OPEN_SYDE;
   }
   else if (orc_String == "none")
   {
      ore_Type = C_OSCNodeProperties::eFL_NONE;
   }
   else
   {
      osc_write_log_error("Loading node definition",
                          "Invalid value for \"properties\".\"flash-loader\": " + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
