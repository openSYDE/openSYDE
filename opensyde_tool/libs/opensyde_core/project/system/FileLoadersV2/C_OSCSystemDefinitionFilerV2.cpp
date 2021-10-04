//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load/save system definition data from/to file (V2)

   Load/save system definition data from/to XML file

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cstdio>
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCSystemDefinitionFilerV2.h"
#include "TGLFile.h"
#include "TGLUtils.h"
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

   \param[out]  orc_SystemDefinition         Pointer to storage
   \param[in]   orc_PathSystemDefinition     Path to system definition
   \param[in]   orc_PathDeviceDefinitions    Path to device definition description file
   \param[in]   oq_UseDeviceDefinitions      Flag for using device definitions, if the flag is true
                                             orc_PathDeviceDefinitions can be an empty string.
                                             It is highly recommended to use the device definitions.
                                             Purpose for not using the device definition is when only read
                                             access to a part of the system definition is necessary.

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFilerV2::h_LoadSystemDefinitionFile(C_OSCSystemDefinition & orc_SystemDefinition,
                                                                const stw_scl::C_SCLString & orc_PathSystemDefinition,
                                                                const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                                const bool oq_UseDeviceDefinitions)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_PathSystemDefinition) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = c_XMLParser.LoadFromFile(orc_PathSystemDefinition);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = h_LoadSystemDefinition(orc_SystemDefinition, c_XMLParser, orc_PathDeviceDefinitions,
                                             oq_UseDeviceDefinitions);
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

   \param[in]  orc_SystemDefinition    Pointer to storage
   \param[in]  orc_Path                Path of system definition

   \return
   C_NO_ERR   data saved
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFilerV2::h_SaveSystemDefinitionFile(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                                const stw_scl::C_SCLString & orc_Path)
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
      C_OSCXMLParser c_XMLParser;
      h_SaveSystemDefinition(orc_SystemDefinition, c_XMLParser);

      s32_Return = c_XMLParser.SaveToFile(orc_Path);
      if (s32_Return != C_NO_ERR)
      {
         osc_write_log_error("Saving System Definition", "Could not write to file \"" + orc_Path + "\".");
         s32_Return = C_RD_WR;
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load system definition

   * Load device definitions from files (if not already loaded)
   * Load system definition from string
   * for each node set a pointer to the used device definition

   \param[out]  orc_SystemDefinition         Pointer to storage
   \param[in]   orc_Content                  XML Content string
   \param[in]   orc_PathDeviceDefinitions    Path to device definition description file
   \param[in]   oq_UseDeviceDefinitions      Flag for using device definitions, if the flag is true
                                             orc_PathDeviceDefinitions can be an empty string.
                                             It is highly recommended to use the device definitions.
                                             Purpose for not using the device definition is when only read
                                             access to a part of the system definition is necessary.

   \return
   C_NO_ERR   data read
   C_CONFIG   content is invalid or incomplete
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFilerV2::h_LoadSystemDefinitionString(C_OSCSystemDefinition & orc_SystemDefinition,
                                                                  const C_SCLString & orc_Content,
                                                                  const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                                  const bool oq_UseDeviceDefinitions)
{
   sint32 s32_Retval;
   //Set up parser
   C_OSCXMLParserString c_XMLParser;

   s32_Retval = c_XMLParser.LoadFromString(orc_Content);
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = h_LoadSystemDefinition(orc_SystemDefinition, c_XMLParser, orc_PathDeviceDefinitions,
                                          oq_UseDeviceDefinitions);
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save system definition

   Save system definition
   Does NOT write the device definition file(s)

   \param[in]   orc_SystemDefinition   Pointer to storage
   \param[out]  orc_Content            XML Content string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemDefinitionFilerV2::h_SaveSystemDefinitionString(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                                C_SCLString & orc_Content)
{
   //Set up parser
   C_OSCXMLParserString c_XMLParser;

   h_SaveSystemDefinition(orc_SystemDefinition, c_XMLParser);
   c_XMLParser.SaveToString(orc_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load nodes

   Load nodes data.
   * load node data and add to system definition
   * for each node set a pointer to the used device definition

    The caller is responsible to provide a static life-time of orc_DeviceDefinitions.
    Otherwise the "device definition" pointers in C_OSCNode will point to invalid data.

   \param[in]      ou16_XmlFormatVersion     version of XML format
   \param[out]     orc_Nodes                 data storage
   \param[in,out]  orc_XMLParser             XML with "nodes" active
   \param[in]      orc_DeviceDefinitions     List of known devices (must contain all device types used by nodes)
   \param[in]      oq_UseDeviceDefinitions   Flag for using device definitions

   \return
   C_NO_ERR    no error
   C_CONFIG    content is invalid or incomplete
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFilerV2::h_LoadNodes(const uint16 ou16_XmlFormatVersion, std::vector<C_OSCNode> & orc_Nodes,
                                                 C_OSCXMLParserBase & orc_XMLParser,
                                                 const C_OSCDeviceManager & orc_DeviceDefinitions,
                                                 const bool oq_UseDeviceDefinitions)

{
   sint32 s32_Retval = C_NO_ERR;
   C_SCLString c_SelectedNode;
   uint32 u32_ExpectedSize = 0UL;
   const bool q_ExpectedSizeHere = orc_XMLParser.AttributeExists("length");

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
      do
      {
         C_OSCNode c_Item;
         s32_Retval = C_OSCNodeFilerV2::h_LoadNode(ou16_XmlFormatVersion, c_Item, orc_XMLParser);
         if (s32_Retval != C_NO_ERR)
         {
            break;
         }
         orc_Nodes.push_back(c_Item);
         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("node");
      }
      while (c_SelectedNode == "node");
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes");
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
         const C_OSCDeviceDefinition * const pc_Device =
            orc_DeviceDefinitions.LookForDevice(orc_Nodes[u32_NodeIndex].c_DeviceType, "",
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
            orc_Nodes[u32_NodeIndex].u32_SubDeviceIndex = 0UL;
            orc_Nodes[u32_NodeIndex].pc_DeviceDefinition = pc_Device;
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
sint32 C_OSCSystemDefinitionFilerV2::h_LoadBuses(std::vector<C_OSCSystemBus> & orc_Buses,
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
         s32_Retval = C_OSCSystemBusFilerV2::h_LoadBus(c_Item, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            orc_Buses.push_back(c_Item);
         }

         //Next
         c_SelectedNode = orc_XMLParser.SelectNodeNext("bus");
      }
      while (c_SelectedNode == "bus");
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "buses");
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

   \param[in]      orc_Nodes        data storage
   \param[in,out]  orc_XMLParser    XML with "nodes" active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemDefinitionFilerV2::h_SaveNodes(const std::vector<C_OSCNode> & orc_Nodes,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Nodes.size());
   for (uint32 u32_Index = 0U; u32_Index < orc_Nodes.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("node") == "node");
      C_OSCNodeFilerV2::h_SaveNode(orc_Nodes[u32_Index], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "nodes");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save buses

   Save buses data.
   The bus data will be saved and the bus will be added to the system definition (sorted).

   \param[in]      orc_Buses        data storage
   \param[in,out]  orc_XMLParser    XML with "buses" active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemDefinitionFilerV2::h_SaveBuses(const std::vector<C_OSCSystemBus> & orc_Buses,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.SetAttributeUint32("length", orc_Buses.size());
   for (uint32 u32_Index = 0U; u32_Index < orc_Buses.size(); u32_Index++)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bus") == "bus");
      C_OSCSystemBusFilerV2::h_SaveBus(orc_Buses[u32_Index], orc_XMLParser);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "buses");
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
   \param[in]      oq_UseDeviceDefinitions      Flag for using device definitions

   \return
   C_NO_ERR    data read
   C_CONFIG    system definition content is invalid or incomplete
               device definition could not be loaded
   C_OVERFLOW  node in system definition references a device not part of the device definitions
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCSystemDefinitionFilerV2::h_LoadSystemDefinition(C_OSCSystemDefinition & orc_SystemDefinition,
                                                            C_OSCXMLParserBase & orc_XMLParser,
                                                            const stw_scl::C_SCLString & orc_PathDeviceDefinitions,
                                                            const bool oq_UseDeviceDefinitions)
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
      //File version
      if (orc_XMLParser.SelectNodeChild("file-version") == "file-version")
      {
         try
         {
            u16_FileVersion = static_cast<uint16>(orc_XMLParser.GetNodeContent().ToInt());
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
            if ((u16_FileVersion != hu16_FILE_VERSION_1) && (u16_FileVersion != hu16_FILE_VERSION_2))
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
      //Node
      orc_SystemDefinition.c_Nodes.clear();
      if (s32_Retval == C_NO_ERR)
      {
         if (orc_XMLParser.SelectNodeChild("nodes") == "nodes")
         {
            s32_Retval = h_LoadNodes(u16_FileVersion, orc_SystemDefinition.c_Nodes, orc_XMLParser,
                                     C_OSCSystemDefinition::hc_Devices, oq_UseDeviceDefinitions);
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
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSystemDefinitionFilerV2::h_SaveSystemDefinition(const C_OSCSystemDefinition & orc_SystemDefinition,
                                                          C_OSCXMLParserBase & orc_XMLParser)
{
   orc_XMLParser.CreateNodeChild("opensyde-system-definition");
   tgl_assert(orc_XMLParser.SelectRoot() == "opensyde-system-definition");
   //File version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XMLParser.SetNodeContent(C_SCLString::IntToStr(hu16_FILE_VERSION_LATEST));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
   //Node
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("nodes") == "nodes");
   h_SaveNodes(orc_SystemDefinition.c_Nodes, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");

   //Bus
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("buses") == "buses");
   h_SaveBuses(orc_SystemDefinition.c_Buses, orc_XMLParser);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-system-definition");
}
