//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class for HALC channel copy and paste.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "C_SdNdeHalcChannelCopyPaste.h"

#include "C_GtGetText.h"
#include "TGLUtils.h"
#include "C_OSCHalcConfigUtil.h"
#include "C_Uti.h"
#include "C_OgeWiCustomMessage.h"
#include "C_PuiSdHandler.h"
#include "C_SdClipBoardHelper.h"
#include "C_SdNdeHalcChannelTreeModel.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy data of selected index.

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  orc_CurrentChannel   Model index of channel that gets copied
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelCopyPaste::h_Copy(const uint32 ou32_NodeIndex, const QModelIndex & orc_CurrentChannel)
{
   uint32 u32_DomainIndex;
   uint32 u32_ChannelIndex;
   bool q_ChannelCase;

   C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(orc_CurrentChannel, u32_DomainIndex, u32_ChannelIndex,
                                                           q_ChannelCase);

   if (q_ChannelCase == true) // domain copy not supported
   {
      const C_OSCHalcConfig * pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(ou32_NodeIndex);

      if (pc_Config != NULL)
      {
         C_OSCHalcConfigStandalone c_CopyData;
         tgl_assert(C_OSCHalcConfigUtil::h_GetConfigStandaloneChannel(*pc_Config, u32_DomainIndex, u32_ChannelIndex,
                                                                      c_CopyData) == C_NO_ERR);

         C_SdClipBoardHelper::h_StoreHalcItemConfigToClipboard(c_CopyData);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paste data to channels of selected indices.

   \param[in,out]  opc_Parent          Parent widget for message box
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_TargetIndexes   Indexes of channels to paste at

   \retval true: configuration was pasted
   \retval false: was not pasted
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::h_Paste(QWidget * const opc_Parent, const uint32 ou32_NodeIndex,
                                          const QModelIndexList & orc_TargetIndexes)
{
   bool q_Return = false;
   C_OSCHalcConfigStandalone c_Data;
   QString c_ErrorDescription;

   // ignore if invalid clipboard data
   if (C_SdClipBoardHelper::h_LoadHalcItemConfigFromClipboard(c_Data) == C_NO_ERR)
   {
      // check prerequisites
      if ((mh_CheckDataFromClipboard(ou32_NodeIndex, c_Data, c_ErrorDescription) == true) &&
          (mh_CheckSelectedChannels(ou32_NodeIndex, c_Data, orc_TargetIndexes, c_ErrorDescription) == true))
      {
         // paste
         mh_PasteToSelectedChannels(ou32_NodeIndex, c_Data, orc_TargetIndexes);
         q_Return = true;
      }
      else
      {
         // report error
         C_OgeWiCustomMessage c_ErrorMessage(opc_Parent, C_OgeWiCustomMessage::eERROR,
                                             C_GtGetText::h_GetText("Could not paste channel configuration."));
         c_ErrorMessage.SetDetails(c_ErrorDescription);
         c_ErrorMessage.SetHeading(C_GtGetText::h_GetText("Paste Channel"));
         c_ErrorMessage.SetCustomMinHeight(230, 400);
         c_ErrorMessage.SetCustomMinWidth(600);
         c_ErrorMessage.Execute();
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check from clipboard loaded data

   Check that only one domain and only one channel.
   Check device type.

   \param[in]   ou32_NodeIndex         Node index
   \param[in]   orc_HalcConfig         HALC standalone configuration to check
   \param[out]  orc_ErrorDescription   Error description

   \retval true: clipboard data is ok for paste
   \retval false: clipboard data is not ok for paste (see error description for details)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::mh_CheckDataFromClipboard(const uint32 ou32_NodeIndex,
                                                            const C_OSCHalcConfigStandalone & orc_HalcConfig,
                                                            QString & orc_ErrorDescription)
{
   bool q_Return = true;
   const QString c_SupportedPasteExplanation =
      C_GtGetText::h_GetText("Pasting is only supported for one channel to n channels in the same domain.");

   const stw_opensyde_core::C_OSCHalcConfig * pc_Config =
      stw_opensyde_gui_logic::C_PuiSdHandler::h_GetInstance()->GetHALCConfig(ou32_NodeIndex);

   // device type match?
   tgl_assert(pc_Config != NULL);
   if (pc_Config != NULL)
   {
      if (orc_HalcConfig.c_DeviceType != pc_Config->c_DeviceName)
      {
         q_Return = false;
         orc_ErrorDescription += QString(C_GtGetText::h_GetText("Cannot paste HALC channel configuration for device "
                                                                "type %1 to device type %2.")).
                                 arg(orc_HalcConfig.c_DeviceType.c_str(), pc_Config->c_DeviceName.c_str());
      }
      else if (orc_HalcConfig.u32_DefinitionContentVersion != pc_Config->u32_ContentVersion)
      {
         q_Return = false;
         orc_ErrorDescription += QString(C_GtGetText::h_GetText("Cannot paste HALC channel configuration with content "
                                                                "version %1 to configuration with content version %2."))
                                 .arg(orc_HalcConfig.u32_DefinitionContentVersion).arg(pc_Config->u32_ContentVersion);
      }
      else
      {
         // device and definition version match
      }
   }

   // only one domain?
   if (q_Return == true)
   {
      if (orc_HalcConfig.c_Domains.size() != 1)
      {
         q_Return = false;
         orc_ErrorDescription += C_GtGetText::h_GetText("Data from clipboard contains more than one domain. ");
         orc_ErrorDescription += c_SupportedPasteExplanation;
      }
      else
      {
         const C_OSCHalcConfigStandaloneDomain & rc_Domain = orc_HalcConfig.c_Domains[0];

         // only one channel?
         if ((rc_Domain.c_ChannelConfigs.size() != 1) || (rc_Domain.c_StandaloneChannels.size() != 1))
         {
            q_Return = false;
            orc_ErrorDescription += C_GtGetText::h_GetText("Data from clipboard contains more than one channel. ");
            orc_ErrorDescription += c_SupportedPasteExplanation;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if selected channels are in the same domain as source standalone HALC config.

   Assumes that orc_HalcConfig contains exactly one domain and exactly one channel.

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      orc_HalcConfig         HALC standalone configuration (copy source)
   \param[in]      orc_TargetIndexes      Target indexes
   \param[in,out]  orc_ErrorDescription   Error description

   \retval true: on selected channels can be pasted
   \retval false: on selected cannot be pasted (see error description for details)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::mh_CheckSelectedChannels(const uint32 ou32_NodeIndex,
                                                           const C_OSCHalcConfigStandalone & orc_HalcConfig,
                                                           const QModelIndexList & orc_TargetIndexes,
                                                           QString & orc_ErrorDescription)
{
   bool q_Return = false;

   tgl_assert((orc_HalcConfig.c_Domains.size() == 1) &&
              (orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 1) &&
              (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 1));
   if ((orc_HalcConfig.c_Domains.size() == 1) &&
       (orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 1) &&
       (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 1))
   {
      const C_OSCHalcConfigStandaloneDomain & rc_Domain = orc_HalcConfig.c_Domains[0];
      const C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[0];
      QString c_InvalidChannels;
      bool q_AllChannelsValid = true;

      for (QModelIndexList::const_iterator c_ItIndex = orc_TargetIndexes.begin();
           (c_ItIndex != orc_TargetIndexes.end()); ++c_ItIndex)
      {
         if (mh_CheckSelectedChannel(ou32_NodeIndex, rc_Domain, rc_Channel, *c_ItIndex, c_InvalidChannels) == false)
         {
            q_AllChannelsValid = false;
            // do not break as we want a complete list of invalid channels
         }
      }

      if (q_AllChannelsValid == false)
      {
         orc_ErrorDescription += C_GtGetText::h_GetText("The following items are invalid as paste targets:\n");
         orc_ErrorDescription += c_InvalidChannels;
      }
      else
      {
         q_Return = true;
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if selected channels are in the same domain as source standalone HALC config.

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      orc_SourceDomain       HALC standalone domain (copy source)
   \param[in]      orc_SourceChannel      HALC standalone channel (copy source)
   \param[in]      orc_TargetIndex        Target index
   \param[in,out]  orc_InvalidChannels    Invalid channels

   \retval true: on selected channels can be pasted
   \retval false: on selected cannot be pasted (see error description for details)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::mh_CheckSelectedChannel(const uint32 ou32_NodeIndex,
                                                          const C_OSCHalcConfigStandaloneDomain & orc_SourceDomain,
                                                          const C_OSCHalcConfigChannel & orc_SourceChannel,
                                                          const QModelIndex & orc_TargetIndex,
                                                          QString & orc_InvalidChannels)
{
   bool q_Return = true;

   uint32 u32_DomainIndex;
   uint32 u32_ChannelIndex;
   bool q_ChannelCase;

   C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(orc_TargetIndex, u32_DomainIndex, u32_ChannelIndex,
                                                           q_ChannelCase);

   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(ou32_NodeIndex, u32_DomainIndex,
                                                                      u32_ChannelIndex, q_ChannelCase);
   const C_OSCHalcConfigDomain * pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(ou32_NodeIndex, u32_DomainIndex);

   tgl_assert((pc_Channel != NULL) && (pc_Domain != NULL));
   if ((pc_Channel != NULL) && (pc_Domain != NULL))
   {
      QString c_ProblemDescription;

      // Handle domain without channels case (copy to domain is not supported)
      if (q_ChannelCase == false)
      {
         q_Return = false;
         c_ProblemDescription = C_GtGetText::h_GetText("Cannot copy channel configuration to domain.\n");
      }

      // check domain
      if ((q_Return == true) && (pc_Domain->c_Id != orc_SourceDomain.c_Id))
      {
         q_Return = false;
         c_ProblemDescription =
            QString(C_GtGetText::h_GetText("Source domain %1 (%2) differs from target domain %3 (%4).")).
            arg(orc_SourceDomain.c_DomainConfig.c_Name.c_str(), orc_SourceDomain.c_Id.c_str(),
                pc_Domain->c_Name.c_str(), pc_Domain->c_Id.c_str());
      }

      // check use case
      if (q_Return == true)
      {
         // skip simple case "same use case"
         if (pc_Channel->u32_UseCaseIndex != orc_SourceChannel.u32_UseCaseIndex)
         {
            // check if configured use case is available
            bool q_UseCaseAvailable = false;
            QString c_UseCaseName;

            tgl_assert(orc_SourceChannel.u32_UseCaseIndex < pc_Domain->c_ChannelUseCases.size());
            if (orc_SourceChannel.u32_UseCaseIndex < pc_Domain->c_ChannelUseCases.size())
            {
               const C_OSCHalcDefChannelUseCase & rc_UseCase =
                  pc_Domain->c_ChannelUseCases[orc_SourceChannel.u32_UseCaseIndex];
               std::vector<C_OSCHalcDefChannelAvailability>::const_iterator c_ItAvail;

               c_UseCaseName = rc_UseCase.c_Display.c_str();

               // search available channel indices for target channel index
               for (c_ItAvail = rc_UseCase.c_Availability.begin();
                    c_ItAvail != rc_UseCase.c_Availability.end(); ++c_ItAvail)
               {
                  const C_OSCHalcDefChannelAvailability & rc_CurrentAvail = *c_ItAvail;

                  if (rc_CurrentAvail.u32_ValueIndex == u32_ChannelIndex)
                  {
                     q_UseCaseAvailable = true;
                     if (!rc_CurrentAvail.c_DependentValues.empty())
                     {
                        if (orc_SourceChannel.u32_UseCaseIndex == static_cast<uint32>(rc_UseCase.c_Value.GetValueU8()))
                        {
                           for (uint32 u32_Counter = 0U; u32_Counter < rc_CurrentAvail.c_DependentValues.size();
                                u32_Counter++)
                           {
                              // update data
                              C_PuiSdHandler::h_GetInstance()->
                              SetHALCDomainChannelConfigUseCase(ou32_NodeIndex, u32_DomainIndex,
                                                                rc_CurrentAvail.c_DependentValues.at(u32_Counter),
                                                                q_ChannelCase, orc_SourceChannel.u32_UseCaseIndex);
                           }
                        }
                     }
                     break;
                  }
               }
            }
            if (q_UseCaseAvailable == false)
            {
               q_Return = false;
               c_ProblemDescription =
                  QString(C_GtGetText::h_GetText("Use case %1 is not available.")).arg(c_UseCaseName);
            }
         }
      }

      // check parameter IDs not necessary as already covered by the combination of device type, content version and use
      // case availability checks

      // add error information
      if (q_Return == false)
      {
         QString c_ChannelId;

         if (u32_ChannelIndex < pc_Domain->c_Channels.size())
         {
            c_ChannelId = QString(" (%1)").arg(pc_Domain->c_Channels[u32_ChannelIndex].c_Name.c_str());
         }
         orc_InvalidChannels += pc_Channel->c_Name.c_str();
         orc_InvalidChannels += c_ChannelId + ": ";
         orc_InvalidChannels += c_ProblemDescription;
         orc_InvalidChannels += "\n";
      }
   }
   else
   {
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paste to selected channels

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  orc_HalcConfig       HALC standalone configuration (copy source)
   \param[in]  orc_TargetIndexes    Target indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelCopyPaste::mh_PasteToSelectedChannels(const uint32 ou32_NodeIndex,
                                                             const C_OSCHalcConfigStandalone & orc_HalcConfig,
                                                             const QModelIndexList & orc_TargetIndexes)
{
   tgl_assert((orc_HalcConfig.c_Domains.size() == 1) &&
              (orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 1) &&
              (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 1));
   if ((orc_HalcConfig.c_Domains.size() == 1) &&
       (orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 1) &&
       (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 1))
   {
      C_OSCHalcConfigChannel c_CopiedChannel = orc_HalcConfig.c_Domains[0].c_ChannelConfigs[0];
      for (QModelIndexList::const_iterator c_ItIndex = orc_TargetIndexes.begin();
           c_ItIndex != orc_TargetIndexes.end(); ++c_ItIndex)
      {
         uint32 u32_DomainIndex;
         uint32 u32_ChannelIndex;
         bool q_ChannelCase;

         // get indexe
         C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(*c_ItIndex, u32_DomainIndex, u32_ChannelIndex,
                                                                 q_ChannelCase);

         // adapt name to have unique name, if current channel does not already have the same name as the copied one
         const C_OSCHalcConfigChannel * pc_CurrentChannel =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(ou32_NodeIndex, u32_DomainIndex,
                                                                            u32_ChannelIndex, q_ChannelCase);
         if ((pc_CurrentChannel != NULL) && (pc_CurrentChannel->c_Name != c_CopiedChannel.c_Name))
         {
            c_CopiedChannel.c_Name = C_Uti::h_GetUniqueName(mh_GetExistingChannelNames(ou32_NodeIndex, u32_DomainIndex),
                                                            c_CopiedChannel.c_Name);
         }

         // set config
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfig(ou32_NodeIndex, u32_DomainIndex,
                                                                                u32_ChannelIndex, q_ChannelCase,
                                                                                c_CopiedChannel) == C_NO_ERR);
         // parameter configs
         for (uint32 u32_ParamIt = 0; u32_ParamIt < c_CopiedChannel.c_Parameters.size(); u32_ParamIt++)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelParameterConfig(
                          ou32_NodeIndex, u32_DomainIndex, u32_ChannelIndex, u32_ParamIt, q_ChannelCase,
                          c_CopiedChannel.c_Parameters[u32_ParamIt]) == C_NO_ERR);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all existing channel names

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  ou32_DomainIndex  Domain index

   \return
   Vector of pointers to all currently registered channel names of specified node and domain
*/
//----------------------------------------------------------------------------------------------------------------------
std::map<stw_scl::C_SCLString, bool> C_SdNdeHalcChannelCopyPaste::mh_GetExistingChannelNames(
   const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex)
{
   std::map<C_SCLString, bool> c_Retval;
   const C_OSCHalcConfigDomain * const pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);
   if (pc_Domain != NULL)
   {
      for (uint32 u32_ItChannel = 0; u32_ItChannel < pc_Domain->c_ChannelConfigs.size(); ++u32_ItChannel)
      {
         c_Retval[pc_Domain->c_ChannelConfigs[u32_ItChannel].c_Name] = true;
      }
   }
   return c_Retval;
}
