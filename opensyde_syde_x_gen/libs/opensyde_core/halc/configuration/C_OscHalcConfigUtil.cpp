//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility for HALC configuration related functionality

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_OscHalcConfigUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscHalcConfigUtil::C_OscHalcConfigUtil()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepares a stand alone configuration of the HALC

   \param[in]       orc_Config            HALC configuration as source
   \param[out]      orc_ConfigStandalone  Standalone HALC configuration as result with the configuration of orc_Config
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscHalcConfigUtil::h_GetConfigStandalone(const C_OscHalcConfig & orc_Config,
                                                C_OscHalcConfigStandalone & orc_ConfigStandalone)
{
   uint32_t u32_DomainCounter;

   orc_ConfigStandalone.c_DeviceType = orc_Config.c_DeviceName;
   orc_ConfigStandalone.u32_DefinitionContentVersion = orc_Config.u32_ContentVersion;

   for (u32_DomainCounter = 0U; u32_DomainCounter < orc_Config.GetDomainSize(); ++u32_DomainCounter)
   {
      const C_OscHalcConfigDomain * const pc_Domain = orc_Config.GetDomainConfigDataConst(u32_DomainCounter);

      if (pc_Domain != NULL)
      {
         uint32_t u32_Channels;
         std::vector<C_OscHalcConfigStandaloneChannel> c_ChannelIds;

         c_ChannelIds.resize(pc_Domain->c_Channels.size());

         // Get the information of the channels
         for (u32_Channels = 0UL; u32_Channels < c_ChannelIds.size(); ++u32_Channels)
         {
            uint32_t u32_Parameter;

            c_ChannelIds[u32_Channels].c_ParameterIds.resize(pc_Domain->c_ChannelValues.c_Parameters.size());

            // Get the Id of the parameters
            for (u32_Parameter = 0UL; u32_Parameter < pc_Domain->c_ChannelValues.c_Parameters.size(); ++u32_Parameter)
            {
               c_ChannelIds[u32_Channels].c_ParameterIds[u32_Parameter] =
                  pc_Domain->c_ChannelValues.c_Parameters[u32_Parameter].c_Id;
            }
         }

         {
            // Add the standalone domain
            const C_OscHalcConfigStandaloneDomain c_StandaloneDomain(*pc_Domain, c_ChannelIds);
            orc_ConfigStandalone.c_Domains.push_back(c_StandaloneDomain);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepares a stand alone configuration of the HALC with a specific channel

   \param[in]       orc_Config            HALC configuration as source
   \param[in]       ou32_DomainIndex      Index of domain in orc_Config
   \param[in]       ou32_ChannelIndex     Index of channel in domain with index ou32_DomainIndex
   \param[in]       oq_DomainOnly         Flag if the domain should be returned, not the concrete channel
                                          When true, ou32_ChannelIndex is not relevant
   \param[out]      orc_ConfigStandalone  Standalone HALC configuration as result with the configuration of orc_Config

   \retval   C_NO_ERR   Stand alone configuration prepared
   \retval   C_RANGE    ou32_DomainIndex or ou32_ChannelIndex is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscHalcConfigUtil::h_GetConfigStandaloneChannel(const C_OscHalcConfig & orc_Config,
                                                          const uint32_t ou32_DomainIndex,
                                                          const uint32_t ou32_ChannelIndex, const bool oq_DomainOnly,
                                                          C_OscHalcConfigStandalone & orc_ConfigStandalone)
{
   int32_t s32_Return = C_RANGE;

   if (ou32_DomainIndex < orc_Config.GetDomainSize())
   {
      const C_OscHalcConfigDomain * const pc_Domain = orc_Config.GetDomainConfigDataConst(ou32_DomainIndex);

      if (pc_Domain != NULL)
      {
         if (((oq_DomainOnly == true) ||
              ((ou32_ChannelIndex < pc_Domain->c_Channels.size()) &&
               (ou32_ChannelIndex < pc_Domain->c_ChannelConfigs.size()))) &&
             (pc_Domain->c_Channels.size() == pc_Domain->c_ChannelConfigs.size()))
         {
            std::vector<C_OscHalcConfigStandaloneChannel> c_ChannelIds;

            s32_Return = C_NO_ERR;

            // Basic information of configuration
            orc_ConfigStandalone.c_DeviceType = orc_Config.c_DeviceName;
            orc_ConfigStandalone.u32_DefinitionContentVersion = orc_Config.u32_ContentVersion;

            // Channel Id with the channel parameters
            if (oq_DomainOnly == false)
            {
               uint32_t u32_Parameter;
               c_ChannelIds.resize(1);
               c_ChannelIds[0].c_ParameterIds.resize(pc_Domain->c_ChannelValues.c_Parameters.size());

               // Get the Id of the parameters
               for (u32_Parameter = 0UL; u32_Parameter < pc_Domain->c_ChannelValues.c_Parameters.size();
                    ++u32_Parameter)
               {
                  c_ChannelIds[0].c_ParameterIds[u32_Parameter] =
                     pc_Domain->c_ChannelValues.c_Parameters[u32_Parameter].c_Id;
               }
            }

            {
               // Add the standalone domain
               C_OscHalcConfigStandaloneDomain c_StandaloneDomain(*pc_Domain, c_ChannelIds);

               if (oq_DomainOnly == false)
               {
                  // Remove the not necessary channels of the original configuration
                  if ((c_StandaloneDomain.c_ChannelConfigs.size() > 1) &&
                      (ou32_ChannelIndex < static_cast<uint32_t>(c_StandaloneDomain.c_ChannelConfigs.size() - 1UL)))
                  {
                     const uint32_t u32_NextIndex = (ou32_ChannelIndex + 1UL);
                     c_StandaloneDomain.c_ChannelConfigs.erase(
                        c_StandaloneDomain.c_ChannelConfigs.begin() + u32_NextIndex,
                        c_StandaloneDomain.c_ChannelConfigs.begin() + c_StandaloneDomain.c_ChannelConfigs.size());
                     c_StandaloneDomain.c_Channels.erase(
                        c_StandaloneDomain.c_Channels.begin() + u32_NextIndex,
                        c_StandaloneDomain.c_Channels.begin() + c_StandaloneDomain.c_Channels.size());
                  }
                  if ((c_StandaloneDomain.c_ChannelConfigs.size() > 1) &&
                      (ou32_ChannelIndex > 0UL))
                  {
                     c_StandaloneDomain.c_ChannelConfigs.erase(c_StandaloneDomain.c_ChannelConfigs.begin(),
                                                               c_StandaloneDomain.c_ChannelConfigs.begin() +
                                                               ou32_ChannelIndex);
                     c_StandaloneDomain.c_Channels.erase(c_StandaloneDomain.c_Channels.begin(),
                                                         c_StandaloneDomain.c_Channels.begin() +
                                                         ou32_ChannelIndex);
                  }
               }
               else
               {
                  // Remove the original configuration of all channels
                  c_StandaloneDomain.c_ChannelConfigs.clear();
                  c_StandaloneDomain.c_Channels.clear();
               }

               orc_ConfigStandalone.c_Domains.push_back(c_StandaloneDomain);
            }
         }
      }
   }

   return s32_Return;
}
