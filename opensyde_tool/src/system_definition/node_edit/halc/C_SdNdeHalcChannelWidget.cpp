//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing hardware configuration of a specific channel

   Widget for displaying and editing hardware configuration of a specific channel

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_SdNdeHalcChannelDpPreviewPopUp.hpp"
#include "C_SdNdeHalcChannelWidget.hpp"
#include "ui_C_SdNdeHalcChannelWidget.h"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::tgl;
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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelWidget::C_SdNdeHalcChannelWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcChannelWidget),
   mu32_NodeIndex(0),
   mu32_DomainIndex(0),
   mu32_ChannelIndex(0),
   mq_UseChannelIndex(true),
   ms32_LastComboboxUseCaseIndex(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // name length restriction
   this->mpc_Ui->pc_LeName->setMaxLength(ms32_C_ITEM_MAX_CHAR_COUNT);

   // set color
   this->mpc_Ui->pc_LabChannelTitle->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabChannelTitle->SetForegroundColor(4);
   this->mpc_Ui->pc_LabChannelTitle->SetFontPixel(13);
   this->mpc_Ui->pc_LabChannelCurrent->SetBackgroundColor(11);
   this->mpc_Ui->pc_LabChannelCurrent->SetForegroundColor(1);
   this->mpc_Ui->pc_LabChannelCurrent->SetFontPixel(13);
   this->mpc_Ui->pc_LabViewDatapools->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LabViewDatapools->setOpenExternalLinks(false);
   this->mpc_Ui->pc_LabViewDatapools->setFocusPolicy(Qt::NoFocus);
   this->mpc_Ui->pc_LabDomainName->SetForegroundColor(7);
   this->mpc_Ui->pc_LabDomainName->SetFontPixel(36, true);

   // linked channel visualization
   this->mpc_Ui->pc_PubLinkedIcon->SetSvg("://images/system_definition/NodeEdit/halc/IconLink.svg");
   this->mpc_Ui->pc_PubLinkedIcon->setVisible(false);
   this->mpc_Ui->pc_LabLinkedChannel->setText("");
   this->mpc_Ui->pc_LabLinkedChannel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LabLinkedChannel->setFocusPolicy(Qt::NoFocus);

   // remove strings
   this->mpc_Ui->pc_LabChannelCurrent->setText("");
   this->mpc_Ui->pc_ChxSafety->setText("");
   this->mpc_Ui->pc_LabSafetyState->setText("");

   // label safety state is invisible on default
   this->mpc_Ui->pc_LabSafetyState->setVisible(false);

   // enable tab key navigation
   this->mpc_Ui->pc_TreeConfig->setTabKeyNavigation(true);

   // connects
   connect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::textChanged,
           this, &C_SdNdeHalcChannelWidget::m_CheckName);

   this->m_ConnectWidgets(true); // special handling of widgets that write to data handling
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelWidget::~C_SdNdeHalcChannelWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::InitStaticNames(void) const
{
   // properties section
   this->mpc_Ui->pc_LabName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabChannelTitle->setText(C_GtGetText::h_GetText("Channel"));
   this->mpc_Ui->pc_LabComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_TedComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here..."));

   // configuration section
   this->mpc_Ui->pc_LabSafety->setText(C_GtGetText::h_GetText("Safety Relevant"));
   this->mpc_Ui->pc_LabConfig->setText(C_GtGetText::h_GetText("Configuration"));
   this->mpc_Ui->pc_LabUseCase->setText(C_GtGetText::h_GetText("Use Case"));
   this->mpc_Ui->pc_LabViewDatapools->setText(C_Uti::h_GetLink("View Datapool Details", mc_STYLE_GUIDE_COLOR_6,
                                                               "View Datapool Details"));

   // tool tips
   this->mpc_Ui->pc_LabName->
   SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                         C_GtGetText::h_GetText("Symbolic channel name. Unique within node.\n"
                                                "Following C naming conventions are required:"
                                                "\n - must not be empty"
                                                "\n - must not start with digits"
                                                "\n - only alphanumeric characters and \"_\""
                                                "\n - should not be longer than 31 characters"));
   this->mpc_Ui->pc_LabChannelTitle->
   SetToolTipInformation(C_GtGetText::h_GetText("Channel"),
                         C_GtGetText::h_GetText("Channel ID from hardware description."));
   this->mpc_Ui->pc_LabComment->
   SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                         C_GtGetText::h_GetText("Comment for this channel."));
   this->mpc_Ui->pc_LabSafety->
   SetToolTipInformation(C_GtGetText::h_GetText("Safety Relevant"),
                         C_GtGetText::h_GetText("Does the current channel hold safety relevant parameters?"));
   this->mpc_Ui->pc_LabUseCase->
   SetToolTipInformation(C_GtGetText::h_GetText("Use Case"),
                         C_GtGetText::h_GetText("Select a use case to configure the current channel. Depending on the "
                                                "underlying hardware description, there can be use cases that link two "
                                                "channels together."));
   this->mpc_Ui->pc_LabLinkedChannel->
   SetToolTipInformation(C_GtGetText::h_GetText("Linked Channel"),
                         C_GtGetText::h_GetText("The current channel is linked to another channel. All parameter "
                                                "values are synchronized. Click link to navigate to linked channel."));
   this->mpc_Ui->pc_PubLinkedIcon->
   SetToolTipInformation(C_GtGetText::h_GetText("Linked Channel"),
                         C_GtGetText::h_GetText("The current channel is linked to another channel. All parameter "
                                                "values are synchronized. Click link to navigate to partner channel."));
   this->mpc_Ui->pc_LabViewDatapools->
   SetToolTipInformation(C_GtGetText::h_GetText("View Datapool Details"),
                         C_GtGetText::h_GetText("Show lists and its elements of the Datapool that gets generated from "
                                                "the current hardware configuration."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update from data handling.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mpc_Ui->pc_TreeConfig->SetNode(this->mu32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set channel

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::SetChannel(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex)
{
   this->mu32_DomainIndex = ou32_DomainIndex;
   this->mu32_ChannelIndex = ou32_ChannelIndex;

   this->m_LoadChannelData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload channel data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::ReloadChannel(void)
{
   this->m_LoadChannelData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set edit focus on name line edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::SetNameFocus(void) const
{
   if (this->mpc_Ui->pc_LeName->isVisible() == true)
   {
      this->mpc_Ui->pc_LeName->setFocus();
      this->mpc_Ui->pc_LeName->selectAll();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::LoadUserSettings(void) const
{
   this->mpc_Ui->pc_TreeConfig->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::SaveUserSettings(void) const
{
   this->mpc_Ui->pc_TreeConfig->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of editing finished of channel name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnNameEdited(void)
{
   int32_t s32_Return;
   const stw::scl::C_SclString c_NewName = this->mpc_Ui->pc_LeName->text().toStdString().c_str();

   // update data
   s32_Return =  C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelConfigName(mu32_NodeIndex, mu32_DomainIndex,
                                                                                 mu32_ChannelIndex, mq_UseChannelIndex,
                                                                                 c_NewName);
   tgl_assert(s32_Return == C_NO_ERR);

   // inform about update
   this->m_EmitUpdateSignal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of comment edited.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnCommentEdited(void)
{
   int32_t s32_Return;
   const stw::scl::C_SclString c_NewComment = this->mpc_Ui->pc_TedComment->toPlainText().toStdString().c_str();

   // update data
   s32_Return = C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelConfigComment(mu32_NodeIndex, mu32_DomainIndex,
                                                                                   mu32_ChannelIndex,
                                                                                   mq_UseChannelIndex, c_NewComment);

   tgl_assert(s32_Return == C_NO_ERR);

   // inform about update
   this->m_EmitUpdateSignal();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of safety checkbox toggle.

   \param[in]  oq_Checked  true: safety relevant on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnSafetyToggled(const bool oq_Checked) const
{
   int32_t s32_Return;

   // update data
   s32_Return = C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelConfigSafety(mu32_NodeIndex, mu32_DomainIndex,
                                                                                  mu32_ChannelIndex, mq_UseChannelIndex,
                                                                                  oq_Checked);
   tgl_assert(s32_Return == C_NO_ERR);

   // no update of any other GUI element necessary here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Honors the Halc Safety Mode and makes SafetyRelevant read-only accordingly
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_HandleHalcNvmFlag(void)
{
   const C_OscHalcConfig * const pc_HalcConfig = C_PuiSdHandler::h_GetInstance()->GetHalcConfig(this->mu32_NodeIndex);

   if (pc_HalcConfig->e_SafetyMode == C_OscHalcConfig::eONE_LEVEL_ALL_SAFE)
   {
      this->mpc_Ui->pc_ChxSafety->setVisible(false);
      this->mpc_Ui->pc_LabSafetyState->setVisible(true);
      this->mpc_Ui->pc_LabSafetyState->setText("ON");
   }
   else if (pc_HalcConfig->e_SafetyMode == C_OscHalcConfig::eONE_LEVEL_ALL_NON_SAFE)
   {
      this->mpc_Ui->pc_ChxSafety->setVisible(false);
      this->mpc_Ui->pc_LabSafetyState->setVisible(true);
      this->mpc_Ui->pc_LabSafetyState->setText("OFF");
   }
   else
   {
      this->mpc_Ui->pc_ChxSafety->setVisible(true);
      this->mpc_Ui->pc_LabSafetyState->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for use case combo box index change.

   \param[in]  os32_NewIndex  New combobox index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnUseCaseChanged(const int32_t os32_NewIndex)
{
   if (this->ms32_LastComboboxUseCaseIndex != os32_NewIndex)
   {
      if ((os32_NewIndex >= 0) && (os32_NewIndex < static_cast<int32_t>(this->mc_CbxUseCaseIndices.size())))
      {
         bool q_IsLinkedOld;
         bool q_IsLinkedNew;
         std::vector<QString> c_LinkedChannelNamesOld;
         std::vector<QString> c_LinkedChannelNamesNew;
         std::vector<uint32_t> c_LinkedChannelIndicesOld;
         std::vector<uint32_t> c_LinkedChannelIndicesNew;
         int32_t s32_Result;
         const uint32_t u32_UseCaseIndexNew = this->mc_CbxUseCaseIndices[os32_NewIndex];
         uint32_t u32_UseCaseIndexOld = 0;
         if ((this->ms32_LastComboboxUseCaseIndex >= 0) &&
             (this->ms32_LastComboboxUseCaseIndex < static_cast<int32_t>(this->mc_CbxUseCaseIndices.size())))
         {
            u32_UseCaseIndexOld = this->mc_CbxUseCaseIndices[this->ms32_LastComboboxUseCaseIndex];
         }

         s32_Result =
            C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                          this->mu32_ChannelIndex, true, q_IsLinkedOld,
                                                                          &c_LinkedChannelNamesOld,
                                                                          &c_LinkedChannelIndicesOld);
         tgl_assert(s32_Result == C_NO_ERR);
         s32_Result =
            C_PuiSdHandler::h_GetInstance()->CheckHalcDomainChannelLinked(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                          this->mu32_ChannelIndex, true, q_IsLinkedNew,
                                                                          &c_LinkedChannelNamesNew,
                                                                          &c_LinkedChannelIndicesNew,
                                                                          &u32_UseCaseIndexNew);
         tgl_assert(s32_Result == C_NO_ERR);
         if (this->m_AskUserToContinueLinkingIfNecessary(q_IsLinkedOld, q_IsLinkedNew,
                                                         c_LinkedChannelNamesOld, c_LinkedChannelNamesNew,
                                                         u32_UseCaseIndexNew) == true)
         {
            // update data
            s32_Result =
               C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelConfigUseCase(this->mu32_NodeIndex,
                                                                                  this->mu32_DomainIndex,
                                                                                  this->mu32_ChannelIndex,
                                                                                  this->mq_UseChannelIndex,
                                                                                  u32_UseCaseIndexNew);
            tgl_assert(s32_Result == C_NO_ERR);

            // update linked channels
            s32_Result =
               C_PuiSdHandler::h_GetInstance()->SetHalcDomainChannelConfigOfLinkedChannels(this->mu32_NodeIndex,
                                                                                           this->mu32_DomainIndex,
                                                                                           this->mu32_ChannelIndex,
                                                                                           this->mq_UseChannelIndex,
                                                                                           u32_UseCaseIndexOld,
                                                                                           u32_UseCaseIndexNew);
            tgl_assert(s32_Result == C_NO_ERR);

            // update icon & link of linked channel
            this->m_SetLinkedSymbolAndChannel(q_IsLinkedNew, c_LinkedChannelIndicesNew);

            // update configuration tree
            this->mpc_Ui->pc_TreeConfig->SetHalcChannelUseCase(this->mu32_DomainIndex, this->mu32_ChannelIndex,
                                                               u32_UseCaseIndexNew, mq_UseChannelIndex);
            this->mpc_Ui->pc_TreeConfig->expandAll();
            this->ms32_LastComboboxUseCaseIndex = os32_NewIndex;

            // update channel tree icon
            this->m_EmitUpdateSignal();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  If an linked use case is set, set a linked symbol and the linked channel

   \param[in]  oq_IsLinked                Flag if channel is linked or not
   \param[in]  orc_LinkedChannelIndices   Linked channel indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_SetLinkedSymbolAndChannel(const bool oq_IsLinked,
                                                           const std::vector<uint32_t> & orc_LinkedChannelIndices) const
{
   QString c_LinkText = "-";

   // get link text
   if ((oq_IsLinked == true) && (orc_LinkedChannelIndices.size() > 0))
   {
      const uint32_t u32_LinkedChannelIndex = orc_LinkedChannelIndices[0];
      const C_OscHalcConfigChannel * const pc_LinkedChannel =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                         u32_LinkedChannelIndex, true);
      const C_OscHalcConfigDomain * const pc_Domain =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(this->mu32_NodeIndex, this->mu32_DomainIndex);

      if ((pc_LinkedChannel != NULL) && (pc_Domain != NULL) && (u32_LinkedChannelIndex < pc_Domain->c_Channels.size()))
      {
         c_LinkText = static_cast<QString>("%1 (%2)").arg(pc_LinkedChannel->c_Name.c_str(),
                                                          pc_Domain->c_Channels[u32_LinkedChannelIndex].c_Name.c_str());
      }
   }

   // update link text
   this->mpc_Ui->pc_LabLinkedChannel->setText(C_Uti::h_GetLink(c_LinkText, mc_STYLE_GUIDE_COLOR_6, c_LinkText));

   // update visibility
   this->mpc_Ui->pc_PubLinkedIcon->setVisible(oq_IsLinked);
   this->mpc_Ui->pc_LabLinkedChannel->setVisible(oq_IsLinked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of linked channel name clicked.

   \param[in]  orc_LinkedChannelName   The name of the linked channel
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnLinkedChannelClicked(const QString & orc_LinkedChannelName)
{
   // get domain
   const C_OscHalcConfigDomain * const pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(this->mu32_NodeIndex, this->mu32_DomainIndex);

   for (uint32_t u32_Counter = 0; u32_Counter < pc_Domain->c_Channels.size(); u32_Counter++)
   {
      if (orc_LinkedChannelName.contains(pc_Domain->c_Channels.at(u32_Counter).c_Name.c_str()) == true)
      {
         Q_EMIT (this->SigChannelSelected(this->mu32_DomainIndex, u32_Counter, this->mq_UseChannelIndex));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of view datapool details clicked.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnViewDatapoolDetailsClicked(void)
{
   // Make sure HALC magician did its work
   const int32_t s32_Result = C_PuiSdHandler::h_GetInstance()->HalcGenerateDatapools(this->mu32_NodeIndex);

   tgl_assert((s32_Result == C_NO_ERR) || (s32_Result == C_NOACT));

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   new C_SdNdeHalcChannelDpPreviewPopUp(*c_New,
                                        this->mu32_NodeIndex,
                                        this->mu32_DomainIndex,
                                        this->mu32_ChannelIndex,
                                        this->mq_UseChannelIndex);

   //Resize
   c_New->SetSize(QSize(1160, 740));

   c_New->exec();

   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel selected click.

   Load channel data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_LoadChannelData(void)
{
   // get domain
   const C_OscHalcConfigDomain * const pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(this->mu32_NodeIndex, this->mu32_DomainIndex);

   if (pc_Domain != NULL)
   {
      // handle domain without channels
      this->mq_UseChannelIndex = !(pc_Domain->c_Channels.empty());

      // get channel
      const C_OscHalcConfigChannel * const pc_Channel =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                         this->mu32_ChannelIndex,
                                                                         this->mq_UseChannelIndex);

      if (pc_Channel != NULL)
      {
         bool q_IsLinked = false;
         uint32_t u32_UseCaseCounter = 0; // for counting all channels and knowing indices of available ones
         int32_t s32_SelectedUseCaseCbxIndex = -1;
         std::vector<uint32_t> c_LinkedChannelIndices;

         // disconnect to avoid data handling update while loading
         this->m_ConnectWidgets(false);

         // adapt GUI elements
         this->m_HandleHalcNvmFlag();
         this->mpc_Ui->pc_LeName->setText(pc_Channel->c_Name.c_str());
         this->mpc_Ui->pc_LeName->setReadOnly(!this->mq_UseChannelIndex); // disable name editing in domain case
         this->mpc_Ui->pc_TedComment->setText(pc_Channel->c_Comment.c_str());
         this->mpc_Ui->pc_ChxSafety->setChecked(pc_Channel->q_SafetyRelevant);
         this->mpc_Ui->pc_LabDomainName->setText(pc_Domain->c_SingularName.c_str());
         switch (pc_Domain->e_Category)
         {
         case C_OscHalcDefDomain::eCA_INPUT:
            this->mpc_Ui->pc_LabImage->SetSvg("://images/system_definition/NodeEdit/halc/InputLargeCenterActive.svg");
            break;
         case C_OscHalcDefDomain::eCA_OUTPUT:
            this->mpc_Ui->pc_LabImage->SetSvg("://images/system_definition/NodeEdit/halc/OutputLargeCenterActive.svg");
            break;
         case C_OscHalcDefDomain::eCA_OTHER:
            this->mpc_Ui->pc_LabImage->SetSvg("://images/system_definition/NodeEdit/halc/OtherLargeCenterActive.svg");
            break;
         default:
            break;
         }

         // handle current channel name
         if (this->mu32_ChannelIndex < pc_Domain->c_Channels.size())
         {
            this->mpc_Ui->pc_LabChannelCurrent->setText(pc_Domain->c_Channels[this->mu32_ChannelIndex].c_Name.c_str());
            this->mpc_Ui->pc_LabChannelCurrent->SetBackgroundColor(11);
         }
         else
         {
            this->mpc_Ui->pc_LabChannelCurrent->setText("-");
            this->mpc_Ui->pc_LabChannelCurrent->SetBackgroundColor(0);
         }

         // combobox use case
         this->mpc_Ui->pc_CbxUseCase->clear();
         this->mc_CbxUseCaseIndices.clear();

         // use case only matters in channel case
         if (mq_UseChannelIndex == true)
         {
            for (std::vector<C_OscHalcDefChannelUseCase>::const_iterator c_ItUseCases =
                    pc_Domain->c_ChannelUseCases.begin();
                 c_ItUseCases != pc_Domain->c_ChannelUseCases.end(); ++c_ItUseCases)
            {
               const C_OscHalcDefChannelUseCase & rc_CurrentUseCase = *c_ItUseCases;

               // only add available use cases to combobox
               for (std::vector<C_OscHalcDefChannelAvailability>::const_iterator c_ItCurrentAvail =
                       rc_CurrentUseCase.c_Availability.begin();
                    c_ItCurrentAvail != rc_CurrentUseCase.c_Availability.end(); ++c_ItCurrentAvail)
               {
                  const C_OscHalcDefChannelAvailability & rc_CurrentAvail = *c_ItCurrentAvail;

                  if (rc_CurrentAvail.u32_ValueIndex == this->mu32_ChannelIndex)
                  {
                     // add to combobox
                     this->mpc_Ui->pc_CbxUseCase->addItem(rc_CurrentUseCase.c_Display.c_str());

                     // remember index
                     this->mc_CbxUseCaseIndices.push_back(u32_UseCaseCounter);

                     // remember current item index for combobox & link property for link
                     if ((pc_Channel->u32_UseCaseIndex == u32_UseCaseCounter))
                     {
                        s32_SelectedUseCaseCbxIndex = static_cast<int32_t>(this->mpc_Ui->pc_CbxUseCase->count() - 1);
                        if (rc_CurrentAvail.c_DependentValues.empty() == false)
                        {
                           q_IsLinked = true;
                           c_LinkedChannelIndices = rc_CurrentAvail.c_DependentValues;
                        }
                     }
                     break;
                  }
               }
               u32_UseCaseCounter++;
            }
         }

         // update linked channel visualization
         this->ms32_LastComboboxUseCaseIndex = s32_SelectedUseCaseCbxIndex;
         this->m_SetLinkedSymbolAndChannel(q_IsLinked, c_LinkedChannelIndices);

         // update combobox visibility
         this->mpc_Ui->pc_CbxUseCase->setVisible(this->mq_UseChannelIndex);
         this->mpc_Ui->pc_LabNoUseCase->setVisible(!this->mq_UseChannelIndex);

         // update configuration tree
         this->mpc_Ui->pc_CbxUseCase->setCurrentIndex(s32_SelectedUseCaseCbxIndex);
         this->mpc_Ui->pc_TreeConfig->SetHalcChannelUseCase(mu32_DomainIndex, mu32_ChannelIndex,
                                                            pc_Channel->u32_UseCaseIndex,
                                                            mq_UseChannelIndex);

         // reconnect
         this->m_ConnectWidgets(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Emit update signal for informing that some data changed.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_EmitUpdateSignal(void)
{
   Q_EMIT (this->SigUpdateChannel(this->mu32_DomainIndex, this->mu32_ChannelIndex, this->mq_UseChannelIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Connect or disconnect all edit widgets

   \param[in]  oq_Connect  true: connect; false: disconnect
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_ConnectWidgets(const bool oq_Connect) const
{
   if (oq_Connect == true)
   {
      connect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::editingFinished,
              this, &C_SdNdeHalcChannelWidget::m_OnNameEdited);
      connect(this->mpc_Ui->pc_TedComment, &C_OgeTedPropertiesComment::SigEditingFinished,
              this, &C_SdNdeHalcChannelWidget::m_OnCommentEdited);
      connect(this->mpc_Ui->pc_ChxSafety, &C_OgeChxTristateTransparentToggle::toggled,
              this, &C_SdNdeHalcChannelWidget::m_OnSafetyToggled);
      connect(this->mpc_Ui->pc_LabLinkedChannel, &QLabel::linkActivated,
              this, &C_SdNdeHalcChannelWidget::m_OnLinkedChannelClicked);
      //lint -save -e929 Cast required to avoid ambiguous signal of Qt interface
      connect(this->mpc_Ui->pc_LabViewDatapools, &QLabel::linkActivated,
              this, &C_SdNdeHalcChannelWidget::m_OnViewDatapoolDetailsClicked);
      connect(this->mpc_Ui->pc_CbxUseCase, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeHalcChannelWidget::m_OnUseCaseChanged);
      //lint -restore
   }
   else
   {
      disconnect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::editingFinished,
                 this, &C_SdNdeHalcChannelWidget::m_OnNameEdited);
      disconnect(this->mpc_Ui->pc_TedComment, &C_OgeTedPropertiesComment::SigEditingFinished,
                 this, &C_SdNdeHalcChannelWidget::m_OnCommentEdited);
      disconnect(this->mpc_Ui->pc_ChxSafety, &C_OgeChxTristateTransparentToggle::toggled,
                 this, &C_SdNdeHalcChannelWidget::m_OnSafetyToggled);
      disconnect(this->mpc_Ui->pc_LabLinkedChannel, &QLabel::linkActivated,
                 this, &C_SdNdeHalcChannelWidget::m_OnLinkedChannelClicked);
      //lint -save -e929 Cast required to avoid ambiguous signal of Qt interface
      disconnect(this->mpc_Ui->pc_LabViewDatapools, &QLabel::linkActivated,
                 this, &C_SdNdeHalcChannelWidget::m_OnViewDatapoolDetailsClicked);
      disconnect(this->mpc_Ui->pc_CbxUseCase,
                 static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged),
                 this, &C_SdNdeHalcChannelWidget::m_OnUseCaseChanged);
      //lint -restore
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check name for valid characters and duplicates.

   Slot of textChanged signal of name line edit.

   \param[in]  orc_NewName    New name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_CheckName(const QString & orc_NewName) const
{
   const stw::scl::C_SclString c_NewName = orc_NewName.toStdString().c_str();

   //check name
   const bool q_NameIsUnique =
      C_PuiSdHandler::h_GetInstance()->CheckHalcChannelNameAvailable(mu32_NodeIndex, mu32_DomainIndex,
                                                                     c_NewName, &mu32_ChannelIndex);
   const bool q_NameIsValid = C_OscUtils::h_CheckValidCeName(c_NewName);

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LeName, "Valid", (q_NameIsUnique && q_NameIsValid));

   if ((q_NameIsUnique == true) && (q_NameIsValid == true))
   {
      this->mpc_Ui->pc_LeName->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                     C_GtGetText::h_GetText(""),
                                                     C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Channel Name");
      QString c_Content;
      if (q_NameIsUnique == false)
      {
         c_Content += C_GtGetText::h_GetText("- is already in use\n");
      }
      if (q_NameIsValid == false)
      {
         c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters.\n");
      }
      this->mpc_Ui->pc_LeName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Ask user to continue linking if necessary

   \param[in]  oq_IsLinkedOld             was linked
   \param[in]  oq_IsLinkedNew             will be linked
   \param[in]  orc_LinkedChannelNamesOld  Linked channel names of old use case
   \param[in]  orc_LinkedChannelNamesNew  Linked channel names of new use case
   \param[in]  ou32_NewUseCaseIndex       New use case index

   \retval   true   continue
   \retval   false  combobox was reverted, nothing to do
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcChannelWidget::m_AskUserToContinueLinkingIfNecessary(const bool oq_IsLinkedOld,
                                                                     const bool oq_IsLinkedNew,
                                                                     const std::vector<QString> & orc_LinkedChannelNamesOld, const
                                                                     std::vector<QString> & orc_LinkedChannelNamesNew,
                                                                     const uint32_t ou32_NewUseCaseIndex)
{
   bool q_Continue = true;

   if ((oq_IsLinkedOld == true) || (oq_IsLinkedNew == true))
   {
      // if linking/unlinking: ask user to continue
      const C_OscHalcDefChannelUseCase * const pc_UseCase =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainFileUseCaseData(this->mu32_NodeIndex,
                                                                       this->mu32_DomainIndex,
                                                                       ou32_NewUseCaseIndex);
      if (pc_UseCase != NULL)
      {
         const std::vector<QString> & rc_LinkedChannelNames =
            (oq_IsLinkedNew == true) ? orc_LinkedChannelNamesNew : orc_LinkedChannelNamesOld;
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         QString c_Description;
         QString c_LinkedChannels = "";

         for (std::vector<QString>::const_iterator c_ItLinkedNames = rc_LinkedChannelNames.begin();
              c_ItLinkedNames != rc_LinkedChannelNames.end(); ++c_ItLinkedNames)
         {
            if (c_ItLinkedNames != rc_LinkedChannelNames.begin())
            {
               c_LinkedChannels += ", ";
            }
            c_LinkedChannels += "'" + *c_ItLinkedNames + "'";
         }

         if (oq_IsLinkedNew == true)
         {
            c_Description =
               static_cast<QString>(C_GtGetText::h_GetText("Are you sure to select the use case '%1'?\n"
                                                           "Linked channel %2 will lose its configuration "
                                                           "and also be set to this use case. After linking, the configuration is "
                                                           "always applied to both channels.")).
               arg(pc_UseCase->c_Display.c_str(), c_LinkedChannels);
            c_MessageBox.SetCustomMinHeight(230, 230);
         }
         else
         {
            c_Description =
               static_cast<QString>(C_GtGetText::h_GetText("Are you sure to select the use case '%1'?\n"
                                                           "Use case of linked channel %2 will be reset to default.")).
               arg(pc_UseCase->c_Display.c_str(), c_LinkedChannels);
            c_MessageBox.SetCustomMinHeight(200, 200);
         }

         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Use Case Selection"));
         c_MessageBox.SetDescription(c_Description);
         c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Apply Selection"));
         c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));

         if (c_MessageBox.Execute() != C_OgeWiCustomMessage::eYES)
         {
            q_Continue = false;
            // reset
            this->mpc_Ui->pc_CbxUseCase->setCurrentIndex(this->ms32_LastComboboxUseCaseIndex);
         }
      }
   }

   return q_Continue;
}
