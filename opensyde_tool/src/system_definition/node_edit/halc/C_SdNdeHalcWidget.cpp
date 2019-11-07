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

#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
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

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcWidget::C_SdNdeHalcWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcWidget),
   mu32_NodeIndex(0)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // TODO GS finish TODOs of this class

   // remove strings
   this->mpc_Ui->pc_LabFileCurrent->setText("");
   this->mpc_Ui->pc_LabChannelCurrent->setText("");
   this->mpc_Ui->pc_ChxSafety->setText("");

   this->mpc_Ui->pc_PubOverview->setCheckable(true);
   // start with overview
   // TODO start with last known channel from user settings
   this->m_OnOverviewClicked();

   // connects
   connect(this->mpc_Ui->pc_PubImportConfig, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnImportConfigClicked);
   connect(this->mpc_Ui->pc_PubExportConfig, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnExportConfigClicked);
   connect(this->mpc_Ui->pc_PubSettings, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnSettingsClicked);
   connect(this->mpc_Ui->pc_PubCleanUp, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnCleanUpClicked);
   connect(this->mpc_Ui->pc_PubLoad, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnLoadClicked);
   connect(this->mpc_Ui->pc_PubViewDetails, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnViewDetailsClicked);
   connect(this->mpc_Ui->pc_PubOverview, &QPushButton::clicked, this, &C_SdNdeHalcWidget::m_OnOverviewClicked);
   connect(this->mpc_Ui->pc_TreeOverview, &C_SdNdeHalcOverviewTreeView::clicked,
           this, &C_SdNdeHalcWidget::m_OnChannelSelected);

   //lint -e{929} Cast required to avoid ambiguous signal of Qt interface
   connect(this->mpc_Ui->pc_CbxUseCase, static_cast<void (QComboBox::*)(sintn)>(&QComboBox::currentIndexChanged),
           this, &C_SdNdeHalcWidget::m_OnUseCaseChanged);
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
/*! \brief  Set new node index.

   \param[in]       oru32_NodeIndex     Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::SetNode(const uint32 & oru32_NodeIndex)
{
   this->mu32_NodeIndex = oru32_NodeIndex;
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for load button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnLoadClicked(void) const
{
   // TODO
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
/*! \brief  Slot for overview button click.

   \param[in]  oq_ShowOverview   true: show overview; false: show channel edit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnOverviewClicked(void) const
{
   this->mpc_Ui->pc_WiChannelEdit->setVisible(false);
   this->mpc_Ui->pc_WiOverviewTable->setVisible(true);
   this->mpc_Ui->pc_PubOverview->setChecked(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for use case combo box index change.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnUseCaseChanged(void) const
{
   // TODO (update  this->mpc_Ui->pc_TreeConfig)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of channel selected click.

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_OnChannelSelected(void) const
{
   this->mpc_Ui->pc_WiChannelEdit->setVisible(true);
   this->mpc_Ui->pc_WiOverviewTable->setVisible(false);
   this->mpc_Ui->pc_PubOverview->setChecked(false);

   // TODO: load channel data
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load data for complete node.

   Fill UI with node data from system definition.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcWidget::m_LoadNodeData(void) const
{
   // TODO Reload on change of relevant information
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      std::vector<C_OSCNodeApplication>::const_iterator c_It;

      // clear configuration
      this->mpc_Ui->pc_CbxOwnerDb->clear(); // TODO no application case

      // add all data blocks to data block relation section
      for (c_It = pc_Node->c_Applications.begin(); c_It != pc_Node->c_Applications.end(); ++c_It)
      {
         const C_OSCNodeApplication & rc_Application = *c_It;
         this->mpc_Ui->pc_CbxOwnerDb->addItem(rc_Application.c_Name.c_str());
      }
      this->m_OnChannelSelected(); // TODO select correct channel
   }
}
