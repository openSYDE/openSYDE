//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node reader/writer (V3)

   Load / save node data from / to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_SclString.hpp"
#include "TglUtils.hpp"
#include "C_OscNodeFiler.hpp"
#include "C_OscNodeCommFiler.hpp"
#include "C_OscSystemFilerUtil.hpp"
#include "C_OscNodeDataPoolFiler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscHalcConfigFiler.hpp"
#include "C_OscDataLoggerJobFiler.hpp"
#include "C_OscCanOpenManagerFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node file

   \param[out]  orc_Node         Data storage
   \param[in]   orc_FilePath     File path
   \param[in]   oq_SkipContent   (Optional parameter) skip content when not needed (datapools, halc etc.)
                                 (default = false)

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::h_LoadNodeFile(C_OscNode & orc_Node, const C_SclString & orc_FilePath,
                                       const bool oq_SkipContent)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForExistingFile(c_XmlParser, orc_FilePath,
                                                                         "opensyde-node-core-definition");

   //File version
   if (c_XmlParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16_t u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16_t>(c_XmlParser.GetNodeContent().ToInt());
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
                            C_SclString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != 1U)
         {
            osc_write_log_error("Loading node definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      c_XmlParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading node definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (c_XmlParser.SelectNodeChild("node") == "node")
      {
         s32_Retval = C_OscNodeFiler::h_LoadNode(orc_Node, c_XmlParser, orc_FilePath, oq_SkipContent);
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
   \param[in,out]  orc_XmlParser    XML with node active
   \param[in]      orc_BasePath     Base path
   \param[in]      oq_SkipContent   (Optional parameter) skip content when not needed (datapools, halc etc.)
                                    (default = false)

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::h_LoadNode(C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                   const stw::scl::C_SclString & orc_BasePath, const bool oq_SkipContent)
{
   int32_t s32_Retval;
   bool q_AutoNvmStartAddressHere;

   orc_Node.Initialize();

   //Check optional auto nvm start address
   q_AutoNvmStartAddressHere = orc_XmlParser.AttributeExists("datapool-auto-nvm-start-address");
   if (q_AutoNvmStartAddressHere == true)
   {
      orc_Node.q_DatapoolAutoNvmStartAddress = orc_XmlParser.GetAttributeBool("datapool-auto-nvm-start-address");
   }

   //Type
   if (orc_XmlParser.SelectNodeChild("type") == "type")
   {
      orc_Node.c_DeviceType = orc_XmlParser.GetNodeContent();
      //Return
      orc_XmlParser.SelectNodeParent(); //back up to node
   }

   s32_Retval = mh_LoadProperties(orc_Node.c_Properties, orc_XmlParser);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadApplications(orc_Node.c_Applications, orc_XmlParser);
      if (s32_Retval == C_NO_ERR)
      {
         if (oq_SkipContent == false)
         {
            s32_Retval = mh_LoadDataPools(orc_Node, orc_XmlParser, orc_BasePath);

            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = mh_LoadHalc(orc_Node.c_HalcConfig, orc_XmlParser, orc_BasePath);
               //AFTER loading the datapools
               if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("com-protocols") == "com-protocols"))
               {
                  s32_Retval = h_LoadNodeComProtocols(orc_Node.c_ComProtocols, orc_Node.c_DataPools, orc_XmlParser,
                                                      orc_BasePath);
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Return
                     tgl_assert(orc_XmlParser.SelectNodeParent() == "node"); //back up to node
                     s32_Retval = mh_LoadCanOpenManagers(orc_Node.c_CanOpenManagers, orc_XmlParser, orc_BasePath);
                     if (s32_Retval == C_NO_ERR)
                     {
                        s32_Retval = mh_LoadDataLoggers(orc_Node.c_DataLoggerJobs, orc_XmlParser, orc_BasePath);
                     }
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
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node

   Save node to XML file

   \param[in]      orc_Node                  data storage
   \param[in,out]  orc_FilePath              File path for xml
   \param[in,out]  opc_CreatedFiles          Optional storage for history of all created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::h_SaveNodeFile(const C_OscNode & orc_Node, const C_SclString & orc_FilePath,
                                       std::vector<C_SclString> * const opc_CreatedFiles, const std::map<uint32_t,
                                                                                                         C_SclString> & orc_NodeIndicesToNameMap)
{
   C_OscXmlParser c_XmlParser;
   int32_t s32_Retval = C_OscSystemFilerUtil::h_GetParserForNewFile(c_XmlParser, orc_FilePath,
                                                                    "opensyde-node-core-definition");

   if (s32_Retval == C_NO_ERR)
   {
      //Version
      c_XmlParser.CreateNodeChild("file-version", "1");
      tgl_assert(c_XmlParser.CreateAndSelectNodeChild("node") == "node");
      //node
      s32_Retval = C_OscNodeFiler::h_SaveNode(orc_Node, c_XmlParser, orc_FilePath, opc_CreatedFiles,
                                              orc_NodeIndicesToNameMap);
      //Don't forget to save!
      if (c_XmlParser.SaveToFile(orc_FilePath) != C_NO_ERR)
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

   \param[in]      orc_Node                  data storage
   \param[in,out]  orc_XmlParser             XML with node active
   \param[in]      orc_BasePath              Base path
   \param[in,out]  opc_CreatedFiles          Optional storage for history of all created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::h_SaveNode(const C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                   const stw::scl::C_SclString & orc_BasePath,
                                   std::vector<C_SclString> * const opc_CreatedFiles, const std::map<uint32_t,
                                                                                                     C_SclString> & orc_NodeIndicesToNameMap)
{
   int32_t s32_Retval;

   orc_XmlParser.SetAttributeBool("datapool-auto-nvm-start-address", orc_Node.q_DatapoolAutoNvmStartAddress);
   //Type
   orc_XmlParser.CreateNodeChild("type", orc_Node.c_DeviceType);
   mh_SaveProperties(orc_Node.c_Properties, orc_XmlParser);
   mh_SaveApplications(orc_Node.c_Applications, orc_XmlParser);
   orc_XmlParser.CreateAndSelectNodeChild("com-protocols");
   s32_Retval = h_SaveNodeComProtocols(orc_Node.c_ComProtocols, orc_Node.c_DataPools, orc_XmlParser, orc_BasePath,
                                       opc_CreatedFiles);
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
      s32_Retval = mh_SaveDataPools(orc_Node.c_DataPools, orc_XmlParser, orc_BasePath, opc_CreatedFiles);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_SaveHalc(orc_Node.c_HalcConfig, orc_XmlParser, orc_BasePath, opc_CreatedFiles);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_SaveCanOpenManagers(orc_Node.c_CanOpenManagers, orc_XmlParser, orc_BasePath, opc_CreatedFiles,
                                          orc_NodeIndicesToNameMap);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_SaveDataLoggers(orc_Node.c_DataLoggerJobs, orc_XmlParser, orc_BasePath, opc_CreatedFiles);
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
   \param[in,out]  orc_XmlParser          XML with list active
   \param[in]      orc_BasePath           Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::h_LoadNodeComProtocols(std::vector<C_OscCanProtocol> & orc_NodeComProtocols,
                                               const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                               C_OscXmlParserBase & orc_XmlParser,
                                               const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   C_SclString c_CurNodeComProtocol;
   uint32_t u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
      orc_NodeComProtocols.reserve(u32_ExpectedSize);
   }

   c_CurNodeComProtocol = orc_XmlParser.SelectNodeChild("com-protocol");

   //Clear
   orc_NodeComProtocols.clear();
   if (c_CurNodeComProtocol == "com-protocol")
   {
      do
      {
         C_OscCanProtocol c_CurComProtocol;
         if (s32_Retval == C_NO_ERR)
         {
            if (orc_BasePath.IsEmpty())
            {
               s32_Retval = C_OscNodeCommFiler::h_LoadNodeComProtocol(c_CurComProtocol, orc_XmlParser,
                                                                      orc_NodeDataPools);
            }
            else
            {
               s32_Retval = C_OscNodeCommFiler::h_LoadNodeComProtocolFile(c_CurComProtocol,
                                                                          C_OscSystemFilerUtil::h_CombinePaths(
                                                                             orc_BasePath,
                                                                             orc_XmlParser.GetNodeContent()),
                                                                          orc_NodeDataPools);
            }
         }

         //Append
         orc_NodeComProtocols.push_back(c_CurComProtocol);

         //Next
         c_CurNodeComProtocol = orc_XmlParser.SelectNodeNext("com-protocol");
      }
      while (c_CurNodeComProtocol == "com-protocol");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocols");
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_NodeComProtocols.size())
      {
         C_SclString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected comm protocol count, expected: %u, got %u", u32_ExpectedSize,
                              static_cast<uint32_t>(orc_NodeComProtocols.size()));
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
   \param[in,out]  orc_XmlParser          XML with list active
   \param[in]      orc_BasePath           Base path
   \param[in,out]  opc_CreatedFiles       Optional storage for history of all created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::h_SaveNodeComProtocols(const std::vector<C_OscCanProtocol> & orc_NodeComProtocols,
                                               const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                               C_OscXmlParserBase & orc_XmlParser,
                                               const stw::scl::C_SclString & orc_BasePath,
                                               std::vector<C_SclString> * const opc_CreatedFiles)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeComProtocols.size()));
   for (uint32_t u32_ItComProtocol = 0; (u32_ItComProtocol < orc_NodeComProtocols.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItComProtocol)
   {
      const C_OscCanProtocol & rc_CurProtocol = orc_NodeComProtocols[u32_ItComProtocol];
      orc_XmlParser.CreateAndSelectNodeChild("com-protocol");
      //Find matching datapool
      if (rc_CurProtocol.u32_DataPoolIndex < orc_NodeDataPools.size())
      {
         const C_OscNodeDataPool & rc_CurDatapool = orc_NodeDataPools[rc_CurProtocol.u32_DataPoolIndex];
         if (orc_BasePath.IsEmpty())
         {
            //To string
            C_OscNodeCommFiler::h_SaveNodeComProtocol(rc_CurProtocol, orc_XmlParser, rc_CurDatapool.c_Name);
         }
         else
         {
            const C_SclString c_FileName = C_OscNodeCommFiler::h_GetFileName(rc_CurDatapool.c_Name);
            const C_SclString c_CombinedFileName = C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
            //Save comm definition file
            s32_Retval = C_OscNodeCommFiler::h_SaveNodeComProtocolFile(orc_NodeComProtocols[u32_ItComProtocol],
                                                                       c_CombinedFileName,
                                                                       rc_CurDatapool.c_Name);
            //Set file reference
            orc_XmlParser.SetNodeContent(c_FileName);
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
                             "Invalid index " + C_SclString::IntToStr(
                                rc_CurProtocol.u32_DataPoolIndex) + " for comm protocol");
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "com-protocols");
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
C_SclString C_OscNodeFiler::h_GetFolderName(const C_SclString & orc_NodeName)
{
   return "node_" + C_OscSystemFilerUtil::h_PrepareItemNameForFileName(orc_NodeName);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get automatically generated file name

   \return
   Automatically generated file name
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscNodeFiler::h_GetFileName(void)
{
   return "node_core.xml";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node properties

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_NodeProperties  data storage
   \param[in,out]  orc_XmlParser       XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadProperties(C_OscNodeProperties & orc_NodeProperties, C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("properties") == "properties")
   {
      //Name
      if (orc_XmlParser.SelectNodeChild("name") == "name")
      {
         orc_NodeProperties.c_Name = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"name\" node.");
         s32_Retval = C_CONFIG;
      }
      //Comment
      if (orc_XmlParser.SelectNodeChild("comment") == "comment")
      {
         orc_NodeProperties.c_Comment = orc_XmlParser.GetNodeContent();
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      //Diagnostic server
      if ((orc_XmlParser.SelectNodeChild("diagnostic-server") == "diagnostic-server") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval =
            mh_StringToDiagnosticServer(orc_XmlParser.GetNodeContent(), orc_NodeProperties.e_DiagnosticServer);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"diagnostic-server\" node.");
         s32_Retval = C_CONFIG;
      }
      //Flash loader
      if ((orc_XmlParser.SelectNodeChild("flash-loader") == "flash-loader") && (s32_Retval == C_NO_ERR))
      {
         s32_Retval = mh_StringToFlashLoader(orc_XmlParser.GetNodeContent(), orc_NodeProperties.e_FlashLoader);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"properties\".\"flash-loader\" node.");
         s32_Retval = C_CONFIG;
      }

      //Communication interfaces
      if ((orc_XmlParser.SelectNodeChild("communication-interfaces") == "communication-interfaces") &&
          (s32_Retval == C_NO_ERR))
      {
         s32_Retval = mh_LoadComInterface(orc_NodeProperties.c_ComInterfaces, orc_XmlParser);
      }

      //openSYDE server settings
      if (orc_XmlParser.SelectNodeChild("open-syde-server-settings") == "open-syde-server-settings")
      {
         if (orc_XmlParser.AttributeExists("max-clients") == true)
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u8_MaxClients =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("max-clients"));
         }
         else
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u8_MaxClients = 1;
         }
         if (orc_XmlParser.AttributeExists("max-parallel-transmissions") == true)
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u8_MaxParallelTransmissions =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("max-parallel-transmissions"));
         }
         else
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u8_MaxParallelTransmissions = 64;
         }
         if (orc_XmlParser.AttributeExists("application-index") == true)
         {
            orc_NodeProperties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex =
               static_cast<int16_t>(orc_XmlParser.GetAttributeSint32("application-index"));
         }
         else
         {
            orc_NodeProperties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex = -1;
         }
         if (orc_XmlParser.AttributeExists("max-tx-message-buffer") == true)
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u16_MaxMessageBufferTx =
               static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("max-tx-message-buffer"));
         }
         else
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u16_MaxMessageBufferTx = 585U;
         }
         if (orc_XmlParser.AttributeExists("max-rx-routing-message-buffer") == true)
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u16_MaxRoutingMessageBufferRx =
               static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("max-rx-routing-message-buffer"));
         }
         else
         {
            orc_NodeProperties.c_OpenSydeServerSettings.u16_MaxRoutingMessageBufferRx = 585U;
         }

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      else
      {
         orc_NodeProperties.c_OpenSydeServerSettings.Initialize();
      }

      //Flashloader settings
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadStwFlashloaderOptions(orc_NodeProperties.c_StwFlashloaderSettings, orc_XmlParser);
      }

      //Code export settings
      if ((orc_XmlParser.SelectNodeChild("code-export-settings") == "code-export-settings") && (s32_Retval == C_NO_ERR))
      {
         if (orc_XmlParser.SelectNodeChild("scaling-support") == "scaling-support")
         {
            s32_Retval = C_OscSystemFilerUtil::h_StringToCodeExportScalingType(
               orc_XmlParser.GetNodeContent(), orc_NodeProperties.c_CodeExportSettings.e_ScalingSupport);
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "code-export-settings");
         }
         else
         {
            orc_NodeProperties.c_CodeExportSettings.e_ScalingSupport = C_OscNodeCodeExportSettings::eFLOAT32;
         }

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
      }
      else
      {
         orc_NodeProperties.c_CodeExportSettings.Initialize();
      }

      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out]  orc_XmlParser       XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeFiler::mh_SaveProperties(const C_OscNodeProperties & orc_NodeProperties,
                                       C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("properties");
   orc_XmlParser.CreateNodeChild("name", orc_NodeProperties.c_Name);
   orc_XmlParser.CreateNodeChild("comment", orc_NodeProperties.c_Comment);
   orc_XmlParser.CreateNodeChild("diagnostic-server",
                                 mh_DiagnosticServerToString(orc_NodeProperties.e_DiagnosticServer));
   orc_XmlParser.CreateNodeChild("flash-loader", mh_FlashLoaderToString(orc_NodeProperties.e_FlashLoader));

   //Com interfaces
   mh_SaveComInterface(orc_NodeProperties.c_ComInterfaces, orc_XmlParser);

   //openSYDE server settings
   orc_XmlParser.CreateAndSelectNodeChild("open-syde-server-settings");
   orc_XmlParser.SetAttributeUint32("max-clients", orc_NodeProperties.c_OpenSydeServerSettings.u8_MaxClients);
   orc_XmlParser.SetAttributeUint32("max-parallel-transmissions",
                                    orc_NodeProperties.c_OpenSydeServerSettings.u8_MaxParallelTransmissions);
   orc_XmlParser.SetAttributeSint32("application-index",
                                    orc_NodeProperties.c_OpenSydeServerSettings.s16_DpdDataBlockIndex);
   orc_XmlParser.SetAttributeUint32("max-tx-message-buffer",
                                    orc_NodeProperties.c_OpenSydeServerSettings.u16_MaxMessageBufferTx);
   orc_XmlParser.SetAttributeUint32("max-rx-routing-message-buffer",
                                    orc_NodeProperties.c_OpenSydeServerSettings.u16_MaxRoutingMessageBufferRx);
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");

   //Flashloader options
   mh_SaveStwFlashloaderOptions(orc_NodeProperties.c_StwFlashloaderSettings, orc_XmlParser);

   //Code export settings
   orc_XmlParser.CreateAndSelectNodeChild("code-export-settings");
   orc_XmlParser.CreateNodeChild("scaling-support",
                                 C_OscSystemFilerUtil::h_CodeExportScalingTypeToString(orc_NodeProperties.
                                                                                       c_CodeExportSettings.
                                                                                       e_ScalingSupport));
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");

   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node STW flashloader settings

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[out]     orc_StwFlashloaderSettings   data storage
   \param[in,out]  orc_XmlParser                XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadStwFlashloaderOptions(C_OscNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                     C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("stw-flashloader-settings") == "stw-flashloader-settings")
   {
      if (orc_XmlParser.SelectNodeChild("reset-message") == "reset-message")
      {
         if (orc_XmlParser.AttributeExists("active") == true)
         {
            orc_StwFlashloaderSettings.q_ResetMessageActive = orc_XmlParser.GetAttributeBool("active");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"active\" attribute in \"reset-message\" node.");
            s32_Retval = C_CONFIG;
         }
         if (orc_XmlParser.AttributeExists("extended") == true)
         {
            orc_StwFlashloaderSettings.q_ResetMessageExtendedId = orc_XmlParser.GetAttributeBool("extended");
         }
         else
         {
            orc_StwFlashloaderSettings.q_ResetMessageExtendedId = false;
         }
         if (orc_XmlParser.AttributeExists("id") == true)
         {
            orc_StwFlashloaderSettings.u32_ResetMessageId = orc_XmlParser.GetAttributeUint32("id");
         }
         else
         {
            orc_StwFlashloaderSettings.u32_ResetMessageId = 0;
         }
         if (orc_XmlParser.AttributeExists("dlc") == true)
         {
            orc_StwFlashloaderSettings.u8_ResetMessageDlc =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("dlc"));
         }
         else
         {
            orc_StwFlashloaderSettings.u8_ResetMessageDlc = 8;
         }
         orc_StwFlashloaderSettings.c_Data.clear();
         if (orc_XmlParser.SelectNodeChild("data-bytes") == "data-bytes")
         {
            C_SclString c_CurNode = orc_XmlParser.SelectNodeChild("data-byte");
            if (c_CurNode == "data-byte")
            {
               do
               {
                  if (orc_XmlParser.AttributeExists("value") == true)
                  {
                     const uint8_t u8_DataByte = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("value"));
                     orc_StwFlashloaderSettings.c_Data.push_back(u8_DataByte);
                  }
                  else
                  {
                     osc_write_log_error("Loading node definition",
                                         "Could not find \"value\" attribute in \"data-byte\" node.");
                     s32_Retval = C_CONFIG;
                  }
                  c_CurNode = orc_XmlParser.SelectNodeNext("data-byte");
               }
               while ((c_CurNode == "data-byte") && (s32_Retval == C_NO_ERR));
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "data-bytes");
            }
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "reset-message");
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "stw-flashloader-settings");
      }
      else
      {
         osc_write_log_error("Loading node definition", "Could not find \"reset-message\" node.");
         s32_Retval = C_CONFIG;
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
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
   \param[in,out]  orc_XmlParser                XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeFiler::mh_SaveStwFlashloaderOptions(const C_OscNodeStwFlashloaderSettings & orc_StwFlashloaderSettings,
                                                  C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("stw-flashloader-settings");
   orc_XmlParser.CreateAndSelectNodeChild("reset-message");
   orc_XmlParser.SetAttributeBool("active", orc_StwFlashloaderSettings.q_ResetMessageActive);
   orc_XmlParser.SetAttributeBool("extended", orc_StwFlashloaderSettings.q_ResetMessageExtendedId);
   orc_XmlParser.SetAttributeUint32("id", orc_StwFlashloaderSettings.u32_ResetMessageId);
   orc_XmlParser.SetAttributeUint32("dlc", static_cast<uint32_t>(orc_StwFlashloaderSettings.u8_ResetMessageDlc));
   orc_XmlParser.CreateAndSelectNodeChild("data-bytes");
   for (uint32_t u32_ItDataByte = 0; u32_ItDataByte < orc_StwFlashloaderSettings.c_Data.size(); ++u32_ItDataByte)
   {
      orc_XmlParser.CreateAndSelectNodeChild("data-byte");
      orc_XmlParser.SetAttributeUint32("value",
                                       static_cast<uint32_t>(orc_StwFlashloaderSettings.c_Data[u32_ItDataByte]));
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-bytes");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "reset-message");
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "stw-flashloader-settings");
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node communication interface settings

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[out]     orc_ComInterfaces   data storage
   \param[in,out]  orc_XmlParser       XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadComInterface(std::vector<C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   C_SclString c_CurNode = orc_XmlParser.SelectNodeChild("communication-interface");

   if (c_CurNode == "communication-interface")
   {
      do
      {
         C_OscNodeComInterfaceSettings c_ComInterface;

         c_ComInterface.u8_InterfaceNumber =
            static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("interface-number"));
         c_ComInterface.u8_NodeId = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("node-id"));
         c_ComInterface.q_IsUpdateEnabled = orc_XmlParser.GetAttributeBool("update-available");
         c_ComInterface.q_IsRoutingEnabled = orc_XmlParser.GetAttributeBool("routing-available");
         c_ComInterface.q_IsDiagnosisEnabled = orc_XmlParser.GetAttributeBool("diagnosis-available");
         //Type
         if ((orc_XmlParser.SelectNodeChild("type") == "type") && (s32_Retval == C_NO_ERR))
         {
            s32_Retval = C_OscSystemFilerUtil::h_BusTypeStringToEnum(
               orc_XmlParser.GetNodeContent(), c_ComInterface.e_InterfaceType);
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"communication-interface\".\"type\" node.");
            s32_Retval = C_CONFIG;
         }

         //IP address
         if ((orc_XmlParser.SelectNodeChild("ip-address") == "ip-address") && (s32_Retval == C_NO_ERR))
         {
            c_ComInterface.c_Ip.au8_IpAddress[0] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte1"));
            c_ComInterface.c_Ip.au8_IpAddress[1] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte2"));
            c_ComInterface.c_Ip.au8_IpAddress[2] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte3"));
            c_ComInterface.c_Ip.au8_IpAddress[3] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte4"));
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            c_ComInterface.c_Ip.au8_IpAddress[0] = 0U;
            c_ComInterface.c_Ip.au8_IpAddress[1] = 0U;
            c_ComInterface.c_Ip.au8_IpAddress[2] = 0U;
            c_ComInterface.c_Ip.au8_IpAddress[3] = 0U;
         }

         //net mask
         if ((orc_XmlParser.SelectNodeChild("net-mask") == "net-mask") && (s32_Retval == C_NO_ERR))
         {
            c_ComInterface.c_Ip.au8_NetMask[0] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte1"));
            c_ComInterface.c_Ip.au8_NetMask[1] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte2"));
            c_ComInterface.c_Ip.au8_NetMask[2] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte3"));
            c_ComInterface.c_Ip.au8_NetMask[3] = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte4"));
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            c_ComInterface.c_Ip.au8_NetMask[0] = 0U;
            c_ComInterface.c_Ip.au8_NetMask[1] = 0U;
            c_ComInterface.c_Ip.au8_NetMask[2] = 0U;
            c_ComInterface.c_Ip.au8_NetMask[3] = 0U;
         }

         //default gateway
         if ((orc_XmlParser.SelectNodeChild("default-gateway") == "default-gateway") && (s32_Retval == C_NO_ERR))
         {
            c_ComInterface.c_Ip.au8_DefaultGateway[0] =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte1"));
            c_ComInterface.c_Ip.au8_DefaultGateway[1] =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte2"));
            c_ComInterface.c_Ip.au8_DefaultGateway[2] =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte3"));
            c_ComInterface.c_Ip.au8_DefaultGateway[3] =
               static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("byte4"));
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            c_ComInterface.c_Ip.au8_DefaultGateway[0] = 0U;
            c_ComInterface.c_Ip.au8_DefaultGateway[1] = 0U;
            c_ComInterface.c_Ip.au8_DefaultGateway[2] = 0U;
            c_ComInterface.c_Ip.au8_DefaultGateway[3] = 0U;
         }
         //Bus
         if (orc_XmlParser.SelectNodeChild("bus") == "bus")
         {
            c_ComInterface.SetBusConnected(orc_XmlParser.GetAttributeBool("connected"));
            if (orc_XmlParser.AttributeExists("interface-connected"))
            {
               c_ComInterface.SetInterfaceConnectedInDevice(orc_XmlParser.GetAttributeBool("interface-connected"));
            }
            else
            {
               c_ComInterface.SetInterfaceConnectedInDevice(true);
            }
            c_ComInterface.u32_BusIndex = orc_XmlParser.GetAttributeUint32("bus-index");
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         }
         else
         {
            osc_write_log_error("Loading node definition",
                                "Could not find \"communication-interface\".\"bus\" node.");
            s32_Retval = C_CONFIG;
         }
         orc_ComInterfaces.push_back(c_ComInterface);
         c_CurNode = orc_XmlParser.SelectNodeNext("communication-interface");
      }
      while (c_CurNode == "communication-interface");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interfaces");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save node communication interface settings

   Save node to XML file
   pre-condition: the passed XML parser has the active node set to "properties"
   post-condition: the passed XML parser has the active node set to the same "properties"

   \param[in]      orc_ComInterfaces   data storage
   \param[in,out]  orc_XmlParser       XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeFiler::mh_SaveComInterface(const std::vector<C_OscNodeComInterfaceSettings> & orc_ComInterfaces,
                                         C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("communication-interfaces");
   for (uint32_t u32_ItComInterface = 0; u32_ItComInterface < orc_ComInterfaces.size();
        ++u32_ItComInterface)
   {
      const C_OscNodeComInterfaceSettings & rc_CurComInterface = orc_ComInterfaces[u32_ItComInterface];

      orc_XmlParser.CreateAndSelectNodeChild("communication-interface");
      orc_XmlParser.SetAttributeUint32("interface-number",
                                       static_cast<uint32_t>(rc_CurComInterface.u8_InterfaceNumber));
      orc_XmlParser.SetAttributeUint32("node-id", static_cast<uint32_t>(rc_CurComInterface.u8_NodeId));
      orc_XmlParser.SetAttributeBool("update-available", rc_CurComInterface.q_IsUpdateEnabled);
      orc_XmlParser.SetAttributeBool("routing-available", rc_CurComInterface.q_IsRoutingEnabled);
      orc_XmlParser.SetAttributeBool("diagnosis-available", rc_CurComInterface.q_IsDiagnosisEnabled);
      orc_XmlParser.CreateNodeChild("type",
                                    C_OscSystemFilerUtil::h_BusTypeEnumToString(rc_CurComInterface.e_InterfaceType));
      if (rc_CurComInterface.e_InterfaceType == C_OscSystemBus::eETHERNET)
      {
         orc_XmlParser.CreateAndSelectNodeChild("ip-address");
         orc_XmlParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_IpAddress[0]);
         orc_XmlParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_IpAddress[1]);
         orc_XmlParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_IpAddress[2]);
         orc_XmlParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_IpAddress[3]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         orc_XmlParser.CreateAndSelectNodeChild("net-mask");
         orc_XmlParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_NetMask[0]);
         orc_XmlParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_NetMask[1]);
         orc_XmlParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_NetMask[2]);
         orc_XmlParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_NetMask[3]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
         orc_XmlParser.CreateAndSelectNodeChild("default-gateway");
         orc_XmlParser.SetAttributeUint32("byte1", rc_CurComInterface.c_Ip.au8_DefaultGateway[0]);
         orc_XmlParser.SetAttributeUint32("byte2", rc_CurComInterface.c_Ip.au8_DefaultGateway[1]);
         orc_XmlParser.SetAttributeUint32("byte3", rc_CurComInterface.c_Ip.au8_DefaultGateway[2]);
         orc_XmlParser.SetAttributeUint32("byte4", rc_CurComInterface.c_Ip.au8_DefaultGateway[3]);
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
      }
      //Create this section for compatibility reasons (had content and was required by previous openSYDE versions)
      orc_XmlParser.CreateAndSelectNodeChild("communication-protocol");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
      orc_XmlParser.CreateAndSelectNodeChild("bus");
      orc_XmlParser.SetAttributeBool("connected", rc_CurComInterface.GetBusConnectedRawValue());
      orc_XmlParser.SetAttributeBool("interface-connected", rc_CurComInterface.GetInterfaceConnectedInDeviceRawValue());
      orc_XmlParser.SetAttributeUint32("bus-index", rc_CurComInterface.u32_BusIndex);
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interface");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "communication-interfaces");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "properties");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node applications

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_NodeApplications   data storage
   \param[in,out]  orc_XmlParser          XML with core active

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadApplications(std::vector<C_OscNodeApplication> & orc_NodeApplications,
                                            C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_XmlParser.SelectNodeChild("applications") == "applications")
   {
      C_SclString c_CurNode;
      uint32_t u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
         orc_NodeApplications.reserve(u32_ExpectedSize);
      }

      c_CurNode = orc_XmlParser.SelectNodeChild("application");
      if (c_CurNode == "application")
      {
         do
         {
            C_OscNodeApplication c_CurApplication;

            if (orc_XmlParser.AttributeExists("active") == true)
            {
               c_CurApplication.q_Active = orc_XmlParser.GetAttributeBool("active");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if (orc_XmlParser.AttributeExists("process-id") == true)
            {
               c_CurApplication.u8_ProcessId = static_cast<uint8_t>(orc_XmlParser.GetAttributeUint32("process-id"));
            }
            else
            {
               c_CurApplication.u8_ProcessId = 0U;
            }

            if (orc_XmlParser.AttributeExists("generated-code-version"))
            {
               c_CurApplication.u16_GenCodeVersion =
                  static_cast<uint16_t>(orc_XmlParser.GetAttributeUint32("generated-code-version"));
            }
            else
            {
               // probably deprecated project -> default to first version
               c_CurApplication.u16_GenCodeVersion = 1U;
            }

            //Type
            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("type") == "type"))
            {
               C_OscNodeApplication::h_StringToApplication(orc_XmlParser.GetNodeContent(), c_CurApplication.e_Type);
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }
            //Name
            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("name") == "name"))
            {
               c_CurApplication.c_Name = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            //Comment
            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("comment") == "comment"))
            {
               c_CurApplication.c_Comment = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("project-path") == "project-path"))
            {
               c_CurApplication.c_ProjectPath = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_ProjectPath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("ide-call") == "ide-call"))
            {
               c_CurApplication.c_IdeCall = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_IdeCall = "";
            }

            if ((s32_Retval == C_NO_ERR) &&
                (orc_XmlParser.SelectNodeChild("code-generator-path") == "code-generator-path"))
            {
               c_CurApplication.c_CodeGeneratorPath = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_CodeGeneratorPath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("generate-path") == "generate-path"))
            {
               c_CurApplication.c_GeneratePath = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else
            {
               // No error, if child does not exist.
               c_CurApplication.c_GeneratePath = "";
            }

            if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("result-path") == "result-path"))
            {
               c_CurApplication.c_ResultPaths.resize(1);
               c_CurApplication.c_ResultPaths[0] = orc_XmlParser.GetNodeContent();
               //Return
               tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
            }
            else if ((s32_Retval == C_NO_ERR) && (orc_XmlParser.SelectNodeChild("result") == "result"))
            {
               uint32_t u32_ExpectedOutputfileNumber = 0UL;
               const bool q_ExpectedSizeForOutputFiles = orc_XmlParser.AttributeExists("length");
               c_CurApplication.c_ResultPaths.clear();

               //Check optional length
               if (q_ExpectedSizeForOutputFiles == true)
               {
                  u32_ExpectedOutputfileNumber = orc_XmlParser.GetAttributeUint32("length");
                  c_CurApplication.c_ResultPaths.reserve(u32_ExpectedOutputfileNumber);
               }

               c_CurNode = orc_XmlParser.SelectNodeChild("output-file");
               if (c_CurNode == "output-file")
               {
                  do
                  {
                     c_CurApplication.c_ResultPaths.push_back(orc_XmlParser.GetNodeContent());
                     c_CurNode = orc_XmlParser.SelectNodeNext("output-file");
                  }
                  while (c_CurNode == "output-file");

                  //Compare length
                  if (q_ExpectedSizeForOutputFiles == true)
                  {
                     if (u32_ExpectedOutputfileNumber != c_CurApplication.c_ResultPaths.size())
                     {
                        C_SclString c_Tmp;
                        c_Tmp.PrintFormatted("Unexpected output file count, expected: %u, got %u",
                                             u32_ExpectedOutputfileNumber,
                                             static_cast<uint32_t>(c_CurApplication.c_ResultPaths.size()));
                        osc_write_log_warning("Load file", c_Tmp.c_str());
                     }
                  }

                  //Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "result");
                  // Return
                  tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
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
            c_CurNode = orc_XmlParser.SelectNodeNext("application");
         }
         while ((c_CurNode == "application") && (s32_Retval == C_NO_ERR));
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "applications");
      }
      //Compare length
      if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
      {
         if (u32_ExpectedSize != orc_NodeApplications.size())
         {
            C_SclString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected application count, expected: %u, got %u", u32_ExpectedSize,
                                 static_cast<uint32_t>(orc_NodeApplications.size()));
            osc_write_log_warning("Load file", c_Tmp.c_str());
         }
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out]  orc_XmlParser          XML with core active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeFiler::mh_SaveApplications(const std::vector<C_OscNodeApplication> & orc_NodeApplications,
                                         C_OscXmlParserBase & orc_XmlParser)
{
   orc_XmlParser.CreateAndSelectNodeChild("applications");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeApplications.size()));
   for (uint32_t u32_ItApplication = 0; u32_ItApplication < orc_NodeApplications.size(); ++u32_ItApplication)
   {
      const C_OscNodeApplication & rc_CurApplication = orc_NodeApplications[u32_ItApplication];
      orc_XmlParser.CreateAndSelectNodeChild("application");
      orc_XmlParser.SetAttributeBool("active", rc_CurApplication.q_Active);
      orc_XmlParser.SetAttributeUint32("process-id", static_cast<uint32_t>(rc_CurApplication.u8_ProcessId));
      orc_XmlParser.SetAttributeUint32("generated-code-version",
                                       static_cast<uint32_t>(rc_CurApplication.u16_GenCodeVersion));
      orc_XmlParser.CreateNodeChild("type", C_OscNodeApplication::h_ApplicationToString(rc_CurApplication.e_Type));
      orc_XmlParser.CreateNodeChild("name", rc_CurApplication.c_Name);
      orc_XmlParser.CreateNodeChild("comment", rc_CurApplication.c_Comment);
      orc_XmlParser.CreateNodeChild("project-path", rc_CurApplication.c_ProjectPath);
      orc_XmlParser.CreateNodeChild("ide-call", rc_CurApplication.c_IdeCall);
      orc_XmlParser.CreateNodeChild("code-generator-path", rc_CurApplication.c_CodeGeneratorPath);
      orc_XmlParser.CreateNodeChild("generate-path", rc_CurApplication.c_GeneratePath);
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("result") == "result");
      orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(rc_CurApplication.c_ResultPaths.size()));
      for (uint32_t u32_ItOutFile = 0UL; u32_ItOutFile < rc_CurApplication.c_ResultPaths.size(); ++u32_ItOutFile)
      {
         orc_XmlParser.CreateNodeChild("output-file", rc_CurApplication.c_ResultPaths[u32_ItOutFile]);
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "application");
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "applications");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load node data pools

   Load node data from XML file
   pre-condition: the passed XML parser has the active node set to "node"
   post-condition: the passed XML parser has the active node set to the same "node"

   \param[out]     orc_Node         data storage
   \param[in,out]  orc_XmlParser    XML with core active
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadDataPools(C_OscNode & orc_Node, C_OscXmlParserBase & orc_XmlParser,
                                         const stw::scl::C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear
   orc_Node.c_DataPools.clear();
   if (orc_XmlParser.SelectNodeChild("data-pools") == "data-pools")
   {
      C_SclString c_CurNode;
      uint32_t u32_ExpectedSize = 0UL;
      const bool q_ExpectedSizeHere = orc_XmlParser.AttributeExists("length");

      //Check optional length
      if (q_ExpectedSizeHere == true)
      {
         u32_ExpectedSize = orc_XmlParser.GetAttributeUint32("length");
         orc_Node.c_DataPools.reserve(u32_ExpectedSize);
      }

      c_CurNode = orc_XmlParser.SelectNodeChild("data-pool");
      if (c_CurNode == "data-pool")
      {
         do
         {
            C_OscNodeDataPool c_CurDataPool;
            if (orc_BasePath.IsEmpty())
            {
               s32_Retval = C_OscNodeDataPoolFiler::h_LoadDataPool(c_CurDataPool, orc_XmlParser);
            }
            else
            {
               s32_Retval =
                  C_OscNodeDataPoolFiler::h_LoadDataPoolFile(c_CurDataPool,
                                                             C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                                  orc_XmlParser.
                                                                                                  GetNodeContent()));
            }
            if (s32_Retval != C_NO_ERR)
            {
               break;
            }
            orc_Node.c_DataPools.push_back(c_CurDataPool);
            c_CurNode = orc_XmlParser.SelectNodeNext("data-pool");
         }
         while (c_CurNode == "data-pool");

         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pools");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         if (q_ExpectedSizeHere == true)
         {
            if (u32_ExpectedSize != orc_Node.c_DataPools.size())
            {
               C_SclString c_Tmp;
               c_Tmp.PrintFormatted("Unexpected Datapool count, expected: %u, got %u", u32_ExpectedSize,
                                    static_cast<uint32_t>(orc_Node.c_DataPools.size()));
               osc_write_log_warning("Load file", c_Tmp.c_str());
            }
         }
         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out]  orc_XmlParser       XML with core active
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Optional storage for history of all created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_SaveDataPools(const std::vector<C_OscNodeDataPool> & orc_NodeDataPools,
                                         C_OscXmlParserBase & orc_XmlParser, const stw::scl::C_SclString & orc_BasePath,
                                         std::vector<C_SclString> * const opc_CreatedFiles)
{
   int32_t s32_Retval = C_NO_ERR;

   orc_XmlParser.CreateAndSelectNodeChild("data-pools");
   orc_XmlParser.SetAttributeUint32("length", static_cast<uint32_t>(orc_NodeDataPools.size()));
   for (uint32_t u32_ItDataPool = 0; (u32_ItDataPool < orc_NodeDataPools.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItDataPool)
   {
      const C_OscNodeDataPool & rc_CurDatapool = orc_NodeDataPools[u32_ItDataPool];
      orc_XmlParser.CreateAndSelectNodeChild("data-pool");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         C_OscNodeDataPoolFiler::h_SaveDataPool(rc_CurDatapool, orc_XmlParser);
      }
      else
      {
         const C_SclString c_FileName = C_OscNodeDataPoolFiler::h_GetFileName(rc_CurDatapool.c_Name);
         const C_SclString c_CombinedFileName = C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Save datapool file
         s32_Retval = C_OscNodeDataPoolFiler::h_SaveDataPoolFile(rc_CurDatapool, c_CombinedFileName);
         //Set file reference
         orc_XmlParser.SetNodeContent(c_FileName);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
         }
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "data-pools");
   }
   //Return
   tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load HALC

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR   data read
   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadHalc(C_OscHalcConfig & orc_Config, C_OscXmlParserBase & orc_XmlParser,
                                    const C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear
   orc_Config.Clear();
   if (orc_XmlParser.SelectNodeChild("halc-file") == "halc-file")
   {
      if (orc_BasePath.IsEmpty())
      {
         //From string
         s32_Retval = C_OscHalcConfigFiler::h_LoadData(orc_Config, orc_XmlParser, orc_BasePath);
      }
      else
      {
         s32_Retval =
            C_OscHalcConfigFiler::h_LoadFile(orc_Config,
                                             C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                  orc_XmlParser.
                                                                                  GetNodeContent()), orc_BasePath);
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
   \param[in,out]  orc_XmlParser       XML parser
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_SaveHalc(const C_OscHalcConfig & orc_Config, C_OscXmlParserBase & orc_XmlParser,
                                    const C_SclString & orc_BasePath, std::vector<C_SclString> * const opc_CreatedFiles)
{
   int32_t s32_Retval = C_NO_ERR;

   if (!orc_Config.c_FileString.IsEmpty())
   {
      orc_XmlParser.CreateAndSelectNodeChild("halc-file");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         s32_Retval = C_OscHalcConfigFiler::h_SaveData(orc_Config, orc_XmlParser, orc_BasePath, opc_CreatedFiles);
      }
      else
      {
         // const C_SclString c_FileName = C_OscNodeDataPoolFiler::h_GetFileName(rc_CurDatapool.c_Name);
         //Fix
         const C_SclString c_FileName = "halc.xml";
         const C_SclString c_CombinedFileName = C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Save datapool file
         s32_Retval = C_OscHalcConfigFiler::h_SaveFile(orc_Config, c_CombinedFileName, orc_BasePath,
                                                       opc_CreatedFiles);
         //Set file reference
         orc_XmlParser.SetNodeContent(c_FileName);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
         }
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load CAN open managers

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   STW error codes

   \retval   C_NO_ERR   data read
   \retval   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadCanOpenManagers(std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                                               C_OscXmlParserBase & orc_XmlParser, const C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear
   orc_Config.clear();
   if (orc_XmlParser.SelectNodeChild("can-open-managers-file") == "can-open-managers-file")
   {
      if (orc_BasePath.IsEmpty())
      {
         //From string
         s32_Retval = C_OscCanOpenManagerFiler::h_LoadData(orc_Config, orc_XmlParser, orc_BasePath);
      }
      else
      {
         s32_Retval =
            C_OscCanOpenManagerFiler::h_LoadFile(orc_Config,
                                                 C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                      orc_XmlParser.
                                                                                      GetNodeContent()), orc_BasePath);
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   else
   {
      //Ignore
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save CAN open managers

   \param[in]      orc_Config                Config
   \param[in,out]  orc_XmlParser             XML parser
   \param[in]      orc_BasePath              Base path
   \param[in,out]  opc_CreatedFiles          Created files
   \param[in]      orc_NodeIndicesToNameMap  Node indices to name map

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_SaveCanOpenManagers(const std::map<uint8_t, C_OscCanOpenManagerInfo> & orc_Config,
                                               C_OscXmlParserBase & orc_XmlParser, const C_SclString & orc_BasePath,
                                               std::vector<C_SclString> * const opc_CreatedFiles,
                                               const std::map<uint32_t,
                                                              stw::scl::C_SclString> & orc_NodeIndicesToNameMap)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Config.size() > 0)
   {
      orc_XmlParser.CreateAndSelectNodeChild("can-open-managers-file");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         C_OscCanOpenManagerFiler::h_SaveData(orc_Config, orc_XmlParser, orc_BasePath, opc_CreatedFiles,
                                              orc_NodeIndicesToNameMap);
      }
      else
      {
         // const C_SclString c_FileName = C_OscNodeDataPoolFiler::h_GetFileName(rc_CurDatapool.c_Name);
         //Fix
         const C_SclString c_FileName = "can_open_managers.xml";
         const C_SclString c_CombinedFileName = C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Save datapool file
         s32_Retval = C_OscCanOpenManagerFiler::h_SaveFile(orc_Config, c_CombinedFileName, orc_BasePath,
                                                           opc_CreatedFiles, orc_NodeIndicesToNameMap);
         //Set file reference
         orc_XmlParser.SetNodeContent(c_FileName);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
         }
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data loggers

   \param[in,out]  orc_Config       Config
   \param[in,out]  orc_XmlParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   STW error codes

   \retval   C_NO_ERR   data read
   \retval   C_CONFIG   content of file is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_LoadDataLoggers(std::vector<C_OscDataLoggerJob> & orc_Config,
                                           C_OscXmlParserBase & orc_XmlParser, const C_SclString & orc_BasePath)
{
   int32_t s32_Retval = C_NO_ERR;

   //Clear
   orc_Config.clear();
   if (orc_XmlParser.SelectNodeChild("data-loggers-file") == "data-loggers-file")
   {
      if (orc_BasePath.IsEmpty())
      {
         //From string
         s32_Retval = C_OscDataLoggerJobFiler::h_LoadData(orc_Config, orc_XmlParser);
      }
      else
      {
         s32_Retval =
            C_OscDataLoggerJobFiler::h_LoadFile(orc_Config,
                                                C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                     orc_XmlParser.
                                                                                     GetNodeContent()));
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
   }
   else
   {
      //Ignore
      if (C_OscDataLoggerJob::hq_AllowDataloggerFeature)
      {
         //Default entry necessary
         orc_Config.resize(1);
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save data loggers

   \param[in]      orc_Config          Config
   \param[in,out]  orc_XmlParser       XML parser
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Created files

   \return
   STW error codes

   \retval   C_NO_ERR   data saved
   \retval   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscNodeFiler::mh_SaveDataLoggers(const std::vector<C_OscDataLoggerJob> & orc_Config,
                                           C_OscXmlParserBase & orc_XmlParser, const C_SclString & orc_BasePath,
                                           std::vector<C_SclString> * const opc_CreatedFiles)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_Config.size() > 0)
   {
      orc_XmlParser.CreateAndSelectNodeChild("data-loggers-file");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         C_OscDataLoggerJobFiler::h_SaveData(orc_Config, orc_XmlParser);
      }
      else
      {
         // const C_SclString c_FileName = C_OscNodeDataPoolFiler::h_GetFileName(rc_CurDatapool.c_Name);
         //Fix
         const C_SclString c_FileName = "data_loggers.xml";
         const C_SclString c_CombinedFileName = C_OscSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Save datapool file
         s32_Retval = C_OscDataLoggerJobFiler::h_SaveFile(orc_Config, c_CombinedFileName);
         //Set file reference
         orc_XmlParser.SetNodeContent(c_FileName);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
         }
      }
      //Return
      tgl_assert(orc_XmlParser.SelectNodeParent() == "node");
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
C_SclString C_OscNodeFiler::mh_DiagnosticServerToString(
   const C_OscNodeProperties::E_DiagnosticServerProtocol & ore_DiagnosticProtocol)
{
   C_SclString c_Retval;

   switch (ore_DiagnosticProtocol)
   {
   case C_OscNodeProperties::eDS_OPEN_SYDE:
      c_Retval = "open-syde";
      break;
   case C_OscNodeProperties::eDS_KEFEX:
      c_Retval = "kefex";
      break;
   case C_OscNodeProperties::eDS_NONE:
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
int32_t C_OscNodeFiler::mh_StringToDiagnosticServer(const C_SclString & orc_String,
                                                    C_OscNodeProperties::E_DiagnosticServerProtocol & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "kefex")
   {
      ore_Type = C_OscNodeProperties::eDS_KEFEX;
   }
   else if (orc_String == "open-syde")
   {
      ore_Type = C_OscNodeProperties::eDS_OPEN_SYDE;
   }
   else if (orc_String == "none")
   {
      ore_Type = C_OscNodeProperties::eDS_NONE;
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
C_SclString C_OscNodeFiler::mh_FlashLoaderToString(const C_OscNodeProperties::E_FlashLoaderProtocol & ore_FlashLoader)
{
   C_SclString c_Retval;

   switch (ore_FlashLoader)
   {
   case C_OscNodeProperties::eFL_OPEN_SYDE:
      c_Retval = "open-syde";
      break;
   case C_OscNodeProperties::eFL_STW:
      c_Retval = "stw";
      break;
   case C_OscNodeProperties::eFL_NONE:
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
int32_t C_OscNodeFiler::mh_StringToFlashLoader(const C_SclString & orc_String,
                                               C_OscNodeProperties::E_FlashLoaderProtocol & ore_Type)
{
   int32_t s32_Retval = C_NO_ERR;

   if (orc_String == "stw")
   {
      ore_Type = C_OscNodeProperties::eFL_STW;
   }
   else if (orc_String == "open-syde")
   {
      ore_Type = C_OscNodeProperties::eFL_OPEN_SYDE;
   }
   else if (orc_String == "none")
   {
      ore_Type = C_OscNodeProperties::eFL_NONE;
   }
   else
   {
      osc_write_log_error("Loading node definition",
                          "Invalid value for \"properties\".\"flash-loader\": " + orc_String);
      s32_Retval = C_RANGE;
   }

   return s32_Retval;
}
