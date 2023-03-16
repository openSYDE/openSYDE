//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing and editing COM interface description

   Widget for editing bus properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QSpacerItem>

#include "C_SdBueComIfDescriptionWidget.hpp"
#include "ui_C_SdBueComIfDescriptionWidget.h"

#include "stwerrors.hpp"
#include "TglUtils.hpp"
#include "C_UsHandler.hpp"
#include "C_SdBueUnoBusProtNodeConnectDisconnectBaseCommand.hpp"
#include "C_Uti.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_CieUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OgeLabGenericNoPaddingNoMargins.hpp"
#include "C_SdBueMessageSelectorWidget.hpp"
#include "C_SdBueMessageSignalEditWidget.hpp"
#include "C_SdBueMessageSignalTableWidget.hpp"
#include "C_SdBueNodeSelectorWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

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
C_SdBueComIfDescriptionWidget::C_SdBueComIfDescriptionWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueComIfDescriptionWidget),
   mu32_BusIndex(0U),
   mu32_NodeIndex(0U),
   mu32_InterfaceIndex(0U),
   mq_ModeSingleNode(false),
   mq_LinkOnly(false),
   mq_IsCoDevice(false),
   mu32_CoDeviceIntfIndex(0U),
   mu32_CoManagerNodeIndexOfCoDevice(0U),
   mu8_CoManagerNodeIndexIntfNumber(0U),
   mq_IndexValid(false),
   mc_UndoManager(NULL)
{
   mpc_Ui->setupUi(this);

   // init static names
   this->InitStaticNames();

   // TODO BAY: Activate again when J1939 will be enabled
   this->mpc_Ui->pc_ProtocolTabWidget->setTabVisible(4, false);

   //Set undo manager
   this->mpc_Ui->pc_MessageSelectorWidget->SetUndoManager(&this->mc_UndoManager);

   //Set message sync manager
   this->mpc_Ui->pc_MessageSelectorWidget->SetMessageSyncManager(&this->mc_MessageSyncManager);
   this->mpc_Ui->pc_MsgSigEditWidget->SetMessageSyncManager(&this->mc_MessageSyncManager);
   this->mpc_Ui->pc_MsgSigTableWidget->SetMessageSyncManager(&this->mc_MessageSyncManager);

   this->mpc_Ui->pc_InterfaceSelectorTitleLabel->SetForegroundColor(4);
   this->mpc_Ui->pc_InterfaceSelectorTitleLabel->SetFontPixel(13, true, false);

   // Splitter stretch: if there is more space stretch right widget (i.e. index 1)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 10);

   //Links
   this->mpc_Ui->pc_LinkToBusLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
   this->mpc_Ui->pc_LinkToBusLabel->setOpenExternalLinks(false);
   this->mpc_Ui->pc_LinkToBusLabel->setFocusPolicy(Qt::NoFocus);
   connect(this->mpc_Ui->pc_LinkToBusLabel, &QLabel::linkActivated, this,
           &C_SdBueComIfDescriptionWidget::m_OnLinkSwitchToBus);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigSwitchToCoManager, this,
           &C_SdBueComIfDescriptionWidget::SigSwitchToCoManager);

   // connect functionality for undo/redo
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigConnectNodeToProt,
           this, &C_SdBueComIfDescriptionWidget::m_ConnectNodeToProt);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigConnectNodeToProtAndAddDataPool,
           this, &C_SdBueComIfDescriptionWidget::m_ConnectNodeToProtAndAddDataPool);
   connect(this->mpc_Ui->pc_NodeSelectorWidget, &C_SdBueNodeSelectorWidget::SigDisconnectNodeFromProt,
           this, &C_SdBueComIfDescriptionWidget::m_DisconnectNodeFromProt);
   connect(this->mpc_Ui->pc_ProtocolTabWidget, &C_OgeTawSelector::currentChanged,
           this, &C_SdBueComIfDescriptionWidget::m_ProtocolChanged);

   //Selection signals
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigMessageSelected,
           this, &C_SdBueComIfDescriptionWidget::m_SelectMessageProperties);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigSignalSelected,
           this, &C_SdBueComIfDescriptionWidget::m_SelectSignalProperties);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigMessagesSelected, this,
           &C_SdBueComIfDescriptionWidget::m_OnMessagesSelected);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigCommDataPoolAdded, this,
           &C_SdBueComIfDescriptionWidget::m_Reload);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigCommDataPoolAdded, this,
           &C_SdBueComIfDescriptionWidget::SigCommDataPoolAdded);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_MsgSigTableWidget, &C_SdBueMessageSignalTableWidget::SigMessageSelected, this,
           static_cast<void (C_SdBueComIfDescriptionWidget::*)(const C_OscCanMessageIdentificationIndices &) const>(
              &C_SdBueComIfDescriptionWidget::m_SelectMessage));
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_MsgSigTableWidget, &C_SdBueMessageSignalTableWidget::SigSignalSelected, this,
           static_cast<void (C_SdBueComIfDescriptionWidget::*)(
                          const C_OscCanMessageIdentificationIndices &,
                          const uint32_t &) const>(&C_SdBueComIfDescriptionWidget::m_SelectSignal));
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigSignalSelected, this,
           static_cast<void (C_SdBueComIfDescriptionWidget::*)(const C_OscCanMessageIdentificationIndices &,
                                                               const uint32_t &) const>(&C_SdBueComIfDescriptionWidget::
                                                                                        m_SelectSignal));

   //Change
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigMessageIdChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnMessageIdChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigMessageNameChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnMessageNameChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigMessageRxChanged,
           this, &C_SdBueComIfDescriptionWidget::m_OnMessageRxChanged);

   //Signal
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigErrorChanged, this,
           &C_SdBueComIfDescriptionWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigSignalCountOfMessageChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnSignalCountOfMessageChanged);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigMessageCountChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnMessageCountChanged);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigSignalNameChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnSignalNameChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigSignalStartBitChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnSignalStartBitChange);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigSignalPositionChanged, this,
           &C_SdBueComIfDescriptionWidget::m_OnSignalPositionChange);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigSelectName,
           this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SelectName);
   connect(this->mpc_Ui->pc_MessageSelectorWidget, &C_SdBueMessageSelectorWidget::SigRefreshSelection,
           this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::RefreshSelection);
   //Error
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigRecheckError, this,
           &C_SdBueComIfDescriptionWidget::m_RecheckError);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigRecheckErrorGlobal, this,
           &C_SdBueComIfDescriptionWidget::m_RecheckErrorGlobal);

   // MLV actions
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigAddSignal, this,
           &C_SdBueComIfDescriptionWidget::AddSignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigAddSignalMultiplexed, this,
           &C_SdBueComIfDescriptionWidget::AddSignalMultiplexed);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigCopySignal, this,
           &C_SdBueComIfDescriptionWidget::CopySignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigCutSignal, this,
           &C_SdBueComIfDescriptionWidget::CutSignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigPasteSignal, this,
           &C_SdBueComIfDescriptionWidget::PasteSignal);
   connect(this->mpc_Ui->pc_MsgSigEditWidget, &C_SdBueMessageSignalEditWidget::SigDeleteSignal, this,
           &C_SdBueComIfDescriptionWidget::DeleteSignal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueComIfDescriptionWidget::~C_SdBueComIfDescriptionWidget(void)
{
   this->TriggerSaveOfSplitterUserSettings();
   SaveUserSettings();
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::InitStaticNames(void) const
{
   QString c_Protocol;

   // tooltip Layer 2
   c_Protocol = C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OscCanProtocol::eLAYER2);
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(
      0, c_Protocol,
      static_cast<QString>(C_GtGetText::h_GetText("Edit Messages and Signals of protocol type %1.")).arg(c_Protocol));

   // tooltip ECeS
   c_Protocol = C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OscCanProtocol::eECES);
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(
      1, c_Protocol, static_cast<QString>(C_GtGetText::h_GetText("Edit Messages and Signals of protocol type %1 "
                                                                 "(ESX CAN efficient safety protocol).")).arg(
         c_Protocol));

   // tooltip ECoS
   c_Protocol = C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OscCanProtocol::eCAN_OPEN_SAFETY);
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(
      2, c_Protocol, static_cast<QString>(C_GtGetText::h_GetText("Edit Messages and Signals of protocol type %1 "
                                                                 "(ESX CANopen safety protocol).")).arg(c_Protocol));

   // tooltip CANopen
   c_Protocol = C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OscCanProtocol::eCAN_OPEN);
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(
      3, c_Protocol, static_cast<QString>(C_GtGetText::h_GetText("Edit PDO Messages and Signals of protocol type %1 "
                                                                 )).arg(c_Protocol));

   // tooltip J1939
   c_Protocol = C_PuiSdUtil::h_ConvertProtocolTypeToString(C_OscCanProtocol::eJ1939);
   this->mpc_Ui->pc_ProtocolTabWidget->SetToolTipInformation(
      4, c_Protocol,
      static_cast<QString>(C_GtGetText::h_GetText("Edit PG Messages and SP Signals of protocol type %1 ")).arg(
         c_Protocol));

   this->mpc_Ui->pc_InterfaceSelectorTitleLabel->setText(C_GtGetText::h_GetText("Node Interface"));

   this->mpc_Ui->pc_HintToBusLabel->setText(C_GtGetText::h_GetText("Node Interface is already used on a bus.\n"
                                                                   "Edit the COMM Messages here:"));
   this->mpc_Ui->pc_HintNoUsageLabel->setText(
      C_GtGetText::h_GetText("Node Interface has no CANopen Manager or\n"
                             "is not assigned to a CANopen Manager.\n"
                             "Activate the CANopen Manager in its\n"
                             "tab on this interface or\n"
                             "add the node as device to an already\n"
                             "existing CANopen Manager."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node 'mode' of the widget with all necessary indexes

   \param[in]  ou32_NodeIndex    Node index
   \param[in]  oe_Protocol       Current protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetNodeId(const uint32_t ou32_NodeIndex, const C_OscCanProtocol::E_Type oe_Protocol)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   this->SaveUserSettings();

   this->mu32_NodeIndex = ou32_NodeIndex;
   //Even if this widget isn't visible, its index is still used
   this->SetProtocol(oe_Protocol); // might change by LoadUserSettings below

   this->mq_ModeSingleNode = true;

   //Set flag
   this->mq_IndexValid = true;
   this->mq_IsCoDevice = false;

   //Map com protocol to Datapools
   this->m_FillNodeDatapoolIndexes(pc_Node);

   // adapt the ui
   this->mpc_Ui->pc_NodeSelectorWidget->setVisible(false);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      uint32_t u32_ItInterface;
      uint32_t u32_CanCounter = 0U;

      // Initialize the interface combo box
      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      disconnect(this->mpc_Ui->pc_CbInterface, static_cast<void (QComboBox::*)(
                                                              int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
                 &C_SdBueComIfDescriptionWidget::m_InterfaceChanged);

      this->mpc_Ui->pc_CbInterface->clear();
      this->mc_InterfaceNames.clear();
      this->mc_BusNames.clear();
      this->mc_MessageCount.clear();
      this->mc_MessageCount.resize(C_OscCanProtocol::hc_ALL_PROTOCOLS.size());
      this->mc_SignalCount.clear();
      this->mc_SignalCount.resize(C_OscCanProtocol::hc_ALL_PROTOCOLS.size());
      this->mc_ProtocolUsedOnBus.clear();
      this->mc_ProtocolUsedOnBus.resize(C_OscCanProtocol::hc_ALL_PROTOCOLS.size());

      for (u32_ItInterface = 0U; u32_ItInterface < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_ItInterface)
      {
         const C_OscNodeComInterfaceSettings & rc_ComInterface =
            pc_Node->c_Properties.c_ComInterfaces[u32_ItInterface];
         if (rc_ComInterface.e_InterfaceType == C_OscSystemBus::eCAN)
         {
            uint32_t u32_ProtocolCounter;
            const QString c_InterfaceName = C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN,
                                                                            rc_ComInterface.u8_InterfaceNumber);
            QString c_BusName = "";

            if (rc_ComInterface.GetBusConnected() == true)
            {
               const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
                  rc_ComInterface.u32_BusIndex);

               // Get bus name for the link and the interface combo box
               if (pc_Bus != NULL)
               {
                  c_BusName = pc_Bus->c_Name.c_str();
               }

               // Check the protocols for usage
               for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
                    ++u32_ProtocolCounter)
               {
                  bool q_ProtocolUsed = false;
                  uint32_t u32_NodeComProtocolCounter;

                  for (u32_NodeComProtocolCounter = 0U; u32_NodeComProtocolCounter < pc_Node->c_ComProtocols.size();
                       ++u32_NodeComProtocolCounter)
                  {
                     const C_OscCanProtocol & rc_Protocol = pc_Node->c_ComProtocols[u32_NodeComProtocolCounter];

                     if (rc_Protocol.e_Type == C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtocolCounter])
                     {
                        if ((u32_CanCounter < rc_Protocol.c_ComMessages.size()) &&
                            (rc_Protocol.c_ComMessages[u32_CanCounter].q_IsComProtocolUsedByInterface == true))
                        {
                           q_ProtocolUsed = true;
                        }
                        break;
                     }
                  }

                  this->mc_ProtocolUsedOnBus[u32_ProtocolCounter].push_back(q_ProtocolUsed);
               }

               // Special case CANopen
               if (this->mc_ProtocolUsedOnBus[static_cast<uint32_t>(C_OscCanProtocol::eCAN_OPEN)]
                   [this->mc_ProtocolUsedOnBus[static_cast<uint32_t>(C_OscCanProtocol::eCAN_OPEN)].size() - 1] == false)
               {
                  const uint32_t u32_CurIntf = static_cast<uint32_t>(
                     this->mc_ProtocolUsedOnBus[static_cast<uint32_t>(C_OscCanProtocol::eCAN_OPEN)].size() - 1U);
                  // The node is not active for the protocol. In this case the node is not the manager
                  // but could be the device. This must be checked separately
                  if (C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerOfDeviceAndId(
                         this->mu32_NodeIndex,
                         &this->mu32_CoManagerNodeIndexOfCoDevice,
                         &this->mu8_CoManagerNodeIndexIntfNumber,
                         &this->mc_CoDeviceInterfaceId) == C_NO_ERR)
                  {
                     if (rc_ComInterface.u8_InterfaceNumber == this->mc_CoDeviceInterfaceId.u8_InterfaceNumber)
                     {
                        // A CANopen Manager found to which this node interface is assigned as device
                        this->mc_ProtocolUsedOnBus[static_cast<uint32_t>(C_OscCanProtocol::eCAN_OPEN)][u32_CurIntf] =
                           true;

                        this->mu32_CoDeviceIntfIndex = u32_CurIntf;
                        this->mq_IsCoDevice = true;
                     }
                  }
               }
            }
            else
            {
               // Bus is not connected, so no specific protocol can be used on the bus
               for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
                    ++u32_ProtocolCounter)
               {
                  this->mc_ProtocolUsedOnBus[u32_ProtocolCounter].push_back(false);
               }
            }

            this->mpc_Ui->pc_CbInterface->addItem(c_InterfaceName);
            this->mc_InterfaceNames.push_back(c_InterfaceName);
            this->mc_BusNames.push_back(c_BusName);

            for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
                 ++u32_ProtocolCounter)
            {
               this->mc_MessageCount[u32_ProtocolCounter].push_back(0U);
               this->mc_SignalCount[u32_ProtocolCounter].push_back(0U);
            }
            ++u32_CanCounter;
         }
      }

      if (this->mpc_Ui->pc_CbInterface->count() > 0)
      {
         this->LoadUserSettings();
      }

      //lint -e{929} Cast required to avoid ambiguous signal of qt interface
      connect(this->mpc_Ui->pc_CbInterface, static_cast<void (QComboBox::*)(
                                                           int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
              &C_SdBueComIfDescriptionWidget::m_InterfaceChanged);
   }

   this->updateGeometry();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the the current protocol type to the type associated to the specific COM Datapool

   Function is only usable in node mode

   \param[in]  ou32_DataPoolIndexw  Index of a COMM Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetProtocolByDataPool(const uint32_t ou32_DataPoolIndexw)
{
   if (this->mq_ModeSingleNode == true)
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
      if ((pc_Node != NULL) &&
          (ou32_DataPoolIndexw < pc_Node->c_DataPools.size()) &&
          (pc_Node->c_DataPools[ou32_DataPoolIndexw].e_Type == C_OscNodeDataPool::eCOM))
      {
         uint32_t u32_ProtocolCounter;

         for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < pc_Node->c_ComProtocols.size(); ++u32_ProtocolCounter)
         {
            if (ou32_DataPoolIndexw == pc_Node->c_ComProtocols[u32_ProtocolCounter].u32_DataPoolIndex)
            {
               // Match found
               this->SetProtocol(pc_Node->c_ComProtocols[u32_ProtocolCounter].e_Type);
               break;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the bus 'mode' of the widget

   \param[in]  ou32_BusIndex  Bus index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetBusId(const uint32_t ou32_BusIndex)
{
   std::vector<uint32_t> c_MsgCounter;

   this->SaveUserSettings();

   this->mu32_BusIndex = ou32_BusIndex;
   this->mq_ModeSingleNode = false;

   // Prepare message counter vector
   c_MsgCounter.push_back(0);
   this->mc_MessageCount.clear();
   this->mc_MessageCount.resize(C_OscCanProtocol::hc_ALL_PROTOCOLS.size(), c_MsgCounter);
   this->mc_SignalCount.clear();
   this->mc_SignalCount.resize(C_OscCanProtocol::hc_ALL_PROTOCOLS.size(), c_MsgCounter);

   //Set flag
   this->mq_IndexValid = true;

   //Undo
   this->mc_UndoManager.clear();

   this->m_Reload();

   // adapt the ui
   this->mpc_Ui->pc_InterfaceSelectorWidget->setVisible(false);

   this->mpc_Ui->pc_NodeSelectorWidget->SetBusId(ou32_BusIndex);
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(this->GetActProtocol());
   this->mpc_Ui->pc_MsgSigEditWidget->SetBusId(ou32_BusIndex);

   this->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the initial focus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetInitialFocus(void) const
{
   //handle initial focus
   // Leaving the line edit with default focus or other element can cause unwanted change handling
   this->mpc_Ui->pc_MsgSigEditWidget->DisconnectAllChanges();
   this->mpc_Ui->pc_MessageSelectorWidget->SetInitialFocus();
   this->mpc_Ui->pc_MsgSigEditWidget->ConnectAllChanges();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current protocol

   Warning: no update mechanism is triggered

   \param[in]  oe_Protocol    Current protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SetProtocol(const C_OscCanProtocol::E_Type oe_Protocol) const
{
   this->mpc_Ui->pc_ProtocolTabWidget->setCurrentIndex(mh_GetIndexOfProtocol(oe_Protocol));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::PartialReload(void)
{
   //Update node check marks
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(this->GetActProtocol());
   m_ReloadMessages();
   //Update error
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_MessageIndex    Message index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SelectMessageSearch(const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_DataPoolIndex,
                                                        const uint32_t ou32_ListIndex,
                                                        const uint32_t ou32_MessageIndex) const
{
   C_OscCanMessageIdentificationIndices c_MessageId;

   // fills all information except the message index
   m_PrepareMessageId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex, c_MessageId);

   if (this->mq_ModeSingleNode == true)
   {
      // In case of node mode the interface must be adapted
      this->mpc_Ui->pc_CbInterface->setCurrentIndex(c_MessageId.u32_InterfaceIndex);
   }

   //Update protocol (might change)
   this->SetProtocol(c_MessageId.e_ComProtocol);

   c_MessageId.u32_MessageIndex = ou32_MessageIndex;
   this->m_SelectMessage(c_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index in the datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SelectSignalSearch(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex,
                                                       const uint32_t ou32_ListIndex,
                                                       const uint32_t ou32_ElementIndex) const
{
   C_OscCanMessageIdentificationIndices c_MessageId;

   // fills all information except the message index
   const std::vector<C_OscCanMessage> * const pc_Messages = m_PrepareMessageId(ou32_NodeIndex, ou32_DataPoolIndex,
                                                                               ou32_ListIndex, c_MessageId);

   if (pc_Messages != NULL)
   {
      uint32_t u32_Counter;

      // search the signal in all messages
      for (u32_Counter = 0U; u32_Counter < pc_Messages->size(); ++u32_Counter)
      {
         const C_OscCanMessage & rc_Message = (*pc_Messages)[u32_Counter];

         for (uint32_t u32_SignalIndex = 0U; u32_SignalIndex < rc_Message.c_Signals.size(); ++u32_SignalIndex)
         {
            if (ou32_ElementIndex == rc_Message.c_Signals[u32_SignalIndex].u32_ComDataElementIndex)
            {
               c_MessageId.u32_MessageIndex = u32_Counter;

               if (this->mq_ModeSingleNode == true)
               {
                  // In case of node mode the interface must be adapted
                  this->mpc_Ui->pc_CbInterface->setCurrentIndex(c_MessageId.u32_InterfaceIndex);
               }

               //Update protocol (might change)
               this->SetProtocol(c_MessageId.e_ComProtocol);

               this->m_SelectSignal(c_MessageId, u32_SignalIndex);
               break;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new signal

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  ou16_StartBit  Start bit for new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::AddSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                              const uint16_t ou16_StartBit) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->AddSignal(orc_MessageId, ou16_StartBit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  ou16_StartBit        Start bit for new signal
   \param[in]  ou16_MultiplexValue  Concrete multiplexed value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::AddSignalMultiplexed(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                         const uint16_t ou16_StartBit,
                                                         const uint16_t ou16_MultiplexValue) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->AddSignalMultiplexed(orc_MessageId, ou16_StartBit, ou16_MultiplexValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::CopySignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                               const uint32_t ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->CopySignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::CutSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                              const uint32_t ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->CutSignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Paste copied signal

   \param[in]  orc_MessageId  Message identification indices
   \param[in]  ou16_StartBit  Start bit for new signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::PasteSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                const uint16_t ou16_StartBit) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->PasteSignal(orc_MessageId, ou16_StartBit);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete specific signal

   \param[in]  orc_MessageId     Message identification indices
   \param[in]  ou32_SignalIndex  Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::DeleteSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                 const uint32_t ou32_SignalIndex) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->DeleteSignal(orc_MessageId, ou32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Import messages from DBC, EDS or DCF file to this bus.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::ImportMessages(void)
{
   const C_OscCanProtocol::E_Type e_Protocol = this->GetActProtocol();

   if (e_Protocol != C_OscCanProtocol::eCAN_OPEN)
   {
      std::vector<uint32_t> c_NodeIndexes;
      std::vector<uint32_t> c_InterfaceIndexes;
      const int32_t s32_Return =
         C_CieUtil::h_ImportFile(this->mu32_BusIndex, e_Protocol, this, c_NodeIndexes, c_InterfaceIndexes);

      if (s32_Return == C_NO_ERR)
      {
         // connect all nodes that were part of message import
         tgl_assert(c_NodeIndexes.size() == c_InterfaceIndexes.size());
         if (c_NodeIndexes.size() == c_InterfaceIndexes.size())
         {
            for (uint32_t u32_ItIndex = 0; u32_ItIndex < c_NodeIndexes.size(); u32_ItIndex++)
            {
               const int32_t s32_NumMessageContainers =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainers(c_NodeIndexes[u32_ItIndex],
                                                                                   e_Protocol,
                                                                                   c_InterfaceIndexes[u32_ItIndex]).size();
               const int32_t s32_NumComDataPools =
                  C_PuiSdHandler::h_GetInstance()->GetOscCanDataPools(c_NodeIndexes[u32_ItIndex], e_Protocol).size();

               // on import a Datapool was created if necessary - just to make sure
               tgl_assert(s32_NumComDataPools == s32_NumMessageContainers);
               if (s32_NumComDataPools > 0)
               {
                  C_PuiSdHandler::h_GetInstance()->
                  SetCanProtocolMessageContainerConnected(c_NodeIndexes[u32_ItIndex], e_Protocol,
                                                          c_InterfaceIndexes[u32_ItIndex], true);
               }
            }
         }

         // reload and update errors
         this->m_Reload();
         Q_EMIT (this->SigErrorChange());
      }
   }
   else
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR,
                                        C_GtGetText::h_GetText("Import Messages Feature is not supported for "
                                                               "CANopen protocol. List of available PDOs messages "
                                                               "is defined by the CANopen Manager Configuration."));
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Import Messages"));
      c_MessageBox.SetCustomMinHeight(180, 180);
      c_MessageBox.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press release event slot

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   //Debug commands
   if (((C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ShiftModifier) == true) &&
        (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)) &&
       (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::AltModifier) == true))
   {
      if (static_cast<Qt::Key>(opc_KeyEvent->key()) == Qt::Key_O)
      {
         //Undo redo command view
         this->mc_UndoManager.ToggleCommandDisplay();
      }
   }
   //Debug commands end
   //alt
   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::AltModifier) == true)
   {
      if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ShiftModifier) == true)
      {
         if (static_cast<Qt::Key>(opc_KeyEvent->key()) == Qt::Key_Backspace)
         {
            // redo
            this->mc_UndoManager.DoRedo();
         }
      }
      else
      {
         if (static_cast<Qt::Key>(opc_KeyEvent->key()) == Qt::Key_Backspace)
         {
            // undo
            this->mc_UndoManager.DoUndo();
         }
      }
   }
   //ctrl
   if (C_Uti::h_CheckKeyModifier(opc_KeyEvent->modifiers(), Qt::ControlModifier) == true)
   {
      switch (opc_KeyEvent->key())
      {
      case Qt::Key_Z:
         // undo
         this->mc_UndoManager.DoUndo();
         break;
      case Qt::Key_Y:
         // redo
         this->mc_UndoManager.DoRedo();
         break;
      default:
         break;
      }
   }

   if (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F5))
   {
      this->mpc_Ui->pc_MsgSigEditWidget->RefreshColors();
   }

   QWidget::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::showEvent(QShowEvent * const opc_Event)
{
   //Redundant call in bus edit but necessary for node edit
   TriggerLoadOfSplitterUserSettings();
   LoadUserSettings();
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: Save info

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::hideEvent(QHideEvent * const opc_Event)
{
   SaveUserSettings();
   QWidget::hideEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ConnectNodeToProt(const uint32_t ou32_NodeIndex,
                                                        const uint32_t ou32_InterfaceIndex)
{
   this->mc_UndoManager.DoConnectNodeToProt(ou32_NodeIndex, ou32_InterfaceIndex,
                                            this->GetActProtocol(), this);
   m_OnConnectionChange();
   Q_EMIT (this->SigChanged());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ConnectNodeToProtAndAddDataPool(const uint32_t ou32_NodeIndex,
                                                                      const uint32_t ou32_InterfaceIndex)
{
   this->mc_UndoManager.DoConnectNodeToProtAndAddDataPool(ou32_NodeIndex, ou32_InterfaceIndex,
                                                          this->GetActProtocol(),
                                                          this);
   m_OnConnectionChange();
   Q_EMIT (this->SigChanged());
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_DisconnectNodeFromProt(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_InterfaceIndex)
{
   // Inform the tree and message signal widget before the old message ids are probably deleted
   this->mpc_Ui->pc_MessageSelectorWidget->OnNodeDisconnected(ou32_NodeIndex, ou32_InterfaceIndex);
   this->mpc_Ui->pc_MsgSigEditWidget->OnNodeDisconnected(ou32_NodeIndex, ou32_InterfaceIndex);

   this->mc_UndoManager.DoDisconnectNodeFromProt(ou32_NodeIndex, ou32_InterfaceIndex,
                                                 this->GetActProtocol(), this);
   m_OnConnectionChange();
   Q_EMIT (this->SigChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle protocol change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ProtocolChanged(void)
{
   //Undo
   this->mc_UndoManager.clear();

   this->m_Reload();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle interface change

   In node mode only
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_InterfaceChanged(void)
{
   //Undo
   this->mc_UndoManager.clear();

   if (this->mpc_Ui->pc_CbInterface->currentIndex() >= 0)
   {
      this->mu32_InterfaceIndex = static_cast<uint32_t>(this->mpc_Ui->pc_CbInterface->currentIndex());

      this->m_Reload();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle complete reload
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_Reload(void)
{
   const C_OscCanProtocol::E_Type e_Protocol = this->GetActProtocol();

   if ((this->mq_ModeSingleNode == false) ||
       ((this->mc_ProtocolUsedOnBus[static_cast<uint32_t>(e_Protocol)][this->mu32_InterfaceIndex] == false) &&
        (e_Protocol != C_OscCanProtocol::eCAN_OPEN)))
   {
      this->mpc_Ui->pc_LinkToBusWidget->setVisible(false);
      this->mpc_Ui->pc_CanOpenNoUsageWidget->setVisible(false);
      this->mpc_Ui->pc_MessageSelectorWidget->setVisible(true);

      if (this->mq_ModeSingleNode == true)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

         this->m_FillNodeDatapoolIndexes(pc_Node);

         // Only in case of node mode
         this->mpc_Ui->pc_MsgSigEditWidget->SetNodeId(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                                                      this->mc_DatapoolIndexes);
      }

      // Selector widget needs to know the protocol before reloading messages,
      // but for all other adaptions of the widget, the messages must be reloaded already
      // -> UpdateButtonText
      this->mpc_Ui->pc_MessageSelectorWidget->SetProtocolType(e_Protocol);

      //Message sync manager
      m_ReloadMessages();

      //Update widgets
      this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(e_Protocol);
      this->mpc_Ui->pc_MessageSelectorWidget->UpdateButtonText();
      this->mpc_Ui->pc_MsgSigEditWidget->SetComProtocol(e_Protocol);

      //Show messages
      //Handle button
      this->mpc_Ui->pc_MessageSelectorWidget->SelectMessagesWithoutSignal();
      m_OnMessagesSelected();

      this->mq_LinkOnly = false;
   }
   else
   {
      if ((e_Protocol == C_OscCanProtocol::eCAN_OPEN) &&
          (this->mc_ProtocolUsedOnBus[static_cast<uint32_t>(e_Protocol)][this->mu32_InterfaceIndex] == false))
      {
         // Special case CANopen and is not connected. Editing COMM Messages are not possible when not connected to a
         // bus and integrated to a CANopen network as manager or device
         this->mpc_Ui->pc_LinkToBusWidget->setVisible(false);
         this->mpc_Ui->pc_CanOpenNoUsageWidget->setVisible(true);
      }
      else
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

         if ((pc_Node != NULL) &&
             (this->mu32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
         {
            const C_OscNodeComInterfaceSettings & rc_ComInterface =
               pc_Node->c_Properties.c_ComInterfaces[this->mu32_InterfaceIndex];

            if (rc_ComInterface.GetBusConnected() == true)
            {
               this->mpc_Ui->pc_LinkToBusLabel->setText(
                  C_Uti::h_GetLink(this->mc_BusNames[this->mu32_InterfaceIndex] + " - COMM Messages",
                                   mc_STYLE_GUIDE_COLOR_6,
                                   this->mc_BusNames[this->mu32_InterfaceIndex]));
            }
         }

         this->mpc_Ui->pc_LinkToBusWidget->setVisible(true);
         this->mpc_Ui->pc_CanOpenNoUsageWidget->setVisible(false);
      }
      this->mpc_Ui->pc_MsgSigEditWidget->setVisible(false);
      this->mpc_Ui->pc_MsgSigTableWidget->setVisible(false);
      this->mpc_Ui->pc_MessageSelectorWidget->setVisible(false);

      this->mq_LinkOnly = true;
   }

   // Update the tab text
   this->m_UpdateText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reload messages after connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_ReloadMessages(void)
{
   //Message sync manager
   if (this->mq_ModeSingleNode == false)
   {
      this->mc_MessageSyncManager.Init(this->mu32_BusIndex, this->GetActProtocol());
      this->mpc_Ui->pc_MessageSelectorWidget->SetBusId(this->mu32_BusIndex);
   }
   else
   {
      this->mc_MessageSyncManager.Init(this->mu32_NodeIndex, this->mu32_InterfaceIndex, this->GetActProtocol());
      this->mpc_Ui->pc_MessageSelectorWidget->SetNodeId(this->mu32_NodeIndex, this->mu32_InterfaceIndex,
                                                        this->mc_DatapoolIndexes);
   }

   //Message tree
   this->mpc_Ui->pc_MessageSelectorWidget->InitFromData();

   //Message table
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SelectMessage(const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   m_SelectMessageProperties(orc_MessageId);
   //Tree
   this->mpc_Ui->pc_MessageSelectorWidget->SelectMessage(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change with all aspects

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SelectSignal(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                   const uint32_t & oru32_SignalIndex) const
{
   m_SelectSignalProperties(orc_MessageId, oru32_SignalIndex);
   //Tree
   this->mpc_Ui->pc_MessageSelectorWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filling the Datapool indexes of the node which are assigned to the current COMM protocol

   \param[in]  opc_Node    Pointer to node which
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_FillNodeDatapoolIndexes(const C_OscNode * const opc_Node)
{
   this->mc_DatapoolIndexes.clear();

   if (opc_Node != NULL)
   {
      uint32_t u32_ItProtocol;

      for (u32_ItProtocol = 0U; u32_ItProtocol < opc_Node->c_ComProtocols.size(); ++u32_ItProtocol)
      {
         const C_OscCanProtocol & rc_Protocol = opc_Node->c_ComProtocols[u32_ItProtocol];
         if (rc_Protocol.e_Type == this->GetActProtocol())
         {
            this->mc_DatapoolIndexes.push_back(rc_Protocol.u32_DataPoolIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the active protocol type depending on which tab the user is navigating.

   \return
   protocol as member of enum
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanProtocol::E_Type C_SdBueComIfDescriptionWidget::GetActProtocol(void) const
{
   C_OscCanProtocol::E_Type e_Protocol;

   switch (this->mpc_Ui->pc_ProtocolTabWidget->currentIndex())
   {
   case 1:
      e_Protocol = C_OscCanProtocol::eECES;
      break;
   case 2:
      e_Protocol = C_OscCanProtocol::eCAN_OPEN_SAFETY;
      break;
   case 3:
      e_Protocol = C_OscCanProtocol::eCAN_OPEN;
      break;
   case 4:
      e_Protocol = C_OscCanProtocol::eJ1939;
      break;
   case 0: // default case: layer 2
   default:
      e_Protocol = C_OscCanProtocol::eLAYER2;
      break;
   }

   return e_Protocol;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger load of splitter user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::TriggerLoadOfSplitterUserSettings(void) const
{
   const int32_t s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetSdBusEditTreeSplitterHorizontal();
   const int32_t s32_SecondSegmentWidth = C_UsHandler::h_GetInstance()->GetSdBusEditTreeSplitterHorizontal2();

   if (s32_SecondSegmentWidth == 0)
   {
      this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
   }
   else
   {
      if (s32_FirstSegmentWidth != 0)
      {
         QList<int32_t> c_List;
         c_List.push_back(s32_FirstSegmentWidth);
         c_List.push_back(s32_SecondSegmentWidth);
         this->mpc_Ui->pc_Splitter->setSizes(c_List);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger save of splitter user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::TriggerSaveOfSplitterUserSettings(void) const
{
   if (this->mq_IndexValid == true)
   {
      bool q_Continue = false;
      if (this->mq_ModeSingleNode == false)
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);
         if ((pc_Bus != NULL) && (pc_Bus->e_Type == C_OscSystemBus::eCAN))
         {
            q_Continue = true;
         }
      }
      else
      {
         //For node case this is not easy to detect if user ever clicked on comm interface description tab...
         q_Continue = true;
      }
      if (q_Continue == true)
      {
         //Store splitter position
         const QList<int32_t> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

         if (c_Sizes.size() > 0)
         {
            const int32_t s32_Size = c_Sizes.at(0);
            //Avoid saving invalid values
            if (s32_Size > 0)
            {
               C_UsHandler::h_GetInstance()->SetSdBusEditTreeSplitterHorizontal(s32_Size);
            }
            if (c_Sizes.size() > 1)
            {
               const int32_t s32_Size2 = c_Sizes.at(1);
               //Avoid saving invalid values
               if (s32_Size2 > 0)
               {
                  C_UsHandler::h_GetInstance()->SetSdBusEditTreeSplitterHorizontal2(s32_Size2);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save current state in user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::SaveUserSettings(void) const
{
   if (this->mq_IndexValid == true)
   {
      const stw::opensyde_core::C_OscCanProtocol::E_Type e_SelectedProtocol = GetActProtocol();
      bool q_MessageSelected;
      QString c_SelectedMessageName;
      bool q_SignalSelected;
      QString c_SelectedSignalName;
      std::vector<int32_t> c_MessageValues;
      std::vector<int32_t> c_SignalValues;
      this->mpc_Ui->pc_MsgSigTableWidget->SaveUserSettings(c_MessageValues, c_SignalValues);

      if (this->mpc_Ui->pc_MessageSelectorWidget->IsSelectionEmpty() == true)
      {
         q_MessageSelected = false;
         q_SignalSelected = false;
         c_SelectedMessageName = "";
         c_SelectedSignalName = "";
      }
      else
      {
         this->mpc_Ui->pc_MsgSigEditWidget->GetLastSelection(q_MessageSelected, &c_SelectedMessageName,
                                                             q_SignalSelected, &c_SelectedSignalName);
      }

      if (this->mq_ModeSingleNode == false)
      {
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);
         if (pc_Bus != NULL)
         {
            C_UsHandler::h_GetInstance()->SetProjSdBusCommSignalOverviewColumnWidth(
               pc_Bus->c_Name.c_str(), c_SignalValues);
            C_UsHandler::h_GetInstance()->SetProjSdBusCommMessageOverviewColumnWidth(
               pc_Bus->c_Name.c_str(), c_MessageValues);
            //Set
            C_UsHandler::h_GetInstance()->SetProjSdBusSelectedMessage(
               pc_Bus->c_Name.c_str(), e_SelectedProtocol, q_MessageSelected, c_SelectedMessageName,
               q_SignalSelected, c_SelectedSignalName);
         }
      }
      else
      {
         // Save the user settings for node mode in the first protocol Datapool user settings
         // The selected message has no relation to the used Datapool index in this logic. The name is used as
         // identifier.
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

         if (pc_Node != NULL)
         {
            //Set
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedProtocol(pc_Node->c_Properties.c_Name.c_str(),
                                                                        e_SelectedProtocol);
            C_UsHandler::h_GetInstance()->SetProjSdNodeSelectedInterface(pc_Node->c_Properties.c_Name.c_str(),
                                                                         this->mu32_InterfaceIndex);

            if (this->mc_DatapoolIndexes.size() > 0)
            {
               const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPool(
                  this->mu32_NodeIndex, e_SelectedProtocol, this->mc_DatapoolIndexes[0]);
               const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolList(
                  this->mu32_NodeIndex, e_SelectedProtocol, this->mu32_InterfaceIndex, this->mc_DatapoolIndexes[0],
                  true);
               if ((pc_DataPool != NULL) && (pc_List != NULL))
               {
                  //Set
                  C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolListSelectedMessage(
                     pc_Node->c_Properties.c_Name.c_str(), pc_DataPool->c_Name.c_str(),
                     pc_List->c_Name.c_str(), e_SelectedProtocol, q_MessageSelected, c_SelectedMessageName, q_SignalSelected,
                     c_SelectedSignalName);
                  C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolCommSignalOverviewColumnWidth(
                     pc_Node->c_Properties.c_Name.c_str(), pc_DataPool->c_Name.c_str(),
                     pc_List->c_Name.c_str(), c_SignalValues);
                  C_UsHandler::h_GetInstance()->SetProjSdNodeDatapoolCommMessageOverviewColumnWidth(
                     pc_Node->c_Properties.c_Name.c_str(), pc_DataPool->c_Name.c_str(),
                     pc_List->c_Name.c_str(), c_MessageValues);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore state from user settings

   Warning: This does reopen a const copy of the user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::LoadUserSettings(void)
{
   C_OscCanProtocol::E_Type e_SelectedProtocol = GetActProtocol();
   bool q_MessageSelected = false;
   QString c_SelectedMessageName;
   bool q_SignalSelected = false;
   QString c_SelectedSignalName;

   if (this->mq_ModeSingleNode == false)
   {
      const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_BusIndex);
      if (pc_Bus != NULL)
      {
         const C_UsCommunication c_UserSettingsBus = C_UsHandler::h_GetInstance()->GetProjSdBus(pc_Bus->c_Name.c_str());
         c_UserSettingsBus.GetLastSelectedMessage(e_SelectedProtocol, q_MessageSelected, c_SelectedMessageName,
                                                  q_SignalSelected, c_SelectedSignalName);
         //Reinit necessary (in some cases)
         this->mc_MessageSyncManager.Init(this->mu32_BusIndex, e_SelectedProtocol);
         this->mpc_Ui->pc_MsgSigTableWidget->LoadUserSettings(c_UserSettingsBus.GetMessageOverviewColumnWidth(),
                                                              c_UserSettingsBus.GetSignalOverviewColumnWidth());
      }

      this->SetProtocol(e_SelectedProtocol);
   }
   else
   {
      // Save the user settings for node mode in the first protocol Datapool user settings
      // The selected message has no relation to the used Datapool index in this logic. The name is used as
      // identifier.
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

      if (pc_Node != NULL)
      {
         const C_UsNode c_UserSettingsNode = C_UsHandler::h_GetInstance()->GetProjSdNode(
            pc_Node->c_Properties.c_Name.c_str());
         uint32_t u32_SelectedInterface = c_UserSettingsNode.GetSelectedInterface();

         // Protocol
         e_SelectedProtocol = c_UserSettingsNode.GetSelectedProtocol();
         this->SetProtocol(e_SelectedProtocol);

         // Interface
         if (u32_SelectedInterface >= static_cast<uint32_t>(this->mpc_Ui->pc_CbInterface->count()))
         {
            u32_SelectedInterface = 0;
         }
         this->mpc_Ui->pc_CbInterface->setCurrentIndex(u32_SelectedInterface);
         this->m_InterfaceChanged();

         // Selection
         if (this->mc_DatapoolIndexes.size() > 0)
         {
            const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPool(
               this->mu32_NodeIndex, e_SelectedProtocol, this->mc_DatapoolIndexes[0]);
            const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolList(
               this->mu32_NodeIndex, e_SelectedProtocol, this->mu32_InterfaceIndex, this->mc_DatapoolIndexes[0], true);
            if ((pc_DataPool != NULL) && (pc_List != NULL))
            {
               //Do not allow change of protocol as this may be different from the expected value in the default case
               C_OscCanProtocol::E_Type e_Tmp;

               const C_UsNodeDatapool c_UserSettingsDataPool = c_UserSettingsNode.GetDatapool(
                  pc_DataPool->c_Name.c_str());
               const C_UsCommunication c_UserSettingsList = c_UserSettingsDataPool.GetCommList(pc_List->c_Name.c_str());

               //Get
               c_UserSettingsList.GetLastSelectedMessage(e_Tmp, q_MessageSelected, c_SelectedMessageName,
                                                         q_SignalSelected, c_SelectedSignalName);
               this->mpc_Ui->pc_MsgSigTableWidget->LoadUserSettings(c_UserSettingsList.GetMessageOverviewColumnWidth(),
                                                                    c_UserSettingsList.GetSignalOverviewColumnWidth());
            }
         }
      }
      else
      {
         this->SetProtocol(e_SelectedProtocol);
      }
   }

   //Set
   this->mpc_Ui->pc_NodeSelectorWidget->SetProtocol(e_SelectedProtocol);
   this->mpc_Ui->pc_MessageSelectorWidget->SetProtocolType(e_SelectedProtocol);
   this->mpc_Ui->pc_MessageSelectorWidget->UpdateButtonText();
   this->mpc_Ui->pc_MsgSigEditWidget->SetComProtocol(e_SelectedProtocol);
   if ((q_MessageSelected == true) || (q_SignalSelected == true))
   {
      C_OscCanMessageIdentificationIndices c_MessageId;
      if (this->mc_MessageSyncManager.GetMessageIdForMessageName(c_SelectedMessageName, c_MessageId) == C_NO_ERR)
      {
         if (q_SignalSelected == true)
         {
            const C_OscCanMessage * const pc_Message = C_PuiSdHandler::h_GetInstance()->GetCanMessage(c_MessageId);
            if (pc_Message != NULL)
            {
               uint32_t u32_FoundIndex = 0UL;
               bool q_Found = false;
               for (uint32_t u32_ItSignal = 0; u32_ItSignal < pc_Message->c_Signals.size(); ++u32_ItSignal)
               {
                  const C_OscNodeDataPoolListElement * const pc_OscData =
                     C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolListElement(c_MessageId, u32_ItSignal);
                  if (pc_OscData != NULL)
                  {
                     if (pc_OscData->c_Name == c_SelectedSignalName.toStdString().c_str())
                     {
                        u32_FoundIndex = u32_ItSignal;
                        q_Found = true;
                        break;
                     }
                  }
               }
               if (q_Found == true)
               {
                  this->m_SelectSignal(c_MessageId, u32_FoundIndex);
               }
            }
         }
         else
         {
            this->m_SelectMessage(c_MessageId);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the index of the protocol

   \param[in]  oe_Protocol    Current protocol

   \return
   Index of protocol in tab order
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueComIfDescriptionWidget::mh_GetIndexOfProtocol(const C_OscCanProtocol::E_Type oe_Protocol)
{
   int32_t s32_Index = 0;

   switch (oe_Protocol)
   {
   case C_OscCanProtocol::eECES:
      s32_Index = 1;
      break;
   case C_OscCanProtocol::eCAN_OPEN_SAFETY:
      s32_Index = 2;
      break;
   case C_OscCanProtocol::eLAYER2:
      s32_Index = 0;
      break;
   case C_OscCanProtocol::eCAN_OPEN:
      s32_Index = 3;
      break;
   case C_OscCanProtocol::eJ1939:
      s32_Index = 4;
      break;
   default:
      tgl_assert(false);
      break;
   }

   return s32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SelectMessageProperties(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MsgSigEditWidget->SelectMessage(orc_MessageId);
   //Visibility
   this->mpc_Ui->pc_MsgSigEditWidget->setVisible(true);
   this->mpc_Ui->pc_MsgSigTableWidget->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Select signal

   \param[in]  orc_MessageId        Message identification indices
   \param[in]  oru32_SignalIndex    Signal index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_SelectSignalProperties(const C_OscCanMessageIdentificationIndices & orc_MessageId,
                                                             const uint32_t & oru32_SignalIndex) const
{
   this->mpc_Ui->pc_MsgSigEditWidget->SelectSignal(orc_MessageId, oru32_SignalIndex);
   //Visibility
   this->mpc_Ui->pc_MsgSigEditWidget->setVisible(true);
   this->mpc_Ui->pc_MsgSigTableWidget->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Any message id is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageIdChange(void) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnMessageIdChange();
   this->mpc_Ui->pc_MessageSelectorWidget->RecheckProtocolError();
   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The specified message data was changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageNameChange(void) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnMessageNameChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal name has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnSignalNameChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnSignalNameChange(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal start bit has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnSignalStartBitChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnSignalStartBitChange(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The signal position has changed

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnSignalPositionChange(const C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   this->mpc_Ui->pc_MessageSelectorWidget->OnSignalPositionChange(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle connection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnConnectionChange(void)
{
   this->mpc_Ui->pc_MsgSigEditWidget->OnConnectionChange();

   if (this->mq_ModeSingleNode == false)
   {
      const C_OscCanProtocol::E_Type e_ProtocolType = this->GetActProtocol();

      // In bus mode the signal count is not relevant
      this->mc_MessageCount[static_cast<uint32_t>(e_ProtocolType)][0] =
         this->mc_MessageSyncManager.GetUniqueMessageCount(e_ProtocolType);

      this->m_UpdateTabText(e_ProtocolType);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of signal count in message

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnSignalCountOfMessageChanged(
   const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   this->mpc_Ui->pc_MsgSigEditWidget->OnSignalCountOfMessageChanged(orc_MessageId);
   //Update messages (All properties changes)
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();

   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   // Update the message counter of the protocol/interface
   // In bus mode the signal count is not relevant
   if ((this->mq_LinkOnly == false) &&
       (this->mq_ModeSingleNode == true))
   {
      const C_OscCanProtocol::E_Type e_ProtocolType = this->GetActProtocol();

      this->mc_MessageCount[static_cast<uint32_t>(e_ProtocolType)][this->mu32_InterfaceIndex] =
         this->mc_MessageSyncManager.GetUniqueMessageCount(
            e_ProtocolType,
            &this->mc_SignalCount[static_cast<uint32_t>(e_ProtocolType)][this->mu32_InterfaceIndex]);

      this->m_UpdateInterfaceText(this->mu32_InterfaceIndex);
   }

   //Signal change
   Q_EMIT (this->SigChanged());

   this->mpc_Ui->pc_MessageSelectorWidget->RecheckProtocolError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger global error check (icons only)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_RecheckErrorGlobal(void) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->RecheckErrorGlobal();
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger recheck of error values for tree

   \param[in]  orc_MessageId  Message identification indices
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_RecheckError(const C_OscCanMessageIdentificationIndices & orc_MessageId) const
{
   this->mpc_Ui->pc_MessageSelectorWidget->RecheckError(orc_MessageId);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   If messages selected
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessagesSelected(void) const
{
   this->mpc_Ui->pc_MsgSigEditWidget->Hide();
   this->mpc_Ui->pc_MsgSigTableWidget->setVisible(true);
   //Update messages (All properties changes may be missing)
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On change of message count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageCountChanged(void)
{
   //Update messages
   this->mpc_Ui->pc_MsgSigTableWidget->UpdateData();

   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   // Update the message counter of the protocol/interface
   if (this->mq_LinkOnly == false)
   {
      const C_OscCanProtocol::E_Type e_ProtocolType = this->GetActProtocol();

      if (this->mq_ModeSingleNode == true)
      {
         this->mc_MessageCount[static_cast<uint32_t>(e_ProtocolType)][this->mu32_InterfaceIndex] =
            this->mc_MessageSyncManager.GetUniqueMessageCount(
               e_ProtocolType,
               &this->mc_SignalCount[static_cast<uint32_t>(e_ProtocolType)][this->mu32_InterfaceIndex]);
      }
      else
      {
         // In bus mode the signal count is not relevant
         this->mc_MessageCount[static_cast<uint32_t>(e_ProtocolType)][0] =
            this->mc_MessageSyncManager.GetUniqueMessageCount(e_ProtocolType);
      }

      // Update tab text
      this->m_UpdateTabText(e_ProtocolType);
   }

   // Update the interface combo box too
   if (this->mq_ModeSingleNode == true)
   {
      this->m_UpdateInterfaceText(this->mu32_InterfaceIndex);
   }

   //Signal change
   Q_EMIT (this->SigChanged());

   this->mpc_Ui->pc_MessageSelectorWidget->RecheckProtocolError();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Change of Rx configuration of message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnMessageRxChanged(void) const
{
   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register any change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnChange(void)
{
   this->mpc_Ui->pc_NodeSelectorWidget->Refresh();

   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_OscCanMessage> * C_SdBueComIfDescriptionWidget::m_PrepareMessageId(const uint32_t ou32_NodeIndex,
                                                                                       const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                                                                                       stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
const
{
   const std::vector<C_OscCanMessage> * pc_Messages = NULL;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscCanMessageContainer * pc_MessageContainer = NULL;
      uint32_t u32_Counter;

      orc_MessageId.u32_InterfaceIndex = ou32_ListIndex / 2;

      // search the com protocol
      for (u32_Counter = 0U; u32_Counter < pc_Node->c_ComProtocols.size(); ++u32_Counter)
      {
         if (ou32_DataPoolIndex == pc_Node->c_ComProtocols[u32_Counter].u32_DataPoolIndex)
         {
            orc_MessageId.e_ComProtocol = pc_Node->c_ComProtocols[u32_Counter].e_Type;

            // get the message container, we need it later
            if (orc_MessageId.u32_InterfaceIndex < pc_Node->c_ComProtocols[u32_Counter].c_ComMessages.size())
            {
               pc_MessageContainer =
                  &pc_Node->c_ComProtocols[u32_Counter].c_ComMessages[orc_MessageId.u32_InterfaceIndex];
            }
            break;
         }
      }

      if (pc_MessageContainer != NULL)
      {
         const C_OscNodeDataPoolList * const pc_OscList = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
            ou32_NodeIndex,
            ou32_DataPoolIndex,
            ou32_ListIndex);
         if (pc_OscList != NULL)
         {
            orc_MessageId.u32_NodeIndex = ou32_NodeIndex;
            orc_MessageId.u32_DatapoolIndex = ou32_DataPoolIndex;

            orc_MessageId.q_MessageIsTx = C_OscCanProtocol::h_ListIsComTx(*pc_OscList);

            // search the matching signal
            if (orc_MessageId.q_MessageIsTx == true)
            {
               pc_Messages = &pc_MessageContainer->c_TxMessages;
            }
            else
            {
               pc_Messages = &pc_MessageContainer->c_RxMessages;
            }
         }
      }
   }

   return pc_Messages;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal for switching to bus

   \param[in]  orc_Link    Link
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_OnLinkSwitchToBus(const QString & orc_Link) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if ((pc_Node != NULL) &&
       (this->mu32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
   {
      const C_OscNodeComInterfaceSettings & rc_ComInterface =
         pc_Node->c_Properties.c_ComInterfaces[this->mu32_InterfaceIndex];

      if (rc_ComInterface.GetBusConnected() == true)
      {
         const C_OscCanProtocol::E_Type e_Protocol = GetActProtocol();
         Q_EMIT (this->SigSwitchToBusProtocol(rc_ComInterface.u32_BusIndex, orc_Link, e_Protocol));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the text with protocol names, interface names and its unique CAN messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_UpdateText(void)
{
   // Update the counters for each protocol of this interface
   // and all interfaces with this protocol

   uint32_t u32_ProtocolCounter;
   const C_OscCanProtocol::E_Type e_CurrentProt = this->GetActProtocol();

   // For current interface
   for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
        ++u32_ProtocolCounter)
   {
      if (this->mq_ModeSingleNode == true)
      {
         if ((this->mq_LinkOnly == false) &&
             (this->mc_ProtocolUsedOnBus[u32_ProtocolCounter][this->mu32_InterfaceIndex] == false))
         {
            this->mc_MessageCount[u32_ProtocolCounter][this->mu32_InterfaceIndex] =
               this->mc_MessageSyncManager.GetUniqueMessageCount(
                  C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtocolCounter],
                  &this->mc_SignalCount[u32_ProtocolCounter][this->
                                                             mu32_InterfaceIndex]);
         }
         else
         {
            this->m_GetNodeMessageAndSignalCount(C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtocolCounter],
                                                 this->mu32_InterfaceIndex,
                                                 this->mc_MessageCount[u32_ProtocolCounter][this->mu32_InterfaceIndex],
                                                 this->mc_SignalCount[u32_ProtocolCounter][this->mu32_InterfaceIndex]);
         }
      }
      else
      {
         // In bus mode the signal count is not relevant
         this->mc_MessageCount[u32_ProtocolCounter][0] =
            this->mc_MessageSyncManager.GetUniqueMessageCount(C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtocolCounter]);
      }
   }

   if (this->mq_ModeSingleNode == true)
   {
      uint32_t u32_InterfaceCounter;
      // For the other interfaces and for each protocol.
      for (u32_InterfaceCounter = 0U;
           static_cast<int32_t>(u32_InterfaceCounter) < this->mpc_Ui->pc_CbInterface->count();
           ++u32_InterfaceCounter)
      {
         if (u32_InterfaceCounter != this->mu32_InterfaceIndex)
         {
            bool q_AllProtocolsUsedOnBus = true;
            bool q_AtLeastOneIsUsedOnBus = false;

            // Check if at least one protocol is not used on bus for using the sync manager in node mode
            for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
                 ++u32_ProtocolCounter)
            {
               q_AllProtocolsUsedOnBus = q_AllProtocolsUsedOnBus &&
                                         this->mc_ProtocolUsedOnBus[u32_ProtocolCounter][u32_InterfaceCounter];
               q_AtLeastOneIsUsedOnBus = q_AtLeastOneIsUsedOnBus ||
                                         this->mc_ProtocolUsedOnBus[u32_ProtocolCounter][u32_InterfaceCounter];
            }

            if (q_AllProtocolsUsedOnBus == false)
            {
               stw::opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager c_TempMessageSyncManager;
               c_TempMessageSyncManager.Init(this->mu32_NodeIndex, u32_InterfaceCounter, e_CurrentProt);

               // One initialization of the temporary sync manager can be used for all protocols
               for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
                    ++u32_ProtocolCounter)
               {
                  if (this->mc_ProtocolUsedOnBus[u32_ProtocolCounter][u32_InterfaceCounter] == false)
                  {
                     this->mc_MessageCount[u32_ProtocolCounter][u32_InterfaceCounter] =
                        c_TempMessageSyncManager.GetUniqueMessageCount(C_OscCanProtocol::hc_ALL_PROTOCOLS[
                                                                          u32_ProtocolCounter],
                                                                       &this->mc_SignalCount[u32_ProtocolCounter][
                                                                          u32_InterfaceCounter]);
                  }
               }
            }

            if (q_AtLeastOneIsUsedOnBus == true)
            {
               // Now get the count for each protocol on each interface which is used on a bus
               for (u32_ProtocolCounter = 0U; u32_ProtocolCounter < C_OscCanProtocol::hc_ALL_PROTOCOLS.size();
                    ++u32_ProtocolCounter)
               {
                  if (this->mc_ProtocolUsedOnBus[u32_ProtocolCounter][u32_InterfaceCounter] == true)
                  {
                     this->m_GetNodeMessageAndSignalCount(C_OscCanProtocol::hc_ALL_PROTOCOLS[u32_ProtocolCounter],
                                                          u32_InterfaceCounter,
                                                          this->mc_MessageCount[u32_ProtocolCounter][
                                                             u32_InterfaceCounter],
                                                          this->mc_SignalCount[u32_ProtocolCounter][u32_InterfaceCounter]);
                  }
               }
            }
         }
      }
   }

   this->m_UpdateTabText();

   if (this->mq_ModeSingleNode == true)
   {
      this->m_UpdateInterfaceText();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the tab text with protocol names and its unique CAN messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_UpdateTabText(void) const
{
   this->m_UpdateTabText(C_OscCanProtocol::eLAYER2);
   this->m_UpdateTabText(C_OscCanProtocol::eECES);
   this->m_UpdateTabText(C_OscCanProtocol::eCAN_OPEN_SAFETY);
   this->m_UpdateTabText(C_OscCanProtocol::eCAN_OPEN);
   this->m_UpdateTabText(C_OscCanProtocol::eJ1939);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the tab text with protocol name and its unique CAN messages

   \param[in]  oe_Protocol    Current protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_UpdateTabText(const C_OscCanProtocol::E_Type oe_Protocol) const
{
   QString c_Text;
   uint32_t u32_Count = 0U;

   if (this->mq_ModeSingleNode == false)
   {
      u32_Count = this->mc_MessageCount[static_cast<uint32_t>(oe_Protocol)][0U];
      // Only necessary, if the messages are managed on the node side
   }
   else
   {
      // Sum of all interfaces
      int32_t s32_IntfCounter;

      u32_Count = 0U;

      for (s32_IntfCounter = 0U; s32_IntfCounter < this->mpc_Ui->pc_CbInterface->count(); ++s32_IntfCounter)
      {
         u32_Count += this->mc_MessageCount[static_cast<uint32_t>(oe_Protocol)][s32_IntfCounter];
      }
   }

   // Protocol name
   c_Text = C_PuiSdUtil::h_ConvertProtocolTypeToString(oe_Protocol);
   c_Text += " (" + QString::number(u32_Count) + ")";

   this->mpc_Ui->pc_ProtocolTabWidget->setTabText(mh_GetIndexOfProtocol(oe_Protocol), c_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the text of the combo box for all interfaces
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_UpdateInterfaceText(void) const
{
   int32_t s32_Counter;

   for (s32_Counter = 0U; s32_Counter < this->mpc_Ui->pc_CbInterface->count(); ++s32_Counter)
   {
      this->m_UpdateInterfaceText(static_cast<uint32_t>(s32_Counter));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the text of the combo box for the specific interface

   \param[in]  ou32_InterfaceIndex  Interface index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_UpdateInterfaceText(const uint32_t ou32_InterfaceIndex) const
{
   if ((this->mq_ModeSingleNode == true) &&
       (static_cast<int32_t>(ou32_InterfaceIndex) < this->mpc_Ui->pc_CbInterface->count()) &&
       (ou32_InterfaceIndex < this->mc_InterfaceNames.size()))
   {
      QString c_Text = this->mc_InterfaceNames[ou32_InterfaceIndex];
      const uint32_t u32_Protocol = static_cast<uint32_t>(this->GetActProtocol());
      const uint32_t u32_MsgCount = this->mc_MessageCount[u32_Protocol][ou32_InterfaceIndex];
      const uint32_t u32_SigCount = this->mc_SignalCount[u32_Protocol][ou32_InterfaceIndex];
      c_Text += C_GtGetText::h_GetText(" - Messages: ") + QString::number(u32_MsgCount) +
                C_GtGetText::h_GetText(", Signals: ") + QString::number(u32_SigCount);

      if (this->mc_ProtocolUsedOnBus[u32_Protocol][ou32_InterfaceIndex] == true)
      {
         c_Text += C_GtGetText::h_GetText(" (used on ") + this->mc_BusNames[ou32_InterfaceIndex] + ")";
      }

      this->mpc_Ui->pc_CbInterface->setItemText(ou32_InterfaceIndex, c_Text);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the message and signal count of a node without the sync manager direct from the node

   \param[in]   oe_Protocol            Protocol type
   \param[in]   ou32_InterfaceIndex    Interface of node
   \param[out]  oru32_MessageCount     Detected message count of node
   \param[out]  oru32_SignalCount      Detected signal count of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueComIfDescriptionWidget::m_GetNodeMessageAndSignalCount(const C_OscCanProtocol::E_Type oe_Protocol,
                                                                   const uint32_t ou32_InterfaceIndex,
                                                                   uint32_t & oru32_MessageCount,
                                                                   uint32_t & oru32_SignalCount) const
{
   uint32_t u32_RxMessageCount = 0U;
   uint32_t u32_TxMessageCount = 0U;

   oru32_SignalCount = 0U;

   if ((oe_Protocol != C_OscCanProtocol::eCAN_OPEN) ||
       (this->mq_IsCoDevice == false))
   {
      // Standard case: Use the current node information
      if (C_SdUtil::h_GetMessageCountOfNode(this->mu32_NodeIndex, ou32_InterfaceIndex, oe_Protocol, NULL,
                                            u32_RxMessageCount,
                                            u32_TxMessageCount, &oru32_SignalCount) != C_NO_ERR)
      {
         tgl_assert(false);
      }
   }
   else if (ou32_InterfaceIndex == this->mu32_CoDeviceIntfIndex)
   {
      // Special case CANopen: Node is a CANopen device, the messages are only detectable at the manager
      const C_OscNode * const pc_Node  = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
         this->mu32_CoManagerNodeIndexOfCoDevice);

      tgl_assert(pc_Node != NULL);
      if (pc_Node != NULL)
      {
         uint32_t u32_UsedInterfaceIndex = 0U;
         uint32_t u32_IntfCounter;
         const C_OscCanInterfaceId * pc_CoDeviceInterfaceId = NULL;

         // Getting the correct interface index of the CANopen Manager
         for (u32_IntfCounter = 0U; u32_IntfCounter < pc_Node->c_Properties.c_ComInterfaces.size(); ++u32_IntfCounter)
         {
            if ((pc_Node->c_Properties.c_ComInterfaces[u32_IntfCounter].e_InterfaceType == C_OscSystemBus::eCAN) &&
                (pc_Node->c_Properties.c_ComInterfaces[u32_IntfCounter].u8_InterfaceNumber ==
                 this->mu8_CoManagerNodeIndexIntfNumber))
            {
               u32_UsedInterfaceIndex = u32_IntfCounter;
               break;
            }
         }

         if (this->mq_IsCoDevice == true)
         {
            // In case of CANopen Device the interface id is for identification necessary
            pc_CoDeviceInterfaceId = &this->mc_CoDeviceInterfaceId;
         }
         if (C_SdUtil::h_GetMessageCountOfNode(this->mu32_CoManagerNodeIndexOfCoDevice, u32_UsedInterfaceIndex,
                                               oe_Protocol, pc_CoDeviceInterfaceId,
                                               u32_RxMessageCount,
                                               u32_TxMessageCount, &oru32_SignalCount) != C_NO_ERR)
         {
            tgl_assert(false);
         }
      }
   }
   else
   {
      // Special case CANopen: Node is device and the current interface is the not used interface to the CANopen
      // manager. In this case, no messages and signals can be here, no counting
      // So... nothing to do
   }

   oru32_MessageCount = u32_RxMessageCount + u32_TxMessageCount;
}
