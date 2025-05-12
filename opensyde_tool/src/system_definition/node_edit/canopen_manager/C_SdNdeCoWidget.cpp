//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing CANopen Manager

   Widget for displaying and editing CANopen Manager

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_UsHandler.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeCoWidget.hpp"
#include "ui_C_SdNdeCoWidget.h"
#include "C_OscNode.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
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
C_SdNdeCoWidget::C_SdNdeCoWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoWidget),
   mu32_NodeIndex(0UL),
   mu8_InterfaceNumber(0)

{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();

   // config the ui
   this->mpc_Ui->pc_PubOverview->setCheckable(true);
   this->mpc_Ui->pc_PubOverview->setChecked(true); // start with overview

   this->mpc_Ui->pc_WiManager->setVisible(true);
   this->mpc_Ui->pc_OverviewWidget->setVisible(true);
   this->mpc_Ui->pc_IntfSettingsWidget->setVisible(false);
   this->mpc_Ui->pc_DeviceWidget->setVisible(false);
   this->mpc_Ui->pc_WiDeviceLink->setVisible(false);

   // splitter
   this->mpc_Ui->pc_Splitter->SetMargins(0, 0);
   // if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 10);

   //Links
   this->mpc_Ui->pc_LinkToManagerLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LinkToManagerLabel->setOpenExternalLinks(false);
   this->mpc_Ui->pc_LinkToManagerLabel->setFocusPolicy(Qt::NoFocus);
   this->mpc_Ui->pc_LinkToManagerLabel->setContextMenuPolicy(Qt::NoContextMenu);
   connect(this->mpc_Ui->pc_LinkToManagerLabel, &QLabel::linkActivated, this,
           &C_SdNdeCoWidget::m_OnLinkSwitchToManager);

   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigDeviceSelected, this,
           &C_SdNdeCoWidget::m_OnDeviceSelected);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigErrorChange, this,
           &C_SdNdeCoWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigInterfaceSelected, this,
           &C_SdNdeCoWidget::m_OnInterfaceSelected);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigCommDatapoolsChanged, this,
           &C_SdNdeCoWidget::SigCommDatapoolsChanged);
   connect(this->mpc_Ui->pc_PubOverview, &QPushButton::clicked, this, &C_SdNdeCoWidget::m_OnOverviewClicked);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigOpenOverview,
           this, &C_SdNdeCoWidget::m_OnOverviewClicked);
   connect(this->mpc_Ui->pc_OverviewWidget, &C_SdNdeCoOverviewWidget::SigInterfaceSelected,
           this->mpc_Ui->pc_CoConfigTree,
           &C_SdNdeCoConfigTreeView::SetInterfaceSelected);
   connect(this->mpc_Ui->pc_OverviewWidget, &C_SdNdeCoOverviewWidget::SigDeviceSelected, this->mpc_Ui->pc_CoConfigTree,
           &C_SdNdeCoConfigTreeView::SetDeviceSelected);
   connect(this->mpc_Ui->pc_IntfSettingsWidget, &C_SdNdeCoManagerIntfWidget::SigErrorChange,
           this, &C_SdNdeCoWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_IntfSettingsWidget, &C_SdNdeCoManagerIntfWidget::SigCommDatapoolsChanged,
           this, &C_SdNdeCoWidget::SigCommDatapoolsChanged);
   connect(this->mpc_Ui->pc_DeviceWidget, &C_SdNdeCoDeviceWidget::SigSwitchToBusProtocol, this,
           &C_SdNdeCoWidget::SigSwitchToBusProtocol);
   connect(this->mpc_Ui->pc_DeviceWidget, &C_SdNdeCoDeviceWidget::SigSwitchToBusProtocolMessage, this,
           &C_SdNdeCoWidget::SigSwitchToBusProtocolMessage);
   connect(this->mpc_Ui->pc_DeviceWidget, &C_SdNdeCoDeviceWidget::SigErrorChange,
           this, &C_SdNdeCoWidget::SigErrorChange);

   // For rechecking the errors in tree
   connect(this, &C_SdNdeCoWidget::SigErrorChange,
           this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::CheckError);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigNodeIdToBeChanged,
           this, &C_SdNdeCoWidget::m_NodeIdToBeChanged);
   connect(this->mpc_Ui->pc_CoConfigTree, &C_SdNdeCoConfigTreeView::SigNodeIdToBeChangedWithInterfaceIndex,
           this, &C_SdNdeCoWidget::m_NodeIdToBeChangedWithInterfaceIndex);
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
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditCoManagerSplitterHorizontal());

   // overview
   this->mpc_Ui->pc_OverviewWidget->LoadUserSettings();

   // tree layout
   this->mpc_Ui->pc_CoConfigTree->LoadUserSettings();

   // pdo table
   this->mpc_Ui->pc_DeviceWidget->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::SaveUserSettings(void) const
{
   // splitter
   const QList<int32_t> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditCoManagerSplitterHorizontal(c_Sizes.at(0));
   }

   // overview
   this->mpc_Ui->pc_OverviewWidget->SaveUserSettings();

   // tree layout
   this->mpc_Ui->pc_CoConfigTree->SaveUserSettings();

   // pdo table
   this->mpc_Ui->pc_DeviceWidget->SaveUserSettings();
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
void C_SdNdeCoWidget::SetNode(const uint32_t ou32_NodeIndex)
{
   uint32_t u32_ManagerNodeIndex;
   int32_t s32_Return;
   bool q_IsDevice;
   bool q_IsManagerSupported = true;

   this->mu32_NodeIndex = ou32_NodeIndex;

   // Check if node is already a CANopen device for a CANopen Manager
   s32_Return = C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerOfDeviceAndId(this->mu32_NodeIndex,
                                                                                &u32_ManagerNodeIndex,
                                                                                NULL,
                                                                                NULL);
   // C_RANGE should not happen
   tgl_assert(s32_Return != C_RANGE);
   q_IsDevice = (s32_Return == C_NO_ERR);

   if (q_IsDevice == false)
   {
      // No part of a manager
      // Check if the node can be a CANopen Manager
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         tgl_assert(pc_Node->pc_DeviceDefinition != NULL);
         if ((pc_Node->pc_DeviceDefinition != NULL) &&
             (pc_Node->u32_SubDeviceIndex < pc_Node->pc_DeviceDefinition->c_SubDevices.size()))
         {
            q_IsManagerSupported =
               pc_Node->pc_DeviceDefinition->c_SubDevices[pc_Node->u32_SubDeviceIndex].q_ProgrammingSupport;
         }
      }

      if (q_IsManagerSupported == true)
      {
         this->mpc_Ui->pc_CoConfigTree->SetNodeId(ou32_NodeIndex);
         this->mpc_Ui->pc_OverviewWidget->SetNodeIndex(ou32_NodeIndex);
      }
      else
      {
         this->mpc_Ui->pc_HintToManagerLabel->setText
            (C_GtGetText::h_GetText("The Node has the Programming Support disabled. No CANopen Manager is supported."));
         this->mpc_Ui->pc_LinkToManagerLabel->setVisible(false);
      }
   }
   else
   {
      // Node is already a device
      const C_OscNode * const pc_ManagerNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ManagerNodeIndex);

      this->mpc_Ui->pc_HintToManagerLabel->setText(C_GtGetText::h_GetText("Device is handled by CANopen Manager:"));
      this->mpc_Ui->pc_LinkToManagerLabel->setVisible(true);

      tgl_assert(pc_ManagerNode != NULL);
      if (pc_ManagerNode != NULL)
      {
         this->mpc_Ui->pc_LinkToManagerLabel->setText(
            C_Uti::h_GetLink(static_cast<QString>(pc_ManagerNode->c_Properties.c_Name.c_str()),
                             mc_STYLE_GUIDE_COLOR_6,
                             QString::number(u32_ManagerNodeIndex)));
      }
   }

   this->mpc_Ui->pc_WiManager->setVisible((q_IsDevice == false) && (q_IsManagerSupported == true));
   this->mpc_Ui->pc_WiDeviceLink->setVisible((q_IsDevice == true) || (q_IsManagerSupported == false));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Refresh widgets. Force them to reload data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::Refresh()
{
   this->mpc_Ui->pc_OverviewWidget->SetNodeIndex(this->mu32_NodeIndex);

   this->mpc_Ui->pc_DeviceWidget->Refresh();

   this->mpc_Ui->pc_IntfSettingsWidget->Refresh();

   this->mpc_Ui->pc_CoConfigTree->CheckError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Navigate to CANopen Manager configuration of specific interface number

   \param[in]  ou8_InterfaceNumber    Interface number of manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::OpenManagerConfiguration(const uint8_t ou8_InterfaceNumber)
{
   this->mpc_Ui->pc_CoConfigTree->OpenManagerConfiguration(ou8_InterfaceNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Navigate to CANopen device in tree

   \param[in]  ou32_DeviceNodeIndex    Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::OpenDeviceConfiguration(const uint32_t ou32_DeviceNodeIndex)
{
   this->mpc_Ui->pc_CoConfigTree->OpenDeviceConfiguration(ou32_DeviceNodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden show event

   Here: Set splitter size.

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::showEvent(QShowEvent * const opc_Event)
{
   this->mpc_Ui->pc_Splitter->SetFirstSegment(C_UsHandler::h_GetInstance()->GetSdNodeEditCoManagerSplitterHorizontal());
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
   this->mpc_Ui->pc_CoConfigTree->selectionModel()->clearSelection();
   this->mpc_Ui->pc_CoConfigTree->setCurrentIndex(QModelIndex()); // reset
   this->mpc_Ui->pc_CoConfigTree->ResetDelegate();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle interface selected

   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_OnInterfaceSelected(const uint8_t ou8_InterfaceNumber)
{
   this->mu8_InterfaceNumber = ou8_InterfaceNumber;
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
void C_SdNdeCoWidget::m_OnDeviceSelected(const uint8_t ou8_ManagerInterfaceNumber,
                                         const stw::opensyde_core::C_OscCanInterfaceId & orc_DeviceNodeId,
                                         const uint32_t ou32_UseCaseIndex)
{
   this->mpc_Ui->pc_OverviewWidget->setVisible(false);
   this->mpc_Ui->pc_PubOverview->setChecked(false);
   this->mpc_Ui->pc_IntfSettingsWidget->setVisible(false);
   this->mpc_Ui->pc_DeviceWidget->setVisible(true);

   this->mpc_Ui->pc_DeviceWidget->SetDeviceUseCase(this->mu32_NodeIndex, ou8_ManagerInterfaceNumber, orc_DeviceNodeId,
                                                   ou32_UseCaseIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal for switching to manager of this device

   \param[in]  orc_Link    Link
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_OnLinkSwitchToManager(const QString & orc_Link) const
{
   uint32_t u32_ManagerNodeIndex;
   bool q_Valid;

   // Link text is the index number of the manager node
   u32_ManagerNodeIndex = orc_Link.toInt(&q_Valid);
   tgl_assert(q_Valid == true);
   if (q_Valid == true)
   {
      const C_OscNode * const pc_ManagerNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ManagerNodeIndex);
      tgl_assert(pc_ManagerNode != NULL);
      if (pc_ManagerNode != NULL)
      {
         Q_EMIT (this->SigSwitchToDeviceNodeInCoManager(u32_ManagerNodeIndex,
                                                        static_cast<QString>(pc_ManagerNode->c_Properties.c_Name.c_str()),
                                                        this->mu32_NodeIndex));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Node change on user request

    \param[in]       ou32_NodeIndex     Current node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_NodeIdToBeChanged(const uint32_t ou32_NodeIndex)
{
   this->m_NodeIdToBeChangedWithInterfaceIndex(ou32_NodeIndex, this->mu8_InterfaceNumber);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Node Id change from Add CanOpen device

    \param[in]       ou32_NodeIndex          Current node index
    \param[in]       ou32_InterfaceIndex     Current node interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoWidget::m_NodeIdToBeChangedWithInterfaceIndex(const uint32_t ou32_NodeIndex,
                                                            const uint32_t ou32_InterfaceIndex)
{
   C_SdUtil::h_NodeIdToBeChanged(ou32_NodeIndex, ou32_InterfaceIndex, this);
   Q_EMIT (this->SigPropAndCoTabUpdateTrigger(ou32_NodeIndex));
}
