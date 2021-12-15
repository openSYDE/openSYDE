//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing hardware configuration.

   Widget for displaying and editing hardware configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeHalcWidget.h"
#include "ui_C_SdNdeHalcWidget.h"

#include "stwerrors.h"
#include "TGLUtils.h"
#include "constants.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_UsHandler.h"
#include "C_Uti.h"
#include "C_PuiProject.h"
#include "C_OgeWiUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCHalcConfigUtil.h"
#include "C_OSCSystemFilerUtil.h"
#include "C_OSCHalcConfigStandaloneFiler.h"
#include "C_SdNdeHalcConfigImportDialog.h"
#include "C_SdNdeHalcDefUpdateDialog.h"

#include "C_OSCHalcDefFiler.h"
#include "C_OSCLoggingHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_errors;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SdNdeHalcWidget::mhc_CONFIG_FILE_TYPE = ".syde_halc_conf";

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
C_SdNdeHalcWidget::C_SdNdeHalcWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcWidget),
   mu32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // remove strings
   this->mpc_Ui->pc_LabFileCurrent->setText("");

   // overview button
   this->mpc_Ui->pc_PubOverview->setCheckable(true);
   this->mpc_Ui->pc_PubOverview->setChecked(true); // start with overview
   this->m_ShowOverview(true);

   // button icons
   this->mpc_Ui->pc_PubImportConfig->InitCustomIcon("://images/system_definition/NodeEdit/halc/HalcImport.svg",
                                                    "://images/system_definition/NodeEdit/halc/HalcImportDisabled.svg");
   this->mpc_Ui->pc_PubExportConfig->InitCustomIcon("://images/system_definition/NodeEdit/halc/HalcExport.svg",
                                                    "://images/system_definition/NodeEdit/halc/HalcExportDisabled.svg");
   // splitter
   this->mpc_Ui->pc_Splitter->SetMargins(0, 0);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 10);

   // panel colors
   this->mpc_Ui->pc_WiHalcDefFilePanel->SetBackgroundColor(11);
   this->mpc_Ui->pc_LabFileTitle->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabFileTitle->SetForegroundColor(3);
   this->mpc_Ui->pc_LabFileTitle->SetFontPixel(16, true);
   this->mpc_Ui->pc_LabFileCurrent->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabFileCurrent->SetForegroundColor(3);
   this->mpc_Ui->pc_LabFileCurrent->SetFontPixel(16);

   // button connects
   connect(this->mpc_Ui->pc_PubImportConfig, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnImportConfigClicked);
   connect(this->mpc_Ui->pc_PubExportConfig, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnExportConfigClicked);
   connect(this->mpc_Ui->pc_PubSelect, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnSelectClicked);
   connect(this->mpc_Ui->pc_PubUpdate, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnUpdateClicked);
   connect(this->mpc_Ui->pc_PubCleanUp, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnCleanUpClicked);
   connect(this->mpc_Ui->pc_PubOverview, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnOverviewClicked);
   connect(this->mpc_Ui->pc_TreeChannels, &C_SdNdeHalcChannelTreeView::SigChannelSelected,
           this, &C_SdNdeHalcWidget::m_OnChannelSelected);
   connect(this->mpc_Ui->pc_TreeChannels, &C_SdNdeHalcChannelTreeView::SigEditName,
           this->mpc_Ui->pc_WiChannelEdit, &C_SdNdeHalcChannelWidget::SetNameFocus);
   connect(this->mpc_Ui->pc_WiOverviewTable, &C_SdNdeHalcOvWidget::SigChannelSelected,
           this->mpc_Ui->pc_TreeChannels, &C_SdNdeHalcChannelTreeView::SelectChannel);
   // C_SdNdeHalcChannelTreeView::SelectChannel also triggers update of channel widget
   connect(this->mpc_Ui->pc_WiChannelEdit, &C_SdNdeHalcChannelWidget::SigUpdateChannel,
           this, &C_SdNdeHalcWidget::m_OnChannelUpdate);
   connect(this->mpc_Ui->pc_WiChannelEdit, &C_SdNdeHalcChannelWidget::SigChannelSelected,
           this, &C_SdNdeHalcWidget::m_OnChannelSelected);
   connect(this->mpc_Ui->pc_WiChannelEdit, &C_SdNdeHalcChannelWidget::SigChannelSelected,
           this->mpc_Ui->pc_TreeChannels, &C_SdNdeHalcChannelTreeView::SelectChannel);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcWidget::~C_SdNdeHalcWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::LoadUserSettings() const
{
   // splitter
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditHalcSplitterX());

   // sub widgets
   this->mpc_Ui->pc_WiOverviewTable->LoadUserSettings();
   this->mpc_Ui->pc_WiChannelEdit->LoadUserSettings();
   this->mpc_Ui->pc_TreeChannels->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::SaveUserSettings() const
{
   // splitter
   const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditHalcSplitterX(c_Sizes.at(0));
   }

   // sub widgets
   this->mpc_Ui->pc_WiOverviewTable->SaveUserSettings();
   this->mpc_Ui->pc_WiChannelEdit->SaveUserSettings();
   this->mpc_Ui->pc_TreeChannels->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::InitStaticNames(void) const
{
   // top section
   this->mpc_Ui->pc_PubImportConfig->setText(C_GtGetText::h_GetText("Import"));
   this->mpc_Ui->pc_PubExportConfig->setText(C_GtGetText::h_GetText("Export"));
   this->mpc_Ui->pc_PubImportConfig->
   SetToolTipInformation(C_GtGetText::h_GetText("Import Configuration"),
                         C_GtGetText::h_GetText("Import hardware configuration from file *.syde_halc_conf. "
                                                "Select the channels whose configuration should be set according "
                                                "to the import source. Imported data must match the current "
                                                "hardware description."));
   this->mpc_Ui->pc_PubExportConfig->
   SetToolTipInformation(C_GtGetText::h_GetText("Export Configuration"),
                         C_GtGetText::h_GetText("Export the current hardware configuration including custom channel "
                                                "names, use case configuration and parameter values. "
                                                "Does not include the hardware description."));

   // panel
   this->mpc_Ui->pc_PubCleanUp->setText(C_GtGetText::h_GetText("Delete"));
   this->mpc_Ui->pc_PubSelect->setText(C_GtGetText::h_GetText("Select"));
   this->mpc_Ui->pc_PubUpdate->setText(C_GtGetText::h_GetText("Update"));
   this->mpc_Ui->pc_LabFileTitle->setText(C_GtGetText::h_GetText("Hardware Description File:"));
   this->mpc_Ui->pc_PubCleanUp->
   SetToolTipInformation(C_GtGetText::h_GetText("Delete Description and Configuration"),
                         C_GtGetText::h_GetText("Delete the current hardware description, configuration and "
                                                "corresponding Datapools."));
   this->mpc_Ui->pc_PubSelect->
   SetToolTipInformation(C_GtGetText::h_GetText("Select Hardware Description"),
                         C_GtGetText::h_GetText("Select hardware description file *.syde_halc_def that describes "
                                                "domains, channels, use cases and parameters."));
   this->mpc_Ui->pc_PubUpdate->
   SetToolTipInformation(C_GtGetText::h_GetText("Update Hardware Description"),
                         C_GtGetText::h_GetText("Update the current hardware description to a new version by "
                                                "selecting a new hardware description file *.syde_halc_def."));
   this->mpc_Ui->pc_LabFileTitle->
   SetToolTipInformation(C_GtGetText::h_GetText("Hardware Description File"),
                         C_GtGetText::h_GetText("The hardware description file *.syde_halc_def describes the "
                                                "node's hardware abstraction layer in words of domains, channels, "
                                                "use cases and parameters. If the current node supports hardware "
                                                "configuration, this file is part of the device's deployment "
                                                "package."));

   // overview
   this->mpc_Ui->pc_PubOverview->setText(C_GtGetText::h_GetText("Channel Overview"));
   this->mpc_Ui->pc_PubOverview->
   SetToolTipInformation(C_GtGetText::h_GetText("Channel Overview"),
                         C_GtGetText::h_GetText("Show overview of all channels/domains."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::SetNode(const uint32 ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;

   // Forward node index
   this->mpc_Ui->pc_TreeChannels->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_WiChannelEdit->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_WiOverviewTable->SetNodeIndex(ou32_NodeIndex);

   // update GUI
   this->m_UpdateDisplayedData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Jumps to the selected HALC channel

   \param[in]  ou32_DomainIndex     Index of HALC domain
   \param[in]  os32_ChannelIndex    Index of channel of the domain (optional: -1 for not used)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::ShowChannel(const stw_types::uint32 ou32_DomainIndex,
                                    const stw_types::sint32 os32_ChannelIndex) const
{
   const bool q_UseChannelIndex = (os32_ChannelIndex >= 0);

   this->mpc_Ui->pc_TreeChannels->SelectChannel(ou32_DomainIndex, static_cast<uint32>(os32_ChannelIndex),
                                                q_UseChannelIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden show event

   Here: Set splitter size.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::showEvent(QShowEvent * const opc_Event)
{
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditHalcSplitterX());
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Enable or disable buttons that require a loaded HALC definition.

   \param[in]  oq_Active   true: enable, false: disable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_DisableButtons(const bool oq_Active) const
{
   this->mpc_Ui->pc_PubImportConfig->setDisabled(oq_Active);
   this->mpc_Ui->pc_PubExportConfig->setDisabled(oq_Active);
   this->mpc_Ui->pc_PubCleanUp->setDisabled(oq_Active);
   this->mpc_Ui->pc_PubUpdate->setDisabled(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for import configuration button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnImportConfigClicked(void)
{
   const QString c_Folder =
      C_SdNdeHalcWidget::mh_GetDialogPath(C_UsHandler::h_GetInstance()->GetLastKnownHalcImportPath());
   const QString c_Filter = static_cast<QString>(C_GtGetText::h_GetText("openSYDE Hardware Configuration File")) +
                            "(*" +
                            mhc_CONFIG_FILE_TYPE + ")";

   const QString c_FileName =
      C_OgeWiUtil::h_GetOpenFileName(this->parentWidget(),
                                     C_GtGetText::h_GetText("openSYDE Hardware Configuration Import"), c_Folder,
                                     c_Filter, mhc_CONFIG_FILE_TYPE);

   if (c_FileName != "")
   {
      sint32 s32_Return;
      QString c_ErrorDetails;
      QPointer<C_OgePopUpDialog> const c_PopUpDialog = new C_OgePopUpDialog(this, this);
      C_SdNdeHalcConfigImportDialog * const pc_ImportDialog =
         new C_SdNdeHalcConfigImportDialog(*c_PopUpDialog, this->mu32_NodeIndex,
                                           c_FileName);

      // remember path for user settings
      C_UsHandler::h_GetInstance()->SetLastKnownHalcImportPath(c_FileName);

      //Resize
      c_PopUpDialog->SetSize(QSize(1000, 820));

      s32_Return = pc_ImportDialog->PrepareDialog(c_ErrorDetails);

      if (s32_Return == C_NO_ERR)
      {
         if (c_PopUpDialog->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            s32_Return = pc_ImportDialog->GetResult(c_ErrorDetails);
         }
      }

      if (s32_Return != C_NO_ERR)
      {
         // Error message box
         C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResult.SetHeading(C_GtGetText::h_GetText("openSYDE Hardware Configuration Import"));
         c_MessageResult.SetDescription(C_GtGetText::h_GetText("Import error occurred."));
         c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_ErrorDetails.toStdString().c_str()));
         c_MessageResult.SetCustomMinHeight(180, 250);
         c_MessageResult.Execute();

         osc_write_log_error("Hardware configuration import", c_ErrorDetails.toStdString().c_str());
      }
      else
      {
         // trigger UI update
         this->m_UpdateNodeData();
         this->m_ShowOverview(true);
      }

      if (c_PopUpDialog != NULL)
      {
         c_PopUpDialog->HideOverlay();
         c_PopUpDialog->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_ImportDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for export configuration button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnExportConfigClicked(void)
{
   const QString c_Folder =
      C_SdNdeHalcWidget::mh_GetDialogPath(C_UsHandler::h_GetInstance()->GetLastKnownHalcExportPath());
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   QString c_DefaultName = "";
   QString c_FileName = "";

   if (pc_Node != NULL)
   {
      c_DefaultName = C_OSCSystemFilerUtil::h_PrepareItemNameForFileName(pc_Node->c_Properties.c_Name.c_str()).c_str();
   }

   c_FileName =
      C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("openSYDE Hardware Configuration Export"), c_Folder,
                                     static_cast<QString>(C_GtGetText::h_GetText(
                                                             "openSYDE Hardware Configuration File")) +
                                     " (*" + mhc_CONFIG_FILE_TYPE + ")", c_DefaultName);

   if (c_FileName != "")
   {
      QFile c_File;
      bool q_Continue = true;
      sint32 s32_Result;

      // remember path for user settings
      C_UsHandler::h_GetInstance()->SetLastKnownHalcExportPath(c_FileName);

      // Remove old file
      c_File.setFileName(c_FileName);
      if (c_File.exists() == true)
      {
         q_Continue = c_File.remove();
      }

      if (q_Continue == true)
      {
         const stw_opensyde_core::C_OSCHalcConfig * const pc_Config =
            stw_opensyde_gui_logic::C_PuiSdHandler::h_GetInstance()->GetHALCConfig(this->mu32_NodeIndex);

         if (pc_Config != NULL)
         {
            C_OSCHalcConfigStandalone c_StandaloneConfig;
            C_OSCHalcConfigUtil::h_GetConfigStandalone(*pc_Config, c_StandaloneConfig);

            s32_Result = C_OSCHalcConfigStandaloneFiler::h_SaveFileStandalone(c_StandaloneConfig,
                                                                              c_FileName.toStdString().c_str());
         }
         else
         {
            s32_Result = C_CONFIG;
         }
      }
      else
      {
         s32_Result = C_RD_WR;
      }

      if (s32_Result != C_NO_ERR)
      {
         C_OgeWiCustomMessage c_MessageResultSave(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_MessageResultSave.SetHeading(C_GtGetText::h_GetText("openSYDE Hardware Configuration Export"));
         c_MessageResultSave.SetDescription(C_GtGetText::h_GetText("Could not save the file."));
         c_MessageResultSave.SetDetails(C_GtGetText::h_GetText("Error code: \n") + QString::number(s32_Result));
         c_MessageResultSave.SetCustomMinHeight(180, 250);
         c_MessageResultSave.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for clean up button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnCleanUpClicked(void) const
{
   C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);

   c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to delete the hardware description?\n"
                                                      "Complete hardware configuration settings and corresponding "
                                                      "HAL Datapools of this node will also be deleted. Associated"
                                                      "Data Blocks will be reset."));
   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Delete Hardware Description"));
   c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
   c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
   c_MessageBox.SetCustomMinHeight(210, 210);
   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eOK)
   {
      this->m_CleanUpHalcDefinition();

      // reset GUI elements
      this->m_UpdateNodeData();

      this->mpc_Ui->pc_TreeChannels->Clear();
      this->m_ShowOverview(true);

      // Run HALC magician now because Datapool number changed
      this->m_RunDatapoolMagician();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of Select button.

   Load a new HALC description. If there already exists a HALC description, first clear it (after user confirmation).
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnSelectClicked(void)
{
   bool q_Load = true;
   bool q_IsClear = false;

   // Does a configuration already exist?
   tgl_assert(C_PuiSdHandler::h_GetInstance()->IsHalcClear(this->mu32_NodeIndex, q_IsClear) == C_NO_ERR);

   if (q_IsClear == false)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Select Hardware Description"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText("There already exists a hardware description. "
                                                         "Do you want to delete the current hardware description and "
                                                         "load a new one?"));
      c_MessageBox.SetDetails(C_GtGetText::h_GetText("Complete hardware configuration settings and corresponding "
                                                     "HAL Datapools of this node will be lost.\n"
                                                     "For updating the hardware description please use the \"Update\" "
                                                     "button."));
      c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete and Load"));
      c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
      c_MessageBox.SetCustomMinHeight(200, 270);
      if (c_MessageBox.Execute() != C_OgeWiCustomMessage::eOK)
      {
         q_Load = false;
      }
   }

   if (q_Load == true)
   {
      uint32 u32_NotHALCDpCount = 0UL;
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
         if (pc_Node != NULL)
         {
            for (uint32 u32_ItDP = 0UL; u32_ItDP < pc_Node->c_DataPools.size(); ++u32_ItDP)
            {
               const C_OSCNodeDataPool & rc_Dp = pc_Node->c_DataPools[u32_ItDP];
               if (rc_Dp.e_Type != C_OSCNodeDataPool::eHALC)
               {
                  ++u32_NotHALCDpCount;
               }
            }
         }
      }
      if (u32_NotHALCDpCount > (C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE - 2UL))
      {
         C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR);

         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Select Hardware Description"));
         c_MessageBox.SetDescription(static_cast<QString>(C_GtGetText::h_GetText(
                                                             "Cannot select hardware description,\n"
                                                             "because HAL Datapools may not be created,\n"
                                                             "as the max Datapool count (%1) would be exceeded.")).arg(
                                        C_OSCNode::hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE));
         c_MessageBox.SetCustomMinHeight(200, 270);
         c_MessageBox.Execute();
      }
      else
      {
         C_OSCHalcConfig c_HalcConfig;
         QString c_Path;
         if (this->m_LoadHalcDefinitionFile(c_HalcConfig, c_Path) == true)
         {
            // clean up if necessary
            if (q_IsClear == false)
            {
               this->m_CleanUpHalcDefinition();
            }

            // set the HALC definition
            tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCConfig(this->mu32_NodeIndex, c_HalcConfig) == C_NO_ERR);

            // update GUI
            this->m_UpdateNodeData();
            this->m_ShowOverview(true);

            // Run HALC magician now because Datapool number changed
            this->m_RunDatapoolMagician();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of update button

   Load a HALC description update.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnUpdateClicked(void)
{
   const C_OSCHalcConfig * const pc_OldConfig = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(this->mu32_NodeIndex);

   tgl_assert((pc_OldConfig != NULL) && (pc_OldConfig->IsClear() == false));
   if ((pc_OldConfig != NULL) && (pc_OldConfig->IsClear() == false))
   {
      C_OSCHalcConfig c_HalcConfig;
      QString c_HalcDefPath;

      if (this->m_LoadHalcDefinitionFile(c_HalcConfig, c_HalcDefPath) == true)
      {
         bool q_Continue = true;

         // Check change of safety mode
         // (the impact on Datapools, Data Blocks and update screen would be massive and therefore we do not allow this)
         if (c_HalcConfig.e_SafetyMode != pc_OldConfig->e_SafetyMode)
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
            c_Message.SetHeading("Update Hardware Description");
            c_Message.SetDescription(C_GtGetText::h_GetText(
                                        "Cannot update hardware description. "
                                        "The safety mode of the selected hardware description "
                                        "differs from the safety mode of the current hardware description."));
            c_Message.SetDetails(C_GtGetText::h_GetText(
                                    "Hardware description update is only possible if the safety mode does not change. "
                                    "For using a hardware description file with changed safety mode, try the features "
                                    "\'Select\' and \'Import\'."));
            c_Message.Execute();
            q_Continue = false;
         }

         // Check content version if update makes sense
         if ((q_Continue == true) && (pc_OldConfig->u32_ContentVersion == c_HalcConfig.u32_ContentVersion))
         {
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);
            c_Message.SetHeading("Update Hardware Description");
            c_Message.SetDescription(
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "The content version '%1' of the selected hardware description "
                                       "equals the current hardware description version.\n"
                                       "Do you want to update the hardware description anyway?")).
               arg(c_HalcConfig.u32_ContentVersion));
            c_Message.SetOKButtonText(C_GtGetText::h_GetText("Update"));
            c_Message.SetNOButtonText(C_GtGetText::h_GetText("Cancel"));
            if (c_Message.Execute() != C_OgeWiCustomMessage::eOK)
            {
               q_Continue = false;
            }
         }

         if (q_Continue == true)
         {
            QString c_ErrorDetails = "";
            QPointer<C_OgePopUpDialog> const c_PopUpDialog = new C_OgePopUpDialog(this, this);
            C_SdNdeHalcDefUpdateDialog * const pc_UpdateDialog =
               new C_SdNdeHalcDefUpdateDialog(*c_PopUpDialog, this->mu32_NodeIndex, c_HalcConfig, c_HalcDefPath);

            //Resize
            c_PopUpDialog->SetSize(QSize(1000, 820));

            if (c_PopUpDialog->exec() == static_cast<sintn>(QDialog::Accepted))
            {
               C_OSCHalcConfig c_UpdatedHalcConfig;
               const sint32 s32_Return = pc_UpdateDialog->GetResult(c_UpdatedHalcConfig, c_ErrorDetails);

               if (s32_Return == C_NO_ERR)
               {
                  tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCConfig(this->mu32_NodeIndex,
                                                                            c_UpdatedHalcConfig) == C_NO_ERR);

                  // update GUI
                  this->m_UpdateNodeData();
                  this->m_ShowOverview(true);

                  // Run HALC magician now because Datapool number changed
                  this->m_RunDatapoolMagician();
               }

               if ((s32_Return != C_NO_ERR) && (s32_Return != C_NOACT))
               {
                  // Report update error
                  C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
                  c_MessageResult.SetHeading(C_GtGetText::h_GetText("Load hardware description"));
                  c_MessageResult.SetDescription(C_GtGetText::h_GetText("Error occured during update of existing "
                                                                        "description."));
                  c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_ErrorDetails.toStdString().c_str()));
                  c_MessageResult.SetCustomMinHeight(180, 250);
                  c_MessageResult.Execute();

                  osc_write_log_error("Load HALC definition file", c_ErrorDetails.toStdString().c_str());
               }
            }

            if (c_PopUpDialog != NULL)
            {
               c_PopUpDialog->HideOverlay();
               c_PopUpDialog->deleteLater();
            }
         } //lint !e429  //no memory leak because of the parent of pc_ImportDialog and the Qt memory management
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Triggering Datapool magician.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_RunDatapoolMagician(void) const
{
   const sint32 s32_Result = C_PuiSdHandler::h_GetInstance()->HALCGenerateDatapools(this->mu32_NodeIndex);

   tgl_assert((s32_Result == C_NO_ERR) || (s32_Result == C_NOACT));

   // make sure all HALC NVM Datapools are assigned if possible (after magician generation!)
   tgl_assert(C_PuiSdHandler::h_GetInstance()->
              AssignAllHalcNvmDataPools(this->mu32_NodeIndex) == stw_errors::C_NO_ERR);

   // Inform Datapool overview about changed existence of HAL Datapools
   Q_EMIT (this->SigHalcDataPoolChanged());
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for overview button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnOverviewClicked(void) const
{
   this->m_ShowOverview(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show or hide overview widget

   Show or hide channel widget inversely.

   \param[in]  oq_Show  true: show overview widget and hide channel widget; false inverse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_ShowOverview(const bool oq_Show) const
{
   this->mpc_Ui->pc_PubOverview->setChecked(oq_Show);
   this->mpc_Ui->pc_WiChannelEdit->setVisible(!oq_Show);
   this->mpc_Ui->pc_WiOverviewTable->setVisible(oq_Show);

   if (oq_Show == true)
   {
      this->mpc_Ui->pc_WiOverviewTable->UpdateData();
      this->mpc_Ui->pc_TreeChannels->selectionModel()->clearSelection();
      this->mpc_Ui->pc_TreeChannels->setCurrentIndex(QModelIndex()); // reset
      this->mpc_Ui->pc_TreeChannels->ResetDelegate();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel selected click.

   Update channel widget and hide overview widget.

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnChannelSelected(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                            const bool oq_UseChannelIndex) const
{
   // channel index only matters in case domain has any channels
   Q_UNUSED(oq_UseChannelIndex)

   this->mpc_Ui->pc_WiChannelEdit->SetChannel(ou32_DomainIndex, ou32_ChannelIndex);
   this->m_ShowOverview(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel update: Update name or comment in channel tree and emit signal for higher widgets

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnChannelUpdate(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                          const bool oq_UseChannelIndex) const
{
   this->mpc_Ui->pc_TreeChannels->UpdateChannelText(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Select and load HALC definition file and do device name check.

   Inform user if something went wrong.

   \param[out]  orc_HalcConfig   Loaded HALC configuration (only valid if returned true)
   \param[out]  orc_HalcDefPath  Path of loaded HALC definition file

   \retval   true   Success
   \retval   false  User canceled or error occured
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeHalcWidget::m_LoadHalcDefinitionFile(C_OSCHalcConfig & orc_HalcConfig, QString & orc_HalcDefPath)
{
   bool q_Return = false;
   const QString c_Folder = mh_GetDialogPath(C_UsHandler::h_GetInstance()->GetLastKnownHalcDefPath());

   orc_HalcDefPath = C_OgeWiUtil::h_GetOpenFileName(this, C_GtGetText::h_GetText("Select hardware description file"),
                                                    c_Folder, "*.syde_halc_def", "");

   if (orc_HalcDefPath.isEmpty() == false)
   {
      // load definition directly into configuration data structure
      const sint32 s32_LoadResult =
         C_OSCHalcDefFiler::h_LoadFile(orc_HalcConfig, orc_HalcDefPath.toStdString().c_str());

      // remember path for user settings
      C_UsHandler::h_GetInstance()->SetLastKnownHalcDefPath(orc_HalcDefPath);

      if (s32_LoadResult == C_NO_ERR)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            // Device type comparison
            if (pc_Node->c_DeviceType == orc_HalcConfig.c_DeviceName)
            {
               q_Return = true;
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
               c_Message.SetHeading(C_GtGetText::h_GetText("Load hardware description"));
               c_Message.SetDescription(C_GtGetText::h_GetText("The selected hardware description is intended for a "
                                                               "different device type. Nothing was loaded."));
               c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText(
                                                            "The selected hardware description is for nodes "
                                                            "of type: %1\n"
                                                            "The current node is of type: %2")).
                                    arg(orc_HalcConfig.c_DeviceName.c_str()).
                                    arg(pc_Node->pc_DeviceDefinition->c_DeviceName.c_str()));
               c_Message.SetCustomMinHeight(200, 250);
               c_Message.Execute();
            }
         }
      }
      else
      {
         // report loading error
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Load hardware description"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Error occured loading hardware description file."));
         c_Message.SetDetails(static_cast<QString>(C_GtGetText::h_GetText("For details see ")) +
                              C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."),  mc_STYLESHEET_GUIDE_COLOR_LINK,
                                               C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()));
         C_OSCLoggingHandler::h_Flush(); // update log file
         c_Message.SetCustomMinHeight(180, 300);
         c_Message.Execute();
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clean up HALC definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_CleanUpHalcDefinition(void) const
{
   // Clear configuration
   tgl_assert(C_PuiSdHandler::h_GetInstance()->ClearHALCConfig(this->mu32_NodeIndex) == C_NO_ERR);

   // Remove HAL Datapools
   tgl_assert(C_PuiSdHandler::h_GetInstance()->HALCRemoveDatapools(this->mu32_NodeIndex) == C_NO_ERR);

   // Reset PSI generation Data Blocks
   tgl_assert(C_PuiSdHandler::h_GetInstance()->HALCResetDataBlocks(this->mu32_NodeIndex) == C_NO_ERR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update node data after change in data handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_UpdateNodeData(void) const
{
   // inform sub widgets
   this->mpc_Ui->pc_TreeChannels->SetNode(this->mu32_NodeIndex);
   this->mpc_Ui->pc_WiChannelEdit->ReloadChannel();
   this->mpc_Ui->pc_WiOverviewTable->UpdateData();

   // update GUI
   this->m_UpdateDisplayedData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update pin count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_UpdatePinCount(void) const
{
   const C_OSCHalcConfig * const pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(this->mu32_NodeIndex);

   if (pc_Config != NULL)
   {
      uint32 u32_PinCount = 0;

      // Update pin counter
      for (uint32 u32_ItDomain = 0; u32_ItDomain < pc_Config->GetDomainSize(); u32_ItDomain++)
      {
         const C_OSCHalcDefDomain * const pc_Domain = pc_Config->GetDomainDefDataConst(u32_ItDomain);
         if (pc_Domain != NULL)
         {
            if (pc_Domain->c_Channels.empty() == true)
            {
               u32_PinCount += 1;
            }
            else
            {
               u32_PinCount += pc_Domain->c_Channels.size();
            }
         }
      }
      if (u32_PinCount > 0)
      {
         this->mpc_Ui->pc_PubOverview->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                       "Channel Overview (%1)")).arg(
                                                  u32_PinCount));
      }
      else
      {
         this->mpc_Ui->pc_PubOverview->setText(C_GtGetText::h_GetText("Channel Overview"));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update displayed data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_UpdateDisplayedData(void) const
{
   const C_OSCHalcConfig * const pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(this->mu32_NodeIndex);

   if (pc_Config != NULL)
   {
      // Update button states
      this->m_DisableButtons(pc_Config->IsClear());

      // update displayed file name
      if (pc_Config->IsClear())
      {
         this->mpc_Ui->pc_LabFileCurrent->setText(C_GtGetText::h_GetText("<none>"));
      }
      else
      {
         this->mpc_Ui->pc_LabFileCurrent->setText(pc_Config->c_OriginalFileName.c_str());
      }
   }

   // update pin counter of overview button
   this->m_UpdatePinCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get path for file browse dialog

   \param[in]  orc_LastKnownPath    Last known path

   \return
   Existing path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeHalcWidget::mh_GetDialogPath(const QString & orc_LastKnownPath)
{
   QString c_Folder = orc_LastKnownPath;

   // use project folder path if no path is known
   if (orc_LastKnownPath.isEmpty() == true)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   // default to exe if path is empty (i.e. project is not saved yet)
   if (c_Folder.isEmpty() == true)
   {
      c_Folder = C_Uti::h_GetExePath();
   }

   return c_Folder;
}
