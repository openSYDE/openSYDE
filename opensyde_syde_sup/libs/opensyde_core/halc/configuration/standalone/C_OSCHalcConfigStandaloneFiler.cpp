//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for standalone HALC config

   Filer for standalone HALC config

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLFile.h"
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCHalcConfigFiler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCHalcConfigStandaloneFiler.h"

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
/*! \brief  Default constructor/destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCHalcConfigStandaloneFiler::C_OSCHalcConfigStandaloneFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO standalone description

   \param[out]  orc_IOData    Storage
   \param[in]   orc_Path      Path to IO description

   \return
   C_NO_ERR    data read
   C_RANGE     specified file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigStandaloneFiler::h_LoadFileStandalone(C_OSCHalcConfigStandalone & orc_IOData,
                                                            const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Retval = C_NO_ERR;

   if (TGL_FileExists(orc_Path) == true)
   {
      C_OSCXMLParser c_XMLParser;
      s32_Retval = c_XMLParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XMLParser.SelectRoot() == "opensyde-node-io-config-standalone")
         {
            s32_Retval = h_LoadDataStandalone(orc_IOData, c_XMLParser);
         }
         else
         {
            osc_write_log_error("Loading IO standalone data",
                                "Could not find \"opensyde-node-io-config-standalone\" node.");
            s32_Retval = C_CONFIG;
         }
      }
      else
      {
         osc_write_log_error("Loading IO standalone data", "File \"" + orc_Path + "\" could not be opened.");
         s32_Retval = C_NOACT;
      }
   }
   else
   {
      osc_write_log_error("Loading IO standalone data", "File \"" + orc_Path + "\" does not exist.");
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO standalone description

   Save IO standalone description
   Will overwrite the file if it already exists.

   \param[in]  orc_IOData  Pointer to storage
   \param[in]  orc_Path    Path of file

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigStandaloneFiler::h_SaveFileStandalone(const C_OSCHalcConfigStandalone & orc_IOData,
                                                            const stw_scl::C_SCLString & orc_Path)
{
   sint32 s32_Retval = C_OSCHalcConfigFiler::h_PrepareForFile(orc_Path);

   if (s32_Retval == C_NO_ERR)
   {
      C_OSCXMLParser c_XMLParser;
      c_XMLParser.CreateNodeChild("opensyde-node-io-config-standalone");
      tgl_assert(c_XMLParser.SelectRoot() == "opensyde-node-io-config-standalone");
      s32_Retval = h_SaveDataStandalone(orc_IOData, c_XMLParser);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = c_XMLParser.SaveToFile(orc_Path);
         if (s32_Retval != C_NO_ERR)
         {
            osc_write_log_error("Saving IO standalone data", "Could not write to file \"" + orc_Path + "\".");
            s32_Retval = C_RD_WR;
         }
      }
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO standalone data

   \param[out]     orc_IOData       Storage
   \param[in,out]  orc_XMLParser    XML with default state

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigStandaloneFiler::h_LoadDataStandalone(C_OSCHalcConfigStandalone & orc_IOData,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;

   // Device Type
   if (orc_XMLParser.SelectNodeChild("definition-content-version") == "definition-content-version")
   {
      orc_IOData.u32_DefinitionContentVersion = 0UL;
      try
      {
         orc_IOData.u32_DefinitionContentVersion = static_cast<uint16>(orc_XMLParser.GetNodeContent().ToInt());
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      catch (...)
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }

   // Definition content version
   if (s32_Retval == C_NO_ERR)
   {
      if (orc_XMLParser.SelectNodeChild("device-type") == "device-type")
      {
         orc_IOData.c_DeviceType = orc_XMLParser.GetNodeContent();
         //Return
         orc_XMLParser.SelectNodeParent();
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      // Domains
      if (orc_XMLParser.SelectNodeChild("domains") == "domains")
      {
         stw_scl::C_SCLString c_NodeDomain = orc_XMLParser.SelectNodeChild("domain");

         while ((c_NodeDomain == "domain") && (s32_Retval == C_NO_ERR))
         {
            C_OSCHalcConfigStandaloneDomain c_Domain;

            // The not stand alone part
            s32_Retval = C_OSCHalcConfigFiler::h_LoadIODomain(c_Domain, orc_XMLParser);

            // Domain Id
            if (orc_XMLParser.SelectNodeChild("domain-id") == "domain-id")
            {
               c_Domain.c_Id = orc_XMLParser.GetNodeContent();
               //Return
               orc_XMLParser.SelectNodeParent();
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if (s32_Retval == C_NO_ERR)
            {
               c_Domain.c_Channels.reserve(c_Domain.c_ChannelConfigs.size());

               // Channel Names
               if (orc_XMLParser.SelectNodeChild("channel-names") == "channel-names")
               {
                  stw_scl::C_SCLString c_NodeChannelId = orc_XMLParser.SelectNodeChild("channel-name");
                  bool q_AtLeastOneChannelId = false;

                  while ((c_NodeChannelId == "channel-name") && (s32_Retval == C_NO_ERR))
                  {
                     C_OSCHalcConfigStandaloneChannel c_ChannelId;

                     q_AtLeastOneChannelId = true;

                     if (orc_XMLParser.SelectNodeChild("name") == "name")
                     {
                        C_OSCHalcDefChannelDef c_ChannelDef;

                        // Channel Name
                        c_ChannelDef.c_Name = orc_XMLParser.GetNodeContent();
                        c_Domain.c_Channels.push_back(c_ChannelDef);

                        //Return
                        orc_XMLParser.SelectNodeParent();
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }

                     // Parameter Ids
                     if (s32_Retval == C_NO_ERR)
                     {
                        if (orc_XMLParser.SelectNodeChild("parameter-ids") == "parameter-ids")
                        {
                           stw_scl::C_SCLString c_NodeParameterId = orc_XMLParser.SelectNodeChild("parameter-id");
                           bool q_AtLeastOneParamterId = false;

                           while (c_NodeParameterId == "parameter-id")
                           {
                              c_ChannelId.c_ParameterIds.push_back(orc_XMLParser.GetNodeContent());

                              q_AtLeastOneParamterId = true;

                              // Iterate over the channel ids
                              c_NodeParameterId = orc_XMLParser.SelectNodeNext("parameter-id");
                           }

                           if (q_AtLeastOneParamterId == true)
                           {
                              //Return to parameter-ids
                              orc_XMLParser.SelectNodeParent();
                           }

                           //Return to channel-name
                           orc_XMLParser.SelectNodeParent();
                        }

                        c_Domain.c_StandaloneChannels.push_back(c_ChannelId);

                        // Iterate over the channel ids
                        c_NodeChannelId = orc_XMLParser.SelectNodeNext("channel-name");
                     }
                     else
                     {
                        break;
                     }
                  }

                  if (q_AtLeastOneChannelId == true)
                  {
                     //Return to channel-names
                     orc_XMLParser.SelectNodeParent();
                  }

                  //Return to domain
                  orc_XMLParser.SelectNodeParent();
               }
            }

            if (s32_Retval == C_NO_ERR)
            {
               orc_IOData.c_Domains.push_back(c_Domain);

               // Iterate over the domains
               c_NodeDomain = orc_XMLParser.SelectNodeNext("domain");
            }
            else
            {
               break;
            }
         }

         if (s32_Retval == C_NO_ERR)
         {
            //Return "domains"
            orc_XMLParser.SelectNodeParent();
         }
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save IO standalone data

   \param[in]      orc_IOData       Pointer to storage
   \param[in,out]  orc_XMLParser    XML with default state

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCHalcConfigStandaloneFiler::h_SaveDataStandalone(const C_OSCHalcConfigStandalone & orc_IOData,
                                                            C_OSCXMLParserBase & orc_XMLParser)
{
   sint32 s32_Retval = C_NO_ERR;
   uint32 u32_DomainCounter;

   // Device Type
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("definition-content-version") == "definition-content-version");
   orc_XMLParser.SetNodeContent(stw_scl::C_SCLString::IntToStr(orc_IOData.u32_DefinitionContentVersion));
   //Return
   orc_XMLParser.SelectNodeParent();

   // Definition content version
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("device-type") == "device-type");
   orc_XMLParser.SetNodeContent(orc_IOData.c_DeviceType);
   //Return
   orc_XMLParser.SelectNodeParent();

   // Domains
   tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domains") == "domains");

   for (u32_DomainCounter = 0U; u32_DomainCounter < orc_IOData.c_Domains.size(); ++u32_DomainCounter)
   {
      const C_OSCHalcConfigStandaloneDomain & rc_Domain = orc_IOData.c_Domains[u32_DomainCounter];

      // Vector size must be identical
      tgl_assert(rc_Domain.c_ChannelConfigs.size() == rc_Domain.c_StandaloneChannels.size());
      tgl_assert(rc_Domain.c_Channels.size() == rc_Domain.c_StandaloneChannels.size());

      // Save the default not stand alone part
      s32_Retval = C_OSCHalcConfigFiler::h_SaveIODomain(rc_Domain, orc_XMLParser);
      // Still in "domain"

      // Save the stand alone part of domain
      tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("domain-id") == "domain-id");
      orc_XMLParser.SetNodeContent(rc_Domain.c_Id);
      //Return
      orc_XMLParser.SelectNodeParent();

      if (s32_Retval == C_NO_ERR)
      {
         // Save the channel ids as stand alone part
         uint32 u32_ChannelCounter;

         tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channel-names") == "channel-names");

         for (u32_ChannelCounter = 0U; u32_ChannelCounter < rc_Domain.c_StandaloneChannels.size(); ++u32_ChannelCounter)
         {
            const C_OSCHalcConfigStandaloneChannel & rc_ChannelId = rc_Domain.c_StandaloneChannels[u32_ChannelCounter];
            uint32 u32_ParameterCounter;

            tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("channel-name") == "channel-name");
            orc_XMLParser.CreateNodeChild("name", rc_Domain.c_Channels[u32_ChannelCounter].c_Name);

            // Parameter Ids of channel
            tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("parameter-ids") == "parameter-ids");

            for (u32_ParameterCounter = 0U; u32_ParameterCounter < rc_ChannelId.c_ParameterIds.size();
                 ++u32_ParameterCounter)
            {
               tgl_assert(orc_XMLParser.CreateAndSelectNodeChild("parameter-id") == "parameter-id");
               orc_XMLParser.SetNodeContent(rc_ChannelId.c_ParameterIds[u32_ParameterCounter]);
               //Return
               orc_XMLParser.SelectNodeParent();
            }

            //Return "channel-name""
            tgl_assert(orc_XMLParser.SelectNodeParent() == "channel-name");

            //Return "channel-names"
            tgl_assert(orc_XMLParser.SelectNodeParent() == "channel-names");
         }

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domain");

         //Return
         tgl_assert(orc_XMLParser.SelectNodeParent() == "domains");
      }
   }

   return s32_Retval;
}
