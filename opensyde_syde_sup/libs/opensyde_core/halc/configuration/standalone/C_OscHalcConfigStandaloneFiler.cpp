//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Filer for standalone HALC config

   Filer for standalone HALC config

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglFile.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscXmlParserLog.hpp"
#include "C_OscHalcConfigFiler.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_OscHalcConfigStandaloneFiler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;

using namespace stw::errors;
using namespace stw::opensyde_core;

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
C_OscHalcConfigStandaloneFiler::C_OscHalcConfigStandaloneFiler(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load IO standalone description

   \param[out]  orc_IoData    Storage
   \param[in]   orc_Path      Path to IO description

   \return
   C_NO_ERR    data read
   C_RANGE     specified file does not exist
   C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcConfigStandaloneFiler::h_LoadFileStandalone(C_OscHalcConfigStandalone & orc_IoData,
                                                             const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Retval = C_NO_ERR;

   if (TglFileExists(orc_Path) == true)
   {
      C_OscXmlParserLog c_XmlParser;
      c_XmlParser.SetLogHeading("Loading IO standalone data");
      s32_Retval = c_XmlParser.LoadFromFile(orc_Path);
      if (s32_Retval == C_NO_ERR)
      {
         if (c_XmlParser.SelectRoot() == "opensyde-node-io-config-standalone")
         {
            s32_Retval = h_LoadDataStandalone(orc_IoData, c_XmlParser);
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

   \param[in]  orc_IoData  Pointer to storage
   \param[in]  orc_Path    Path of file

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
   C_RD_WR    could not erase pre-existing file before saving
   C_RD_WR    could not write to file (e.g. missing write permissions; missing folder)
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcConfigStandaloneFiler::h_SaveFileStandalone(const C_OscHalcConfigStandalone & orc_IoData,
                                                             const stw::scl::C_SclString & orc_Path)
{
   int32_t s32_Retval = C_OscHalcConfigFiler::h_PrepareForFile(orc_Path);

   if (s32_Retval == C_NO_ERR)
   {
      C_OscXmlParser c_XmlParser;
      c_XmlParser.CreateNodeChild("opensyde-node-io-config-standalone");
      tgl_assert(c_XmlParser.SelectRoot() == "opensyde-node-io-config-standalone");
      s32_Retval = h_SaveDataStandalone(orc_IoData, c_XmlParser);
      if (s32_Retval == C_NO_ERR)
      {
         s32_Retval = c_XmlParser.SaveToFile(orc_Path);
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

   \param[out]     orc_IoData       Storage
   \param[in,out]  orc_XmlParser    XML with default state

   \return
   C_NO_ERR    data read
   C_CONFIG    HALC configuration content is invalid or incomplete
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcConfigStandaloneFiler::h_LoadDataStandalone(C_OscHalcConfigStandalone & orc_IoData,
                                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;

   // Device Type
   if (orc_XmlParser.SelectNodeChild("definition-content-version") == "definition-content-version")
   {
      orc_IoData.u32_DefinitionContentVersion = 0UL;
      try
      {
         orc_IoData.u32_DefinitionContentVersion = static_cast<uint16_t>(orc_XmlParser.GetNodeContent().ToInt());
         //Return
         orc_XmlParser.SelectNodeParent();
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
      if (orc_XmlParser.SelectNodeChild("device-type") == "device-type")
      {
         orc_IoData.c_DeviceType = orc_XmlParser.GetNodeContent();
         //Return
         orc_XmlParser.SelectNodeParent();
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      // Domains
      if (orc_XmlParser.SelectNodeChild("domains") == "domains")
      {
         stw::scl::C_SclString c_NodeDomain = orc_XmlParser.SelectNodeChild("domain");

         while ((c_NodeDomain == "domain") && (s32_Retval == C_NO_ERR))
         {
            C_OscHalcConfigStandaloneDomain c_Domain;

            // The not stand alone part
            s32_Retval = C_OscHalcConfigFiler::h_LoadIoDomain(c_Domain, orc_XmlParser);

            // Domain Id
            if (orc_XmlParser.SelectNodeChild("domain-id") == "domain-id")
            {
               c_Domain.c_Id = orc_XmlParser.GetNodeContent();
               //Return
               orc_XmlParser.SelectNodeParent();
            }
            else
            {
               s32_Retval = C_CONFIG;
            }

            if (s32_Retval == C_NO_ERR)
            {
               c_Domain.c_Channels.reserve(c_Domain.c_ChannelConfigs.size());

               // Channel Names
               if (orc_XmlParser.SelectNodeChild("channel-names") == "channel-names")
               {
                  stw::scl::C_SclString c_NodeChannelId = orc_XmlParser.SelectNodeChild("channel-name");
                  bool q_AtLeastOneChannelId = false;

                  while ((c_NodeChannelId == "channel-name") && (s32_Retval == C_NO_ERR))
                  {
                     C_OscHalcConfigStandaloneChannel c_ChannelId;

                     q_AtLeastOneChannelId = true;

                     if (orc_XmlParser.SelectNodeChild("name") == "name")
                     {
                        C_OscHalcDefChannelDef c_ChannelDef;

                        // Channel Name
                        c_ChannelDef.c_Name = orc_XmlParser.GetNodeContent();
                        c_Domain.c_Channels.push_back(c_ChannelDef);

                        //Return
                        orc_XmlParser.SelectNodeParent();
                     }
                     else
                     {
                        s32_Retval = C_CONFIG;
                     }

                     // Parameter Ids
                     if (s32_Retval == C_NO_ERR)
                     {
                        if (orc_XmlParser.SelectNodeChild("parameter-ids") == "parameter-ids")
                        {
                           stw::scl::C_SclString c_NodeParameterId = orc_XmlParser.SelectNodeChild("parameter-id");
                           bool q_AtLeastOneParamterId = false;

                           while (c_NodeParameterId == "parameter-id")
                           {
                              c_ChannelId.c_ParameterIds.push_back(orc_XmlParser.GetNodeContent());

                              q_AtLeastOneParamterId = true;

                              // Iterate over the channel ids
                              c_NodeParameterId = orc_XmlParser.SelectNodeNext("parameter-id");
                           }

                           if (q_AtLeastOneParamterId == true)
                           {
                              //Return to parameter-ids
                              orc_XmlParser.SelectNodeParent();
                           }

                           //Return to channel-name
                           orc_XmlParser.SelectNodeParent();
                        }

                        c_Domain.c_StandaloneChannels.push_back(c_ChannelId);

                        // Iterate over the channel ids
                        c_NodeChannelId = orc_XmlParser.SelectNodeNext("channel-name");
                     }
                     else
                     {
                        break;
                     }
                  }

                  if (q_AtLeastOneChannelId == true)
                  {
                     //Return to channel-names
                     orc_XmlParser.SelectNodeParent();
                  }

                  //Return to domain
                  orc_XmlParser.SelectNodeParent();
               }
            }

            if (s32_Retval == C_NO_ERR)
            {
               orc_IoData.c_Domains.push_back(c_Domain);

               // Iterate over the domains
               c_NodeDomain = orc_XmlParser.SelectNodeNext("domain");
            }
            else
            {
               break;
            }
         }

         if (s32_Retval == C_NO_ERR)
         {
            //Return "domains"
            orc_XmlParser.SelectNodeParent();
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

   \param[in]      orc_IoData       Pointer to storage
   \param[in,out]  orc_XmlParser    XML with default state

   \return
   C_NO_ERR   data saved
   C_CONFIG   data invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcConfigStandaloneFiler::h_SaveDataStandalone(const C_OscHalcConfigStandalone & orc_IoData,
                                                             C_OscXmlParserBase & orc_XmlParser)
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_DomainCounter;

   // Device Type
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("definition-content-version") == "definition-content-version");
   orc_XmlParser.SetNodeContent(stw::scl::C_SclString::IntToStr(orc_IoData.u32_DefinitionContentVersion));
   //Return
   orc_XmlParser.SelectNodeParent();

   // Definition content version
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("device-type") == "device-type");
   orc_XmlParser.SetNodeContent(orc_IoData.c_DeviceType);
   //Return
   orc_XmlParser.SelectNodeParent();

   // Domains
   tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("domains") == "domains");

   for (u32_DomainCounter = 0U; u32_DomainCounter < orc_IoData.c_Domains.size(); ++u32_DomainCounter)
   {
      const C_OscHalcConfigStandaloneDomain & rc_Domain = orc_IoData.c_Domains[u32_DomainCounter];

      // Vector size must be identical
      tgl_assert(rc_Domain.c_ChannelConfigs.size() == rc_Domain.c_StandaloneChannels.size());
      tgl_assert(rc_Domain.c_Channels.size() == rc_Domain.c_StandaloneChannels.size());

      // Save the default not stand alone part
      s32_Retval = C_OscHalcConfigFiler::h_SaveIoDomain(rc_Domain, orc_XmlParser);
      // Still in "domain"

      // Save the stand alone part of domain
      tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("domain-id") == "domain-id");
      orc_XmlParser.SetNodeContent(rc_Domain.c_Id);
      //Return
      orc_XmlParser.SelectNodeParent();

      if (s32_Retval == C_NO_ERR)
      {
         // Save the channel ids as stand alone part
         uint32_t u32_ChannelCounter;

         tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("channel-names") == "channel-names");

         for (u32_ChannelCounter = 0U; u32_ChannelCounter < rc_Domain.c_StandaloneChannels.size(); ++u32_ChannelCounter)
         {
            const C_OscHalcConfigStandaloneChannel & rc_ChannelId = rc_Domain.c_StandaloneChannels[u32_ChannelCounter];
            uint32_t u32_ParameterCounter;

            tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("channel-name") == "channel-name");
            orc_XmlParser.CreateNodeChild("name", rc_Domain.c_Channels[u32_ChannelCounter].c_Name);

            // Parameter Ids of channel
            tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("parameter-ids") == "parameter-ids");

            for (u32_ParameterCounter = 0U; u32_ParameterCounter < rc_ChannelId.c_ParameterIds.size();
                 ++u32_ParameterCounter)
            {
               tgl_assert(orc_XmlParser.CreateAndSelectNodeChild("parameter-id") == "parameter-id");
               orc_XmlParser.SetNodeContent(rc_ChannelId.c_ParameterIds[u32_ParameterCounter]);
               //Return
               orc_XmlParser.SelectNodeParent();
            }

            //Return "channel-name""
            tgl_assert(orc_XmlParser.SelectNodeParent() == "channel-name");

            //Return "channel-names"
            tgl_assert(orc_XmlParser.SelectNodeParent() == "channel-names");
         }

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "domain");

         //Return
         tgl_assert(orc_XmlParser.SelectNodeParent() == "domains");
      }
   }

   return s32_Retval;
}
