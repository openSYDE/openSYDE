//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC configuration data

   Load HALC configuration data

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <sstream>

#include "TGLFile.h"
#include "stwerrors.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCHalcDefFiler.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCHalcConfigFiler.h"
#include "C_OSCHalcDefStructFiler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
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

   \param[out]  orc_IOData    Storage
   \param[in]   orc_Path      Path to IO description
   \param[in]   orc_BasePath  Base path

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    system definition file content is invalid or incomplete
               device definition file could not be loaded
   C_CHECKSUM  HALC configuration config version mismatch
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::h_LoadFile(C_OSCHalcConfig & orc_IOData, const C_SCLString & orc_Path,
                                        const C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XMLParser.SelectRoot() == "opensyde-node-io-config")
         {
            s32_Retval = h_LoadData(orc_IOData, c_XMLParser, orc_BasePath);
         }
         else
         {
            osc_write_log_error("Loading IO data", "Could not find \"opensyde-node-io-config\" node.");
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Loading IO data", "File \"" + orc_Path + "\" could not be opened.");
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
/*! \brief  Save IO description

   Save IO description
   Will overwrite the file if it already exists.

   \param[in]      orc_IOData          Pointer to storage
   \param[in]      orc_Path            Path of system definition
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::h_SaveFile(const C_OSCHalcConfig & orc_IOData, const C_SCLString & orc_Path,
                                        const C_SCLString & orc_BasePath,
                                        std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval = C_OSCHalcConfigFiler::h_PrepareForFile(orc_Path);

   if (s32_Retval == C_NO_ERR)
   {
      C_OSCXMLParser c_XMLParser;
      c_XMLParser.CreateNodeChild("opensyde-node-io-config");
      tgl_assert(c_XMLParser.SelectRoot() == "opensyde-node-io-config");
      s32_Retval = h_SaveData(orc_IOData, c_XMLParser, orc_BasePath, opc_CreatedFiles);
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
/*! \brief  Load IO description

   \param[out]     orc_IOData       Storage
   \param[in,out]  orc_XMLParser    XML with default state
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
   C_CHECKSUM  HALC configuration config version mismatch
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::h_LoadData(C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                        const C_SCLString & orc_BasePath)
{
   sint32 s32_Retval = mh_LoadIODataBase(orc_IOData, orc_XMLParser, orc_BasePath);

   //File version
   if (orc_XMLParser.SelectNodeChild("file-version") == "file-version")
   {
      uint16 u16_FileVersion = 0U;
      try
      {
         u16_FileVersion = static_cast<uint16>(orc_XMLParser.GetNodeContent().ToInt());
      }
      catch (...)
      {
         osc_write_log_error("Loading IO description", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading IO description", "Value of \"file-version\": " +
                            C_SCLString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != mhu16_FILE_VERSION_1)
         {
            osc_write_log_error("Loading IO description",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading IO description", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
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
                                   "\"ref-content-version\" mismatch between expected " +
                                   C_SCLString::IntToStr(u32_RefId) +
                                   " and current " + C_SCLString::IntToStr(orc_IOData.u32_ContentVersion) + ".");
               s32_Retval = C_CHECKSUM;
            }
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            orc_XMLParser.SelectNodeParent();
         }
      }
      else
      {
         osc_write_log_error("Loading IO description", "Could not find \"ref-content-version\" node.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         if (orc_XMLParser.SelectNodeChild("general") == "general")
         {
            // No general section data. Nothing to do.
            //Return
            orc_XMLParser.SelectNodeParent();
         }
         else
         {
            osc_write_log_error("Loading IO description", "Could not find \"general\" node.");
            s32_Retval = C_CONFIG;
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = mh_LoadIODomains(orc_IOData, orc_XMLParser);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO data

   \param[in]      orc_IOData          Pointer to storage
   \param[in,out]  orc_XMLParser       XML with default state
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::h_SaveData(const C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                        const C_SCLString & orc_BasePath,
                                        std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval;

   //File version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XMLParser.SetNodeContent(C_SCLString::IntToStr(mhu16_FILE_VERSION_1));
   //Return
   orc_XMLParser.SelectNodeParent();
   //Content version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("ref-content-version") == "ref-content-version");
   orc_XMLParser.SetNodeContent(C_SCLString::IntToStr(orc_IOData.u32_ContentVersion));
   //Return
   orc_XMLParser.SelectNodeParent();
   //IO data
   s32_Retval = mh_SaveIODataBase(orc_IOData, orc_XMLParser, orc_BasePath, opc_CreatedFiles);
   if (s32_Retval == C_NO_ERR)
   {
      {
         //General
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("general") == "general");
         // Datablock assignment is now only in Datapools. Fill structure with dummies to be able to open HALC projects
         // always in older tool versions too.
         orc_XMLParser.SetAttributeBool("is-safe-datablock-set", false);
         orc_XMLParser.SetAttributeUint32("safe-datablock-index", 0);
         orc_XMLParser.SetAttributeBool("is-unsafe-datablock-set", false);
         orc_XMLParser.SetAttributeUint32("unsafe-datablock-index", 0);
         //Return
         orc_XMLParser.SelectNodeParent();
      }

      //Domains
      s32_Retval = mh_SaveIODomains(orc_IOData, orc_XMLParser);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Make preparations for file

   - Removing an already existing file
   - Creating the directory if it does not exist

   \param[in]  orc_Path    Path of file

   \return
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
stw_types::sint32 C_OSCHalcConfigFiler::h_PrepareForFile(const C_SCLString & orc_Path)
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
      const C_SCLString c_Folder = TGL_ExtractFilePath(orc_Path);
      if (TGL_DirectoryExists(c_Folder) == false)
      {
         if (TGL_CreateDirectory(c_Folder) != 0)
         {
            osc_write_log_error("Saving IO data", "Could not create folder \"" + c_Folder + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO domain

   \param[in]      orc_IODomain     IO data
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::h_SaveIODomain(const C_OSCHalcConfigDomain & orc_IODomain,
                                            C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domain") == "domain");
   //Config
   s32_Retval = mh_SaveIOChannel(orc_IODomain.c_DomainConfig, orc_XMLParser, "config", "domain");
   //Channels
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_SaveIOChannels(orc_IODomain.c_ChannelConfigs, orc_XMLParser);
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO domain

   \param[in,out]  orc_IODomain     IO domain
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::h_LoadIODomain(C_OSCHalcConfigDomain & orc_IODomain, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("config") == "config")
   {
      s32_Retval = mh_LoadIOChannel(orc_IODomain.c_DomainConfig, orc_XMLParser, "config");
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"config\" node.");
      s32_Retval = C_CONFIG;
   }
   if (orc_XMLParser.SelectNodeChild("channels") == "channels")
   {
      if (orc_XMLParser.AttributeExists("length"))
      {
         const uint32 u32_ExpectedCount = orc_XMLParser.GetAttributeUint32("length");
         uint32 u32_ActualCount = 0UL;
         C_SCLString c_NodeChannel = orc_XMLParser.SelectNodeChild("channel");
         //Clear any existing configuration
         orc_IODomain.c_ChannelConfigs.clear();
         if (c_NodeChannel == "channel")
         {
            do
            {
               C_OSCHalcConfigChannel c_Channel;
               s32_Retval = mh_LoadIOChannel(c_Channel, orc_XMLParser, "channel");
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
            C_SCLString c_Tmp;
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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigFiler::C_OSCHalcConfigFiler()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO data base

   \param[in]      orc_IOData          IO data
   \param[in,out]  orc_XMLParser       XML parser
   \param[in]      orc_BasePath        Base path
   \param[in,out]  opc_CreatedFiles    Created files

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIODataBase(const C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                               const C_SCLString & orc_BasePath,
                                               std::vector<C_SCLString> * const opc_CreatedFiles)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("io-base-file") == "io-base-file");
   if (orc_BasePath.IsEmpty())
   {
      //To string
      s32_Retval = C_OSCHalcDefFiler::h_SaveData(orc_IOData, orc_XMLParser);
   }
   else
   {
      const C_SCLString c_IODataFileName =
         (orc_IOData.c_OriginalFileName.IsEmpty() == false) ?
         orc_IOData.c_OriginalFileName : static_cast<C_SCLString>("halc_definition_file.xml");
      const C_SCLString c_CombinedFileName = C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                                  c_IODataFileName);
      s32_Retval = C_OSCHalcDefFiler::h_SaveFile(orc_IOData, c_CombinedFileName);
      if (s32_Retval != C_NO_ERR)
      {
         //More details are in log
         s32_Retval = C_CONFIG;
      }
      else
      {
         if (opc_CreatedFiles != NULL)
         {
            opc_CreatedFiles->push_back(c_IODataFileName);
         }
         orc_XMLParser.SetNodeContent(c_IODataFileName);
      }
   }
   //Return
   if (s32_Retval == C_NO_ERR)
   {
      orc_XMLParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO domains

   \param[in]      orc_IOData       IO data
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIODomains(const C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domains") == "domains");
   orc_XMLParser.SetAttributeUint32("length", orc_IOData.GetDomainSize());
   for (uint32 u32_ItDomain = 0UL; (u32_ItDomain < orc_IOData.GetDomainSize()) && (s32_Retval == C_NO_ERR);
        ++u32_ItDomain)
   {
      const C_OSCHalcConfigDomain * const pc_Domain = orc_IOData.GetDomainConfigDataConst(u32_ItDomain);
      if (pc_Domain != NULL)
      {
         s32_Retval = C_OSCHalcConfigFiler::h_SaveIODomain(*pc_Domain, orc_XMLParser);

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
                             "Could not find domain " + C_SCLString::IntToStr(u32_ItDomain) + ".");
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      orc_XMLParser.SelectNodeParent();
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO channels

   \param[in]      orc_Channels     Channels
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIOChannels(const std::vector<C_OSCHalcConfigChannel> & orc_Channels,
                                               C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channels") == "channels");
   orc_XMLParser.SetAttributeUint32("length", orc_Channels.size());
   for (uint32 u32_ItChannel = 0UL; (u32_ItChannel < orc_Channels.size()) && (s32_Retval == C_NO_ERR); ++u32_ItChannel)
   {
      s32_Retval = mh_SaveIOChannel(orc_Channels[u32_ItChannel], orc_XMLParser, "channel", "channels");
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

   \param[in]      orc_Channel         Channel
   \param[in,out]  orc_XMLParser       XML parser
   \param[in]      orc_NodeName        Node name
   \param[in]      orc_NodeParentName  Node parent name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIOChannel(const C_OSCHalcConfigChannel & orc_Channel,
                                              C_OSCXMLParserBase & orc_XMLParser, const C_SCLString & orc_NodeName,
                                              const C_SCLString & orc_NodeParentName)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild(orc_NodeName) == orc_NodeName);
   orc_XMLParser.SetAttributeBool("safety-relevant", orc_Channel.q_SafetyRelevant);
   orc_XMLParser.SetAttributeUint32("use-case-index", orc_Channel.u32_UseCaseIndex);
   orc_XMLParser.CreateNodeChild("name", orc_Channel.c_Name.c_str());
   orc_XMLParser.CreateNodeChild("comment", orc_Channel.c_Comment.c_str());
   s32_Retval = mh_SaveIOParameterStructs(orc_Channel.c_Parameters, orc_XMLParser, orc_NodeName);
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_NodeParentName);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO parameter structs

   \param[in]      orc_ParameterStructs   Parameter structs
   \param[in,out]  orc_XMLParser          XML parser
   \param[in]      orc_NodeName           Node name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIOParameterStructs(
   const std::vector<C_OSCHalcConfigParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser,
   const C_SCLString & orc_NodeName)
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
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_NodeName);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO parameter struct

   \param[in]      orc_ParameterStruct    Parameter struct
   \param[in,out]  orc_XMLParser          XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIOParameterStruct(const C_OSCHalcConfigParameterStruct & orc_ParameterStruct,
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

   \param[in]      orc_Parameters   Parameters
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIOParameters(const std::vector<C_OSCHalcConfigParameter> & orc_Parameters,
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

   \param[in]      orc_Parameter    Parameter
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BaseNode     Base node name

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_SaveIOParameter(const C_OSCHalcConfigParameter & orc_Parameter,
                                                C_OSCXMLParserBase & orc_XMLParser, const C_SCLString & orc_BaseNode)
{
   sint32 s32_Retval;

   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("value") == "value");
   switch (orc_Parameter.c_Value.GetComplexType())
   {
   case C_OSCHalcDefContent::eCT_PLAIN:
      orc_XMLParser.SetAttributeString("type",
                                       C_OSCHalcDefStructFiler::h_GetTypeString(orc_Parameter.c_Value.GetType()));
      break;
   case C_OSCHalcDefContent::eCT_ENUM:
      orc_XMLParser.SetAttributeString("type", "enum");
      orc_XMLParser.SetAttributeString("base-type",
                                       C_OSCHalcDefStructFiler::h_GetTypeString(orc_Parameter.c_Value.GetType()));
      break;
   case C_OSCHalcDefContent::eCT_BIT_MASK:
      orc_XMLParser.SetAttributeString("type", "bitmask");
      orc_XMLParser.SetAttributeString("base-type",
                                       C_OSCHalcDefStructFiler::h_GetTypeString(orc_Parameter.c_Value.GetType()));
      break;
   default:
      break;
   }
   orc_XMLParser.SetAttributeBool("is-array", orc_Parameter.c_Value.GetArray());
   orc_XMLParser.SetAttributeUint32("array-size", orc_Parameter.c_Value.GetArraySize());
   s32_Retval = C_OSCHalcDefStructFiler::h_SaveSimpleValueAsAttribute("value", orc_XMLParser, orc_Parameter.c_Value);
   if (s32_Retval == C_NO_ERR)
   {
      const std::map<C_SCLString,
                     C_OSCNodeDataPoolContent> & rc_EnumItems = orc_Parameter.c_Value.GetEnumItems();
      for (std::map<C_SCLString, C_OSCNodeDataPoolContent>::const_iterator c_It = rc_EnumItems.begin();
           (c_It != rc_EnumItems.end()) && (s32_Retval == C_NO_ERR); ++c_It)
      {
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("enum-item") == "enum-item");
         orc_XMLParser.SetAttributeString("display", c_It->first);
         s32_Retval = C_OSCHalcDefStructFiler::h_SaveSimpleValueAsAttribute("value", orc_XMLParser, c_It->second);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "value");
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      const std::vector<C_OSCHalcDefContentBitmaskItem> & rc_BitmaskItems = orc_Parameter.c_Value.GetBitmaskItems();
      for (uint32 u32_ItBitMask = 0UL; u32_ItBitMask < rc_BitmaskItems.size(); ++u32_ItBitMask)
      {
         std::stringstream c_Mask;
         const C_OSCHalcDefContentBitmaskItem & rc_BitmaskItem = rc_BitmaskItems[u32_ItBitMask];
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("bitmask-selection") == "bitmask-selection");
         orc_XMLParser.SetAttributeString("display", rc_BitmaskItem.c_Display);
         orc_XMLParser.SetAttributeBool("initial-apply-value-setting", rc_BitmaskItem.q_ApplyValueSetting);
         c_Mask << "0x" << &std::hex << rc_BitmaskItem.u64_Value;
         orc_XMLParser.SetAttributeString("value", c_Mask.str().c_str());
         orc_XMLParser.CreateNodeChild("comment", rc_BitmaskItem.c_Comment);
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "value");
      }
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == orc_BaseNode);
      orc_XMLParser.CreateNodeChild("comment", orc_Parameter.c_Comment);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO data base

   \param[in,out]  orc_IOData       IO data
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BasePath     Base path

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIODataBase(C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser,
                                               const C_SCLString & orc_BasePath)
{
   sint32 s32_Retval;

   if (orc_XMLParser.SelectNodeChild("io-base-file") == "io-base-file")
   {
      if (orc_BasePath.IsEmpty())
      {
         //From string
         s32_Retval = C_OSCHalcDefFiler::h_LoadData(orc_IOData, orc_XMLParser);
      }
      else
      {
         s32_Retval =
            C_OSCHalcDefFiler::h_LoadFile(orc_IOData,
                                          C_OSCSystemFilerUtil::h_CombinePaths(orc_BasePath,
                                                                               orc_XMLParser.GetNodeContent()));
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         orc_XMLParser.SelectNodeParent();
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

   \param[in,out]  orc_IOData       IO data
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIODomains(C_OSCHalcConfig & orc_IOData, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.SelectNodeChild("domains") == "domains")
   {
      if (orc_XMLParser.AttributeExists("length"))
      {
         const uint32 u32_ExpectedCount = orc_XMLParser.GetAttributeUint32("length");
         uint32 u32_ActualCount = 0UL;
         C_SCLString c_NodeDomain = orc_XMLParser.SelectNodeChild("domain");
         if (c_NodeDomain == "domain")
         {
            do
            {
               const C_OSCHalcConfigDomain * const pc_Domain = orc_IOData.GetDomainConfigDataConst(u32_ActualCount);
               if (pc_Domain != NULL)
               {
                  C_OSCHalcConfigDomain c_DomainCopy = *pc_Domain;
                  s32_Retval = h_LoadIODomain(c_DomainCopy, orc_XMLParser);
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
            C_SCLString c_Tmp;
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
/*! \brief  Load IO channel

   \param[in,out]  orc_IOChannel    IO channel
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_NodeName     Node name

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIOChannel(C_OSCHalcConfigChannel & orc_IOChannel,
                                              C_OSCXMLParserBase & orc_XMLParser, const C_SCLString & orc_NodeName)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("safety-relevant"))
   {
      orc_IOChannel.q_SafetyRelevant = orc_XMLParser.GetAttributeBool("safety-relevant");
   }
   else
   {
      osc_write_log_error("Loading IO data", "Could not find \"safety-relevant\" attribute.");
      s32_Retval = C_CONFIG;
   }

   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.AttributeExists("use-case-index"))
      {
         orc_IOChannel.u32_UseCaseIndex = orc_XMLParser.GetAttributeUint32("use-case-index");
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"use-case-index\" attribute.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_IOChannel.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_NodeName);
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
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_NodeName);
      }
      else
      {
         osc_write_log_error("Loading IO data", "Could not find \"comment\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadIOParameterStructs(orc_IOChannel.c_Parameters, orc_XMLParser, orc_NodeName);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO parameter structs

   \param[in,out]  orc_ParameterStructs   Parameter structs
   \param[in,out]  orc_XMLParser          XML parser
   \param[in]      orc_NodeName           Node name

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIOParameterStructs(
   std::vector<C_OSCHalcConfigParameterStruct> & orc_ParameterStructs, C_OSCXMLParserBase & orc_XMLParser,
   const C_SCLString & orc_NodeName)
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
         C_SCLString c_CurrentParameterNode = orc_XMLParser.SelectNodeChild("parameter-struct");
         if (c_CurrentParameterNode == "parameter-struct")
         {
            do
            {
               C_OSCHalcConfigParameterStruct c_Parameter;
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
                                   "Content of \"length\" attribute (" + C_SCLString::IntToStr(u32_ExpectedLength) +
                                   ") mismatches with the number of found parameters (" +
                                   C_SCLString::IntToStr(orc_ParameterStructs.size()) + ").");
               s32_Retval = C_CONFIG;
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == orc_NodeName);
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

   \param[in,out]  orc_ParameterStruct    Parameter struct
   \param[in,out]  orc_XMLParser          XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIOParameterStruct(C_OSCHalcConfigParameterStruct & orc_ParameterStruct,
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

   \param[in,out]  orc_Parameters   Parameters
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIOParameters(std::vector<C_OSCHalcConfigParameter> & orc_Parameters,
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
         C_SCLString c_CurrentParameterNode = orc_XMLParser.SelectNodeChild("parameter");
         if (c_CurrentParameterNode == "parameter")
         {
            do
            {
               C_OSCHalcConfigParameter c_Parameter;
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
                                   "Content of \"length\" attribute (" + C_SCLString::IntToStr(u32_ExpectedLength) +
                                   ") mismatches with the number of found parameters (" +
                                   C_SCLString::IntToStr(orc_Parameters.size()) + ").");
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

   \param[in,out]  orc_Parameter    Parameter
   \param[in,out]  orc_XMLParser    XML parser
   \param[in]      orc_BaseName     Base name

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigFiler::mh_LoadIOParameter(C_OSCHalcConfigParameter & orc_Parameter,
                                                C_OSCXMLParserBase & orc_XMLParser, const C_SCLString & orc_BaseName)
{
   sint32 s32_Retval = C_NO_ERR;

   C_SCLString c_TypeStr;
   C_SCLString c_BaseTypeStr;

   if (orc_XMLParser.SelectNodeChild("value") == "value")
   {
      s32_Retval = C_OSCHalcDefStructFiler::h_SetType(orc_XMLParser, orc_Parameter.c_Value, c_TypeStr,
                                                      c_BaseTypeStr, "value",
                                                      "Loading IO description");
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = C_OSCHalcDefStructFiler::h_ParseAttributeIntoContent(orc_Parameter.c_Value, orc_XMLParser,
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
