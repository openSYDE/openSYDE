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
#include "C_OgeWiUtil.h"

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
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->m_LoadNodeData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for import configuration button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnImportConfigClicked(void) const
{
   // TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for export configuration button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnExportConfigClicked(void) const
{
   // TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for settings button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnSettingsClicked(void) const
{
   // TODO
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for clean up button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnCleanUpClicked(void) const
{
   // TODO
   // reset GUI elements (TODO: only clear visible widgets?)
   this->mpc_Ui->pc_TreeChannels->Clear();
   this->mpc_Ui->pc_PubOverview->setChecked(true); // also triggers update of visible widgets
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for load button click.

   Load HALC definition description file.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnLoadClicked(void)
{
   // TODO user settings last known halc path
   const QString c_HalcDefPath = C_OgeWiUtil::h_GetSaveFileName(this,
                                                                C_GtGetText::h_GetText("Select HALC description file"),
                                                                "", "*.xml", "", QFileDialog::DontConfirmOverwrite);

   if (c_HalcDefPath.isEmpty() == false)
   {
      // TODO: check what a second load does and eventually discuss what should happen

      // load definition directly into configuration data structure
      C_OSCHalcConfig c_HalcConfig;
      const sint32 s32_LoadResult = C_OSCHalcDefFiler::h_LoadFile(c_HalcConfig, c_HalcDefPath.toStdString().c_str());

      tgl_assert(C_PuiSdHandler::h_GetInstance()->SetHALCConfig(this->mu32_NodeIndex, c_HalcConfig) == C_NO_ERR);

      if (s32_LoadResult == C_NO_ERR)
      {
         // TODO: device type comparison

         // Forward Data to overview tree TODO move set node to set node and call update here
         this->mpc_Ui->pc_TreeChannels->SetNode(this->mu32_NodeIndex);

         // Channel data is updated on channel select
      }
      else
      {
         // TODO error handling & success message
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetType(C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading("Load HALC definition file");
         c_Message.SetDescription(C_GtGetText::h_GetText("Error occured loading HALC file."));
         c_Message.SetDetails(QString(C_GtGetText::h_GetText("For details see ")) +
                              C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."),  mc_STYLESHEET_GUIDE_COLOR_LINK,
                                               C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str()));
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
   // TODO check message boxes sizes when finished
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
   }
   else if (s32_Result == C_RANGE)
   {
      c_Message.SetType(C_OgeWiCustomMessage::eERROR);
      c_Message.SetDescription(C_GtGetText::h_GetText("Node index invalid."));
      c_Message.SetCustomMinHeight(180, 180);
   }
   else if (s32_Result == C_BUSY)
   {
      c_Message.SetType(C_OgeWiCustomMessage::eERROR);
      c_Message.SetDescription(C_GtGetText::h_GetText(
                                  "No programmable application found to assign the generated datapools to."));
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
   if ((orc_Index.isValid() == true) && (orc_Index.parent().isValid() == true))
   {
      const uint32 u32_DomainIndex = static_cast<uint32>(orc_Index.parent().row());
      const uint32 u32_ChannelIndex = static_cast<uint32>(orc_Index.row());

      this->mpc_Ui->pc_WiChannelEdit->SetChannel(u32_DomainIndex, u32_ChannelIndex);
      this->mpc_Ui->pc_PubOverview->setChecked(false); // also triggers update of visible widgets
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel update: Update name or comment in channel tree

   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnChannelUpdate(const uint32 ou32_DomainIndex, const uint32 ou32_ChannelIndex) const
{
   // TODO maybe it would be better to hold the tree model here, to avoid having to forward everything...

   this->mpc_Ui->pc_TreeChannels->UpdateChannelText(ou32_DomainIndex, ou32_ChannelIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data for complete node.

   Fill UI with node data from system definition.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_LoadNodeData(void) const
{
   // TODO Reload on change of relevant information

   // Forward node index
   this->mpc_Ui->pc_TreeChannels->SetNode(this->mu32_NodeIndex);
   this->mpc_Ui->pc_WiChannelEdit->SetNode(this->mu32_NodeIndex);

   // TODO select last known channel
}
