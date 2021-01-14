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
   const C_OSCHalcConfig * pc_Config;

   C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(orc_CurrentChannel, u32_DomainIndex, u32_ChannelIndex,
                                                           q_ChannelCase);

   pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(ou32_NodeIndex);
   if (pc_Config != NULL)
   {
      C_OSCHalcConfigStandalone c_CopyData;
      tgl_assert(C_OSCHalcConfigUtil::h_GetConfigStandaloneChannel(*pc_Config, u32_DomainIndex, u32_ChannelIndex,
                                                                   !q_ChannelCase,
                                                                   c_CopyData) == C_NO_ERR);

      C_SdClipBoardHelper::h_StoreHalcItemConfigToClipboard(c_CopyData);
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
      bool q_ChannelCase;
      bool q_LinkedChange = false;

      // check prerequisites
      if ((mh_CheckDataFromClipboard(ou32_NodeIndex, c_Data, c_ErrorDescription) == true) &&
          (mh_CheckSelectedChannels(ou32_NodeIndex, c_Data, orc_TargetIndexes, q_ChannelCase,
                                    c_ErrorDescription, q_LinkedChange) == true))
      {
         bool q_Continue = true;

         // ask user to continue if linked
         if (q_LinkedChange == true)
         {
            C_OgeWiCustomMessage c_Message(opc_Parent, C_OgeWiCustomMessage::eQUESTION,
                                           C_GtGetText::h_GetText("Do you really want to apply this configuration "
                                                                  "to the selected channel(s)? This will change "
                                                                  "linkage of channel(s) and might overwrite "
                                                                  "the configuration of channel(s) that are not "
                                                                  "selected."));
            c_Message.SetHeading(C_GtGetText::h_GetText("Apply Channel"));
            c_Message.SetOKButtonText("Apply");
            c_Message.SetNOButtonText("Cancel");
            c_Message.SetCustomMinHeight(200, 200);
            if (c_Message.Execute() != C_OgeWiCustomMessage::eYES)
            {
               q_Continue = false;
            }
         }

         if (q_Continue == true)
         {
            // paste
            if (q_ChannelCase == true)
            {
               mh_PasteToSelectedChannels(ou32_NodeIndex, c_Data, orc_TargetIndexes);
            }
            else
            {
               mh_PasteToSelectedDomain(ou32_NodeIndex, c_Data, orc_TargetIndexes);
            }

            q_Return = true;
         }
      }
      else
      {
         // report error
         C_OgeWiCustomMessage c_ErrorMessage(opc_Parent, C_OgeWiCustomMessage::eERROR,
                                             C_GtGetText::h_GetText("Could not apply channel configuration."));
         c_ErrorMessage.SetDetails(c_ErrorDescription);
         c_ErrorMessage.SetHeading(C_GtGetText::h_GetText("Apply Channel"));
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
         orc_ErrorDescription += static_cast<QString>(C_GtGetText::h_GetText("Cannot apply HALC channel configuration for device "
                                                                "type %1 to device type %2.")).
                                 arg(orc_HalcConfig.c_DeviceType.c_str(), pc_Config->c_DeviceName.c_str());
      }
      else if (orc_HalcConfig.u32_DefinitionContentVersion != pc_Config->u32_ContentVersion)
      {
         q_Return = false;
         orc_ErrorDescription += static_cast<QString>(C_GtGetText::h_GetText("Cannot apply HALC channel configuration with content "
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

         // only one channel or domain only?
         if ((rc_Domain.c_ChannelConfigs.size() > 1) || (rc_Domain.c_StandaloneChannels.size() > 1))
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
   \param[out]     orq_ChannelCase        Flag if the selected channel is a channel or domain only
   \param[in,out]  orc_ErrorDescription   Error description
   \param[out]     orq_LinkedChange       Will be set to 'true' if the selected channel will get linked or unlinked

   \retval true: on selected channels can be pasted
   \retval false: on selected cannot be pasted (see error description for details)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::mh_CheckSelectedChannels(const uint32 ou32_NodeIndex,
                                                           const C_OSCHalcConfigStandalone & orc_HalcConfig,
                                                           const QModelIndexList & orc_TargetIndexes,
                                                           bool & orq_ChannelCase, QString & orc_ErrorDescription,
                                                           bool & orq_LinkedChange)
{
   bool q_Return = false;

   if (orc_HalcConfig.c_Domains.size() == 1)
   {
      const C_OSCHalcConfigStandaloneDomain & rc_Domain = orc_HalcConfig.c_Domains[0];

      // One specific channel
      if ((orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 1) &&
          (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 1))
      {
         const C_OSCHalcConfigChannel & rc_Channel = rc_Domain.c_ChannelConfigs[0];
         QString c_InvalidChannels;
         bool q_AllChannelsValid = true;

         orq_ChannelCase = true;

         for (QModelIndexList::const_iterator c_ItIndex = orc_TargetIndexes.begin();
              (c_ItIndex != orc_TargetIndexes.end()); ++c_ItIndex)
         {
            if (mh_CheckSelectedChannel(ou32_NodeIndex, rc_Domain, rc_Channel, *c_ItIndex, c_InvalidChannels,
                                        orq_LinkedChange) == false)
            {
               q_AllChannelsValid = false;
               // do not break as we want a complete list of invalid channels
            }
         }

         if (q_AllChannelsValid == false)
         {
            orc_ErrorDescription += C_GtGetText::h_GetText("The following items are invalid targets:\n");
            orc_ErrorDescription += c_InvalidChannels;
         }
         else
         {
            q_Return = true;
         }
      }
      else if ((orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 0) &&
               (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 0))
      {
         orq_ChannelCase = false;

         if (orc_TargetIndexes.size() == 1)
         {
            QString c_InvalidDomain;
            // On domain layer only one index is possible
            const QModelIndexList::const_iterator c_ItIndex = orc_TargetIndexes.begin();

            // One specific domain only
            if (mh_CheckSelectedDomain(ou32_NodeIndex, rc_Domain, *c_ItIndex, c_InvalidDomain) == false)
            {
               orc_ErrorDescription += C_GtGetText::h_GetText("The following domain is an invalid target:\n");
               orc_ErrorDescription += c_InvalidDomain;
            }
            else
            {
               q_Return = true;
            }
         }
         else
         {
            orc_ErrorDescription += C_GtGetText::h_GetText("Only one specific domain can be selected as target"
                                                           " when applying a copied domain configuration.\n");
         }
      }
      else
      {
         tgl_assert(false);
      }
   }
   else
   {
      tgl_assert(false);
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
   \param[out]     orq_LinkedChange       Will be set to 'true' if the selected channel will get linked or unlinked

   \retval true: on selected channels can be pasted
   \retval false: on selected cannot be pasted (see error description for details)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::mh_CheckSelectedChannel(const uint32 ou32_NodeIndex,
                                                          const C_OSCHalcConfigStandaloneDomain & orc_SourceDomain,
                                                          const C_OSCHalcConfigChannel & orc_SourceChannel,
                                                          const QModelIndex & orc_TargetIndex,
                                                          QString & orc_InvalidChannels, bool & orq_LinkedChange)
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

      // Handle domain without channels case (copy channel to domain is not supported)
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
            static_cast<QString>(C_GtGetText::h_GetText("Source domain %1 (%2) differs from target domain %3 (%4).")).
            arg(orc_SourceDomain.c_DomainConfig.c_Name.c_str(), orc_SourceDomain.c_Id.c_str(),
                pc_Domain->c_Name.c_str(), pc_Domain->c_Id.c_str());
      }

      // check use case
      if (q_Return == true)
      {
         // skip simple case "same use case"
         if (pc_Channel->u32_UseCaseIndex != orc_SourceChannel.u32_UseCaseIndex)
         {
            bool q_WasLinked;
            // check if channel was linked before paste
            tgl_assert(C_PuiSdHandler::h_GetInstance()->CheckHALCDomainChannelLinked(ou32_NodeIndex, u32_DomainIndex,
                                                                                     u32_ChannelIndex, q_ChannelCase,
                                                                                     q_WasLinked) == C_NO_ERR);
            if (q_WasLinked == true)
            {
               orq_LinkedChange = true;
            }

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
                     // check if channel would get linked when pasting
                     if (rc_CurrentAvail.c_DependentValues.empty() == false)
                     {
                        orq_LinkedChange = true;
                     }
                     break;
                  }
               }
            }
            if (q_UseCaseAvailable == false)
            {
               q_Return = false;
               c_ProblemDescription =
                  static_cast<QString>(C_GtGetText::h_GetText("Use case %1 is not available.")).arg(c_UseCaseName);
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
            c_ChannelId = static_cast<QString>(" (%1)").arg(pc_Domain->c_Channels[u32_ChannelIndex].c_Name.c_str());
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
/*! \brief  Check if selected domain is compatible with the domain as source standalone HALC config.

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_SourceDomain    HALC standalone domain (copy source)
   \param[in]      orc_TargetIndex     Target index
   \param[in,out]  orc_InvalidDomain   Invalid domain

   \retval true: on selected domain can be pasted
   \retval false: on selected domain cannot be pasted (see error description for details)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelCopyPaste::mh_CheckSelectedDomain(const stw_types::uint32 ou32_NodeIndex,
                                                         const stw_opensyde_core::C_OSCHalcConfigStandaloneDomain & orc_SourceDomain, const QModelIndex & orc_TargetIndex,
                                                         QString & orc_InvalidDomain)
{
   bool q_Return = true;

   uint32 u32_DomainIndex;
   uint32 u32_ChannelIndex;
   bool q_ChannelCase;

   C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(orc_TargetIndex, u32_DomainIndex, u32_ChannelIndex,
                                                           q_ChannelCase);
   const C_OSCHalcConfigDomain * pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(ou32_NodeIndex, u32_DomainIndex);

   tgl_assert(pc_Domain != NULL);
   if ((pc_Domain != NULL))
   {
      QString c_ProblemDescription;

      // Handle channel (Copy domain configuration to channel configuration is not supported)
      if (q_ChannelCase == true)
      {
         q_Return = false;
         c_ProblemDescription = C_GtGetText::h_GetText("Cannot copy domain configuration to channel.\n");
      }

      // check domain
      if ((q_Return == true) && (pc_Domain->c_Id != orc_SourceDomain.c_Id))
      {
         q_Return = false;
         c_ProblemDescription =
            static_cast<QString>(C_GtGetText::h_GetText("Source domain %1 (%2) differs from target domain %3 (%4).")).
            arg(orc_SourceDomain.c_DomainConfig.c_Name.c_str(), orc_SourceDomain.c_Id.c_str(),
                pc_Domain->c_Name.c_str(), pc_Domain->c_Id.c_str());
      }

      // check use case
      if (q_Return == true)
      {
         // No different use cases possible in domain
         if (pc_Domain->c_DomainConfig.u32_UseCaseIndex != orc_SourceDomain.c_DomainConfig.u32_UseCaseIndex)
         {
            // check if configured use case is available
            q_Return = false;
            c_ProblemDescription =
               static_cast<QString>(C_GtGetText::h_GetText("Use case configuration of domain is not valid."));
         }
      }

      // check parameter IDs not necessary as already covered by the combination of device type, content version and use
      // case availability checks

      // add error information
      if (q_Return == false)
      {
         QString c_DomainId;

         if (u32_ChannelIndex < pc_Domain->c_Channels.size())
         {
            c_DomainId = static_cast<QString>(" (%1)").arg(pc_Domain->c_Id.c_str());
         }
         orc_InvalidDomain += pc_Domain->c_Name.c_str();
         orc_InvalidDomain += c_DomainId + ": ";
         orc_InvalidDomain += c_ProblemDescription;
         orc_InvalidDomain += "\n";
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
         uint32 u32_PreviousUseCaseIndex = 0;

         // get indexe
         C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(*c_ItIndex, u32_DomainIndex, u32_ChannelIndex,
                                                                 q_ChannelCase);
         const C_OSCHalcConfigChannel * pc_CurrentChannel =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(ou32_NodeIndex, u32_DomainIndex,
                                                                            u32_ChannelIndex, q_ChannelCase);

         if (pc_CurrentChannel != NULL)
         {
            // keep name and comment
            c_CopiedChannel.c_Name = pc_CurrentChannel->c_Name;
            c_CopiedChannel.c_Comment = pc_CurrentChannel->c_Comment;
            u32_PreviousUseCaseIndex = pc_CurrentChannel->u32_UseCaseIndex;
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

         // update linked channels
         if (pc_CurrentChannel != NULL)
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigOfLinkedChannels
                          (ou32_NodeIndex, u32_DomainIndex, u32_ChannelIndex, q_ChannelCase,
                          u32_PreviousUseCaseIndex, c_CopiedChannel.u32_UseCaseIndex) == C_NO_ERR);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Paste to selected channels

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  orc_HalcConfig       HALC standalone configuration (copy source)
   \param[in]  orc_TargetIndexes    Target indexes
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelCopyPaste::mh_PasteToSelectedDomain(const uint32 ou32_NodeIndex,
                                                           const C_OSCHalcConfigStandalone & orc_HalcConfig,
                                                           const QModelIndexList & orc_TargetIndexes)
{
   tgl_assert((orc_HalcConfig.c_Domains.size() == 1) &&
              (orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 0) &&
              (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 0) &&
              (orc_TargetIndexes.size() == 1));
   if ((orc_HalcConfig.c_Domains.size() == 1) &&
       (orc_HalcConfig.c_Domains[0].c_ChannelConfigs.size() == 0) &&
       (orc_HalcConfig.c_Domains[0].c_StandaloneChannels.size() == 0) &&
       (orc_TargetIndexes.size() == 1))
   {
      const QModelIndexList::const_iterator c_ItIndex = orc_TargetIndexes.begin();
      uint32 u32_DomainIndex;
      uint32 u32_ChannelIndex;
      bool q_ChannelCase;

      // get index
      C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(*c_ItIndex, u32_DomainIndex, u32_ChannelIndex,
                                                              q_ChannelCase);

      tgl_assert(q_ChannelCase == false);
      if (q_ChannelCase == false)
      {
         const C_OSCHalcConfigDomain * pc_CurrentDomain =
            C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(ou32_NodeIndex, u32_DomainIndex);

         tgl_assert(pc_CurrentDomain != NULL);
         if (pc_CurrentDomain != NULL)
         {
            // Copy original domain
            C_OSCHalcConfigDomain c_CurrentDomain = *pc_CurrentDomain;

            // Name and comment will not be change. Use case index is not changeable for domains
            // Get the relevant configuration data for the domain
            c_CurrentDomain.c_DomainConfig.q_SafetyRelevant =
               orc_HalcConfig.c_Domains[0].c_DomainConfig.q_SafetyRelevant;

            c_CurrentDomain.c_DomainConfig.c_Parameters = orc_HalcConfig.c_Domains[0].c_DomainConfig.c_Parameters;

            // set config
            tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCDomainConfig(ou32_NodeIndex, u32_DomainIndex,
                                                                            c_CurrentDomain) == C_NO_ERR);
         }
      }
   }
}
