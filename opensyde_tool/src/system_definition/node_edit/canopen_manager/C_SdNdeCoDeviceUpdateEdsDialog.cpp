//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for updating the EDS file for any existing device

   Pop up dialog widget for updating the EDS file for any existing device

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "stwerrors.hpp"
#include "C_CieUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_CieImportReportWidget.hpp"
#include "C_SdNdeCoDeviceUpdateEdsDialog.hpp"
#include "ui_C_SdNdeCoDeviceUpdateEdsDialog.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
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

   \param[in,out]  orc_Parent                   Reference to parent
   \param[in]      ou32_ManagerNodeIndex        Manager node index
   \param[in]      ou8_ManagerInterfaceId       Manager interface id
   \param[in]      orc_DeviceNodeId             Device node id
   \param[in]      orc_FilePath                 File path
   \param[in]      orc_NewConfig                New config
   \param[in]      orc_NodeAssignment           Node assignment
   \param[in]      orc_InvalidNodeAssignment    Invalid node assignment
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoDeviceUpdateEdsDialog::C_SdNdeCoDeviceUpdateEdsDialog(
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ManagerNodeIndex,
   const uint8_t ou8_ManagerInterfaceId, const C_OscCanInterfaceId & orc_DeviceNodeId, const QString & orc_FilePath,
   const C_OscCanOpenManagerDeviceInfo & orc_NewConfig, const C_CieImportDataAssignment & orc_NodeAssignment,
   const C_CieImportDataAssignment & orc_InvalidNodeAssignment) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoDeviceUpdateEdsDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_ManagerNodeIndex(ou32_ManagerNodeIndex),
   mu8_ManagerInterfaceId(ou8_ManagerInterfaceId),
   mc_DeviceNodeId(orc_DeviceNodeId),
   mc_FilePath(orc_FilePath),
   mc_NewConfig(orc_NewConfig),
   mc_ImportedNodeAssignment(orc_NodeAssignment),
   mc_ImportedInvalidNodeAssignment(orc_InvalidNodeAssignment)
{
   const QFileInfo c_FileInfo(orc_FilePath);

   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   this->mpc_Ui->pc_TextEditFlash_2->setReadOnly(true);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set main title
   this->mrc_ParentDialog.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("%1 File Update")).arg(c_FileInfo.
                                                                                                      completeSuffix().
                                                                                                      toUpper()));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));

   //Before data processing
   m_InitMessageNames(this->mc_ImportedNodeAssignment);

   m_SetCommonDefaultData(this->mc_ImportedNodeAssignment);
   m_SetCommonDefaultData(this->mc_ImportedInvalidNodeAssignment);

   //Check
   this->mc_ImportedNodeAssignment.CheckInternalConsistency();
   this->mc_ImportedInvalidNodeAssignment.CheckInternalConsistency();

   this->m_CompareData();

   m_SetAllDefaultData(this->mc_NewNodeAssignment);
   m_SetAllDefaultData(this->mc_AdaptedNodeAssignment);
   m_SetAllDefaultData(this->mc_DeletedNodeAssignment);

   //Check
   this->mc_NewNodeAssignment.CheckInternalConsistency();
   this->mc_AdaptedNodeAssignment.CheckInternalConsistency();
   this->mc_DeletedNodeAssignment.CheckInternalConsistency();

   //content
   this->m_ShowReport();

   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SdNdeCoDeviceUpdateEdsDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked, this,
           &C_SdNdeCoDeviceUpdateEdsDialog::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoDeviceUpdateEdsDialog::~C_SdNdeCoDeviceUpdateEdsDialog(void) noexcept
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Do apply data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::DoApplyData(void) const
{
   const std::vector<C_CieImportDataAssignment> c_NewNodeAssignmentVector = {this->mc_NewNodeAssignment};
   const std::vector<C_CieImportDataAssignment> c_AdaptedNodeAssignmentVector = {this->mc_AdaptedNodeAssignment};
   const C_OscCanOpenManagerDeviceInfo c_AdaptedConfig = this->m_AdaptConfig(this->mc_NewConfig);

   tgl_assert(C_PuiSdHandler::h_GetInstance()->ReplaceCanOpenManagerDevice(this->mu32_ManagerNodeIndex,
                                                                           this->
                                                                           mu8_ManagerInterfaceId,
                                                                           this->mc_DeviceNodeId,
                                                                           c_AdaptedConfig) == C_NO_ERR);
   tgl_assert(C_PuiSdHandler::h_GetInstance()->DeleteCanOpenManagerMessages(this->mu32_ManagerNodeIndex,
                                                                            this->
                                                                            mu8_ManagerInterfaceId,
                                                                            this->mc_DeviceNodeId) == C_NO_ERR);
   C_CieUtil::h_InsertMessages(c_NewNodeAssignmentVector, C_OscCanProtocol::eCAN_OPEN, false);
   C_CieUtil::h_InsertMessages(c_AdaptedNodeAssignmentVector, C_OscCanProtocol::eCAN_OPEN, false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set all default data

   \param[in,out]  orc_NodeAssignment  Node assignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_SetAllDefaultData(C_CieImportDataAssignment & orc_NodeAssignment)
{
   uint32_t u32_ManagerInterfaceIndex;

   this->m_SetCommonDefaultData(orc_NodeAssignment);
   orc_NodeAssignment.u32_OsyNodeIndex = this->mu32_ManagerNodeIndex;
   if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceNumberToIndex(this->mu32_ManagerNodeIndex,
                                                                           this->mu8_ManagerInterfaceId,
                                                                           u32_ManagerInterfaceIndex) == C_NO_ERR)
   {
      orc_NodeAssignment.u32_OsyInterfaceIndex = u32_ManagerInterfaceIndex;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set common default data

   \param[in,out]  orc_NodeAssignment  Node assignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_SetCommonDefaultData(C_CieImportDataAssignment & orc_NodeAssignment)
{
   orc_NodeAssignment.u32_NewMessageCount =
      static_cast<uint32_t>(orc_NodeAssignment.c_ImportData.c_Core.c_OscRxMessageData.size()) +
      static_cast<uint32_t>(orc_NodeAssignment.c_ImportData.c_Core.
                            c_OscTxMessageData.size());
   orc_NodeAssignment.c_RxMessageOverrideIndices.resize(
      orc_NodeAssignment.c_ImportData.c_Core.c_OscRxMessageData.size(),
      std::pair<int32_t, int32_t>(-1, -1));
   orc_NodeAssignment.c_TxMessageOverrideIndices.resize(
      orc_NodeAssignment.c_ImportData.c_Core.c_OscTxMessageData.size(),
      std::pair<int32_t, int32_t>(-1, -1));
   orc_NodeAssignment.s32_DatapoolIndexForNew = this->m_GetDataPoolIndex();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show report
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_ShowReport()
{
   const QFileInfo c_FileInfo(this->mc_FilePath);
   const std::vector<C_CieImportDataAssignment> c_NodeAssignmentVector = {this->mc_ImportedNodeAssignment};

   QString c_Report = C_CieImportReportWidget::h_GetInitialReportPart(c_NodeAssignmentVector, this->mc_FilePath,
                                                                      true,
                                                                      this->mc_ImportedNodeAssignment.u32_NewMessageCount >
                                                                      0UL);

   C_SdNdeCoDeviceUpdateEdsDialog::mh_AddToReport(C_GtGetText::h_GetText("New Messages"),
                                                  c_FileInfo, this->mc_NewNodeAssignment, c_Report);
   C_SdNdeCoDeviceUpdateEdsDialog::mh_AddToReport(C_GtGetText::h_GetText("Adapted Messages"),
                                                  c_FileInfo, this->mc_AdaptedNodeAssignment, c_Report);
   C_SdNdeCoDeviceUpdateEdsDialog::mh_AddToReport(C_GtGetText::h_GetText("Deleted Messages"),
                                                  c_FileInfo, this->mc_DeletedNodeAssignment, c_Report);
   C_SdNdeCoDeviceUpdateEdsDialog::mh_AddToReport(C_GtGetText::h_GetText("Skipped Messages"),
                                                  c_FileInfo, this->mc_ImportedInvalidNodeAssignment, c_Report);

   this->mpc_Ui->pc_TextEditFlash_2->setHtml(c_Report);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add to report

   \param[in]      orc_Heading   Heading
   \param[in]      orc_FileInfo  File info
   \param[in]      orc_Data      Data
   \param[in,out]  orc_Report    Report
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::mh_AddToReport(const QString & orc_Heading, const QFileInfo & orc_FileInfo,
                                                    const C_CieImportDataAssignment & orc_Data, QString & orc_Report)
{
   QString c_TableContent;

   std::vector<C_CieImportDataAssignment> c_DataVector = {orc_Data};
   if (C_CieImportReportWidget::h_GetMessageTableContent(c_TableContent, orc_FileInfo.
                                                         completeSuffix(), c_DataVector,
                                                         C_OscCanProtocol::eCAN_OPEN, NULL, orc_Heading,
                                                         false) == C_NO_ERR)
   {
      orc_Report += c_TableContent;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init new data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_CompareData()
{
   C_CieImportedMessageVectorData c_ExistingRxMessages;
   C_CieImportedMessageVectorData c_ExistingTxMessages;
   C_CieImportedMessageVectorData c_ImportedRxMessages;
   C_CieImportedMessageVectorData c_ImportedTxMessages;
   C_CieImportedMessageVectorData c_NewRxMessages;
   C_CieImportedMessageVectorData c_NewTxMessages;
   C_CieImportedMessageVectorData c_AdaptedRxMessages;
   C_CieImportedMessageVectorData c_AdaptedTxMessages;
   C_CieImportedMessageVectorData c_DeletedRxMessages;
   C_CieImportedMessageVectorData c_DeletedTxMessages;

   if (this->m_PrepareCompareData(c_ExistingRxMessages, c_ExistingTxMessages, c_ImportedRxMessages,
                                  c_ImportedTxMessages, c_NewRxMessages, c_NewTxMessages,
                                  c_AdaptedRxMessages, c_AdaptedTxMessages, c_DeletedRxMessages,
                                  c_DeletedTxMessages) == C_NO_ERR)
   {
      //Compare
      this->m_CompareDataMessageVector(c_ExistingRxMessages,
                                       c_ImportedRxMessages,
                                       c_NewRxMessages,
                                       c_AdaptedRxMessages,
                                       c_DeletedRxMessages,
                                       true);
      this->m_CompareDataMessageVector(c_ExistingTxMessages,
                                       c_ImportedTxMessages,
                                       c_NewTxMessages,
                                       c_AdaptedTxMessages,
                                       c_DeletedTxMessages,
                                       false);
      //write back
      this->m_WriteComparedData(c_NewRxMessages, c_NewTxMessages, c_AdaptedRxMessages, c_AdaptedTxMessages,
                                c_DeletedRxMessages,
                                c_DeletedTxMessages);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Prepare compare data

   \param[in,out]  orc_ExistingRxMessages    Existing rx messages
   \param[in,out]  orc_ExistingTxMessages    Existing tx messages
   \param[in,out]  orc_ImportedRxMessages    Imported rx messages
   \param[in,out]  orc_ImportedTxMessages    Imported tx messages
   \param[in,out]  orc_NewRxMessages         New rx messages
   \param[in,out]  orc_NewTxMessages         New tx messages
   \param[in,out]  orc_AdaptedRxMessages     Adapted rx messages
   \param[in,out]  orc_AdaptedTxMessages     Adapted tx messages
   \param[in,out]  orc_DeletedRxMessages     Deleted rx messages
   \param[in,out]  orc_DeletedTxMessages     Deleted tx messages

   \return
   STW error codes

   \retval   C_NO_ERR   All data initialized
   \retval   C_CONFIG   Could not initialize
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoDeviceUpdateEdsDialog::m_PrepareCompareData(C_CieImportedMessageVectorData & orc_ExistingRxMessages,
                                                             C_CieImportedMessageVectorData & orc_ExistingTxMessages,
                                                             C_CieImportedMessageVectorData & orc_ImportedRxMessages,
                                                             C_CieImportedMessageVectorData & orc_ImportedTxMessages,
                                                             C_CieImportedMessageVectorData & orc_NewRxMessages,
                                                             C_CieImportedMessageVectorData & orc_NewTxMessages,
                                                             C_CieImportedMessageVectorData & orc_AdaptedRxMessages,
                                                             C_CieImportedMessageVectorData & orc_AdaptedTxMessages,
                                                             C_CieImportedMessageVectorData & orc_DeletedRxMessages,
                                                             C_CieImportedMessageVectorData & orc_DeletedTxMessages)
{
   int32_t s32_Retval = C_NO_ERR;
   uint32_t u32_ManagerInterfaceIndex;

   if (C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceNumberToIndex(this->mu32_ManagerNodeIndex,
                                                                           this->mu8_ManagerInterfaceId,
                                                                           u32_ManagerInterfaceIndex) == C_NO_ERR)
   {
      const int32_t s32_DataPoolIndex = this->m_GetDataPoolIndex();
      const C_OscCanMessageContainer * const pc_OscContainer =
         C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(this->mu32_ManagerNodeIndex,
                                                                         C_OscCanProtocol::eCAN_OPEN,
                                                                         u32_ManagerInterfaceIndex,
                                                                         static_cast<
                                                                            uint32_t>(s32_DataPoolIndex));
      const C_PuiSdNodeCanProtocol * const pc_UiContainer = C_PuiSdHandler::h_GetInstance()->GetUiCanProtocolConst(
         this->mu32_ManagerNodeIndex,
         C_OscCanProtocol::eCAN_OPEN,
         static_cast<
            uint32_t>(
            s32_DataPoolIndex));
      const C_OscNodeDataPoolList * const pc_OscTxList = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolList(
         this->mu32_ManagerNodeIndex,
         C_OscCanProtocol::eCAN_OPEN,
         u32_ManagerInterfaceIndex,
         static_cast<
            uint32_t>(
            s32_DataPoolIndex),
         true);
      const C_OscNodeDataPoolList * const pc_OscRxList = C_PuiSdHandler::h_GetInstance()->GetOscCanDataPoolList(
         this->mu32_ManagerNodeIndex,
         C_OscCanProtocol::eCAN_OPEN,
         u32_ManagerInterfaceIndex,
         static_cast<
            uint32_t>(
            s32_DataPoolIndex),
         false);
      const C_PuiSdNodeDataPoolList * const pc_UiTxList = C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolList(
         this->mu32_ManagerNodeIndex,
         C_OscCanProtocol::eCAN_OPEN,
         u32_ManagerInterfaceIndex,
         static_cast<
            uint32_t>(
            s32_DataPoolIndex),
         true);
      const C_PuiSdNodeDataPoolList * const pc_UiRxList = C_PuiSdHandler::h_GetInstance()->GetUiCanDataPoolList(
         this->mu32_ManagerNodeIndex,
         C_OscCanProtocol::eCAN_OPEN,
         u32_ManagerInterfaceIndex,
         static_cast<
            uint32_t>(
            s32_DataPoolIndex),
         false);
      tgl_assert(s32_DataPoolIndex >= 0);
      if ((pc_OscContainer != NULL) && (pc_UiContainer != NULL) && (pc_OscRxList != NULL) && (pc_OscTxList != NULL) &&
          (pc_UiTxList != NULL) && (pc_UiRxList != NULL) &&
          (u32_ManagerInterfaceIndex < pc_UiContainer->c_ComMessages.size()))
      {
         const C_PuiSdNodeCanMessageContainer & rc_UiContainer =
            pc_UiContainer->c_ComMessages[u32_ManagerInterfaceIndex];
         //Init
         orc_NewRxMessages = C_CieImportedMessageVectorData(
            this->mc_NewNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData,
            this->mc_NewNodeAssignment.c_ImportData.c_Core.c_OscRxSignalData,
            this->mc_NewNodeAssignment.c_ImportData.c_Ui.c_UiRxMessageData,
            this->mc_NewNodeAssignment.c_ImportData.c_Ui.c_UiRxSignalData,
            &this->mc_NewNodeAssignment.c_ImportData.c_Core.c_WarningMessagesPerRxMessage);
         orc_AdaptedRxMessages = C_CieImportedMessageVectorData(
            this->mc_AdaptedNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData,
            this->mc_AdaptedNodeAssignment.c_ImportData.c_Core.c_OscRxSignalData,
            this->mc_AdaptedNodeAssignment.c_ImportData.c_Ui.c_UiRxMessageData,
            this->mc_AdaptedNodeAssignment.c_ImportData.c_Ui.c_UiRxSignalData,
            &this->mc_AdaptedNodeAssignment.c_ImportData.c_Core.c_WarningMessagesPerRxMessage);
         orc_DeletedRxMessages = C_CieImportedMessageVectorData(
            this->mc_DeletedNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData,
            this->mc_DeletedNodeAssignment.c_ImportData.c_Core.c_OscRxSignalData,
            this->mc_DeletedNodeAssignment.c_ImportData.c_Ui.c_UiRxMessageData,
            this->mc_DeletedNodeAssignment.c_ImportData.c_Ui.c_UiRxSignalData,
            &this->mc_DeletedNodeAssignment.c_ImportData.c_Core.c_WarningMessagesPerRxMessage);
         orc_NewTxMessages = C_CieImportedMessageVectorData(this->mc_NewNodeAssignment.c_ImportData.c_Core.
                                                            c_OscTxMessageData,
                                                            this->mc_NewNodeAssignment.c_ImportData.c_Core.
                                                            c_OscTxSignalData,
                                                            this->mc_NewNodeAssignment.c_ImportData.c_Ui.
                                                            c_UiTxMessageData,
                                                            this->mc_NewNodeAssignment.c_ImportData.c_Ui.
                                                            c_UiTxSignalData,
                                                            &this->mc_NewNodeAssignment.c_ImportData.c_Core
                                                            .c_WarningMessagesPerTxMessage);
         orc_AdaptedTxMessages = C_CieImportedMessageVectorData(this->mc_AdaptedNodeAssignment.c_ImportData.
                                                                c_Core.c_OscTxMessageData,
                                                                this->mc_AdaptedNodeAssignment.c_ImportData.
                                                                c_Core.c_OscTxSignalData,
                                                                this->mc_AdaptedNodeAssignment.c_ImportData.
                                                                c_Ui.c_UiTxMessageData,
                                                                this->mc_AdaptedNodeAssignment.c_ImportData.
                                                                c_Ui.c_UiTxSignalData,
                                                                &this->mc_AdaptedNodeAssignment.c_ImportData.
                                                                c_Core.c_WarningMessagesPerTxMessage);
         orc_DeletedTxMessages = C_CieImportedMessageVectorData(this->mc_DeletedNodeAssignment.c_ImportData.
                                                                c_Core.c_OscTxMessageData,
                                                                this->mc_DeletedNodeAssignment.c_ImportData.
                                                                c_Core.c_OscTxSignalData,
                                                                this->mc_DeletedNodeAssignment.c_ImportData.
                                                                c_Ui.c_UiTxMessageData,
                                                                this->mc_DeletedNodeAssignment.c_ImportData.
                                                                c_Ui.c_UiTxSignalData,
                                                                &this->mc_DeletedNodeAssignment.c_ImportData.
                                                                c_Core.c_WarningMessagesPerTxMessage);
         //Compare
         orc_ExistingRxMessages = C_CieImportedMessageVectorData(
            pc_OscContainer->c_RxMessages,
            pc_OscRxList->c_Elements,
            rc_UiContainer.c_RxMessages,
            pc_UiRxList->c_DataPoolListElements);
         orc_ImportedRxMessages = C_CieImportedMessageVectorData(this->mc_ImportedNodeAssignment.c_ImportData.
                                                                 c_Core.c_OscRxMessageData,
                                                                 this->mc_ImportedNodeAssignment.c_ImportData.
                                                                 c_Core.c_OscRxSignalData,
                                                                 this->mc_ImportedNodeAssignment.c_ImportData.
                                                                 c_Ui.c_UiRxMessageData,
                                                                 this->mc_ImportedNodeAssignment.c_ImportData.
                                                                 c_Ui.c_UiRxSignalData,
                                                                 &this->mc_ImportedNodeAssignment.c_ImportData.
                                                                 c_Core.c_WarningMessagesPerRxMessage);
         orc_ExistingTxMessages = C_CieImportedMessageVectorData(
            pc_OscContainer->c_TxMessages,
            pc_OscTxList->c_Elements,
            rc_UiContainer.c_TxMessages,
            pc_UiTxList->c_DataPoolListElements);
         orc_ImportedTxMessages =
            C_CieImportedMessageVectorData(this->mc_ImportedNodeAssignment.c_ImportData.
                                           c_Core.c_OscTxMessageData,
                                           this->mc_ImportedNodeAssignment.c_ImportData.
                                           c_Core.c_OscTxSignalData,
                                           this->mc_ImportedNodeAssignment.c_ImportData.
                                           c_Ui.c_UiTxMessageData,
                                           this->mc_ImportedNodeAssignment.c_ImportData.
                                           c_Ui.c_UiTxSignalData,
                                           &this->mc_ImportedNodeAssignment.c_ImportData.
                                           c_Core.c_WarningMessagesPerTxMessage);
      }
      else
      {
         s32_Retval = C_CONFIG;
      }
   }
   else
   {
      s32_Retval = C_CONFIG;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write compared data

   \param[in]  orc_NewRxMessages       New rx messages
   \param[in]  orc_NewTxMessages       New tx messages
   \param[in]  orc_AdaptedRxMessages   Adapted rx messages
   \param[in]  orc_AdaptedTxMessages   Adapted tx messages
   \param[in]  orc_DeletedRxMessages   Deleted rx messages
   \param[in]  orc_DeletedTxMessages   Deleted tx messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_WriteComparedData(const C_CieImportedMessageVectorData & orc_NewRxMessages,
                                                         const C_CieImportedMessageVectorData & orc_NewTxMessages,
                                                         const C_CieImportedMessageVectorData & orc_AdaptedRxMessages,
                                                         const C_CieImportedMessageVectorData & orc_AdaptedTxMessages,
                                                         const C_CieImportedMessageVectorData & orc_DeletedRxMessages,
                                                         const C_CieImportedMessageVectorData & orc_DeletedTxMessages)
{
   orc_NewRxMessages.WriteBack(this->mc_NewNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Core.c_OscRxSignalData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Ui.c_UiRxMessageData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Ui.c_UiRxSignalData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Core.c_WarningMessagesPerRxMessage);
   orc_AdaptedRxMessages.WriteBack(
      this->mc_AdaptedNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData,
      this->mc_AdaptedNodeAssignment.c_ImportData.c_Core.c_OscRxSignalData,
      this->mc_AdaptedNodeAssignment.c_ImportData.c_Ui.c_UiRxMessageData,
      this->mc_AdaptedNodeAssignment.c_ImportData.c_Ui.c_UiRxSignalData,
      this->mc_AdaptedNodeAssignment.c_ImportData.c_Core.c_WarningMessagesPerRxMessage);
   orc_DeletedRxMessages.WriteBack(
      this->mc_DeletedNodeAssignment.c_ImportData.c_Core.c_OscRxMessageData,
      this->mc_DeletedNodeAssignment.c_ImportData.c_Core.c_OscRxSignalData,
      this->mc_DeletedNodeAssignment.c_ImportData.c_Ui.c_UiRxMessageData,
      this->mc_DeletedNodeAssignment.c_ImportData.c_Ui.c_UiRxSignalData,
      this->mc_DeletedNodeAssignment.c_ImportData.c_Core.c_WarningMessagesPerRxMessage);
   orc_NewTxMessages.WriteBack(this->mc_NewNodeAssignment.c_ImportData.c_Core.
                               c_OscTxMessageData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Core.
                               c_OscTxSignalData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Ui.
                               c_UiTxMessageData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Ui.
                               c_UiTxSignalData,
                               this->mc_NewNodeAssignment.c_ImportData.c_Core
                               .c_WarningMessagesPerTxMessage);
   orc_AdaptedTxMessages.WriteBack(this->mc_AdaptedNodeAssignment.c_ImportData.
                                   c_Core.c_OscTxMessageData,
                                   this->mc_AdaptedNodeAssignment.c_ImportData.
                                   c_Core.c_OscTxSignalData,
                                   this->mc_AdaptedNodeAssignment.c_ImportData.
                                   c_Ui.c_UiTxMessageData,
                                   this->mc_AdaptedNodeAssignment.c_ImportData.
                                   c_Ui.c_UiTxSignalData,
                                   this->mc_AdaptedNodeAssignment.c_ImportData.
                                   c_Core.c_WarningMessagesPerTxMessage);
   orc_DeletedTxMessages.WriteBack(this->mc_DeletedNodeAssignment.c_ImportData.
                                   c_Core.c_OscTxMessageData,
                                   this->mc_DeletedNodeAssignment.c_ImportData.
                                   c_Core.c_OscTxSignalData,
                                   this->mc_DeletedNodeAssignment.c_ImportData.
                                   c_Ui.c_UiTxMessageData,
                                   this->mc_DeletedNodeAssignment.c_ImportData.
                                   c_Ui.c_UiTxSignalData,
                                   this->mc_DeletedNodeAssignment.c_ImportData.
                                   c_Core.c_WarningMessagesPerTxMessage);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init new data message vector

   \param[in]      orc_ExistingMessages   Existing messages
   \param[in]      orc_ImportedMessages   Imported messages
   \param[in,out]  orc_NewMessages        New messages
   \param[in,out]  orc_AdaptedMessages    Adapted messages
   \param[in,out]  orc_DeletedMessages    Deleted messages
   \param[in]      oq_MessageIsTxInEds    Message is tx in eds
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_CompareDataMessageVector(
   const C_CieImportedMessageVectorData & orc_ExistingMessages,
   const C_CieImportedMessageVectorData & orc_ImportedMessages, C_CieImportedMessageVectorData & orc_NewMessages,
   C_CieImportedMessageVectorData & orc_AdaptedMessages, C_CieImportedMessageVectorData & orc_DeletedMessages,
   const bool oq_MessageIsTxInEds)
{
   //Clear new
   orc_NewMessages.Clear();
   //Clear adapted
   orc_AdaptedMessages.Clear();
   //Clear deleted
   orc_DeletedMessages.Clear();
   //Vector size check
   tgl_assert(orc_ExistingMessages.c_OscMessageData.size() == orc_ExistingMessages.c_UiMessageData.size());
   tgl_assert(orc_ExistingMessages.c_OscSignalData.size() == orc_ExistingMessages.c_UiSignalData.size());
   tgl_assert(orc_ImportedMessages.c_OscMessageData.size() == orc_ImportedMessages.c_UiMessageData.size());
   tgl_assert(orc_ImportedMessages.c_OscSignalData.size() == orc_ImportedMessages.c_UiSignalData.size());
   tgl_assert(orc_ImportedMessages.c_OscMessageData.size() == orc_ImportedMessages.c_WarningMessagesPerMessage.size());
   if ((orc_ExistingMessages.c_OscMessageData.size() == orc_ExistingMessages.c_UiMessageData.size()) &&
       (orc_ExistingMessages.c_OscSignalData.size() == orc_ExistingMessages.c_UiSignalData.size()) &&
       (orc_ImportedMessages.c_OscMessageData.size() == orc_ImportedMessages.c_UiMessageData.size()) &&
       (orc_ImportedMessages.c_OscSignalData.size() == orc_ImportedMessages.c_UiSignalData.size()) &&
       (orc_ImportedMessages.c_OscMessageData.size() == orc_ImportedMessages.c_WarningMessagesPerMessage.size()))
   {
      //New and adapted messages
      m_InitNewAndAdaptedMessageVector(orc_ExistingMessages, orc_ImportedMessages, orc_NewMessages,
                                       orc_AdaptedMessages, oq_MessageIsTxInEds);
      //Deleted messages
      m_InitDeletedMessageVector(orc_ExistingMessages, orc_ImportedMessages, orc_DeletedMessages);

      //Handle signals
      m_HandleSignalChangesForAdaptedMessageVector(orc_AdaptedMessages);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init new and adapted message vector

   \param[in]      orc_ExistingMessages   Existing messages
   \param[in]      orc_ImportedMessages   Imported messages
   \param[in,out]  orc_NewMessages        New messages
   \param[in,out]  orc_AdaptedMessages    Adapted messages
   \param[in]      oq_MessageIsTxInEds    Message is tx in eds
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_InitNewAndAdaptedMessageVector(
   const C_CieImportedMessageVectorData & orc_ExistingMessages,
   const C_CieImportedMessageVectorData & orc_ImportedMessages, C_CieImportedMessageVectorData & orc_NewMessages,
   C_CieImportedMessageVectorData & orc_AdaptedMessages, const bool oq_MessageIsTxInEds)
{
   for (uint32_t u32_ItNewMessage = 0UL; u32_ItNewMessage < orc_ImportedMessages.c_OscMessageData.size();
        ++u32_ItNewMessage)
   {
      const C_OscCanMessage & rc_NewMessage = orc_ImportedMessages.c_OscMessageData[u32_ItNewMessage];
      bool q_MatchFound = false;
      for (uint32_t u32_ItExistingMessage = 0UL; u32_ItExistingMessage < orc_ExistingMessages.c_OscMessageData.size();
           ++u32_ItExistingMessage)
      {
         const C_OscCanMessage & rc_ExistingMessage = orc_ExistingMessages.c_OscMessageData[u32_ItExistingMessage];
         //Match node
         if (rc_ExistingMessage.c_CanOpenManagerOwnerNodeIndex == this->mc_DeviceNodeId)
         {
            //Match message
            if (rc_NewMessage.u16_CanOpenManagerPdoIndex == rc_ExistingMessage.u16_CanOpenManagerPdoIndex)
            {
               q_MatchFound = true;
               mh_AddAdaptedMessage(this->mc_NewConfig, orc_ExistingMessages.c_OscMessageData[u32_ItExistingMessage],
                                    orc_ExistingMessages.c_UiMessageData[u32_ItExistingMessage],
                                    orc_ExistingMessages.c_OscSignalData, orc_ExistingMessages.c_UiSignalData,
                                    orc_ImportedMessages.c_OscMessageData[u32_ItNewMessage],
                                    orc_ImportedMessages.c_OscSignalData,
                                    orc_ImportedMessages.c_UiMessageData[u32_ItNewMessage],
                                    orc_ImportedMessages.c_UiSignalData,
                                    orc_ImportedMessages.c_WarningMessagesPerMessage[u32_ItNewMessage],
                                    orc_AdaptedMessages.c_OscMessageData,
                                    orc_AdaptedMessages.c_OscSignalData, orc_AdaptedMessages.c_UiMessageData,
                                    orc_AdaptedMessages.c_UiSignalData,
                                    orc_AdaptedMessages.c_WarningMessagesPerMessage, oq_MessageIsTxInEds);
               break;
            }
         }
      }
      if (!q_MatchFound)
      {
         mh_AddNewMessage(orc_ImportedMessages.c_OscMessageData[u32_ItNewMessage], orc_ImportedMessages.c_OscSignalData,
                          orc_ImportedMessages.c_UiMessageData[u32_ItNewMessage], orc_ImportedMessages.c_UiSignalData,
                          orc_ImportedMessages.c_WarningMessagesPerMessage[u32_ItNewMessage],
                          orc_NewMessages.c_OscMessageData,
                          orc_NewMessages.c_OscSignalData, orc_NewMessages.c_UiMessageData,
                          orc_NewMessages.c_UiSignalData,
                          orc_NewMessages.c_WarningMessagesPerMessage);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init deleted message vector

   \param[in]      orc_ExistingMessages   Existing messages
   \param[in]      orc_ImportedMessages   Imported messages
   \param[in,out]  orc_DeletedMessages    Deleted messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_InitDeletedMessageVector(
   const C_CieImportedMessageVectorData & orc_ExistingMessages,
   const C_CieImportedMessageVectorData & orc_ImportedMessages, C_CieImportedMessageVectorData & orc_DeletedMessages)
{
   for (uint32_t u32_ItExistingMessage = 0UL; u32_ItExistingMessage < orc_ExistingMessages.c_OscMessageData.size();
        ++u32_ItExistingMessage)
   {
      const C_OscCanMessage & rc_ExistingMessage = orc_ExistingMessages.c_OscMessageData[u32_ItExistingMessage];
      //Match node
      if (rc_ExistingMessage.c_CanOpenManagerOwnerNodeIndex == this->mc_DeviceNodeId)
      {
         bool q_MatchFound = false;
         for (uint32_t u32_ItNewMessage = 0UL; u32_ItNewMessage < orc_ImportedMessages.c_OscMessageData.size();
              ++u32_ItNewMessage)
         {
            const C_OscCanMessage & rc_NewMessage = orc_ImportedMessages.c_OscMessageData[u32_ItNewMessage];
            //Match message
            if (rc_NewMessage.u16_CanOpenManagerPdoIndex == rc_ExistingMessage.u16_CanOpenManagerPdoIndex)
            {
               q_MatchFound = true;
               break;
            }
         }
         if (!q_MatchFound)
         {
            const QString c_Empty;
            mh_AddNewMessage(rc_ExistingMessage, orc_ExistingMessages.c_OscSignalData,
                             orc_ExistingMessages.c_UiMessageData[u32_ItExistingMessage],
                             orc_ExistingMessages.c_UiSignalData,
                             c_Empty, orc_DeletedMessages.c_OscMessageData,
                             orc_DeletedMessages.c_OscSignalData, orc_DeletedMessages.c_UiMessageData,
                             orc_DeletedMessages.c_UiSignalData,
                             orc_DeletedMessages.c_WarningMessagesPerMessage);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle signal changes for adapted message vector

   \param[in,out]  orc_AdaptedMessages    Adapted messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_HandleSignalChangesForAdaptedMessageVector(
   C_CieImportedMessageVectorData & orc_AdaptedMessages)
{
   uint32_t u32_SignalIndexOffset = 0UL;

   for (uint32_t u32_ItAdaptedMessage = 0UL; u32_ItAdaptedMessage < orc_AdaptedMessages.c_OscMessageData.size();
        ++u32_ItAdaptedMessage)
   {
      QString c_RemovedSignalsInfo;
      QString & rc_MessageInfoString = orc_AdaptedMessages.c_WarningMessagesPerMessage[u32_ItAdaptedMessage];
      C_OscCanMessage & rc_AdaptedMessage = orc_AdaptedMessages.c_OscMessageData[u32_ItAdaptedMessage];
      C_PuiSdNodeCanMessage & rc_AdaptedUiMessage = orc_AdaptedMessages.c_UiMessageData[u32_ItAdaptedMessage];
      for (uint32_t u32_ItAdaptedSignal = 0UL; u32_ItAdaptedSignal < rc_AdaptedMessage.c_Signals.size();)
      {
         C_OscCanSignal & rc_AdaptedSignal = rc_AdaptedMessage.c_Signals[u32_ItAdaptedSignal];
         rc_AdaptedSignal.u32_ComDataElementIndex -= u32_SignalIndexOffset;
         {
            C_OscNodeDataPoolListElement & rc_OscSignalData =
               orc_AdaptedMessages.c_OscSignalData[rc_AdaptedSignal.u32_ComDataElementIndex];
            const C_OscCanOpenManagerMappableSignal * const pc_OrgSignal = this->m_GetOrgSignal(
               rc_AdaptedSignal.u16_CanOpenManagerObjectDictionaryIndex,
               rc_AdaptedSignal.u8_CanOpenManagerObjectDictionarySubIndex);
            if (pc_OrgSignal != NULL)
            {
               //Cmp
               this->m_AdaptSignalProperties(rc_AdaptedSignal, rc_OscSignalData,
                                             orc_AdaptedMessages.c_UiSignalData[rc_AdaptedSignal.u32_ComDataElementIndex],
                                             *pc_OrgSignal, rc_MessageInfoString);
               //Msg DLC
               if (rc_AdaptedMessage.u16_Dlc <
                   ((rc_AdaptedSignal.u16_ComBitStart + rc_AdaptedSignal.u16_ComBitLength) / 8))
               {
                  rc_AdaptedMessage.u16_Dlc = (rc_AdaptedSignal.u16_ComBitStart + rc_AdaptedSignal.u16_ComBitLength) /
                                              8;
               }
               //Iterate!
               ++u32_ItAdaptedSignal;
            }
            else
            {
               //Removed
               //Info
               if (!c_RemovedSignalsInfo.isEmpty())
               {
                  c_RemovedSignalsInfo += ", ";
               }
               c_RemovedSignalsInfo += C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(
                  rc_OscSignalData.c_Name);

               //Remove
               orc_AdaptedMessages.c_OscSignalData.erase(
                  orc_AdaptedMessages.c_OscSignalData.begin() + rc_AdaptedSignal.u32_ComDataElementIndex);
               orc_AdaptedMessages.c_UiSignalData.erase(orc_AdaptedMessages.c_UiSignalData.begin() +
                                                        rc_AdaptedSignal.u32_ComDataElementIndex);
               //Remove last as these are referenced by the above remove step
               rc_AdaptedMessage.c_Signals.erase(rc_AdaptedMessage.c_Signals.begin() + u32_ItAdaptedSignal);
               rc_AdaptedUiMessage.c_Signals.erase(rc_AdaptedUiMessage.c_Signals.begin() + u32_ItAdaptedSignal);
               // Keep track of removed items
               ++u32_SignalIndexOffset;
            }
         }
      }
      if (!c_RemovedSignalsInfo.isEmpty())
      {
         rc_MessageInfoString += static_cast<QString>(C_GtGetText::h_GetText(
                                                         "Signals removed, because it could not be found in the updated EDS: %1\n"))
                                 .arg(c_RemovedSignalsInfo);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new message

   \param[in]      orc_ImportedOscMessageData         Imported osc message data
   \param[in]      orc_ImportedOscSignalData          Imported osc signal data
   \param[in]      orc_ImportedUiMessageData          Imported ui message data
   \param[in]      orc_ImportedUiSignalData           Imported ui signal data
   \param[in]      orc_ImportedWarningMessages        Imported warning messages
   \param[in,out]  orc_NewOscMessageData              New osc message data
   \param[in,out]  orc_NewOscSignalData               New osc signal data
   \param[in,out]  orc_NewUiMessageData               New ui message data
   \param[in,out]  orc_NewUiSignalData                New ui signal data
   \param[in,out]  orc_NewWarningMessagesPerMessage   New warning messages per message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::mh_AddNewMessage(const C_OscCanMessage & orc_ImportedOscMessageData,
                                                      const std::vector<C_OscNodeDataPoolListElement> & orc_ImportedOscSignalData, const C_PuiSdNodeCanMessage & orc_ImportedUiMessageData, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_ImportedUiSignalData, const QString & orc_ImportedWarningMessages, std::vector<C_OscCanMessage> & orc_NewOscMessageData, std::vector<C_OscNodeDataPoolListElement> & orc_NewOscSignalData, std::vector<C_PuiSdNodeCanMessage> & orc_NewUiMessageData, std::vector<C_PuiSdNodeDataPoolListElement> & orc_NewUiSignalData,
                                                      std::vector<QString> & orc_NewWarningMessagesPerMessage)
{
   C_OscCanMessage c_NewOscMessage = orc_ImportedOscMessageData;

   //Vector size check
   tgl_assert(c_NewOscMessage.c_Signals.size() == orc_ImportedUiMessageData.c_Signals.size());
   if (c_NewOscMessage.c_Signals.size() == orc_ImportedUiMessageData.c_Signals.size())
   {
      const uint32_t u32_SignalOffset = orc_NewOscSignalData.size();
      for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < c_NewOscMessage.c_Signals.size(); ++u32_ItSignal)
      {
         C_OscCanSignal & rc_NewOscSignal = c_NewOscMessage.c_Signals[u32_ItSignal];
         tgl_assert(rc_NewOscSignal.u32_ComDataElementIndex < orc_ImportedOscSignalData.size());
         tgl_assert(rc_NewOscSignal.u32_ComDataElementIndex < orc_ImportedUiSignalData.size());
         if ((rc_NewOscSignal.u32_ComDataElementIndex < orc_ImportedOscSignalData.size()) &&
             (rc_NewOscSignal.u32_ComDataElementIndex < orc_ImportedUiSignalData.size()))
         {
            orc_NewOscSignalData.push_back(orc_ImportedOscSignalData[rc_NewOscSignal.u32_ComDataElementIndex]);
            orc_NewUiSignalData.push_back(orc_ImportedUiSignalData[rc_NewOscSignal.u32_ComDataElementIndex]);
            rc_NewOscSignal.u32_ComDataElementIndex = u32_SignalOffset + u32_ItSignal;
         }
      }
      orc_NewOscMessageData.push_back(c_NewOscMessage);
      orc_NewUiMessageData.push_back(orc_ImportedUiMessageData);
      orc_NewWarningMessagesPerMessage.push_back(orc_ImportedWarningMessages);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add adapted message

   \param[in]      orc_NewConfig                         New config
   \param[in]      orc_ExistingOscMessage                Existing osc message
   \param[in]      orc_ExistingUiMessage                 Existing ui message
   \param[in]      orc_ExistingOscSignalData             Existing osc signal data
   \param[in]      orc_ExistingUiSignalData              Existing ui signal data
   \param[in]      orc_ImportedOscMessageData            Imported osc message data
   \param[in]      orc_ImportedOscSignalData             Imported osc signal data
   \param[in]      orc_ImportedUiMessageData             Imported ui message data
   \param[in]      orc_ImportedUiSignalData              Imported ui signal data
   \param[in]      orc_ImportedWarningMessages           Imported warning messages
   \param[in,out]  orc_AdaptedOscMessageData             Adapted osc message data
   \param[in,out]  orc_AdaptedOscSignalData              Adapted osc signal data
   \param[in,out]  orc_AdaptedUiMessageData              Adapted ui message data
   \param[in,out]  orc_AdaptedUiSignalData               Adapted ui signal data
   \param[in,out]  orc_AdaptedWarningMessagesPerMessage  Adapted warning messages per message
   \param[in]      oq_MessageIsTxInEds                   Message is tx in eds
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::mh_AddAdaptedMessage(const C_OscCanOpenManagerDeviceInfo & orc_NewConfig,
                                                          const C_OscCanMessage & orc_ExistingOscMessage,
                                                          const C_PuiSdNodeCanMessage & orc_ExistingUiMessage,
                                                          const std::vector<C_OscNodeDataPoolListElement> & orc_ExistingOscSignalData, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_ExistingUiSignalData, const C_OscCanMessage & orc_ImportedOscMessageData, const std::vector<C_OscNodeDataPoolListElement> & orc_ImportedOscSignalData, const C_PuiSdNodeCanMessage & orc_ImportedUiMessageData, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_ImportedUiSignalData, const QString & orc_ImportedWarningMessages, std::vector<C_OscCanMessage> & orc_AdaptedOscMessageData, std::vector<C_OscNodeDataPoolListElement> & orc_AdaptedOscSignalData, std::vector<C_PuiSdNodeCanMessage> & orc_AdaptedUiMessageData, std::vector<C_PuiSdNodeDataPoolListElement> & orc_AdaptedUiSignalData, std::vector<QString> & orc_AdaptedWarningMessagesPerMessage,
                                                          const bool oq_MessageIsTxInEds)
{
   QString c_WarningMessages = orc_ImportedWarningMessages;
   C_OscCanMessage c_NewOscMessage = orc_ExistingOscMessage;
   C_PuiSdNodeCanMessage c_NewUiMessage = orc_ExistingUiMessage;
   bool q_IsRo;

   C_SdNdeCoDeviceUpdateEdsDialog::mh_AdaptMessageProperties(orc_NewConfig, orc_ImportedOscMessageData,
                                                             c_NewOscMessage,
                                                             oq_MessageIsTxInEds, c_WarningMessages);

   tgl_assert(orc_NewConfig.c_EdsFileContent.IsPdoMappingRo(c_NewOscMessage.u16_CanOpenManagerPdoIndex,
                                                            oq_MessageIsTxInEds, q_IsRo) == C_NO_ERR);
   if (q_IsRo)
   {
      //Add info
      c_WarningMessages +=
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Message mapping is read-only. Message layout will be reset. Replaced existing signals (%1) by (%2)\n"))
         .arg(C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNames(orc_ExistingOscMessage, orc_ExistingOscSignalData)).arg(C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNames(
                                                                                                                           orc_ImportedOscMessageData,
                                                                                                                           orc_ImportedOscSignalData));
      //Use imported DLC value
      c_NewOscMessage.u16_Dlc = orc_ImportedOscMessageData.u16_Dlc;
      //Replace
      C_SdNdeCoDeviceUpdateEdsDialog::mh_TransferMessageSignals(orc_ImportedOscMessageData, orc_ImportedUiMessageData,
                                                                orc_ImportedOscSignalData, orc_ImportedUiSignalData,
                                                                c_NewOscMessage, c_NewUiMessage,
                                                                orc_AdaptedOscSignalData, orc_AdaptedUiSignalData);
   }
   else
   {
      //Re-add same signal
      C_SdNdeCoDeviceUpdateEdsDialog::mh_TransferMessageSignals(orc_ExistingOscMessage, orc_ExistingUiMessage,
                                                                orc_ExistingOscSignalData, orc_ExistingUiSignalData,
                                                                c_NewOscMessage, c_NewUiMessage,
                                                                orc_AdaptedOscSignalData, orc_AdaptedUiSignalData);
   }
   orc_AdaptedOscMessageData.push_back(c_NewOscMessage);
   orc_AdaptedUiMessageData.push_back(c_NewUiMessage);
   orc_AdaptedWarningMessagesPerMessage.push_back(c_WarningMessages);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Transfer message signals

   \param[in]      orc_SourceOscMessage      Source osc message
   \param[in]      orc_SourceUiMessage       Source ui message
   \param[in]      orc_SourceOscSignalData   Source osc signal data
   \param[in]      orc_SourceUiSignalData    Source ui signal data
   \param[in,out]  orc_TargetOscMessageData  Target osc message data
   \param[in,out]  orc_TargetUiMessageData   Target ui message data
   \param[in,out]  orc_TargetOscSignalData   Target osc signal data
   \param[in,out]  orc_TargetUiSignalData    Target ui signal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::mh_TransferMessageSignals(const C_OscCanMessage & orc_SourceOscMessage,
                                                               const C_PuiSdNodeCanMessage & orc_SourceUiMessage,
                                                               const std::vector<C_OscNodeDataPoolListElement> & orc_SourceOscSignalData, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_SourceUiSignalData, C_OscCanMessage & orc_TargetOscMessageData, C_PuiSdNodeCanMessage & orc_TargetUiMessageData, std::vector<C_OscNodeDataPoolListElement> & orc_TargetOscSignalData,
                                                               std::vector<C_PuiSdNodeDataPoolListElement> & orc_TargetUiSignalData)
{
   const uint32_t u32_SignalOffset = orc_TargetOscSignalData.size();

   //Replace
   orc_TargetOscMessageData.c_Signals = orc_SourceOscMessage.c_Signals;
   orc_TargetUiMessageData.c_Signals = orc_SourceUiMessage.c_Signals;
   //Vector size check
   tgl_assert(orc_TargetOscMessageData.c_Signals.size() == orc_TargetUiMessageData.c_Signals.size());

   if (orc_TargetOscMessageData.c_Signals.size() == orc_TargetUiMessageData.c_Signals.size())
   {
      for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < orc_TargetOscMessageData.c_Signals.size(); ++u32_ItSignal)
      {
         C_OscCanSignal & rc_NewOscSignal = orc_TargetOscMessageData.c_Signals[u32_ItSignal];
         tgl_assert(rc_NewOscSignal.u32_ComDataElementIndex < orc_SourceOscSignalData.size());
         tgl_assert(rc_NewOscSignal.u32_ComDataElementIndex < orc_SourceUiSignalData.size());
         if ((rc_NewOscSignal.u32_ComDataElementIndex < orc_SourceOscSignalData.size()) &&
             (rc_NewOscSignal.u32_ComDataElementIndex < orc_SourceUiSignalData.size()))
         {
            orc_TargetOscSignalData.push_back(orc_SourceOscSignalData[rc_NewOscSignal.u32_ComDataElementIndex]);
            orc_TargetUiSignalData.push_back(orc_SourceUiSignalData[rc_NewOscSignal.u32_ComDataElementIndex]);
            rc_NewOscSignal.u32_ComDataElementIndex = u32_SignalOffset + u32_ItSignal;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt config

   \param[in]  orc_NewConfig  New config

   \return
   Adapted config
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscCanOpenManagerDeviceInfo C_SdNdeCoDeviceUpdateEdsDialog::m_AdaptConfig(
   const C_OscCanOpenManagerDeviceInfo & orc_NewConfig) const
{
   C_OscCanOpenManagerDeviceInfo c_NewConfig;
   const C_OscCanOpenManagerDeviceInfo * const pc_ExistingConfig =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerDevice(this->mu32_ManagerNodeIndex,
                                                               this->mu8_ManagerInterfaceId,
                                                               this->mc_DeviceNodeId);

   tgl_assert(pc_ExistingConfig != NULL);
   if (pc_ExistingConfig != NULL)
   {
      const std::set<uint8_t> c_Map = orc_NewConfig.c_EdsFileContent.GetAllAvailableFactorySettingsSubIndices();
      const std::set<uint8_t>::const_iterator c_ItResult = c_Map.find(
         pc_ExistingConfig->u8_ResetNodeObjectDictionarySubIndex);
      // check for read-only
      bool q_IsRo = true;
      c_NewConfig = *pc_ExistingConfig;
      // Always replace
      c_NewConfig.c_EdsFileContent = orc_NewConfig.c_EdsFileContent;
      c_NewConfig.c_EdsFileMappableSignals = orc_NewConfig.c_EdsFileMappableSignals;
      c_NewConfig.c_EdsFileName = orc_NewConfig.c_EdsFileName;

      // Conditional replace
      if (orc_NewConfig.c_EdsFileContent.IsHeartbeatProducerSupported())
      {
         tgl_assert(orc_NewConfig.c_EdsFileContent.IsHeartbeatProducerRo(q_IsRo) == C_NO_ERR);
         if (q_IsRo)
         {
            c_NewConfig.q_EnableHeartbeatProducing = false;
            c_NewConfig.u16_HeartbeatProducerTimeMs = orc_NewConfig.u16_HeartbeatProducerTimeMs;
         }
      }
      else
      {
         c_NewConfig.q_EnableHeartbeatProducing = false;
         c_NewConfig.u16_HeartbeatProducerTimeMs = orc_NewConfig.u16_HeartbeatProducerTimeMs;
      }
      if (orc_NewConfig.c_EdsFileContent.GetNumHeartbeatConsumers() != 0)
      {
         tgl_assert(orc_NewConfig.c_EdsFileContent.IsHeartbeatConsumerRo(q_IsRo) == C_NO_ERR);
         if (q_IsRo)
         {
            c_NewConfig.q_EnableHeartbeatConsuming = false;
            c_NewConfig.q_EnableHeartbeatConsumingAutoCalculation = false;
            c_NewConfig.u16_HeartbeatConsumerTimeMs = orc_NewConfig.u16_HeartbeatConsumerTimeMs;
         }
      }
      else
      {
         c_NewConfig.q_EnableHeartbeatConsuming = false;
         c_NewConfig.q_EnableHeartbeatConsumingAutoCalculation = false;
         c_NewConfig.u16_HeartbeatConsumerTimeMs = orc_NewConfig.u16_HeartbeatConsumerTimeMs;
      }
      if (c_ItResult == c_Map.end())
      {
         c_NewConfig.q_FactorySettingsActive = false;
         c_NewConfig.u8_ResetNodeObjectDictionarySubIndex = 0U;
      }
   }
   else
   {
      c_NewConfig = orc_NewConfig;
   }
   return c_NewConfig;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt message properties

   \param[in]      orc_NewConfig                New config
   \param[in]      orc_ImportedOscMessageData   Imported osc message data
   \param[in,out]  orc_NewOscMessage            New osc message
   \param[in]      oq_MessageIsTxInEds          Message is tx in eds
   \param[in,out]  orc_WarningMessages          Warning messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::mh_AdaptMessageProperties(const C_OscCanOpenManagerDeviceInfo & orc_NewConfig,
                                                               const C_OscCanMessage & orc_ImportedOscMessageData,
                                                               C_OscCanMessage & orc_NewOscMessage,
                                                               const bool oq_MessageIsTxInEds,
                                                               QString & orc_WarningMessages)
{
   bool q_IsRo;

   tgl_assert(orc_NewConfig.c_EdsFileContent.IsCobIdRo(orc_NewOscMessage.u16_CanOpenManagerPdoIndex,
                                                       oq_MessageIsTxInEds, q_IsRo) == C_NO_ERR);
   if (q_IsRo)
   {
      if (orc_NewOscMessage.u32_CanId != orc_ImportedOscMessageData.u32_CanId)
      {
         orc_WarningMessages +=
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "CAN-ID read-only so the current CAN-ID value 0x%1 was replaced by 0x%2\n"))
            .
            arg(QString::number(
                   orc_NewOscMessage.u32_CanId, 16).toUpper()).arg(QString::number(
                                                                      orc_ImportedOscMessageData.u32_CanId,
                                                                      16).toUpper());
         orc_NewOscMessage.u32_CanId = orc_ImportedOscMessageData.u32_CanId;
      }
      if (orc_NewOscMessage.q_CanOpenManagerMessageActive != orc_ImportedOscMessageData.q_CanOpenManagerMessageActive)
      {
         orc_WarningMessages +=
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "CAN-ID read-only so the current message active value %1 was replaced by %2\n"))
            .
            arg(
               C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(orc_NewOscMessage.q_CanOpenManagerMessageActive)).arg(
               C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(orc_ImportedOscMessageData.q_CanOpenManagerMessageActive));
         orc_NewOscMessage.q_CanOpenManagerMessageActive = orc_ImportedOscMessageData.q_CanOpenManagerMessageActive;
      }
      if (orc_NewOscMessage.q_CanOpenManagerCobIdIncludesNodeId !=
          orc_ImportedOscMessageData.q_CanOpenManagerCobIdIncludesNodeId)
      {
         orc_WarningMessages +=
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "CAN-ID read-only so the current setting to include the Node-ID %1 was replaced by %2\n"))
            .
            arg(
               C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(orc_NewOscMessage.q_CanOpenManagerCobIdIncludesNodeId)).
            arg(
               C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(orc_ImportedOscMessageData.
                                                               q_CanOpenManagerCobIdIncludesNodeId));
         orc_NewOscMessage.q_CanOpenManagerCobIdIncludesNodeId =
            orc_ImportedOscMessageData.q_CanOpenManagerCobIdIncludesNodeId;
      }
      if (orc_NewOscMessage.q_IsExtended != orc_ImportedOscMessageData.q_IsExtended)
      {
         orc_WarningMessages +=
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "CAN-ID read-only so the current extended type value %1 was replaced by %2\n"))
            .
            arg(
               C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(orc_NewOscMessage.q_IsExtended)).arg(
               C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(orc_ImportedOscMessageData.q_IsExtended));
         orc_NewOscMessage.q_IsExtended = orc_ImportedOscMessageData.q_IsExtended;
      }
   }
   tgl_assert(orc_NewConfig.c_EdsFileContent.IsTransmissionTypeRo(orc_NewOscMessage.u16_CanOpenManagerPdoIndex,
                                                                  oq_MessageIsTxInEds, q_IsRo) == C_NO_ERR);
   if (q_IsRo)
   {
      if (orc_NewOscMessage.e_TxMethod != orc_ImportedOscMessageData.e_TxMethod)
      {
         orc_WarningMessages +=
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "Tx method read-only so the current Tx method value %1 was replaced by %2\n"))
            .
            arg(
               C_SdUtil::h_ConvertTxMethodToName(orc_NewOscMessage.e_TxMethod)).arg(C_SdUtil::h_ConvertTxMethodToName(
                                                                                       orc_ImportedOscMessageData.
                                                                                       e_TxMethod));
         orc_NewOscMessage.e_TxMethod = orc_ImportedOscMessageData.e_TxMethod;
         orc_NewOscMessage.u8_CanOpenTxMethodAdditionalInfo =
            orc_ImportedOscMessageData.u8_CanOpenTxMethodAdditionalInfo;
      }
   }
   if (orc_NewConfig.c_EdsFileContent.IsInhibitTimeRo(orc_NewOscMessage.u16_CanOpenManagerPdoIndex,
                                                      oq_MessageIsTxInEds, q_IsRo) == C_NO_ERR)
   {
      if (q_IsRo)
      {
         if (orc_NewOscMessage.u16_DelayTimeMs != orc_ImportedOscMessageData.u16_DelayTimeMs)
         {
            orc_WarningMessages +=
               static_cast<QString>(C_GtGetText::h_GetText(
                                       "Inhibit time read-only so the current value %1 was replaced by %2\n"))
               .
               arg(orc_NewOscMessage.u16_DelayTimeMs).arg(
                  orc_ImportedOscMessageData.
                  u16_DelayTimeMs);
            orc_NewOscMessage.u16_DelayTimeMs = orc_ImportedOscMessageData.u16_DelayTimeMs;
         }
      }
   }
   if (orc_NewConfig.c_EdsFileContent.IsEventTimerRo(orc_NewOscMessage.u16_CanOpenManagerPdoIndex,
                                                     oq_MessageIsTxInEds, q_IsRo) == C_NO_ERR)
   {
      if (q_IsRo)
      {
         if (oq_MessageIsTxInEds)
         {
            if (orc_NewOscMessage.u32_CycleTimeMs != orc_ImportedOscMessageData.u32_CycleTimeMs)
            {
               orc_WarningMessages +=
                  static_cast<QString>(C_GtGetText::h_GetText(
                                          "Event time read-only so the current value %1 was replaced by %2\n"))
                  .
                  arg(orc_NewOscMessage.u32_CycleTimeMs).arg(
                     orc_ImportedOscMessageData.
                     u32_CycleTimeMs);
               orc_NewOscMessage.u32_CycleTimeMs = orc_ImportedOscMessageData.u32_CycleTimeMs;
            }
         }
         else
         {
            if (orc_NewOscMessage.u32_TimeoutMs != orc_ImportedOscMessageData.u32_TimeoutMs)
            {
               orc_WarningMessages +=
                  static_cast<QString>(C_GtGetText::h_GetText(
                                          "Event time read-only so the current timeout value %1 was replaced by %2\n"))
                  .
                  arg(orc_NewOscMessage.u32_TimeoutMs).arg(
                     orc_ImportedOscMessageData.
                     u32_TimeoutMs);
               orc_NewOscMessage.u32_TimeoutMs = orc_ImportedOscMessageData.u32_TimeoutMs;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt signal properties

   \param[in,out]  orc_Signal                Signal
   \param[in,out]  orc_OscSignalData         Osc signal data
   \param[in,out]  orc_UiSignalData          Ui signal data
   \param[in]      orc_OrgSignal             Org signal
   \param[in,out]  orc_AdaptedMessageInfo    Adapted message info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_AdaptSignalProperties(C_OscCanSignal & orc_Signal,
                                                             C_OscNodeDataPoolListElement & orc_OscSignalData,
                                                             C_PuiSdNodeDataPoolListElement & orc_UiSignalData,
                                                             const C_OscCanOpenManagerMappableSignal & orc_OrgSignal,
                                                             QString & orc_AdaptedMessageInfo)
{
   if (orc_Signal.u16_ComBitLength != orc_OrgSignal.c_SignalData.u16_ComBitLength)
   {
      orc_AdaptedMessageInfo +=
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Signal %1 length changed from current value %2 to %3\n"))
         .
         arg(C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(orc_OscSignalData.c_Name)).arg(
            orc_Signal.u16_ComBitLength).arg(
            orc_OrgSignal.c_SignalData.u16_ComBitLength);
      orc_Signal.u16_ComBitLength = orc_OrgSignal.c_SignalData.u16_ComBitLength;
   }
   if (orc_Signal.e_ComByteOrder != orc_OrgSignal.c_SignalData.e_ComByteOrder)
   {
      orc_AdaptedMessageInfo +=
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Signal %1 byte order changed from current value %2 to %3\n"))
         .
         arg(C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(orc_OscSignalData.c_Name)).arg(C_SdUtil::h_ConvertByteOrderToName(
                                                                                                          orc_Signal.
                                                                                                          e_ComByteOrder))
         .arg(
            C_SdUtil::h_ConvertByteOrderToName(orc_OrgSignal.c_SignalData.e_ComByteOrder));
      orc_Signal.e_ComByteOrder = orc_OrgSignal.c_SignalData.e_ComByteOrder;
   }
   if (orc_OscSignalData.GetType() != orc_OrgSignal.c_DatapoolData.GetType())
   {
      orc_AdaptedMessageInfo +=
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Signal %1 data type changed from current value %2 to %3\n"))
         .
         arg(C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(orc_OscSignalData.c_Name)).arg(C_SdNdeDpUtil::h_ConvertContentTypeToString(
                                                                                                          orc_OscSignalData
                                                                                                          .GetType())).
         arg(
            C_SdNdeDpUtil::h_ConvertContentTypeToString(orc_OrgSignal.c_DatapoolData.GetType()));
      //Nothing to do, only affects reinit step
   }
   //Reinit data
   if ((orc_Signal.u16_ComBitLength != orc_OrgSignal.c_SignalData.u16_ComBitLength) ||
       (orc_OscSignalData.GetType() != orc_OrgSignal.c_DatapoolData.GetType()))
   {
      orc_OscSignalData.c_DataSetValues = orc_OrgSignal.c_DatapoolData.c_DataSetValues;
      orc_OscSignalData.c_MaxValue = orc_OrgSignal.c_DatapoolData.c_MaxValue;
      orc_OscSignalData.c_MinValue = orc_OrgSignal.c_DatapoolData.c_MinValue;
      orc_OscSignalData.c_NvmValue = orc_OrgSignal.c_DatapoolData.c_NvmValue;
      orc_OscSignalData.c_Value = orc_OrgSignal.c_DatapoolData.c_Value;
      orc_UiSignalData.q_AutoMinMaxActive = orc_OrgSignal.q_AutoMinMaxUsed;
      //Also replace scaling
      orc_OscSignalData.f64_Factor = orc_OrgSignal.c_DatapoolData.f64_Factor;
      orc_OscSignalData.f64_Offset = orc_OrgSignal.c_DatapoolData.f64_Offset;
      orc_OscSignalData.c_Unit = orc_OrgSignal.c_DatapoolData.c_Unit;
      //Add info
      orc_AdaptedMessageInfo +=
         static_cast<QString>(C_GtGetText::h_GetText(
                                 "Signal %1 data type changes occured therefore the min, max and init values were replaced by the imported values\n"))
         .arg(C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(orc_OscSignalData.c_Name));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get data pool index

   \return
   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeCoDeviceUpdateEdsDialog::m_GetDataPoolIndex(void) const
{
   int32_t s32_DataPoolIndex = -1;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_ManagerNodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32_t u32_ItProt = 0UL; u32_ItProt < pc_Node->c_ComProtocols.size(); ++u32_ItProt)
      {
         const C_OscCanProtocol & rc_Protocol = pc_Node->c_ComProtocols[u32_ItProt];
         if (rc_Protocol.e_Type == C_OscCanProtocol::eCAN_OPEN)
         {
            s32_DataPoolIndex = static_cast<int32_t>(rc_Protocol.u32_DataPoolIndex);
         }
      }
   }
   return s32_DataPoolIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Bool to string

   \param[in]  oq_Value    Value

   \return
   Bool as string
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeCoDeviceUpdateEdsDialog::mh_BoolToString(const bool oq_Value)
{
   QString c_Retval;

   if (oq_Value)
   {
      c_Retval = C_GtGetText::h_GetText("True");
   }
   else
   {
      c_Retval = C_GtGetText::h_GetText("False");
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get signal names

   \param[in]  orc_Message       Message
   \param[in]  orc_SignalData    Signal data

   \return
   Signal names
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNames(const C_OscCanMessage & orc_Message,
                                                          const std::vector<C_OscNodeDataPoolListElement> & orc_SignalData)
{
   QString c_Retval;

   for (const auto & rc_Signal : orc_Message.c_Signals)
   {
      tgl_assert(rc_Signal.u32_ComDataElementIndex < orc_SignalData.size());
      if (rc_Signal.u32_ComDataElementIndex < orc_SignalData.size())
      {
         const C_OscNodeDataPoolListElement & rc_SignalData = orc_SignalData[rc_Signal.u32_ComDataElementIndex];
         if (c_Retval.isEmpty() == false)
         {
            c_Retval += ", ";
         }
         c_Retval += C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(rc_SignalData.c_Name);
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get signal name for display

   \param[in]  orc_Name    Name

   \return
   Signal name for display
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeCoDeviceUpdateEdsDialog::mh_GetSignalNameForDisplay(const stw::scl::C_SclString & orc_Name)
{
   return static_cast<QString>("\"") + orc_Name.c_str() + "\"";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get org signal

   \param[in]  ou16_ObjectIndex  Object index
   \param[in]  ou8_SubIndex      Sub index

   \return
   Org signal
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscCanOpenManagerMappableSignal * C_SdNdeCoDeviceUpdateEdsDialog::m_GetOrgSignal(
   const uint16_t ou16_ObjectIndex, const uint8_t ou8_SubIndex) const
{
   const C_OscCanOpenManagerMappableSignal * pc_Retval = NULL;

   for (uint32_t u32_ItSignal = 0UL; u32_ItSignal < this->mc_NewConfig.c_EdsFileMappableSignals.size(); ++u32_ItSignal)
   {
      const C_OscCanOpenManagerMappableSignal & rc_Signal = this->mc_NewConfig.c_EdsFileMappableSignals[u32_ItSignal];
      if ((rc_Signal.c_SignalData.u16_CanOpenManagerObjectDictionaryIndex == ou16_ObjectIndex) &&
          (rc_Signal.c_SignalData.u8_CanOpenManagerObjectDictionarySubIndex == ou8_SubIndex))
      {
         pc_Retval = &this->mc_NewConfig.c_EdsFileMappableSignals[u32_ItSignal];
         break;
      }
   }

   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init message names

   \param[in,out]  orc_Messages  Messages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::m_InitMessageNames(C_CieImportDataAssignment & orc_Messages) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(
      this->mc_DeviceNodeId.u32_NodeIndex);

   if (pc_Node != NULL)
   {
      uint32_t u32_SrdoIndex = 0UL;
      C_SdNdeCoDeviceUpdateEdsDialog::mh_InitMessageVectorNames(orc_Messages.c_ImportData.c_Core.c_OscRxMessageData,
                                                                orc_Messages.c_ImportData.c_Core.c_EdsOscRxMessageIsSrdo,
                                                                *pc_Node, true, u32_SrdoIndex);
      C_SdNdeCoDeviceUpdateEdsDialog::mh_InitMessageVectorNames(orc_Messages.c_ImportData.c_Core.c_OscTxMessageData,
                                                                orc_Messages.c_ImportData.c_Core.c_EdsOscTxMessageIsSrdo,
                                                                *pc_Node, false, u32_SrdoIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Init message vector names

   \param[in,out]  orc_Messages           Messages
   \param[in,out]  orc_MessageIsSrdo      Flag if message is SRDO
   \param[in]      orc_Node               Node
   \param[in]      oq_MessageIsTxInEds    Message is tx in eds
   \param[in,out]  oru32_SrdoIndex        SRDO index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoDeviceUpdateEdsDialog::mh_InitMessageVectorNames(std::vector<C_OscCanMessage> & orc_Messages,
                                                               std::vector<uint8_t> & orc_MessageIsSrdo,
                                                               const C_OscNode & orc_Node,
                                                               const bool oq_MessageIsTxInEds,
                                                               uint32_t & oru32_SrdoIndex)
{
   for (uint32_t u32_ItMessage = 0UL; u32_ItMessage < orc_Messages.size(); ++u32_ItMessage)
   {
      C_OscCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      const bool q_IsSrdo =
         ((u32_ItMessage < orc_MessageIsSrdo.size()) && (orc_MessageIsSrdo[u32_ItMessage] == 1U)) ? true : false;
      rc_Message.c_Name = C_CieUtil::h_GetMessageName(orc_Node, !oq_MessageIsTxInEds, u32_ItMessage,
                                                      C_OscCanProtocol::eCAN_OPEN, NULL, q_IsSrdo,
                                                      oru32_SrdoIndex).toStdString();
   }
}
