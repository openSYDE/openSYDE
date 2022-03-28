//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load/save system definition data from/to file (V3)

   Load/save system definition data from/to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstdio>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCXMLParserLog.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCSystemDefinitionFilerV2.h"
#include "C_OSCSystemDefinitionFiler.h"
#include "TGLFile.h"
#include "TGLUtils.h"
#include "C_OSCNodeSquadFiler.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system definition

   Steps:
   * Load device definitions (if not already loaded)
   * Load system definition
   * for each node set a pointer to the used device definition

   For system definition version V3 the optional parameters to load only active nodes (opc_NodesToLoad),
   skip unnecessary content (oq_SkipContent) and to load only given node (opc_ExpectedNodeName),
   as alternative to list of active nodes, are supported.

   \param[out]     orc_SystemDefinition         Pointer to storage
   \param[in]      orc_PathSystemDefinition     Path to system definition
   \param[in]      orc_PathDeviceDefinitions    Path to device definition description file
   \param[in]      oq_UseDeviceDefinitions      Flag for using device definitions, if the flag is false
                                                orc_PathDeviceDefinitions can be an empty string.
                                                It is highly recommended to use the device definitions.
                                                Purpose for not using the device definition is when only read
                                                access to a part of the system definition is necessary.
   \param[in,out]  opu16_ReadFileVersion        Optional storage for read file version (only use in C_NO_ERR case)
   \param[in]      opc_NodesToLoad              (Optional parameter) only load content of nodes which are active in sysdef
   \param[in]      oq_SkipContent               (Optional parameter) skip content when not needed (datapools, halc etc.)
                                                (default = false)
   \param[in]      opc_ExpectedNodeName         (Optional parameter) only load node content of given node. Parameter is
                                                used if no node index is available to fill opc_NodesToLoad parameter.

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_LoadSystemDefinitionFile(C_OSCSystemDefinition & orc_SystemDefinition,
                                                              const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                                              const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                              const bool oq_UseDeviceDefinitions,
                                                              uint16 * const opu16_ReadFileVersion,
                                                              const std::vector<uint8> * const opc_NodesToLoad,
                                                              const bool oq_SkipContent,
                                                              const stw_scl::C_SCLString * const opc_ExpectedNodeName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_PathSystemDefinition) == true)
   {
      C_OSCXMLParserLog c_XMLParser;
      c_XMLParser.SetLogHeading("Loading System Definition");
      s32_Retval = c_XMLParser.LoadFromFile(orc_PathSystemDefinition);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = h_LoadSystemDefinition(orc_SystemDefinition, c_XMLParser, orc_PathDeviceDefinitions,
                                             orc_PathSystemDefinition, oq_UseDeviceDefinitions,
                                             opu16_ReadFileVersion, opc_NodesToLoad, oq_SkipContent,
                                             opc_ExpectedNodeName);
      }
      else
      {
         osc_write_log_error("Loading System Definition",
                             "File \"" + orc_PathSystemDefinition + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading System Definition", "File \"" + orc_PathSystemDefinition + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system definition

   Save system definition
   Will overwrite the file if it already exists.
   Does NOT write the device definition file(s)

   \param[in]      orc_SystemDefinition   Pointer to storage
   \param[in]      orc_Path               Path of system definition
   \param[in,out]  opc_CreatedFiles       Optional storage for history of all created files (and without sysdef)

   \return
   C_NO_ERR   data saved
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_SaveSystemDefinitionFile(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                              const stw_scl::C_SCLString & orc_Path,
                                                              std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Return = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      //erase it:
      sintn sn_Return;
      sn_Return = std::remove(orc_Path.c_str());
      if (sn_Return != 0)
      {
         osc_write_log_error("Saving System Definition", "Could not erase pre-existing file \"" + orc_Path + "\".");
         s32_Return = C_RD_WR;
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      const C_SCLString c_Folder = TGL_ExtractFilePath(orc_Path);
      if (TGL_DirectoryExists(c_Folder) == false)
      {
         if (TGL_CreateDirectory(c_Folder) != 0)
         {
            osc_write_log_error("Saving System Definition", "Could not create folder \"" + c_Folder + "\".");
            s32_Return = C_RD_WR;
         }
      }
   }
   if (s32_Return == C_NO_ERR)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Return = h_SaveSystemDefinition(orc_SystemDefinition, c_XMLParser, orc_Path, opc_CreatedFiles);
      if (s32_Return == C_NO_ERR)
      {
         s32_Return = c_XMLParser.SaveToFile(orc_Path);
         if (s32_Return != C_NO_ERR)
         {
            osc_write_log_error("Saving System Definition", "Could not write to file \"" + orc_Path + "\".");
            s32_Return = C_RD_WR;
         }
      }
      else
      {
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load nodes

   Load nodes data.
   * load node data and add to system definition
   * for each node set a pointer to the used device definition

    The caller is responsible to provide a static life-time of orc_DeviceDefinitions.
    Otherwise the "device definition" pointers in C_OSCNode will point to invalid data.

   \param[out]     orc_Nodes                 data storage
   \param[in,out]  orc_XMLParser             XML with "nodes" active
   \param[in]      orc_DeviceDefinitions     List of known devices (must contain all device types used by nodes)
   \param[in]      orc_BasePath              Base path
   \param[in]      oq_UseDeviceDefinitions   Flag for using device definitions
   \param[in]      oq_UseFileInterface       Flag for switching between multiple file and single file interface
                                             True: Assume content is split over multiple files
                                             False: Assume all relevant content is in this file
   \param[in]      opc_NodesToLoad           (Optional parameter) only load content of nodes which are active in sysdef
   \param[in]      oq_SkipContent            (Optional parameter) skip content when not needed (datapools, halc etc.)
                                             (default = false)
   \param[in]      opc_ExpectedNodeName      (Optional parameter) only load node content of given node. Parameter is
                                             used if no node index is available to fill opc_NodesToLoad parameter.

   \return
   C_NO_ERR    no error
   C_CONFIG    content is invalid or incomplete
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_LoadNodes(std::vector<C_OSCNode> & orc_Nodes, C_OSCXMLParserBase & orc_XMLParser,
                                               const C_OSCDeviceManager & orc_DeviceDefinitions,
                                               const stw_scl::C_SCLString & orc_BasePath,
                                               const bool oq_UseDeviceDefinitions, const bool oq_UseFileInterface,
                                               const std::vector<stw_types::uint8> * const opc_NodesToLoad,
                                               const bool oq_SkipContent,
                                               const stw_scl::C_SCLString * const opc_ExpectedNodeName)

{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");
   const C_SCLString c_UnloadedNode = "UnloadedNodeWithNodeIndex";

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Nodes.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("node");

   //clear list of nodes:
   orc_Nodes.clear();

   if (c_SelectedNode == "node")
   {
      uint8 u8_NodeIndex = 0U;
      do
      {
         C_OSCNode c_Item;
         bool q_SkipNode = false;

         if ((opc_NodesToLoad == NULL) || (opc_NodesToLoad->size() == 0) || ((*opc_NodesToLoad)[u8_NodeIndex] == 1U))
         {
            if (oq_UseFileInterface)
            {
               const C_SCLString c_FileName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                   orc_XMLParser.GetNodeContent());
               if ((opc_ExpectedNodeName != NULL) && ((*opc_ExpectedNodeName) != ""))
               {
                  // get current node and compare with expected node name
                  const uint32 u32_BasePathLength = TGL_ExtractFilePath(orc_BasePath).Length();
                  C_SCLString c_LastFolderName = TGL_ExtractFilePath(c_FileName);
                  c_LastFolderName = c_LastFolderName.Delete(1, u32_BasePathLength);
                  c_LastFolderName = c_LastFolderName.Delete(c_LastFolderName.Length(), 1); // remove trailing delim
                  C_SCLString c_ExpectedFolder = "node_" + (*opc_ExpectedNodeName);
                  // special handling for squad nodes necessary since the separator can't be used for folder names
                  const uint32 u32_Pos = c_ExpectedFolder.Pos(C_OSCNodeSquad::hc_SEPARATOR);
                  if (u32_Pos > 0)
                  {
                     // Squad nodes have '5858' in folder name for '::'
                     const uint32 u32_SubstituteLength = (C_OSCNodeSquad::hc_SEPARATOR).Length();
                     c_ExpectedFolder = c_ExpectedFolder.Delete(u32_Pos, u32_SubstituteLength);
                     C_SCLString c_Substitute = "";
                     for (uint32 u32_Iter = 1; u32_Iter <= u32_SubstituteLength; u32_Iter++)
                     {
                        c_Substitute += static_cast<uint8>((C_OSCNodeSquad::hc_SEPARATOR)[u32_Iter]);
                     }
                     c_ExpectedFolder = c_ExpectedFolder.Insert(c_Substitute, u32_Pos);
                  }

                  if (c_LastFolderName.AnsiCompareIC(c_ExpectedFolder) != 0)
                  {
                     q_SkipNode = true;
                  }
               }
               if (q_SkipNode == false)
               {
                  s32_Retval = C_OSCNodeFiler::h_LoadNodeFile(c_Item, c_FileName, oq_SkipContent);
               }
            }
            else
            {
               s32_Retval = C_OSCNodeFiler::h_LoadNode(c_Item, orc_XMLParser, "", oq_SkipContent);
            }
            if (s32_Retval != C_NO_ERR)
            {
               break;
            }
         }
         else
         {
            q_SkipNode = true;
         }

         if (q_SkipNode == true)
         {
            //don't load; node will have default values
            c_Item.c_Properties.c_Name = c_UnloadedNode + C_SCLString::IntToStr(u8_NodeIndex);
         }

         orc_Nodes.push_back(c_Item);
         u8_NodeIndex++;
         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("node");
      }
      while (c_SelectedNode == "node");
      if (s32_Retval == C_NO_ERR)
      {
         //Return (no check to allow reuse)
         orc_XMLParser.SelectNodeParent();
      }
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Nodes.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected nodes count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Nodes.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }

   if ((oq_UseDeviceDefinitions == true) &&
       (s32_Retval == C_NO_ERR))
   {
      //set pointers to device definitions
      for (uint32 u32_NodeIndex = 0U; u32_NodeIndex < orc_Nodes.size(); u32_NodeIndex++)
      {
         // check if we have an active node
         if (orc_Nodes[u32_NodeIndex].c_Properties.c_Name.Pos(c_UnloadedNode) == 0)
         {
            stw_scl::C_SCLString c_SubDeviceName = "";
            stw_scl::C_SCLString c_MainDeviceName = "";
            C_OSCSystemDefinitionFiler::h_SplitDeviceType(orc_Nodes[u32_NodeIndex].c_DeviceType, c_MainDeviceName,
                                                          c_SubDeviceName);
            {
               const C_OSCDeviceDefinition * const pc_Device =
                  orc_DeviceDefinitions.LookForDevice(c_SubDeviceName, c_MainDeviceName,
                                                      orc_Nodes[u32_NodeIndex].u32_SubDeviceIndex);
               if (pc_Device == NULL)
               {
                  s32_Retval = C_OVERFLOW;
                  osc_write_log_error("Loading System Definition",
                                      "System Definition contains node \"" + orc_Nodes[u32_NodeIndex].c_Properties.c_Name +
                                      "\" of device type \"" +
                                      orc_Nodes[u32_NodeIndex].c_DeviceType + "\" which is not a known device.");
                  break;
               }
               else
               {
                  orc_Nodes[u32_NodeIndex].pc_DeviceDefinition = pc_Device;
               }
            }
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load buses

   Load buses data.
   The bus data will be loaded and the bus will be added.

   \param[in,out]  orc_Buses        data storage
   \param[in,out]  orc_XMLParser    XML with "buses" active

   \return
   C_NO_ERR   no error
   C_CONFIG   content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_LoadBuses(std::vector<C_OSCSystemBus> & orc_Buses,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

   //Check optional length
   if (q_ExpectedSizeHere == true)
   {
      u32_ExpectedSize = orc_XMLParser.GetAttributeUint32("length");
      orc_Buses.reserve(u32_ExpectedSize);
   }

   c_SelectedNode = orc_XMLParser.SelectNodeChild("bus");

   orc_Buses.clear();
   if (c_SelectedNode == "bus")
   {
      do
      {
         C_OSCSystemBus c_Item;
         s32_Retval = C_OSCSystemBusFiler::h_LoadBus(c_Item, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Buses.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("bus");
      }
      while (c_SelectedNode == "bus");
      //Return (no check to allow reuse)
      orc_XMLParser.SelectNodeParent();
   }
   //Compare length
   if ((s32_Retval == C_NO_ERR) && (q_ExpectedSizeHere == true))
   {
      if (u32_ExpectedSize != orc_Buses.size())
      {
         C_SCLString c_Tmp;
         c_Tmp.PrintFormatted("Unexpected bus count, expected: %i, got %i", u32_ExpectedSize,
                              orc_Buses.size());
         osc_write_log_warning("Load file", c_Tmp.c_str());
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save nodes

   Save nodes data.
   The node data will be saved and the node will be added.

   \param[in]      orc_Nodes           data storage
   \param[in,out]  orc_XMLParser       XML with "nodes" active
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Optional storage for history of all created files

   \return
   C_NO_ERR   no error
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_SaveNodes(const std::vector<C_OSCNode> & orc_Nodes,
                                               C_OSCXMLParserBase & orc_XMLParser,
                                               const stw_scl::C_SCLString & orc_BasePath,
                                               std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_XMLParser.SetAttributeUint32("length", orc_Nodes.size());
   for (uint32 u32_Index = 0U; (u32_Index < orc_Nodes.size()) && (s32_Retval == C_NO_ERR); u32_Index++)
   {
      const C_OSCNode & rc_Node = orc_Nodes[u32_Index];
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("node") == "node");
      if (orc_BasePath.IsEmpty())
      {
         //To string
         s32_Retval = C_OSCNodeFiler::h_SaveNode(rc_Node, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
      }
      else
      {
         std::vector<C_SCLString> c_CreatedFiles;
         const C_SCLString c_FolderName = C_OSCNodeFiler::h_GetFolderName(rc_Node.c_Properties.c_Name);
         const C_SCLString c_FileName = c_FolderName + "/" + C_OSCNodeFiler::h_GetFileName();
         const C_SCLString c_CombinedFolderName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FolderName);
         const C_SCLString c_CombinedFileName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath, c_FileName);
         //Create folder
         if (TGL_CreateDirectory(c_CombinedFolderName) != 0)
         {
            osc_write_log_error("Saving node definition",
                                "Could not create directory \"" + c_CombinedFolderName + "\"");
         }
         //Save node file
         s32_Retval = C_OSCNodeFiler::h_SaveNodeFile(rc_Node, c_CombinedFileName,
                                                     (opc_CreatedFiles != NULL) ? &c_CreatedFiles : NULL);
         //Store if necessary
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_FileName);
            for (uint32 u32_ItSubFile = 0UL; u32_ItSubFile < c_CreatedFiles.size(); ++u32_ItSubFile)
            {
               const C_SCLString c_FileNameWithFolder = c_FolderName + "/" + c_CreatedFiles[u32_ItSubFile];
               opc_CreatedFiles->push_back(c_FileNameWithFolder);
            }
         }
         //Set file reference
         orc_XMLParser.SetNodeContent(c_FileName);
      }
      //Return (don't check to allow reuse)
      orc_XMLParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save buses

   Save buses data.
   The bus data will be saved and the bus will be added to the system definition (sorted).

   \param[in]      orc_Buses        data storage
   \param[in,out]  orc_XMLParser    XML with "buses" active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemDefinitionFiler::h_SaveBuses(const std::vector<C_OSCSystemBus> & orc_Buses,
                                             C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Buses.size());
   for (uint32 u32_Index = 0U; u32_Index < orc_Buses.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bus") == "bus");
      C_OSCSystemBusFiler::h_SaveBus(orc_Buses[u32_Index], orc_XMLParser);
      //Return (don't check to allow reuse)
      orc_XMLParser.SelectNodeParent();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system definition

   Steps:
   * Load device definitions (if not already loaded)
   * Load system definition
   * for each node set a pointer to the used device definition

   \param[out]     orc_SystemDefinition         Pointer to storage
   \param[in,out]  orc_XMLParser                XML with default state
   \param[in]      orc_PathDeviceDefinitions    Path to device definition description file
   \param[in]      orc_BasePath                 Base path
   \param[in]      oq_UseDeviceDefinitions      Flag for using device definitions
   \param[in,out]  opu16_ReadFileVersion        Optional storage for read file version (only use in C_NO_ERR case)
   \param[in]      opc_NodesToLoad              (Optional parameter) only load content of nodes which are active in sysdef
   \param[in]      oq_SkipContent               (Optional parameter) skip content when not needed (datapools, halc etc.)
                                                (default = false)
   \param[in]      opc_ExpectedNodeName         (Optional parameter) only load node content of given node. Parameter is
                                                used if no node index is available to fill opc_NodesToLoad parameter.

   \return
   C_NO_ERR    data read
   C_CONFIG    system definition content is invalid or incomplete
               device definition could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_LoadSystemDefinition(C_OSCSystemDefinition & orc_SystemDefinition,
                                                          C_OSCXMLParserBase & orc_XMLParser,
                                                          const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                          const stw_scl::C_SCLString & orc_BasePath,
                                                          const bool oq_UseDeviceDefinitions,
                                                          uint16 * const opu16_ReadFileVersion,
                                                          const std::vector<uint8> * const opc_NodesToLoad,
                                                          const bool oq_SkipContent,
                                                          const stw_scl::C_SCLString * const opc_ExpectedNodeName)
{
   sint32 s32_Retval = C_NO_ERR;
   uint16 u16_FileVersion = 0U;

   //do we need to load the device definitions ?
   if ((oq_UseDeviceDefinitions == true) &&
       (C_OSCSystemDefinition::hc_Devices.WasLoaded() == false))
   {
      s32_Retval = C_OSCSystemDefinition::hc_Devices.LoadFromFile(orc_PathDeviceDefinitions, false, NULL);
      if (s32_Retval != C_NO_ERR)
      {
         osc_write_log_error("Loading System Definition", "Could not load Device definitions.");
         s32_Retval = C_CONFIG;
      }
   }

   if (orc_XMLParser.SelectRoot() == "opensyde-system-definition")
   {
      bool q_UseV2Filer = false;
      bool q_UseV3Filer = false;
      //File version
      if (orc_XMLParser.SelectNodeChild("file-version") == "file-version")
      {
         try
         {
            u16_FileVersion = static_cast<uint16>(orc_XMLParser.GetNodeContent().ToInt());
            if (opu16_ReadFileVersion != NULL)
            {
               *opu16_ReadFileVersion = u16_FileVersion;
            }
         }
         catch (...)
         {
            osc_write_log_error("Loading System Definition", "\"file-version\" could not be converted to a number.");
            s32_Retval = C_CONFIG;
         }

         //is the file version one we know ?
         if (s32_Retval == C_NO_ERR)
         {
            osc_write_log_info("Loading System Definition", "Value of \"file-version\": " +
                               C_SCLString::IntToStr(u16_FileVersion));
            //Check which loader needs to be used
            if ((u16_FileVersion == hu16_FILE_VERSION_1) || (u16_FileVersion == hu16_FILE_VERSION_2))
            {
               q_UseV2Filer = true;
            }
            else if (u16_FileVersion == hu16_FILE_VERSION_3)
            {
               q_UseV3Filer = true;
            }
            else
            {
               osc_write_log_error("Loading System Definition",
                                   "Version defined by \"file-version\" is not supported.");
               s32_Retval = C_CONFIG;
            }
         }

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
      }
      else
      {
         osc_write_log_error("Loading System Definition", "Could not find \"file-version\" node.");
         s32_Retval = C_CONFIG;
      }
      //Only continue if no error so far
      if (s32_Retval == C_NO_ERR)
      {
         if (q_UseV2Filer)
         {
            //Unselect root
            tgl_assert(orc_XMLParser.SelectNodeParent() == "");
            //Completely rely on V2 loader
            s32_Retval = C_OSCSystemDefinitionFilerV2::h_LoadSystemDefinition(orc_SystemDefinition, orc_XMLParser,
                                                                              orc_PathDeviceDefinitions,
                                                                              oq_UseDeviceDefinitions);
         }
         if (q_UseV3Filer)
         {
            //Completely rely on V3 loader
            //Groups
            orc_SystemDefinition.c_NodeSquads.clear();
            if (s32_Retval == C_NO_ERR)
            {
               s32_Retval = C_OSCNodeSquadFiler::h_LoadNodeGroups(orc_SystemDefinition.c_NodeSquads, orc_XMLParser);
            }

            //Node
            orc_SystemDefinition.c_Nodes.clear();
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XMLParser.SelectNodeChild("nodes") == "nodes")
               {
                  s32_Retval = h_LoadNodes(orc_SystemDefinition.c_Nodes, orc_XMLParser,
                                           C_OSCSystemDefinition::hc_Devices, orc_BasePath, oq_UseDeviceDefinitions,
                                           true, opc_NodesToLoad, oq_SkipContent, opc_ExpectedNodeName);
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Return
                     tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
                  }
               }
               else
               {
                  osc_write_log_error("Loading System Definition", "Could not find \"nodes\" node.");
                  s32_Retval = C_CONFIG;
               }
            }

            //Bus
            orc_SystemDefinition.c_Buses.clear();
            if (s32_Retval == C_NO_ERR)
            {
               if (orc_XMLParser.SelectNodeChild("buses") == "buses")
               {
                  s32_Retval = h_LoadBuses(orc_SystemDefinition.c_Buses, orc_XMLParser);
                  if (s32_Retval == C_NO_ERR)
                  {
                     //Return
                     tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
                  }
               }
               else
               {
                  osc_write_log_error("Loading System Definition", "Could not find \"buses\" node.");
                  s32_Retval = C_CONFIG;
               }
            }
         }
      }
   }
   else
   {
      osc_write_log_error("Loading System Definition", "Could not find \"opensyde-system-definition\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system definition

   Save system definition
   Does NOT write the device definition file(s)

   \param[in]      orc_SystemDefinition   Pointer to storage
   \param[in,out]  orc_XMLParser          XML with default state
   \param[in]      orc_BasePath           Base path
   \param[in,out]  opc_CreatedFiles       Optional storage for history of all created files

   \return
   C_NO_ERR   no error
   C_CONFIG   file could not be created
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFiler::h_SaveSystemDefinition(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                          C_OSCXMLParserBase & orc_XMLParser,
                                                          const stw_scl::C_SCLString & orc_BasePath,
                                                          std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Return;

   orc_XMLParser.CreateNodeChild("opensyde-system-definition");
   tgl_assert(orc_XMLParser.SelectRoot() == "opensyde-system-definition");
   //File version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XMLParser.SetNodeContent(C_SCLString::IntToStr(hu16_FILE_VERSION_LATEST));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
   C_OSCNodeSquadFiler::h_SaveNodeGroups(orc_SystemDefinition.c_NodeSquads, orc_XMLParser);
   //Node
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("nodes") == "nodes");
   s32_Return = h_SaveNodes(orc_SystemDefinition.c_Nodes, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
   if (s32_Return == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");

      //Bus
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("buses") == "buses");
      h_SaveBuses(orc_SystemDefinition.c_Buses, orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Split device type

   \param[in]      orc_CompleteType    Complete type
   \param[in,out]  orc_MainType        Main type (empty if none)
   \param[in,out]  orc_SubType         Sub type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemDefinitionFiler::h_SplitDeviceType(const C_SCLString & orc_CompleteType, C_SCLString & orc_MainType,
                                                   C_SCLString & orc_SubType)
{
   const std::string c_Tmp = *orc_CompleteType.AsStdString();
   const uint32 u32_Pos = c_Tmp.find(C_OSCNodeSquad::hc_SEPARATOR.c_str());

   if (u32_Pos < c_Tmp.size())
   {
      orc_MainType = c_Tmp.substr(0, u32_Pos);
      //lint -e{9114} kept for readability
      orc_SubType = c_Tmp.substr(u32_Pos + C_OSCNodeSquad::hc_SEPARATOR.Length(), c_Tmp.size());
   }
   else
   {
      orc_MainType = "";
      orc_SubType = orc_CompleteType;
   }
}
