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
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

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
   mu32_ChannelIndex(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // remove strings
   this->mpc_Ui->pc_LabChannelCurrent->setText("");
   this->mpc_Ui->pc_ChxSafety->setText("");

   // connects
   this->m_ConnectWidgets(true);
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
   this->mpc_Ui->pc_ChxActive->setText(C_GtGetText::h_GetText("Active"));
   this->mpc_Ui->pc_LabName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabChannelTitle->setText(C_GtGetText::h_GetText("Channel"));
   this->mpc_Ui->pc_LabComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_TedComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here..."));
   this->mpc_Ui->pc_LabSafety->setText(C_GtGetText::h_GetText("Safety Relevant"));
   this->mpc_Ui->pc_LabShareWith->setText(C_GtGetText::h_GetText("Share configuration with: "));

   this->mpc_Ui->pc_LabDbRelation->setText(C_GtGetText::h_GetText("Data Block Relation"));
   this->mpc_Ui->pc_LabOwnerDb->setText(C_GtGetText::h_GetText("Owner Data Block"));
   this->mpc_Ui->pc_LabOwnerDb->setText(C_GtGetText::h_GetText("Owner Data Block"));

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
   this->mpc_Ui->pc_TreeConfig->SetNode(this->mu32_NodeIndex);

   this->m_LoadNodeData();
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
/*! \brief  Slot of checkbox "active".

   \param[in]  oq_Checked  true: is checked, false: is not checked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnActiveToggled(const bool oq_Checked) const
{
   // get channel
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if (pc_Channel != NULL)
   {
      sint32 s32_Return;

      // update data
      s32_Return = C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigActive(mu32_NodeIndex, mu32_DomainIndex,
                                                                                     mu32_ChannelIndex, oq_Checked);

      tgl_assert(s32_Return == C_NO_ERR);

      // no update of any other GUI element necessary here
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of editing finished of channel name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnNameEdited(void)
{
   // get channel
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if (pc_Channel != NULL)
   {
      sint32 s32_Return;
      const stw_scl::C_SCLString c_NewName = this->mpc_Ui->pc_LeName->text().toStdString().c_str();

      // update data
      s32_Return =  C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigName(mu32_NodeIndex, mu32_DomainIndex,
                                                                                    mu32_ChannelIndex, c_NewName);
      tgl_assert(s32_Return == C_NO_ERR);

      // inform about update
      this->m_EmitUpdateSignal();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of comment edited.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnCommentEdited(void)
{
   // get channel
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if (pc_Channel != NULL)
   {
      sint32 s32_Return;
      const stw_scl::C_SCLString c_NewComment = this->mpc_Ui->pc_TedComment->toPlainText().toStdString().c_str();

      // update data
      s32_Return = C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigComment(mu32_NodeIndex, mu32_DomainIndex,
                                                                                      mu32_ChannelIndex, c_NewComment);

      tgl_assert(s32_Return == C_NO_ERR);

      // inform about update
      this->m_EmitUpdateSignal();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of safety checkbox toggle.

   \param[in]  oq_Checked  true: safety relevant on
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnSafetyToggled(const bool oq_Checked) const
{
   // get channel
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if (pc_Channel != NULL)
   {
      sint32 s32_Return;

      // update data
      s32_Return = C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigSafety(mu32_NodeIndex, mu32_DomainIndex,
                                                                                     mu32_ChannelIndex, oq_Checked);
      tgl_assert(s32_Return == C_NO_ERR);

      // no update of any other GUI element necessary here
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of configuration sharing changed.

   \param[in]  os32_NewIndex  New index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnShareConfigChanged(const sint32 os32_NewIndex) const
{
   Q_UNUSED(os32_NewIndex)
   // TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for use case combo box index change.

   \param[in]  os32_NewIndex  New combobox index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnUseCaseChanged(const sint32 os32_NewIndex) const
{
   // get channel
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if (pc_Channel != NULL)
   {
      sint32 s32_Return = C_NO_ERR;

      if ((os32_NewIndex > 0) && ((os32_NewIndex - 1) < static_cast<sint32>(this->mc_CbxUseCaseIndices.size())))
      {
         sint32 s32_CbxIndicesIndex = os32_NewIndex - 1; // -1 because of "not assigned" case
         uint32 u32_UseCaseIndex = this->mc_CbxUseCaseIndices[s32_CbxIndicesIndex];

         // update data
         s32_Return =
            C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigUseCase(mu32_NodeIndex, mu32_DomainIndex,
                                                                               mu32_ChannelIndex, true,
                                                                               u32_UseCaseIndex);

         // update configuration tree
         this->mpc_Ui->pc_TreeConfig->SetHalcChannelUseCase(this->mu32_DomainIndex, this->mu32_ChannelIndex,
                                                            u32_UseCaseIndex);
      }
      else
      {
         // case: "not assigned" is selected or index is -1

         // update data
         s32_Return =
            C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigUseCase(mu32_NodeIndex, mu32_DomainIndex,
                                                                               mu32_ChannelIndex, false,
                                                                               0 /*irrelevant*/);

         // update configuration tree: show nothing
         this->mpc_Ui->pc_TreeConfig->Clear();
      }

      tgl_assert(s32_Return == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of owner Data Block selection change

   \param[in]  os32_NewIndex  New combobox index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_OnOwnerDbChange(const sint32 os32_NewIndex) const
{
   // get channel
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if (pc_Channel != NULL)
   {
      sint32 s32_Return = C_NO_ERR;

      if (os32_NewIndex > 0)
      {
         s32_Return =
            C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigDatablock(mu32_NodeIndex, mu32_DomainIndex,
                                                                                 mu32_ChannelIndex, true,
                                                                                 static_cast<uint32>(os32_NewIndex));
      }
      else
      {
         s32_Return =
            C_PuiSdHandler::h_GetInstance()->SetHALCDomainChannelConfigDatablock(mu32_NodeIndex, mu32_DomainIndex,
                                                                                 mu32_ChannelIndex, false,
                                                                                 0 /*irrelevant*/);
      }

      tgl_assert(s32_Return == C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel selected click.

   Load channel data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_LoadChannelData(void)
{
   // get domain and channel
   const C_OSCHalcConfigDomain * pc_Domain =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(this->mu32_NodeIndex,
                                                                    this->mu32_DomainIndex);
   const C_OSCHalcConfigChannel * pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(mu32_NodeIndex, mu32_DomainIndex,
                                                                      mu32_ChannelIndex);

   if ((pc_Domain != NULL) && (pc_Channel != NULL))
   {
      uint32 u32_UseCaseCounter = 0; // for counting all channels and knowing indices of available ones
      sintn sn_SelectedUseCaseCbxIndex = -1;

      // disconnect to avoid data handling update while loading
      this->m_ConnectWidgets(false);

      // adapt GUI elements
      this->mpc_Ui->pc_ChxActive->setChecked(pc_Channel->q_Active);
      this->mpc_Ui->pc_LeName->setText(pc_Channel->c_Name.c_str());
      this->mpc_Ui->pc_TedComment->setText(pc_Channel->c_Comment.c_str());
      this->mpc_Ui->pc_ChxSafety->setChecked(pc_Channel->q_SafetyRelevant);
      this->mpc_Ui->pc_LabChannelCurrent->setText(pc_Domain->c_Channels[this->mu32_ChannelIndex].c_str());

      // combobox owner Data Block
      if ((pc_Channel->q_DatablockSet == true) &&
          (pc_Channel->u32_DatablockIndex < static_cast<uint32>(this->mpc_Ui->pc_CbxOwnerDb->count())))
      {
         this->mpc_Ui->pc_CbxOwnerDb->setCurrentIndex(static_cast<sintn>(pc_Channel->u32_DatablockIndex));
      }
      else
      {
         this->mpc_Ui->pc_CbxOwnerDb->setCurrentIndex(0);
      }

      // combobox use case
      this->mpc_Ui->pc_CbxUseCase->clear();
      this->mc_CbxUseCaseIndices.clear();

      this->mpc_Ui->pc_CbxUseCase->addItem(C_GtGetText::h_GetText("<not assigned>"));
      for (std::vector<C_OSCHalcDefChannelUseCase>::const_iterator c_ItUseCases =
              pc_Domain->c_ChannelUseCases.begin();
           c_ItUseCases != pc_Domain->c_ChannelUseCases.end(); ++c_ItUseCases)
      {
         const C_OSCHalcDefChannelUseCase & rc_CurrentUseCase = *c_ItUseCases;

         // only add available use cases to combobox
         for (std::vector<C_OSCHalcDefChannelAvailability>::const_iterator c_ItAvail =
                 rc_CurrentUseCase.c_Availability.begin();
              c_ItAvail != rc_CurrentUseCase.c_Availability.end(); ++c_ItAvail)
         {
            const C_OSCHalcDefChannelAvailability & rc_CurrentAvail = *c_ItAvail;
            if (rc_CurrentAvail.u32_ValueIndex == this->mu32_ChannelIndex)
            {
               // add to combobox
               this->mpc_Ui->pc_CbxUseCase->addItem(rc_CurrentUseCase.c_Display.c_str());

               // remember indices
               this->mc_CbxUseCaseIndices.push_back(u32_UseCaseCounter);

               // remember current item index for combobox
               if ((pc_Channel->q_UseCaseSet == true) && (pc_Channel->u32_UseCaseIndex == u32_UseCaseCounter))
               {
                  sn_SelectedUseCaseCbxIndex = this->mpc_Ui->pc_CbxUseCase->count() - 1;
               }
               break;
            }
         }

         u32_UseCaseCounter++;
      }

      // update configuration tree
      if (sn_SelectedUseCaseCbxIndex > 0)
      {
         this->mpc_Ui->pc_CbxUseCase->setCurrentIndex(sn_SelectedUseCaseCbxIndex);

         // update configuration tree
         this->mpc_Ui->pc_TreeConfig->SetHalcChannelUseCase(this->mu32_DomainIndex, this->mu32_ChannelIndex,
                                                            pc_Channel->u32_UseCaseIndex);
      }
      else
      {
         this->mpc_Ui->pc_CbxUseCase->setCurrentIndex(0);

         // update configuration tree
         this->mpc_Ui->pc_TreeConfig->Clear();
      }

      // update
      // reconnect
      this->m_ConnectWidgets(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data for complete node.

   Fill UI with node data from system definition.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_LoadNodeData(void) const
{
   // TODO Reload on change of relevant information
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   // Forward information
   this->mpc_Ui->pc_TreeConfig->SetNode(this->mu32_NodeIndex);

   // Fill Data Block combobox
   if (pc_Node != NULL)
   {
      std::vector<C_OSCNodeApplication>::const_iterator c_It;

      // clear configuration
      this->mpc_Ui->pc_CbxOwnerDb->clear();

      // add item for no application case
      this->mpc_Ui->pc_CbxOwnerDb->addItem(C_GtGetText::h_GetText("<not assigned>"));

      // add all data blocks to data block relation section
      for (c_It = pc_Node->c_Applications.begin(); c_It != pc_Node->c_Applications.end(); ++c_It)
      {
         const C_OSCNodeApplication & rc_Application = *c_It;
         this->mpc_Ui->pc_CbxOwnerDb->addItem(rc_Application.c_Name.c_str());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Emit update signal for informing that some data changed.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelWidget::m_EmitUpdateSignal(void)
{
   Q_EMIT (SigUpdateChannel(this->mu32_DomainIndex, this->mu32_DomainIndex));
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
      connect(this->mpc_Ui->pc_ChxActive, &C_OgeChxProperties::toggled,
              this, &C_SdNdeHalcChannelWidget::m_OnActiveToggled);
      connect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::editingFinished,
              this, &C_SdNdeHalcChannelWidget::m_OnNameEdited);
      connect(this->mpc_Ui->pc_TedComment, &C_OgeTedPropertiesComment::textChanged,
              this, &C_SdNdeHalcChannelWidget::m_OnCommentEdited);
      connect(this->mpc_Ui->pc_ChxSafety, &C_OgeChxTristateTransparentToggle::toggled,
              this, &C_SdNdeHalcChannelWidget::m_OnSafetyToggled);
      //lint -save -e929 Cast required to avoid ambiguous signal of Qt interface
      connect(this->mpc_Ui->pc_CbxShareWith, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeHalcChannelWidget::m_OnShareConfigChanged);
      connect(this->mpc_Ui->pc_CbxUseCase, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeHalcChannelWidget::m_OnUseCaseChanged);
      connect(this->mpc_Ui->pc_CbxOwnerDb, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
              this, &C_SdNdeHalcChannelWidget::m_OnOwnerDbChange);
      //lint -restore
   }
   else
   {
      disconnect(this->mpc_Ui->pc_ChxActive, &C_OgeChxProperties::toggled,
                 this, &C_SdNdeHalcChannelWidget::m_OnActiveToggled);
      disconnect(this->mpc_Ui->pc_LeName, &C_OgeLePropertiesName::editingFinished,
                 this, &C_SdNdeHalcChannelWidget::m_OnNameEdited);
      disconnect(this->mpc_Ui->pc_TedComment, &C_OgeTedPropertiesComment::textChanged,
                 this, &C_SdNdeHalcChannelWidget::m_OnCommentEdited);
      disconnect(this->mpc_Ui->pc_ChxSafety, &C_OgeChxTristateTransparentToggle::toggled,
                 this, &C_SdNdeHalcChannelWidget::m_OnSafetyToggled);
      //lint -save -e929 Cast required to avoid ambiguous signal of Qt interface
      disconnect(this->mpc_Ui->pc_CbxShareWith, static_cast<void (QComboBox::*)(
                                                               sintn)>(&QComboBox::currentIndexChanged),
                 this, &C_SdNdeHalcChannelWidget::m_OnShareConfigChanged);
      disconnect(this->mpc_Ui->pc_CbxUseCase, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
                 this, &C_SdNdeHalcChannelWidget::m_OnUseCaseChanged);
      disconnect(this->mpc_Ui->pc_CbxOwnerDb, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
                 this, &C_SdNdeHalcChannelWidget::m_OnOwnerDbChange);
      //lint -restore
   }
}
