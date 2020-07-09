//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Load HALC definition

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <cctype>
#include <fstream>

#include "TGLFile.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_OSCHalcDefFiler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCHalcDefStructFiler.h"

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
/*! \brief  Load HALC definition

   \param[out]  orc_IOData    Storage
   \param[in]   orc_Path      Path to HALC definition

   \return
   C_NO_ERR    data read
   C_RANGE     specified system definition file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    IO file content is invalid or incomplete
               IO file could not be loaded
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::h_LoadFile(C_OSCHalcDefBase & orc_IOData, const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XMLParser.SelectRoot() == "opensyde-HALC-description")
         {
            s32_Retval = h_LoadData(orc_IOData, c_XMLParser);
         }
         else
         {
            osc_write_log_error("Loading HALC definition", "Could not find \"opensyde-HALC-description\" node.");
            s32_Retval = C_CONFIG;
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Save file to string
            C_OSCUtils::h_FileToString(orc_Path, orc_IOData.c_FileString);
         }
      }
      else
      {
         osc_write_log_error("Loading HALC definition",
                             "File \"" + orc_Path + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "File \"" + orc_Path + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save HALC definition

   Save HALC definition
   Will overwrite the file if it already exists.

   \param[in]  orc_IOData  Pointer to storage
   \param[in]  orc_Path    Path of system definition

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::h_SaveFile(const C_OSCHalcDefBase & orc_IOData, const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      //erase it:
      sintn sn_Return;
      sn_Return = std::remove(orc_Path.c_str());
      if (sn_Return != 0)
      {
         osc_write_log_error("Saving HALC definition", "Could not erase pre-existing file \"" + orc_Path + "\".");
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
            osc_write_log_error("Saving HALC definition", "Could not create folder \"" + c_Folder + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Write
      std::ofstream c_File;

      c_File.open(orc_Path.c_str(), std::ofstream::out);
      if (c_File.is_open())
      {
         c_File.write(orc_IOData.c_FileString.c_str(), orc_IOData.c_FileString.Length());
         c_File.close();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load HALC definition

   \param[out]     orc_IOData       Storage
   \param[in,out]  orc_XMLParser    XML with default state

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::h_LoadData(C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

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
         osc_write_log_error("Loading HALC definition", "\"file-version\" could not be converted to a number.");
         s32_Retval = C_CONFIG;
      }

      //is the file version one we know ?
      if (s32_Retval == C_NO_ERR)
      {
         osc_write_log_info("Loading HALC definition", "Value of \"file-version\": " +
                            stw_scl::C_SCLString::IntToStr(u16_FileVersion));
         //Check file version
         if (u16_FileVersion != hu16_FILE_VERSION_1)
         {
            osc_write_log_error("Loading HALC definition",
                                "Version defined by \"file-version\" is not supported.");
            s32_Retval = C_CONFIG;
         }
      }

      //Return
      orc_XMLParser.SelectNodeParent();
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "Could not find \"file-version\" node.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("content-version") == "content-version")
      {
         try
         {
            orc_IOData.u32_ContentVersion = static_cast<uint16>(orc_XMLParser.GetNodeContent().ToInt());
         }
         catch (...)
         {
            osc_write_log_error("Loading HALC definition",
                                "\"content-version\" could not be converted to a number.");
            s32_Retval = C_CONFIG;
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            orc_XMLParser.SelectNodeParent();
         }
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"content-version\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("device-name") == "device-name")
      {
         orc_IOData.c_DeviceName = orc_XMLParser.GetNodeContent();
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"device-name\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      stw_scl::C_SCLString c_DomainNode = orc_XMLParser.SelectNodeChild("domain");
      if (c_DomainNode == "domain")
      {
         do
         {
            C_OSCHalcDefDomain c_Domain;
            s32_Retval = mh_LoadIODataDomain(c_Domain, orc_XMLParser);
            if (s32_Retval == C_NO_ERR)
            {
               orc_IOData.AddDomain(c_Domain);
               c_DomainNode = orc_XMLParser.SelectNodeNext("domain");
            }
         }
         while ((c_DomainNode == "domain") && (s32_Retval == C_NO_ERR));
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            orc_XMLParser.SelectNodeParent();
         }
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"domain\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO data

   \param[in]      orc_IOData       Pointer to storage
   \param[in,out]  orc_XMLParser    XML with default state

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::h_SaveData(const C_OSCHalcDefBase & orc_IOData, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   //File version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("file-version") == "file-version");
   orc_XMLParser.SetNodeContent(stw_scl::C_SCLString::IntToStr(hu16_FILE_VERSION_1));
   //Return
   orc_XMLParser.SelectNodeParent();
   //Content version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("content-version") == "content-version");
   orc_XMLParser.SetNodeContent(stw_scl::C_SCLString::IntToStr(orc_IOData.u32_ContentVersion));
   //Return
   orc_XMLParser.SelectNodeParent();
   //Device
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("device-name") == "device-name");
   orc_XMLParser.SetNodeContent(orc_IOData.c_DeviceName);
   //Return
   orc_XMLParser.SelectNodeParent();
   for (uint32 u32_ItDomain = 0UL; (u32_ItDomain < orc_IOData.GetDomainSize()) && (s32_Retval == C_NO_ERR);
        ++u32_ItDomain)
   {
      const C_OSCHalcDefDomain * const pc_Domain = orc_IOData.GetDomainDefDataConst(u32_ItDomain);
      tgl_assert(pc_Domain != NULL);
      if (pc_Domain != NULL)
      {
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domain") == "domain");
         s32_Retval = mh_SaveIODomain(*pc_Domain, orc_XMLParser);
         //Return
         orc_XMLParser.SelectNodeParent();
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcDefFiler::C_OSCHalcDefFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save HALC definition

   Save HALC definition
   Will overwrite the file if it already exists.

   \param[in]  orc_IOData  Pointer to storage
   \param[in]  orc_Path    Path of system definition

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_SaveFile(const C_OSCHalcDefBase & orc_IOData, const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      //erase it:
      sintn sn_Return;
      sn_Return = std::remove(orc_Path.c_str());
      if (sn_Return != 0)
      {
         osc_write_log_error("Saving HALC definition", "Could not erase pre-existing file \"" + orc_Path + "\".");
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
            osc_write_log_error("Saving HALC definition", "Could not create folder \"" + c_Folder + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      C_OSCXMLParser c_XMLParser;
      c_XMLParser.CreateNodeChild("opensyde-HALC-description");
      tgl_assert(c_XMLParser.SelectRoot() == "opensyde-HALC-description");
      s32_Retval = h_SaveData(orc_IOData, c_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = c_XMLParser.SaveToFile(orc_Path);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Saving HALC definition", "Could not write to file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO domain data

   \param[in]      orc_IODataDomain    Pointer to storage
   \param[in,out]  orc_XMLParser       XML with default state

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_SaveIODomain(const C_OSCHalcDefDomain & orc_IODataDomain,
                                          C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_XMLParser.SetAttributeString("id", orc_IODataDomain.c_Id);
   //Name
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("name") == "name");
   orc_XMLParser.SetNodeContent(orc_IODataDomain.c_Name);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
   //Singular name
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("singular-name") == "singular-name");
   orc_XMLParser.SetNodeContent(orc_IODataDomain.c_SingularName);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
   //Channels
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channels") == "channels");
   orc_XMLParser.SetAttributeUint32("count", orc_IODataDomain.c_Channels.size());
   for (uint32 u32_ItChannel = 0UL; u32_ItChannel < orc_IODataDomain.c_Channels.size(); ++u32_ItChannel)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channel") == "channel");
      orc_XMLParser.SetAttributeString("name", orc_IODataDomain.c_Channels[u32_ItChannel]);
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "channels");
   }
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
   //Channel Usecases
   if (orc_IODataDomain.c_ChannelUseCases.size() > 0UL)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channel-use-cases") == "channel-use-cases");
      for (uint32 u32_ItCu = 0UL; (u32_ItCu < orc_IODataDomain.c_ChannelUseCases.size()) && (s32_Retval == C_NO_ERR);
           ++u32_ItCu)
      {
         const C_OSCHalcDefChannelUseCase & rc_UseCase = orc_IODataDomain.c_ChannelUseCases[u32_ItCu];
         if (u32_ItCu == 0UL)
         {
            //First saves the type
            orc_XMLParser.SetAttributeString("type",
                                             C_OSCHalcDefStructFiler::h_GetTypeString(rc_UseCase.c_Value.GetType()));
         }
         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channel-use-case") == "channel-use-case");
         s32_Retval = mh_SaveUseCase(orc_IODataDomain.c_ChannelUseCases[u32_ItCu], orc_XMLParser);
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "channel-use-cases");
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCHalcDefStructFiler::h_SaveStructs(orc_IODataDomain.c_Parameters,
                                                          orc_IODataDomain.c_ChannelUseCases, orc_XMLParser,
                                                          "domain",
                                                          "parameters", "parameter", "parameter-element");
   }
   if (s32_Retval == C_NO_ERR)
   {
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("values") == "values");
      s32_Retval = C_OSCHalcDefStructFiler::h_SaveStructs(orc_IODataDomain.c_InputValues,
                                                          orc_IODataDomain.c_ChannelUseCases, orc_XMLParser,
                                                          "values",
                                                          "input-values", "value", "value-element");
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCHalcDefStructFiler::h_SaveStructs(orc_IODataDomain.c_OutputValues,
                                                          orc_IODataDomain.c_ChannelUseCases, orc_XMLParser,
                                                          "values",
                                                          "output-values", "value", "value-element");
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCHalcDefStructFiler::h_SaveStructs(orc_IODataDomain.c_StatusValues,
                                                          orc_IODataDomain.c_ChannelUseCases, orc_XMLParser,
                                                          "values",
                                                          "status-values", "value", "value-element");
   }
   if (s32_Retval == C_NO_ERR)
   {
      //Return
      tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO domain

   \param[out]     orc_IODataDomain    Storage
   \param[in,out]  orc_XMLParser       XML with default state

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_LoadIODataDomain(C_OSCHalcDefDomain & orc_IODataDomain, C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   if (orc_XMLParser.AttributeExists("id"))
   {
      orc_IODataDomain.c_Id = orc_XMLParser.GetAttributeString("id");
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "Could not find \"id\" attribute.");
      s32_Retval = C_CONFIG;
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("name") == "name")
      {
         orc_IODataDomain.c_Name = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"name\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("singular-name") == "singular-name")
      {
         orc_IODataDomain.c_SingularName = orc_XMLParser.GetNodeContent();
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
      else
      {
         //Optional
         orc_IODataDomain.c_SingularName = orc_IODataDomain.c_Name;
      }
      s32_Retval = mh_LoadChannels(orc_IODataDomain.c_Channels, orc_XMLParser);
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = mh_LoadChannelUseCases(orc_IODataDomain.c_ChannelUseCases, orc_XMLParser,
                                          orc_IODataDomain.c_Channels.size());
   }
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCHalcDefStructFiler::h_LoadStructs(orc_IODataDomain.c_Parameters, orc_XMLParser,
                                                          orc_IODataDomain.c_ChannelUseCases,  "domain",
                                                          "parameters", "parameter", "parameter-element", true,
                                                          true, orc_IODataDomain.c_Name.Length());
   }
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("values") == "values")
      {
         s32_Retval = C_OSCHalcDefStructFiler::h_LoadStructs(orc_IODataDomain.c_InputValues, orc_XMLParser,
                                                             orc_IODataDomain.c_ChannelUseCases,
                                                             "values",
                                                             "input-values", "value", "value-element", false,
                                                             false, orc_IODataDomain.c_Name.Length());
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_OSCHalcDefStructFiler::h_LoadStructs(orc_IODataDomain.c_OutputValues, orc_XMLParser,
                                                                orc_IODataDomain.c_ChannelUseCases,
                                                                "values", "output-values", "value", "value-element",
                                                                false, false, orc_IODataDomain.c_Name.Length());
         }
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_OSCHalcDefStructFiler::h_LoadStructs(orc_IODataDomain.c_StatusValues, orc_XMLParser,
                                                                orc_IODataDomain.c_ChannelUseCases,
                                                                "values", "status-values", "value", "value-element",
                                                                false, false, orc_IODataDomain.c_Name.Length());
         }
         if (s32_Retval == C_NO_ERR)
         {
            //Return
            tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
         }
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"values\" node.");
         s32_Retval = C_CONFIG;
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO channels

   \param[out]     orc_Channels     Storage
   \param[in,out]  orc_XMLParser    XML with default state

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_LoadChannels(std::vector<stw_scl::C_SCLString> & orc_Channels,
                                          C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Channels.clear();
   if (orc_XMLParser.SelectNodeChild("channels") == "channels")
   {
      uint32 u32_ExpectedCount = 0UL;
      if (orc_XMLParser.AttributeExists("count"))
      {
         u32_ExpectedCount = orc_XMLParser.GetAttributeUint32("count");
         orc_Channels.reserve(u32_ExpectedCount);
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"count\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if ((u32_ExpectedCount > 0UL) && (s32_Retval == C_NO_ERR))
      {
         stw_scl::C_SCLString c_NodeChannel = orc_XMLParser.SelectNodeChild("channel");
         if (c_NodeChannel == "channel")
         {
            do
            {
               if (orc_XMLParser.AttributeExists("name"))
               {
                  orc_Channels.push_back(orc_XMLParser.GetAttributeString("name"));
               }
               else
               {
                  osc_write_log_error("Loading HALC definition", "Could not find \"name\" attribute.");
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
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
         else
         {
            osc_write_log_error("Loading HALC definition", "Could not find \"channel\" node.");
            s32_Retval = C_CONFIG;
         }
         //Check
         if (u32_ExpectedCount != orc_Channels.size())
         {
            const stw_scl::C_SCLString c_Error = stw_scl::C_SCLString("Expected ") +
                                                 stw_scl::C_SCLString::IntToStr(u32_ExpectedCount) +
                                                 stw_scl::C_SCLString(
               " channels, got ") + stw_scl::C_SCLString::IntToStr(
               orc_Channels.size()) + stw_scl::C_SCLString(" channels");
            osc_write_log_error("Loading HALC definition", c_Error.c_str());
            s32_Retval = C_CONFIG;
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
   }
   else
   {
      osc_write_log_error("Loading HALC definition", "Could not find \"channels\" node.");
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO channel use-cases

   \param[out]     orc_ChannelUsecases    Storage
   \param[in,out]  orc_XMLParser          XML with default state
   \param[in]      ou32_NumChannels       Number of available channels for this domain

   \return
   C_NO_ERR    data read
   C_CONFIG    IO definition content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_LoadChannelUseCases(std::vector<C_OSCHalcDefChannelUseCase> & orc_ChannelUsecases,
                                                 C_OSCXMLParserBase & orc_XMLParser, const uint32 ou32_NumChannels)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_ChannelUsecases.clear();
   if (orc_XMLParser.SelectNodeChild("channel-use-cases") == "channel-use-cases")
   {
      C_OSCNodeDataPoolContent c_Value;
      //Necessary later so get before checking if it exists, but algorithm should abort when this is the case
      const stw_scl::C_SCLString c_TypeStr = orc_XMLParser.GetAttributeString("type");
      c_Value.SetArray(false);
      if (orc_XMLParser.AttributeExists("type"))
      {
         C_OSCNodeDataPoolContent::E_Type e_Type;
         s32_Retval = C_OSCHalcDefStructFiler::h_GetTypeForSimplestTypeString(c_TypeStr, e_Type);
         c_Value.SetType(e_Type);
      }
      else
      {
         osc_write_log_error("Loading HALC definition", "Could not find \"type\" attribute.");
         s32_Retval = C_CONFIG;
      }
      if (s32_Retval == C_NO_ERR)
      {
         stw_scl::C_SCLString c_NodeChannel = orc_XMLParser.SelectNodeChild("channel-use-case");
         if (c_NodeChannel == "channel-use-case")
         {
            do
            {
               C_OSCHalcDefChannelUseCase c_UseCase;
               if (orc_XMLParser.AttributeExists("id"))
               {
                  c_UseCase.c_Id = orc_XMLParser.GetAttributeString("id");
               }
               else
               {
                  osc_write_log_error("Loading HALC definition", "Could not find \"id\" attribute.");
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.AttributeExists("display"))
                  {
                     c_UseCase.c_Display = orc_XMLParser.GetAttributeString("display");
                  }
                  else
                  {
                     osc_write_log_error("Loading HALC definition", "Could not find \"display\" attribute.");
                     s32_Retval = C_CONFIG;
                  }
               }
               if (orc_XMLParser.AttributeExists("value"))
               {
                  c_UseCase.c_Value = c_Value;
                  C_OSCHalcDefStructFiler::h_ParseSimplestTypeValue(c_TypeStr, c_UseCase.c_Value, orc_XMLParser,
                                                                    "value");
               }
               else
               {
                  osc_write_log_error("Loading HALC definition", "Could not find \"value\" attribute.");
                  s32_Retval = C_CONFIG;
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.AttributeExists("availability"))
                  {
                     s32_Retval = mh_LoadAvailability(orc_XMLParser.GetAttributeString(
                                                         "availability"), c_UseCase.c_Availability, ou32_NumChannels);
                  }
                  else
                  {
                     osc_write_log_error("Loading HALC definition", "Could not find \"availability\" attribute.");
                     s32_Retval = C_CONFIG;
                  }
               }
               if (s32_Retval == C_NO_ERR)
               {
                  if (orc_XMLParser.SelectNodeChild("comment") == "comment")
                  {
                     c_UseCase.c_Comment = orc_XMLParser.GetNodeContent();
                     //Return
                     tgl_assert(orc_XMLParser.SelectNodeParent() == "channel-use-case");
                  }
                  orc_ChannelUsecases.push_back(c_UseCase);
                  c_NodeChannel = orc_XMLParser.SelectNodeNext("channel-use-case");
               }
            }
            while ((c_NodeChannel == "channel-use-case") && (s32_Retval == C_NO_ERR));
            if (s32_Retval == C_NO_ERR)
            {
               //Return
               tgl_assert(orc_XMLParser.SelectNodeParent() == "channel-use-cases");
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse IO channel use-case availability

   \param[in]   orc_AvailabilityString    Availability string
   \param[out]  orc_Availability          Storage
   \param[in]   ou32_NumChannels          Number of available channels for this domain

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_LoadAvailability(const stw_scl::C_SCLString & orc_AvailabilityString,
                                              std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability,
                                              const uint32 ou32_NumChannels)
{
   std::vector<stw_scl::C_SCLString> c_SubElements;
   sint32 s32_Retval = C_OSCHalcDefFiler::mh_SplitAvailabilityString(orc_AvailabilityString, c_SubElements);

   orc_Availability.clear();
   if (s32_Retval == C_NO_ERR)
   {
      s32_Retval = C_OSCHalcDefFiler::mh_ParseAvailabilityStringSubElements(c_SubElements, orc_Availability,
                                                                            ou32_NumChannels);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse IO channel use-case availability string into its string sub elements

   \param[in]   orc_AvailabilityString    Availability string
   \param[out]  orc_SubElements           String subelements

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_SplitAvailabilityString(const stw_scl::C_SCLString & orc_AvailabilityString,
                                                     std::vector<stw_scl::C_SCLString> & orc_SubElements)
{
   sint32 s32_Retval = C_NO_ERR;
   bool q_IgnoreComma = false;

   stw_scl::C_SCLString c_CurSubElement;

   orc_SubElements.clear();

   //Parse subelements
   for (uint32 u32_ItChar = 0UL; (u32_ItChar < orc_AvailabilityString.Length()) && (s32_Retval == C_NO_ERR);
        ++u32_ItChar)
   {
      const charn cn_CurChar = orc_AvailabilityString[static_cast<sintn>(u32_ItChar + 1UL)];
      if (cn_CurChar == ',')
      {
         //Char:comma
         if (q_IgnoreComma)
         {
            //Special: part of enumeration, just add, don't start new section
            c_CurSubElement += cn_CurChar;
         }
         else
         {
            if (c_CurSubElement.IsEmpty() == false)
            {
               //Finish substring
               orc_SubElements.push_back(c_CurSubElement);
               //Clean
               c_CurSubElement = "";
            }
            else
            {
               osc_write_log_error("Loading HALC definition",
                                   "\"availability\" attribute contains empty section");
               s32_Retval = C_CONFIG;
            }
         }
      }
      else
      {
         //Char:any except comma
         c_CurSubElement += cn_CurChar;
         if (cn_CurChar == '{')
         {
            if (q_IgnoreComma)
            {
               osc_write_log_error("Loading HALC definition",
                                   "\"availability\" attribute does currently not support nested sections of '{' characters.");
               s32_Retval = C_CONFIG;
            }
            else
            {
               q_IgnoreComma = true;
            }
         }
         else if (cn_CurChar == '}')
         {
            if (q_IgnoreComma)
            {
               q_IgnoreComma = false;
            }
            else
            {
               osc_write_log_error("Loading HALC definition",
                                   "\"availability\" attribute contains unexpected '}' character");
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            //No special handling
         }
      }
   }
   //Handle last element
   if (c_CurSubElement.IsEmpty() == false)
   {
      //Finish substring
      orc_SubElements.push_back(c_CurSubElement);
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse IO channel use-case availability string into its string sub elements

   \param[in]      orc_SubElements     String subelements
   \param[in,out]  orc_Availability    Availability
   \param[in]      ou32_NumChannels    Number of available channels for this domain

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_ParseAvailabilityStringSubElements(
   const std::vector<stw_scl::C_SCLString> & orc_SubElements,
   std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability, const uint32 ou32_NumChannels)
{
   sint32 s32_Retval = C_NO_ERR;

   for (uint32 u32_ItSubStr = 0UL; (u32_ItSubStr < orc_SubElements.size()) && (s32_Retval == C_NO_ERR); ++u32_ItSubStr)
   {
      const stw_scl::C_SCLString & rc_CurSubStr = orc_SubElements[u32_ItSubStr];
      if (rc_CurSubStr == "all")
      {
         for (uint32 u32_ItChannel = 0UL; u32_ItChannel < ou32_NumChannels; ++u32_ItChannel)
         {
            C_OSCHalcDefChannelAvailability c_Availability;
            c_Availability.u32_ValueIndex = u32_ItChannel;
            orc_Availability.push_back(c_Availability);
         }
      }
      else
      {
         stw_scl::C_SCLString c_Number;
         std::vector<sintn> c_FoundNumbers;
         bool q_IsGroupSection = false;
         bool q_LastNumDeclaredSection = false;

         for (uint32 u32_ItChar = 0UL; (u32_ItChar < rc_CurSubStr.Length()) && (s32_Retval == C_NO_ERR); ++u32_ItChar)
         {
            const charn cn_CurChar = rc_CurSubStr[static_cast<sintn>(u32_ItChar + 1UL)];
            if (std::isdigit(cn_CurChar) != 0)
            {
               //Number
               c_Number += cn_CurChar;
            }
            else
            {
               //What to do with this number
               if (cn_CurChar == '-')
               {
                  if (q_LastNumDeclaredSection)
                  {
                     osc_write_log_error("Loading HALC definition",
                                         stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                                 "\"availability\" attribute contains unexpected character sequence with multiple '-' characters:\"")
                                                              +
                                                              rc_CurSubStr + "\"").c_str());
                     s32_Retval = C_CONFIG;
                  }
                  else
                  {
                     sintn sn_NumberTmp = 0;
                     //Handle any left over number
                     s32_Retval = C_OSCHalcDefFiler::mh_ConvertStringToNumber(c_Number, sn_NumberTmp);
                     c_Number = "";
                     c_FoundNumbers.push_back(sn_NumberTmp);
                     q_LastNumDeclaredSection = true;
                  }
               }
               else if ((cn_CurChar == '{') || (cn_CurChar == '}'))
               {
                  q_IsGroupSection = true;
               }
               else if (cn_CurChar == ',')
               {
                  //Handle vector
                  s32_Retval = C_OSCHalcDefFiler::mh_HandleNumberSection(c_Number, c_FoundNumbers,
                                                                         q_LastNumDeclaredSection, rc_CurSubStr);
               }
               else
               {
                  osc_write_log_error("Loading HALC definition",
                                      stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                              "\"availability\" attribute contains unexpected character '")
                                                           +
                                                           stw_scl::C_SCLString(cn_CurChar) + "'").c_str());
                  s32_Retval = C_CONFIG;
               }
            }
         }
         //Handle vector
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_OSCHalcDefFiler::mh_HandleNumberSection(c_Number, c_FoundNumbers,
                                                                   q_LastNumDeclaredSection, rc_CurSubStr);
         }
         //Handle availability
         if (s32_Retval == C_NO_ERR)
         {
            s32_Retval = C_OSCHalcDefFiler::mh_HandleNumberSectionEnd(c_FoundNumbers, q_IsGroupSection,
                                                                      orc_Availability,
                                                                      ou32_NumChannels);
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse IO channel use-case availability string into number

   \param[in]   orc_Number    String number
   \param[out]  orsn_Number   Converted number

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_ConvertStringToNumber(const stw_scl::C_SCLString & orc_Number, sintn & orsn_Number)
{
   sint32 s32_Retval = C_NO_ERR;

   try
   {
      orsn_Number = orc_Number.ToInt();
   }
   catch (...)
   {
      osc_write_log_error("Loading HALC definition",
                          stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                  "\"availability\" attribute contains unexpected character sequence \"")
                                               +
                                               orc_Number + "\"").c_str());
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle number section segment

   \param[in,out]  orc_Number                   String number
   \param[in,out]  orc_FoundNumbers             Last found numbers
   \param[in,out]  orq_LastNumDeclaredSection   Flag if last element of section should be connected to this number
   \param[in]      orc_Section                  Section string for user information

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_HandleNumberSection(stw_scl::C_SCLString & orc_Number,
                                                 std::vector<sintn> & orc_FoundNumbers,
                                                 bool & orq_LastNumDeclaredSection,
                                                 const stw_scl::C_SCLString & orc_Section)
{
   sint32 s32_Retval;
   sintn sn_NumberTmp = 0;

   //Handle any left over number
   s32_Retval = C_OSCHalcDefFiler::mh_ConvertStringToNumber(orc_Number, sn_NumberTmp);
   orc_Number = "";
   if (s32_Retval == C_NO_ERR)
   {
      if (orq_LastNumDeclaredSection)
      {
         orq_LastNumDeclaredSection = false;
         if (orc_FoundNumbers.size() > 0UL)
         {
            const sintn sn_NumberStart =
               orc_FoundNumbers[static_cast<std::vector<sintn>::size_type>(orc_FoundNumbers.size() - 1UL)];
            orc_FoundNumbers.pop_back();
            if (sn_NumberStart < sn_NumberTmp)
            {
               for (sintn sn_It = sn_NumberStart; sn_It <= sn_NumberTmp; ++sn_It)
               {
                  orc_FoundNumbers.push_back(sn_It);
               }
            }
            else
            {
               osc_write_log_error("Loading HALC definition",
                                   stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                           "\"availability\" attribute contains invalid usage of '-' character in \"")
                                                        +
                                                        orc_Section + "\"").c_str());
               s32_Retval = C_CONFIG;
            }
         }
         else
         {
            osc_write_log_error("Loading HALC definition",
                                stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                        "\"availability\" attribute contains confusing usage of '-' character in \"")
                                                     +
                                                     orc_Section + "\"").c_str());
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         orc_FoundNumbers.push_back(sn_NumberTmp);
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle number section segment end

   \param[in]      orc_FoundNumbers    Last found numbers
   \param[in]      oq_IsGroupSection   Flag if current section is group of elements
   \param[in,out]  orc_Availability    Availability
   \param[in]      ou32_NumChannels    Number of available channels for this domain

   \return
   C_NO_ERR    data read
   C_CONFIG    string invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_HandleNumberSectionEnd(const std::vector<sintn> & orc_FoundNumbers,
                                                    const bool oq_IsGroupSection,
                                                    std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability,
                                                    const uint32 ou32_NumChannels)
{
   sint32 s32_Retval = C_NO_ERR;

   //Check
   for (uint32 u32_ItNum = 0UL; u32_ItNum < orc_FoundNumbers.size(); ++u32_ItNum)
   {
      if (orc_FoundNumbers[u32_ItNum] >= static_cast<sintn>(ou32_NumChannels))
      {
         osc_write_log_error("Loading HALC definition",
                             stw_scl::C_SCLString(stw_scl::C_SCLString(
                                                     "\"availability\" attribute has number ") +
                                                  stw_scl::C_SCLString::IntToStr(orc_FoundNumbers[u32_ItNum]) +
                                                  " which is out of range of number of channels: " +
                                                  stw_scl::C_SCLString::IntToStr(ou32_NumChannels)).c_str());
         s32_Retval = C_CONFIG;
      }
   }
   //Assign
   if (oq_IsGroupSection)
   {
      if (orc_FoundNumbers.size() > 0UL)
      {
         C_OSCHalcDefChannelAvailability c_Availability;
         c_Availability.u32_ValueIndex = orc_FoundNumbers[0UL];
         for (uint32 u32_ItChannel = 1UL; u32_ItChannel < orc_FoundNumbers.size(); ++u32_ItChannel)
         {
            c_Availability.c_DependentValues.push_back(orc_FoundNumbers[u32_ItChannel]);
         }
         orc_Availability.push_back(c_Availability);
      }
   }
   else
   {
      for (uint32 u32_ItNum = 0UL; u32_ItNum < orc_FoundNumbers.size(); ++u32_ItNum)
      {
         C_OSCHalcDefChannelAvailability c_Availability;
         c_Availability.u32_ValueIndex = orc_FoundNumbers[u32_ItNum];
         orc_Availability.push_back(c_Availability);
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get availability string

   \param[in]  orc_Availability  Availability

   \return
   Availability string
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCHalcDefFiler::mh_GetAvailabilityString(
   const std::vector<C_OSCHalcDefChannelAvailability> & orc_Availability)
{
   stw_scl::C_SCLString c_Retval;
   for (uint32 u32_ItAva = 0UL; u32_ItAva < orc_Availability.size(); ++u32_ItAva)
   {
      const C_OSCHalcDefChannelAvailability & rc_Avail = orc_Availability[u32_ItAva];
      if (c_Retval.IsEmpty() == false)
      {
         c_Retval += ",";
      }
      if (rc_Avail.c_DependentValues.size() == 0UL)
      {
         c_Retval += stw_scl::C_SCLString::IntToStr(rc_Avail.u32_ValueIndex);
      }
      else
      {
         c_Retval += "{";
         c_Retval += stw_scl::C_SCLString::IntToStr(rc_Avail.u32_ValueIndex);
         for (uint32 u32_ItDep = 0UL; u32_ItDep < rc_Avail.c_DependentValues.size(); ++u32_ItDep)
         {
            c_Retval += ",";
            c_Retval += stw_scl::C_SCLString::IntToStr(rc_Avail.c_DependentValues[u32_ItDep]);
         }
         c_Retval += "}";
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save use case

   \param[in]      orc_UseCase      Use case
   \param[in,out]  orc_XMLParser    XML parser

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcDefFiler::mh_SaveUseCase(const C_OSCHalcDefChannelUseCase & orc_UseCase,
                                         C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval;

   orc_XMLParser.SetAttributeString("id", orc_UseCase.c_Id);
   orc_XMLParser.SetAttributeString("display", orc_UseCase.c_Display);
   s32_Retval = C_OSCHalcDefStructFiler::h_SaveSimpleValueAsAttribute("value", orc_XMLParser, orc_UseCase.c_Value);
   orc_XMLParser.SetAttributeString("availability",
                                    C_OSCHalcDefFiler::mh_GetAvailabilityString(orc_UseCase.c_Availability));
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("comment") == "comment");
   orc_XMLParser.SetNodeContent(orc_UseCase.c_Comment);
   //Return
   tgl_assert(orc_XMLParser.SelectNodeParent() == "channel-use-case");
   return s32_Retval;
}
