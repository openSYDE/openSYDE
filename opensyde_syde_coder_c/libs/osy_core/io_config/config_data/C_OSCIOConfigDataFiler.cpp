//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load IO config data

   Load IO config data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <sstream>

#include "TGLFile.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCIOFileDataFiler.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCIOConfigDataFiler.h"
#include "C_OSCIOFileDataStructFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO description

   \param[out] orc_IOData   Storage
   \param[in]  orc_Path     Path to IO description
   \param[in]  orc_BasePath Base path

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_CHECKSUM  IO definition config version mismatch
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::h_LoadIODataFile(C_OSCIOConfigData & orc_IOData, const stw_scl::C_SCLString & orc_Path,
                                                const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadIOData(orc_IOData, c_XMLParser, orc_BasePath);
      }
      else
      {
         osc_write_log_error("Loading IO data",
                             "File \"" + orc_Path + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "File \"" + orc_Path + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save IO description

   Save IO description
   Will overwrite the file if it already exists.

   \param[in]     orc_IOData       Pointer to storage
   \param[in]     orc_Path         Path of system definition
   \param[in]     orc_BasePath     Base path
   \param[in,out] opc_CreatedFiles Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::h_SaveIODataFile(const C_OSCIOConfigData & orc_IOData,
                                                const stw_scl::C_SCLString & orc_Path,
                                                const stw_scl::C_SCLString & orc_BasePath,
                                                std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      //erase it:
      sintn sn_Return;
      sn_Return = std::remove(orc_Path.c_str());
      if (sn_Return != 0)
      {
         osc_write_log_error("Saving IO data", "Could not erase pre-existing file \"" + orc_Path + "\".");
         s32_Retval = C_RD_WR;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      const stw_scl::C_SCLString c_Folder = TGL_ExtractFilePath(orc_Path);
      if (TGL_DirectoryExists(c_Folder) == false)
      {
         if (TGL_CreateDirectory(c_Folder) != 0)
         {
            osc_write_log_error("Saving IO data", "Could not create folder \"" + c_Folder + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = mh_SaveIOData(orc_IOData, c_XMLParser, orc_BasePath, opc_CreatedFiles);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = c_XMLParser.SaveToFile(orc_Path);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Saving IO data", "Could not write to file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCIOConfigDataFiler::C_OSCIOConfigDataFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO data

   \param[in]     orc_IOData       Pointer to storage
   \param[in,out] orc_XMLParser    XML with default state
   \param[in]     orc_BasePath     Base path
   \param[in,out] opc_CreatedFiles Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOData(const C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                             const stw_scl::C_SCLString & orc_BasePath,
                                             std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval;

   orc_XMLParser.CreateNodeChild("opensyde-node-io-config");
   tgl_assert(orc_XMLParser.SelectRoot() == "opensyde-node-io-config");
   //File version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XMLParser.SetNodeContent(stw_scl::C_SCLString::IntToStr(hu16_FILE_VERSION_1));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
   //Content version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("ref-content-version") == "ref-content-version");
   orc_XMLParser.SetNodeContent(stw_scl::C_SCLString::IntToStr(orc_IOData.u32_ContentVersion));
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
   //IO data
   s32_Retval = mh_SaveIODataBase(orc_IOData, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
   if (s32_Retval == C_NO_ERR)
   {
      //Domains
      s32_Retval = mh_SaveIODomains(orc_IOData, orc_XMLParser);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO data base

   \param[in]     orc_IOData       IO data
   \param[in,out] orc_XMLParser    XML parser
   \param[in]     orc_BasePath     Base path
   \param[in,out] opc_CreatedFiles Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIODataBase(const C_OSCIOFileDataBase & orc_IOData,
                                                 C_OSCXMLParserBase & orc_XMLParser,
                                                 const stw_scl::C_SCLString & orc_BasePath,
                                                 std::vector<stw_scl::C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("io-base-file") == "io-base-file");
   const stw_scl::C_SCLString c_IODataFileName = "io_file.xml";
   const stw_scl::C_SCLString c_CombinedFileName = C_OSCSystemFilerUtil::mh_CombinePaths(orc_BasePath,
                                                                                         c_IODataFileName);
   s32_Retval = C_OSCIOFileDataFiler::h_SaveIODataFile(orc_IOData, c_CombinedFileName);
   if (s32_Retval != C_NO_ERR)
   {
      //More details are in log
      s32_Retval = C_CONFIG;
   }
   else
   {
      if (opc_CreatedFiles != NULL)
      {
         opc_CreatedFiles->push_back(c_CombinedFileName);
      }
      orc_XMLParser.SetNodeContent(c_IODataFileName);

      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO domains

   \param[in]     orc_IOData    IO data
   \param[in,out] orc_XMLParser XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIODomains(const C_OSCIOConfigData & orc_IOData,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domains") == "domains");
   orc_XMLParser.SetAttributeUint32("length", orc_IOData.GetDomainSize());
   for (uint32 u32_ItDomain = 0UL; (u32_ItDomain < orc_IOData.GetDomainSize()) && (s32_Retval == C_NO_ERR);
        ++u32_ItDomain)
   {
      const C_OSCIOConfigDataDomain * const pc_Domain = orc_IOData.GetDomainConfigDataConst(u32_ItDomain);
      if (pc_Domain != NULL)
      {
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domain") == "domain");
         //Channels
         s32_Retval = mh_SaveIOChannels(pc_Domain->c_ChannelConfigs, orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "domains");
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
         osc_write_log_error("Saving IO data",
                             stw_scl::C_SCLString(stw_scl::C_SCLString("Could not find domain ") +
                                                  stw_scl::C_SCLString::IntToStr(u32_ItDomain) + ".").c_str());
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO channels

   \param[in]     orc_Channels  Channels
   \param[in,out] orc_XMLParser XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOChannels(const std::vector<C_OSCIOConfigDataChannel> & orc_Channels,
                                                 C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channels") == "channels");
   orc_XMLParser.SetAttributeUint32("length", orc_Channels.size());
   for (uint32 u32_ItChannel = 0UL; (u32_ItChannel < orc_Channels.size()) && (s32_Retval == C_NO_ERR); ++u32_ItChannel)
   {
      s32_Retval = mh_SaveIOChannel(orc_Channels[u32_ItChannel], orc_XMLParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO channel

   \param[in]     orc_Channel   Channel
   \param[in,out] orc_XMLParser XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOChannel(const C_OSCIOConfigDataChannel & orc_Channel,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channel") == "channel");
   orc_XMLParser.SetAttributeBool("active", orc_Channel.q_Active);
   orc_XMLParser.SetAttributeBool("safety-relevant", orc_Channel.q_SafetyRelevant);
   orc_XMLParser.SetAttributeBool("datablock-set", orc_Channel.q_DatablockSet);
   orc_XMLParser.SetAttributeUint32("datablock-index", orc_Channel.u32_DatablockIndex);
   orc_XMLParser.CreateNodeChild("name", orc_Channel.c_Name.c_str());
   orc_XMLParser.CreateNodeChild("comment", orc_Channel.c_Comment.c_str());
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("datablock-configs") == "datablock-configs");
   orc_XMLParser.SetAttributeUint32("length", orc_Channel.c_DatablockConfig.size());
   for (uint32 u32_ItDb = 0UL; u32_ItDb < orc_Channel.c_DatablockConfig.size(); ++u32_ItDb)
   {
      orc_XMLParser.CreateNodeChild("datablock-config",
                                    mh_GetStringForDbConfig(orc_Channel.c_DatablockConfig[u32_ItDb]));
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "channel");
   s32_Retval = mh_SaveIOParameterStructs(orc_Channel.c_Parameters, orc_XMLParser);
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "channels");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO parameter structs

   \param[in]     orc_ParameterStructs Parameter structs
   \param[in,out] orc_XMLParser        XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOParameterStructs(
   const std::vector<C_OSCIOConfigDataParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("parameter-structs") == "parameter-structs");
   orc_XMLParser.SetAttributeUint32("length", orc_ParameterStructs.size());
   for (uint32 u32_ItParameter = 0UL; (u32_ItParameter < orc_ParameterStructs.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItParameter)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("parameter-struct") == "parameter-struct");
      s32_Retval = mh_SaveIOParameterStruct(orc_ParameterStructs[u32_ItParameter], orc_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "parameter-structs");
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "channel");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO parameter struct

   \param[in]     orc_ParameterStruct Parameter struct
   \param[in,out] orc_XMLParser       XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOParameterStruct(const C_OSCIOConfigDataParameterStruct & orc_ParameterStruct,
                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   if (orc_ParameterStruct.c_ParameterElements.size() > 0UL)
   {
      s32_Retval = mh_SaveIOParameters(orc_ParameterStruct.c_ParameterElements, orc_XMLParser);
   }
   else
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("single-value") == "single-value");
      s32_Retval = mh_SaveIOParameter(orc_ParameterStruct, orc_XMLParser, "single-value");
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "parameter-struct");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO parameters

   \param[in]     orc_Parameters Parameters
   \param[in,out] orc_XMLParser  XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOParameters(const std::vector<C_OSCIOConfigDataParameter> & orc_Parameters,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("parameters") == "parameters");
   orc_XMLParser.SetAttributeUint32("length", orc_Parameters.size());
   for (uint32 u32_ItParameter = 0UL; (u32_ItParameter < orc_Parameters.size()) && (s32_Retval == C_NO_ERR);
        ++u32_ItParameter)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("parameter") == "parameter");
      s32_Retval = mh_SaveIOParameter(orc_Parameters[u32_ItParameter], orc_XMLParser, "parameter");
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "parameters");
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "parameter-struct");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO parameter

   \param[in]     orc_Parameter Parameter
   \param[in,out] orc_XMLParser XML parser
   \param[in]     orc_BaseNode  Base node name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_SaveIOParameter(const C_OSCIOConfigDataParameter & orc_Parameter,
                                                  C_OSCXMLParserBase & orc_XMLParser,
                                                  const stw_scl::C_SCLString & orc_BaseNode)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("value") == "value");
   switch (orc_Parameter.c_Value.GetComplexType())
   {
   case C_OSCIOFileDataContent::eCT_PLAIN:
      orc_XMLParser.SetAttributeString("type",
                                       C_OSCIOFileDataStructFiler::h_GetTypeString(orc_Parameter.c_Value.GetType()));
      break;
   case C_OSCIOFileDataContent::eCT_ENUM:
      orc_XMLParser.SetAttributeString("type", "enum");
      orc_XMLParser.SetAttributeString("base-type",
                                       C_OSCIOFileDataStructFiler::h_GetTypeString(orc_Parameter.c_Value.GetType()));
      break;
   case C_OSCIOFileDataContent::eCT_BIT_MASK:
      orc_XMLParser.SetAttributeString("type", "bitmask");
      orc_XMLParser.SetAttributeString("base-type",
                                       C_OSCIOFileDataStructFiler::h_GetTypeString(orc_Parameter.c_Value.GetType()));
      break;
   }
   orc_XMLParser.SetAttributeBool("is-array", orc_Parameter.c_Value.GetArray());
   orc_XMLParser.SetAttributeUint32("array-size", orc_Parameter.c_Value.GetArraySize());
   s32_Retval = C_OSCIOFileDataStructFiler::h_SaveSimpleValueAsAttribute("value", orc_XMLParser, orc_Parameter.c_Value);
   if (s32_Retval == C_NO_ERR)
   {
      const std::map<stw_scl::C_SCLString,
                     C_OSCNodeDataPoolContent> & rc_EnumItems = orc_Parameter.c_Value.GetEnumItems();
      for (std::map<stw_scl::C_SCLString, C_OSCNodeDataPoolContent>::const_iterator c_It = rc_EnumItems.begin();
           (c_It != rc_EnumItems.end()) && (s32_Retval == C_NO_ERR); ++c_It)
      {
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("enum-item") == "enum-item");
         orc_XMLParser.SetAttributeString("display", c_It->first);
         s32_Retval = C_OSCIOFileDataStructFiler::h_SaveSimpleValueAsAttribute("value", orc_XMLParser, c_It->second);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "value");
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      const std::vector<C_OSCIOFileDataContentBitmaskItem> & rc_BitmaskItems = orc_Parameter.c_Value.GetBitmaskItems();
      for (uint32 u32_ItBitMask = 0UL; u32_ItBitMask < rc_BitmaskItems.size(); ++u32_ItBitMask)
      {
         std::stringstream c_Mask;
         const C_OSCIOFileDataContentBitmaskItem & rc_BitmaskItem = rc_BitmaskItems[u32_ItBitMask];
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bitmask-selection") == "bitmask-selection");
         orc_XMLParser.SetAttributeString("display", rc_BitmaskItem.c_Display);
         orc_XMLParser.SetAttributeBool("initial-apply-value-setting", rc_BitmaskItem.q_ApplyValueSetting);
         c_Mask << "0x" << &std::hex << rc_BitmaskItem.u64_Value;
         orc_XMLParser.SetAttributeString("value", c_Mask.str().c_str());
         orc_XMLParser.CreateNodeChild("comment", rc_BitmaskItem.c_Comment);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "value");
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_BaseNode);
   }
   if (s32_Retval == C_NO_ERR)
   {
      orc_XMLParser.CreateNodeChild("comment", orc_Parameter.c_Comment);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get string for db config

   \param[in] oe_Config Config

   \return
   String for db config
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCIOConfigDataFiler::mh_GetStringForDbConfig(
   const C_OSCIOConfigDataChannel::E_DataBlockConfig oe_Config)
{
   stw_scl::C_SCLString c_Retval;
   switch (oe_Config)
   {
   case C_OSCIOConfigDataChannel::eDC_READ_WRITE:
      c_Retval = "read-write";
      break;
   case C_OSCIOConfigDataChannel::eDC_READ_ONLY:
      c_Retval = "read-only";
      break;
   case C_OSCIOConfigDataChannel::eDC_NONE:
      c_Retval = "none";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get db config for string

   \param[in]  orc_Str    Str
   \param[out] ore_Config Config

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_GetDbConfigForString(const stw_scl::C_SCLString & orc_Str,
                                                       C_OSCIOConfigDataChannel::E_DataBlockConfig & ore_Config)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_Str == "read-write")
   {
      ore_Config = C_OSCIOConfigDataChannel::eDC_READ_WRITE;
   }
   else if (orc_Str == "read-only")
   {
      ore_Config = C_OSCIOConfigDataChannel::eDC_READ_ONLY;
   }
   else if (orc_Str == "none")
   {
      ore_Config = C_OSCIOConfigDataChannel::eDC_NONE;
   }
   else
   {
      osc_write_log_error("Loading IO description",
                          stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                  "Invalid content \"") + orc_Str +
                                               "\" of  \"datablock-config\" node.").c_str());
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO description

   \param[out]    orc_IOData    Storage
   \param[in,out] orc_XMLParser XML with default state
   \param[in]     orc_BasePath  Base path


   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
   C_CHECKSUM  IO definition config version mismatch
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIOData(C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                             const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectRoot() == "opensyde-node-io-config")
   {
      s32_Retval = mh_LoadIODataBase(orc_IOData, orc_XMLParser, orc_BasePath);
      if (s32_Retval == C_NO_ERR)
      {
         if (orc_XMLParser.SelectNodeChild("ref-content-version") == "ref-content-version")
         {
            uint32 u32_RefId = 0UL;
            try
            {
               u32_RefId = static_cast<uint16>(orc_XMLParser.GetNodeContent().ToInt());
            }
            catch (...)
            {
               osc_write_log_error("Loading IO description",
                                   "\"ref-content-version\" could not be converted to a number.");
               s32_Retval = C_CONFIG;
            }
            if (s32_Retval == C_NO_ERR)
            {
               if (u32_RefId != orc_IOData.u32_ContentVersion)
               {
                  osc_write_log_error("Loading IO description",
                                      stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                              "\"ref-content-version\" mismatch between expected ") +
                                                           stw_scl::C_SCLString::IntToStr(u32_RefId) +
                                                           " and current " +
                                                           stw_scl::C_SCLString::IntToStr(orc_IOData.u32_ContentVersion)
                                                           +
                                                           ".").c_str());
                  s32_Retval = C_CHECKSUM;
               }
            }
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
            }
         }
         else
         {
            osc_write_log_error("Loading IO description", "Could not find \"ref-content-version\" node.");
            s32_Retval = C_CONFIG;
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = mh_LoadIODomains(orc_IOData, orc_XMLParser);
         }
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"opensyde-node-io-config\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO data base

   \param[in,out] orc_IOData    IO data
   \param[in,out] orc_XMLParser XML parser
   \param[in]     orc_BasePath  Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIODataBase(C_OSCIOFileDataBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                                 const stw_scl::C_SCLString & orc_BasePath)
{
   sint32 s32_Retval;

   if (orc_XMLParser.SelectNodeChild("io-base-file") == "io-base-file")
   {
      s32_Retval =
         C_OSCIOFileDataFiler::h_LoadIODataFile(orc_IOData,
                                                C_OSCSystemFilerUtil::mh_CombinePaths(orc_BasePath,
                                                                                      orc_XMLParser.
                                                                                      GetNodeContent()));
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
      }
      else
      {
         //More details are in log
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"io-base-file\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO domains

   \param[in,out] orc_IOData    IO data
   \param[in,out] orc_XMLParser XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIODomains(C_OSCIOConfigData & orc_IOData, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("domains") == "domains")
   {
      if (orc_XMLParser.AttributeExists("length"))
      {
         const uint32 u32_ExpectedCount = orc_XMLParser.GetAttributeUint32("length");
         uint32 u32_ActualCount = 0UL;
         stw_scl::C_SCLString c_NodeDomain = orc_XMLParser.SelectNodeChild("domain");
         if (c_NodeDomain == "domain")
         {
            do
            {
               const C_OSCIOConfigDataDomain * const pc_Domain = orc_IOData.GetDomainConfigDataConst(u32_ActualCount);
               if (pc_Domain != NULL)
               {
                  C_OSCIOConfigDataDomain c_DomainCopy = *pc_Domain;
                  s32_Retval = mh_LoadIODomain(c_DomainCopy, orc_XMLParser);
                  if (s32_Retval == C_NO_ERR)
                  {
                     tgl_assert(orc_IOData.SetDomainConfig(u32_ActualCount, c_DomainCopy) == C_NO_ERR);
                     //Count
                     ++u32_ActualCount;
                     //Iterate
                     c_NodeDomain = orc_XMLParser.SelectNodeNext("domain");
                  }
               }
               else
               {
                  osc_write_log_error("Loading IO data", "Domain count higher than in io spec file");
                  s32_Retval = C_CONFIG;
               }
            }
            while ((c_NodeDomain == "domain") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "domains");
            }
         }
         if (u32_ExpectedCount != u32_ActualCount)
         {
            stw_scl::C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected domain count, expected: %i, got %i", u32_ExpectedCount, u32_ActualCount);
            osc_write_log_warning("Loading IO data", c_Tmp.c_str());
         }
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"length\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "opensyde-node-io-config");
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"domains\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO domain

   \param[in,out] orc_IODomain       IO domain
   \param[in]     orc_ParametersFile Current parameters spec
   \param[in,out] orc_XMLParser      XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIODomain(C_OSCIOConfigDataDomain & orc_IODomain,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("channels") == "channels")
   {
      if (orc_XMLParser.AttributeExists("length"))
      {
         const uint32 u32_ExpectedCount = orc_XMLParser.GetAttributeUint32("length");
         uint32 u32_ActualCount = 0UL;
         stw_scl::C_SCLString c_NodeChannel = orc_XMLParser.SelectNodeChild("channel");
         //Clear any existing configuration
         orc_IODomain.c_ChannelConfigs.clear();
         if (c_NodeChannel == "channel")
         {
            do
            {
               C_OSCIOConfigDataChannel c_Channel;
               s32_Retval = mh_LoadIOChannel(c_Channel, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_IODomain.c_ChannelConfigs.push_back(c_Channel);
                  //Count
                  ++u32_ActualCount;
                  //Iterate
                  c_NodeChannel = orc_XMLParser.SelectNodeNext("channel");
               }
            }
            while ((c_NodeChannel == "channel") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "channels");
            }
         }
         if (u32_ExpectedCount != u32_ActualCount)
         {
            stw_scl::C_SCLString c_Tmp;
            c_Tmp.PrintFormatted("Unexpected channel count, expected: %i, got %i", u32_ExpectedCount, u32_ActualCount);
            osc_write_log_warning("Loading IO data", c_Tmp.c_str());
         }
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"length\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"channels\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO channel

   \param[in,out] orc_IOChannel      IO channel
   \param[in]     orc_ParametersFile Current parameters spec
   \param[in,out] orc_XMLParser      XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIOChannel(C_OSCIOConfigDataChannel & orc_IOChannel,
                                                C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("active"))
   {
      orc_IOChannel.q_Active = orc_XMLParser.GetAttributeBool("active");
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"active\" attribute.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("safety-relevant"))
      {
         orc_IOChannel.q_SafetyRelevant = orc_XMLParser.GetAttributeBool("safety-relevant");
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"safety-relevant\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("datablock-set"))
      {
         orc_IOChannel.q_DatablockSet = orc_XMLParser.GetAttributeBool("datablock-set");
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"datablock-set\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("datablock-index"))
      {
         orc_IOChannel.u32_DatablockIndex = orc_XMLParser.GetAttributeUint32("datablock-index");
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"datablock-index\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_IOChannel.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "channel");
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"name\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_IOChannel.c_Comment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "channel");
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"comment\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadIODatablockConfigs(orc_IOChannel.c_DatablockConfig, orc_XMLParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadIOParameterStructs(orc_IOChannel.c_Parameters, orc_XMLParser);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO datablock configs

   \param[in,out] orc_DatablockConfigs Datablock configs
   \param[in,out] orc_XMLParser        XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIODatablockConfigs(
   std::vector<C_OSCIOConfigDataChannel::E_DataBlockConfig> & orc_DatablockConfigs, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("datablock-configs") == "datablock-configs")
   {
      uint32 u32_ExpectedLength = 0UL;
      orc_DatablockConfigs.clear();
      //Reserve
      if (orc_XMLParser.AttributeExists("length"))
      {
         u32_ExpectedLength = orc_XMLParser.GetAttributeUint32("length");
         orc_DatablockConfigs.reserve(u32_ExpectedLength);
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"length\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_CurrentDbNode = orc_XMLParser.SelectNodeChild("datablock-config");
         if (c_CurrentDbNode == "datablock-config")
         {
            do
            {
               C_OSCIOConfigDataChannel::E_DataBlockConfig eConfig;
               s32_Retval = mh_GetDbConfigForString(orc_XMLParser.GetNodeContent(), eConfig);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_DatablockConfigs.push_back(eConfig);
                  //Iterate
                  c_CurrentDbNode = orc_XMLParser.SelectNodeNext("datablock-config");
               }
            }
            while ((c_CurrentDbNode == "datablock-config") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "datablock-configs");
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Check length
            if (u32_ExpectedLength != orc_DatablockConfigs.size())
            {
               osc_write_log_error("Loading IO data",
                                   stw_scl::C_SCLString(stw_scl::C_SCLString("Content of \"length\" attribute (") +
                                                        stw_scl::C_SCLString::IntToStr(u32_ExpectedLength) +
                                                        ") mismatches with the number of found data blocks (" +
                                                        stw_scl::C_SCLString::IntToStr(
                                                           orc_DatablockConfigs.size()) + ").").c_str());
               s32_Retval = C_CONFIG;
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "channel");
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"datablock-configs\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameter structs

   \param[in,out] orc_ParameterStructs Parameter structs
   \param[in,out] orc_XMLParser        XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIOParameterStructs(
   std::vector<C_OSCIOConfigDataParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("parameter-structs") == "parameter-structs")
   {
      uint32 u32_ExpectedLength = 0UL;
      //Clean up existing
      orc_ParameterStructs.clear();
      //Reserve
      if (orc_XMLParser.AttributeExists("length"))
      {
         u32_ExpectedLength = orc_XMLParser.GetAttributeUint32("length");
         orc_ParameterStructs.reserve(u32_ExpectedLength);
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"length\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_CurrentParameterNode = orc_XMLParser.SelectNodeChild("parameter-struct");
         if (c_CurrentParameterNode == "parameter-struct")
         {
            do
            {
               C_OSCIOConfigDataParameterStruct c_Parameter;
               s32_Retval = mh_LoadIOParameterStruct(c_Parameter, orc_XMLParser);
               if (s32_Retval == C_NO_ERR)
               {
                  orc_ParameterStructs.push_back(c_Parameter);
                  //Iterate
                  c_CurrentParameterNode = orc_XMLParser.SelectNodeNext("parameter-struct");
               }
            }
            while ((c_CurrentParameterNode == "parameter-struct") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "parameter-structs");
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Check length
            if (u32_ExpectedLength != orc_ParameterStructs.size())
            {
               osc_write_log_error("Loading IO data",
                                   stw_scl::C_SCLString(stw_scl::C_SCLString("Content of \"length\" attribute (") +
                                                        stw_scl::C_SCLString::IntToStr(u32_ExpectedLength) +
                                                        ") mismatches with the number of found parameters (" +
                                                        stw_scl::C_SCLString::IntToStr(
                                                           orc_ParameterStructs.size()) + ").").c_str());
               s32_Retval = C_CONFIG;
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "channel");
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"parameter-structs\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameter struct

   \param[in,out] orc_ParameterStruct Parameter struct
   \param[in,out] orc_XMLParser       XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIOParameterStruct(C_OSCIOConfigDataParameterStruct & orc_ParameterStruct,
                                                        C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = mh_LoadIOParameters(orc_ParameterStruct.c_ParameterElements, orc_XMLParser);

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("single-value") == "single-value")
      {
         s32_Retval = mh_LoadIOParameter(orc_ParameterStruct, orc_XMLParser, "single-value");
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "parameter-struct");
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameters

   \param[in,out] orc_Parameters Parameters
   \param[in,out] orc_XMLParser  XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIOParameters(std::vector<C_OSCIOConfigDataParameter> & orc_Parameters,
                                                   C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("parameters") == "parameters")
   {
      uint32 u32_ExpectedLength = 0UL;
      //Reserve
      if (orc_XMLParser.AttributeExists("length"))
      {
         u32_ExpectedLength = orc_XMLParser.GetAttributeUint32("length");
         orc_Parameters.reserve(u32_ExpectedLength);
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"length\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_CurrentParameterNode = orc_XMLParser.SelectNodeChild("parameter");
         if (c_CurrentParameterNode == "parameter")
         {
            do
            {
               C_OSCIOConfigDataParameter c_Parameter;
               s32_Retval = mh_LoadIOParameter(c_Parameter, orc_XMLParser, "parameter");
               if (s32_Retval == C_NO_ERR)
               {
                  orc_Parameters.push_back(c_Parameter);
                  //Iterate
                  c_CurrentParameterNode = orc_XMLParser.SelectNodeNext("parameter");
               }
            }
            while ((c_CurrentParameterNode == "parameter") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "parameters");
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Check length
            if (u32_ExpectedLength != orc_Parameters.size())
            {
               osc_write_log_error("Loading IO data",
                                   stw_scl::C_SCLString(stw_scl::C_SCLString("Content of \"length\" attribute (") +
                                                        stw_scl::C_SCLString::IntToStr(u32_ExpectedLength) +
                                                        ") mismatches with the number of found parameters (" +
                                                        stw_scl::C_SCLString::IntToStr(
                                                           orc_Parameters.size()) + ").").c_str());
               s32_Retval = C_CONFIG;
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "parameter-struct");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameter

   \param[in,out] orc_Parameter Parameter
   \param[in,out] orc_XMLParser XML parser
   \param[in]     orc_BaseName  Base name

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCIOConfigDataFiler::mh_LoadIOParameter(C_OSCIOConfigDataParameter & orc_Parameter,
                                                  C_OSCXMLParserBase & orc_XMLParser,
                                                  const stw_scl::C_SCLString & orc_BaseName)
{
   sint32 s32_Retval = C_NO_ERR;

   stw_scl::C_SCLString c_TypeStr;
   stw_scl::C_SCLString c_BaseTypeStr;

   if (orc_XMLParser.SelectNodeChild("value") == "value")
   {
      s32_Retval = C_OSCIOFileDataStructFiler::h_SetType(orc_XMLParser, orc_Parameter.c_Value, c_TypeStr,
                                                         c_BaseTypeStr, "value",
                                                         "Loading IO description");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OSCIOFileDataStructFiler::h_ParseAttributeIntoContent(orc_Parameter.c_Value, orc_XMLParser,
                                                                              "value",
                                                                              c_TypeStr,
                                                                              c_BaseTypeStr, true, "value");
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_BaseName);
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"value\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("comment") == "comment")
      {
         orc_Parameter.c_Comment = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_BaseName);
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"comment\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}
