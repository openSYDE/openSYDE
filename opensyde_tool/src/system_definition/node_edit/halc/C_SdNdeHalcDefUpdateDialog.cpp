//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for updating an existing HALC definition

   Dialog for updating an existing HALC definition

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_ImpUtil.hpp"
#include "TglUtils.hpp"
#include "C_SdNdeHalcDefUpdateDialog.hpp"
#include "ui_C_SdNdeHalcDefUpdateDialog.h"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent             Reference to parent
   \param[in]      ou32_NodeIndex         Node index of node for updating the HALC definition and its configuration
   \param[in]      orc_LoadedHalcConfig   Loaded HALC definition for updating
   \param[in]      orc_FilePath           File path of loaded HALC definition
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcDefUpdateDialog::C_SdNdeHalcDefUpdateDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                       const uint32_t ou32_NodeIndex,
                                                       const C_OscHalcConfig & orc_LoadedHalcConfig,
                                                       const QString & orc_FilePath) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcDefUpdateDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mrc_LoadedHalcConfig(orc_LoadedHalcConfig),
   ms32_Result(C_NOACT),
   mc_NewContent(QStringList()),
   mc_ResetContent(QStringList()),
   mc_RemovedContent(QStringList())
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // text edit
   this->mpc_Ui->pc_TextEditFlash->setReadOnly(true);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeHalcDefUpdateDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this,
           &C_SdNdeHalcDefUpdateDialog::m_OnCancel);

   this->m_UpdateHalcConfiguration();
   this->m_CreateReport(orc_FilePath);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcDefUpdateDialog::~C_SdNdeHalcDefUpdateDialog(void) noexcept
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("HALC Configuration"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Update Definition"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Update Definition"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Gets the result of updating the current definition/configuration

   \param[out] orc_UpdatedHalcConfig   Definition/Configuration based on current node HALC configuration updated by
                                       loaded definition
   \param[out] orc_ErrorDetails        String with details of error. Empty string in case of no error

   \retval   C_NO_ERR   All ok, configuration prepared
   \retval   C_NOACT    No error, nothing done, user did not accepted or the content version did not changed
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeHalcDefUpdateDialog::GetResult(C_OscHalcConfig & orc_UpdatedHalcConfig, QString & orc_ErrorDetails) const
{
   // Error details
   switch (this->ms32_Result)
   {
   case C_NO_ERR:
   case C_NOACT: // No real error, the user aborted or the version did not changed
      orc_ErrorDetails = "";
      break;
   default:
      orc_ErrorDetails =
         static_cast<QString>(C_GtGetText::h_GetText("Unknown error: %1")).arg(C_Uti::h_StwError(this->ms32_Result));
      break;
   }

   if (this->ms32_Result == C_NO_ERR)
   {
      orc_UpdatedHalcConfig = this->mc_UpdatedHalcConfig;
   }

   return this->ms32_Result;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_OkClicked(void)
{
   if (this->ms32_Result == C_NOACT)
   {
      // User accepted the update
      this->ms32_Result = C_NO_ERR;
   }

   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_OnCancel(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create report header and add it to text edit.

   \param[in]  orc_FilePath   File path of loaded HALC definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_CreateReport(const QString & orc_FilePath) const
{
   QString c_ReadContent;
   QString c_Text;

   // Content summary
   c_ReadContent += static_cast<QString>(C_GtGetText::h_GetText("Content version: %1; ")).
                    arg(this->mrc_LoadedHalcConfig.u32_ContentVersion);
   c_ReadContent += static_cast<QString>(C_GtGetText::h_GetText("Domains: %1")).
                    arg(this->mrc_LoadedHalcConfig.GetDomainSize());

   // Source file information
   c_Text += C_ImpUtil::h_FormatSourceFileInfoForReport(orc_FilePath, c_ReadContent);

   // Preview heading
   c_Text += "<h3>";
   c_Text += C_GtGetText::h_GetText("Update Preview");
   c_Text += "</h3>";

   // Content
   c_Text += this->m_AddSection(C_GtGetText::h_GetText("New"), this->mc_NewContent);
   c_Text += this->m_AddSection(C_GtGetText::h_GetText("Reset to Default"), this->mc_ResetContent);
   c_Text += this->m_AddSection(C_GtGetText::h_GetText("Removed"), this->mc_RemovedContent);

   // Finally add to text edit field
   this->mpc_Ui->pc_TextEditFlash->setHtml(c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add section

   \param[in]  orc_SectionName   Name of the section (new, updated or removed)
   \param[in]  orc_Content       Content of the section

   \return
   section content
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeHalcDefUpdateDialog::m_AddSection(const QString & orc_SectionName, const QStringList & orc_Content) const
{
   QString c_Text = "";

   c_Text += "<p><b>";
   c_Text += static_cast<QString>(C_GtGetText::h_GetText("%1 (%2)")).arg(orc_SectionName).arg(orc_Content.length());
   c_Text += "</b></p>";
   if (orc_Content.length() == 0)
   {
      c_Text += static_cast<QString>(C_GtGetText::h_GetText("No %1 features.")).arg(orc_SectionName.toLower());
   }
   else
   {
      const QString c_TableHeaderStart =
         "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 0px 0px;white-space:pre;font-weight:bold;\">";
      const QString c_TableDataStart =
         "<td align=\"left\" valign=\"top\" style=\"padding: 0px 18px 0px 0px;white-space:pre;\">";

      c_Text += "<table>";
      c_Text += "<tr>";
      c_Text += c_TableHeaderStart;
      c_Text += "#</td>";
      c_Text += c_TableHeaderStart;
      c_Text += C_GtGetText::h_GetText("Description");
      c_Text += "</td>";
      c_Text += "</tr>";

      for (int32_t s32_ItEntry = 0U; s32_ItEntry < orc_Content.length(); s32_ItEntry++)
      {
         c_Text += "<tr>";
         c_Text += c_TableDataStart;
         c_Text += QString::number(s32_ItEntry + 1);
         c_Text += "</td>";
         c_Text += c_TableDataStart;
         c_Text += orc_Content[s32_ItEntry];
         c_Text += "</td>";
         c_Text += "</tr>";
      }

      c_Text += "</table>";
   }

   return c_Text;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Loads the current HALC configuration of the node and "merge" it with the loaded configuration

   The result is in mc_UpdatedHalcConfig
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_UpdateHalcConfiguration(void)
{
   const C_OscHalcConfig * const pc_CurrentConfig =
      C_PuiSdHandler::h_GetInstance()->GetHalcConfig(this->mu32_NodeIndex);

   tgl_assert(pc_CurrentConfig != NULL);
   if (pc_CurrentConfig != NULL)
   {
      uint32_t u32_UpdatedDomainCounter;
      uint32_t u32_CurrentDomainCounter;

      // Copy the entire loaded configuration for further editing
      this->mc_UpdatedHalcConfig = this->mrc_LoadedHalcConfig;

      // Building the result by adding the necessary domains to mc_UpdatedHalcConfig
      // By adding each domain to the updated configuration the order is like in the loaded configuration
      // Check domains (update and add)
      for (u32_UpdatedDomainCounter = 0UL; u32_UpdatedDomainCounter < this->mc_UpdatedHalcConfig.GetDomainSize();
           ++u32_UpdatedDomainCounter)
      {
         const C_OscHalcConfigDomain * const pc_UpdatedDomain = this->mc_UpdatedHalcConfig.GetDomainConfigDataConst(
            u32_UpdatedDomainCounter);

         tgl_assert(pc_UpdatedDomain != NULL);
         if (pc_UpdatedDomain != NULL)
         {
            bool q_DomainFound = false;

            // Comparing to the current definition/configuration
            for (u32_CurrentDomainCounter = 0UL; u32_CurrentDomainCounter < pc_CurrentConfig->GetDomainSize();
                 ++u32_CurrentDomainCounter)
            {
               const C_OscHalcConfigDomain * const pc_CurrentDomain = pc_CurrentConfig->GetDomainConfigDataConst(
                  u32_CurrentDomainCounter);

               tgl_assert(pc_CurrentDomain != NULL);
               if ((pc_CurrentDomain != NULL) &&
                   (pc_UpdatedDomain->c_Id == pc_CurrentDomain->c_Id))
               {
                  C_OscHalcConfigDomain c_AdatedUpdatedDomain = *pc_UpdatedDomain;

                  // Same Id -> Compare and if necessary update
                  C_SdNdeHalcDefUpdateDialog::m_UpdateDomainConfiguration(*pc_CurrentDomain, c_AdatedUpdatedDomain);
                  this->mc_UpdatedHalcConfig.SetDomainConfig(u32_UpdatedDomainCounter, c_AdatedUpdatedDomain);

                  q_DomainFound = true;
                  break;
               }
            }

            if (q_DomainFound == false)
            {
               // Report new domain
               this->mc_NewContent.append(static_cast<QString>(C_GtGetText::h_GetText("Domain ")) +
                                          pc_UpdatedDomain->c_Name.c_str());
            }
         }
      }

      // Check domains with second loop which domains were removed for reporting by
      // searching all current domain ids in the updated configuration
      for (u32_CurrentDomainCounter = 0UL; u32_CurrentDomainCounter < pc_CurrentConfig->GetDomainSize();
           ++u32_CurrentDomainCounter)
      {
         const C_OscHalcConfigDomain * const pc_CurrentDomain = pc_CurrentConfig->GetDomainConfigDataConst(
            u32_CurrentDomainCounter);

         tgl_assert(pc_CurrentDomain != NULL);
         if (pc_CurrentDomain != NULL)
         {
            bool q_CurrentDomainFound = false;

            for (u32_UpdatedDomainCounter = 0UL;
                 u32_UpdatedDomainCounter < this->mc_UpdatedHalcConfig.GetDomainSize();
                 ++u32_UpdatedDomainCounter)
            {
               const C_OscHalcConfigDomain * const pc_UpdatedDomain =
                  this->mc_UpdatedHalcConfig.GetDomainConfigDataConst(u32_UpdatedDomainCounter);

               tgl_assert(pc_UpdatedDomain != NULL);
               if ((pc_UpdatedDomain != NULL) &&
                   (pc_UpdatedDomain->c_Id == pc_CurrentDomain->c_Id))
               {
                  q_CurrentDomainFound = true;
                  break;
               }
            }
            if (q_CurrentDomainFound == false)
            {
               // Report domain removed
               this->mc_RemovedContent.append(static_cast<QString>(C_GtGetText::h_GetText("Domain ")) +
                                              pc_CurrentDomain->c_Name.c_str());
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the domain configuration by merging the loaded and current configuration and adding it

   \param[in]       orc_CurrentConfig    Current domain configuration with user specific adaptions
   \param[in,out]   orc_UpdatedConfig    Domain configuration to update. Configuration must be set by default values
                                         with the channel configuration of the loaded definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_UpdateDomainConfiguration(const C_OscHalcConfigDomain & orc_CurrentConfig,
                                                             C_OscHalcConfigDomain & orc_UpdatedConfig)
{
   uint32_t u32_UpdatedChannelCounter;
   uint32_t u32_CurrentChannelCounter;

   // Getting the set domain configuration data of the current configuration
   C_SdNdeHalcDefUpdateDialog::m_UpdateChannelConfiguration(orc_CurrentConfig, orc_UpdatedConfig,
                                                            orc_CurrentConfig.c_Name,
                                                            true);

   tgl_assert(orc_CurrentConfig.c_ChannelConfigs.size() == orc_CurrentConfig.c_Channels.size());
   tgl_assert(orc_UpdatedConfig.c_ChannelConfigs.size() == orc_UpdatedConfig.c_Channels.size());

   // Checking all channels
   for (u32_UpdatedChannelCounter = 0UL; u32_UpdatedChannelCounter < orc_UpdatedConfig.c_ChannelConfigs.size();
        ++u32_UpdatedChannelCounter)
   {
      bool q_ChannelFound = false;

      for (u32_CurrentChannelCounter = 0UL; u32_CurrentChannelCounter < orc_CurrentConfig.c_ChannelConfigs.size();
           ++u32_CurrentChannelCounter)
      {
         // Search the matching channels by comparing the names of the definition
         if (orc_CurrentConfig.c_Channels[u32_CurrentChannelCounter].c_Name ==
             orc_UpdatedConfig.c_Channels[u32_UpdatedChannelCounter].c_Name)
         {
            // Same channel -> Compare and if necessary update
            C_SdNdeHalcDefUpdateDialog::m_UpdateChannelConfiguration(orc_CurrentConfig, orc_UpdatedConfig,
                                                                     orc_CurrentConfig.
                                                                     c_Channels[u32_CurrentChannelCounter].c_Name,
                                                                     false,
                                                                     u32_CurrentChannelCounter,
                                                                     u32_UpdatedChannelCounter);

            q_ChannelFound = true;

            break;
         }
      }

      if (q_ChannelFound == false)
      {
         // Report new channel
         this->mc_NewContent.append(static_cast<QString>(C_GtGetText::h_GetText("Channel ")) +
                                    orc_UpdatedConfig.c_ChannelConfigs[u32_UpdatedChannelCounter].c_Name.c_str());
      }
   }

   // Check channels with second loop which channels were removed for reporting by
   // searching all current channel names in the updated configuration
   for (u32_CurrentChannelCounter = 0UL; u32_CurrentChannelCounter < orc_CurrentConfig.c_Channels.size();
        ++u32_CurrentChannelCounter)
   {
      bool q_CurrentChannelFound = false;

      for (u32_UpdatedChannelCounter = 0UL;
           u32_UpdatedChannelCounter < orc_UpdatedConfig.c_Channels.size();
           ++u32_UpdatedChannelCounter)
      {
         if (orc_CurrentConfig.c_Channels[u32_CurrentChannelCounter].c_Name ==
             orc_UpdatedConfig.c_Channels[u32_UpdatedChannelCounter].c_Name)
         {
            q_CurrentChannelFound = true;
            break;
         }
      }

      if (q_CurrentChannelFound == false)
      {
         // Report channel removed
         this->mc_RemovedContent.append(static_cast<QString>(C_GtGetText::h_GetText("Channel ")) +
                                        orc_CurrentConfig.c_ChannelConfigs[u32_CurrentChannelCounter].c_Name.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the domain channel configuration by merging the loaded and current channel configuration

   \param[in]     orc_CurrentConfig           Current domain configuration with user specific adaptions
   \param[in,out] orc_UpdatedConfig           Domain configuration to update. Configuration must be set by default values
                                              with the channel configuration of the loaded definition
   \param[in]     orc_PreviousDefChannelName  Channel name of current configuration for its definition and not the
                                              user configuration
   \param[in]     oq_IsDomainConfiguration    Flag if the current channel configuration is a domain configuration or a
                                              channel configuration
   \param[in]     ou32_CurrentChannelIndex    Current channel configuration index of configuration with
                                              user specific adaptions (if q_IsDomainConfiguration == false)
   \param[in]     ou32_UpdatedChannelIndex    Channel configuration index of configuration to update
                                              (if q_IsDomainConfiguration == false)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_UpdateChannelConfiguration(const C_OscHalcConfigDomain & orc_CurrentConfig,
                                                              C_OscHalcConfigDomain & orc_UpdatedConfig,
                                                              const C_SclString & orc_PreviousDefChannelName,
                                                              const bool oq_IsDomainConfiguration,
                                                              const uint32_t ou32_CurrentChannelIndex,
                                                              const uint32_t ou32_UpdatedChannelIndex)
{
   const C_OscHalcConfigChannel & rc_CurrentChannelConfig = (oq_IsDomainConfiguration == true) ?
                                                            orc_CurrentConfig.c_DomainConfig :
                                                            orc_CurrentConfig.c_ChannelConfigs[ou32_CurrentChannelIndex];
   C_OscHalcConfigChannel & rc_UpdatedChannelConfig = (oq_IsDomainConfiguration == true) ?
                                                      orc_UpdatedConfig.c_DomainConfig :
                                                      orc_UpdatedConfig.c_ChannelConfigs[ou32_UpdatedChannelIndex];
   bool q_ResetToDefault = false;

   // Parameters to compare
   std::vector<C_OscHalcDefStruct> * pc_UpdatedParametersToCompare = NULL;
   const std::vector<C_OscHalcDefStruct> * pc_CurrentParametersToCompare = NULL;

   // Update all users adaption of configuration in loaded configuration except parameters
   // Check if name was changed by user. If it equals the definition name of the current definition
   // it was not changed and will be updated to the new name of the loaded definition
   if ((rc_CurrentChannelConfig.c_Name != orc_PreviousDefChannelName) ||
       (rc_CurrentChannelConfig.c_Name == ""))
   {
      rc_UpdatedChannelConfig.c_Name = rc_CurrentChannelConfig.c_Name;
   }
   rc_UpdatedChannelConfig.c_Comment = rc_CurrentChannelConfig.c_Comment;
   rc_UpdatedChannelConfig.q_SafetyRelevant = rc_CurrentChannelConfig.q_SafetyRelevant;

   if (oq_IsDomainConfiguration == false)
   {
      uint32_t u32_UseCaseCounter;
      bool q_UseCaseFound = false;
      bool q_UseCaseAvailable = false;
      const C_OscHalcDefChannelUseCase & rc_CurrentUseCase =
         orc_CurrentConfig.c_ChannelUseCases[rc_CurrentChannelConfig.u32_UseCaseIndex];

      // Check previous use case. Only relevant for channels, not domains
      for (u32_UseCaseCounter = 0U; u32_UseCaseCounter < orc_UpdatedConfig.c_ChannelUseCases.size();
           ++u32_UseCaseCounter)
      {
         const C_OscHalcDefChannelUseCase & rc_UseCase = orc_UpdatedConfig.c_ChannelUseCases[u32_UseCaseCounter];

         if (rc_CurrentUseCase.c_Id == rc_UseCase.c_Id)
         {
            uint32_t u32_AvailabilityCounter;

            // Use case does still exist
            q_UseCaseFound = true;

            // Check if use case is still available for this channel (with new updated index)
            for (u32_AvailabilityCounter = 0; u32_AvailabilityCounter < rc_UseCase.c_Availability.size();
                 ++u32_AvailabilityCounter)
            {
               if (rc_UseCase.c_Availability[u32_AvailabilityCounter].u32_ValueIndex == ou32_UpdatedChannelIndex)
               {
                  const uint32_t u32_UseCaseIndex = static_cast<uint32_t>(rc_UseCase.c_Value.GetValueU8());
                  // Use case is available too. Can be updated. Check of parameters
                  rc_UpdatedChannelConfig.u32_UseCaseIndex = u32_UseCaseIndex;
                  q_UseCaseAvailable = true;
                  break;
               }
            }

            break;
         }
      }

      if ((q_UseCaseFound == true) &&
          (q_UseCaseAvailable == true))
      {
         // Channel configuration of current configuration must checked for compatible parameters
         pc_UpdatedParametersToCompare = &orc_UpdatedConfig.c_ChannelValues.c_Parameters;
         pc_CurrentParametersToCompare = &orc_CurrentConfig.c_ChannelValues.c_Parameters;
      }
      else
      {
         // Use case does not exist or is not available anymore,
         // no further adaption of channel to current configuration possible
         // Using the loaded default values of definition.
         q_ResetToDefault = true;
         // Report channel reset to default due to missing use case
         this->mc_ResetContent.append(static_cast<QString>(C_GtGetText::h_GetText(
                                                              "Configured use case of channel %1 is not "
                                                              "available anymore and was reset to default."))
                                      .arg(rc_UpdatedChannelConfig.c_Name.c_str()));
      }
   }
   else
   {
      // Channel configuration of current configuration must checked for compatible parameters
      pc_UpdatedParametersToCompare = &orc_UpdatedConfig.c_DomainValues.c_Parameters;
      pc_CurrentParametersToCompare = &orc_UpdatedConfig.c_DomainValues.c_Parameters;
   }

   if ((q_ResetToDefault == false) &&
       (pc_UpdatedParametersToCompare != NULL) &&
       (pc_CurrentParametersToCompare != NULL))
   {
      uint32_t u32_UpdatedDefParameterCounter;
      uint32_t u32_CurrentDefParameterCounter;

      tgl_assert(rc_CurrentChannelConfig.c_Parameters.size() == pc_CurrentParametersToCompare->size());
      tgl_assert(rc_UpdatedChannelConfig.c_Parameters.size() == pc_UpdatedParametersToCompare->size());

      // Check for changed parameters
      // Checking all parameters for existence
      for (u32_UpdatedDefParameterCounter = 0UL;
           u32_UpdatedDefParameterCounter < pc_UpdatedParametersToCompare->size();
           ++u32_UpdatedDefParameterCounter)
      {
         bool q_ParameterFound = false;
         const C_OscHalcDefStruct & rc_UpdatedParameterDef =
            (*pc_UpdatedParametersToCompare)[u32_UpdatedDefParameterCounter];

         for (u32_CurrentDefParameterCounter = 0UL;
              u32_CurrentDefParameterCounter < pc_CurrentParametersToCompare->size();
              ++u32_CurrentDefParameterCounter)
         {
            const C_OscHalcDefStruct & rc_CurrentParameterDef =
               (*pc_CurrentParametersToCompare)[u32_CurrentDefParameterCounter];

            // Search the matching parameters by comparing the names of the definition
            if (rc_CurrentParameterDef.c_Id == rc_UpdatedParameterDef.c_Id)
            {
               // Same parameter
               q_ParameterFound = true;

               if ((rc_CurrentParameterDef.c_StructElements.size() == 0) &&
                   (rc_UpdatedParameterDef.c_StructElements.size() == 0))
               {
                  // Both have no children and have the value content. Update it when possible
                  this->m_UpdateParameterElementConfiguration(
                     rc_CurrentParameterDef, rc_UpdatedParameterDef,
                     oq_IsDomainConfiguration,
                     rc_UpdatedChannelConfig.u32_UseCaseIndex,
                     rc_CurrentChannelConfig.c_Parameters[u32_CurrentDefParameterCounter],
                     rc_UpdatedChannelConfig.c_Parameters[u32_UpdatedDefParameterCounter],
                     rc_UpdatedChannelConfig.c_Name);
               }
               else if ((rc_CurrentParameterDef.c_StructElements.size() > 0) &&
                        (rc_UpdatedParameterDef.c_StructElements.size() > 0))
               {
                  // Both have children
                  // Compare and update when possible matching children
                  this->m_UpdateSubParameterConfiguration(rc_CurrentParameterDef, rc_UpdatedParameterDef,
                                                          oq_IsDomainConfiguration,
                                                          rc_UpdatedChannelConfig.u32_UseCaseIndex,
                                                          rc_CurrentChannelConfig.c_Parameters[
                                                             u32_CurrentDefParameterCounter],
                                                          rc_UpdatedChannelConfig.c_Parameters[
                                                             u32_UpdatedDefParameterCounter],
                                                          rc_UpdatedChannelConfig.c_Name);
               }
               else
               {
                  // Parameter struct changed, no update possible
                  // Reporting parameter structure changed
                  this->mc_ResetContent.append(static_cast<QString>(C_GtGetText::h_GetText(
                                                                       "Configured parameter %1 of channel %2 is not "
                                                                       "updated due to changed configuration structure."))
                                               .arg(rc_UpdatedParameterDef.c_Display.c_str())
                                               .arg(rc_UpdatedChannelConfig.c_Name.c_str()));
               }

               break;
            }
         }

         if (q_ParameterFound == false)
         {
            // Reporting new parameter
            this->mc_NewContent.append(static_cast<QString>(C_GtGetText::h_GetText("Parameter %1 of channel %2"))
                                       .arg(rc_UpdatedParameterDef.c_Display.c_str())
                                       .arg(rc_UpdatedChannelConfig.c_Name.c_str()));
         }
      }

      // Check parameters with second loop which parameters were removed for reporting by
      // searching all current parameter names in the updated configuration
      for (u32_CurrentDefParameterCounter = 0UL;
           u32_CurrentDefParameterCounter < pc_CurrentParametersToCompare->size();
           ++u32_CurrentDefParameterCounter)
      {
         bool q_CurrentParameterFound = false;

         for (u32_UpdatedDefParameterCounter = 0UL;
              u32_UpdatedDefParameterCounter < pc_UpdatedParametersToCompare->size();
              ++u32_UpdatedDefParameterCounter)
         {
            if ((*pc_CurrentParametersToCompare)[u32_CurrentDefParameterCounter].c_Id ==
                (*pc_UpdatedParametersToCompare)[u32_UpdatedDefParameterCounter].c_Id)
            {
               q_CurrentParameterFound = true;
               break;
            }
         }

         if (q_CurrentParameterFound == false)
         {
            // Reporting parameter removed
            this->mc_RemovedContent.append(static_cast<QString>(C_GtGetText::h_GetText("Parameter %1 of channel %2"))
                                           .arg((*pc_CurrentParametersToCompare)[u32_CurrentDefParameterCounter].
                                                c_Display.c_str())
                                           .arg(rc_UpdatedChannelConfig.c_Name.c_str()));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the channel sub parameter configuration by merging the loaded and current channel configuration

   \param[in]     orc_CurrentParameterDef     Current parameter definition
   \param[in]     orc_UpdatedParameterDef     Updated parameter definition
   \param[in]     oq_IsDomainConfiguration    Flag if the current channel configuration is a domain configuration or a
                                              channel configuration
   \param[in]     ou32_UseCaseIndex           Current channel use case index of configuration with
                                              user specific adaptions (if q_IsDomainConfiguration == false)
   \param[in]     orc_CurrentParameterConfig  Current sub parameter configuration
   \param[in,out] orc_UpdatedParameterConfig  Parameter  sub configuration to update. Configuration must be set by
                                              default values with the channel configuration of the loaded definition
   \param[in]     orc_ChannelName             Name of channel for reporting only
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_UpdateSubParameterConfiguration(const C_OscHalcDefStruct & orc_CurrentParameterDef,
                                                                   const C_OscHalcDefStruct & orc_UpdatedParameterDef,
                                                                   const bool oq_IsDomainConfiguration,
                                                                   const uint32_t ou32_UseCaseIndex,
                                                                   const C_OscHalcConfigParameterStruct & orc_CurrentParameterConfig, C_OscHalcConfigParameterStruct & orc_UpdatedParameterConfig,
                                                                   const C_SclString & orc_ChannelName)
{
   uint32_t u32_UpdatedDefSubParameterCounter;
   uint32_t u32_CurrentDefSubParameterCounter;

   for (u32_UpdatedDefSubParameterCounter = 0UL;
        u32_UpdatedDefSubParameterCounter < orc_UpdatedParameterDef.c_StructElements.size();
        ++u32_UpdatedDefSubParameterCounter)
   {
      bool q_ParameterFound = false;
      const C_OscHalcDefElement & rc_UpdatedParameterDef =
         orc_UpdatedParameterDef.c_StructElements[u32_UpdatedDefSubParameterCounter];

      for (u32_CurrentDefSubParameterCounter = 0UL;
           u32_CurrentDefSubParameterCounter < orc_CurrentParameterDef.c_StructElements.size();
           ++u32_CurrentDefSubParameterCounter)
      {
         const C_OscHalcDefElement & rc_CurrentParameterDef =
            orc_CurrentParameterDef.c_StructElements[u32_CurrentDefSubParameterCounter];

         // Search the matching parameters by comparing the names of the definition
         if (rc_CurrentParameterDef.c_Id == rc_UpdatedParameterDef.c_Id)
         {
            // Same parameter
            q_ParameterFound = true;

            tgl_assert(
               orc_CurrentParameterConfig.c_ParameterElements.size() ==
               orc_CurrentParameterDef.c_StructElements.size());
            tgl_assert(
               orc_UpdatedParameterConfig.c_ParameterElements.size() ==
               orc_UpdatedParameterDef.c_StructElements.size());

            this->m_UpdateParameterElementConfiguration(
               rc_CurrentParameterDef, rc_UpdatedParameterDef,
               oq_IsDomainConfiguration,
               ou32_UseCaseIndex,
               orc_CurrentParameterConfig.c_ParameterElements[u32_CurrentDefSubParameterCounter],
               orc_UpdatedParameterConfig.c_ParameterElements[u32_UpdatedDefSubParameterCounter],
               orc_ChannelName);

            break;
         }
      }

      if (q_ParameterFound == false)
      {
         // Reporting new sub parameter
         this->mc_NewContent.append(static_cast<QString>(C_GtGetText::h_GetText("Parameter %1 of channel %2"))
                                    .arg(rc_UpdatedParameterDef.c_Display.c_str())
                                    .arg(orc_ChannelName.c_str()));
      }
   }

   // Check parameters with second loop which parameters were removed for reporting by
   // searching all current parameter names in the updated configuration
   for (u32_CurrentDefSubParameterCounter = 0UL;
        u32_CurrentDefSubParameterCounter < orc_CurrentParameterDef.c_StructElements.size();
        ++u32_CurrentDefSubParameterCounter)
   {
      bool q_CurrentParameterFound = false;

      for (u32_UpdatedDefSubParameterCounter = 0UL;
           u32_UpdatedDefSubParameterCounter < orc_UpdatedParameterDef.c_StructElements.size();
           ++u32_UpdatedDefSubParameterCounter)
      {
         if (orc_CurrentParameterDef.c_StructElements[u32_CurrentDefSubParameterCounter].c_Id ==
             orc_UpdatedParameterDef.c_StructElements[u32_UpdatedDefSubParameterCounter].c_Id)
         {
            q_CurrentParameterFound = true;
            break;
         }
      }

      if (q_CurrentParameterFound == false)
      {
         // Reporting sub parameter removed
         this->mc_RemovedContent.append(static_cast<QString>(C_GtGetText::h_GetText("Parameter %1 of channel %2"))
                                        .arg(orc_CurrentParameterDef.c_StructElements[u32_CurrentDefSubParameterCounter]
                                             .c_Display.c_str())
                                        .arg(orc_ChannelName.c_str()));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the domain channel parameter configuration by merging the loaded and current channel configuration

   \param[in]     orc_CurrentParameterDef     Current parameter definition
   \param[in]     orc_UpdatedParameterDef     Updated parameter definition
   \param[in]     oq_IsDomainConfiguration    Flag if the current channel configuration is a domain configuration or a
                                              channel configuration
   \param[in]     ou32_UseCaseIndex           Current channel use case index of configuration with
                                              user specific adaptions (if q_IsDomainConfiguration == false)
   \param[in]     orc_CurrentParameterConfig  Current parameter configuration
   \param[in,out] orc_UpdatedParameterConfig  Parameter configuration to update. Configuration must be set by default
                                              values with the channel configuration of the loaded definition
   \param[in]     orc_ChannelName             Name of channel for reporting only
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcDefUpdateDialog::m_UpdateParameterElementConfiguration(
   const C_OscHalcDefElement & orc_CurrentParameterDef, const C_OscHalcDefElement & orc_UpdatedParameterDef,
   const bool oq_IsDomainConfiguration, const uint32_t ou32_UseCaseIndex,
   const C_OscHalcConfigParameter & orc_CurrentParameterConfig, C_OscHalcConfigParameter & orc_UpdatedParameterConfig,
   const C_SclString & orc_ChannelName)
{
   // Check if compatible
   // Check use case of current parameter
   bool q_ParameterRelevantAndCompatible = C_SdNdeHalcDefUpdateDialog::mh_CheckParameterDefForUseCase(
      orc_CurrentParameterDef, oq_IsDomainConfiguration, ou32_UseCaseIndex);
   // Check use case of updated parameter. Could be different
   const bool q_UpdatedParameterRelevantForUseCase = C_SdNdeHalcDefUpdateDialog::mh_CheckParameterDefForUseCase(
      orc_UpdatedParameterDef, oq_IsDomainConfiguration, ou32_UseCaseIndex);

   if (q_ParameterRelevantAndCompatible == true)
   {
      q_ParameterRelevantAndCompatible = q_UpdatedParameterRelevantForUseCase;
   }
   else if (q_UpdatedParameterRelevantForUseCase == true)
   {
      // In previous definition the parameter was not relevant for the use case but with the new definition it is
      // Nothing to update, but to report
      this->mc_NewContent.append(static_cast<QString>(C_GtGetText::h_GetText(
                                                         "Parameter %1 of channel %2 is now available for its set use case"))
                                 .arg(orc_UpdatedParameterDef.c_Display.c_str())
                                 .arg(orc_ChannelName.c_str()));
   }
   else
   {
      // Nothing to do, in previous and updated definition the parameter is not relevant for the use case
   }

   // Check the content member (min/max/initial and current content)
   if (q_ParameterRelevantAndCompatible == true)
   {
      // Check the parameter content
      if (orc_CurrentParameterDef.c_InitialValue.GetType() ==
          orc_UpdatedParameterDef.c_InitialValue.GetType())
      {
         // Type has not changed
         // Check the min and max value against the current configuration value
         if ((orc_CurrentParameterConfig.c_Value >= orc_UpdatedParameterDef.c_MinValue) &&
             (orc_CurrentParameterConfig.c_Value <= orc_UpdatedParameterDef.c_MaxValue))
         {
            // Parameter content is compatible, assign the old value to the updated configuration
            orc_UpdatedParameterConfig.c_Value = orc_CurrentParameterConfig.c_Value;
         }
         else
         {
            // Reporting parameter not updated due to changed min max values
            this->mc_ResetContent.append(static_cast<QString>(C_GtGetText::h_GetText(
                                                                 "Configured parameter %1 of channel %2 is not "
                                                                 "updated due to changed range and was reset to default."))
                                         .arg(orc_UpdatedParameterDef.c_Display.c_str())
                                         .arg(orc_ChannelName.c_str()));
         }
      }
      else
      {
         // Reporting parameter not updated due to changed value type
         this->mc_ResetContent.append(static_cast<QString>(C_GtGetText::h_GetText(
                                                              "Configured parameter %1 of channel %2 is not "
                                                              "updated due to changed value type and was reset to default."))
                                      .arg(orc_UpdatedParameterDef.c_Display.c_str())
                                      .arg(orc_ChannelName.c_str()));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if a parameter definition is relevant for a specific use case index

   \param[in]     orc_UpdatedParameterDef     Parameter definition to check
   \param[in]     oq_IsDomainConfiguration    Flag if the current channel configuration is a domain configuration or a
                                              channel configuration
   \param[in]     ou32_UseCaseIndex           Current channel use case index of configuration with
                                              user specific adaptions (if q_IsDomainConfiguration == false)

   \retval   true   Parameter definition is relevant for the use case index
   \retval   false  Parameter definition is not relevant for the use case index
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcDefUpdateDialog::mh_CheckParameterDefForUseCase(const C_OscHalcDefElement & orc_ParameterDef,
                                                                const bool oq_IsDomainConfiguration,
                                                                const uint32_t ou32_UseCaseIndex)
{
   bool q_ParameterRelevantAndCompatible = false;

   if (oq_IsDomainConfiguration == false)
   {
      uint32_t u32_UseCaseAvailabilitiesCounter;

      for (u32_UseCaseAvailabilitiesCounter = 0U;
           u32_UseCaseAvailabilitiesCounter < orc_ParameterDef.c_UseCaseAvailabilities.size();
           ++u32_UseCaseAvailabilitiesCounter)
      {
         if (orc_ParameterDef.c_UseCaseAvailabilities[u32_UseCaseAvailabilitiesCounter] == ou32_UseCaseIndex)
         {
            q_ParameterRelevantAndCompatible = true;
         }
      }
   }
   else
   {
      q_ParameterRelevantAndCompatible = true;
   }

   return q_ParameterRelevantAndCompatible;
}
