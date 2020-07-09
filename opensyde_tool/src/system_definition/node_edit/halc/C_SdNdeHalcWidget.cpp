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
#include "C_Uti.h"
#include "C_PuiProject.h"
#include "C_OgeWiUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCHalcConfigUtil.h"
#include "C_OSCHalcConfigStandaloneFiler.h"
#include "C_SdNdeHalcConfigImportDialog.h"
#include "C_SdNdeHalcDefUpdateDialog.h"
#include "C_SdNdeHalcSettingsDialog.h"

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
   // TODO start with last known channel from user settings
   // TODO do not allow to set unchecked when clicking on checked overview button ("one way toggle")
   this->m_OnOverviewToggled(true);

   // disable buttons which require a loaded HALC definition
   this->m_DisableButtons(true);

   // button connects
   connect(this->mpc_Ui->pc_PubImportConfig, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnImportConfigClicked);
   connect(this->mpc_Ui->pc_PubExportConfig, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnExportConfigClicked);
   connect(this->mpc_Ui->pc_PubSettings, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnSettingsClicked);
   connect(this->mpc_Ui->pc_PubCleanUp, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnCleanUpClicked);
   connect(this->mpc_Ui->pc_PubLoad, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnLoadClicked);
   connect(this->mpc_Ui->pc_PubViewDetails, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnViewDetailsClicked);
   connect(this->mpc_Ui->pc_PubMagic, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnMagicRequested);
   connect(this->mpc_Ui->pc_PubOverview, &QPushButton::toggled, this, &C_SdNdeHalcWidget::m_OnOverviewToggled);
   connect(this->mpc_Ui->pc_TreeChannels->selectionModel(), &QItemSelectionModel::currentRowChanged,
           this, &C_SdNdeHalcWidget::m_OnChannelSelected);
   connect(this->mpc_Ui->pc_WiChannelEdit, &C_SdNdeHalcChannelWidget::SigUpdateChannel,
           this, &C_SdNdeHalcWidget::m_OnChannelUpdate);
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
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::InitStaticNames(void) const
{
   // top sections
   this->mpc_Ui->pc_PubImportConfig->setText(C_GtGetText::h_GetText("Import Configuration"));
   this->mpc_Ui->pc_PubExportConfig->setText(C_GtGetText::h_GetText("Export Configuration"));
   this->mpc_Ui->pc_PubSettings->setText(C_GtGetText::h_GetText("Settings"));

   this->mpc_Ui->pc_PubCleanUp->setText(C_GtGetText::h_GetText("Clean Up"));
   this->mpc_Ui->pc_PubLoad->setText(C_GtGetText::h_GetText("Load"));
   this->mpc_Ui->pc_PubViewDetails->setText(C_GtGetText::h_GetText("View Details"));
   this->mpc_Ui->pc_LabFileTitle->setText(C_GtGetText::h_GetText("Hardware Description File"));

   // overview section
   this->mpc_Ui->pc_PubOverview->setText(C_GtGetText::h_GetText("Overview"));

   // TODO tool tips
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::SetNode(const uint32 ou32_NodeIndex)
{
   const C_OSCHalcConfig * const pc_Config = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(ou32_NodeIndex);

   this->mu32_NodeIndex = ou32_NodeIndex;

   // Forward node index
   this->mpc_Ui->pc_TreeChannels->SetNode(ou32_NodeIndex);
   this->mpc_Ui->pc_WiChannelEdit->SetNode(ou32_NodeIndex);

   // TODO select last known channel

   // Update buttons
   if (pc_Config != NULL)
   {
      this->m_DisableButtons(pc_Config->IsClear());
   }
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
   this->mpc_Ui->pc_PubSettings->setDisabled(oq_Active);
   this->mpc_Ui->pc_PubCleanUp->setDisabled(oq_Active);
   this->mpc_Ui->pc_PubViewDetails->setDisabled(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for import configuration button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnImportConfigClicked(void)
{
   const QString c_Folder = C_SdNdeHalcWidget::mh_GetDialogPath();
   const QString c_Filter = QString(C_GtGetText::h_GetText("openSYDE Hardware Configuration File")) + "(*" +
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
      }

      if (c_PopUpDialog != NULL)
      {
         c_PopUpDialog->HideOverlay();
      }

      //lint -e{429}  no memory leak because of the parent of pc_ImportDialog and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for export configuration button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnExportConfigClicked(void)
{
   const QString c_Folder = C_SdNdeHalcWidget::mh_GetDialogPath();
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   QString c_DefaultName = "";
   QString c_FileName = "";

   if (pc_Node != NULL)
   {
      c_DefaultName = pc_Node->c_Properties.c_Name.c_str();
   }

   c_FileName =
      C_OgeWiUtil::h_GetSaveFileName(this, C_GtGetText::h_GetText("openSYDE Hardware Configuration Export"), c_Folder,
                                     QString(C_GtGetText::h_GetText("openSYDE Hardware Configuration File")) +
                                     " (*" + mhc_CONFIG_FILE_TYPE + ")", c_DefaultName);

   if (c_FileName != "")
   {
      // Check if file exists already
      QFile c_File;
      bool q_Continue = true;
      sint32 s32_Result;

      // Remove old file
      c_File.setFileName(c_FileName);
      if (c_File.exists() == true)
      {
         q_Continue = c_File.remove();
      }

      if (q_Continue == true)
      {
         const stw_opensyde_core::C_OSCHalcConfig * pc_Config =
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
/*! \brief  Slot for settings button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnSettingsClicked(void)
{
   QPointer<C_OgePopUpDialog> const c_PopUpDialog = new C_OgePopUpDialog(this, this);
   C_SdNdeHalcSettingsDialog * const pc_UpdateDialog =
      new C_SdNdeHalcSettingsDialog(*c_PopUpDialog, this->mu32_NodeIndex);

   Q_UNUSED(pc_UpdateDialog)

   //Resize and execute
   c_PopUpDialog->SetSize(QSize(750, 300));
   c_PopUpDialog->exec();

   if (c_PopUpDialog != NULL)
   {
      c_PopUpDialog->HideOverlay();
   }

   //lint -e{429}  no memory leak because of the parent of pc_UpdateDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for clean up button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnCleanUpClicked(void) const
{
   C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);

   c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to clear the current hardware configuration "
                                                      "and delete all channel settings and corresponding Datapools?"));
   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Clear Configuration"));
   c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Clear"));
   c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
   c_MessageBox.SetCustomMinHeight(180, 180);
   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eOK)
   {
      // Clear configuration
      tgl_assert(C_PuiSdHandler::h_GetInstance()->ClearHALCConfig(this->mu32_NodeIndex) == C_NO_ERR);

      // Remove HALC Datapools
      tgl_assert(C_PuiSdHandler::h_GetInstance()->HALCRemoveDatapools(this->mu32_NodeIndex) == C_NO_ERR);

      // reset GUI elements
      this->m_DisableButtons(true);
      this->mpc_Ui->pc_TreeChannels->Clear();
      this->mpc_Ui->pc_PubOverview->setChecked(true); // also triggers update of visible widgets
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for load button click.

   Load HALC definition description file.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnLoadClicked(void)
{
   // TODO user settings last known halc path
   const QString c_HalcDefPath = C_OgeWiUtil::h_GetOpenFileName(this,
                                                                C_GtGetText::h_GetText("Select HALC description file"),
                                                                "", "*.syde_halc_def", "");

   if (c_HalcDefPath.isEmpty() == false)
   {
      // load definition directly into configuration data structure
      C_OSCHalcConfig c_HalcConfig;
      const sint32 s32_LoadResult = C_OSCHalcDefFiler::h_LoadFile(c_HalcConfig, c_HalcDefPath.toStdString().c_str());

      if (s32_LoadResult == C_NO_ERR)
      {
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            // Device type comparison
            if (pc_Node->c_DeviceType == c_HalcConfig.c_DeviceName)
            {
               const C_OSCHalcConfig * const pc_OldConfig = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(
                  this->mu32_NodeIndex);

               tgl_assert(pc_OldConfig != NULL);
               if (pc_OldConfig != NULL)
               {
                  // Check for a second load
                  if (pc_OldConfig->IsClear() == true)
                  {
                     // Initial load, set the HALC definition
                     tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCConfig(this->mu32_NodeIndex,
                                                                               c_HalcConfig) == C_NO_ERR);
                     // update GUI
                     this->m_DisableButtons(false);
                  }
                  // Check the content version for a change
                  else if (pc_OldConfig->u32_ContentVersion == c_HalcConfig.u32_ContentVersion)
                  {
                     C_OgeWiCustomMessage c_Message(this);
                     c_Message.SetHeading("Load HALC definition file");
                     c_Message.SetDescription(
                        QString(C_GtGetText::h_GetText("The content version %1 of the loaded HALC file matches the "
                                                       "content version of the current HALC definition. "
                                                       "Nothing was loaded.")).arg(c_HalcConfig.u32_ContentVersion));
                     c_Message.SetDetails(C_GtGetText::h_GetText("For loading a HALC definition of the same content "
                                                                 "version clear the current HALC configuration."));
                     c_Message.Execute();
                  }
                  else
                  {
                     // Update necessary
                     sint32 s32_Return = C_NO_ERR;
                     QString c_ErrorDetails = "";
                     QPointer<C_OgePopUpDialog> const c_PopUpDialog = new C_OgePopUpDialog(this, this);
                     C_SdNdeHalcDefUpdateDialog * const pc_UpdateDialog =
                        new C_SdNdeHalcDefUpdateDialog(*c_PopUpDialog, this->mu32_NodeIndex, c_HalcConfig,
                                                       c_HalcDefPath);

                     //Resize
                     c_PopUpDialog->SetSize(QSize(1000, 820));

                     if (c_PopUpDialog->exec() == static_cast<sintn>(QDialog::Accepted))
                     {
                        C_OSCHalcConfig c_UpdatedHalcConfig;
                        s32_Return = pc_UpdateDialog->GetResult(c_UpdatedHalcConfig, c_ErrorDetails);

                        if (s32_Return == C_NO_ERR)
                        {
                           tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCConfig(this->mu32_NodeIndex,
                                                                                     c_UpdatedHalcConfig) == C_NO_ERR);
                        }
                     }

                     if ((s32_Return != C_NO_ERR) &&
                         (s32_Return != C_NOACT))
                     {
                        // Error message box
                        C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
                        c_MessageResult.SetHeading(C_GtGetText::h_GetText("Load HALC definition file"));
                        c_MessageResult.SetDescription(C_GtGetText::h_GetText(
                                                          "Error on updating of existing definition."));
                        c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_ErrorDetails.toStdString().c_str()));
                        c_MessageResult.SetCustomMinHeight(180, 250);
                        c_MessageResult.Execute();

                        osc_write_log_error("Load HALC definition file", c_ErrorDetails.toStdString().c_str());
                     }

                     if (c_PopUpDialog != NULL)
                     {
                        c_PopUpDialog->HideOverlay();
                     }

                     //lint -e{429} no memory leak because of the parent of pc_UpdateDialog and the Qt memory management
                  }

                  // Reload overview tree
                  this->mpc_Ui->pc_TreeChannels->SetNode(this->mu32_NodeIndex);

                  // Channel data is updated on channel select
               }
            }
            else
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
               c_Message.SetHeading("Load HALC definition file");
               c_Message.SetDescription(C_GtGetText::h_GetText("The loaded HALC file is supposed for an other device "
                                                               "type. Nothing was loaded."));
               c_Message.SetDetails(QString(C_GtGetText::h_GetText("The loaded HALC file is supposed for: %1\n"
                                                                   "The current node is: %2")).
                                    arg(c_HalcConfig.c_DeviceName.c_str()).
                                    arg(pc_Node->c_DeviceType.c_str()));
               c_Message.SetCustomMinHeight(200, 250);
               c_Message.Execute();
            }
         }
      }
      else
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading("Load HALC definition file");
         c_Message.SetDescription(C_GtGetText::h_GetText("Error occured loading HALC file."));
         c_Message.SetDetails(QString(C_GtGetText::h_GetText("For details see ")) +
                              C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."),  mc_STYLESHEET_GUIDE_COLOR_LINK,
                                               C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()));

         // update log file:
         C_OSCLoggingHandler::h_Flush();

         c_Message.SetCustomMinHeight(180, 300);
         c_Message.Execute();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for view details button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnViewDetailsClicked(void) const
{
   // TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Button for triggering datapool magician. Development only!
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnMagicRequested(void)
{
   C_OgeWiCustomMessage c_Message(this);

   const sint32 s32_Result = C_PuiSdHandler::h_GetInstance()->HALCGenerateDatapools(this->mu32_NodeIndex);

   c_Message.SetHeading(C_GtGetText::h_GetText("Generating DataPools for HALC"));

   if (s32_Result == C_CONFIG)
   {
      c_Message.SetType(C_OgeWiCustomMessage::eERROR);
      c_Message.SetDescription(C_GtGetText::h_GetText("Configuration Error!"));
      c_Message.SetDetails(QString("<a href=\"file:%1\"><span style=\"color: %2;\">%3</span></a>.").
                           arg(C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()).
                           arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                           arg(C_GtGetText::h_GetText("log file")));
      c_Message.SetCustomMinHeight(180, 300);

      // update log file:
      C_OSCLoggingHandler::h_Flush();
   }
   else if (s32_Result == C_RANGE)
   {
      c_Message.SetType(C_OgeWiCustomMessage::eERROR);
      c_Message.SetDescription(C_GtGetText::h_GetText("Node index invalid."));
      c_Message.SetCustomMinHeight(180, 180);
   }
   else
   {
      c_Message.SetType(C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetDescription(C_GtGetText::h_GetText("Success."));
      c_Message.SetCustomMinHeight(180, 180);
   }
   c_Message.exec();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for overview button click.

   \param[in]  oq_Checked  true: show overview; false: show channel edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnOverviewToggled(const bool oq_Checked) const
{
   this->mpc_Ui->pc_WiChannelEdit->setVisible(!oq_Checked);
   this->mpc_Ui->pc_WiOverviewTable->setVisible(oq_Checked);

   if (oq_Checked == true)
   {
      this->mpc_Ui->pc_TreeChannels->selectionModel()->clearSelection();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel selected click.

   Load channel data.

   \param[in]  orc_Index   model index of selected channel
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnChannelSelected(const QModelIndex & orc_Index) const
{
   if (orc_Index.isValid() == true)
   {
      uint32 u32_DomainIndex;
      uint32 u32_ChannelIndex;
      bool q_ChannelCase;

      C_SdNdeHalcChannelTreeModel::h_GetIndexesFromModelIndex(orc_Index, u32_DomainIndex, u32_ChannelIndex,
                                                              q_ChannelCase);

      // channel index only matters in case domain has any channels
      this->mpc_Ui->pc_WiChannelEdit->SetChannel(u32_DomainIndex, u32_ChannelIndex);
      this->mpc_Ui->pc_PubOverview->setChecked(false); // also triggers update of visible widgets
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel update: Update name or comment in channel tree

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnChannelUpdate(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex,
                                          const bool oq_UseChannelIndex) const
{
   this->mpc_Ui->pc_TreeChannels->UpdateChannelText(ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update node data after change in data handling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_UpdateNodeData(void) const
{
   // reload complete channel tree (easier than reloading only changed channels)
   this->mpc_Ui->pc_TreeChannels->SetNode(this->mu32_NodeIndex);
   this->mpc_Ui->pc_WiChannelEdit->ReloadChannel();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get path for file browse dialog

   \return
   Existing path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeHalcWidget::mh_GetDialogPath(void)
{
   QString c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();

   // default to exe if path is empty (i.e. project is not saved yet)
   if (c_Folder.isEmpty() == true)
   {
      c_Folder = C_Uti::h_GetExePath();
   }

   // TODO BAY: Last path?

   return c_Folder;
}
