//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to display one data pool list com header (implementation)

   Widget to display one data pool list com header

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPainter>
#include "C_SdNdeDataPoolListComHeaderWidget.h"
#include "ui_C_SdNdeDataPoolListComHeaderWidget.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_Uti.h"
#include "stwerrors.h"
#include "constants.h"
#include "C_CieDataPoolListStructure.h"
#include "C_CieUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_NodeIndex          Node index
   \param[in]     ou32_DataPoolIndex      Data pool index
   \param[in]     ou32_ListIndex          List index
   \param[in]     orq_UseAlternatingColor Flag for has alternative color
   \param[in,out] opc_Parent              Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDataPoolListComHeaderWidget::C_SdNdeDataPoolListComHeaderWidget(const uint32 ou32_NodeIndex,
                                                                       const uint32 ou32_DataPoolIndex,
                                                                       const uint32 ou32_ListIndex,
                                                                       const bool & orq_UseAlternatingColor,
                                                                       QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDataPoolListComHeaderWidget),
   mu32_NodeIndex(ou32_NodeIndex),
   mu32_DataPoolIndex(ou32_DataPoolIndex),
   mu32_ListIndex(ou32_ListIndex),
   mq_Connected(false)
{
   mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_LabelListError->setPixmap(QIcon("://images/Error_iconV2.svg").pixmap(mc_ICON_SIZE_24));
   this->mpc_Ui->pc_LabelListError->setVisible(false);

   m_UpdateBusLink();

   InitStaticNames();

   UpdateMessagesAndSignals();

   //Deactivate debug label
   this->mpc_Ui->pc_GroupBoxConnection->setTitle("");
   this->mpc_Ui->pc_GroupBoxMessages->setTitle("");
   this->mpc_Ui->pc_GroupBoxSignals->setTitle("");

   this->setMinimumHeight(66);
   this->setMaximumHeight(66);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "HasAlternatingBackground", orq_UseAlternatingColor);

   //Links
   this->mpc_Ui->pc_LabelBusLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
   this->mpc_Ui->pc_LabelBusLink->setFocusPolicy(Qt::NoFocus);
   this->mpc_Ui->pc_LabelBusLink->setContextMenuPolicy(Qt::NoContextMenu);
   connect(this->mpc_Ui->pc_LabelBusLink, &QLabel::linkActivated, this,
           &C_SdNdeDataPoolListComHeaderWidget::m_OnLinkSwitchToBus);

   // edit
   this->mpc_Ui->pc_PushButtonEdit->SetCustomIcons("://images/IconEditActive.svg", "://images/IconEditHovered.svg",
                                                   "://images/IconEditClicked.svg",
                                                   "://images/IconEditDisabledBright.svg");
   connect(this->mpc_Ui->pc_PushButtonEdit, &QPushButton::clicked, this,
           &C_SdNdeDataPoolListComHeaderWidget::m_OnLinkEdit);

   // file import
   this->mpc_Ui->pc_PushButtonImport->SetCustomIcons("://images/IconImportFile.svg",  // enabled
                                                     "://images/IconImportFile.svg",  // TODO: hovered
                                                     "://images/IconImportFile.svg",  // TODO: clicked
                                                     "://images/IconImportFile.svg"); // TODO: disabled
   connect(this->mpc_Ui->pc_PushButtonImport, &QPushButton::clicked, this,
           &C_SdNdeDataPoolListComHeaderWidget::m_OnLinkFileImport);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDataPoolListComHeaderWidget::~C_SdNdeDataPoolListComHeaderWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::InitStaticNames(void)
{
   this->mpc_Ui->pc_LabelConnected->setText(C_GtGetText::h_GetText("Used on: "));
   this->mpc_Ui->pc_LabelNotConnected->setText(C_GtGetText::h_GetText("Not used"));

   //Tool tips
   this->mpc_Ui->pc_PushButtonImport->SetToolTipInformation(C_GtGetText::h_GetText("Import"),
                                                            C_GtGetText::h_GetText(
                                                               "Import messages and signals from standard file formats (*.dbc, *.eds, *.dcf)"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get error active flag

   \return
   True  Error active
   False Error inactive
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDataPoolListComHeaderWidget::GetErrorActive(void) const
{
   return this->mpc_Ui->pc_LabelListError->isVisible();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update message and signals label
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::UpdateMessagesAndSignals(void) const
{
   const C_OSCNodeDataPoolList * const pc_DataPoolList1 = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex,
      this->mu32_DataPoolIndex,
      this->mu32_ListIndex);
   const C_OSCNodeDataPoolList * const pc_DataPoolList2 = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
      this->mu32_NodeIndex,
      this->mu32_DataPoolIndex,
      this->mu32_ListIndex + 1);

   if ((pc_DataPoolList1 != NULL) && (pc_DataPoolList2 != NULL))
   {
      const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
      const uint32 u32_SignalSize1 = pc_DataPoolList1->c_Elements.size();
      const uint32 u32_SignalSize2 = pc_DataPoolList2->c_Elements.size();
      const uint32 u32_SignalSize = u32_SignalSize1 + u32_SignalSize2;

      if (pc_Node != NULL)
      {
         const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetRelatedCANProtocolConst(this->mu32_DataPoolIndex);
         if (pc_Protocol != NULL)
         {
            const uint32 u32_InterfaceIndex = this->mu32_ListIndex / 2;
            if ((u32_InterfaceIndex < pc_Protocol->c_ComMessages.size()) &&
                (u32_InterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size()))
            {
               const C_OSCNodeComInterfaceSettings & rc_Interface =
                  pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex];
               const C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[u32_InterfaceIndex];
               std::vector<uint32> c_InvalidTxMessages;
               std::vector<uint32> c_InvalidRxMessages;
               bool q_Error;
               const uint32 u32_RxSize = rc_MessageContainer.c_RxMessages.size();
               const uint32 u32_TxSize = rc_MessageContainer.c_TxMessages.size();
               //Update label
               if ((u32_RxSize > 0U) ||
                   (u32_TxSize > 0U))
               {
                  this->mpc_Ui->pc_LabelMessage->setText(QString(C_GtGetText::h_GetText(
                                                                    "Messages: %1 Tx, %2 Rx")).arg(
                                                            QString::number(u32_TxSize),
                                                            QString::number(u32_RxSize)));
               }
               else
               {
                  this->mpc_Ui->pc_LabelMessage->setText(C_GtGetText::h_GetText("No Messages"));
               }
               //Check if error relevant
               if ((rc_MessageContainer.q_IsComProtocolUsedByInterface == true) &&
                   (rc_Interface.q_IsBusConnected == true))
               {
                  q_Error = false;
               }
               else
               {
                  //Error check
                  if (C_OSCCanProtocol::h_ListIsComTx(*pc_DataPoolList1) == true)
                  {
                     q_Error = rc_MessageContainer.CheckLocalError(*pc_DataPoolList1, *pc_DataPoolList2, C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                                                      pc_Protocol->e_Type), &c_InvalidTxMessages,
                                                                   &c_InvalidRxMessages);
                  }
                  else
                  {
                     q_Error = rc_MessageContainer.CheckLocalError(*pc_DataPoolList2, *pc_DataPoolList1, C_OSCCanProtocol::h_GetCANMessageValidSignalsDLCOffset(
                                                                      pc_Protocol->e_Type), &c_InvalidTxMessages,
                                                                   &c_InvalidRxMessages);
                  }
               }
               if (q_Error == true)
               {
                  QString c_ErrorText;
                  this->mpc_Ui->pc_LabelListError->setVisible(true);
                  //Add tool tip
                  if (c_InvalidTxMessages.size() > 0UL)
                  {
                     c_ErrorText += C_GtGetText::h_GetText("Invalid TX messages:\n");
                     for (uint32 u32_ItMsg = 0;
                          (u32_ItMsg < c_InvalidTxMessages.size()) &&
                          (u32_ItMsg < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                          ++u32_ItMsg)
                     {
                        if (c_InvalidTxMessages[u32_ItMsg] < rc_MessageContainer.c_TxMessages.size())
                        {
                           const C_OSCCanMessage & rc_CurMessage =
                              rc_MessageContainer.c_TxMessages[c_InvalidTxMessages[u32_ItMsg]];
                           c_ErrorText += QString("%1\n").arg(rc_CurMessage.c_Name.c_str());
                        }
                     }
                     if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidTxMessages.size())
                     {
                        c_ErrorText += QString("+%1\n").arg(
                           c_InvalidTxMessages.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
                     }
                  }
                  if (c_InvalidRxMessages.size() > 0UL)
                  {
                     c_ErrorText += C_GtGetText::h_GetText("Invalid RX messages:\n");
                     for (uint32 u32_ItMsg = 0;
                          (u32_ItMsg < c_InvalidRxMessages.size()) &&
                          (u32_ItMsg < mu32_TOOL_TIP_MAXIMUM_ITEMS);
                          ++u32_ItMsg)
                     {
                        if (c_InvalidRxMessages[u32_ItMsg] < rc_MessageContainer.c_RxMessages.size())
                        {
                           const C_OSCCanMessage & rc_CurMessage =
                              rc_MessageContainer.c_RxMessages[c_InvalidRxMessages[u32_ItMsg]];
                           c_ErrorText += QString("%1\n").arg(rc_CurMessage.c_Name.c_str());
                        }
                     }
                     if (mu32_TOOL_TIP_MAXIMUM_ITEMS < c_InvalidRxMessages.size())
                     {
                        c_ErrorText += QString("+%1\n").arg(
                           c_InvalidRxMessages.size() - mu32_TOOL_TIP_MAXIMUM_ITEMS);
                     }
                  }
                  this->mpc_Ui->pc_LabelListError->SetToolTipInformation(C_GtGetText::h_GetText(
                                                                            "Invalid content:"), c_ErrorText,
                                                                         C_NagToolTip::eERROR);
               }
               else
               {
                  this->mpc_Ui->pc_LabelListError->setVisible(false);
               }
               Q_EMIT this->SigErrorChange();
            }
         }
      }
      if (u32_SignalSize > 0U)
      {
         this->mpc_Ui->pc_LabelSignals->setText(QString(C_GtGetText::h_GetText("Signals: %1")).arg(u32_SignalSize));
      }
      else
      {
         this->mpc_Ui->pc_LabelSignals->setText(C_GtGetText::h_GetText("No Signals"));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Manually draw background

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);

   //draw background
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this, &c_Painter);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update bus link & edit button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::m_UpdateBusLink(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetRelatedCANProtocolConst(this->mu32_DataPoolIndex);
      const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
         this->mu32_NodeIndex,
         this->mu32_DataPoolIndex);
      const C_OSCNodeDataPoolList * const pc_DataPoolList = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolList(
         this->mu32_NodeIndex,
         this->mu32_DataPoolIndex,
         this->mu32_ListIndex);

      if ((pc_DataPoolList != NULL) && (pc_DataPool != NULL))
      {
         QString c_Name = pc_DataPoolList->c_Name.c_str();
         //Cut of _TX
         c_Name.chop(3);
         if (pc_Protocol != NULL)
         {
            //Add data pool type
            switch (pc_Protocol->e_Type)
            {
            case C_OSCCanProtocol::eLAYER2:
               c_Name += " (OSI Layer 2)";
               break;
            case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
               c_Name += " (ECoS)";
               break;
            case C_OSCCanProtocol::eECES:
               c_Name += " (ECeS)";
               break;
            default:
               //Unknown
               break;
            }
         }
         this->mpc_Ui->pc_LabelName->setText(c_Name);
      }
      if ((pc_DataPool != NULL) && (pc_Protocol != NULL))
      {
         const uint32 u32_ComInterfaceIndex = this->mu32_ListIndex / 2U;
         if ((pc_Node->c_Properties.c_ComInterfaces.size() > 0) &&
             (u32_ComInterfaceIndex < pc_Protocol->c_ComMessages.size()))
         {
            if (u32_ComInterfaceIndex < pc_Node->c_Properties.c_ComInterfaces.size())
            {
               const C_OSCNodeComInterfaceSettings & rc_ComInterface =
                  pc_Node->c_Properties.c_ComInterfaces[u32_ComInterfaceIndex];
               const C_OSCCanMessageContainer & rc_MessageContainer = pc_Protocol->c_ComMessages[u32_ComInterfaceIndex];
               this->mq_Connected = rc_MessageContainer.q_IsComProtocolUsedByInterface &&
                                    (rc_ComInterface.q_IsBusConnected == true);
               if (this->mq_Connected == true)
               {
                  const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
                     rc_ComInterface.u32_BusIndex);
                  this->mpc_Ui->pc_LabelBusLink->setVisible(true);
                  this->mpc_Ui->pc_LabelConnected->setVisible(true);
                  this->mpc_Ui->pc_LabelNotConnected->setVisible(false);
                  if (pc_Bus != NULL)
                  {
                     this->mpc_Ui->pc_LabelBusLink->setText(C_Uti::h_GetLink(pc_Bus->c_Name.c_str()));
                  }
                  this->mpc_Ui->pc_PushButtonEdit->setEnabled(false);
                  //Dynamic tool tip
                  this->mpc_Ui->pc_LabelBusLink->SetToolTipInformation(QString(C_GtGetText::h_GetText(
                                                                                  "Navigate to %1")).arg(
                                                                          pc_Bus->c_Name.c_str()),
                                                                       C_GtGetText::h_GetText(
                                                                          "Edit COMM interface description on bus level"));
                  this->mpc_Ui->pc_PushButtonEdit->SetToolTipInformation(C_GtGetText::h_GetText("Edit"),
                                                                         C_GtGetText::h_GetText(
                                                                            "The option \"Edit\" is disabled for the "
                                                                            "node due to the reason that the interface "
                                                                            "is already used on a bus.\n"
                                                                            "Click \"Used on\" to navigate to the bus. "
                                                                            "COMM interface description can be edited "
                                                                            "on bus level."));
               }
               else
               {
                  this->mpc_Ui->pc_LabelBusLink->setVisible(false);
                  this->mpc_Ui->pc_LabelConnected->setVisible(false);
                  this->mpc_Ui->pc_LabelNotConnected->setVisible(true);
                  this->mpc_Ui->pc_PushButtonEdit->setEnabled(true);
                  //Dynamic tool tip
                  this->mpc_Ui->pc_PushButtonEdit->SetToolTipInformation(C_GtGetText::h_GetText("Edit"),
                                                                         C_GtGetText::h_GetText(
                                                                            "Edit COMM interface description"));
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::m_OnLinkEdit(void)
{
   Q_EMIT this->SigEdit(this->mu32_DataPoolIndex, this->mu32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::m_OnLinkSwitchToBus(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const uint32 u32_InterfaceIndex = this->mu32_ListIndex / 2;

      if (pc_Node->c_Properties.c_ComInterfaces.size() > u32_InterfaceIndex)
      {
         const C_OSCNodeComInterfaceSettings & rc_ComInterface =
            pc_Node->c_Properties.c_ComInterfaces[u32_InterfaceIndex];
         if (rc_ComInterface.q_IsBusConnected == true)
         {
            const C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
               rc_ComInterface.u32_BusIndex);
            if (pc_Bus != NULL)
            {
               Q_EMIT this->SigSwitchToBus(rc_ComInterface.u32_BusIndex, pc_Bus->c_Name.c_str());
            }
         }
      }
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDataPoolListComHeaderWidget::m_OnLinkFileImport(void)
{
   sint32 s32_Return;

   s32_Return = C_CieUtil::h_ImportFile(this->mu32_NodeIndex,
                                        this->mu32_DataPoolIndex,
                                        (this->mu32_ListIndex / 2), // divide by factor 2 because of Tx/Rx
                                        this->parentWidget()->parentWidget()->parentWidget()->parentWidget()->
                                        parentWidget()); // parent of parent of ... because of style issues
   if (s32_Return == C_NO_ERR)
   {
      this->UpdateMessagesAndSignals();
   }
}
