//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing hardware configuration of a specific channel

   Widget for displaying and editing hardware configuration of a specific channel

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeHalcChannelWidget.h"
#include "ui_C_SdNdeHalcChannelWidget.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_OSCUtils.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_tgl;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

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
   mq_UseChannelIndex(true)
{
   this->mpc_Ui->setupUi(this);

   // set tree model (managed here to avoid having to pass data changes through view)
   this->mpc_Ui->pc_TreeConfig->setModel(&this->mc_Model);

   this->InitStaticNames();

   // name length restriction
   this->mpc_Ui->pc_LeName->setMaxLength(msn_C_ITEM_MAX_CHAR_COUNT);

   // linked channel visualization
   this->mpc_Ui->pc_PubLinkedIcon->SetSvg("://images/system_definition/IconShare.svg");
   this->mpc_Ui->pc_PubLinkedIcon->setVisible(false);
   this->mpc_Ui->pc_LabLinkedChannel->setVisible(false);

   // remove strings
   this->mpc_Ui->pc_LabChannelCurrent->setText("");
   this->mpc_Ui->pc_ChxSafety->setText("");

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
   this->mpc_Ui->pc_LabSafety->setText(C_GtGetText::h_GetText("Safety Relevant"));

   // configuration section
   this->mpc_Ui->pc_LabConfig->setText(C_GtGetText::h_GetText("Configuration"));
   this->mpc_Ui->pc_LabUseCase->setText(C_GtGetText::h_GetText("Use Case"));

   // TODO tool tips
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update from data handling.

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::SetNode(const uint32 ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mc_Model.SetNode(this->mu32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set channel

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::SetChannel(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex)
{
   this->mu32_DomainIndex = ou32_DomainIndex;
   this->mu32_ChannelIndex = ou32_ChannelIndex;

   this->m_LoadChannelData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload channel data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::ReloadChannel()
{
   this->m_LoadChannelData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of editing finished of channel name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnNameEdited(void)
{
   sint32 s32_Return;
   const stw_scl::C_SCLString c_NewName = this->mpc_Ui->pc_LeName->text().toStdString().c_str();

   // update data
   s32_Return =  C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigName(mu32_NodeIndex, mu32_DomainIndex,
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
   sint32 s32_Return;
   const stw_scl::C_SCLString c_NewComment = this->mpc_Ui->pc_TedComment->toPlainText().toStdString().c_str();

   // update data
   s32_Return = C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigComment(mu32_NodeIndex, mu32_DomainIndex,
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
   sint32 s32_Return;

   // update data
   s32_Return = C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigSafety(mu32_NodeIndex, mu32_DomainIndex,
                                                                                  mu32_ChannelIndex, mq_UseChannelIndex,
                                                                                  oq_Checked);
   tgl_assert(s32_Return == C_NO_ERR);

   // no update of any other GUI element necessary here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for use case combo box index change.

   \param[in]  os32_NewIndex  New combobox index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnUseCaseChanged(const sint32 os32_NewIndex)
{
   sint32 s32_Return = C_RANGE;

   if ((os32_NewIndex >= 0) && (os32_NewIndex < static_cast<sint32>(this->mc_CbxUseCaseIndices.size())))
   {
      uint32 u32_UseCaseIndex = this->mc_CbxUseCaseIndices[os32_NewIndex];

      // all use cases with default value(s)
      std::vector<C_OSCHalcDefChannelUseCase> c_DefaultUseCases;

      // get domain
      const C_OSCHalcConfigDomain * pc_Domain =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(mu32_NodeIndex, mu32_DomainIndex);
      if (pc_Domain != NULL)
      {
         for (std::vector<C_OSCHalcDefChannelUseCase>::const_iterator c_ItUseCases =
                 pc_Domain->c_ChannelUseCases.begin();
              c_ItUseCases != pc_Domain->c_ChannelUseCases.end(); ++c_ItUseCases)
         {
            const C_OSCHalcDefChannelUseCase & rc_CurrentUseCase = *c_ItUseCases;

            // only add available use cases to combobox
            if (rc_CurrentUseCase.c_DefaultChannels.empty() == false)
            {
               c_DefaultUseCases.push_back(rc_CurrentUseCase);
            }
         }
      }

      const C_OSCHalcDefChannelUseCase * pc_OldUseCase =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileUseCaseData(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                       this->mu32_Value);

      const C_OSCHalcDefChannelUseCase * pc_UseCase =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainFileUseCaseData(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                       u32_UseCaseIndex);

      if ((pc_OldUseCase != NULL) && (pc_UseCase != NULL))
      {
         for (uint32 u32_OldAvaIt = 0U; u32_OldAvaIt < pc_OldUseCase->c_Availability.size(); u32_OldAvaIt++)
         {
            if (pc_OldUseCase->c_Availability.at(u32_OldAvaIt).u32_ValueIndex == this->mu32_ChannelIndex)
            {
               if (pc_OldUseCase->c_Availability.at(u32_OldAvaIt).c_DependentValues.empty() == false)
               {
                  for (uint32 u32_OldCounter = 0U; u32_OldCounter < pc_OldUseCase->c_Availability.at(u32_OldAvaIt)
                       .c_DependentValues.size(); u32_OldCounter++)
                  {
                     for (uint32 u32_NewAvaIt = 0U; u32_NewAvaIt < pc_UseCase->c_Availability.size(); u32_NewAvaIt++)
                     {
                        if (pc_UseCase->c_Availability.at(u32_NewAvaIt).u32_ValueIndex == this->mu32_ChannelIndex)
                        {
                           if (pc_UseCase->c_Availability.at(u32_NewAvaIt).c_DependentValues.size() == 1)
                           {
                              s32_Return = this->m_SetDependentValuesOfNewUseCase(s32_Return, c_DefaultUseCases,
                                                                                  pc_UseCase->c_Availability
                                                                                  .at(u32_NewAvaIt).c_DependentValues,
                                                                                  pc_OldUseCase->c_Availability
                                                                                  .at(u32_OldAvaIt).c_DependentValues
                                                                                  .at(u32_OldCounter),
                                                                                  u32_UseCaseIndex,
                                                                                  pc_Domain);
                           }
                           else if (pc_UseCase->c_Availability.at(u32_NewAvaIt).c_DependentValues.size() > 1)
                           {
                              s32_Return = this->m_SetDependentValuesOfNewUseCase(s32_Return, c_DefaultUseCases,
                                                                                  pc_UseCase->c_Availability
                                                                                  .at(u32_NewAvaIt).c_DependentValues,
                                                                                  0U, u32_UseCaseIndex,
                                                                                  pc_Domain);
                           }
                           else
                           {
                              s32_Return = this->m_FindAndSetDefaultUseCase(s32_Return, c_DefaultUseCases,
                                                                            pc_OldUseCase->c_Availability
                                                                            .at(u32_OldAvaIt).c_DependentValues
                                                                            .at(u32_OldCounter));
                           }
                        }
                     }
                  }
               }
               else
               {
                  for (uint32 u32_AvaIt = 0U; u32_AvaIt < pc_UseCase->c_Availability.size(); u32_AvaIt++)
                  {
                     if (pc_UseCase->c_Availability.at(u32_AvaIt).u32_ValueIndex == this->mu32_ChannelIndex)
                     {
                        if (pc_UseCase->c_Availability.at(u32_AvaIt).c_DependentValues.empty() == false)
                        {
                           s32_Return = this->m_SetDependentValuesOfNewUseCase(s32_Return, c_DefaultUseCases,
                                                                               pc_UseCase->c_Availability.at(u32_AvaIt)
                                                                               .c_DependentValues, 0U,
                                                                               u32_UseCaseIndex,
                                                                               pc_Domain);
                        }
                     }
                  }
               }
            }
         }
      }

      // update data
      s32_Return =
         C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigUseCase(this->mu32_NodeIndex,
                                                                            this->mu32_DomainIndex,
                                                                            this->mu32_ChannelIndex,
                                                                            this->mq_UseChannelIndex,
                                                                            u32_UseCaseIndex);

      // update configuration tree
      this->mc_Model.SetHalcChannelUseCase(this->mu32_DomainIndex, this->mu32_ChannelIndex,
                                           u32_UseCaseIndex, mq_UseChannelIndex);
      this->mpc_Ui->pc_TreeConfig->expandAll(); // TODO decide if some/all/none tree items shall be expanded on use case
                                                // change

      this->mu32_Value = u32_UseCaseIndex;
   }
   tgl_assert(s32_Return == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Find and set use cases of others not linked channels to their "default" configuration

   \param[in,out]  os32_Return               Error type
   \param[in]      orc_DefaultUseCases       All default use cases of this channel
   \param[in]      oru32_OldDependentValue   Dependent value of previous channel

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcChannelWidget::m_FindAndSetDefaultUseCase(sint32 os32_Return,
                                                            const std::vector<C_OSCHalcDefChannelUseCase> & orc_DefaultUseCases,
                                                            const uint32 & oru32_OldDependentValue) const
{
   this->mpc_Ui->pc_PubLinkedIcon->setVisible(false);
   this->mpc_Ui->pc_LabLinkedChannel->setVisible(false);
   for (uint32 u32_DefaultUseCaseIt = 0U; u32_DefaultUseCaseIt < orc_DefaultUseCases.size();
        u32_DefaultUseCaseIt++)
   {
      for (uint32 u32_DefaultIt = 0U;
           u32_DefaultIt < orc_DefaultUseCases.at(u32_DefaultUseCaseIt).c_DefaultChannels.size(); u32_DefaultIt++)
      {
         if (orc_DefaultUseCases.at(u32_DefaultUseCaseIt).c_DefaultChannels.at(u32_DefaultIt) ==
             this->mu32_ChannelIndex)
         {
            // update data
            os32_Return =
               C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigUseCase(
                  this->mu32_NodeIndex,
                  this->mu32_DomainIndex,
                  oru32_OldDependentValue,
                  this->mq_UseChannelIndex,
                  static_cast<uint32>(orc_DefaultUseCases.at(u32_DefaultUseCaseIt).c_Value.GetValueU8()));
         }
      }
   }
   return os32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set one or more dependent values of new use case

   \param[in,out]  os32_Return               Error type
   \param[in]      orc_DefaultUseCases       All default use cases of this channel
   \param[in]      orc_NewDependentValues    Dependent values of current channel
   \param[in]      ou32_OldDependentValue    Old dependent value
   \param[in]      ou32_UseCaseIndex         Dependent value of previous channel
   \param[in]      opc_Domain                Domain

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeHalcChannelWidget::m_SetDependentValuesOfNewUseCase(sint32 os32_Return, const std::vector<C_OSCHalcDefChannelUseCase>
                                                                  & orc_DefaultUseCases,
                                                                  const std::vector<uint32> & orc_NewDependentValues,
                                                                  const uint32 ou32_OldDependentValue,
                                                                  const uint32 ou32_UseCaseIndex,
                                                                  const C_OSCHalcConfigDomain * const opc_Domain) const
{
   for (uint32 u32_NewCounter = 0U; u32_NewCounter < orc_NewDependentValues.size(); u32_NewCounter++)
   {
      if ((ou32_OldDependentValue == orc_NewDependentValues.at(u32_NewCounter)) || (ou32_OldDependentValue == 0U))
      {
         // update data
         os32_Return =
            C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigUseCase(this->mu32_NodeIndex,
                                                                               this->mu32_DomainIndex,
                                                                               orc_NewDependentValues
                                                                               .at(u32_NewCounter),
                                                                               this->mq_UseChannelIndex,
                                                                               ou32_UseCaseIndex);
         this->m_SetLinkedSymbolAndChannel(orc_NewDependentValues, opc_Domain);
      }
      else
      {
         os32_Return = this->m_FindAndSetDefaultUseCase(os32_Return,
                                                        orc_DefaultUseCases,
                                                        ou32_OldDependentValue);
      }
   }
   return os32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  If an linked use case is set, set a linked symbol and the linked channel

   \param[in]  orc_DependentValues  Dependent values
   \param[in]  opc_Domain           Domain
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_SetLinkedSymbolAndChannel(const std::vector<uint32> & orc_DependentValues,
                                                           const C_OSCHalcConfigDomain * const opc_Domain) const
{
   const C_OSCHalcConfigChannel * pc_LinkedChannel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                      orc_DependentValues.at(0),
                                                                      this->mq_UseChannelIndex);

   if (opc_Domain != NULL)
   {
      this->mpc_Ui->pc_LabLinkedChannel->setText(QString("%1 (%2)").arg(pc_LinkedChannel->c_Name.c_str(),
                                                                        opc_Domain->c_Channels[
                                                                           orc_DependentValues.at(0)].c_Name.c_str()));
   }
   this->mpc_Ui->pc_LabLinkedChannel->setVisible(true);
   this->mpc_Ui->pc_PubLinkedIcon->setVisible(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel selected click.

   Load channel data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_LoadChannelData(void)
{
   // get domain
   const C_OSCHalcConfigDomain * pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(this->mu32_NodeIndex, this->mu32_DomainIndex);

   if (pc_Domain != NULL)
   {
      // work in progress bitmask
      this->mpc_Ui->pc_CbxBitfields->RemoveAllItems();
      this->mpc_Ui->pc_CbxBitmasks->RemoveAllItems();
      for (uint32 u32_Counter = 0; u32_Counter < pc_Domain->c_DomainConfig.c_Parameters.size(); u32_Counter++)
      {
         for (uint32 u32_Bitmasks = 0;
              u32_Bitmasks < pc_Domain->c_DomainConfig.c_Parameters.at(u32_Counter).c_Value.GetBitmaskItems().size();
              u32_Bitmasks++)
         {
            if (u32_Counter == 2)
            {
               this->mpc_Ui->pc_CbxBitfields->AddItem(pc_Domain->c_DomainConfig.c_Parameters.at(u32_Counter).c_Value
                                                      .GetBitmaskItems().at(u32_Bitmasks).c_Display.c_str());
            }
            else if (u32_Counter == 3)
            {
               this->mpc_Ui->pc_CbxBitmasks->AddItem(pc_Domain->c_DomainConfig.c_Parameters.at(u32_Counter).c_Value
                                                     .GetBitmaskItems().at(u32_Bitmasks).c_Display.c_str());
            }
            else
            {
               this->mpc_Ui->pc_CbxBitfields->SetDisplayText("Nothing to Set");
               this->mpc_Ui->pc_CbxBitmasks->SetDisplayText("Nothing to Set");
            }
         }
      }

      const C_OSCHalcConfigChannel * pc_Channel;

      // handle domain without channels
      this->mq_UseChannelIndex = !(pc_Domain->c_Channels.empty());

      // get channel
      pc_Channel =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(this->mu32_NodeIndex, this->mu32_DomainIndex,
                                                                         this->mu32_ChannelIndex,
                                                                         this->mq_UseChannelIndex);

      if (pc_Channel != NULL)
      {
         bool q_IsLinked = false;
         uint32 u32_UseCaseCounter = 0; // for counting all channels and knowing indices of available ones
         sintn sn_SelectedUseCaseCbxIndex = -1;

         // disconnect to avoid data handling update while loading
         this->m_ConnectWidgets(false);

         // adapt GUI elements
         this->mpc_Ui->pc_LeName->setText(pc_Channel->c_Name.c_str());
         this->mpc_Ui->pc_LeName->setReadOnly(!this->mq_UseChannelIndex); // disable name editing in domain case
         this->mpc_Ui->pc_TedComment->setText(pc_Channel->c_Comment.c_str());
         this->mpc_Ui->pc_ChxSafety->setChecked(pc_Channel->q_SafetyRelevant);

         // handle current channel name
         if (this->mu32_ChannelIndex < pc_Domain->c_Channels.size())
         {
            this->mpc_Ui->pc_LabChannelCurrent->setText(pc_Domain->c_Channels[this->mu32_ChannelIndex].c_Name.c_str());
         }
         else
         {
            this->mpc_Ui->pc_LabChannelCurrent->setText("-"); // TODO design concept missing in domain case
         }

         // combobox use case
         this->mpc_Ui->pc_CbxUseCase->clear();
         this->mc_CbxUseCaseIndices.clear();

         // use case only matters in channel case // TODO design concept missing in domain case
         if (mq_UseChannelIndex == true)
         {
            for (std::vector<C_OSCHalcDefChannelUseCase>::const_iterator c_ItUseCases =
                    pc_Domain->c_ChannelUseCases.begin();
                 c_ItUseCases != pc_Domain->c_ChannelUseCases.end(); ++c_ItUseCases)
            {
               const C_OSCHalcDefChannelUseCase & rc_CurrentUseCase = *c_ItUseCases;

               // only add available use cases to combobox
               for (std::vector<C_OSCHalcDefChannelAvailability>::const_iterator c_ItCurrentAvail =
                       rc_CurrentUseCase.c_Availability.begin();
                    c_ItCurrentAvail != rc_CurrentUseCase.c_Availability.end(); ++c_ItCurrentAvail)
               {
                  const C_OSCHalcDefChannelAvailability & rc_CurrentAvail = *c_ItCurrentAvail;

                  if (rc_CurrentAvail.u32_ValueIndex == this->mu32_ChannelIndex)
                  {
                     // add to combobox
                     this->mpc_Ui->pc_CbxUseCase->addItem(rc_CurrentUseCase.c_Display.c_str());

                     // remember indices
                     this->mc_CbxUseCaseIndices.push_back(static_cast<uint32>(rc_CurrentUseCase.c_Value.GetValueU8()));
                     tgl_assert(rc_CurrentUseCase.c_Value.GetType() == C_OSCNodeDataPoolContent::eUINT8);

                     // remember current item index for combobox
                     if (pc_Channel->u32_UseCaseIndex == static_cast<uint32>(rc_CurrentUseCase.c_Value.GetValueU8()))
                     {
                        this->mu32_Value = static_cast<uint32>(rc_CurrentUseCase.c_Value.GetValueU8());
                        sn_SelectedUseCaseCbxIndex = this->mpc_Ui->pc_CbxUseCase->count() - 1;

                        if (!rc_CurrentAvail.c_DependentValues.empty())
                        {
                           this->m_SetLinkedSymbolAndChannel(rc_CurrentAvail.c_DependentValues, pc_Domain);
                           q_IsLinked = true;
                        }
                     }
                     break;
                  }
               }
               u32_UseCaseCounter++;
            }
         }

         // update linked channel visualization
         this->mpc_Ui->pc_PubLinkedIcon->setVisible(q_IsLinked);
         this->mpc_Ui->pc_LabLinkedChannel->setVisible(q_IsLinked);

         // update configuration tree
         this->mpc_Ui->pc_CbxUseCase->setCurrentIndex(sn_SelectedUseCaseCbxIndex);
         this->mc_Model.SetHalcChannelUseCase(mu32_DomainIndex, mu32_ChannelIndex, pc_Channel->u32_UseCaseIndex,
                                              mq_UseChannelIndex);
         this->mpc_Ui->pc_TreeConfig->expandAll(); // TODO decide if some/all/none tree items shall be expanded on
                                                   // use case change

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
   Q_EMIT (SigUpdateChannel(this->mu32_DomainIndex, this->mu32_ChannelIndex, this->mq_UseChannelIndex));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Connect or disconnect all edit widgets

   \param[in]  oq_Connect  true: connect; false: disconnect
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_ConnectWidgets(const bool oq_Connect)
{
   if (oq_Connect == true)
   {
      connect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::editingFinished,
              this, &C_SdNdeHalcChannelWidget::m_OnNameEdited);
      connect(this->mpc_Ui->pc_TedComment, &C_OgeTedPropertiesComment::textChanged,
              this, &C_SdNdeHalcChannelWidget::m_OnCommentEdited);
      connect(this->mpc_Ui->pc_ChxSafety, &C_OgeChxTristateTransparentToggle::toggled,
              this, &C_SdNdeHalcChannelWidget::m_OnSafetyToggled);
      //lint -save -e929 Cast required to avoid ambiguous signal of Qt interface
      connect(this->mpc_Ui->pc_CbxUseCase, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeHalcChannelWidget::m_OnUseCaseChanged);
      //lint -restore
   }
   else
   {
      disconnect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::editingFinished,
                 this, &C_SdNdeHalcChannelWidget::m_OnNameEdited);
      disconnect(this->mpc_Ui->pc_TedComment, &C_OgeTedPropertiesComment::textChanged,
                 this, &C_SdNdeHalcChannelWidget::m_OnCommentEdited);
      disconnect(this->mpc_Ui->pc_ChxSafety, &C_OgeChxTristateTransparentToggle::toggled,
                 this, &C_SdNdeHalcChannelWidget::m_OnSafetyToggled);
      //lint -save -e929 Cast required to avoid ambiguous signal of Qt interface
      disconnect(this->mpc_Ui->pc_CbxUseCase, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
                 this, &C_SdNdeHalcChannelWidget::m_OnUseCaseChanged);
      //lint -restore
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check name for valid characters and duplicates.

   Slot of textChanged signal of name line edit.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_CheckName(const QString & rc_NewName) const
{
   const stw_scl::C_SCLString c_NewName = rc_NewName.toStdString().c_str();

   //check name
   const bool q_NameIsUnique =
      C_PuiSdHandler::h_GetInstance()->CheckHALCChannelNameAvailable(mu32_NodeIndex, mu32_DomainIndex,
                                                                     c_NewName, &mu32_ChannelIndex);
   const bool q_NameIsValid = C_OSCUtils::h_CheckValidCName(c_NewName);

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LeName, "Valid", (q_NameIsUnique && q_NameIsValid));

   if ((q_NameIsUnique == true) && (q_NameIsValid == true))
   {
      this->mpc_Ui->pc_LeName->SetToolTipInformation("", "", C_NagToolTip::eDEFAULT);
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
