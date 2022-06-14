//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing CANopen Manager

   Widget for displaying and editing CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_GtGetText.h"
#include "C_UsHandler.h"
#include "C_SdNdeCoWidget.h"
#include "ui_C_SdNdeCoWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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
C_SdNdeCoWidget::C_SdNdeCoWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoWidget),
   mu32_NodeIndex(0UL)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // config the ui
   this->mpc_Ui->pc_PubOverview->setCheckable(true);
   this->mpc_Ui->pc_PubOverview->setChecked(true); // start with overview
   this->mpc_Ui->pc_PubOverview->SetForegroundColor(3);
   this->mpc_Ui->pc_PubOverview->SetBackgroundColor(11);
   this->mpc_Ui->pc_PubOverview->SetCheckedBackgroundColor(10);
   this->mpc_Ui->pc_PubOverview->SetFontPixel(13, true, false);

   this->mpc_Ui->pc_OverviewWidget->setVisible(true);
   this->mpc_Ui->pc_IntfSettingsWidget->setVisible(false);
   this->mpc_Ui->pc_DeviceWidget->setVisible(false);

   // splitter
   this->mpc_Ui->pc_Splitter->SetMargins(0, 0);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 10);

   // show overview data
   this->mpc_Ui->pc_OverviewWidget->UpdateData();

   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigDeviceSelected, this,
           &C_SdNdeCoWidget::m_OnDeviceSelected);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigErrorChange, this,
           &C_SdNdeCoWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigInterfaceSelected, this,
           &C_SdNdeCoWidget::m_OnInterfaceSelected);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigCommDatapoolsChanged, this,
           &C_SdNdeCoWidget::SigCommDatapoolsChanged);
   connect(this->mpc_Ui->pc_PubOverview, &QPushButton::clicked, this, &C_SdNdeCoWidget::m_OnOverviewClicked);
   connect(this->mpc_Ui->pc_OverviewWidget, &C_SdNdeCoOverviewWidget::SigInterfaceSelected,
           this->mpc_Ui->pc_CoConfigTree,
           &C_SdNdeCoConfigTreeView::SetInterfaceSelected);
   connect(this->mpc_Ui->pc_OverviewWidget, &C_SdNdeCoOverviewWidget::SigDeviceSelected, this->mpc_Ui->pc_CoConfigTree,
           &C_SdNdeCoConfigTreeView::SetDeviceSelected);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoWidget::~C_SdNdeCoWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::LoadUserSettings(void) const
{
   // splitter
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditCoManagerSplitterX());

   // TODO: Load user settings
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::SaveUserSettings(void) const
{
   // splitter
   const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditCoManagerSplitterX(c_Sizes.at(0));
   }

   // TODO: Save user settings
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_PubOverview->setText(C_GtGetText::h_GetText("CANopen Manager Configuration"));
   this->mpc_Ui->pc_PubOverview->
   SetToolTipInformation(C_GtGetText::h_GetText("CANopen Manager Configuration"),
                         C_GtGetText::h_GetText("Show overview of CANopen Manager Configuration of current node."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set new node index.

   \param[in]  ou32_NodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::SetNode(const uint32 ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;
   this->mpc_Ui->pc_CoConfigTree->SetNodeId(ou32_NodeIndex);
   this->mpc_Ui->pc_OverviewWidget->SetNodeIndex(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden show event

   Here: Set splitter size.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::showEvent(QShowEvent * const opc_Event)
{
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditCoManagerSplitterX());
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for overview button click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_OnOverviewClicked(void) const
{
   this->mpc_Ui->pc_PubOverview->setChecked(true);

   this->mpc_Ui->pc_DeviceWidget->setVisible(false);
   this->mpc_Ui->pc_IntfSettingsWidget->setVisible(false);
   this->mpc_Ui->pc_OverviewWidget->setVisible(true);

   this->mpc_Ui->pc_OverviewWidget->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle interface selected

   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_OnInterfaceSelected(const uint8 ou8_InterfaceNumber)
{
   Q_UNUSED(ou8_InterfaceNumber)

   this->mpc_Ui->pc_OverviewWidget->setVisible(false);
   this->mpc_Ui->pc_PubOverview->setChecked(false);
   //set Node Index and Interface ID
   this->mpc_Ui->pc_IntfSettingsWidget->SetNodeIndexAndInterfaceId(this->mu32_NodeIndex, ou8_InterfaceNumber);
   this->mpc_Ui->pc_IntfSettingsWidget->setVisible(true);

   this->mpc_Ui->pc_DeviceWidget->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle device selected

   \param[in]  ou8_InterfaceNumber  Interface number
   \param[in]  orc_NodeId           Node ID
   \param[in]  ou32_UseCaseIndex    Use case index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_OnDeviceSelected(const uint8 ou8_InterfaceNumber,
                                         const stw_opensyde_core::C_OSCCanInterfaceId & orc_NodeId,
                                         const uint32 ou32_UseCaseIndex)
{
   Q_UNUSED(ou8_InterfaceNumber)
   Q_UNUSED(orc_NodeId)
   Q_UNUSED(ou32_UseCaseIndex)

   this->mpc_Ui->pc_OverviewWidget->setVisible(false);
   this->mpc_Ui->pc_PubOverview->setChecked(false);
   this->mpc_Ui->pc_IntfSettingsWidget->setVisible(false);
   this->mpc_Ui->pc_DeviceWidget->setVisible(true);

   this->mpc_Ui->pc_DeviceWidget->SetDeviceUseCase(this->mu32_NodeIndex, ou8_InterfaceNumber, orc_NodeId,
                                                   ou32_UseCaseIndex);
}
